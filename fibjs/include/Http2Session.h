/*
 * Http2Session.h
 *
 *  Created on: Mar 21, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/Http2Session.h"
#include "ifs/Stream.h"
#include "ifs/TLSSocket.h"
#include "Http2Stream.h"
#include <nghttp2/nghttp2.h>
#include <map>
#include <atomic>
#include <list>

namespace fibjs {

// Thread-safe wrapper around nghttp2_session with spinlock protection
class NgHttp2Handler {
public:
    NgHttp2Handler()
        : m_session(nullptr)
    {
    }

    ~NgHttp2Handler()
    {
        if (m_session) {
            nghttp2_session_del(m_session);
            m_session = nullptr;
        }
    }

    int init_server(nghttp2_session_callbacks* callbacks, void* user_data)
    {
        return nghttp2_session_server_new(&m_session, callbacks, user_data);
    }

    int init_client(nghttp2_session_callbacks* callbacks, void* user_data)
    {
        return nghttp2_session_client_new(&m_session, callbacks, user_data);
    }

    void del()
    {
        m_lock.lock();
        if (m_session) {
            nghttp2_session_del(m_session);
            m_session = nullptr;
        }
        m_lock.unlock();
    }

    int submit_settings(const nghttp2_settings_entry* iv, size_t niv)
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_submit_settings(m_session, NGHTTP2_FLAG_NONE, iv, niv) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    int32_t submit_request(const nghttp2_nv* nva, size_t nvlen,
        const nghttp2_data_provider* data_prd)
    {
        m_lock.lock();
        int32_t rv = m_session ? nghttp2_submit_request(m_session, nullptr, nva, nvlen, data_prd, nullptr) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    // Atomically submit request + collect output.
    // Prevents readLoop's asyncFlushOutput from stealing HEADERS.
    int32_t submit_request_and_collect(const nghttp2_nv* nva, size_t nvlen,
        const nghttp2_data_provider* data_prd, exlib::string& output)
    {
        m_lock.lock();
        int32_t rv = m_session ? nghttp2_submit_request(m_session, nullptr, nva, nvlen, data_prd, nullptr) : NGHTTP2_ERR_INVALID_STATE;
        if (rv > 0) {
            const uint8_t* data;
            ssize_t len;
            while ((len = nghttp2_session_mem_send(m_session, &data)) > 0)
                output.append((const char*)data, len);
        }
        m_lock.unlock();
        return rv;
    }

    // Atomically submit request + resume data + collect output.
    // Prevents readLoop's asyncFlushOutput from stealing HEADERS
    // before DATA is ready in concurrent scenarios.
    // on_submit is called after submit succeeds (with stream_id) but
    // before resume_data, while the lock is still held.
    template <typename F>
    int32_t submit_request_with_data(const nghttp2_nv* nva, size_t nvlen,
        const nghttp2_data_provider* data_prd,
        int32_t& stream_id, exlib::string& output, F&& on_submit)
    {
        m_lock.lock();
        stream_id = m_session ? nghttp2_submit_request(m_session, nullptr, nva, nvlen, data_prd, nullptr) : NGHTTP2_ERR_INVALID_STATE;
        if (stream_id > 0) {
            on_submit(stream_id);

            nghttp2_session_resume_data(m_session, stream_id);

            const uint8_t* data;
            ssize_t len;
            while ((len = nghttp2_session_mem_send(m_session, &data)) > 0)
                output.append((const char*)data, len);
        }
        m_lock.unlock();
        return stream_id;
    }

    int submit_response(int32_t stream_id, const nghttp2_nv* nva, size_t nvlen,
        const nghttp2_data_provider* data_prd)
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_submit_response(m_session, stream_id, nva, nvlen, data_prd) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    int submit_headers(int32_t stream_id, const nghttp2_nv* nva, size_t nvlen)
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_submit_headers(m_session, NGHTTP2_FLAG_NONE,
            stream_id, nullptr, nva, nvlen, nullptr) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    int submit_trailer(int32_t stream_id, const nghttp2_nv* nva, size_t nvlen)
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_submit_trailer(m_session, stream_id, nva, nvlen) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    int submit_rst_stream(int32_t stream_id, uint32_t error_code)
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_submit_rst_stream(m_session, NGHTTP2_FLAG_NONE, stream_id, error_code) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    int submit_goaway(int32_t last_stream_id, uint32_t error_code)
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_submit_goaway(m_session, NGHTTP2_FLAG_NONE,
            last_stream_id, error_code, nullptr, 0) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    int submit_ping()
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_submit_ping(m_session, NGHTTP2_FLAG_NONE, nullptr) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    int submit_window_update(int32_t stream_id, int32_t window_size_increment)
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_submit_window_update(m_session, NGHTTP2_FLAG_NONE,
            stream_id, window_size_increment) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    int resume_data(int32_t stream_id)
    {
        m_lock.lock();
        int rv = m_session ? nghttp2_session_resume_data(m_session, stream_id) : NGHTTP2_ERR_INVALID_STATE;
        m_lock.unlock();
        return rv;
    }

    ssize_t recv(const uint8_t* data, size_t len)
    {
        // Lock is required: although recv is only called from readLoop,
        // its callbacks (_emit) post async tasks to the JS thread, and
        // those tasks may call submit_*/collect_output concurrently.
        // All _emit calls in callbacks are fire-and-forget (post_task),
        // so they never re-acquire m_lock — no deadlock risk.
        m_lock.lock();
        if (!m_session) {
            m_lock.unlock();
            return NGHTTP2_ERR_INVALID_STATE;
        }
        size_t off = 0;
        ssize_t rv = 0;

        while (off < len) {
            rv = nghttp2_session_mem_recv(m_session, data + off, len - off);
            if (rv < 0)
                break;
            if (rv == 0)
                break;
            off += (size_t)rv;
        }

        if (rv >= 0)
            rv = (ssize_t)off;
        m_lock.unlock();
        return rv;
    }

    ssize_t collect_output(exlib::string& output)
    {
        const uint8_t* data;
        ssize_t len;

        m_lock.lock();
        if (!m_session) {
            m_lock.unlock();
            return NGHTTP2_ERR_INVALID_STATE;
        }
        while ((len = nghttp2_session_mem_send(m_session, &data)) > 0)
            output.append((const char*)data, len);
        m_lock.unlock();

        return len;
    }

    uint32_t get_setting(bool remote, int32_t id)
    {
        m_lock.lock();
        uint32_t val = !m_session ? 0 : (remote
            ? nghttp2_session_get_remote_settings(m_session, (nghttp2_settings_id)id)
            : nghttp2_session_get_local_settings(m_session, (nghttp2_settings_id)id));
        m_lock.unlock();
        return val;
    }

    operator bool() const { return m_session != nullptr; }

    int want_read() { return m_session ? nghttp2_session_want_read(m_session) : 0; }
    int want_write() { return m_session ? nghttp2_session_want_write(m_session) : 0; }

private:
    nghttp2_session* m_session;
    exlib::spinlock m_lock;
};

// Write queue item for serializing writes to the connection
class AsyncFlushItem : public exlib::linkitem {
public:
    AsyncFlushItem(obj_ptr<Buffer_base> buf, bool blocking = false)
        : m_buf(buf)
        , m_result(0)
        , m_blocking(blocking)
    {
    }

    obj_ptr<Buffer_base> m_buf;
    exlib::Event m_event;
    int32_t m_result;
    bool m_blocking;
};

class Http2Session : public Http2Session_base {
    FIBER_FREE();

public:
    Http2Session(bool is_server)
        : m_is_server(is_server)
    {
    }

    ~Http2Session() { }

public:
    // Http2Session_base
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func);
    virtual result_t get_remoteSettings(v8::Local<v8::Object>& retVal);
    virtual result_t get_localSettings(v8::Local<v8::Object>& retVal);
    virtual result_t get_destroyed(bool& retVal);
    virtual result_t get_closed(bool& retVal);
    virtual result_t get_alpnProtocol(exlib::string& retVal);
    virtual result_t get_socket(obj_ptr<Stream_base>& retVal);
    virtual result_t request(v8::Local<v8::Object> headers, v8::Local<v8::Object> options, obj_ptr<Http2Stream_base>& retVal);
    virtual result_t goaway(int32_t code, int32_t lastStreamId);

    // Pure C++ request interface (no V8 access needed, safe for async threads)
    result_t request(const std::vector<std::pair<exlib::string, exlib::string>>& headers,
        bool endStream, obj_ptr<Http2Stream>& retVal);
    virtual result_t ping(int32_t& retVal, AsyncEvent* ac);
    virtual result_t settings(v8::Local<v8::Object> settings);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t destroy();

public:
    // Initialize session (client or server)
    result_t init(Stream_base* conn);

    // Start read/write loops
    void startLoops();

    // Signal that session readLoop has finished
    void signalDone()
    {
        m_done_lock.lock();
        AsyncEvent* ac = m_done_ac;
        m_done_ac = nullptr;
        m_done_lock.unlock();
        if (ac)
            ac->post(0);
    }

    // Collect pending nghttp2 output into a buffer (no I/O)
    result_t collectPendingData(obj_ptr<Buffer_base>& buf);

    // Send pending data synchronously (JS thread only)
    result_t sendPendingData();

    // Post async flush of pending nghttp2 output (safe from any thread)
    void asyncFlushOutput();

    // Enqueue a write item (starts writer if queue was empty)
    void enqueueFlush(AsyncFlushItem* item);

    // Queue JS event emissions from nghttp2 callbacks and flush them
    // outside nghttp2 recv lock to avoid lock inversion and stalls.
    void enqueueHeaderEvent(Http2Stream* stream, NObject* headers);
    void enqueueStreamEvent(Http2Stream* stream, NObject* headers);
    void flushPendingEvents();

    // Get stream by ID
    obj_ptr<Http2Stream> getStream(int32_t stream_id);

    // Remove stream from map
    void removeStream(int32_t stream_id);

    // Add stream to map
    void addStream(int32_t stream_id, Http2Stream* stream);

    // Close all streams (notify waiting readers)
    void closeAllStreams();

private:
    // Setup nghttp2 callbacks
    void setupCallbacks(nghttp2_session_callbacks* callbacks);

    // The read loop fiber: reads from connection and feeds to nghttp2
    void readLoop();

    // nghttp2 callbacks
    static int on_begin_headers_callback(nghttp2_session* session,
        const nghttp2_frame* frame, void* user_data);
    static int on_header_callback(nghttp2_session* session,
        const nghttp2_frame* frame, const uint8_t* name, size_t namelen,
        const uint8_t* value, size_t valuelen, uint8_t flags, void* user_data);
    static int on_frame_recv_callback(nghttp2_session* session,
        const nghttp2_frame* frame, void* user_data);
    static int on_data_chunk_recv_callback(nghttp2_session* session,
        uint8_t flags, int32_t stream_id, const uint8_t* data, size_t len,
        void* user_data);
    static int on_stream_close_callback(nghttp2_session* session,
        int32_t stream_id, uint32_t error_code, void* user_data);

public:
    // Data provider callback for sending DATA frames
    static ssize_t data_source_read_callback(nghttp2_session* session,
        int32_t stream_id, uint8_t* buf, size_t length, uint32_t* data_flags,
        nghttp2_data_source* source, void* user_data);

public:
    bool m_is_server;
    bool m_internal = false; // true when used by HttpClient auto-upgrade (no V8 access)
    bool m_destroyed = false;
    bool m_closed = false;
    bool m_ref_active = false;

    NgHttp2Handler m_nghttp2;
    obj_ptr<Stream_base> m_conn;

    exlib::spinlock m_stream_lock;
    std::map<int32_t, obj_ptr<Http2Stream>> m_streams;

    // Temporary headers being built during on_header callbacks
    exlib::spinlock m_header_lock;
    std::map<int32_t, std::vector<std::pair<exlib::string, exlib::string>>> m_pending_headers;

    exlib::Event m_close_event;
    exlib::Event m_remote_settings_event; // signaled when remote SETTINGS received
    std::atomic<bool> m_remote_settings_ready { false };

    // Signaled when readLoop finishes; used by Http2Server to keep
    // the TcpServer handler alive until the session is done.
    exlib::spinlock m_done_lock;
    AsyncEvent* m_done_ac = nullptr;

    // Serialize submit_request + enqueueFlush to preserve HPACK encoding order
    exlib::spinlock m_request_lock;

    // Serialize all writes to the connection via write queue
    exlib::spinlock m_write_spinlock;
    exlib::List<AsyncFlushItem> m_write_queue;
    bool m_writer_active = false;

    exlib::spinlock m_pending_event_lock;
    std::list<std::pair<obj_ptr<Http2Stream>, obj_ptr<NObject>>> m_pending_header_events;
    std::list<std::pair<obj_ptr<Http2Stream>, obj_ptr<NObject>>> m_pending_stream_events;

    // Keep JS wrapper alive while there are active session-level listeners.
    obj_ptr<ValueHolder> m_listener_holder;

    // Connection authority info (for auto-filling pseudo-headers in request())
    exlib::string m_authority;
    exlib::string m_scheme;
};

} /* namespace fibjs */
