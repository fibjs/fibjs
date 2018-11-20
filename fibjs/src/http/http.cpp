/*
 * http.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/http.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "Url.h"
#include "HttpRequest.h"
#include "HttpClient.h"
#include "BufferedStream.h"
#include "map"
#include "Isolate.h"
#include "ifs/zlib.h"

namespace fibjs {

DECLARE_MODULE(http);
DECLARE_MODULE_EX(https, http);

static HttpClient* get_httpClient(Isolate* isolate = NULL)
{
    if (isolate == NULL)
        isolate = Isolate::current();

    if (!isolate->m_httpclient)
        isolate->m_httpclient = new HttpClient();
    return (HttpClient*)(obj_base*)isolate->m_httpclient;
}

result_t http_request(exlib::string method, exlib::string url,
    SeekableStream_base* body, NObject* headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(method, url, body, headers, retVal, ac);
}

result_t http_request2(HttpClient_base* httpClient, exlib::string method, exlib::string url,
    SeekableStream_base* body, NObject* headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (httpClient != NULL)
        return ((HttpClient*)httpClient)->request(method, url, body, headers, retVal, ac);
    else
        return get_httpClient(ac->isolate())->request(method, url, body, headers, retVal, ac);
}

result_t http_base::get_cookies(obj_ptr<NArray>& retVal)
{
    return get_httpClient()->get_cookies(retVal);
}

result_t http_base::get_timeout(int32_t& retVal)
{
    return get_httpClient()->get_timeout(retVal);
}

result_t http_base::set_timeout(int32_t newVal)
{
    return get_httpClient()->set_timeout(newVal);
}

result_t http_base::get_enableCookie(bool& retVal)
{
    return get_httpClient()->get_enableCookie(retVal);
}

result_t http_base::set_enableCookie(bool newVal)
{
    return get_httpClient()->set_enableCookie(newVal);
}

result_t http_base::get_autoRedirect(bool& retVal)
{
    return get_httpClient()->get_autoRedirect(retVal);
}

result_t http_base::set_autoRedirect(bool newVal)
{
    return get_httpClient()->set_autoRedirect(newVal);
}

result_t http_base::get_maxBodySize(int32_t& retVal)
{
    return get_httpClient()->get_maxBodySize(retVal);
}

result_t http_base::set_maxBodySize(int32_t newVal)
{
    return get_httpClient()->set_maxBodySize(newVal);
}

result_t http_base::get_userAgent(exlib::string& retVal)
{
    return get_httpClient()->get_userAgent(retVal);
}

result_t http_base::set_userAgent(exlib::string newVal)
{
    return get_httpClient()->set_userAgent(newVal);
}

result_t http_base::get_poolSize(int32_t& retVal)
{
    return get_httpClient()->get_poolSize(retVal);
}

result_t http_base::set_poolSize(int32_t newVal)
{
    return get_httpClient()->set_poolSize(newVal);
}

result_t http_base::get_poolTimeout(int32_t& retVal)
{
    return get_httpClient()->get_poolTimeout(retVal);
}

result_t http_base::set_poolTimeout(int32_t newVal)
{
    return get_httpClient()->set_poolTimeout(newVal);
}

result_t http_base::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpResponse_base>& retVal,
    AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(conn, req, retVal, ac);
}

result_t http_base::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(method, url, opts, retVal, ac);
}

result_t http_base::get(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->get(url, headers, retVal, ac);
}

result_t http_base::post(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->post(url, headers, retVal, ac);
}

result_t http_base::del(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->del(url, headers, retVal, ac);
}

result_t http_base::put(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->put(url, headers, retVal, ac);
}

result_t http_base::patch(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->patch(url, headers, retVal, ac);
}
}
