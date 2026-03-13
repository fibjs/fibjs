/*
 * HttpClient.cpp
 *
 *  Created on: Aug 12, 2016
 */

#include "object.h"
#include "HttpClient.h"
#include "WebResponse.h"
#include "HttpMessage.h"
#include "Buffer.h"
#include "Blob.h"
#include "MemoryStream.h"
#include "HttpRequest.h"
#include "TLSSocket.h"
#include "BufferedStream.h"
#include "inetAddr.h"
#include "ifs/net.h"
#include "ifs/tls.h"
#include "ifs/zlib.h"
#include "ifs/json.h"
#include "ifs/msgpack.h"
#include "ifs/URLSearchParams.h"
#include "ifs/FormData.h"
#include "ifs/querystring.h"
#include <string.h>

namespace fibjs {

result_t HttpClient_base::_new(obj_ptr<HttpClient_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);
    return _new(isolate->m_ctx, retVal, This);
}

result_t HttpClient_base::_new(SecureContext_base* context, obj_ptr<HttpClient_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpClient(context);
    return 0;
}

result_t HttpClient_base::_new(v8::Local<v8::Object> options, obj_ptr<HttpClient_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<HttpClient> hc = new HttpClient(nullptr);
    hc->wrap(This);

    result_t hr = hc->init(options);
    if (hr < 0)
        return hr;

    retVal = hc;

    return 0;
}

result_t HttpClient::init(v8::Local<v8::Object> options)
{
    result_t hr = tls_base::createSecureContext(options, false, m_context);
    if (hr < 0)
        return hr;

    hr = GetConfigValue(options, "keepAlive", m_keepAlive);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "timeout", m_timeout);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "enableCookie", m_enableCookie);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "autoRedirect", m_autoRedirect);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "enableEncoding", m_enableEncoding);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "maxHeadersCount", m_maxHeadersCount);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "maxHeaderSize", m_maxHeaderSize);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "maxBodySize", m_maxBodySize);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "userAgent", m_userAgent);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "poolSize", m_poolSize);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "poolTimeout", m_poolTimeout);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    Isolate* isolate = Isolate::current(options);
    v8::Local<v8::Value> v = options->Get(isolate->context(), isolate->NewString("proxyEnv")).FromMaybe(v8::Local<v8::Value>());
    if (!v.IsEmpty() && v->IsObject()) {
        hr = set_proxyEnv(v8::Local<v8::Object>::Cast(v));
        if (hr < 0)
            return hr;
    }

    return 0;
}

bool HttpClient::should_bypass_proxy(exlib::string hostname, int32_t port)
{
    // Always bypass localhost
    if (hostname == "localhost" || hostname == "127.0.0.1" || hostname == "::1")
        return true;

    // If no_proxy is empty, don't bypass
    if (m_no_proxy.empty())
        return false;

    // Check if hostname matches any pattern in no_proxy
    // no_proxy format: comma-separated list of hosts/domains/IPs
    // Examples: "localhost,.example.com,192.168.1.0/24"
    size_t pos = 0;
    exlib::string no_proxy = m_no_proxy;

    while (pos < no_proxy.length()) {
        size_t end = no_proxy.find(',', pos);
        if (end == exlib::string::npos)
            end = no_proxy.length();

        exlib::string pattern = no_proxy.substr(pos, end - pos);

        // Trim whitespace
        while (!pattern.empty() && (pattern[0] == ' ' || pattern[0] == '\t'))
            pattern = pattern.substr(1);
        while (!pattern.empty() && (pattern[pattern.length() - 1] == ' ' || pattern[pattern.length() - 1] == '\t'))
            pattern = pattern.substr(0, pattern.length() - 1);

        if (!pattern.empty()) {
            // Check for wildcard *
            if (pattern == "*")
                return true;

            // Check for port-specific pattern (e.g., "example.com:8080")
            size_t colonPos = pattern.find(':');
            if (colonPos != exlib::string::npos) {
                exlib::string patternHost = pattern.substr(0, colonPos);
                int32_t patternPort = atoi(pattern.substr(colonPos + 1).c_str());
                if (patternPort != port) {
                    pos = end + 1;
                    continue;
                }
                pattern = patternHost;
            }

            // Check for exact match
            if (hostname == pattern)
                return true;

            // Check for domain suffix match (e.g., ".example.com" matches "sub.example.com")
            if (pattern[0] == '.') {
                if (hostname.length() > pattern.length() &&
                    hostname.substr(hostname.length() - pattern.length()) == pattern)
                    return true;
            }

            // Check for wildcard domain match (e.g., "*.example.com")
            if (pattern.length() > 2 && pattern[0] == '*' && pattern[1] == '.') {
                exlib::string suffix = pattern.substr(1); // ".example.com"
                if (hostname.length() > suffix.length() &&
                    hostname.substr(hostname.length() - suffix.length()) == suffix)
                    return true;
                // Also match exact domain (e.g., "*.example.com" matches "example.com")
                if (hostname == pattern.substr(2))
                    return true;
            }

            // Check for suffix match without leading dot
            if (hostname.length() > pattern.length() &&
                hostname[hostname.length() - pattern.length() - 1] == '.' &&
                hostname.substr(hostname.length() - pattern.length()) == pattern)
                return true;
        }

        pos = end + 1;
    }

    return false;
}

result_t HttpClient::get_enableCookie(bool& retVal)
{
    retVal = m_enableCookie;
    return 0;
}

result_t HttpClient::set_enableCookie(bool newVal)
{
    m_enableCookie = newVal;
    return 0;
}

result_t HttpClient::get_keepAlive(bool& retVal)
{
    retVal = m_keepAlive;
    return 0;
}

result_t HttpClient::set_keepAlive(bool newVal)
{
    m_keepAlive = newVal;
    return 0;
}

result_t HttpClient::get_timeout(int32_t& retVal)
{
    retVal = m_timeout;
    return 0;
}

result_t HttpClient::set_timeout(int32_t newVal)
{
    m_timeout = newVal;
    return 0;
}

result_t HttpClient::get_cookies(obj_ptr<NArray>& retVal)
{
    retVal = m_cookies;
    return 0;
}

result_t HttpClient::get_autoRedirect(bool& retVal)
{
    retVal = m_autoRedirect;
    return 0;
}

result_t HttpClient::set_autoRedirect(bool newVal)
{
    m_autoRedirect = newVal;
    return 0;
}

result_t HttpClient::get_enableEncoding(bool& retVal)
{
    retVal = m_enableEncoding;
    return 0;
}

result_t HttpClient::set_enableEncoding(bool newVal)
{
    m_enableEncoding = newVal;
    return 0;
}

result_t HttpClient::get_maxHeadersCount(int32_t& retVal)
{
    retVal = m_maxHeadersCount;
    return 0;
}

result_t HttpClient::set_maxHeadersCount(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxHeadersCount = newVal;
    return 0;
}

result_t HttpClient::get_maxHeaderSize(int32_t& retVal)
{
    retVal = m_maxHeaderSize;
    return 0;
}

result_t HttpClient::set_maxHeaderSize(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxHeaderSize = newVal;
    return 0;
}

result_t HttpClient::get_maxChunkSize(int32_t& retVal)
{
    retVal = m_maxChunkSize;
    return 0;
}

result_t HttpClient::set_maxChunkSize(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxChunkSize = newVal;
    return 0;
}

result_t HttpClient::get_maxBodySize(int32_t& retVal)
{
    retVal = m_maxBodySize;
    return 0;
}

result_t HttpClient::set_maxBodySize(int32_t newVal)
{
    m_maxBodySize = newVal;
    return 0;
}

result_t HttpClient::get_userAgent(exlib::string& retVal)
{
    retVal = m_userAgent;
    return 0;
}

result_t HttpClient::set_userAgent(exlib::string newVal)
{
    m_userAgent = newVal;
    return 0;
}

result_t HttpClient::get_poolSize(int32_t& retVal)
{
    retVal = m_poolSize;
    return 0;
}

result_t HttpClient::set_poolSize(int32_t newVal)
{
    m_poolSize = newVal;
    return 0;
}

result_t HttpClient::get_poolTimeout(int32_t& retVal)
{
    retVal = m_poolTimeout;
    return 0;
}

result_t HttpClient::set_poolTimeout(int32_t newVal)
{
    m_poolTimeout = newVal;
    return 0;
}

result_t HttpClient::get_http_proxy(exlib::string& retVal)
{
    retVal = m_http_proxy;
    return 0;
}

result_t HttpClient::set_http_proxy(exlib::string newVal)
{
    if (newVal.empty()) {
        m_http_proxy.clear();
    } else {
        obj_ptr<Url> u = new Url();
        result_t hr;

        hr = u->parse(newVal);
        if (hr < 0)
            return hr;

        exlib::string protocol = u->protocol();
        if (protocol != "https:" && protocol != "http:" && protocol != "socks5:")
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol"));

        if (u->host().empty())
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown host"));

        m_http_proxy = newVal;
    }

    date_t d;

    d.now();
    d.add(m_poolTimeout, date_t::_MICROSECOND);
    clean_coon(d);

    return 0;
}

result_t HttpClient::get_https_proxy(exlib::string& retVal)
{
    retVal = m_https_proxy;
    return 0;
}

result_t HttpClient::set_https_proxy(exlib::string newVal)
{
    if (newVal.empty()) {
        m_https_proxy.clear();
    } else {
        obj_ptr<Url> u = new Url();
        result_t hr;

        hr = u->parse(newVal);
        if (hr < 0)
            return hr;

        exlib::string protocol = u->protocol();
        if (protocol != "https:" && protocol != "http:" && protocol != "socks5:")
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol"));

        if (u->host().empty())
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown host"));

        m_https_proxy = newVal;
    }

    date_t d;

    d.now();
    d.add(m_poolTimeout, date_t::_MICROSECOND);
    clean_coon(d);

    return 0;
}

result_t HttpClient::get_proxyEnv(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    retVal = v8::Object::New(isolate->m_isolate);

    if (!m_http_proxy.empty())
        retVal->Set(context, isolate->NewString("http_proxy"),
            isolate->NewString(m_http_proxy))
            .IsJust();

    if (!m_https_proxy.empty())
        retVal->Set(context, isolate->NewString("https_proxy"),
            isolate->NewString(m_https_proxy))
            .IsJust();

    if (!m_no_proxy.empty())
        retVal->Set(context, isolate->NewString("no_proxy"),
            isolate->NewString(m_no_proxy))
            .IsJust();

    return 0;
}

result_t HttpClient::set_proxyEnv(v8::Local<v8::Object> newVal)
{
    result_t hr;
    exlib::string http_proxy, https_proxy, no_proxy;

    // Check lowercase variants first (higher priority)
    hr = GetConfigValue(newVal, "http_proxy", http_proxy);
    if (hr == CALL_E_PARAMNOTOPTIONAL) {
        // Try uppercase variant
        hr = GetConfigValue(newVal, "HTTP_PROXY", http_proxy);
    }
    if (hr >= 0 || hr == CALL_E_PARAMNOTOPTIONAL) {
        if (hr >= 0) {
            hr = set_http_proxy(http_proxy);
            if (hr < 0)
                return hr;
        }
    } else {
        return hr;
    }

    hr = GetConfigValue(newVal, "https_proxy", https_proxy);
    if (hr == CALL_E_PARAMNOTOPTIONAL) {
        // Try uppercase variant
        hr = GetConfigValue(newVal, "HTTPS_PROXY", https_proxy);
    }
    if (hr >= 0 || hr == CALL_E_PARAMNOTOPTIONAL) {
        if (hr >= 0) {
            hr = set_https_proxy(https_proxy);
            if (hr < 0)
                return hr;
        }
    } else {
        return hr;
    }

    hr = GetConfigValue(newVal, "no_proxy", no_proxy);
    if (hr == CALL_E_PARAMNOTOPTIONAL) {
        // Try uppercase variant
        hr = GetConfigValue(newVal, "NO_PROXY", no_proxy);
    }
    if (hr >= 0 || hr == CALL_E_PARAMNOTOPTIONAL) {
        if (hr >= 0)
            m_no_proxy = no_proxy;
    } else {
        return hr;
    }

    return 0;
}

result_t HttpClient::update(HttpCookie_base* cookie)
{
    int32_t length, i;
    exlib::string str, str1;
    bool b = false, b1 = false;
    date_t t, t1;

    length = m_cookies->length();
    if (length == 0)
        return 0;

    for (i = 0; i < length; i++) {
        Variant v;

        m_cookies->_indexed_getter(i, v);
        obj_ptr<HttpCookie_base> hc = HttpCookie_base::getInstance(v.object());
        if (!hc)
            continue;

        hc->get_name(str);
        cookie->get_name(str1);
        if (str != str1)
            continue;

        hc->get_path(str);
        cookie->get_path(str1);
        if (str != str1)
            continue;

        hc->get_domain(str);
        cookie->get_domain(str1);
        if (str != str1)
            continue;

        hc->get_httpOnly(b);
        cookie->get_httpOnly(b1);
        if (b != b1)
            continue;

        hc->get_secure(b);
        cookie->get_secure(b1);
        if (b != b1)
            continue;

        hc->get_value(str);
        cookie->get_value(str1);
        if (str != str1)
            hc->set_value(str1);

        hc->get_expires(t);
        cookie->get_expires(t1);
        if (!t.empty() && !t1.empty() && t.diff(t1) != 0)
            hc->set_expires(t1);

        return -1;
    }
    return 0;
}

result_t HttpClient::update_cookies(exlib::string url, NArray* cookies)
{
    result_t hr;
    int32_t length, i;
    bool match = false;
    exlib::string domain;

    obj_ptr<Url> u = new Url();
    hr = u->parse(url);
    if (hr < 0)
        return hr;

    m_lock.lock();

    length = cookies->length();
    if (length == 0) {
        m_lock.unlock();
        return 0;
    }

    for (i = 0; i < length; i++) {
        Variant v;

        cookies->_indexed_getter(i, v);
        obj_ptr<HttpCookie_base> hc = HttpCookie_base::getInstance(v.object());
        if (!hc)
            continue;

        hc->match(url, match);
        if (!match)
            continue;

        hc->get_domain(domain);
        if (domain.empty())
            hc->set_domain(u->hostname());

        if (update(hc) == 0)
            m_cookies->append(hc);
    }

    m_lock.unlock();
    return 0;
}

result_t HttpClient::get_cookie(exlib::string url, exlib::string& retVal)
{
    result_t hr;
    int32_t length, i;
    bool match = false, secure = false;
    exlib::string s;
    exlib::string s1;
    exlib::string s2;
    date_t now;

    obj_ptr<Url> u = new Url();
    hr = u->parse(url);
    if (hr < 0)
        return hr;

    m_lock.lock();
    length = m_cookies->length();
    if (length == 0) {
        m_lock.unlock();
        return 0;
    }

    now.now();

    for (i = 0; i < length; i++) {
        Variant v;
        date_t date;

        m_cookies->_indexed_getter(i, v);
        obj_ptr<HttpCookie_base> hc = HttpCookie_base::getInstance(v.object());
        if (!hc)
            continue;

        hc->get_expires(date);
        if (!date.empty() && date.diff(now) < 0)
            continue;

        hc->get_secure(secure);
        if (secure && u->protocol() != "https:")
            continue;

        hc->match(url, match);
        if (match) {
            hc->get_name(s2);
            s1 = s2;
            s1 += '=';
            hc->get_value(s2);
            s1 += s2;
            s1 += "; ";
            s += s1;
        }
    }
    if (s.length() > 0)
        retVal = s.substr(0, s.length() - 2);

    m_lock.unlock();
    return 0;
}

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac, bool headerOnly)
{
    class asyncRequest : public AsyncState {
    public:
        asyncRequest(HttpClient* hc, Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body,
            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac, bool headerOnly)
            : AsyncState(ac)
            , m_hc(hc)
            , m_conn(conn)
            , m_req(req)
            , m_response_body(response_body)
            , m_retVal(retVal)
            , m_headerOnly(headerOnly)
        {
            next(send);

            exlib::string method;
            m_req->get_method(method);
            m_bNoBody = !qstricmp(method.c_str(), "head", 4);
        }

        ON_STATE(asyncRequest, send)
        {
            return m_req->sendTo(m_conn, v8::Local<v8::Object>(), next(recv));
        }

        ON_STATE(asyncRequest, recv)
        {
            m_response = new HttpResponse();
            m_retVal = m_response;

            m_response->m_message->m_bNoBody = m_bNoBody;

            if (m_response_body)
                m_response->set_body(m_response_body);

            m_response->set_maxHeadersCount(m_hc->m_maxHeadersCount);
            m_response->set_maxHeaderSize(m_hc->m_maxHeaderSize);
            m_response->set_maxChunkSize(m_hc->m_maxChunkSize);
            m_response->set_maxBodySize(m_hc->m_maxBodySize);
            m_bs = new BufferedStream(m_conn);
            m_bs->set_EOL("\r\n");

            return m_response->readHeader(m_bs, next(body));
        }

        ON_STATE(asyncRequest, body)
        {
            if (m_headerOnly) {
                int32_t status;

                result_t hr = m_response->get_statusCode(status);
                if (hr < 0)
                    return hr;

                if (!m_hc->m_autoRedirect || (status != 301 && status != 302 && status != 307))
                    return next();
            }

            return m_response->readBody(next(m_hc->m_enableEncoding ? unzip : close));
        }

        ON_STATE(asyncRequest, unzip)
        {
            exlib::string hdr;

            if (!m_response_body && m_response->firstHeader("Content-Encoding", hdr) != CALL_RETURN_NULL) {
                m_response->removeHeader("Content-Encoding");

                if (m_response->get_body(m_body) != CALL_RETURN_NULL && m_body) {
                    m_unzip = new MemoryStream();

                    if (hdr == "gzip")
                        return zlib_base::gunzipTo(m_body, m_unzip,
                            m_hc->m_maxBodySize, next(close));
                    else if (hdr == "deflate")
                        return zlib_base::inflateRawTo(m_body, m_unzip,
                            m_hc->m_maxBodySize, next(close));
                }
            }

            return next(close);
        }

        ON_STATE(asyncRequest, close)
        {
            if (m_unzip) {
                m_unzip->rewind();
                m_response->set_body(m_unzip);
            }

            return next();
        }

    private:
        obj_ptr<HttpClient> m_hc;
        Stream_base* m_conn;
        HttpRequest_base* m_req;
        obj_ptr<BufferedStream> m_bs;
        obj_ptr<MemoryStream> m_unzip;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<HttpResponse> m_response;
        obj_ptr<SeekableStream_base> m_response_body;
        obj_ptr<HttpResponse_base>& m_retVal;
        bool m_headerOnly;
        bool m_bNoBody;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, conn, req, response_body, retVal, ac, headerOnly))->post(0);
}

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request(conn, req, response_body, retVal, ac, false);
}

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpResponse_base>& retVal,
    AsyncEvent* ac)
{
    return request(conn, req, NULL, retVal, ac);
}

result_t HttpClient::request(HttpRequest::Options* o, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac, bool headerOnly)
{
    class asyncRequest : public AsyncState {
    public:
        asyncRequest(HttpClient* hc, HttpRequest::Options* o,
            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac, bool headerOnly)
            : AsyncState(ac)
            , m_o(o)
            , m_retVal(retVal)
            , m_hc(hc)
            , m_headerOnly(headerOnly)
        {
            m_o->u->toString(m_url);
            if (m_o->response_body)
                m_o->response_body->tell(m_response_pos);
            next(prepare);
        }

        ON_STATE(asyncRequest, prepare)
        {
            bool _domain = false;

            m_urls[m_url] = true;

            exlib::string path;
            exlib::string cookie;

            m_ssl = false;
            exlib::string protocol = m_o->u->protocol();
            exlib::string host = m_o->u->host();
            if (protocol == "https:") {
                m_ssl = true;
                m_connUrl = "ssl://";
            } else if (protocol == "http:") {
                if (host[0] == '/') {
                    _domain = true;
                    m_connUrl = "unix:";
                } else
                    m_connUrl = "tcp://";
            } else
                return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol"));

            if (host.empty())
                return CHECK_ERROR(Runtime::setError("HttpClient: unknown host"));

            m_connUrl.append(host);

            if (!_domain && m_o->u->port().empty())
                m_connUrl.append(m_ssl ? ":443" : ":80");

            m_req = new HttpRequest();

            m_req->set_method(m_o->method);

            exlib::string hostname = m_o->u->hostname();
            exlib::string portStr = m_o->u->port();
            int32_t port = portStr.empty() ? (m_ssl ? 443 : 80) : atoi(portStr.c_str());
            if (!m_hc->should_bypass_proxy(hostname, port)) {
                m_http_proxy = m_hc->m_http_proxy;
                if (m_ssl && !m_hc->m_https_proxy.empty())
                    m_http_proxy = m_hc->m_https_proxy;
            }

            if (m_http_proxy.empty() || m_ssl) {
                m_o->u->get_path(path);
                m_req->set_address(path);
            } else
                m_req->set_address(m_url);

            bool enableEncoding = false;
            m_hc->get_enableEncoding(enableEncoding);
            if (enableEncoding)
                m_req->appendHeader("Accept-Encoding", "gzip,deflate");

            bool enableCookie = false;
            m_hc->get_enableCookie(enableCookie);
            if (enableCookie) {
                m_hc->get_cookie(m_url, cookie);
                if (cookie.length() > 0)
                    m_req->appendHeader("Cookie", cookie);
            }

            m_req->set_keepAlive(m_o->keepAlive);

            if (m_o->headers)
                m_req->appendHeader(m_o->headers);

            exlib::string a = m_hc->agent();
            if (!a.empty()) {
                bool bCheck = false;
                m_req->hasHeader("User-Agent", bCheck);
                if (!bCheck)
                    m_req->appendHeader("User-Agent", a);
            }

            bool bHost = false;
            m_req->hasHeader("Host", bHost);
            if (!bHost)
                m_req->appendHeader("Host", host);

            if (m_o->body)
                m_req->set_body(m_o->body);

            if (m_ssl)
                m_sslhost = m_o->u->hostname();
            else
                m_sslhost.clear();

            m_reuse = false;
            if (m_hc->get_conn(m_connUrl, m_conn)) {
                m_reuse = true;
                return next(connected);
            }

            if (m_http_proxy.empty()) {
                if (m_ssl)
                    return tls_base::connect(m_connUrl, m_hc->m_context, m_hc->m_timeout, m_conn, next(connected));
                else
                    return net_base::connect(m_connUrl, m_hc->m_timeout, m_conn, next(connected));
            } else {
                bool socks = m_http_proxy[0] == 's';

                if (m_ssl && !socks) {
                    exlib::string host = m_connUrl.substr(6);
                    m_reqConn = new HttpRequest();

                    m_reqConn->set_method("CONNECT");
                    m_reqConn->set_address(host);
                    m_reqConn->appendHeader("Host", host);

                    exlib::string a = m_hc->agent();
                    if (!a.empty())
                        m_reqConn->appendHeader("User-Agent", a);
                }

                if (m_hc->get_conn(m_http_proxy, m_conn)) {
                    m_reuse = true;
                    return next(m_ssl ? ssl_connect : connected);
                }

                obj_ptr<Url> u = new Url();
                exlib::string connUrl;
                const char* def_port;

                u->parse(m_http_proxy);

                exlib::string protocol = u->protocol();
                if (protocol == "https:") {
                    connUrl = "ssl://";
                    def_port = "443";
                } else if (protocol == "http:") {
                    connUrl = "tcp://";
                    def_port = "80";
                } else if (protocol == "socks5:") {
                    connUrl = "tcp://";
                    def_port = "1080";
                }

                connUrl.append(u->host());

                if (u->port().empty())
                    connUrl.append(def_port);

                return net_base::connect(connUrl, m_hc->m_timeout, m_conn,
                    next(socks
                            ? socks_hello
                            : m_ssl
                            ? ssl_connect
                            : connected));
            }
        }

        ON_STATE(asyncRequest, socks_hello)
        {
            obj_ptr<Buffer_base> buf = new Buffer("\5\1\0", 3);

            m_conn.As<Socket_base>()->set_timeout(m_hc->m_timeout);
            return m_conn->writeBuffer(buf, next(socks_hello_response));
        }

        ON_STATE(asyncRequest, socks_hello_response)
        {
            return m_conn->readBuffer(2, m_buffer, next(socks_connect));
        }

        ON_STATE(asyncRequest, socks_connect)
        {
            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("HttpClient: connection reset by socks 5 server."));

            exlib::string strBuffer;

            m_buffer->toString(strBuffer);
            if (strBuffer.length() != 2 || strBuffer[0] != 5 || strBuffer[1] != 0)
                return CHECK_ERROR(Runtime::setError("HttpClient: socks 5 handshake failed."));

            obj_ptr<Url> u = new Url();
            u->parse(m_connUrl);

            sockaddr_in dst;
            sockaddr_in6 dst6;

            exlib::string hostname = u->hostname();
            if (!uv_ip4_addr(hostname.c_str(), 0, &dst)) {
                strBuffer.assign("\5\1\0\1", 4);
                strBuffer.append((char*)&dst.sin_addr, 4);
            } else if (!uv_ip6_addr(hostname.c_str(), 0, &dst6)) {
                strBuffer.assign("\5\1\0\4", 4);
                strBuffer.append((char*)&dst6.sin6_addr, 16);
            } else {
                strBuffer.assign("\5\1\0\3", 4);
                strBuffer.append(1, (char)hostname.length());
                strBuffer.append(hostname);
            }

            int16_t port = htons(atoi(u->port().c_str()));
            strBuffer.append((char*)&port, 2);

            obj_ptr<Buffer_base> buf = new Buffer(strBuffer.c_str(), strBuffer.length());
            return m_conn->writeBuffer(buf, next(socks_connect_req_5_bytes));
        }

        ON_STATE(asyncRequest, socks_connect_req_5_bytes)
        {
            return m_conn->readBuffer(5, m_buffer, next(socks_connect_res_5_bytes));
        }

        ON_STATE(asyncRequest, socks_connect_res_5_bytes)
        {
            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("HttpClient: connection reset by socks 5 server."));

            Buffer* buf = Buffer::Cast(m_buffer);
            const uint8_t* p = buf->data();
            if (buf->length() != 5 || p[0] != 5 || p[1] != 0)
                return CHECK_ERROR(Runtime::setError("HttpClient: socks 5 connect failed."));

            uint8_t len = 0;

            if (p[3] == 1) {
                len = 4 - 1 + 2; // ipv4
            } else if (p[3] == 4) {
                len = 16 - 1 + 2; // ipv6
            } else {
                len = (uint8_t)p[4] + 2; // domain
            }

            return m_conn->readBuffer(len, m_buffer, next(socks_connected));
        }

        ON_STATE(asyncRequest, socks_connected)
        {
            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("HttpClient: connection reset by socks 5 server."));

            // discard socks server response

            return next(m_ssl ? ssl_handshake : connected);
        }

        ON_STATE(asyncRequest, ssl_connect)
        {
            m_conn.As<Socket_base>()->set_timeout(m_hc->m_timeout);
            return m_hc->request(m_conn, m_reqConn, m_retVal, next(ssl_handshake));
        }

        ON_STATE(asyncRequest, ssl_connected)
        {
            int32_t status;
            result_t hr;

            hr = m_retVal->get_statusCode(status);
            if (hr < 0)
                return hr;

            if (status != 200) {
                exlib::string msg;

                m_retVal->get_statusMessage(msg);
                return CHECK_ERROR(Runtime::setError("HttpClient: " + msg));
            }

            m_reqConn.Release();
            m_retVal.Release();

            return next(ssl_handshake);
        }

        ON_STATE(asyncRequest, ssl_handshake)
        {
            obj_ptr<TLSSocket> ss = new TLSSocket();
            ss->init(m_hc->m_context);

            obj_ptr<Stream_base> conn = m_conn;
            m_conn = ss;

            return ss->connect(conn, m_sslhost, next(connected));
        }

        ON_STATE(asyncRequest, connected)
        {
            if (!m_ssl)
                m_conn.As<Socket_base>()->set_timeout(m_hc->m_timeout);

            return m_hc->request(m_conn, m_req, m_o->response_body, m_retVal, next(requested), m_headerOnly);
        }

        ON_STATE(asyncRequest, requested)
        {
            bool enableCookie;
            m_hc->get_enableCookie(enableCookie);
            if (enableCookie) {
                obj_ptr<NArray> cookies;
                m_retVal->get_cookies(cookies);
                m_hc->update_cookies(m_url, cookies);
            }

            bool upgrade;
            m_retVal->get_upgrade(upgrade);
            if (upgrade)
                return next(end);

            if (m_headerOnly)
                return next(end);

            bool keepAlive;
            m_retVal->get_keepAlive(keepAlive);
            if (keepAlive) {
                if (m_http_proxy.empty() || m_http_proxy[0] == 's' || !m_sslhost.empty())
                    m_hc->save_conn(m_connUrl, m_conn);
                else
                    m_hc->save_conn(m_http_proxy, m_conn);

                return next(end);
            }

            return m_conn->close(next(end));
        }

        ON_STATE(asyncRequest, end)
        {
            result_t hr;
            int32_t status;
            exlib::string location;
            obj_ptr<UrlObject_base> u1;

            hr = m_retVal->get_statusCode(status);
            if (hr < 0)
                return hr;

            if (!m_hc->m_autoRedirect || (status != 301 && status != 302 && status != 307))
                return next();

            hr = m_retVal->firstHeader("location", location);
            if (hr < 0)
                return hr;

            m_o->u->resolve(location, u1);
            m_o->u = u1.As<Url>();
            m_url.resize(0);
            m_o->u->toString(m_url);

            if (m_urls.find(m_url) != m_urls.end())
                return CHECK_ERROR(Runtime::setTypeError("HttpClient: redirect cycle"));

            if (m_o->response_body)
                m_o->response_body->seek(m_response_pos, fs_base::C_SEEK_SET);

            return next(prepare);
        }

        virtual int32_t error(int32_t v)
        {
            if (m_reuse && (at(ssl_connect) || at(connected))) {
                m_reuse = false;
                next(prepare);
                return 0;
            }

            return v;
        }

    private:
        obj_ptr<HttpRequest::Options> m_o;
        exlib::string m_url;
        exlib::string m_sslhost;
        bool m_ssl;
        exlib::string m_http_proxy;
        int64_t m_response_pos;
        obj_ptr<HttpResponse_base>& m_retVal;
        std::unordered_map<exlib::string, bool> m_urls;
        obj_ptr<Stream_base> m_conn;
        obj_ptr<HttpRequest> m_req;
        obj_ptr<HttpRequest> m_reqConn;
        exlib::string m_connUrl;
        obj_ptr<HttpClient> m_hc;
        bool m_headerOnly;
        obj_ptr<Buffer_base> m_buffer;
        bool m_reuse;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, o, retVal, ac, headerOnly))->post(0);
}

result_t HttpClient::request(exlib::string method, exlib::string url, SeekableStream_base* body,
    SeekableStream_base* response_body, Headers_base* headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest::Options> o = new HttpRequest::Options();
    o->method = method;
    o->keepAlive = m_keepAlive;
    o->headers = headers;
    o->body = body;
    o->response_body = response_body;

    obj_ptr<Url> u = new Url();
    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;
    o->u = u;

    return request(o.get(), retVal, ac, false);
}

result_t HttpClient::get_request_opts(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    ac->m_ctx.resize(1);

    obj_ptr<HttpRequest::Options> o = new HttpRequest::Options();
    result_t hr = o->from_opts(method, opts, true);
    if (hr < 0)
        return hr;

    hr = o->resolve_url(url, opts);
    if (hr < 0)
        return hr;

    o->apply_keepalive_default(m_keepAlive);

    ac->m_ctx[0] = o;
    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t HttpClient::request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac, bool headerOnly)
{
    if (ac->isSync())
        return get_request_opts(method, url, opts, ac);

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return request(o.get(), retVal, ac, headerOnly);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request(method, url, opts, retVal, ac, false);
}

result_t HttpClient::request(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("GET", url, opts, retVal, ac);
}

result_t HttpClient::request(v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("GET", "", opts, retVal, ac);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("GET", url, opts, retVal, ac);
}

result_t HttpClient::post(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("POST", url, opts, retVal, ac);
}

result_t HttpClient::del(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("DELETE", url, opts, retVal, ac);
}

result_t HttpClient::put(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("PUT", url, opts, retVal, ac);
}

result_t HttpClient::patch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("PATCH", url, opts, retVal, ac);
}

result_t HttpClient::head(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("HEAD", url, opts, retVal, ac);
}

// Async state machine shared by both fetch(url) and fetch(request).
class asyncFetch : public AsyncState {
public:
    asyncFetch(HttpClient* hc, obj_ptr<HttpRequest::Options> o,
        obj_ptr<WebResponse_base>& retVal, AsyncEvent* ac)
        : AsyncState(ac)
        , m_hc(hc)
        , m_o(o)
        , m_retVal(retVal)
    {
        next(do_request);
    }

    virtual int32_t error(int32_t v) override
    {
        // Per Fetch spec, all fetch errors are TypeErrors
        exlib::string msg = (v == CALL_E_EXCEPTION)
            ? Runtime::errMessage()
            : getResultMessage(v);
        Runtime::setTypeError(msg);
        return CALL_E_EXCEPTION;
    }

    ON_STATE(asyncFetch, do_request)
    {
        return m_hc->request(m_o.get(), m_httpResp, next(do_wrap), false);
    }

    ON_STATE(asyncFetch, do_wrap)
    {
        exlib::string finalUrl;
        m_o->u->toString(finalUrl);

        obj_ptr<WebResponse> resp = new WebResponse();
        result_t hr = resp->initFromHttpResponse(m_httpResp, finalUrl, false);
        if (hr < 0)
            return hr;

        m_retVal = resp;
        return next();
    }

private:
    obj_ptr<HttpClient> m_hc;
    obj_ptr<HttpRequest::Options> m_o;
    obj_ptr<HttpResponse_base> m_httpResp;
    obj_ptr<WebResponse_base>& m_retVal;
};

result_t HttpClient::fetch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<WebResponse_base>& retVal, AsyncEvent* ac)
{
    // Sync phase: parse v8::Local opts into ac->m_ctx as a single Options object.
    if (ac->isSync())
        return get_request_opts("GET", url, opts, ac);

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return (new asyncFetch(this, o, retVal, ac))->post(0);
}

result_t HttpClient::fetch(HttpRequest_base* request, v8::Local<v8::Object> opts,
    obj_ptr<WebResponse_base>& retVal, AsyncEvent* ac)
{
    // Sync phase: extract url/method from the request, apply opts overrides.
    if (ac->isSync()) {
        exlib::string req_method;
        request->get_method(req_method);

        obj_ptr<HttpRequest::Options> o = new HttpRequest::Options();
        result_t hr = o->from_opts(req_method, opts, true, true);
        if (hr < 0)
            return hr;

        hr = o->apply_from_request(request);
        if (hr < 0)
            return hr;

        o->apply_keepalive_default(m_keepAlive);

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = o;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return (new asyncFetch(this, o, retVal, ac))->post(0);
}
}
