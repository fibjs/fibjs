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

result_t ws_base::upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal)
{
    retVal = new WebSocketHandler(accept);
    return 0;
}

WebSocketHandler::WebSocketHandler(v8::Local<v8::Function> accept)
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
            obj_ptr<Message_base> rep;

            m_httpreq->get_response(rep);
            m_httprep = (HttpResponse_base*)(Message_base*)rep;

            m_httpreq->get_stream(m_stm);

            set(handshake);
        }

        static int32_t handshake(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            Variant v;
            result_t hr;

            hr = pThis->m_httpreq->firstHeader("Upgrade", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Upgrade header."));

            if (qstricmp(v.string().c_str(), "websocket"))
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid Upgrade header."));

            hr = pThis->m_httpreq->firstHeader("Sec-WebSocket-Version", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Sec-WebSocket-Version header."));

            if (qstricmp(v.string().c_str(), "13"))
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid Sec-WebSocket-Version header."));

            bool bUpgrade;
            pThis->m_httpreq->get_upgrade(bUpgrade);
            if (!bUpgrade)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: invalid connection header."));

            hr = pThis->m_httpreq->firstHeader("Sec-WebSocket-Key", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: missing Sec-WebSocket-Key header."));

            exlib::string key(v.string());

            key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

            unsigned char output[20];
            mbedtls_sha1((const unsigned char*)key.c_str(), key.length(), output);

            exlib::string out;

            baseEncode(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
                6, (const char*)output, 20, out);

            pThis->m_httprep->set_statusCode(101);
            pThis->m_httprep->addHeader("Sec-WebSocket-Accept", out);
            pThis->m_httprep->addHeader("Upgrade", "websocket");
            pThis->m_httprep->set_upgrade(true);

            hr = pThis->m_httpreq->firstHeader("Sec-WebSocket-Extensions", v);
            if (hr < 0)
                return hr;

            if (hr != CALL_RETURN_NULL && !qstricmp(v.string().c_str(), "permessage-deflate", 18)) {
                pThis->m_httprep->addHeader("Sec-WebSocket-Extensions", "permessage-deflate");
                pThis->m_compress = true;
            }

            pThis->set(accept);
            return pThis->m_httprep->sendTo(pThis->m_stm, pThis);
        }

        static int32_t accept(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            pThis->done(CALL_RETURN_NULL);

            obj_ptr<WebSocketHandler> pHandler = pThis->m_pThis;
            obj_ptr<WebSocket> sock = new WebSocket(pThis->m_stm, "", pThis);
            if (pThis->m_compress)
                sock->enableCompress();

            Variant v = sock;
            pHandler->_emit("accept", &v, 1);

            // sock->startRecv();

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
