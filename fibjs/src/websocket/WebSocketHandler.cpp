/*
 * WebSocketHandler.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "ifs/mq.h"
#include "WebSocketHandler.h"
#include "WebSocketMessage.h"
#include "ifs/HttpRequest.h"
#include "ifs/HttpResponse.h"
#include "BufferedStream.h"
#include "JSHandler.h"
#include "ifs/console.h"
#include <mbedtls/mbedtls/sha1.h>
#include "encoding.h"

namespace fibjs
{

static const char *s_staticCounter[] =
{ "total", "pendding" };
static const char *s_Counter[] =
{ "request", "response", "error" };

enum
{
    PACKET_TOTAL = 0,
    PACKET_PENDDING,
    PACKET_REQUEST,
    PACKET_RESPONSE,
    PACKET_ERROR
};

result_t WebSocketHandler_base::_new(v8::Local<v8::Value> hdlr,
                                     obj_ptr<WebSocketHandler_base> &retVal,
                                     v8::Local<v8::Object> This)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;

    obj_ptr<WebSocketHandler> pk_hdlr = new WebSocketHandler();
    pk_hdlr->wrap(This);
    pk_hdlr->set_handler(hdlr1);

    retVal = pk_hdlr;

    return 0;
}

WebSocketHandler::WebSocketHandler() : m_maxSize(67108864)
{
    m_stats = new Stats();
    m_stats->init(s_staticCounter, 2, s_Counter, 3);
}

result_t WebSocketHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                                  AsyncEvent *ac)
{
    class asyncInvoke: public AsyncState
    {
    public:
        asyncInvoke(WebSocketHandler *pThis, HttpRequest_base *req, AsyncEvent *ac) :
            AsyncState(ac), m_pThis(pThis), m_httpreq(req)
        {
            obj_ptr<Message_base> rep;

            m_httpreq->get_response(rep);
            m_httprep = (HttpResponse_base*)(Message_base*)rep;

            m_httpreq->get_stream(m_stm);
            m_stmBuffered = new BufferedStream(m_stm);

            set(handshake);
        }

        static int32_t handshake(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

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

            std::string key(v.string());

            key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

            unsigned char output[20];
            mbedtls_sha1((const unsigned char*)key.data(), key.size(), output);

            std::string out;

            baseEncode(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
                6, (const char*)output, 20, out);

            pThis->m_httprep->addHeader("Sec-WebSocket-Accept", out.c_str());
            pThis->m_httprep->addHeader("Upgrade", "websocket");
            pThis->m_httprep->set_upgrade(true);

            pThis->set(read);
            return pThis->m_httprep->sendTo(pThis->m_stm, pThis);
        }

        static int32_t read(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->m_msg = new WebSocketMessage(websocket_base::_TEXT, false, pThis->m_pThis->m_maxSize);

            pThis->set(invoke);
            return pThis->m_msg->readFrom(pThis->m_stmBuffered, pThis);
        }

        static int32_t invoke(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            if (n == CALL_RETURN_NULL)
                return pThis->done(CALL_RETURN_NULL);

            bool masked;
            pThis->m_msg->get_masked(masked);
            if (!masked)
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: Payload data is not masked."));

            int32_t type;
            pThis->m_msg->get_type(type);

            if (type == websocket_base::_CLOSE)
                return pThis->done(CALL_RETURN_NULL);

            if (type != websocket_base::_TEXT &&
                    type != websocket_base::_BINARY &&
                    type != websocket_base::_PING)
            {
                pThis->set(read);
                return 0;
            }

            pThis->m_pThis->m_stats->inc(PACKET_TOTAL);
            pThis->m_pThis->m_stats->inc(PACKET_REQUEST);
            pThis->m_pThis->m_stats->inc(PACKET_PENDDING);

            pThis->set(send);
            if (type == websocket_base::_PING)
                return 0;

            return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_msg, pThis);
        }

        static int32_t send(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->m_msg->get_response(pThis->m_rep);

            pThis->set(end);
            return pThis->m_rep->sendTo(pThis->m_stm, pThis);
        }

        static int32_t end(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->m_rep.Release();

            pThis->m_pThis->m_stats->inc(PACKET_RESPONSE);
            pThis->m_pThis->m_stats->dec(PACKET_PENDDING);

            pThis->set(read);
            return 0;
        }

        virtual int32_t error(int32_t v)
        {
            m_pThis->m_stats->inc(PACKET_ERROR);

            if (is(send))
            {
                m_pThis->m_stats->dec(PACKET_PENDDING);
                asyncLog(console_base::_ERROR, "WebSocketHandler: " + getResultMessage(v));
            }

            if (is(end))
                m_pThis->m_stats->dec(PACKET_PENDDING);

            if (is(invoke))
            {
                m_pThis->m_stats->inc(PACKET_TOTAL);
                m_pThis->m_stats->inc(PACKET_REQUEST);
            }

            return v;
        }

    private:
        obj_ptr<WebSocketHandler> m_pThis;
        obj_ptr<HttpRequest_base> m_httpreq;
        obj_ptr<HttpResponse_base> m_httprep;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<BufferedStream_base> m_stmBuffered;
        obj_ptr<WebSocketMessage_base> m_msg;
        obj_ptr<Message_base> m_rep;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);
    if (req == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, req, ac))->post(0);
}

result_t WebSocketHandler::get_maxSize(int32_t &retVal)
{
    retVal = m_maxSize;
    return 0;
}

result_t WebSocketHandler::set_maxSize(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxSize = newVal;
    return 0;
}

result_t WebSocketHandler::get_handler(obj_ptr<Handler_base> &retVal)
{
    retVal = m_hdlr;
    return 0;
}

result_t WebSocketHandler::set_handler(Handler_base *newVal)
{
    wrap()->SetHiddenValue(v8::String::NewFromUtf8(Isolate::now()->m_isolate, "handler"), newVal->wrap());
    m_hdlr = newVal;
    return 0;
}

result_t WebSocketHandler::get_stats(obj_ptr<Stats_base> &retVal)
{
    retVal = m_stats;
    return 0;
}

}
