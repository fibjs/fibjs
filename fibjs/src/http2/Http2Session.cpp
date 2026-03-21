/*
 * Http2Session.cpp
 *
 *  Created on: Mar 21, 2026
 *      Author: lion
 */

#include "object.h"
#include "Http2Session.h"
#include "Http2Stream.h"
#include "Buffer.h"
#include "Fiber.h"
#include "ifs/TLSSocket.h"
#include <nghttp2/nghttp2.h>

namespace fibjs {

result_t Http2Session::init(Stream_base* conn)
{
    m_conn = conn;

    nghttp2_session_callbacks* callbacks;
    nghttp2_session_callbacks_new(&callbacks);
    setupCallbacks(callbacks);

    int rv;
    if (m_is_server)
        rv = m_nghttp2.init_server(callbacks, this);
    else
        rv = m_nghttp2.init_client(callbacks, this);

    nghttp2_session_callbacks_del(callbacks);

    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Session: session creation failed: ") + nghttp2_strerror(rv));

    // Submit initial SETTINGS frame
    nghttp2_settings_entry iv[] = {
        { NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS, 100 },
        { NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE, 65535 }
    };

    rv = m_nghttp2.submit_settings(iv, sizeof(iv) / sizeof(iv[0]));
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Session: submit settings failed: ") + nghttp2_strerror(rv));

    return 0;
}

void Http2Session::setupCallbacks(nghttp2_session_callbacks* callbacks)
{
    nghttp2_session_callbacks_set_on_begin_headers_callback(callbacks, on_begin_headers_callback);
    nghttp2_session_callbacks_set_on_header_callback(callbacks, on_header_callback);
    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks, on_frame_recv_callback);
    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, on_data_chunk_recv_callback);
    nghttp2_session_callbacks_set_on_stream_close_callback(callbacks, on_stream_close_callback);
}

// -- nghttp2 callbacks --

int Http2Session::on_begin_headers_callback(nghttp2_session* session,
    const nghttp2_frame* frame, void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);

    if (frame->hd.type == NGHTTP2_HEADERS) {
        int32_t stream_id = frame->hd.stream_id;

        if (frame->headers.cat == NGHTTP2_HCAT_REQUEST) {
            // Server-side: create stream for incoming request in JS thread
            obj_ptr<Http2Session> pThis(self);
            self->holder()->sync([pThis, stream_id]() -> int {
                JSFiber::EnterJsScope s;

                obj_ptr<Http2Stream> stream = new Http2Stream(pThis, stream_id);
                stream->wrap();
                pThis->addStream(stream_id, stream);
                return 0;
            });
        }
        // For NGHTTP2_HCAT_RESPONSE, the stream was already created by request()

        // Initialize pending headers
        self->m_header_lock.lock();
        self->m_pending_headers[stream_id].clear();
        self->m_header_lock.unlock();
    }

    return 0;
}

int Http2Session::on_header_callback(nghttp2_session* session,
    const nghttp2_frame* frame, const uint8_t* name, size_t namelen,
    const uint8_t* value, size_t valuelen, uint8_t flags, void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);

    if (frame->hd.type == NGHTTP2_HEADERS) {
        int32_t stream_id = frame->hd.stream_id;

        self->m_header_lock.lock();
        self->m_pending_headers[stream_id].push_back(
            std::make_pair(
                exlib::string((const char*)name, namelen),
                exlib::string((const char*)value, valuelen)));
        self->m_header_lock.unlock();
    }

    return 0;
}

int Http2Session::on_frame_recv_callback(nghttp2_session* session,
    const nghttp2_frame* frame, void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);

    if (frame->hd.type == NGHTTP2_HEADERS) {
        int32_t stream_id = frame->hd.stream_id;
        obj_ptr<Http2Stream> stream = self->getStream(stream_id);
        if (!stream)
            return 0;

        // Build NObject from pending headers (pure C++, no V8)
        obj_ptr<NObject> hdrs = new NObject();
        self->m_header_lock.lock();
        auto it = self->m_pending_headers.find(stream_id);
        if (it != self->m_pending_headers.end()) {
            for (auto& p : it->second)
                hdrs->add(p.first, p.second);
            self->m_pending_headers.erase(it);
        }
        self->m_header_lock.unlock();

        if (frame->headers.cat == NGHTTP2_HCAT_HEADERS) {
            stream->onTrailers(hdrs);
        } else {
            stream->onHeaders(hdrs);
            if (!self->m_internal)
                stream->_emit("headers", hdrs);
        }
    } else if (frame->hd.type == NGHTTP2_GOAWAY) {
        self->m_closed = true;
        self->m_close_event.set();
    } else if (frame->hd.type == NGHTTP2_PING) {
        if (frame->hd.flags & NGHTTP2_FLAG_ACK)
            self->m_close_event.set();
    }

    return 0;
}

int Http2Session::on_data_chunk_recv_callback(nghttp2_session* session,
    uint8_t flags, int32_t stream_id, const uint8_t* data, size_t len,
    void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);

    obj_ptr<Http2Stream> stream = self->getStream(stream_id);
    if (stream)
        stream->onData(data, len);

    return 0;
}

int Http2Session::on_stream_close_callback(nghttp2_session* session,
    int32_t stream_id, uint32_t error_code, void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);

    obj_ptr<Http2Stream> stream = self->getStream(stream_id);
    if (stream)
        stream->onClose(error_code);

    return 0;
}

ssize_t Http2Session::data_source_read_callback(nghttp2_session* session,
    int32_t stream_id, uint8_t* buf, size_t length, uint32_t* data_flags,
    nghttp2_data_source* source, void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);

    obj_ptr<Http2Stream> stream = self->getStream(stream_id);
    if (!stream) {
        *data_flags |= NGHTTP2_DATA_FLAG_EOF;
        return 0;
    }

    stream->m_send_lock.lock();

    size_t avail = stream->m_send_data.length() - stream->m_send_offset;
    if (avail > 0) {
        size_t to_copy = std::min(avail, length);
        memcpy(buf, stream->m_send_data.c_str() + stream->m_send_offset, to_copy);
        stream->m_send_offset += to_copy;

        if (stream->m_send_offset >= stream->m_send_data.length() && stream->m_send_end)
            *data_flags |= NGHTTP2_DATA_FLAG_EOF;

        stream->m_send_lock.unlock();
        return (ssize_t)to_copy;
    }

    if (stream->m_send_end) {
        stream->m_send_lock.unlock();
        *data_flags |= NGHTTP2_DATA_FLAG_EOF;
        return 0;
    }

    stream->m_send_lock.unlock();
    return NGHTTP2_ERR_DEFERRED;
}

// -- stream map management --

obj_ptr<Http2Stream> Http2Session::getStream(int32_t stream_id)
{
    m_stream_lock.lock();
    auto it = m_streams.find(stream_id);
    obj_ptr<Http2Stream> stream = (it != m_streams.end()) ? it->second : nullptr;
    m_stream_lock.unlock();
    return stream;
}

void Http2Session::removeStream(int32_t stream_id)
{
    m_stream_lock.lock();
    m_streams.erase(stream_id);
    m_stream_lock.unlock();
}

void Http2Session::addStream(int32_t stream_id, Http2Stream* stream)
{
    m_stream_lock.lock();
    m_streams[stream_id] = stream;
    m_stream_lock.unlock();
}

// -- I/O loops --

result_t Http2Session::collectPendingData(obj_ptr<Buffer_base>& buf)
{
    exlib::string pending;
    ssize_t len = m_nghttp2.collect_output(pending);

    if (len < 0)
        return Runtime::setError(exlib::string("Http2Session: mem_send failed: ") + nghttp2_strerror((int)len));

    if (!pending.empty())
        buf = new Buffer(pending.c_str(), pending.length());
    else
        buf = nullptr;

    return 0;
}

result_t Http2Session::sendPendingData()
{
    obj_ptr<Buffer_base> buf;
    result_t hr = collectPendingData(buf);
    if (hr < 0)
        return hr;

    if (buf)
        return m_conn->ac_writeBuffer(buf);

    return 0;
}

void Http2Session::startLoops()
{
    // Start the read loop as an async state machine
    class asyncReadLoop : public AsyncState {
    public:
        asyncReadLoop(Http2Session* session)
            : AsyncState(NULL)
            , m_session(session)
        {
            if (!m_session->m_internal)
                m_session->isolate_ref();
            next(read);
        }

    public:
        ON_STATE(asyncReadLoop, read)
        {
            if (m_session->m_destroyed || m_session->m_closed) {
                if (!m_session->m_internal)
                    m_session->isolate_unref();
                return next(CALL_RETURN_NULL);
            }

            return m_session->m_conn->readBuffer(-1, m_buf, next(process));
        }

        ON_STATE(asyncReadLoop, process)
        {
            if (n == CALL_RETURN_NULL || !m_buf || m_session->m_destroyed) {
                m_session->m_closed = true;
                m_session->m_close_event.set();
                if (!m_session->m_internal)
                    m_session->isolate_unref();
                return next(CALL_RETURN_NULL);
            }

            Buffer* buf = Buffer::Cast(m_buf);
            ssize_t rv = m_session->m_nghttp2.recv(buf->data(), buf->length());
            m_buf.Release();

            if (rv < 0) {
                m_session->m_destroyed = true;
                m_session->m_close_event.set();
                if (!m_session->m_internal)
                    m_session->isolate_unref();
                return next(CALL_RETURN_NULL);
            }

            // Collect pending response data for async write
            result_t hr = m_session->collectPendingData(m_write_buf);
            if (hr < 0) {
                m_session->m_destroyed = true;
                m_session->m_close_event.set();
                if (!m_session->m_internal)
                    m_session->isolate_unref();
                return next(CALL_RETURN_NULL);
            }

            if (m_write_buf)
                return m_session->m_conn->writeBuffer(m_write_buf, next(write_done));

            return next(read);
        }

        ON_STATE(asyncReadLoop, write_done)
        {
            m_write_buf.Release();
            return next(read);
        }

        virtual int32_t error(int32_t v)
        {
            m_session->m_destroyed = true;
            m_session->m_close_event.set();
            if (!m_session->m_internal)
                m_session->isolate_unref();
            return next(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<Http2Session> m_session;
        obj_ptr<Buffer_base> m_buf;
        obj_ptr<Buffer_base> m_write_buf;
    };

    (new asyncReadLoop(this))->apost(0);
}

// -- property getters --

static v8::Local<v8::Object> build_settings_object(Isolate* isolate, NgHttp2Handler& handler,
    bool remote)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);

    obj->Set(context, isolate->NewString("headerTableSize"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate,
            handler.get_setting(remote, NGHTTP2_SETTINGS_HEADER_TABLE_SIZE)))
        .IsJust();
    obj->Set(context, isolate->NewString("enablePush"),
        v8::Boolean::New(isolate->m_isolate,
            handler.get_setting(remote, NGHTTP2_SETTINGS_ENABLE_PUSH) != 0))
        .IsJust();
    obj->Set(context, isolate->NewString("maxConcurrentStreams"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate,
            handler.get_setting(remote, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS)))
        .IsJust();
    obj->Set(context, isolate->NewString("initialWindowSize"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate,
            handler.get_setting(remote, NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE)))
        .IsJust();
    obj->Set(context, isolate->NewString("maxFrameSize"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate,
            handler.get_setting(remote, NGHTTP2_SETTINGS_MAX_FRAME_SIZE)))
        .IsJust();
    obj->Set(context, isolate->NewString("maxHeaderListSize"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate,
            handler.get_setting(remote, NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE)))
        .IsJust();

    return obj;
}

result_t Http2Session::get_remoteSettings(v8::Local<v8::Object>& retVal)
{
    if (!m_nghttp2)
        return Runtime::setError("Http2Session: session is destroyed.");

    retVal = build_settings_object(holder(), m_nghttp2, true);
    return 0;
}

result_t Http2Session::get_localSettings(v8::Local<v8::Object>& retVal)
{
    if (!m_nghttp2)
        return Runtime::setError("Http2Session: session is destroyed.");

    retVal = build_settings_object(holder(), m_nghttp2, false);
    return 0;
}

result_t Http2Session::get_destroyed(bool& retVal)
{
    retVal = m_destroyed;
    return 0;
}

result_t Http2Session::get_closed(bool& retVal)
{
    retVal = m_closed;
    return 0;
}

result_t Http2Session::get_alpnProtocol(exlib::string& retVal)
{
    obj_ptr<TLSSocket_base> tls = TLSSocket_base::getInstance(m_conn);
    if (tls)
        return tls->get_alpnProtocol(retVal);

    retVal = "h2c";
    return 0;
}

result_t Http2Session::get_socket(obj_ptr<Stream_base>& retVal)
{
    retVal = m_conn;
    return 0;
}

// -- methods --

result_t Http2Session::request(v8::Local<v8::Object> headers,
    v8::Local<v8::Object> options, obj_ptr<Http2Stream_base>& retVal)
{
    if (m_is_server)
        return Runtime::setError("Http2Session: request() is only available on client sessions.");

    if (m_destroyed || m_closed)
        return Runtime::setError("Http2Session: session is closed.");

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Array> keys = headers->GetOwnPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
    uint32_t nheaders = keys.IsEmpty() ? 0 : keys->Length();

    // Separate pseudo-headers from regular headers (pseudo must come first per RFC 7540)
    std::vector<exlib::string> pseudo_names, pseudo_values;
    std::vector<exlib::string> regular_names, regular_values;

    bool has_scheme = false, has_authority = false, has_method = false, has_path = false;
    exlib::string method;

    for (uint32_t i = 0; i < nheaders; i++) {
        v8::Local<v8::Value> key = keys->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        v8::Local<v8::Value> val = headers->Get(context, key).FromMaybe(v8::Local<v8::Value>());

        v8::String::Utf8Value key_str(isolate->m_isolate, key);
        v8::String::Utf8Value val_str(isolate->m_isolate, val);

        exlib::string k(*key_str, key_str.length());
        exlib::string v(*val_str, val_str.length());

        if (k == ":scheme")
            has_scheme = true;
        else if (k == ":authority")
            has_authority = true;
        else if (k == ":method") {
            has_method = true;
            method = v;
        } else if (k == ":path")
            has_path = true;

        if (k.length() > 0 && k[0] == ':') {
            pseudo_names.push_back(k);
            pseudo_values.push_back(v);
        } else {
            regular_names.push_back(k);
            regular_values.push_back(v);
        }
    }

    // Auto-fill missing pseudo-headers
    if (!has_method) {
        pseudo_names.push_back(":method");
        pseudo_values.push_back("GET");
        method = "GET";
    }
    if (!has_path) {
        pseudo_names.push_back(":path");
        pseudo_values.push_back("/");
    }
    if (!has_scheme) {
        pseudo_names.push_back(":scheme");
        pseudo_values.push_back(m_scheme);
    }
    if (!has_authority) {
        pseudo_names.push_back(":authority");
        pseudo_values.push_back(m_authority);
    }

    // Merge: pseudo-headers first, then regular headers
    std::vector<exlib::string> name_bufs;
    std::vector<exlib::string> value_bufs;
    name_bufs.reserve(pseudo_names.size() + regular_names.size());
    value_bufs.reserve(pseudo_values.size() + regular_values.size());

    for (size_t i = 0; i < pseudo_names.size(); i++) {
        name_bufs.push_back(pseudo_names[i]);
        value_bufs.push_back(pseudo_values[i]);
    }
    for (size_t i = 0; i < regular_names.size(); i++) {
        name_bufs.push_back(regular_names[i]);
        value_bufs.push_back(regular_values[i]);
    }

    // Build nghttp2_nv array
    std::vector<nghttp2_nv> nva(name_bufs.size());
    for (size_t i = 0; i < name_bufs.size(); i++) {
        nva[i] = {
            (uint8_t*)name_bufs[i].c_str(), (uint8_t*)value_bufs[i].c_str(),
            name_bufs[i].length(), value_bufs[i].length(),
            NGHTTP2_NV_FLAG_NO_COPY_NAME | NGHTTP2_NV_FLAG_NO_COPY_VALUE
        };
    }

    // Check endStream option; default true for GET/HEAD
    bool endStream = (method == "GET" || method == "HEAD");
    GetConfigValue(options, "endStream", endStream, true);

    nghttp2_data_provider data_prd;
    data_prd.source.ptr = nullptr;
    data_prd.read_callback = data_source_read_callback;

    int32_t stream_id = m_nghttp2.submit_request(nva.data(), nva.size(),
        endStream ? nullptr : &data_prd);

    if (stream_id < 0)
        return Runtime::setError(exlib::string("Http2Session: submit request failed: ") + nghttp2_strerror(stream_id));

    obj_ptr<Http2Stream> stream = new Http2Stream(this, stream_id);
    stream->wrap();
    addStream(stream_id, stream);

    result_t hr = sendPendingData();
    if (hr < 0)
        return hr;

    retVal = stream;
    return 0;
}

result_t Http2Session::request(const std::vector<std::pair<exlib::string, exlib::string>>& headers,
    bool endStream, obj_ptr<Http2Stream>& retVal)
{
    if (m_is_server)
        return Runtime::setError("Http2Session: request() is only available on client sessions.");

    if (m_destroyed || m_closed)
        return Runtime::setError("Http2Session: session is closed.");

    // Build nghttp2_nv array from headers
    std::vector<nghttp2_nv> nva(headers.size());
    for (size_t i = 0; i < headers.size(); i++) {
        nva[i] = {
            (uint8_t*)headers[i].first.c_str(), (uint8_t*)headers[i].second.c_str(),
            headers[i].first.length(), headers[i].second.length(),
            NGHTTP2_NV_FLAG_NO_COPY_NAME | NGHTTP2_NV_FLAG_NO_COPY_VALUE
        };
    }

    nghttp2_data_provider data_prd;
    data_prd.source.ptr = nullptr;
    data_prd.read_callback = data_source_read_callback;

    int32_t stream_id = m_nghttp2.submit_request(nva.data(), nva.size(),
        endStream ? nullptr : &data_prd);

    if (stream_id < 0)
        return Runtime::setError(exlib::string("Http2Session: submit request failed: ") + nghttp2_strerror(stream_id));

    obj_ptr<Http2Stream> stream = new Http2Stream(this, stream_id);
    addStream(stream_id, stream);

    retVal = stream;
    return 0;
}

result_t Http2Session::goaway(int32_t code, int32_t lastStreamId)
{
    if (m_destroyed || !m_nghttp2)
        return Runtime::setError("Http2Session: session is destroyed.");

    int rv = m_nghttp2.submit_goaway(lastStreamId, code);
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Session: submit goaway failed: ") + nghttp2_strerror(rv));

    return sendPendingData();
}

result_t Http2Session::ping(int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_destroyed || !m_nghttp2)
        return Runtime::setError("Http2Session: session is destroyed.");

    int rv = m_nghttp2.submit_ping();
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Session: submit ping failed: ") + nghttp2_strerror(rv));

    obj_ptr<Buffer_base> buf;
    result_t hr = collectPendingData(buf);
    if (hr < 0)
        return hr;

    retVal = 0;

    if (buf)
        return m_conn->writeBuffer(buf, ac);

    return 0;
}

result_t Http2Session::settings(v8::Local<v8::Object> settings)
{
    if (m_destroyed || !m_nghttp2)
        return Runtime::setError("Http2Session: session is destroyed.");

    Isolate* isolate = holder();
    std::vector<nghttp2_settings_entry> iv;

    int32_t val;
    if (GetConfigValue(settings, "headerTableSize", val, true) == 0)
        iv.push_back({ NGHTTP2_SETTINGS_HEADER_TABLE_SIZE, (uint32_t)val });
    if (GetConfigValue(settings, "enablePush", val, true) == 0)
        iv.push_back({ NGHTTP2_SETTINGS_ENABLE_PUSH, (uint32_t)val });
    if (GetConfigValue(settings, "maxConcurrentStreams", val, true) == 0)
        iv.push_back({ NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS, (uint32_t)val });
    if (GetConfigValue(settings, "initialWindowSize", val, true) == 0)
        iv.push_back({ NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE, (uint32_t)val });
    if (GetConfigValue(settings, "maxFrameSize", val, true) == 0)
        iv.push_back({ NGHTTP2_SETTINGS_MAX_FRAME_SIZE, (uint32_t)val });
    if (GetConfigValue(settings, "maxHeaderListSize", val, true) == 0)
        iv.push_back({ NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE, (uint32_t)val });

    int rv = m_nghttp2.submit_settings(iv.data(), iv.size());
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Session: submit settings failed: ") + nghttp2_strerror(rv));

    return sendPendingData();
}

result_t Http2Session::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_closed)
        return 0;

    class asyncClose : public AsyncState {
    public:
        asyncClose(Http2Session* session, AsyncEvent* ac)
            : AsyncState(ac)
            , m_session(session)
        {
            if (m_session->m_nghttp2) {
                m_session->m_nghttp2.submit_goaway(0, 0);
                m_session->collectPendingData(m_buf);
            }

            m_session->m_closed = true;
            m_session->m_close_event.set();

            next(m_buf ? write_goaway : close_conn);
        }

        ON_STATE(asyncClose, write_goaway)
        {
            return m_session->m_conn->writeBuffer(m_buf, next(close_conn));
        }

        ON_STATE(asyncClose, close_conn)
        {
            m_buf.Release();
            return m_session->m_conn->close(next());
        }

    private:
        obj_ptr<Http2Session> m_session;
        obj_ptr<Buffer_base> m_buf;
    };

    return (new asyncClose(this, ac))->post(0);
}

result_t Http2Session::destroy()
{
    if (m_destroyed)
        return 0;

    m_destroyed = true;
    m_closed = true;

    // Close all streams
    m_stream_lock.lock();
    for (auto& pair : m_streams) {
        pair.second->m_closed = true;
        pair.second->m_recv_end = true;
        pair.second->m_recv_event.set();
    }
    m_streams.clear();
    m_stream_lock.unlock();

    m_nghttp2.del();

    m_close_event.set();

    // Close underlying connection asynchronously so readLoop unblocks
    if (m_conn) {
        class asyncCloseConn : public AsyncState {
        public:
            asyncCloseConn(Stream_base* conn)
                : AsyncState(NULL)
                , m_conn(conn)
            {
                next(do_close);
            }

            ON_STATE(asyncCloseConn, do_close)
            {
                return m_conn->close(next());
            }

        private:
            obj_ptr<Stream_base> m_conn;
        };

        (new asyncCloseConn(m_conn))->apost(0);
    }

    return 0;
}

} /* namespace fibjs */
