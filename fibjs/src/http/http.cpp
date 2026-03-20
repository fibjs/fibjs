/*
 * http.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/http.h"
#include "ifs/HttpServer.h"
#include "ifs/HttpsServer.h"
#include "ifs/tls.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "Url.h"
#include "HttpRequest.h"
#include "HttpClient.h"
#include "BufferedStream.h"
#include <unordered_map>
#include "Isolate.h"
#include "ifs/zlib.h"
#include <uv/include/uv.h>

namespace fibjs {

DECLARE_MODULE(http);
DECLARE_MODULE_EX(https, http);

static HttpClient* get_httpClient(Isolate* isolate = NULL)
{
    if (isolate == NULL)
        isolate = Isolate::current();
    return isolate->m_httpclient;
}

result_t http_request(exlib::string method, exlib::string url,
    SeekableStream_base* body, Headers_base* headers,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(method, url, body, headers, retVal, ac);
}

result_t http_request2(HttpClient_base* httpClient, exlib::string method, exlib::string url,
    SeekableStream_base* body, Headers_base* headers,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
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

result_t http_base::get_keepAlive(bool& retVal)
{
    return get_httpClient()->get_keepAlive(retVal);
}

result_t http_base::set_keepAlive(bool newVal)
{
    return get_httpClient()->set_keepAlive(newVal);
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

result_t http_base::get_enableEncoding(bool& retVal)
{
    return get_httpClient()->get_enableEncoding(retVal);
}

result_t http_base::set_enableEncoding(bool newVal)
{
    return get_httpClient()->set_enableEncoding(newVal);
}

result_t http_base::get_maxHeadersCount(int32_t& retVal)
{
    return get_httpClient()->get_maxHeadersCount(retVal);
}

result_t http_base::set_maxHeadersCount(int32_t newVal)
{
    return get_httpClient()->set_maxHeadersCount(newVal);
}

result_t http_base::get_maxHeaderSize(int32_t& retVal)
{
    return get_httpClient()->get_maxHeaderSize(retVal);
}

result_t http_base::set_maxHeaderSize(int32_t newVal)
{
    return get_httpClient()->set_maxHeaderSize(newVal);
}

result_t http_base::get_maxChunkSize(int32_t& retVal)
{
    return get_httpClient()->get_maxChunkSize(retVal);
}

result_t http_base::set_maxChunkSize(int32_t newVal)
{
    return get_httpClient()->set_maxChunkSize(newVal);
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

result_t http_base::setGlobalProxyFromEnv(v8::Local<v8::Object> proxyEnv, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current(proxyEnv);
    HttpClient* hc = get_httpClient(isolate);

    // Save current proxy settings for restore function
    exlib::string old_http_proxy, old_https_proxy, old_no_proxy;
    hc->get_http_proxy(old_http_proxy);
    hc->get_https_proxy(old_https_proxy);
    old_no_proxy = hc->m_no_proxy;

    v8::Local<v8::Array> keys = proxyEnv->GetOwnPropertyNames(isolate->context()).FromMaybe(v8::Local<v8::Array>());
    bool useEnv = keys.IsEmpty() || keys->Length() == 0;

    exlib::string http_proxy, https_proxy, no_proxy;

    if (useEnv) {
        // Read directly from system environment variables using uv_os_getenv
        char buf[4096];
        size_t sz;

        // http_proxy (lowercase first, then uppercase)
        sz = sizeof(buf);
        if (uv_os_getenv("http_proxy", buf, &sz) == 0)
            http_proxy = buf;
        else {
            sz = sizeof(buf);
            if (uv_os_getenv("HTTP_PROXY", buf, &sz) == 0)
                http_proxy = buf;
        }

        // https_proxy (lowercase first, then uppercase)
        sz = sizeof(buf);
        if (uv_os_getenv("https_proxy", buf, &sz) == 0)
            https_proxy = buf;
        else {
            sz = sizeof(buf);
            if (uv_os_getenv("HTTPS_PROXY", buf, &sz) == 0)
                https_proxy = buf;
        }

        // no_proxy (lowercase first, then uppercase)
        sz = sizeof(buf);
        if (uv_os_getenv("no_proxy", buf, &sz) == 0)
            no_proxy = buf;
        else {
            sz = sizeof(buf);
            if (uv_os_getenv("NO_PROXY", buf, &sz) == 0)
                no_proxy = buf;
        }
    } else {
        // Read from provided proxyEnv object
        result_t hr = hc->set_proxyEnv(proxyEnv);
        if (hr < 0)
            return hr;

        // Get the values that were set
        hc->get_http_proxy(http_proxy);
        hc->get_https_proxy(https_proxy);
        no_proxy = hc->m_no_proxy;
    }

    if (useEnv) {
        // Apply the proxy settings directly
        hc->set_http_proxy(http_proxy);
        hc->set_https_proxy(https_proxy);
        hc->m_no_proxy = no_proxy;
    }

    // Create restore function using native callback with captured data in an array
    v8::Local<v8::Array> data = v8::Array::New(isolate->m_isolate, 3);
    data->Set(isolate->context(), 0, isolate->NewString(old_http_proxy)).FromJust();
    data->Set(isolate->context(), 1, isolate->NewString(old_https_proxy)).FromJust();
    data->Set(isolate->context(), 2, isolate->NewString(old_no_proxy)).FromJust();

    v8::MaybeLocal<v8::Function> maybeFunc = v8::Function::New(
        isolate->context(),
        [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = Isolate::current(info);
            v8::Local<v8::Context> context = isolate->context();
            v8::Local<v8::Array> data = info.Data().As<v8::Array>();

            exlib::string http_proxy, https_proxy, no_proxy;
            GetArgumentValue(isolate, JSValue(data->Get(context, 0)), http_proxy);
            GetArgumentValue(isolate, JSValue(data->Get(context, 1)), https_proxy);
            GetArgumentValue(isolate, JSValue(data->Get(context, 2)), no_proxy);

            HttpClient* hc = get_httpClient(isolate);
            hc->set_http_proxy(http_proxy);
            hc->set_https_proxy(https_proxy);
            hc->m_no_proxy = no_proxy;
        },
        data);

    if (maybeFunc.IsEmpty())
        return CHECK_ERROR(Runtime::setError("Failed to create restore function"));

    retVal = maybeFunc.ToLocalChecked();
    return 0;
}

result_t http_base::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(conn, req, retVal, ac);
}

result_t http_base::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(method, url, opts, retVal, ac);
}

result_t http_base::request(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(url, opts, retVal, ac);
}

result_t http_base::request(v8::Local<v8::Object> opts,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(opts, retVal, ac);
}

result_t http_base::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(method, url, opts, callback, retVal, ac);
}

result_t http_base::request(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(url, opts, callback, retVal, ac);
}

result_t http_base::request(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(url, callback, retVal, ac);
}

result_t http_base::request(v8::Local<v8::Object> opts, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->request(opts, callback, retVal, ac);
}

result_t http_base::get(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->get(url, opts, retVal, ac);
}

result_t http_base::get(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->get(url, opts, callback, retVal, ac);
}

result_t http_base::get(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->get(url, callback, retVal, ac);
}

result_t http_base::post(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->post(url, opts, retVal, ac);
}

result_t http_base::post(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->post(url, opts, callback, retVal, ac);
}

result_t http_base::post(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->post(url, callback, retVal, ac);
}

result_t http_base::del(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->del(url, opts, retVal, ac);
}

result_t http_base::del(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->del(url, opts, callback, retVal, ac);
}

result_t http_base::del(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->del(url, callback, retVal, ac);
}

result_t http_base::put(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->put(url, opts, retVal, ac);
}

result_t http_base::put(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->put(url, opts, callback, retVal, ac);
}

result_t http_base::put(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->put(url, callback, retVal, ac);
}

result_t http_base::patch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->patch(url, opts, retVal, ac);
}

result_t http_base::patch(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->patch(url, opts, callback, retVal, ac);
}

result_t http_base::patch(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->patch(url, callback, retVal, ac);
}

result_t http_base::head(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->head(url, opts, retVal, ac);
}

result_t http_base::head(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->head(url, opts, callback, retVal, ac);
}

result_t http_base::head(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->head(url, callback, retVal, ac);
}

result_t http_base::createServer(Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal)
{
    return HttpServer_base::_new(hdlr, retVal);
}

result_t http_base::createServer(SecureContext_base* context, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal)
{
    obj_ptr<HttpsServer_base> server;
    result_t hr = HttpsServer_base::_new(context, hdlr, server);
    if (hr < 0)
        return hr;

    retVal = server;
    return 0;
}

result_t http_base::createServer(v8::Local<v8::Object> options, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal)
{
    Isolate* isolate = Isolate::current(options);
    v8::Local<v8::Context> context = isolate->context();

    // detect TLS-related fields to decide http vs https
    bool hasCert = options->Has(context, isolate->NewString("cert")).FromMaybe(false);
    bool hasCa = options->Has(context, isolate->NewString("ca")).FromMaybe(false);

    if (hasCert || hasCa) {
        obj_ptr<SecureContext_base> ctx;
        result_t hr = tls_base::createSecureContext(options, true, ctx);
        if (hr < 0)
            return hr;

        return createServer(ctx, hdlr, retVal);
    }

    return createServer(hdlr, retVal);
}

result_t http_base::fetch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->fetch(url, opts, retVal, ac);
}

result_t http_base::fetch(HttpRequest_base* request, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return get_httpClient(ac->isolate())->fetch(request, opts, retVal, ac);
}
}
