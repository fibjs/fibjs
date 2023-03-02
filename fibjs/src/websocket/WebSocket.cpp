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
#include "HttpClient.h"
#include <stdlib.h>

namespace fibjs {

DECLARE_MODULE(ws);

#define WS_DEFALTE_BUF_SIZE (32 * 1024)

result_t http_request2(HttpClient_base* httpClient, exlib::string method, exlib::string url,
    SeekableStream_base* body, NObject* headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);

class asyncSend : public AsyncState {
public:
    asyncSend(WebSocket* pThis, exlib::string data, int32_t type = ws_base::C_TEXT)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_type(type)
    {
        m_this->m_ioState.inc();

        obj_ptr<Buffer_base> _data = new Buffer(data);
        m_msg = new WebSocketMessage(type, m_this->m_masked, m_this->m_compress, 0);
        m_msg->cc_write(_data);

        next(start);
    }

    asyncSend(WebSocket* pThis, Buffer_base* data, int32_t type = ws_base::C_BINARY)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_type(type)
    {
        m_this->m_ioState.inc();

        m_msg = new WebSocketMessage(type, m_this->m_masked, m_this->m_compress, 0);
        m_msg->cc_write(data);

        next(start);
    }

    asyncSend(WebSocket* pThis, int32_t code, exlib::string reason)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_type(ws_base::C_CLOSE)
    {
        m_this->m_ioState.inc();

        exlib::string buf;

        buf.append(1, (code >> 8) & 255);
        buf.append(1, code & 255);
        buf.append(reason);

        obj_ptr<Buffer_base> data = new Buffer(buf);

        m_msg = new WebSocketMessage(ws_base::C_CLOSE, m_this->m_masked, m_this->m_compress, 0);
        m_msg->cc_write(data);

        next(start);
    }

    asyncSend(WebSocket* pThis, SeekableStream_base* body, int32_t type)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_type(type)
    {
        m_this->m_ioState.inc();

        m_msg = new WebSocketMessage(type, m_this->m_masked, m_this->m_compress, 0);
        if (body)
            m_msg->set_body(body);

        next(start);
    }

    ~asyncSend()
    {
        m_this->m_lockSend.unlock(this);
        m_this->free_mem();
    }

    ON_STATE(asyncSend, start)
    {
        return lock(m_this->m_lockEncode, next(lock_buffer_for_encode));
    }

    ON_STATE(asyncSend, lock_buffer_for_encode)
    {
        return lock(m_this->m_lockBuffer, next(encode));
    }

    ON_STATE(asyncSend, encode)
    {
        if (!m_this->m_buffer)
            m_this->m_buffer = new MemoryStream();

        if (m_this->m_compress && !m_this->m_deflate)
            m_this->m_deflate = new defraw(NULL);

        return m_msg->sendTo(m_this->m_buffer, m_this, next(encode_ok));
    }

    ON_STATE(asyncSend, encode_ok)
    {
        m_this->m_lockBuffer.unlock(this);
        m_this->m_lockEncode.unlock(this);

        return lock(m_this->m_lockSend, next(lock_buffer_for_send));
    }

    ON_STATE(asyncSend, lock_buffer_for_send)
    {
        return lock(m_this->m_lockBuffer, next(send));
    }

    ON_STATE(asyncSend, send)
    {
        m_buffer = m_this->m_buffer;
        m_this->m_buffer.Release();
        m_this->m_lockBuffer.unlock(this);

        if (!m_buffer)
            return next(ok);

        m_buffer->rewind();
        return m_buffer->copyTo(m_this->m_stream, -1, m_size, next(ok));
    }

    ON_STATE(asyncSend, ok)
    {
        if (m_type == ws_base::C_CLOSE) {
            obj_ptr<SeekableStream_base> body;

            m_msg->get_body(body);
            m_this->endConnect(body);
        }
        return next();
    }

    virtual int32_t error(int32_t v)
    {
        m_this->endConnect(1001, "");
        return v;
    }

    result_t lock(exlib::Locker& l, AsyncState* pThis)
    {
        return l.lock(pThis) ? 0 : CALL_E_PENDDING;
    }

private:
    obj_ptr<WebSocketMessage> m_msg;
    obj_ptr<WebSocket> m_this;
    obj_ptr<SeekableStream_base> m_buffer;
    int32_t m_type;
    int64_t m_size;
};

result_t WebSocket_base::_new(exlib::string url, exlib::string protocol,
    exlib::string origin, obj_ptr<WebSocket_base>& retVal,
    v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

    opts->Set(context, isolate->NewString("protocol", 8), isolate->NewString(protocol)).IsJust();
    opts->Set(context, isolate->NewString("origin", 6), isolate->NewString(origin)).IsJust();

    return WebSocket_base::_new(url, opts, retVal, This);
}

result_t WebSocket_base::_new(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<WebSocket_base>& retVal,
    v8::Local<v8::Object> This)
{
    class asyncConnect : public AsyncState {
    public:
        asyncConnect(WebSocket* pThis, obj_ptr<NObject> headers, HttpClient_base* hc, Isolate* isolate)
            : AsyncState(NULL)
            , m_this(pThis)
            , m_headers(headers)
            , m_hc(hc)
        {
            m_isolate = isolate;
            m_this->isolate_ref();
            next(handshake);
        }

        virtual Isolate* isolate()
        {
            assert(m_isolate);
            return m_isolate;
        }

        ON_STATE(asyncConnect, handshake)
        {
            exlib::string url;

            if (!qstrcmp(m_this->m_url.c_str(), "wss://", 6))
                url = "https://" + m_this->m_url.substr(6);
            else if (!qstrcmp(m_this->m_url.c_str(), "ws://", 5))
                url = "http://" + m_this->m_url.substr(5);
            else {
                m_this->endConnect(1002, "unknown protocol");
                return CHECK_ERROR(Runtime::setError("websocket: unknown protocol"));
            }

            m_headers->add("Upgrade", "websocket");
            m_headers->add("Connection", "Upgrade");
            m_headers->add("Sec-WebSocket-Version", "13");

            if (m_this->m_enableCompress)
                m_headers->add("Sec-WebSocket-Extensions", "permessage-deflate");

            if (!m_this->m_origin.empty())
                m_headers->add("Origin", m_this->m_origin);

            char keys[16];
            int32_t i;

            for (i = 0; i < (int32_t)sizeof(keys); i++)
                keys[i] = (char)rand();

            exlib::string key;
            base64Encode((const char*)&keys, sizeof(keys), false, key);

            m_headers->add("Sec-WebSocket-Key", key);

            key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

            unsigned char output[20];
            mbedtls_sha1((const unsigned char*)key.c_str(), key.length(), output);

            base64Encode((const char*)output, 20, false, m_accept);

            return http_request2(m_hc, "GET", url, NULL, m_headers,
                m_httprep, next(response));
        }

        ON_STATE(asyncConnect, response)
        {
            exlib::string v;
            result_t hr;
            int32_t status;

            m_httprep->get_statusCode(status);
            if (status != 101) {
                m_this->endConnect(1002, "server error.");
                return CHECK_ERROR(Runtime::setError("websocket: server error."));
            }

            hr = m_httprep->firstHeader("Upgrade", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL) {
                m_this->endConnect(1002, "missing Upgrade header.");
                return CHECK_ERROR(Runtime::setError("websocket: missing Upgrade header."));
            }

            if (qstricmp(v.c_str(), "websocket")) {
                m_this->endConnect(1002, "invalid Upgrade header.");
                return CHECK_ERROR(Runtime::setError("websocket: invalid Upgrade header."));
            }

            bool bUpgrade;
            m_httprep->get_upgrade(bUpgrade);
            if (!bUpgrade) {
                m_this->endConnect(1002, "invalid connection header.");
                return CHECK_ERROR(Runtime::setError("websocket: invalid connection header."));
            }

            hr = m_httprep->firstHeader("Sec-WebSocket-Accept", v);
            if (hr < 0)
                return hr;

            if (hr == CALL_RETURN_NULL) {
                m_this->endConnect(1002, "missing Sec-WebSocket-Accept header.");
                return CHECK_ERROR(Runtime::setError("websocket: missing Sec-WebSocket-Accept header."));
            }

            if (v != m_accept) {
                m_this->endConnect(1002, "invalid Sec-WebSocket-Accept header.");
                return CHECK_ERROR(Runtime::setError("websocket: invalid Sec-WebSocket-Accept header."));
            }

            hr = m_httprep->firstHeader("Sec-WebSocket-Extensions", v);
            if (hr < 0)
                return hr;

            if (hr != CALL_RETURN_NULL && m_this->m_enableCompress && !qstricmp(v.c_str(), "permessage-deflate", 18))
                m_this->enableCompress();

            m_httprep->get_stream(m_this->m_stream);

            m_this->m_readyState = ws_base::C_OPEN;
            m_this->_emit("open");

            m_this->startRecv(m_isolate);

            return next(0);
        }

        virtual int32_t error(int32_t v)
        {
            m_this->m_holder.Release();
            m_this->endConnect(1002, "");
            m_this->isolate_unref();
            return v;
        }

    private:
        Isolate* m_isolate;
        obj_ptr<WebSocket> m_this;
        obj_ptr<HttpResponse_base> m_httprep;
        obj_ptr<NObject> m_headers;
        obj_ptr<HttpClient_base> m_hc;
        exlib::string m_accept;
    };

    Isolate* isolate = Isolate::current();
    exlib::string origin = "";
    exlib::string protocol = "";
    bool perMessageDeflate = false;
    int32_t maxPayload = WS_DEF_SIZE;
    v8::Local<v8::Object> v;
    obj_ptr<NObject> headers = new NObject();
    obj_ptr<HttpClient_base> hc = NULL;

    GetConfigValue(isolate->m_isolate, opts, "protocol", protocol);
    GetConfigValue(isolate->m_isolate, opts, "origin", origin);
    GetConfigValue(isolate->m_isolate, opts, "perMessageDeflate", perMessageDeflate);
    GetConfigValue(isolate->m_isolate, opts, "maxPayload", maxPayload);

    if (GetConfigValue(isolate->m_isolate, opts, "headers", v) >= 0)
        headers->add(v);

    GetConfigValue(isolate->m_isolate, opts, "httpClient", hc);

    obj_ptr<WebSocket> sock = new WebSocket(url, protocol, origin, perMessageDeflate, maxPayload);
    sock->m_holder = new ValueHolder(sock->wrap(This));

    (new asyncConnect(sock, headers, hc, sock->holder()))->apost(0);

    retVal = sock;

    return 0;
}

WebSocket::~WebSocket()
{
    if (m_closeState.xchg(ws_base::C_CLOSED) != ws_base::C_CLOSED) {
        if (m_stream)
            m_stream->cc_close();

        if (m_ac)
            m_ac->post(CALL_RETURN_NULL);
    }
}

void WebSocket::free_mem()
{
    if (m_ioState.dec() == 0) {
        m_deflate.Release();
        m_inflate.Release();
        m_flushTail.Release();

        m_buffer.Release();

        m_holder.Release();

        if (m_compress)
            extMemory(-WS_DEFALTE_BUF_SIZE);
    }
}

void WebSocket::startRecv(Isolate* isolate)
{
    class asyncRead : public AsyncState {
    public:
        asyncRead(WebSocket* pThis, Isolate* isolate)
            : AsyncState(NULL)
            , m_this(pThis)
        {
            if (m_this->m_holder == NULL)
                m_this->m_holder = new ValueHolder(m_this->wrap());
            m_isolate = isolate;
            next(recv);
        }

        ~asyncRead()
        {
            m_this->free_mem();
            m_this->isolate_unref();
        }

        ON_STATE(asyncRead, recv)
        {
            if (m_this->m_compress && !m_this->m_inflate)
                m_this->m_inflate = new infraw(NULL, m_this->m_maxSize);

            m_msg = new WebSocketMessage(ws_base::C_TEXT, false, false, m_this->m_maxSize);
            return m_msg->readFrom(m_this->m_stream, m_this, next(event));
        }

        ON_STATE(asyncRead, event)
        {
            bool masked;
            m_msg->get_masked(masked);
            if (masked == m_this->m_masked) {
                m_msg->m_error = 1007;
                return CHECK_ERROR(Runtime::setError("WebSocketHandler: Payload data is not masked."));
            }

            int32_t type;
            m_msg->get_type(type);

            switch (type) {
            case ws_base::C_PING: {
                obj_ptr<SeekableStream_base> body;
                m_msg->get_body(body);
                (new asyncSend(m_this, body, ws_base::C_PONG))->post(0);
                break;
            }
            case ws_base::C_CLOSE: {
                obj_ptr<SeekableStream_base> body;
                m_msg->get_body(body);

                if (m_this->m_closeState.CompareAndSwap(ws_base::C_OPEN, ws_base::C_CLOSING) == ws_base::C_OPEN)
                    (new asyncSend(m_this, body, ws_base::C_CLOSE))->post(0);
                else
                    m_this->endConnect(body);

                return next(0);
            }
            case ws_base::C_TEXT:
            case ws_base::C_BINARY:
                m_this->_emit("message", m_msg);
            }

            return next(recv);
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

    if (m_stream && m_readState.xchg(ws_base::C_OPEN) != ws_base::C_OPEN)
        (new asyncRead(this, isolate))->apost(0);
}

void WebSocket::endConnect(int32_t code, exlib::string reason)
{
    if (m_closeState.xchg(ws_base::C_CLOSED) != ws_base::C_CLOSED) {
        Isolate* isolate = holder();
        if (isolate) {
            if (code > 1000 && code < 3000) {
                m_code = 1006;
                m_reason = "Abnormal Closure";

                _emit("error", new EventInfo(this, "error", code, reason));
            } else {
                m_code = code;
                m_reason = reason;
            }

            _emit("close", new EventInfo(this, "close", m_code, m_reason));
        }

        if (m_stream)
            m_stream->cc_close();

        if (m_ac)
            m_ac->post(CALL_RETURN_NULL);
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
    m_flushTail = new Buffer("\x0\x0\xff\xff", 4);
    extMemory(WS_DEFALTE_BUF_SIZE);
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

    if (m_readyState.CompareAndSwap(ws_base::C_OPEN, ws_base::C_CLOSING) != ws_base::C_OPEN)
        return 0;

    (new asyncSend(this, code, reason))->post(0);
    return 0;
}

result_t WebSocket::send(exlib::string data)
{
    switch (m_readyState) {
    case ws_base::C_CONNECTING:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CONNECTING state."));
    case ws_base::C_CLOSING:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CLOSING state."));
    case ws_base::C_CLOSED:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CLOSED state."));
    }

    (new asyncSend(this, data))->post(0);
    return 0;
}

result_t WebSocket::send(Buffer_base* data)
{
    switch (m_readyState) {
    case ws_base::C_CONNECTING:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CONNECTING state."));
    case ws_base::C_CLOSING:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CLOSING state."));
    case ws_base::C_CLOSED:
        return CHECK_ERROR(Runtime::setError("websocket: WebSocket is in CLOSED state."));
    }

    (new asyncSend(this, data))->post(0);
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
