/*
 * Http2Stream.cpp
 *
 *  Created on: Mar 21, 2026
 *      Author: lion
 */

#include "object.h"
#include "Http2Stream.h"
#include "Http2Session.h"
#include "Buffer.h"
#include "Fiber.h"
#include <nghttp2/nghttp2.h>

namespace fibjs {

result_t Http2Stream::get_id(int32_t& retVal)
{
    retVal = m_stream_id;
    return 0;
}

result_t Http2Stream::get_closed(bool& retVal)
{
    retVal = m_closed;
    return 0;
}

result_t Http2Stream::get_destroyed(bool& retVal)
{
    retVal = m_destroyed;
    return 0;
}

result_t Http2Stream::get_headers(obj_ptr<NObject>& retVal)
{
    if (!m_headers)
        return CALL_RETURN_NULL;

    retVal = m_headers;
    return 0;
}

result_t Http2Stream::respond(v8::Local<v8::Object> headers)
{
    if (m_closed || m_destroyed)
        return Runtime::setError("Http2Stream: stream is closed.");

    if (!m_session || !m_session->m_nghttp2)
        return Runtime::setError("Http2Stream: session is destroyed.");

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    // Convert headers object to nghttp2_nv array
    v8::Local<v8::Array> keys = headers->GetOwnPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
    uint32_t nheaders = keys.IsEmpty() ? 0 : keys->Length();

    std::vector<nghttp2_nv> nva;
    std::vector<exlib::string> name_bufs;
    std::vector<exlib::string> value_bufs;
    name_bufs.reserve(nheaders + 1);
    value_bufs.reserve(nheaders + 1);

    // Ensure :status is included
    bool has_status = false;
    for (uint32_t i = 0; i < nheaders; i++) {
        v8::Local<v8::Value> key = keys->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        v8::String::Utf8Value key_str(isolate->m_isolate, key);
        if (strcmp(*key_str, ":status") == 0) {
            has_status = true;
            break;
        }
    }

    if (!has_status) {
        name_bufs.push_back(":status");
        value_bufs.push_back("200");
        nghttp2_nv nv = {
            (uint8_t*)name_bufs.back().c_str(), (uint8_t*)value_bufs.back().c_str(),
            name_bufs.back().length(), value_bufs.back().length(),
            NGHTTP2_NV_FLAG_NO_COPY_NAME | NGHTTP2_NV_FLAG_NO_COPY_VALUE
        };
        nva.push_back(nv);
    }

    for (uint32_t i = 0; i < nheaders; i++) {
        v8::Local<v8::Value> key = keys->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        v8::Local<v8::Value> val = headers->Get(context, key).FromMaybe(v8::Local<v8::Value>());

        v8::String::Utf8Value key_str(isolate->m_isolate, key);
        v8::String::Utf8Value val_str(isolate->m_isolate, val);

        name_bufs.push_back(exlib::string(*key_str, key_str.length()));
        value_bufs.push_back(exlib::string(*val_str, val_str.length()));

        nghttp2_nv nv = {
            (uint8_t*)name_bufs.back().c_str(), (uint8_t*)value_bufs.back().c_str(),
            name_bufs.back().length(), value_bufs.back().length(),
            NGHTTP2_NV_FLAG_NO_COPY_NAME | NGHTTP2_NV_FLAG_NO_COPY_VALUE
        };
        nva.push_back(nv);
    }

    nghttp2_data_provider data_prd;
    data_prd.source.ptr = this;
    data_prd.read_callback = Http2Session::data_source_read_callback;

    int rv = m_session->m_nghttp2.submit_response(m_stream_id, nva.data(), nva.size(), &data_prd);
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Stream: submit response failed: ") + nghttp2_strerror(rv));

    m_session->asyncFlushOutput();
    return 0;
}

result_t Http2Stream::additionalHeaders(v8::Local<v8::Object> headers)
{
    if (m_closed || m_destroyed)
        return Runtime::setError("Http2Stream: stream is closed.");

    // Submit additional 1xx informational headers
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Array> keys = headers->GetOwnPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
    uint32_t nheaders = keys.IsEmpty() ? 0 : keys->Length();

    std::vector<nghttp2_nv> nva;
    std::vector<exlib::string> name_bufs;
    std::vector<exlib::string> value_bufs;
    name_bufs.reserve(nheaders);
    value_bufs.reserve(nheaders);

    for (uint32_t i = 0; i < nheaders; i++) {
        v8::Local<v8::Value> key = keys->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        v8::Local<v8::Value> val = headers->Get(context, key).FromMaybe(v8::Local<v8::Value>());

        v8::String::Utf8Value key_str(isolate->m_isolate, key);
        v8::String::Utf8Value val_str(isolate->m_isolate, val);

        name_bufs.push_back(exlib::string(*key_str, key_str.length()));
        value_bufs.push_back(exlib::string(*val_str, val_str.length()));

        nghttp2_nv nv = {
            (uint8_t*)name_bufs.back().c_str(), (uint8_t*)value_bufs.back().c_str(),
            name_bufs.back().length(), value_bufs.back().length(),
            NGHTTP2_NV_FLAG_NO_COPY_NAME | NGHTTP2_NV_FLAG_NO_COPY_VALUE
        };
        nva.push_back(nv);
    }

    int rv = m_session->m_nghttp2.submit_headers(m_stream_id, nva.data(), nva.size());
    if (rv < 0)
        return Runtime::setError(exlib::string("Http2Stream: submit headers failed: ") + nghttp2_strerror(rv));

    m_session->asyncFlushOutput();
    return 0;
}

result_t Http2Stream::sendTrailers(v8::Local<v8::Object> headers)
{
    if (m_closed || m_destroyed)
        return Runtime::setError("Http2Stream: stream is closed.");

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Array> keys = headers->GetOwnPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
    uint32_t nheaders = keys.IsEmpty() ? 0 : keys->Length();

    std::vector<nghttp2_nv> nva;
    std::vector<exlib::string> name_bufs;
    std::vector<exlib::string> value_bufs;
    name_bufs.reserve(nheaders);
    value_bufs.reserve(nheaders);

    for (uint32_t i = 0; i < nheaders; i++) {
        v8::Local<v8::Value> key = keys->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        v8::Local<v8::Value> val = headers->Get(context, key).FromMaybe(v8::Local<v8::Value>());

        v8::String::Utf8Value key_str(isolate->m_isolate, key);
        v8::String::Utf8Value val_str(isolate->m_isolate, val);

        name_bufs.push_back(exlib::string(*key_str, key_str.length()));
        value_bufs.push_back(exlib::string(*val_str, val_str.length()));

        nghttp2_nv nv = {
            (uint8_t*)name_bufs.back().c_str(), (uint8_t*)value_bufs.back().c_str(),
            name_bufs.back().length(), value_bufs.back().length(),
            NGHTTP2_NV_FLAG_NO_COPY_NAME | NGHTTP2_NV_FLAG_NO_COPY_VALUE
        };
        nva.push_back(nv);
    }

    int rv = m_session->m_nghttp2.submit_trailer(m_stream_id, nva.data(), nva.size());
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Stream: submit trailers failed: ") + nghttp2_strerror(rv));

    m_session->asyncFlushOutput();
    return 0;
}

result_t Http2Stream::rstStream(int32_t code)
{
    if (m_destroyed)
        return Runtime::setError("Http2Stream: stream is destroyed.");

    if (!m_session || !m_session->m_nghttp2)
        return Runtime::setError("Http2Stream: session is destroyed.");

    int rv = m_session->m_nghttp2.submit_rst_stream(m_stream_id, code);
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Stream: submit rst_stream failed: ") + nghttp2_strerror(rv));

    m_session->asyncFlushOutput();
    return 0;
}

result_t Http2Stream::get_fd(int32_t& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Http2Stream::readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    class AsyncRead : public AsyncState {
    public:
        AsyncRead(Http2Stream* stream, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_stream(stream)
            , m_retVal(retVal)
        {
            next(try_lock);
        }

        ~AsyncRead()
        {
            if (m_locked)
                m_stream->m_read_lock.unlock(this);
        }

    public:
        ON_STATE(AsyncRead, try_lock)
        {
            m_locked = true;
            return m_stream->m_read_lock.lock(next(read)) ? 0 : CALL_E_PENDDING;
        }

        ON_STATE(AsyncRead, read)
        {
            m_stream->m_recv_lock.lock();

            if (!m_stream->m_recv_queue.empty()) {
                m_retVal = m_stream->m_recv_queue.front();
                m_stream->m_recv_queue.pop_front();
                m_stream->m_recv_lock.unlock();
                m_stream->m_read_lock.unlock(this);
                m_locked = false;
                return next();
            }

            if (m_stream->m_recv_end) {
                m_stream->m_recv_lock.unlock();
                m_stream->m_read_lock.unlock(this);
                m_locked = false;
                return next(CALL_RETURN_NULL);
            }

            if (m_stream->m_closed || m_stream->m_destroyed) {
                m_stream->m_recv_lock.unlock();
                m_stream->m_read_lock.unlock(this);
                m_locked = false;
                if (m_stream->m_error_code != 0)
                    return CHECK_ERROR(Runtime::setError("Http2Stream: stream reset"));
                return next(CALL_RETURN_NULL);
            }

            // No data available — register as waiter
            m_stream->m_recv_ac = this;
            m_stream->m_recv_retVal = &m_retVal;
            m_stream->m_recv_lock.unlock();
            next(data_ready);
            return CALL_E_PENDDING;
        }

        ON_STATE(AsyncRead, data_ready)
        {
            // Awoken by onData (n=0), onEnd/onClose (n=CALL_RETURN_NULL or error)
            m_stream->m_read_lock.unlock(this);
            m_locked = false;
            if (n == CALL_RETURN_NULL)
                return next(CALL_RETURN_NULL);
            if (n < 0)
                return n;
            return next();
        }

    public:
        obj_ptr<Http2Stream> m_stream;
        obj_ptr<Buffer_base>& m_retVal;
        bool m_locked = false;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncRead(this, retVal, ac))->post(0);
}

result_t Http2Stream::writeBuffer(Buffer_base* data, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_closed || m_destroyed)
        return Runtime::setError("Http2Stream: stream is closed.");

    if (!m_session || !m_session->m_nghttp2)
        return Runtime::setError("Http2Stream: session is destroyed.");

    // Append data to send buffer
    Buffer* buf = Buffer::Cast(data);
    m_send_lock.lock();
    m_send_data.append((const char*)buf->data(), buf->length());
    m_send_lock.unlock();

    // Signal nghttp2 that data is available via resume
    m_session->m_nghttp2.resume_data(m_stream_id);
    m_session->asyncFlushOutput();
    return 0;
}

result_t Http2Stream::flush(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);
    return 0;
}

result_t Http2Stream::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_closed) {
        m_recv_lock.lock();
        m_closed = true;
        m_recv_end = true;
        AsyncEvent* rac = m_recv_ac;
        m_recv_ac = nullptr;
        m_recv_retVal = nullptr;
        m_recv_lock.unlock();
        if (rac)
            rac->apost(CALL_RETURN_NULL);

        // Signal send-side end so data_source_read_callback returns EOF
        m_send_lock.lock();
        m_send_end = true;
        m_send_lock.unlock();

        // Resume nghttp2 data sending and flush pending data
        if (m_session && m_session->m_nghttp2) {
            m_session->m_nghttp2.resume_data(m_stream_id);
            result_t hr = m_session->sendPendingData();
            return hr;
        }
    }

    return 0;
}

void Http2Stream::onData(const uint8_t* data, size_t len)
{
    obj_ptr<Buffer_base> buf = new Buffer(data, len);
    m_recv_lock.lock();
    if (m_recv_ac) {
        // A reader is waiting — deliver directly
        *m_recv_retVal = buf;
        AsyncEvent* ac = m_recv_ac;
        m_recv_ac = nullptr;
        m_recv_retVal = nullptr;
        m_recv_lock.unlock();
        ac->apost(0);
        return;
    }
    m_recv_queue.push_back(buf);
    m_recv_lock.unlock();
}

void Http2Stream::onHeaders(obj_ptr<NObject> headers)
{
    m_headers_lock.lock();
    m_headers = headers;
    AsyncEvent* ac = m_headers_ac;
    m_headers_ac = nullptr;
    m_headers_lock.unlock();

    if (ac)
        ac->apost(0);
}

result_t Http2Stream::waitHeaders(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_headers_lock.lock();

    if (m_headers) {
        m_headers_lock.unlock();
        return 0;
    }

    if (m_closed || m_destroyed || (m_session && (m_session->m_closed || m_session->m_destroyed))) {
        m_headers_lock.unlock();
        return CHECK_ERROR(Runtime::setError("Http2Stream: stream closed before headers received"));
    }

    m_headers_ac = ac;
    m_headers_lock.unlock();
    return CALL_E_PENDDING;
}

void Http2Stream::onClose(uint32_t error_code)
{
    m_recv_lock.lock();
    m_closed = true;
    m_error_code = error_code;
    AsyncEvent* rac = m_recv_ac;
    m_recv_ac = nullptr;
    m_recv_retVal = nullptr;
    m_recv_lock.unlock();
    if (rac) {
        if (error_code != 0)
            rac->apost(CHECK_ERROR(Runtime::setError("Http2Stream: stream reset")));
        else
            rac->apost(CALL_RETURN_NULL);
    }

    m_headers_lock.lock();
    m_closed = true;
    AsyncEvent* hac = m_headers_ac;
    m_headers_ac = nullptr;
    m_headers_lock.unlock();
    if (hac)
        hac->apost(CHECK_ERROR(Runtime::setError("Http2Stream: stream closed before headers received")));

    if (m_session)
        m_session->removeStream(m_stream_id);
}

void Http2Stream::onTrailers(obj_ptr<NObject> headers)
{
    Variant v = headers;
    _emit("trailers", &v, 1);
}

void Http2Stream::onEnd()
{
    m_recv_lock.lock();
    m_recv_end = true;
    AsyncEvent* rac = m_recv_ac;
    m_recv_ac = nullptr;
    m_recv_retVal = nullptr;
    m_recv_lock.unlock();
    if (rac)
        rac->apost(CALL_RETURN_NULL);
}

} /* namespace fibjs */
