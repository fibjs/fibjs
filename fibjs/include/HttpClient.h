/*
 * HttpClient.h
 *
 *  Created on: Aug 12, 2016
 */

#pragma once

#include "ifs/HttpClient.h"
#include "HttpCookie.h"
#include "ifs/ssl.h"
#include "Url.h"

namespace fibjs {

class HttpClient : public HttpClient_base {
    FIBER_FREE();

public:
    HttpClient()
        : m_timeout(0)
        , m_enableCookie(true)
        , m_autoRedirect(true)
        , m_enableEncoding(true)
        , m_sslVerification(-1)
        , m_maxBodySize(-1)
        , m_poolSize(128)
        , m_poolTimeout(10000)
    {
        m_cookies = new NArray();
        m_userAgent = "Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.98 Safari/537.36";
    }

public:
    // HttpClient_base
    virtual result_t get_cookies(obj_ptr<NArray>& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t get_enableCookie(bool& retVal);
    virtual result_t set_enableCookie(bool newVal);
    virtual result_t get_autoRedirect(bool& retVal);
    virtual result_t set_autoRedirect(bool newVal);
    virtual result_t get_enableEncoding(bool& retVal);
    virtual result_t set_enableEncoding(bool newVal);
    virtual result_t get_maxBodySize(int32_t& retVal);
    virtual result_t set_maxBodySize(int32_t newVal);
    virtual result_t get_userAgent(exlib::string& retVal);
    virtual result_t set_userAgent(exlib::string newVal);
    virtual result_t get_poolSize(int32_t& retVal);
    virtual result_t set_poolSize(int32_t newVal);
    virtual result_t get_poolTimeout(int32_t& retVal);
    virtual result_t set_poolTimeout(int32_t newVal);
    virtual result_t get_proxyAgent(exlib::string& retVal);
    virtual result_t set_proxyAgent(exlib::string newVal);
    virtual result_t get_sslVerification(int32_t& retVal);
    virtual result_t set_sslVerification(int32_t newVal);
    virtual result_t setClientCert(X509Cert_base* crt, PKey_base* key);
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t get(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t post(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t del(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t put(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t patch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t head(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);

public:
    result_t request(exlib::string method, exlib::string url, SeekableStream_base* body,
        SeekableStream_base* response_body, NObject* opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    result_t update_cookies(exlib::string url, NArray* cookies);
    result_t get_cookie(exlib::string url, exlib::string& retVal);

    exlib::string agent()
    {
        return m_userAgent;
    }

public:
    void clean_coon(date_t d)
    {
        std::vector<obj_ptr<Conn>> keep_conns;

        m_lock.lock();
        while (((int32_t)m_conns.size() > m_poolSize)
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
        if ((int32_t)m_conns.size() > m_poolSize) {
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

private:
    obj_ptr<NArray> m_cookies;
    exlib::spinlock m_lock;
    int32_t m_timeout;
    bool m_enableCookie;
    bool m_autoRedirect;
    bool m_enableEncoding;
    int32_t m_sslVerification;
    int32_t m_maxBodySize;
    exlib::string m_userAgent;
    obj_ptr<X509Cert_base> m_crt;
    obj_ptr<PKey_base> m_key;

private:
    class Conn : public obj_base {
    public:
        date_t d;
        exlib::string url;
        obj_ptr<Stream_base> conn;
    };

    std::vector<obj_ptr<Conn>> m_conns;
    int32_t m_poolSize;
    int32_t m_poolTimeout;
    exlib::string m_proxyAgent;
};
} /* namespace fibjs */
