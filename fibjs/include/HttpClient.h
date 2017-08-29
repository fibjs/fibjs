/*
 * HttpClient.h
 *
 *  Created on: Aug 12, 2016
 */

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include "ifs/HttpClient.h"
#include "HttpCookie.h"
#include "List.h"

namespace fibjs {

class HttpClient : public HttpClient_base {
    FIBER_FREE();

public:
    HttpClient()
        : m_timeout(0)
        , m_enableCookie(true)
        , m_autoRedirect(true)
        , m_maxBodySize(-1)
    {
        m_cookies = new List();
        m_userAgent = "Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.98 Safari/537.36";
    }

public:
    // HttpClient_base
    virtual result_t get_cookies(obj_ptr<List_base>& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t get_enableCookie(bool& retVal);
    virtual result_t set_enableCookie(bool newVal);
    virtual result_t get_autoRedirect(bool& retVal);
    virtual result_t set_autoRedirect(bool newVal);
    virtual result_t get_maxBodySize(int32_t& retVal);
    virtual result_t set_maxBodySize(int32_t newVal);
    virtual result_t get_userAgent(exlib::string& retVal);
    virtual result_t set_userAgent(exlib::string newVal);
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string method, exlib::string url, SeekableStream_base* body, Map_base* headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string method, exlib::string url, SeekableStream_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t request(exlib::string method, exlib::string url, Buffer_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t get(exlib::string url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t post(exlib::string url, SeekableStream_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t post(exlib::string url, Buffer_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t post(exlib::string url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t del(exlib::string url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t put(exlib::string url, SeekableStream_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t put(exlib::string url, Buffer_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t put(exlib::string url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t patch(exlib::string url, SeekableStream_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t patch(exlib::string url, Buffer_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    virtual result_t patch(exlib::string url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);

public:
    result_t update_cookies(exlib::string url, obj_ptr<List_base> cookies);
    result_t get_cookie(exlib::string url, exlib::string& retVal);

    exlib::string agent()
    {
        return m_userAgent;
    }

private:
    result_t update(obj_ptr<HttpCookie_base> cookie);

private:
    obj_ptr<List> m_cookies;
    int32_t m_timeout;
    bool m_enableCookie;
    bool m_autoRedirect;
    int32_t m_maxBodySize;
    exlib::string m_userAgent;
};
} /* namespace fibjs */

#endif /* HTTPCLIENT_H_ */
