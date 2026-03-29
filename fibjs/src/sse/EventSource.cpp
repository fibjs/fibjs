/*
 * EventSource.cpp
 *
 *  Created on: Dec 31, 2024
 */

#include "object.h"
#include "EventSource.h"
#include "HttpClient.h"
#include "HttpResponse.h"
#include "EventInfo.h"
#include "BufferedStream.h"
#include "Buffer.h"
#include "ifs/console.h"

namespace fibjs {

class AsyncEventSource : public AsyncState {
public:
    AsyncEventSource(HttpClient* hc, EventSource* es, exlib::string url)
        : AsyncState(NULL)
        , m_hc(hc)
        , m_es(es)
        , m_url(url)
    {
        m_isolate = es->holder();
        m_isolate->Ref();

        m_holder = new ValueHolder(es->wrap());

        next(open);
    }

    ~AsyncEventSource()
    {
        m_isolate->Unref();
    }

public:
    ON_STATE(AsyncEventSource, open)
    {
        if (m_es->m_readyState == sse_base::C_CLOSED)
            return next();

        obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)m_ctx[0].object();
        o->keepAlive = true;

        return m_hc->requestSync(o.get(), m_es->m_response, next(opened));
    }

    ON_STATE(AsyncEventSource, opened)
    {
        if (m_es->m_readyState == sse_base::C_CLOSED)
            return next();

        int32_t status;
        m_es->m_response->get_status(status);
        if (status != 200)
            return next(close_body);

        exlib::string contentType;
        m_es->m_response->firstHeader("Content-Type", contentType);
        if (qstricmp(contentType.c_str(), "text/event-stream", 17))
            return next(close_body);

        // The streaming path in asyncRequest has already assembled the transport
        // decode chain (ChunkedStream / RangeStream → BodyStream) and stored it
        // in m_bodyStream.  Just retrieve it and wrap with BufferedStream for
        // line-oriented SSE parsing.
        obj_ptr<Stream_base> stm;
        m_es->m_response->get_body(stm);

        m_sse_stm = new BufferedStream(stm);
        m_sse_stm->set_EOL("\n");

        m_es->m_readyState = sse_base::C_OPEN;
        (new EventInfo(m_es, "open"))->emit();

        return m_sse_stm->readLine(4096, strLine, next(read_message));
    }

    ON_STATE(AsyncEventSource, close_body)
    {
        // Error path: close the BodyStream to release the TCP connection.
        obj_ptr<Stream_base> stm;
        m_es->m_response->get_body(stm);
        if (stm)
            return stm->close(next(read_body_done));
        return next(read_body_done);
    }

    ON_STATE(AsyncEventSource, read_body_done)
    {
        int32_t status;
        HttpResponse_base* resp = static_cast<HttpResponse_base*>(m_es->m_response.get());
        resp->get_status(status);
        if (status != 200) {
            exlib::string statusMessage;
            resp->get_statusMessage(statusMessage);
            m_es->m_readyState = sse_base::C_CLOSED;
            (new EventInfo(m_es, "error", status, "Invalid status: " + statusMessage))->emit();
            return next();
        }

        exlib::string contentType;
        m_es->m_response->firstHeader("Content-Type", contentType);
        m_es->m_readyState = sse_base::C_CLOSED;
        (new EventInfo(m_es, "error", 0, "Invalid Content-Type: " + contentType))->emit();
        return next();
    }

    ON_STATE(AsyncEventSource, read_message)
    {
        if (m_es->m_readyState == sse_base::C_CLOSED)
            return next();

        if (strLine.empty()) {
            if (m_line_count == 0) {
                m_es->m_readyState = sse_base::C_CLOSED;
                (new EventInfo(m_es, "close"))->emit();
                return next();
            } else {
                obj_ptr<EventInfo> ei = new EventInfo(m_es, event);
                if (!id.empty())
                    ei->add("id", id);
                ei->add("data", data);
                ei->add("retry", retry);
                ei->emit();

                id.clear();
                event = "message";
                data.clear();
                retry.clear();
                m_line_count = 0;
            }
        } else {
            const char* p = strLine.c_str();
            exlib::string name, value;
            if (*p != ':') {
                const char* q = strchr(p, ':');
                if (q) {
                    name = exlib::string(p, q - p);
                    p = q + 1;
                    while (*p == ' ')
                        p++;

                    value = exlib::string(p, strLine.length() - (p - strLine.c_str()));
                } else
                    name = strLine;

                if (!qstricmp(name.c_str(), "id"))
                    id = value;
                else if (!qstricmp(name.c_str(), "event"))
                    event = value;
                else if (!qstricmp(name.c_str(), "data")) {
                    if (!data.empty())
                        data += "\n";
                    data += value;
                } else if (!qstricmp(name.c_str(), "retry"))
                    retry = value;
            }

            m_line_count++;

            strLine.clear();
        }

        return m_sse_stm->readLine(4096, strLine, next(read_message));
    }

    virtual int32_t error(int32_t v)
    {
        (new EventInfo(m_es, "error", 0, "Connection error"))->emit();
        return v;
    }

private:
    obj_ptr<HttpClient> m_hc;
    obj_ptr<EventSource> m_es;
    obj_ptr<ValueHolder> m_holder;
    exlib::string m_url;
    Isolate* m_isolate;
    obj_ptr<BufferedStream_base> m_sse_stm;
    exlib::string strLine;
    int32_t m_line_count = 0;
    exlib::string id;
    exlib::string event = "message";
    exlib::string data;
    exlib::string retry;
};

result_t EventSource_base::_new(exlib::string url, v8::Local<v8::Object> options,
    obj_ptr<EventSource_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<EventSource> es = new EventSource();
    es->wrap(This);

    retVal = es;

    Isolate* isolate = es->holder();
    obj_ptr<HttpClient_base> hc = isolate->m_httpclient;
    GetConfigValue(options, "httpClient", hc, true);

    AsyncEventSource* ac = new AsyncEventSource(hc.As<HttpClient>(), es, url);

    // Default to POST when a request body is implied by body/json/pack options,
    // mirroring the behaviour of http.post(). The caller can still override with
    // an explicit method option (e.g. method:'GET').
    exlib::string default_method = "GET";
    {
        Isolate* iso = Isolate::current();
        v8::Local<v8::Context> ctx = iso->context();
        auto has_key = [&](const char* key, int len) -> bool {
            JSValue v = options->Get(ctx, iso->NewString(key, len));
            return !v.IsEmpty() && !v->IsUndefined() && !v->IsNull();
        };
        if (has_key("body", 4) || has_key("json", 4) || has_key("pack", 4))
            default_method = "POST";
    }

    result_t hr = hc.As<HttpClient>()->get_request_opts(default_method, url, options, ac);
    if (hr != CALL_E_NOSYNC)
        return hr;

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    es->m_url = o->u->href();

    ac->apost(0);

    return 0;
}

result_t EventSource::close(AsyncEvent* ac)
{
    class asyncClose : public AsyncState {
    public:
        asyncClose(Stream_base* pStream, AsyncEvent* ac, AsyncEvent* ac_req)
            : AsyncState(ac)
            , m_stream(pStream)
            , m_ac_req(ac_req)
        {
            next(send);
        }

    public:
        ON_STATE(asyncClose, send)
        {
            m_buf = new Buffer("0\r\n\r\n", 5);
            return m_stream->writeBuffer(m_buf, next(done));
        }

        ON_STATE(asyncClose, done)
        {
            m_ac_req->post(CALL_RETURN_NULL);
            return next();
        }

        virtual int32_t error(int32_t v)
        {
            m_ac_req->post(v);
            return v;
        }

    private:
        obj_ptr<Stream_base> m_stream;
        AsyncEvent* m_ac_req;
        obj_ptr<Buffer> m_buf;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_readyState == sse_base::C_OPEN) {
        m_readyState = sse_base::C_CLOSED;

        if (m_response) {
            obj_ptr<Stream_base> stm;
            m_response->get_stream(stm);
            return stm->close(ac);
        }
    } else if (m_readyState == sse_base::C_SENDER) {
        m_readyState = sse_base::C_CLOSED;
        (new asyncClose(m_stream, ac, m_ac))->apost(0);
        return CALL_E_PENDDING;
    }

    return 0;
}

result_t EventSource::send(exlib::string data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    if (m_readyState != sse_base::C_SENDER) {
        return CHECK_ERROR(Runtime::setError("EventSource.send: can only be called when readyState is sse.SENDER"));
    }

    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(options);

        obj_ptr<SendOptions> opts;
        result_t hr = SendOptions::load(options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = opts;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    SendOptions* opts = (SendOptions*)ac->m_ctx[0].object();

    std::vector<int32_t> breakLinePos;
    breakLinePos.reserve(data.length() + 1);
    breakLinePos.push_back(-1); // Start position marker
    const char* p = data.c_str();
    for (int32_t i = 0; i < data.length(); ++i) {
        if (p[i] == '\n') {
            breakLinePos.push_back(i);
        }
    }

    int32_t buf_size = breakLinePos.size() * 6 + data.length() + 2; // "data: \n" for each line and appending "\n"

    if (opts->id.has_value())
        buf_size += opts->id->length() + 5; // "id: \n"

    if (opts->event.has_value())
        buf_size += opts->event->length() + 8; // "event: \n"

    char retry_str[32];
    int32_t retry_len = 0;
    if (opts->retry.has_value()) {
        if (*opts->retry < 0)
            return CHECK_ERROR(Runtime::setError("EventSource.send: retry must be a positive integer."));
        retry_len = snprintf(retry_str, sizeof(retry_str), "%d", *opts->retry);
        buf_size += retry_len + 8; // "retry: \n"
    }

    char chunded_size_str[32];
    int32_t chunded_size_len = snprintf(chunded_size_str, sizeof(chunded_size_str), "%x", buf_size);
    buf_size += chunded_size_len + 4; // "data: \r\n" + "\r\n"

    obj_ptr<Buffer> buf = new Buffer(nullptr, buf_size);
    uint8_t* pBuf = buf->data();
    int32_t pos = 0;

    memcpy(pBuf, chunded_size_str, chunded_size_len);
    pos += chunded_size_len;
    pBuf[pos++] = '\r';
    pBuf[pos++] = '\n';

    if (opts->id.has_value()) {
        memcpy(pBuf + pos, "id: ", 4);
        pos += 4;
        memcpy(pBuf + pos, opts->id->c_str(), opts->id->length());
        pos += opts->id->length();
        pBuf[pos++] = '\n';
    }

    if (opts->event.has_value()) {
        memcpy(pBuf + pos, "event: ", 7);
        pos += 7;
        memcpy(pBuf + pos, opts->event->c_str(), opts->event->length());
        pos += opts->event->length();
        pBuf[pos++] = '\n';
    }

    if (opts->retry.has_value()) {
        memcpy(pBuf + pos, "retry: ", 7);
        pos += 7;
        memcpy(pBuf + pos, retry_str, retry_len);
        pos += retry_len;
        pBuf[pos++] = '\n';
    }

    for (int32_t i = 1; i <= breakLinePos.size(); ++i) {
        memcpy(pBuf + pos, "data: ", 6);
        pos += 6;
        int32_t start = breakLinePos[i - 1] + 1;
        int32_t end = (i == breakLinePos.size()) ? data.length() : breakLinePos[i];
        int32_t len = end - start;
        if (len > 0) {
            memcpy(pBuf + pos, data.c_str() + start, len);
            pos += len;
        }
        pBuf[pos++] = '\n';
    }
    pBuf[pos++] = '\n';

    pBuf[pos++] = '\r';
    pBuf[pos++] = '\n';

    return m_stream->writeBuffer(buf, ac);
}

result_t EventSource::get_readyState(int32_t& retVal)
{
    retVal = m_readyState;
    return 0;
}

result_t EventSource::get_url(exlib::string& retVal)
{
    retVal = m_url;
    return 0;
}

result_t EventSource::get_withCredentials(bool& retVal)
{
    retVal = false;
    return 0;
}

result_t EventSource::get_response(obj_ptr<HttpResponse_base>& retVal)
{
    if (!m_response)
        return CALL_RETURN_NULL;

    retVal = static_cast<HttpResponse_base*>(m_response.get());

    return 0;
}

}
