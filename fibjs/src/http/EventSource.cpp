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
#include "ChunkedStream.h"
#include "BufferedStream.h"
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
        if (m_es->m_readyState == EventSource::C_CLOSED)
            return next();

        return m_hc->request("POST", "", v8::Local<v8::Object>(), m_es->m_response, next(opened), true);
    }

    ON_STATE(AsyncEventSource, opened)
    {
        if (m_es->m_readyState == EventSource::C_CLOSED)
            return next();

        int32_t status;
        m_es->m_response->get_status(status);
        if (status != 200)
            return next(read_body);

        exlib::string contentType;
        m_es->m_response->firstHeader("Content-Type", contentType);
        if (qstricmp(contentType.c_str(), "text/event-stream", 17))
            return next(read_body);

        obj_ptr<Stream_base> stm;
        m_es->m_response->get_stream(stm);

        if (m_es->m_response.As<HttpResponse>()->m_message->m_bChunked)
            stm = new ChunkedStream(stm.As<BufferedStream_base>(), m_hc->m_maxChunkSize, m_hc->m_maxBodySize);

        m_sse_stm = new BufferedStream(stm);
        m_sse_stm->set_EOL("\n");

        m_es->m_readyState = EventSource::C_OPEN;
        (new EventInfo(m_es, "open"))->emit();

        return m_sse_stm->readLine(4096, strLine, next(read_message));
    }

    ON_STATE(AsyncEventSource, read_body)
    {
        m_response = m_es->m_response.As<HttpResponse>();
        return m_response->readBody(next(read_body_done));
    }

    ON_STATE(AsyncEventSource, read_body_done)
    {
        int32_t status;
        m_es->m_response->get_status(status);
        if (status != 200) {
            exlib::string statusMessage;
            m_es->m_response->get_statusMessage(statusMessage);
            m_es->m_readyState = EventSource::C_CLOSED;
            (new EventInfo(m_es, "error", status, "Invalid status: " + statusMessage))->emit();
            return next();
        }

        exlib::string contentType;
        m_es->m_response->firstHeader("Content-Type", contentType);
        m_es->m_readyState = EventSource::C_CLOSED;
        (new EventInfo(m_es, "error", 0, "Invalid Content-Type: " + contentType))->emit();
        return next();
    }

    ON_STATE(AsyncEventSource, read_message)
    {
        if (m_es->m_readyState == EventSource::C_CLOSED)
            return next();

        if (strLine.empty()) {
            if (m_line_count == 0) {
                m_es->m_readyState = EventSource::C_CLOSED;
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
    obj_ptr<HttpResponse> m_response;
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
    GetConfigValue(isolate, options, "httpClient", hc, true);

    AsyncEventSource* ac = new AsyncEventSource(hc.As<HttpClient>(), es, url);

    result_t hr = hc.As<HttpClient>()->get_request_opts("POST", url, options, ac);
    if (hr != CALL_E_NOSYNC)
        return hr;

    obj_ptr<Url> u = (Url*)ac->m_ctx[1].object();
    es->m_url = u->href();

    ac->apost(0);

    return 0;
}

result_t EventSource::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_readyState = C_CLOSED;

    if (m_response) {
        obj_ptr<Stream_base> stm;
        m_response->get_stream(stm);
        return stm->close(ac);
    }

    return 0;
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

    retVal = m_response;

    return 0;
}

}
