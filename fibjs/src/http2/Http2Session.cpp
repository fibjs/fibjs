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
#include "TLSSocket.h"
#include "ifs/Socket.h"
#include <nghttp2/nghttp2.h>
#include <stdlib.h>


namespace fibjs {

static bool h2_trace_enabled()
{
    static int s_enabled = -1;
    if (s_enabled == -1) {
        const char* p = getenv("FIBJS_H2_TRACE");
        s_enabled = (p && *p && *p != '0') ? 1 : 0;
    }
    return s_enabled == 1;
}

#define H2_TRACE(fmt, ...) \
    do { \
        if (h2_trace_enabled()) \
            fprintf(stderr, "[H2TRACE][Http2Session:%p] " fmt "\n", (void*)this, ##__VA_ARGS__); \
    } while (0)

static void abort_transport(Stream_base* conn)
{
    Socket_base* sock = Socket_base::getInstance(conn);
    if (sock) {
        sock->abort();
        return;
    }

    TLSSocket* tls = (TLSSocket*)TLSSocket_base::getInstance(conn);
    if (tls && tls->m_stream) {
        sock = Socket_base::getInstance(tls->m_stream);
        if (sock)
            sock->abort();
    }
}

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
        { NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE, 1024 * 1024 }
    };

    rv = m_nghttp2.submit_settings(iv, sizeof(iv) / sizeof(iv[0]));
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Session: submit settings failed: ") + nghttp2_strerror(rv));

    // Increase the connection-level flow control window (stream 0).
    // SETTINGS_INITIAL_WINDOW_SIZE only affects per-stream windows;
    // the connection window defaults to 65535 and must be enlarged
    // separately via WINDOW_UPDATE.
    if (!m_is_server) {
        rv = m_nghttp2.submit_window_update(0, 16 * 1024 * 1024 - 65535);
        if (rv != 0)
            return Runtime::setError(exlib::string("Http2Session: submit window_update failed: ") + nghttp2_strerror(rv));
    }

    return 0;
}

result_t Http2Session::onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
{
    (void)type;
    (void)func;

    if (m_internal || ev != "stream")
        return 0;

    int32_t count = 0;
    listenerCount("stream", count);

    H2_TRACE("listener_holder onEventChange type=%s ev=%s stream_listeners=%d holder=%p",
        type.c_str(), ev.c_str(), count, (void*)m_listener_holder.get());

    // EventEmitter triggers "newListener" before insertion, so count is old value.
    if (type == "newListener") {
        if (m_listener_holder == NULL) {
            H2_TRACE("listener_holder create reason=newListener(stream) begin");
            m_listener_holder = new ValueHolder(wrap());
            H2_TRACE("listener_holder create done holder=%p", (void*)m_listener_holder.get());
        }
    } else if (type == "removeListener") {
        // "removeListener" is triggered after removal; count is remaining listeners.
        if (count == 0 && m_listener_holder != NULL) {
            H2_TRACE("listener_holder release reason=removeListener(stream)_count0 holder=%p", (void*)m_listener_holder.get());
            m_listener_holder.Release();
        }
    }

    return 0;
}

static const char* frame_type_name(uint8_t type)
{
    switch (type) {
    case NGHTTP2_DATA: return "DATA";
    case NGHTTP2_HEADERS: return "HEADERS";
    case NGHTTP2_PRIORITY: return "PRIORITY";
    case NGHTTP2_RST_STREAM: return "RST_STREAM";
    case NGHTTP2_SETTINGS: return "SETTINGS";
    case NGHTTP2_PUSH_PROMISE: return "PUSH_PROMISE";
    case NGHTTP2_PING: return "PING";
    case NGHTTP2_GOAWAY: return "GOAWAY";
    case NGHTTP2_WINDOW_UPDATE: return "WINDOW_UPDATE";
    case NGHTTP2_CONTINUATION: return "CONTINUATION";
    default: return "UNKNOWN";
    }
}

static int on_frame_send_callback(nghttp2_session* session,
    const nghttp2_frame* frame, void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);
    if (frame->hd.type == NGHTTP2_GOAWAY) {
    } else if (frame->hd.type == NGHTTP2_RST_STREAM) {
    }
    return 0;
}

static int on_invalid_frame_recv_callback(nghttp2_session* session,
    const nghttp2_frame* frame, int lib_error_code, void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);
    return 0;
}

void Http2Session::setupCallbacks(nghttp2_session_callbacks* callbacks)
{
    nghttp2_session_callbacks_set_on_begin_headers_callback(callbacks, on_begin_headers_callback);
    nghttp2_session_callbacks_set_on_header_callback(callbacks, on_header_callback);
    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks, on_frame_recv_callback);
    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, on_data_chunk_recv_callback);
    nghttp2_session_callbacks_set_on_stream_close_callback(callbacks, on_stream_close_callback);
    nghttp2_session_callbacks_set_on_frame_send_callback(callbacks, on_frame_send_callback);
    nghttp2_session_callbacks_set_on_invalid_frame_recv_callback(callbacks, on_invalid_frame_recv_callback);
}

// -- nghttp2 callbacks --

int Http2Session::on_begin_headers_callback(nghttp2_session* session,
    const nghttp2_frame* frame, void* user_data)
{
    Http2Session* self = static_cast<Http2Session*>(user_data);

    if (frame->hd.type == NGHTTP2_HEADERS) {
        int32_t stream_id = frame->hd.stream_id;

        if (frame->headers.cat == NGHTTP2_HCAT_REQUEST) {
            // Server-side: create stream for incoming request.
            // Do not use sync() here because it's non-blocking and the stream
            // must exist before on_frame_recv_callback calls findStream().
            obj_ptr<Http2Stream> stream = new Http2Stream(self, stream_id);
            stream->holder(self->get_holder());
            self->addStream(stream_id, stream);
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

    if (h2_trace_enabled())
        fprintf(stderr, "[H2TRACE][Http2Session:%p] frame_recv type=%s sid=%d flags=0x%x\n",
            (void*)self, frame_type_name(frame->hd.type), frame->hd.stream_id, frame->hd.flags);

    if (frame->hd.type == NGHTTP2_GOAWAY) {
    } else if (frame->hd.type == NGHTTP2_RST_STREAM) {
    }

    if (frame->hd.type == NGHTTP2_HEADERS) {
        int32_t stream_id = frame->hd.stream_id;
        obj_ptr<Http2Stream> stream = self->getStream(stream_id);
        if (!stream) {
            if (h2_trace_enabled())
                fprintf(stderr, "[H2TRACE][Http2Session:%p] frame_recv HEADERS sid=%d but stream not found\n",
                    (void*)self, stream_id);
            return 0;
        }

        if (h2_trace_enabled())
            fprintf(stderr, "[H2TRACE][Http2Session:%p] headers sid=%d cat=%d is_server=%d\n",
                (void*)self, stream_id, (int)frame->headers.cat, (int)self->m_is_server);

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
            if (!self->m_internal) {
                self->enqueueHeaderEvent(stream, hdrs);
                if (self->m_is_server && frame->headers.cat == NGHTTP2_HCAT_REQUEST) {
                    if (h2_trace_enabled())
                        fprintf(stderr, "[H2TRACE][Http2Session:%p] emit stream sid=%d\n", (void*)self, stream_id);
                    self->enqueueStreamEvent(stream, hdrs);
                } else if (h2_trace_enabled() && self->m_is_server) {
                    fprintf(stderr, "[H2TRACE][Http2Session:%p] skip stream emit sid=%d cat=%d\n",
                        (void*)self, stream_id, (int)frame->headers.cat);
                }
            }
        }
    } else if (frame->hd.type == NGHTTP2_SETTINGS) {
        // Signal that the remote SETTINGS have been received so that
        // nghttp2 now knows the peer's MAX_CONCURRENT_STREAMS limit.
        if (!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
            self->m_remote_settings_ready.store(true, std::memory_order_release);
            self->m_remote_settings_event.set();
        }
    } else if (frame->hd.type == NGHTTP2_GOAWAY) {
        self->m_closed = true;
        self->m_close_event.set();
    } else if (frame->hd.type == NGHTTP2_PING) {
        if (frame->hd.flags & NGHTTP2_FLAG_ACK)
            self->m_close_event.set();
    }

    // END_STREAM signals no more data from the remote side
    if ((frame->hd.type == NGHTTP2_HEADERS || frame->hd.type == NGHTTP2_DATA)
        && (frame->hd.flags & NGHTTP2_FLAG_END_STREAM)) {
        obj_ptr<Http2Stream> stream = self->getStream(frame->hd.stream_id);
        if (stream)
            stream->onEnd();
    }

    return 0;
}

void Http2Session::enqueueHeaderEvent(Http2Stream* stream, NObject* headers)
{
    m_pending_event_lock.lock();
    m_pending_header_events.push_back({ stream, headers });
    m_pending_event_lock.unlock();
}

void Http2Session::enqueueStreamEvent(Http2Stream* stream, NObject* headers)
{
    m_pending_event_lock.lock();
    m_pending_stream_events.push_back({ stream, headers });
    m_pending_event_lock.unlock();
}

void Http2Session::flushPendingEvents()
{
    std::list<std::pair<obj_ptr<Http2Stream>, obj_ptr<NObject>>> header_events;
    std::list<std::pair<obj_ptr<Http2Stream>, obj_ptr<NObject>>> stream_events;

    m_pending_event_lock.lock();
    if (!m_pending_header_events.empty())
        header_events.swap(m_pending_header_events);
    if (!m_pending_stream_events.empty())
        stream_events.swap(m_pending_stream_events);
    m_pending_event_lock.unlock();

    for (auto& ev : header_events)
        ev.first->_emit("headers", ev.second);

    for (auto& ev : stream_events) {
        Variant args[2];
        args[0] = ev.first;
        args[1] = ev.second;
        _emit("stream", args, 2);
    }
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

    if (h2_trace_enabled())
        fprintf(stderr, "[H2TRACE][Http2Session:%p] stream_close sid=%d err=%u\n",
            (void*)self, stream_id, error_code);


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

        bool eof = (stream->m_send_offset >= stream->m_send_data.length() && stream->m_send_end);
        if (eof)
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
    H2_TRACE("removeStream sid=%d", stream_id);
    m_stream_lock.lock();
    m_streams.erase(stream_id);
    m_stream_lock.unlock();
}

void Http2Session::addStream(int32_t stream_id, Http2Stream* stream)
{
    H2_TRACE("addStream sid=%d closed=%d destroyed=%d", stream_id, (int)m_closed, (int)m_destroyed);
    m_stream_lock.lock();
    m_streams[stream_id] = stream;
    // If the session is already closed/destroyed, close the new stream
    // immediately to prevent hangs in waitHeaders/readBuffer.
    if (m_closed || m_destroyed) {
        stream->m_recv_lock.lock();
        stream->m_closed = true;
        AsyncEvent* rac = stream->m_recv_ac;
        stream->m_recv_ac = nullptr;
        stream->m_recv_retVal = nullptr;
        stream->m_recv_lock.unlock();
        if (rac)
            rac->apost(CALL_RETURN_NULL);

        stream->m_headers_lock.lock();
        AsyncEvent* hac = stream->m_headers_ac;
        stream->m_headers_ac = nullptr;
        stream->m_headers_lock.unlock();
        if (hac)
            hac->apost(CALL_E_INTERNAL);
    }
    m_stream_lock.unlock();
}

void Http2Session::closeAllStreams()
{
    // Signal settings event so fibers blocked in h2_wait_settings
    // are unblocked when the session dies before SETTINGS exchange.
    m_remote_settings_event.set();

    std::vector<obj_ptr<Http2Stream>> streams;

    m_stream_lock.lock();
    streams.reserve(m_streams.size());
    for (auto& pair : m_streams)
        streams.push_back(pair.second);
    m_stream_lock.unlock();

    for (auto& stream : streams) {
        stream->m_destroyed = true;
        stream->onClose(0);
    }

    H2_TRACE("closeAllStreams done streams=%zu", streams.size());
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
    if (m_destroyed)
        return Runtime::setError("Http2Session: session is destroyed.");

    obj_ptr<Buffer_base> buf;
    result_t hr = collectPendingData(buf);
    if (hr < 0)
        return hr;
    if (!buf) {
        return 0;
    }

    AsyncFlushItem item(buf, true);
    enqueueFlush(&item);
    item.m_event.wait();
    return item.m_result;
}

void Http2Session::asyncFlushOutput()
{
    if (m_destroyed) {
        return;
    }

    obj_ptr<Buffer_base> buf;
    result_t hr = collectPendingData(buf);
    if (hr < 0 || !buf) {
        if (hr < 0)
            H2_TRACE("asyncFlushOutput collect error hr=%d", hr);
        return;
    }

    H2_TRACE("asyncFlushOutput enqueue bytes=%d", Buffer::Cast(buf)->length());
    enqueueFlush(new AsyncFlushItem(buf));
}

void Http2Session::enqueueFlush(AsyncFlushItem* item)
{
    class asyncWriter : public AsyncState {
    public:
        asyncWriter(Http2Session* session)
            : AsyncState(NULL)
            , m_session(session)
        {
            next(do_write);
        }

    public:
        ON_STATE(asyncWriter, do_write)
        {
            m_session->m_write_spinlock.lock();
            AsyncFlushItem* head = m_session->m_write_queue.head();
            m_session->m_write_spinlock.unlock();

            if (h2_trace_enabled() && head && head->m_buf)
                fprintf(stderr, "[H2TRACE][Http2Session:%p] writer do_write bytes=%d blocking=%d\n",
                    (void*)m_session.get(), Buffer::Cast(head->m_buf)->length(), (int)head->m_blocking);

            return m_session->m_conn->writeBuffer(head->m_buf, next(write_done));
        }

        ON_STATE(asyncWriter, write_done)
        {
            m_session->m_write_spinlock.lock();
            AsyncFlushItem* item = m_session->m_write_queue.getHead();
            bool has_more = !m_session->m_write_queue.empty();
            if (!has_more)
                m_session->m_writer_active = false;
            m_session->m_write_spinlock.unlock();

            if (h2_trace_enabled() && item && item->m_buf)
                fprintf(stderr, "[H2TRACE][Http2Session:%p] writer write_done bytes=%d blocking=%d has_more=%d\n",
                    (void*)m_session.get(), Buffer::Cast(item->m_buf)->length(), (int)item->m_blocking, (int)has_more);

            if (item->m_blocking) {
                item->m_result = 0;
                item->m_event.set();
            } else {
                delete item;
            }

            if (has_more)
                return next(do_write);
            return next(CALL_RETURN_NULL);
        }

        virtual int32_t error(int32_t v)
        {
            if (h2_trace_enabled())
                fprintf(stderr, "[H2TRACE][Http2Session:%p] writer error v=%d\n", (void*)m_session.get(), v);

            // Drain entire queue on error
            m_session->m_write_spinlock.lock();
            while (m_session->m_write_queue.count()) {
                AsyncFlushItem* item = m_session->m_write_queue.getHead();
                m_session->m_write_spinlock.unlock();
                if (item->m_blocking) {
                    item->m_result = v;
                    item->m_event.set();
                } else {
                    delete item;
                }
                m_session->m_write_spinlock.lock();
            }
            m_session->m_writer_active = false;
            m_session->m_write_spinlock.unlock();

            // Write-path failures (e.g. broken pipe) make the session unusable.
            // Wake all pending stream waiters to prevent hangs in waitHeaders/read.
            m_session->m_closed = true;
            m_session->m_destroyed = true;
            m_session->m_close_event.set();
            m_session->closeAllStreams();
            H2_TRACE("listener_holder release reason=writer_error holder=%p", (void*)m_session->m_listener_holder.get());
            m_session->m_listener_holder.Release();
            abort_transport(m_session->m_conn);

            return next(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<Http2Session> m_session;
    };

    m_write_spinlock.lock();
    m_write_queue.putTail(item);
    bool should_start = false;
    if (!m_writer_active) {
        m_writer_active = true;
        should_start = true;
    }
    size_t qlen = m_write_queue.count();
    m_write_spinlock.unlock();

    H2_TRACE("enqueueFlush blocking=%d start=%d qlen=%zu", (int)item->m_blocking, (int)should_start, qlen);

    if (should_start)
        (new asyncWriter(this))->apost(0);
}

void Http2Session::startLoops()
{
    // Flush any pending data (e.g. server initial SETTINGS) before reading
    asyncFlushOutput();

    // Start the read loop as an async state machine
    class asyncReadLoop : public AsyncState {
    public:
        asyncReadLoop(Http2Session* session)
            : AsyncState(NULL)
            , m_session(session)
        {
            if (!m_session->m_internal) {
                m_session->isolate_ref();
                m_session->m_ref_active = true;
            }
            next(read);
        }

        void releaseRef()
        {
            if (m_session->m_ref_active) {
                m_session->m_ref_active = false;
                m_session->isolate_unref();
            }
        }

    public:
        ON_STATE(asyncReadLoop, read)
        {
            if (m_session->m_destroyed) {
                m_session->closeAllStreams();
                H2_TRACE("listener_holder release reason=readloop_destroyed holder=%p", (void*)m_session->m_listener_holder.get());
                m_session->m_listener_holder.Release();
                releaseRef();
                m_session->signalDone();
                return next(CALL_RETURN_NULL);
            }

            // After GOAWAY (m_closed=true), continue reading only while
            // there are active streams that may still receive data.
            if (m_session->m_closed) {
                m_session->m_stream_lock.lock();
                bool has_streams = !m_session->m_streams.empty();
                m_session->m_stream_lock.unlock();
                if (!has_streams) {
                    m_session->closeAllStreams();
                    H2_TRACE("listener_holder release reason=readloop_closed_no_streams holder=%p", (void*)m_session->m_listener_holder.get());
                    m_session->m_listener_holder.Release();
                    releaseRef();
                    m_session->signalDone();
                    return next(CALL_RETURN_NULL);
                }
            }

            // Drain buffered input first. If nghttp2 previously consumed only
            // part of the bytes, blocking on socket read here can stall forever.
            if (!m_pending_input.empty()) {
                if (h2_trace_enabled())
                    fprintf(stderr, "[H2TRACE][Http2Session:%p] readLoop use pending bytes=%zu\n",
                        (void*)m_session.get(), m_pending_input.length());
                m_buf.Release();
                return next(process);
            }

            if (h2_trace_enabled()) {
                m_session->m_stream_lock.lock();
                size_t nstreams = m_session->m_streams.size();
                m_session->m_stream_lock.unlock();
                fprintf(stderr, "[H2TRACE][Http2Session:%p] readLoop read begin closed=%d destroyed=%d streams=%zu\n",
                    (void*)m_session.get(), (int)m_session->m_closed, (int)m_session->m_destroyed, nstreams);
            }

            return m_session->m_conn->readBuffer(-1, m_buf, next(process));
        }

        ON_STATE(asyncReadLoop, process)
        {
            if (h2_trace_enabled() && m_buf)
                fprintf(stderr, "[H2TRACE][Http2Session:%p] readLoop process n=%d bytes=%d\n",
                    (void*)m_session.get(), n, Buffer::Cast(m_buf)->length());

            if (n == CALL_RETURN_NULL || m_session->m_destroyed || (!m_buf && m_pending_input.empty())) {
                if (h2_trace_enabled())
                    fprintf(stderr, "[H2TRACE][Http2Session:%p] readLoop process exit n=%d has_buf=%d destroyed=%d\n",
                        (void*)m_session.get(), n, m_buf ? 1 : 0, (int)m_session->m_destroyed);
                m_session->m_closed = true;
                m_session->m_close_event.set();
                m_session->closeAllStreams();
                H2_TRACE("listener_holder release reason=readloop_eof_or_destroyed holder=%p", (void*)m_session->m_listener_holder.get());
                m_session->m_listener_holder.Release();
                releaseRef();
                m_session->signalDone();
                return next(CALL_RETURN_NULL);
            }

            Buffer* buf = m_buf ? Buffer::Cast(m_buf) : nullptr;

            if (buf)
                m_pending_input.append((const char*)buf->data(), buf->length());

            const uint8_t* input_data = !m_pending_input.empty()
                ? (const uint8_t*)m_pending_input.c_str()
                : (buf ? buf->data() : nullptr);
            size_t input_len = !m_pending_input.empty()
                ? m_pending_input.length()
                : (buf ? buf->length() : 0);

            ssize_t rv = m_session->m_nghttp2.recv(input_data, input_len);

            if (rv < 0) {
                m_buf.Release();
                if (h2_trace_enabled())
                    fprintf(stderr, "[H2TRACE][Http2Session:%p] readLoop recv error rv=%zd\n",
                        (void*)m_session.get(), rv);
                m_session->m_destroyed = true;
                m_session->m_close_event.set();
                m_session->closeAllStreams();
                H2_TRACE("listener_holder release reason=readloop_recv_error holder=%p", (void*)m_session->m_listener_holder.get());
                m_session->m_listener_holder.Release();
                releaseRef();
                m_session->signalDone();
                return next(CALL_RETURN_NULL);
            }

            if ((size_t)rv < input_len) {
                m_pending_input.assign((const char*)input_data + rv, input_len - rv);
                if (h2_trace_enabled())
                    fprintf(stderr, "[H2TRACE][Http2Session:%p] readLoop recv partial rv=%zd input=%zu pending=%zu\n",
                        (void*)m_session.get(), rv, input_len, m_pending_input.length());
            } else
                m_pending_input.resize(0);

            m_buf.Release();

            // Emit queued JS events only after recv() returns and nghttp2 lock
            // is released, so event handlers can safely call submit_* APIs.
            m_session->flushPendingEvents();

            m_session->asyncFlushOutput();

            if (!m_pending_input.empty())
                return next(process);

            return next(read);
        }

        virtual int32_t error(int32_t v)
        {
            m_session->m_destroyed = true;
            m_session->m_close_event.set();
            m_session->closeAllStreams();
            H2_TRACE("listener_holder release reason=readloop_state_error holder=%p", (void*)m_session->m_listener_holder.get());
            m_session->m_listener_holder.Release();
            releaseRef();
            m_session->signalDone();
            return next(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<Http2Session> m_session;
        obj_ptr<Buffer_base> m_buf;
        exlib::string m_pending_input;
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

    exlib::string pending;
    int32_t stream_id;

    if (endStream) {
        // No body: atomically submit + collect output to prevent
        // readLoop's asyncFlushOutput from stealing HEADERS data.
        stream_id = m_nghttp2.submit_request_and_collect(nva.data(), nva.size(),
            nullptr, pending);
    } else {
        stream_id = m_nghttp2.submit_request(nva.data(), nva.size(), &data_prd);
    }

    if (stream_id < 0)
        return Runtime::setError(exlib::string("Http2Session: submit request failed: ") + nghttp2_strerror(stream_id));

    obj_ptr<Http2Stream> stream = new Http2Stream(this, stream_id);
    stream->wrap();
    addStream(stream_id, stream);

    // Flush atomically collected output, or do async flush for non-endStream
    if (!pending.empty()) {
        obj_ptr<Buffer_base> buf = new Buffer(pending.c_str(), pending.length());
        enqueueFlush(new AsyncFlushItem(buf));
    } else if (!endStream) {
        asyncFlushOutput();
    }

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

    exlib::string pending;
    int32_t stream_id;

    if (endStream) {
        // No body: atomically submit + collect output to prevent
        // readLoop's asyncFlushOutput from stealing HEADERS data.
        stream_id = m_nghttp2.submit_request_and_collect(nva.data(), nva.size(),
            nullptr, pending);
    } else {
        stream_id = m_nghttp2.submit_request(nva.data(), nva.size(), &data_prd);
    }

    if (stream_id < 0)
        return Runtime::setError(exlib::string("Http2Session: submit request failed: ") + nghttp2_strerror(stream_id));

    obj_ptr<Http2Stream> stream = new Http2Stream(this, stream_id);
    addStream(stream_id, stream);


    retVal = stream;

    // Flush atomically collected output (or nothing for non-endStream)
    if (!pending.empty()) {
        obj_ptr<Buffer_base> buf = new Buffer(pending.c_str(), pending.length());
        enqueueFlush(new AsyncFlushItem(buf));
    }

    return 0;
}

result_t Http2Session::goaway(int32_t code, int32_t lastStreamId)
{
    if (m_destroyed || !m_nghttp2)
        return Runtime::setError("Http2Session: session is destroyed.");

    int rv = m_nghttp2.submit_goaway(lastStreamId, code);
    if (rv != 0)
        return Runtime::setError(exlib::string("Http2Session: submit goaway failed: ") + nghttp2_strerror(rv));

    asyncFlushOutput();
    return 0;
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

    asyncFlushOutput();
    return 0;
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
            H2_TRACE("listener_holder release reason=close holder=%p", (void*)m_session->m_listener_holder.get());
            m_session->m_listener_holder.Release();

            if (m_session->m_ref_active) {
                m_session->m_ref_active = false;
                m_session->isolate_unref();
            }

            next(m_buf ? write_goaway : close_conn);
        }

        ON_STATE(asyncClose, write_goaway)
        {
            return m_session->m_conn->writeBuffer(m_buf, next(close_conn));
        }

        ON_STATE(asyncClose, close_conn)
        {
            m_buf.Release();
            return m_session->m_conn->close(next(abort_socket));
        }

        ON_STATE(asyncClose, abort_socket)
        {
            // Abort underlying TCP socket so readLoop unblocks immediately.
            // TLSSocket::close() only sends close_notify, it does not
            // close the transport socket.
            if (m_session->m_conn) {
                Socket_base* sock = Socket_base::getInstance(m_session->m_conn);
                if (sock) {
                    sock->abort();
                } else {
                    TLSSocket* tls = (TLSSocket*)TLSSocket_base::getInstance(m_session->m_conn);
                    if (tls && tls->m_stream) {
                        sock = Socket_base::getInstance(tls->m_stream);
                        if (sock)
                            sock->abort();
                    }
                }
            }
            return next();
        }

        virtual int32_t error(int32_t v)
        {
            // Ignore write errors during close (e.g. broken pipe if the
            // remote side already closed the connection).
            if (at(write_goaway))
                return next(close_conn);
            if (at(close_conn) || at(abort_socket))
                return next();
            return v;
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

    if (m_ref_active) {
        m_ref_active = false;
        isolate_unref();
    }

    H2_TRACE("listener_holder release reason=destroy holder=%p", (void*)m_listener_holder.get());
    m_listener_holder.Release();

    // Close all streams
    closeAllStreams();

    m_stream_lock.lock();
    m_streams.clear();
    m_stream_lock.unlock();

    m_nghttp2.del();

    m_close_event.set();

    // Abort underlying socket so readLoop unblocks immediately
    if (m_conn) {
        Socket_base* sock = Socket_base::getInstance(m_conn);
        if (sock) {
            sock->abort();
        } else {
            TLSSocket* tls = (TLSSocket*)TLSSocket_base::getInstance(m_conn);
            if (tls && tls->m_stream) {
                sock = Socket_base::getInstance(tls->m_stream);
                if (sock)
                    sock->abort();
            }
        }
    }

    return 0;
}

} /* namespace fibjs */
