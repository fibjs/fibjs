/*
 * WebSocket.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: lion
 */

#include "object.h"
#include "WebSocket.h"
#include "WebSocketMessage.h"
#include "EventInfo.h"
#include "ifs/io.h"
#include "ifs/http.h"
#include "ifs/zlib.h"
#include <mbedtls/mbedtls/sha1.h>
#include "encoding.h"
#include "MemoryStream.h"
#include <stdlib.h>

namespace fibjs {

DECLARE_MODULE(ws);

result_t http_request(exlib::string method, exlib::string url,
    SeekableStream_base* body, NObject* headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);

class asyncSend : public AsyncState {
public:
    asyncSend(WebSocket* pThis, exlib::string data, int32_t type = ws_base::_TEXT)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_type(type)
    {
        obj_ptr<Buffer_base> _data = new Buffer(data);
        m_msg = new WebSocketMessage(type, m_this->m_masked, m_this->m_compress, 0);
        m_msg->cc_write(_data);

        start();
    }

    asyncSend(WebSocket* pThis, Buffer_base* data, int32_t type = ws_base::_BINARY)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_type(type)
    {
        m_msg = new WebSocketMessage(type, m_this->m_masked, m_this->m_compress, 0);
        m_msg->cc_write(data);

        start();
    }

    asyncSend(WebSocket* pThis, int32_t code, exlib::string reason)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_type(ws_base::_CLOSE)
    {
        exlib::string buf = "  " + reason;

        buf[0] = (code >> 8) & 255;
        buf[1] = code & 255;

        obj_ptr<Buffer_base> data = new Buffer(buf);

        m_msg = new WebSocketMessage(ws_base::_CLOSE, m_this->m_masked, m_this->m_compress, 0);
        m_msg->cc_write(data);

        start();
    }

    asyncSend(WebSocket* pThis, SeekableStream_base* body, int32_t type)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_type(type)
    {
        m_msg = new WebSocketMessage(type, m_this->m_masked, m_this->m_compress, 0);
        if (body)
            m_msg->set_body(body);

        start();
    }

    ~asyncSend()
    {
        unlock(m_this->m_lockSend);
    }

    void start()
    {
        set(lock_buffer_for_encode);
        if (m_this->m_lockEncode.lock(this))
            apost(0);
    }

    static int32_t lock_buffer_for_encode(AsyncState* pState, int32_t n)
    {
        asyncSend* pThis = (asyncSend*)pState;
        pThis->set(encode);
        return pThis->lock(pThis->m_this->m_lockBuffer);
    }

    static int32_t encode(AsyncState* pState, int32_t n)
    {
        asyncSend* pThis = (asyncSend*)pState;

        if (!pThis->m_this->m_buffer)
            pThis->m_this->m_buffer = new MemoryStream();

        pThis->set(encode_ok);
        return pThis->m_msg->sendTo(pThis->m_this->m_buffer, pThis->m_this, pThis);
    }

    static int32_t encode_ok(AsyncState* pState, int32_t n)
    {
        asyncSend* pThis = (asyncSend*)pState;
        pThis->unlock(pThis->m_this->m_lockBuffer);
        pThis->unlock(pThis->m_this->m_lockEncode);

        pThis->set(lock_buffer_for_send);
        return pThis->lock(pThis->m_this->m_lockSend);
    }

    static int32_t lock_buffer_for_send(AsyncState* pState, int32_t n)
    {
        asyncSend* pThis = (asyncSend*)pState;
        pThis->set(send);
        return pThis->lock(pThis->m_this->m_lockBuffer);
    }

    static int32_t send(AsyncState* pState, int32_t n)
    {
        asyncSend* pThis = (asyncSend*)pState;

        pThis->m_buffer = pThis->m_this->m_buffer;
        pThis->m_this->m_buffer.Release();
        pThis->unlock(pThis->m_this->m_lockBuffer);

        pThis->set(ok);

        if (!pThis->m_buffer)
            return 0;

        pThis->m_buffer->rewind();
        return pThis->m_buffer->copyTo(pThis->m_this->m_stream, -1, pThis->m_size, pThis);
    }

    static int32_t ok(AsyncState* pState, int32_t n)
    {
        asyncSend* pThis = (asyncSend*)pState;

        if (pThis->m_type == ws_base::_CLOSE) {
            obj_ptr<SeekableStream_base> body;
            pThis->m_msg->get_body(body);

            pThis->m_this->endConnect(body);
        }
        return pThis->done();
    }

    virtual int32_t error(int32_t v)
    {
        m_this->endConnect(1001, "");
        return v;
    }

private:
    obj_ptr<WebSocketMessage> m_msg;
    obj_ptr<WebSocket> m_this;
    obj_ptr<SeekableStream_base> m_buffer;
    int32_t m_type;
    int64_t m_size;
};

result_t WebSocket_base::_new(exlib::string url, exlib::string protocol, exlib::string origin,
    obj_ptr<WebSocket_base>& retVal,
    v8::Local<v8::Object> This)
{
    class asyncConnect : public AsyncState {
    public:
        asyncConnect(WebSocket* pThis, Isolate* isolate)
            : AsyncState(NULL)
            , m_this(pThis)
        {
            m_isolate = isolate;
            m_this->isolate_ref();
            set(handshake);
        }

        virtual Isolate* isolate()
        {
            assert(m_isolate);
            return m_isolate;
        }

        static int32_t handshake(AsyncState* pState, int32_t n)
        {
            asyncConnect* pThis = (asyncConnect*)pState;
            exlib::string url;

            if (!qstrcmp(pThis->m_this->m_url.c_str(), "wss://", 6))
                url = "https://" + pThis->m_this->m_url.substr(6);
            else if (!qstrcmp(pThis->m_this->m_url.c_str(), "ws://", 5))
                url = "http://" + pThis->m_this->m_url.substr(5);
            else {
                pThis->m_this->endConnect(1002, "unknown protocol");
                return CHECK_ERROR(Runtime::setError("websocket: unknown protocol"));
            }

            pThis->m_headers = new NObject();

            pThis->m_headers->add("Upgrade", "websocket");
            pThis->m_headers->add("Connection", "Upgrade");
            pThis->m_headers->add("Sec-WebSocket-Version", "13");
            pThis->m_headers->add("Sec-WebSocket-Extensions", "permessage-deflate");

            if (!pThis->m_this->m_origin.empty())
                pThis->m_headers->add("Origin", pThis->m_this->m_origin);

            char keys[16];
            int32_t i;

            for (i = 0; i < (int32_t)sizeof(keys); i++)
                keys[i] = (char)rand();

            exlib::string key;
            baseEncode(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
                6, (const char*)&keys, sizeof(keys), key);

            pThis->m_headers->add("Sec-WebSocket-Key", key);

            key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

            unsigned char output[20];
            mbedtls_sha1((const unsigned char*)key.c_str(), key.length(), output);

            baseEncode(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
                6, (const char*)output, 20, pThis->m_accept);

            pThis->set(response);
            return http_request("GET", url, NULL, pThis->m_headers,
                pThis->m_httprep, pThis);
        }

        static int32_t response(AsyncState* pState, int32_t n)
        {
            asyncConnect* pThis = (asyncConnect*)pState;

            Variant v;
            result_t hr;
            int32_t status;

            pThis->m_httprep->get_statusCode(status);
            if (status != 101) {
                pThis->m_this->endConnect(1002, "server error.");
                return CHECK_ERROR(Runtime::setError("websocket: server error."));
            }

            hr = pThis->m_httprep->firstHeader("Upgrade", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL) {
                pThis->m_this->endConnect(1002, "missing Upgrade header.");
                return CHECK_ERROR(Runtime::setError("websocket: missing Upgrade header."));
            }

            if (qstricmp(v.string().c_str(), "websocket")) {
                pThis->m_this->endConnect(1002, "invalid Upgrade header.");
                return CHECK_ERROR(Runtime::setError("websocket: invalid Upgrade header."));
            }

            bool bUpgrade;
            pThis->m_httprep->get_upgrade(bUpgrade);
            if (!bUpgrade) {
                pThis->m_this->endConnect(1002, "invalid connection header.");
                return CHECK_ERROR(Runtime::setError("websocket: invalid connection header."));
            }

            hr = pThis->m_httprep->firstHeader("Sec-WebSocket-Accept", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL) {
                pThis->m_this->endConnect(1002, "missing Sec-WebSocket-Accept header.");
                return CHECK_ERROR(Runtime::setError("websocket: missing Sec-WebSocket-Accept header."));
            }

            if (v.string() != pThis->m_accept) {
                pThis->m_this->endConnect(1002, "invalid Sec-WebSocket-Accept header.");
                return CHECK_ERROR(Runtime::setError("websocket: invalid Sec-WebSocket-Accept header."));
            }

            hr = pThis->m_httprep->firstHeader("Sec-WebSocket-Extensions", v);
            if (hr < 0)
                return hr;

            if (hr != CALL_RETURN_NULL && !qstricmp(v.string().c_str(), "permessage-deflate", 18))
                pThis->m_this->enableCompress();

            pThis->m_httprep->get_stream(pThis->m_this->m_stream);

            pThis->m_this->m_readyState = ws_base::_OPEN;
            pThis->m_this->_emit("open", NULL, 0);

            pThis->m_this->startRecv(pThis->m_isolate);

            return pThis->done(0);
        }

        virtual int32_t error(int32_t v)
        {
            m_this->isolate_unref();
            m_this->endConnect(1002, "");
            return v;
        }

    private:
        Isolate* m_isolate;
        obj_ptr<WebSocket> m_this;
        obj_ptr<HttpResponse_base> m_httprep;
        obj_ptr<NObject> m_headers;
        exlib::string m_accept;
    };

    obj_ptr<WebSocket> sock = new WebSocket(url, protocol, origin);
    sock->wrap(This);

    (new asyncConnect(sock, sock->holder()))->apost(0);

    retVal = sock;

    return 0;
}

void WebSocket::startRecv(Isolate* isolate)
{
    class asyncRead : public AsyncState {
    public:
        asyncRead(WebSocket* pThis, Isolate* isolate)
            : AsyncState(NULL)
            , m_this(pThis)
        {
            m_isolate = isolate;
            set(recv);
        }

        ~asyncRead()
        {
            m_this->isolate_unref();
        }

        static int32_t recv(AsyncState* pState, int32_t n)
        {
            asyncRead* pThis = (asyncRead*)pState;

            pThis->m_msg = new WebSocketMessage(ws_base::_TEXT, false, false, pThis->m_this->m_maxSize);

            pThis->set(event);
            return pThis->m_msg->readFrom(pThis->m_this->m_stream, pThis->m_this, pThis);
        }

        static int32_t event(AsyncState* pState, int32_t n)
        {
            asyncRead* pThis = (asyncRead*)pState;

            bool masked;
            pThis->m_msg->get_masked(masked);
            if (masked == pThis->m_this->m_masked) {
                pThis->m_msg->m_error = 1007;
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: Payload data is not masked."));
            }

            int32_t type;
            pThis->m_msg->get_type(type);

            switch (type) {
            case ws_base::_PING: {
                obj_ptr<SeekableStream_base> body;
                pThis->m_msg->get_body(body);
                new asyncSend(pThis->m_this, body, ws_base::_PONG);
                break;
            }
            case ws_base::_CLOSE: {
                obj_ptr<SeekableStream_base> body;
                pThis->m_msg->get_body(body);

                if (pThis->m_this->m_closeState.CompareAndSwap(ws_base::_OPEN, ws_base::_CLOSING) == ws_base::_OPEN)
                    new asyncSend(pThis->m_this, body, ws_base::_CLOSE);
                else
                    pThis->m_this->endConnect(body);

                return pThis->done(0);
            }
            case ws_base::_TEXT:
            case ws_base::_BINARY: {
                Variant v = pThis->m_msg;
                pThis->m_this->_emit("message", &v, 1);
            }
            }

            pThis->set(recv);
            return 0;
        }

        virtual int32_t error(int32_t v)
        {
            if (m_msg->m_error)
                m_this->endConnect(m_msg->m_error, "");
            else
                m_this->endConnect(1001, "Going Away");

            return v;
        }

    private:
        Isolate* m_isolate;
        obj_ptr<WebSocket> m_this;
        obj_ptr<WebSocketMessage> m_msg;
    };

    if (m_stream && m_readState.xchg(ws_base::_OPEN) != ws_base::_OPEN)
        (new asyncRead(this, isolate))->apost(0);
}

void WebSocket::endConnect(int32_t code, exlib::string reason)
{
    if (m_closeState.xchg(ws_base::_CLOSED) != ws_base::_CLOSED) {
        Isolate* isolate = holder();
        if (isolate) {
            if (code > 1000 && code < 3000) {
                m_code = 1006;
                m_reason = "Abnormal Closure";

                Variant v = new EventInfo(this, "error", code, reason);
                _emit("error", &v, 1);
            } else {
                m_code = code;
                m_reason = reason;
            }

            if (m_stream)
                m_stream->cc_close();

            if (m_ac)
                m_ac->post(CALL_RETURN_NULL);

            Variant v = new EventInfo(this, "close", m_code, m_reason);
            _emit("close", &v, 1);
        }
    }
}

void WebSocket::endConnect(SeekableStream_base* body)
{
    obj_ptr<Buffer_base> buf;
    int32_t code = 0;
    exlib::string reason;

    body->rewind();
    body->cc_readAll(buf);
    if (buf) {
        exlib::string str;

        buf->toString(str);
        if (str.length() >= 2) {
            const unsigned char* data = (const unsigned char*)str.c_str();
            code = ((int32_t)data[0] << 8) + data[1];
            reason = str.substr(2);

            if (code != 1000 && (code < 3000 || code > 4999)) {
                code = 1002;
                reason = "Protocol error";
            }
        }
    }

    endConnect(code, reason);
}

void WebSocket::enableCompress()
{
    m_compress = true;
    m_deflate = new defraw(NULL);
    m_inflate = new infraw(NULL);
    m_flushTail = new Buffer("\x0\x0\xff\xff", 4);
}

result_t WebSocket::get_url(exlib::string& retVal)
{
    retVal = m_url;
    return 0;
}

result_t WebSocket::get_protocol(exlib::string& retVal)
{
    retVal = m_protocol;
    return 0;
}

result_t WebSocket::get_origin(exlib::string& retVal)
{
    retVal = m_origin;
    return 0;
}

result_t WebSocket::get_readyState(int32_t& retVal)
{
    retVal = (int32_t)m_readyState;
    return 0;
}

result_t WebSocket::close(int32_t code, exlib::string reason)
{
    if (code != 1000 && (code < 3000 || code > 4999))
        return CHECK_ERROR(Runtime::setError("websocket: The code must be either 1000, or between 3000 and 4999."));

    if (m_readyState.CompareAndSwap(ws_base::_OPEN, ws_base::_CLOSING) != ws_base::_OPEN)
        return 0;

    new asyncSend(this, code, reason);
    return 0;
}

result_t WebSocket::send(exlib::string data)
{
    switch (m_readyState) {
    case ws_base::_CONNECTING:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CONNECTING state."));
    case ws_base::_CLOSING:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CLOSING state."));
    case ws_base::_CLOSED:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CLOSED state."));
    }

    new asyncSend(this, data);
    return 0;
}

result_t WebSocket::send(Buffer_base* data)
{
    switch (m_readyState) {
    case ws_base::_CONNECTING:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CONNECTING state."));
    case ws_base::_CLOSING:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CLOSING state."));
    case ws_base::_CLOSED:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CLOSED state."));
    }

    new asyncSend(this, data);
    return 0;
}

result_t WebSocket::ref(obj_ptr<WebSocket_base>& retVal)
{
    isolate_ref();
    retVal = this;
    return 0;
}

result_t WebSocket::unref(obj_ptr<WebSocket_base>& retVal)
{
    isolate_unref();
    retVal = this;
    return 0;
}
}
