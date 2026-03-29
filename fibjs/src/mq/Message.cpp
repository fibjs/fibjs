/*
 * _Message.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "Message.h"
#include "MemoryStream.h"
#include "Buffer.h"
#include "Blob.h"
#include "Isolate.h"
#include "ifs/json.h"
#include "ifs/msgpack.h"
#include "ifs/fs.h"
#include "v8_api.h"
#include <functional>

namespace fibjs {

result_t Message::get_sent(bool& retVal)
{
    retVal = false;
    return 0;
}

result_t Message::get_value(exlib::string& retVal)
{
    retVal = m_value;
    return 0;
}

result_t Message::set_value(exlib::string newVal)
{
    m_value = newVal;
    return 0;
}

result_t Message::get_params(obj_ptr<NArray>& retVal)
{
    if (m_params == NULL)
        m_params = new NArray();

    retVal = m_params;
    return 0;
}

result_t Message::get_type(int32_t& retVal)
{
    retVal = m_type;
    return 0;
}

result_t Message::set_type(int32_t newVal)
{
    m_type = newVal;
    return 0;
}

result_t Message::get_body(obj_ptr<Stream_base>& retVal)
{
    // Streaming body takes precedence over buffered body
    if (m_bodyStream) {
        retVal = m_bodyStream;
        return 0;
    }

    if (m_body == NULL)
        return CALL_RETURN_NULL;

    // Return null for empty body (Web API compatibility)
    int64_t size = 0;
    m_body->size(size);
    if (size == 0)
        return CALL_RETURN_NULL;

    retVal = m_body;
    return 0;
}

result_t Message::set_body(Stream_base* newVal)
{
    if (newVal) {
        obj_ptr<SeekableStream_base> seekable = SeekableStream_base::getInstance(newVal);
        if (seekable) {
            m_body = seekable;
            m_bodyStream.Release();
        } else {
            m_bodyStream = newVal;
            m_body.Release();
        }
    } else {
        m_body.Release();
        m_bodyStream.Release();
    }
    return 0;
}

result_t Message::get_bodyUsed(bool& retVal)
{
    retVal = m_bodyUsed;
    return 0;
}

result_t Message::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (m_bodyStream)
        return m_bodyStream->readBuffer(bytes, retVal, ac);

    if (m_body == NULL)
        return CALL_RETURN_NULL;

    return m_body->readBuffer(bytes, retVal, ac);
}

// Reads the full message body then invokes fn(status, data).
// Handles streaming (loop-reads Stream_base), buffered (SeekableStream_base), and null body.
class asyncConsumeBody : public AsyncState {
public:
    using ProcessFn = std::function<result_t(result_t, obj_ptr<Buffer_base>)>;

    asyncConsumeBody(Message* pThis, ProcessFn fn, AsyncEvent* ac)
        : AsyncState(ac)
        , m_pThis(pThis)
        , m_fn(std::move(fn))
    {
        if (pThis->m_bodyUsed && !pThis->m_body) {
            // Enforce single-consumption only for streaming bodies (no seekable
            // m_body to rewind). Buffered bodies (m_body != null) may be re-read.
            next(alreadyUsed);
            return;
        }
        if (pThis->m_bodyStream)
            next(read);
        else if (pThis->m_body) {
            pThis->m_body->rewind();
            next(seekable);
        } else
            next(noBody);
    }

    // Streaming path: loop-read until EOF
    ON_STATE(asyncConsumeBody, read)
    {
        return m_pThis->m_bodyStream->readBuffer(-1, m_chunk, next(process));
    }

    ON_STATE(asyncConsumeBody, process)
    {
        if (n == CALL_RETURN_NULL) {
            obj_ptr<Buffer_base> data;
            if (!m_buf.empty())
                data = new Buffer(m_buf.c_str(), m_buf.length());
            return next(m_fn(m_buf.empty() ? CALL_RETURN_NULL : 0, data));
        }
        if (m_chunk) {
            Buffer* b = (Buffer*)m_chunk.get();
            m_buf.append((const char*)b->data(), b->length());
            m_chunk.Release();
        }
        return next(read);
    }

    // Buffered path: single readAll on seekable stream
    ON_STATE(asyncConsumeBody, seekable)
    {
        return m_pThis->m_body->readAll(m_data, next(done));
    }

    ON_STATE(asyncConsumeBody, done)
    {
        return next(m_fn(n, std::move(m_data)));
    }

    // No body path
    ON_STATE(asyncConsumeBody, noBody)
    {
        return next(m_fn(CALL_RETURN_NULL, nullptr));
    }

    // Body already consumed path: throw TypeError
    ON_STATE(asyncConsumeBody, alreadyUsed)
    {
        return next(CHECK_ERROR(Runtime::setError(kTypeError, "body has already been consumed")));
    }

private:
    obj_ptr<Message> m_pThis;
    ProcessFn m_fn;
    obj_ptr<Buffer_base> m_data;
    obj_ptr<Buffer_base> m_chunk;
    exlib::string m_buf;
};

result_t Message::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (!m_bodyStream && !m_body)
        return CALL_RETURN_NULL;
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);
    return (new asyncConsumeBody(this, [&retVal](result_t n, obj_ptr<Buffer_base> data) -> result_t {
        retVal = data;
        return n;
    }, ac))->post(0);
}

result_t Message::bytes(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncConsumeBody(this, [this, &retVal](result_t n, obj_ptr<Buffer_base> data) -> result_t {
        m_bodyUsed = true;
        retVal = (n == CALL_RETURN_NULL || !data) ? new Buffer("", 0) : data;
        return 0;
    }, ac))->post(0);
}

result_t Message::blob(exlib::string type, obj_ptr<Blob_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncConsumeBody(this, [this, type, &retVal](result_t n, obj_ptr<Buffer_base> data) -> result_t {
        m_bodyUsed = true;
        if (n == CALL_RETURN_NULL || !data)
            data = new Buffer("", 0);
        retVal = new Blob(data, type);
        return 0;
    }, ac))->post(0);
}

result_t Message::write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    return m_body->writeBuffer(data, ac);
}

result_t Message::text(exlib::string data, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = new Buffer(data.c_str(), data.length());
        return CALL_E_NOSYNC;
    }

    obj_ptr<Buffer_base> buf = (Buffer_base*)ac->m_ctx[0].object();
    obj_ptr<MemoryStream> ms = new MemoryStream();
    ms->writeBuffer(buf, nullptr);
    m_body = ms;
    return 0;
}

result_t Message::text(exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncConsumeBody(this, [this, &retVal](result_t n, obj_ptr<Buffer_base> data) -> result_t {
        m_bodyUsed = true;
        if (n == CALL_RETURN_NULL || !data) { retVal = ""; return 0; }
        return data->toString(retVal);
    }, ac))->post(0);
}

result_t Message::arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncConsumeBody(this, [this, &retVal](result_t n, obj_ptr<Buffer_base> data) -> result_t {
        m_bodyUsed = true;
        if (n == CALL_RETURN_NULL || !data) { retVal = NewBackingStore(0); return 0; }
        retVal = data.As<Buffer>()->backingStore();
        return 0;
    }, ac))->post(0);
}

result_t Message::json(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        exlib::string str;
        result_t hr = json_base::encode(data, str);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = new Buffer(str.c_str(), str.length());

        return CALL_E_NOSYNC;
    }

    obj_ptr<Buffer_base> buf = (Buffer_base*)ac->m_ctx[0].object();
    obj_ptr<MemoryStream> ms = new MemoryStream();
    ms->writeBuffer(buf, nullptr);
    m_body = ms;
    return 0;
}

result_t Message::json(Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncConsumeBody(this, [this, &retVal](result_t n, obj_ptr<Buffer_base> data) -> result_t {
        m_bodyUsed = true;
        if (n == CALL_RETURN_NULL || !data)
            return CALL_RETURN_NULL;
        exlib::string str;
        data->toString(str);
        retVal.setJSON(str);
        return 0;
    }, ac))->post(0);
}

result_t Message::pack(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        obj_ptr<Buffer_base> buf;
        result_t hr = msgpack_base::encode(data, buf);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = buf;

        return CALL_E_NOSYNC;
    }

    obj_ptr<Buffer_base> buf = (Buffer_base*)ac->m_ctx[0].object();
    obj_ptr<MemoryStream> ms = new MemoryStream();
    ms->writeBuffer(buf, nullptr);
    m_body = ms;
    return 0;
}

result_t Message::pack(Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncConsumeBody(this, [this, &retVal](result_t n, obj_ptr<Buffer_base> data) -> result_t {
        if (n == CALL_RETURN_NULL || !data)
            return CALL_RETURN_NULL;
        Buffer* buf = data.As<Buffer>();
        retVal.setMsgpack(exlib::string((const char*)buf->data(), buf->length()));
        return 0;
    }, ac))->post(0);
}

result_t Message::get_length(int64_t& retVal)
{
    if (m_body == NULL) {
        retVal = 0;
        return 0;
    }
    return m_body->size(retVal);
}

result_t Message_base::_new(obj_ptr<Message_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Message();
    return 0;
}

result_t Message::end(int32_t& retVal, AsyncEvent* ac)
{
    m_end = true;
    retVal = 0;
    return 0;
}

result_t Message::end(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    m_end = true;
    return m_body->end(data, retVal, ac);
}

result_t Message::end(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    m_end = true;
    return m_body->end(data, encoding, retVal, ac);
}

result_t Message::end(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    m_end = true;
    return m_body->end(data, encoding, retVal, ac);
}

result_t Message::isEnded(bool& retVal)
{
    retVal = m_end;
    return 0;
}

result_t Message::clear()
{
    m_end = false;
    m_params.Release();
    m_value.clear();
    m_body.Release();
    m_bodyStream.Release();

    return 0;
}

result_t Message::sendTo(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::readFrom(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::get_stream(obj_ptr<Stream_base>& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::get_lastError(exlib::string& retVal)
{
    retVal = m_lastError;
    return 0;
}

result_t Message::set_lastError(exlib::string newVal)
{
    m_lastError = newVal;
    return 0;
}

static bool is_stream_event(exlib::string& ev)
{
    return ev == "data" || ev == "end" || ev == "close"
        || ev == "error" || ev == "readable";
}

result_t Message::onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
{
    if (type == "newListener" && is_stream_event(ev)) {
        obj_ptr<Stream_base> body;
        if (get_body(body) == 0 && body) {
            v8::Local<v8::Object> retVal;
            body->on(ev, func, retVal);
        }
    } else if (type == "removeListener" && is_stream_event(ev)) {
        obj_ptr<Stream_base> body;
        if (get_body(body) == 0 && body) {
            v8::Local<v8::Object> retVal;
            body->off(ev, func, retVal);
        }
    }

    return 0;
}

result_t Message::clone(obj_ptr<Message_base>& retVal)
{
    // Message is an abstract base class, cannot be cloned directly
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::resume(obj_ptr<Message_base>& retVal)
{
    obj_ptr<Stream_base> body;
    if (get_body(body) == 0 && body) {
        obj_ptr<Stream_base> r;
        body->resume(r);
    }
    retVal = this;
    return 0;
}

result_t Message::pause(obj_ptr<Message_base>& retVal)
{
    obj_ptr<Stream_base> body;
    if (get_body(body) == 0 && body) {
        obj_ptr<Stream_base> r;
        body->pause(r);
    }
    retVal = this;
    return 0;
}

result_t Message::pipe(v8::Local<v8::Value> destination, v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal)
{
    return holder()->call_pipe(wrap(), destination, options, retVal);
}

result_t Message::unpipe(Stream_base* destination)
{
    obj_ptr<Stream_base> body;
    if (get_body(body) == 0 && body)
        body->unpipe(destination);
    return 0;
}

void Message::copyTo(Message* target)
{
    target->m_type = m_type;
    target->m_value = m_value;
    target->m_lastError = m_lastError;
    target->m_end = m_end;

    // Clone params array
    if (m_params) {
        target->m_params = new NArray();
        int32_t len = m_params->length();
        for (int32_t i = 0; i < len; i++) {
            Variant v;
            m_params->_indexed_getter(i, v);
            target->m_params->append(v);
        }
    }

    // Clone body stream
    if (m_body) {
        int64_t pos;
        m_body->tell(pos);
        m_body->rewind();

        obj_ptr<Buffer_base> buf;
        result_t hr = m_body->ac_readAll(buf);

        m_body->seek(pos, fs_base::C_SEEK_SET);

        if (hr >= 0 && buf) {
            obj_ptr<MemoryStream> ms = new MemoryStream();
            ms->writeBuffer(buf, nullptr);
            ms->rewind();
            target->m_body = ms;
        }
    }
}

} /* namespace fibjs */
