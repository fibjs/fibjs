/*
 * HttpClient.cpp
 *
 *  Created on: Aug 12, 2016
 */

#include "object.h"
#include "HttpClient.h"

#include "HttpMessage.h"
#include "Buffer.h"
#include "Blob.h"
#include "MemoryStream.h"
#include "HttpRequest.h"
#include "TLSSocket.h"
#include "BufferedStream.h"
#include "ChunkedStream.h"
#include "RangeStream.h"
#undef _close // Stat.h (pulled via RangeStream.h) defines _close=close which
              // breaks ON_STATE(asyncRequest, close) macro expansion
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
#include <memory>

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

// BodyStream wraps the transport stream chain (ChunkedStream / RangeStream +
// BufferedStream + Socket) and keeps the underlying socket alive until the
// caller explicitly closes the body.  Stage 1: close-only (always tears down
// the TCP connection on close regardless of whether EOF was reached).
class BodyStream : public AsyncStream<Stream_base> {
public:
    using CleanupFn = std::function<void(bool eof_complete)>;

    BodyStream(Stream_base* inner, obj_ptr<Stream_base> socket)
        : m_inner(inner)
        , m_socket(socket)
    {
    }

    void setCleanup(CleanupFn fn) { m_cleanup = std::move(fn); }

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        if (!m_inner)
            return CALL_RETURN_NULL; // already closed
        if (!m_cleanup)
            return m_inner->readBuffer(bytes, retVal, ac);

        // When cleanup is set, intercept EOF to trigger connection lifecycle.
        class asyncRead : public AsyncState {
        public:
            asyncRead(BodyStream* ps, int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
                : AsyncState(ac)
                , m_bs(ps)
                , m_bytes(bytes)
                , m_retVal(retVal)
            {
                next(start);
            }

            ON_STATE(asyncRead, start)
            {
                return m_bs->m_inner->readBuffer(m_bytes, m_retVal, next(done));
            }

            ON_STATE(asyncRead, done)
            {
                if (n == CALL_RETURN_NULL && m_bs->m_cleanup) {
                    // Release socket before save_conn to prevent double-close.
                    m_bs->m_socket.Release();
                    auto fn = std::move(m_bs->m_cleanup);
                    fn(true); // eof_complete: return connection to pool
                }
                return next(n);
            }

        private:
            obj_ptr<BodyStream> m_bs;
            int32_t m_bytes;
            obj_ptr<Buffer_base>& m_retVal;
        };

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);
        return (new asyncRead(this, bytes, retVal, ac))->post(0);
    }

    virtual result_t writeBuffer(Buffer_base* data, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t flush(AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t close(AsyncEvent* ac)
    {
        m_cleanup = nullptr; // abandon: don't return to pool
        obj_ptr<Stream_base> socket = m_socket;
        m_socket.Release();
        m_inner.Release();
        if (socket)
            return socket->close(ac);
        return 0;
    }

private:
    obj_ptr<Stream_base> m_inner;
    obj_ptr<Stream_base> m_socket; // kept alive until body is consumed or closed
    CleanupFn m_cleanup;
};

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac, bool)
{
    class asyncRequest : public AsyncState {
    public:
        asyncRequest(HttpClient* hc, Stream_base* conn, HttpRequest_base* req,
            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_hc(hc)
            , m_conn(conn)
            , m_req(req)
            , m_retVal(retVal)
        {
            next(send);

            exlib::string method;
            m_req->get_method(method);
            // HEAD: response MUST NOT contain a body (RFC 9110 §9.3.2)
            // CONNECT: decision depends on response status code, deferred to body state
            m_bNoBody = !qstricmp(method.c_str(), "head", 4);
            m_bConnect = !qstricmp(method.c_str(), "connect", 7);
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
            // Determine no-body from response status code (RFC 9110 §6.3.1, §9.3.6):
            //   1xx informational, 204 No Content, 304 Not Modified: MUST NOT have body
            //   CONNECT 2xx: tunnel established, HTTP framing ends
            if (!m_bNoBody) {
                int32_t status = m_response->m_statusCode;
                m_bNoBody = (status >= 100 && status < 200)
                    || status == 204
                    || status == 304
                    || (m_bConnect && status >= 200 && status < 300);
            }

            if (!m_bNoBody) {
                int64_t cl = m_response->m_message->m_contentLength;
                bool chunked = m_response->m_message->m_bChunked;

                if (chunked || cl != 0) {
                    obj_ptr<Stream_base> stm = (Stream_base*)m_bs.get();

                    if (chunked) {
                        stm = new ChunkedStream(m_bs, m_hc->m_maxChunkSize, m_hc->m_maxBodySize);
                    } else if (cl > 0) {
                        stm = new RangeStream(stm, cl);
                    }
                    // connection-close (cl == -1): read until EOF, no wrapper

                    obj_ptr<BodyStream> bs = new BodyStream(stm, m_conn);

                    // Wire cleanup so EOF returns the connection to the pool.
                    // connUrl is extracted from the Host header; falls back to no pooling.
                    exlib::string host;
                    if (m_req->firstHeader("Host", host) == 0 && !host.empty()) {
                        auto hc = m_hc;
                        auto conn = m_conn;
                        bool keepAlive;
                        m_response->get_keepAlive(keepAlive);
                        if (keepAlive) {
                            bs->setCleanup([hc, host, conn](bool eof) {
                                if (eof)
                                    hc->save_conn(host, conn);
                            });
                        }
                    }

                    m_response->m_message->m_bodyStream = bs;
                }
            }
            return next();
        }

    private:
        obj_ptr<HttpClient> m_hc;
        obj_ptr<Stream_base> m_conn;
        HttpRequest_base* m_req;
        obj_ptr<BufferedStream> m_bs;
        obj_ptr<Stream_base> m_body;
        obj_ptr<HttpResponse> m_response;
        obj_ptr<HttpResponse_base>& m_retVal;
        bool m_bNoBody;
        bool m_bConnect;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, conn, req, retVal, ac))->post(0);
}

// Public virtual implementation — delegates to the streaming overload.
result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request(conn, req, retVal, ac, true);
}

result_t HttpClient::request(HttpRequest::Options* o, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    class asyncRequest : public AsyncState {
    public:
        ~asyncRequest()
        {
            if (m_o && m_o->signal)
                m_o->abort_signal()->clearAbort();
        }

        asyncRequest(HttpClient* hc, HttpRequest::Options* o,
            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_o(o)
            , m_retVal(retVal)
            , m_hc(hc)
        {
            m_o->u->toString(m_url);
            // D.3: abort callback closes the socket to cancel any pending
            // operation (connect, read, or write).  We capture both the
            // shared pconn slot AND a raw 'this' so we can fall back to
            // m_conn during the connect phase when *pconn is still null.
            // Capturing 'this' is safe because ~asyncRequest calls
            // clearAbort() before member destruction.
            if (m_o->signal) {
                auto conn = m_pconn;
                auto pthis = this;
                m_o->abort_signal()->addAbortCallback([conn, pthis]() {
                    obj_ptr<Stream_base> c = *conn;
                    if (!c)
                        c = pthis->m_conn;
                    if (c) {
                        Socket_base* sock = Socket_base::getInstance(c);
                        if (sock) {
                            sock->abort();
                        } else {
                            TLSSocket* tls = (TLSSocket*)TLSSocket_base::getInstance(c);
                            if (tls && tls->m_stream) {
                                sock = Socket_base::getInstance(tls->m_stream);
                                if (sock)
                                    sock->abort();
                            }
                        }
                    }
                });
            }

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
                if (m_ssl) {
                    // Split into TCP connect + TLS handshake so m_conn holds
                    // the TCP socket during connect, enabling abort callbacks
                    // to find and cancel the pending connection.
                    exlib::string tcpUrl = "tcp://";
                    tcpUrl.append(m_connUrl.substr(6));
                    return net_base::connect(tcpUrl, m_hc->m_timeout, m_conn, next(ssl_handshake));
                } else
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
            *m_pconn = m_conn; // sync slot during TLS handshake

            return ss->connect(conn, m_sslhost, next(connected));
        }

        ON_STATE(asyncRequest, connected)
        {
            *m_pconn = m_conn; // sync slot so abort callback holds the live connection
            if (!m_ssl)
                m_conn.As<Socket_base>()->set_timeout(m_hc->m_timeout);

            return m_hc->request(m_conn, m_req, m_retVal, next(requested));
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

            // Inner asyncRequest always returns a BodyStream (streaming path).
            // Wire up EOF-triggered connection pool return on the BodyStream.
            bool keepAlive;
            m_retVal->get_keepAlive(keepAlive);
            if (keepAlive) {
                obj_ptr<Stream_base> bodyStream;
                if (m_retVal->get_body(bodyStream) == 0 && bodyStream) {
                    BodyStream* bs = static_cast<BodyStream*>(bodyStream.get());
                    auto hc = m_hc;
                    // Proxy connections are pooled under proxy URL; direct under connUrl.
                    exlib::string connUrl = (!m_http_proxy.empty() && m_http_proxy[0] != 's' && m_sslhost.empty())
                        ? m_http_proxy : m_connUrl;
                    auto conn = m_conn;
                    bs->setCleanup([hc, connUrl, conn](bool eof) {
                        if (eof)
                            hc->save_conn(connUrl, conn);
                    });
                } else {
                    // Empty body (e.g. Content-Length: 0): return connection immediately.
                    exlib::string connUrl = (!m_http_proxy.empty() && m_http_proxy[0] != 's' && m_sslhost.empty())
                        ? m_http_proxy : m_connUrl;
                    m_hc->save_conn(connUrl, m_conn);
                }
            }
            return next(end);
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

            bool isRedirect = (status == 301 || status == 302 || status == 303
                || status == 307 || status == 308);
            if (!isRedirect)
                return next();

            // Fetch API redirect mode takes precedence over HttpClient autoRedirect
            if (m_o->redirect == "error")
                return CHECK_ERROR(Runtime::setTypeError("fetch: redirect not allowed"));
            if (m_o->redirect == "manual")
                return next(); // return redirect response as-is
            if (!m_hc->m_autoRedirect)
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

            // 303: per spec force GET and clear request body
            if (status == 303) {
                m_o->method = "GET";
                m_o->body.Release();
            }

            m_o->redirected = true;

            return next(prepare);
        }

        virtual int32_t error(int32_t v)
        {
            if (m_reuse && (at(ssl_connect) || at(connected))) {
                m_reuse = false;
                next(prepare);
                return 0;
            }

            // D.3: if abort was requested, convert the connection error to TypeError
            if (m_o->signal) {
                bool aborted;
                m_o->signal->get_aborted(aborted);
                if (aborted) {
                    Runtime::setTypeError("AbortError");
                    return CALL_E_EXCEPTION;
                }
            }

            return v;
        }

    private:
        obj_ptr<HttpRequest::Options> m_o;
        exlib::string m_url;
        exlib::string m_sslhost;
        bool m_ssl;
        exlib::string m_http_proxy;
        obj_ptr<HttpResponse_base>& m_retVal;
        std::unordered_map<exlib::string, bool> m_urls;
        obj_ptr<Stream_base> m_conn;
        std::shared_ptr<obj_ptr<Stream_base>> m_pconn = std::make_shared<obj_ptr<Stream_base>>();
        obj_ptr<HttpRequest> m_req;
        obj_ptr<HttpRequest> m_reqConn;
        exlib::string m_connUrl;
        obj_ptr<HttpClient> m_hc;
        obj_ptr<Buffer_base> m_buffer;
        bool m_reuse;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, o, retVal, ac))->post(0);
}

result_t HttpClient::request(exlib::string method, exlib::string url, SeekableStream_base* body,
    Headers_base* headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest::Options> o = new HttpRequest::Options();
    o->method = method;
    o->keepAlive = m_keepAlive;
    o->headers = headers;
    o->body = body;

    obj_ptr<Url> u = new Url();
    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;
    o->u = u;

    return request(o.get(), retVal, ac);
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
    return request(o.get(), retVal, ac);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return get_request_opts(method, url, opts, ac);

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return request(o.get(), retVal, ac);
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
        obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
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
        // D.2: reject immediately if signal is already aborted
        if (m_o->signal) {
            bool aborted;
            m_o->signal->get_aborted(aborted);
            if (aborted) {
                Runtime::setTypeError("AbortError");
                return CALL_E_EXCEPTION;
            }
        }
        return m_hc->request(m_o.get(), m_httpResp, next(do_wrap));
    }

    ON_STATE(asyncFetch, do_wrap)
    {
        exlib::string finalUrl;
        m_o->u->toString(finalUrl);

        HttpResponse* resp = (HttpResponse*)m_httpResp.get();
        resp->m_fetchUrl = finalUrl;
        resp->m_redirected = m_o->redirected;
        resp->m_fetchType = "basic";

        m_retVal = m_httpResp;
        return next();
    }

private:
    obj_ptr<HttpClient> m_hc;
    obj_ptr<HttpRequest::Options> m_o;
    obj_ptr<HttpResponse_base> m_httpResp;
    obj_ptr<HttpResponse_base>& m_retVal;
};

result_t HttpClient::fetch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    // Sync phase: parse v8::Local opts into ac->m_ctx as a single Options object.
    if (ac->isSync())
        return get_request_opts("GET", url, opts, ac);

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return (new asyncFetch(this, o, retVal, ac))->post(0);
}

result_t HttpClient::fetch(HttpRequest_base* request, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
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
