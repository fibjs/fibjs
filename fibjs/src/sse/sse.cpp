/*
 * sse.cpp
 *
 *  Created on: Jun 17, 2025
 */

#include "object.h"
#include "ifs/sse.h"
#include "ifs/HttpRequest.h"
#include "HttpResponse.h"
#include "EventSourceHandler.h"
#include "EventSource.h"

namespace fibjs {
DECLARE_MODULE(sse);

result_t sse_base::upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal)
{
    Isolate* isolate = Isolate::current(accept);
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

    retVal = new EventSourceHandler(accept);
    return 0;
}

EventSourceHandler::EventSourceHandler(v8::Local<v8::Function> accept)
{
    v8::Local<v8::Object> r;
    on("accept", accept, r);
}

result_t EventSourceHandler::isRouting(bool& retVal)
{
    retVal = false;
    return 0;
}

result_t EventSourceHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(EventSourceHandler* pThis, HttpRequest_base* req, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_httpreq(req)
        {
            m_httpreq->get_response(m_httprep);
            m_httpreq->get_stream(m_stm);

            next(handshake);
        }

        ON_STATE(asyncInvoke, handshake)
        {
            m_httprep->addHeader("Content-Type", "text/event-stream");
            m_httprep->addHeader("Transfer-Encoding", "chunked");

            HttpResponse* resp = m_httprep.As<HttpResponse>();
            return resp->sendHeader(m_stm, false, next(accept));
        }

        ON_STATE(asyncInvoke, accept)
        {
            obj_ptr<EventSourceHandler> pHandler = m_pThis;
            obj_ptr<EventSource> es = new EventSource();
            es->m_readyState = sse_base::C_SENDER;
            es->m_stream = m_stm;
            es->m_ac = this;

            Variant vs[2];
            vs[0] = es;
            vs[1] = m_httpreq;
            pHandler->_emit("accept", vs, 2);

            next(CALL_RETURN_NULL);
            return CALL_E_PENDDING;
        }

    private:
        obj_ptr<EventSourceHandler> m_pThis;
        obj_ptr<HttpRequest_base> m_httpreq;
        obj_ptr<HttpResponse_base> m_httprep;
        obj_ptr<Stream_base> m_stm;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);
    if (req == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, req, ac))->post(0);
}

}
