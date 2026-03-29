/*
 * HttpClient.h
 *
 *  Created on: Aug 12, 2016
 */

#pragma once

#include "ifs/HttpClient.h"
#include "HttpCookie.h"
#include "Url.h"
#include "HttpRequest.h"
#include "Http2Session.h"
#include "LruCache.h"

namespace fibjs {

class HttpClient : public HttpClient_base {
    FIBER_FREE();

public:
    HttpClient(SecureContext_base* context)
        : m_context(context)
        , m_keepAlive(true)
        , m_timeout(0)
        , m_enableCookie(true)
        , m_autoRedirect(true)
        , m_enableEncoding(true)
        , m_enableH2(true)
        , m_maxHeadersCount(128)
        , m_maxHeaderSize(8192)
        , m_maxChunkSize(2)
        , m_maxBodySize(-1)
        , m_poolTimeout(10000)
        , m_maxSockets(INT32_MAX)
        , m_maxTotalSockets(INT32_MAX)
        , m_maxFreeSockets(256)
        , m_defaultPort(80)
        , m_protocol("http:")
    {
        m_cookies = new NArray();
        m_userAgent = "curl/8.14.1";
    }

public:
    // HttpClient_base
    virtual result_t get_cookies(obj_ptr<NArray>& retVal);
    virtual result_t get_keepAlive(bool& retVal);
    virtual result_t set_keepAlive(bool newVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t get_enableCookie(bool& retVal);
    virtual result_t set_enableCookie(bool newVal);
    virtual result_t get_autoRedirect(bool& retVal);
    virtual result_t set_autoRedirect(bool newVal);
    virtual result_t get_enableEncoding(bool& retVal);
    virtual result_t set_enableEncoding(bool newVal);
    virtual result_t get_enableH2(bool& retVal);
    virtual result_t set_enableH2(bool newVal);
    virtual result_t get_maxHeadersCount(int32_t& retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxHeaderSize(int32_t& retVal);
    virtual result_t set_maxHeaderSize(int32_t newVal);
    virtual result_t get_maxChunkSize(int32_t& retVal);
    virtual result_t set_maxChunkSize(int32_t newVal);
    virtual result_t get_maxBodySize(int32_t& retVal);
    virtual result_t set_maxBodySize(int32_t newVal);
    virtual result_t get_userAgent(exlib::string& retVal);
    virtual result_t set_userAgent(exlib::string newVal);
    virtual result_t get_poolTimeout(int32_t& retVal);
    virtual result_t set_poolTimeout(int32_t newVal);
    result_t get_http_proxy(exlib::string& retVal);
    result_t set_http_proxy(exlib::string newVal);
    result_t get_https_proxy(exlib::string& retVal);
    result_t set_https_proxy(exlib::string newVal);
    virtual result_t get_proxyEnv(v8::Local<v8::Object>& retVal);
    virtual result_t set_proxyEnv(v8::Local<v8::Object> newVal);
    virtual result_t get_maxSockets(int32_t& retVal);
    virtual result_t set_maxSockets(int32_t newVal);
    virtual result_t get_maxTotalSockets(int32_t& retVal);
    virtual result_t set_maxTotalSockets(int32_t newVal);
    virtual result_t get_maxFreeSockets(int32_t& retVal);
    virtual result_t set_maxFreeSockets(int32_t newVal);
    virtual result_t get_defaultPort(int32_t& retVal);
    virtual result_t set_defaultPort(int32_t newVal);
    virtual result_t get_protocol(exlib::string& retVal);
    virtual result_t set_protocol(exlib::string newVal);
    virtual result_t get_freeSockets(v8::Local<v8::Object>& retVal);
    virtual result_t get_sockets(v8::Local<v8::Object>& retVal);
    virtual result_t get_totalSocketCount(int32_t& retVal);
    virtual result_t getName(v8::Local<v8::Object> options, exlib::string& retVal);
    virtual result_t destroy();
    // Interface methods generated from IDL (no AsyncEvent in signature).
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t request(v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t request(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t request(v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t request(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t request(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t requestSync(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t requestSync(v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t requestSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t getSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t get(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t get(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t get(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t postSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t post(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t post(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t post(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t delSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t del(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t del(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t del(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t putSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t put(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t put(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t put(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t patchSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t patch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t patch(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t patch(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t headSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t head(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t head(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    virtual result_t head(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);

    // Internal async-context helpers.
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t request(v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t request(v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t get(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t get(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t get(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t post(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t post(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t post(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t del(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t del(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t del(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t put(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t put(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t put(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t patch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t patch(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t patch(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t head(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t head(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t head(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac);
    virtual result_t fetch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t fetch(HttpRequest_base* request, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);

public:
    result_t init(v8::Local<v8::Object> options);
    bool should_bypass_proxy(exlib::string hostname, int32_t port);
    result_t get_request_opts(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, AsyncEvent* ac,
        v8::Local<v8::Function> callback = v8::Local<v8::Function>(), bool skip_body = false);
    result_t requestSync(HttpRequest::Options* o, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    result_t request(HttpRequest::Options* o, AsyncEvent* ac);
    result_t fire_request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts,
        obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac);
    result_t fire_callback_request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts,
        v8::Local<v8::Function> callback, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac);
    result_t request(exlib::string method, exlib::string url, SeekableStream_base* body,
        Headers_base* headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac, bool streaming);
    result_t requestSync(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac, bool headerOnly);

    result_t update_cookies(exlib::string url, NArray* cookies);
    result_t get_cookie(exlib::string url, exlib::string& retVal);

    void setEnvProxy();

    exlib::string agent()
    {
        return m_userAgent;
    }

public:
    void clean_coon(date_t d)
    {
        std::vector<obj_ptr<Conn>> keep_conns;

        m_lock.lock();
        while (((int32_t)m_conns.size() > m_maxFreeSockets)
            || (m_conns.size() && d.diff(m_conns[0]->d) >= (double)m_poolTimeout)) {
            keep_conns.push_back(m_conns[0]);
            m_conns.erase(m_conns.begin());
        }
        m_lock.unlock();
    }

    void save_conn(exlib::string url, Stream_base* _conn)
    {
        obj_ptr<Conn> conn = new Conn();

        conn->d.now();
        conn->url = url;
        conn->conn = _conn;

        clean_coon(conn->d);

        m_lock.lock();
        m_conns.push_back(conn);
        if ((int32_t)m_conns.size() > m_maxFreeSockets) {
            conn = m_conns[0];
            m_conns.erase(m_conns.begin());
        }
        m_lock.unlock();
    }

    bool get_conn(exlib::string url, obj_ptr<Stream_base>& _conn)
    {
        date_t d;

        d.now();
        clean_coon(d);

        m_lock.lock();
        for (std::vector<obj_ptr<Conn>>::iterator it = m_conns.begin(); it != m_conns.end(); it++)
            if ((*it)->url == url) {
                _conn = (*it)->conn;
                m_conns.erase(it);

                m_lock.unlock();
                return true;
            }
        m_lock.unlock();
        return false;
    }

private:
    result_t update(HttpCookie_base* cookie);

public:
    obj_ptr<SecureContext_base> m_context;
    obj_ptr<NArray> m_cookies;
    exlib::spinlock m_lock;
    bool m_keepAlive;
    int32_t m_timeout;
    bool m_enableCookie;
    bool m_autoRedirect;
    bool m_enableEncoding;
    bool m_enableH2;
    int32_t m_maxHeadersCount;
    int32_t m_maxHeaderSize;
    int32_t m_maxChunkSize;
    int32_t m_maxBodySize;
    exlib::string m_userAgent;

private:
    friend class http_base;

    class Conn : public obj_base {
    public:
        date_t d;
        exlib::string url;
        obj_ptr<Stream_base> conn;
    };

    std::vector<obj_ptr<Conn>> m_conns;
    int32_t m_poolTimeout;
    int32_t m_maxSockets;
    int32_t m_maxTotalSockets;
    int32_t m_maxFreeSockets;
    int32_t m_defaultPort;
    exlib::string m_protocol;

public:
    // HTTP/2 session pool (keyed by "ssl://host:port")
    void save_h2session(exlib::string url, Http2Session* session)
    {
        m_h2sessions.set(url, session);
    }

    obj_ptr<Http2Session> get_h2session(exlib::string url)
    {
        obj_ptr<Http2Session> session;
        m_h2sessions.lookup(url, session);
        return session;
    }

    void remove_h2session(exlib::string url)
    {
        m_h2sessions.erase(url);
    }

    // Pending H2 handshake queue: prevents redundant TCP+TLS connections
    // when multiple fibers request the same H2 host concurrently.
    struct H2PendingItem {
        obj_ptr<Http2Session>* session;
        obj_ptr<Stream_base>* conn;
        AsyncEvent* ac;
    };

    struct H2PendingEntry {
        std::vector<H2PendingItem> waiters;
    };

    // Try to acquire H2 session creation for the given URL.
    // Returns true if this fiber is the leader (should perform handshake).
    // Returns false if queued as a waiter (caller should return CALL_E_PENDDING).
    bool h2_acquire(exlib::string url,
        obj_ptr<Http2Session>* retSession,
        obj_ptr<Stream_base>* retConn,
        AsyncEvent* ac)
    {
        m_h2_pending_lock.lock();
        auto it = m_h2_pending.find(url);
        if (it != m_h2_pending.end()) {
            it->second->waiters.push_back({ retSession, retConn, ac });
            m_h2_pending_lock.unlock();
            return false;
        }
        m_h2_pending[url] = new H2PendingEntry();
        m_h2_pending_lock.unlock();
        return true;
    }

    // Leader calls this after successful H2 session creation.
    // Wakes all queued waiters with the session and connection.
    void h2_complete(exlib::string url, Http2Session* session, Stream_base* conn)
    {
        H2PendingEntry* entry = nullptr;

        m_h2_pending_lock.lock();
        auto it = m_h2_pending.find(url);
        if (it != m_h2_pending.end()) {
            entry = it->second;
            m_h2_pending.erase(it);
        }
        m_h2_pending_lock.unlock();

        if (entry) {
            for (auto& item : entry->waiters) {
                *item.session = session;
                *item.conn = conn;
                item.ac->post(0);
            }
            delete entry;
        }
    }

    // Leader calls this on failure. Propagates error to all queued waiters.
    void h2_fail(exlib::string url, int32_t hr)
    {
        H2PendingEntry* entry = nullptr;

        m_h2_pending_lock.lock();
        auto it = m_h2_pending.find(url);
        if (it != m_h2_pending.end()) {
            entry = it->second;
            m_h2_pending.erase(it);
        }
        m_h2_pending_lock.unlock();

        if (entry) {
            for (auto& item : entry->waiters)
                item.ac->post(hr);
            delete entry;
        }
    }

private:
    LruCache<obj_ptr<Http2Session>> m_h2sessions;
    std::unordered_map<exlib::string, H2PendingEntry*> m_h2_pending;
    exlib::spinlock m_h2_pending_lock;

public:
    exlib::string m_http_proxy;
    exlib::string m_https_proxy;
    exlib::string m_no_proxy;
};
} /* namespace fibjs */
