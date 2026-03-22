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
        int rv = nghttp2_submit_settings(m_session, NGHTTP2_FLAG_NONE, iv, niv);
        m_lock.unlock();
        return rv;
    }

    int32_t submit_request(const nghttp2_nv* nva, size_t nvlen,
        const nghttp2_data_provider* data_prd)
    {
        m_lock.lock();
        int32_t rv = nghttp2_submit_request(m_session, nullptr, nva, nvlen, data_prd, nullptr);
        m_lock.unlock();
        return rv;
    }

    int submit_response(int32_t stream_id, const nghttp2_nv* nva, size_t nvlen,
        const nghttp2_data_provider* data_prd)
    {
        m_lock.lock();
        int rv = nghttp2_submit_response(m_session, stream_id, nva, nvlen, data_prd);
        m_lock.unlock();
        return rv;
    }

    int submit_headers(int32_t stream_id, const nghttp2_nv* nva, size_t nvlen)
    {
        m_lock.lock();
        int rv = nghttp2_submit_headers(m_session, NGHTTP2_FLAG_NONE,
            stream_id, nullptr, nva, nvlen, nullptr);
        m_lock.unlock();
        return rv;
    }

    int submit_trailer(int32_t stream_id, const nghttp2_nv* nva, size_t nvlen)
    {
        m_lock.lock();
        int rv = nghttp2_submit_trailer(m_session, stream_id, nva, nvlen);
        m_lock.unlock();
        return rv;
    }

    int submit_rst_stream(int32_t stream_id, uint32_t error_code)
    {
        m_lock.lock();
        int rv = nghttp2_submit_rst_stream(m_session, NGHTTP2_FLAG_NONE, stream_id, error_code);
        m_lock.unlock();
        return rv;
    }

    int submit_goaway(int32_t last_stream_id, uint32_t error_code)
    {
        m_lock.lock();
        int rv = nghttp2_submit_goaway(m_session, NGHTTP2_FLAG_NONE,
            last_stream_id, error_code, nullptr, 0);
        m_lock.unlock();
        return rv;
    }

    int submit_ping()
    {
        m_lock.lock();
        int rv = nghttp2_submit_ping(m_session, NGHTTP2_FLAG_NONE, nullptr);
        m_lock.unlock();
        return rv;
    }

    int resume_data(int32_t stream_id)
    {
        m_lock.lock();
        int rv = nghttp2_session_resume_data(m_session, stream_id);
        m_lock.unlock();
        return rv;
    }

    ssize_t recv(const uint8_t* data, size_t len)
    {
        m_lock.lock();
        ssize_t rv = nghttp2_session_mem_recv(m_session, data, len);
        m_lock.unlock();
        return rv;
    }

    ssize_t collect_output(exlib::string& output)
    {
        const uint8_t* data;
        ssize_t len;

        m_lock.lock();
        while ((len = nghttp2_session_mem_send(m_session, &data)) > 0)
            output.append((const char*)data, len);
        m_lock.unlock();

        return len;
    }

    uint32_t get_setting(bool remote, int32_t id)
    {
        m_lock.lock();
        uint32_t val = remote
            ? nghttp2_session_get_remote_settings(m_session, (nghttp2_settings_id)id)
            : nghttp2_session_get_local_settings(m_session, (nghttp2_settings_id)id);
        m_lock.unlock();
        return val;
    }

    operator bool() const { return m_session != nullptr; }

private:
    nghttp2_session* m_session;
    exlib::spinlock m_lock;
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

    // Collect pending nghttp2 output into a buffer (no I/O)
    result_t collectPendingData(obj_ptr<Buffer_base>& buf);

    // Send pending data synchronously (JS thread only)
    result_t sendPendingData();

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

    NgHttp2Handler m_nghttp2;
    obj_ptr<Stream_base> m_conn;

    exlib::spinlock m_stream_lock;
    std::map<int32_t, obj_ptr<Http2Stream>> m_streams;

    // Temporary headers being built during on_header callbacks
    exlib::spinlock m_header_lock;
    std::map<int32_t, std::vector<std::pair<exlib::string, exlib::string>>> m_pending_headers;

    exlib::Event m_close_event;

    // Connection authority info (for auto-filling pseudo-headers in request())
    exlib::string m_authority;
    exlib::string m_scheme;
};

} /* namespace fibjs */
