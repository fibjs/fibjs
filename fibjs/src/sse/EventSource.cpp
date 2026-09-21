/*
 * EventSource.cpp
 *
 *  Created on: Dec 31, 2024
 */

#include "object.h"
#include "EventSource.h"
#include "HttpClient.h"
#include "EventInfo.h"
#include "BufferedStream.h"
#include "Buffer.h"
#include "ifs/console.h"

namespace fibjs {

// 操作型 hold token：统一管理一次 EventSource 读取操作（= 一个 ref 事务）对 isolate 的持有。
//
// 引用语义（重要）：
//   - isolate_ref() 在事务开始时（构造）获取，代表“本事务未结束，isolate 不能 idle”
//   - isolate_unref() 只在事务真正结束时（token 析构，即 AsyncEventSource 销毁）释放
//   - stop() 只负责中断事务，**不得**自行 unref：
//     否则会在读状态机仍在跑的时候就把 isolate 放回可 idle 状态，
//     导致 isolate 带着未结束的事务进入收尾。
class EventSourceHoldToken : public object_base {
public:
    EventSourceHoldToken(Isolate* isolate)
    {
        holder(isolate);
        isolate_ref();
    }

    // 在 JS 线程上登记读循环正在使用的 BufferedStream
    void setStream(BufferedStream_base* stm)
    {
        m_lock.lock();
        m_stream = stm;
        m_lock.unlock();
    }

    void clearStream()
    {
        m_lock.lock();
        m_stream.Release();
        m_lock.unlock();
    }

    // isolate 终止时由 stopHoldingObjects() 调用：
    // 只中断事务，让事务自己走完结束流程。
    virtual result_t stop()
    {
        obj_ptr<BufferedStream_base> stm;

        m_lock.lock();
        stm = m_stream;
        m_stream.Release();
        m_lock.unlock();

        // 关闭读者正在使用的 BufferedStream（不是传输层）。
        // 这会同步驱动读状态机收尾：readLine() 以 EOF 返回，
        // 状态机跑完后 AsyncState::post() 会 delete this，
        // 进而释放本 token 并在此处之外完成 isolate_unref()。
        if (stm)
            stm->cc_close(holder());

        return 0;
    }

    ~EventSourceHoldToken()
    {
        // 事务结束的唯一释放点
        isolate_unref();
    }

private:
    exlib::spinlock m_lock;
    obj_ptr<BufferedStream_base> m_stream;
};

class AsyncEventSource : public AsyncState {
public:
    AsyncEventSource(HttpClient* hc, EventSource* es, exlib::string url)
        : AsyncState(NULL)
        , m_hc(hc)
        , m_es(es)
        , m_url(url)
    {
        m_isolate = es->holder();
        m_token = new EventSourceHoldToken(m_isolate);

        m_holder = new ValueHolder(es->wrap());

        init(open);
    }

    ~AsyncEventSource()
    {
        // 事务结束：摘掉读流并释放 token，
        // isolate_unref() 由 ~EventSourceHoldToken() 完成。
        m_token->clearStream();
        m_token.Release();
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

        obj_ptr<Stream_base> stm;
        m_es->m_response->get_body(stm);

        m_sse_stm = new BufferedStream(stm);
        m_sse_stm->set_EOL("\n");
        m_token->setStream(m_sse_stm);

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
                m_token->clearStream();
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
    obj_ptr<EventSourceHoldToken> m_token;
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

// 对端已经消失（客户端断开、进程被终止、网络中断）时，终止分块 "0\r\n\r\n" 已经没有
// 接收方。写入是唯一能发现这一点的方式，而 close() 无论如何都必须完成：连接已经结束，
// 调用方也无法从一次失败的 close 中恢复。若把这类错误当成失败上报，关闭一个已死的
// sender 会同时让调用方抛异常、并让仍然挂着的请求以错误收尾（Windows 上表现为
// ERROR_NETNAME_DELETED(64) -> ECONNRESET）。
static bool is_connection_gone(result_t hr)
{
    if (hr >= 0)
        return false;

    // socket 层可能回投 libuv 错误码，也可能回投原始系统 errno（Windows 上不是同一套
    // 取值），两种都接受。
    switch (hr) {
    case UV_ECONNRESET:
    case UV_EPIPE:
    case UV_ENOTCONN:
    case UV_ECONNABORTED:
        return true;
    }

    switch (uv_translate_sys_error(-hr)) {
    case UV_ECONNRESET:
    case UV_EPIPE:
    case UV_ENOTCONN:
    case UV_ECONNABORTED:
        return true;
    }

    return false;
}

// 一次性票据：包装 sse.upgrade 交出的「票」（AsyncState 的 continuation）。
// 票指向的机器在 post_ 结尾 delete this —— 所以「回投有且只有一次」是硬约束：
// 第二次回投就是对已释放对象的虚调用（Windows worker 套件 core dump 的成因）。
// 认领（xchg）之后的重复/迟到回投一律 no-op，于是任意回投路径组合都安全：
// close() 的 done、error()、~EventSource()，以及将来任何收尾入口。
class EventSource::Ticket : public AsyncEvent {
public:
    Ticket(AsyncEvent* target)
        : AsyncEvent(target->isolate())
        , m_target(target)
    {
        setAsync();
    }

public:
    virtual int32_t post(int32_t v) override
    {
        if (m_claimed.xchg(1) == 0)
            m_target->post(v);
        return 0;
    }

    virtual void apost(int32_t v) override
    {
        if (m_claimed.xchg(1) == 0)
            m_target->apost(v);
    }

    virtual Isolate* isolate() override
    {
        return m_target->isolate();
    }

private:
    AsyncEvent* m_target; /* 票据目标，只在首次送达时使用 */
    exlib::atomic m_claimed { 0 };
};

void EventSource::setTicket(AsyncEvent* target)
{
    m_ac = new Ticket(target);
}

EventSource::~EventSource()
{
    // 对象被回收时票据还挂着：收掉它，避免请求永远 pending（对齐 ~WebSocket）
    if (m_ac) {
        m_ac->post(CALL_RETURN_NULL);
        delete m_ac;
        m_ac = nullptr;
    }
}

result_t EventSource::close(AsyncEvent* ac)
{
    class asyncClose : public AsyncState {
    public:
        asyncClose(Stream_base* pStream, AsyncEvent* ac, Ticket* ac_req)
            : AsyncState(ac)
            , m_stream(pStream)
            , m_ac_req(ac_req)
        {
            init(send);
        }

        ~asyncClose() override
        {
            // 票据由本状态机消费：无论走 done 还是 error，都在这里释放
            delete m_ac_req;
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
            // 对端已消失时终止分块写不出去，但关闭本身已经完成：按成功收尾，让
            // close() 正常返回、挂着的请求正常收尾（见 is_connection_gone）。
            //
            // 这里可以放心地「先投递 + 返回非负」（AsyncState::post_ 会把非负返回值
            // 当作继续，于是 done 再投一次）：票据是一次性的，重复回投是 no-op。
            // 正确性由 EventSource::Ticket 的认领语义保证，不依赖本函数的返回值约定。
            if (is_connection_gone(v))
                v = CALL_RETURN_NULL;

            m_ac_req->post(v);
            return v;
        }

    private:
        obj_ptr<Stream_base> m_stream;
        Ticket* m_ac_req;
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

        // 票据交给 close 状态机消费（一次性），本对象不再持有。认领语义见
        // EventSource::Ticket：重复/迟到的回投都是 no-op。
        Ticket* ac_req = m_ac;
        m_ac = nullptr;

        (new asyncClose(m_stream, ac, ac_req))->apost(0);
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
