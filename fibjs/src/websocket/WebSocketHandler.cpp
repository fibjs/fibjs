/*
 * WebSocketHandler.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/mq.h"
#include "WebSocketHandler.h"
#include "WebSocketMessage.h"
#include "WebSocket.h"
#include "ifs/HttpRequest.h"
#include "ifs/HttpResponse.h"
#include "JSHandler.h"
#include "ifs/console.h"
#include <mbedtls/mbedtls/sha1.h>
#include "encoding.h"

namespace fibjs {

result_t ws_base::upgrade(v8::Local<v8::Object> opts, v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    bool perMessageDeflate = false;
    int32_t maxPayload = WS_DEF_SIZE;

    GetConfigValue(isolate->m_isolate, opts, "perMessageDeflate", perMessageDeflate);
    GetConfigValue(isolate->m_isolate, opts, "maxPayload", maxPayload);

    retVal = new WebSocketHandler(accept, perMessageDeflate, maxPayload);
    return 0;
}

result_t ws_base::upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

    return upgrade(opts, accept, retVal);
}

WebSocketHandler::WebSocketHandler(v8::Local<v8::Function> accept, bool enableCompress, int32_t maxSize)
    : m_enableCompress(enableCompress)
    , m_maxSize(maxSize)
{
    v8::Local<v8::Object> r;
    on("accept", accept, r);
}

result_t WebSocketHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(WebSocketHandler* pThis, HttpRequest_base* req, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_httpreq(req)
            , m_compress(false)
        {
            m_httpreq->get_response(m_httprep);
            m_httpreq->get_stream(m_stm);

            next(handshake);
        }

        ON_STATE(asyncInvoke, handshake)
        {
            exlib::string v;
            result_t hr;

            hr = m_httpreq->firstHeader("Upgrade", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Upgrade header."));

            if (qstricmp(v.c_str(), "websocket"))
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid Upgrade header."));

            hr = m_httpreq->firstHeader("Sec-WebSocket-Version", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Sec-WebSocket-Version header."));

            if (qstricmp(v.c_str(), "13"))
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid Sec-WebSocket-Version header."));

            bool bUpgrade;
            m_httpreq->get_upgrade(bUpgrade);
            if (!bUpgrade)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid connection header."));

            hr = m_httpreq->firstHeader("Sec-WebSocket-Key", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Sec-WebSocket-Key header."));

            exlib::string key(v);

            key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

            unsigned char output[20];
            mbedtls_sha1((const unsigned char*)key.c_str(), key.length(), output);

            exlib::string out;

            base64Encode((const char*)output, 20, false, out);

            m_httprep->set_statusCode(101);
            m_httprep->addHeader("Sec-WebSocket-Accept", out);
            m_httprep->addHeader("Upgrade", "websocket");
            m_httprep->set_upgrade(true);

            hr = m_httpreq->firstHeader("Sec-WebSocket-Extensions", v);
            if (hr < 0)
                return hr;

            if (hr != CALL_RETURN_NULL && m_pThis->m_enableCompress && !qstricmp(v.c_str(), "permessage-deflate", 18)) {
                m_httprep->addHeader("Sec-WebSocket-Extensions", "permessage-deflate");
                m_compress = true;
            }

            return m_httprep->sendTo(m_stm, next(accept));
        }

        ON_STATE(asyncInvoke, accept)
        {
            obj_ptr<WebSocketHandler> pHandler = m_pThis;
            obj_ptr<WebSocket> sock = new WebSocket(m_stm, "", this, m_pThis->m_enableCompress, m_pThis->m_maxSize);
            if (m_compress)
                sock->enableCompress();

            Variant vs[2];
            vs[0] = sock;
            vs[1] = m_httpreq;
            pHandler->_emit("accept", vs, 2);

            next(CALL_RETURN_NULL);
            return CALL_E_PENDDING;
        }

    private:
        obj_ptr<WebSocketHandler> m_pThis;
        obj_ptr<HttpRequest_base> m_httpreq;
        obj_ptr<HttpResponse_base> m_httprep;
        obj_ptr<Stream_base> m_stm;
        bool m_compress;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);
    if (req == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, req, ac))->post(0);
}
}
