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
#include "FileStream.h"
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
#include "ifs/fs.h"
#include "ifs/mime.h"
#include "ifs/url.h"
#include "ifs/URLSearchParams.h"
#include "ifs/FormData.h"
#include "ifs/querystring.h"
#include <string.h>
#include <memory>
#include <stdio.h>
#include "Http2Session.h"
#include "Http2Stream.h"
#include "SecureContext.h"
#include "Fiber.h"
#include "Event.h"
#include <openssl/ssl.h>



namespace fibjs {

static result_t build_file_fetch_response(HttpRequest::Options* o, obj_ptr<HttpResponse_base>& retVal)
{
    exlib::string path;
    result_t hr = url_base::fileURLToPath(o->u->href(), v8::Local<v8::Object>(), path);
    if (hr < 0)
        return hr;

    exlib::string method = o->method;
    for (size_t i = 0; i < method.length(); i++)
        method[i] = (char)toupper((unsigned char)method[i]);

    if (method != "GET" && method != "HEAD")
        return CHECK_ERROR(Runtime::setError(kTypeError, "fetch(file:): only GET and HEAD are supported"));

    FileStream file;
    hr = file.open(path, "r");
    if (hr < 0)
        return hr;

    obj_ptr<HttpResponse> resp = new HttpResponse();
    resp->m_fetchUrl = o->u->href();
    resp->m_redirected = o->redirected;
    resp->m_fetchType = "basic";
    resp->set_statusCode(200);

    exlib::string mimeType;
    if (mime_base::getType(path, mimeType) >= 0 && !mimeType.empty())
        resp->setHeader("Content-Type", mimeType);

    int64_t size = 0;
    if (file.size(size) >= 0)
        resp->setHeader("Content-Length", std::to_string(size).c_str());

    if (method == "HEAD") {
        file.close();
        retVal = resp;
        return 0;
    }

    obj_ptr<Buffer_base> buf;
    hr = file.cc_readAll(buf);
    file.cc_close();
    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_NULL || !buf)
        buf = new Buffer();

    obj_ptr<MemoryStream> body = new MemoryStream();
    hr = body->writeBuffer(buf, nullptr);
    if (hr < 0)
        return hr;

    body->rewind();
    resp->set_body(body);

    retVal = resp;
    return 0;
}

// Best-effort stale-socket probe for pooled connections.
// Non-blocking: if uncertain, keep the connection and let normal retry handle it.
static inline bool http_conn_looks_alive(Stream_base* conn)
{
    if (!conn)
        return false;

    Socket_base* sock = Socket_base::getInstance(conn);
    if (!sock) {
        TLSSocket* tls = (TLSSocket*)TLSSocket_base::getInstance(conn);
        if (tls && tls->m_stream)
            sock = Socket_base::getInstance(tls->m_stream);
    }

    if (!sock)
        return false;

    bool alive;
    result_t hr = sock->isAlive(alive);
    if (hr < 0)
        return false;

    return alive;
}

LruCache<obj_ptr<Http2Session>> HttpClient::s_h2sessions;
std::unordered_map<exlib::string, HttpClient::H2PendingEntry*> HttpClient::s_h2_pending;
exlib::spinlock HttpClient::s_h2_pending_lock;

static inline exlib::string build_h2_pool_key(const exlib::string& connUrl,
    const exlib::string& proxyUrl,
    const exlib::string& sslHost,
    SecureContext_base* context)
{
    exlib::string origin = "https://";
    if (connUrl.length() > 6)
        origin.append(connUrl.substr(6));

    exlib::string key = origin;
    key.append("|proxy=");
    key.append(proxyUrl.empty() ? "direct" : proxyUrl);
    key.append("|sni=");
    key.append(sslHost);

    SecureContext* ctx = static_cast<SecureContext*>(context);
    key.append("|alpn=");
    key.append((ctx && ctx->hasAlpn()) ? "custom" : "auto");

    // Use content-based fingerprint of client cert and CA instead of the
    // raw pointer value so that different Agent instances with identical
    // TLS configuration share the same H2 session pool entry.
    key.append("|cert=");
    if (ctx) {
        obj_ptr<X509Certificate_base> cert;
        if (ctx->get_cert(cert) == 0 && cert) {
            exlib::string fp;
            cert->get_fingerprint256(fp);
            key.append(fp);
        } else {
            key.append("none");
        }
    } else {
        key.append("none");
    }
    key.append("|ca=");
    if (ctx) {
        obj_ptr<X509Certificate_base> ca;
        if (ctx->get_ca(ca) == 0 && ca) {
            exlib::string fp;
            ca->get_fingerprint256(fp);
            key.append(fp);
        } else {
            key.append("default");
        }
    } else {
        key.append("default");
    }

    return key;
}

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

    hr = GetConfigValue(options, "enableH2", m_enableH2);
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

    hr = GetConfigValue(options, "poolTimeout", m_poolTimeout);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(options, "maxFreeSockets", m_maxFreeSockets);
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
                if (hostname.length() > pattern.length() && hostname.substr(hostname.length() - pattern.length()) == pattern)
                    return true;
            }

            // Check for wildcard domain match (e.g., "*.example.com")
            if (pattern.length() > 2 && pattern[0] == '*' && pattern[1] == '.') {
                exlib::string suffix = pattern.substr(1); // ".example.com"
                if (hostname.length() > suffix.length() && hostname.substr(hostname.length() - suffix.length()) == suffix)
                    return true;
                // Also match exact domain (e.g., "*.example.com" matches "example.com")
                if (hostname == pattern.substr(2))
                    return true;
            }

            // Check for suffix match without leading dot
            if (hostname.length() > pattern.length() && hostname[hostname.length() - pattern.length() - 1] == '.' && hostname.substr(hostname.length() - pattern.length()) == pattern)
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

result_t HttpClient::get_enableH2(bool& retVal)
{
    retVal = m_enableH2;
    return 0;
}

result_t HttpClient::set_enableH2(bool newVal)
{
    m_enableH2 = newVal;
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
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol: '%s'.", protocol.c_str()));

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
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol: '%s'.", protocol.c_str()));

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

void HttpClient::setEnvProxy()
{
    char buf[4096];
    size_t sz;
    exlib::string http_proxy, https_proxy;

    sz = sizeof(buf);
    if (uv_os_getenv("http_proxy", buf, &sz) == 0)
        http_proxy = buf;
    else {
        sz = sizeof(buf);
        if (uv_os_getenv("HTTP_PROXY", buf, &sz) == 0)
            http_proxy = buf;
    }

    sz = sizeof(buf);
    if (uv_os_getenv("https_proxy", buf, &sz) == 0)
        https_proxy = buf;
    else {
        sz = sizeof(buf);
        if (uv_os_getenv("HTTPS_PROXY", buf, &sz) == 0)
            https_proxy = buf;
    }

    sz = sizeof(buf);
    if (uv_os_getenv("no_proxy", buf, &sz) == 0)
        m_no_proxy = buf;
    else {
        sz = sizeof(buf);
        if (uv_os_getenv("NO_PROXY", buf, &sz) == 0)
            m_no_proxy = buf;
    }

    set_http_proxy(http_proxy);
    set_https_proxy(https_proxy);
}

result_t HttpClient::get_maxSockets(int32_t& retVal)
{
    retVal = m_maxSockets;
    return 0;
}

result_t HttpClient::set_maxSockets(int32_t newVal)
{
    m_maxSockets = newVal;
    return 0;
}

result_t HttpClient::get_maxTotalSockets(int32_t& retVal)
{
    retVal = m_maxTotalSockets;
    return 0;
}

result_t HttpClient::set_maxTotalSockets(int32_t newVal)
{
    m_maxTotalSockets = newVal;
    return 0;
}

result_t HttpClient::get_maxFreeSockets(int32_t& retVal)
{
    retVal = m_maxFreeSockets;
    return 0;
}

result_t HttpClient::set_maxFreeSockets(int32_t newVal)
{
    m_maxFreeSockets = newVal;
    return 0;
}

result_t HttpClient::get_defaultPort(int32_t& retVal)
{
    retVal = m_defaultPort;
    return 0;
}

result_t HttpClient::set_defaultPort(int32_t newVal)
{
    m_defaultPort = newVal;
    return 0;
}

result_t HttpClient::get_protocol(exlib::string& retVal)
{
    retVal = m_protocol;
    return 0;
}

result_t HttpClient::set_protocol(exlib::string newVal)
{
    m_protocol = newVal;
    return 0;
}

result_t HttpClient::get_freeSockets(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    retVal = v8::Object::New(isolate->m_isolate);
    return 0;
}

result_t HttpClient::get_sockets(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    retVal = v8::Object::New(isolate->m_isolate);
    return 0;
}

result_t HttpClient::get_totalSocketCount(int32_t& retVal)
{
    retVal = 0;
    return 0;
}

result_t HttpClient::getName(v8::Local<v8::Object> options, exlib::string& retVal)
{
    Isolate* isolate = holder();
    exlib::string host, localAddress;
    int32_t port = m_defaultPort;

    GetConfigValue(options, "host", host);
    GetConfigValue(options, "port", port);
    GetConfigValue(options, "localAddress", localAddress);

    retVal = m_protocol;
    retVal.append("//");
    retVal.append(host);
    retVal.append(":");
    retVal.append(std::to_string(port));
    if (!localAddress.empty()) {
        retVal.append(":");
        retVal.append(localAddress);
    }

    return 0;
}

result_t HttpClient::destroy()
{
    m_lock.lock();
    m_conns.clear();
    m_lock.unlock();

    s_h2sessions.forEach([](exlib::string key, obj_ptr<Http2Session>& session) {
        session->destroy();
    });
    s_h2sessions.clear();

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

    void setAbortSignal(AbortSignal* signal)
    {
        if (!signal)
            return;
        m_abort_signal = signal;
        obj_ptr<Stream_base> socket = m_socket;
        signal->addAbortCallback([socket]() {
            HttpRequest::abort_socket(socket);
        });
    }

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
        if (m_abort_signal) {
            m_abort_signal->clearAbort();
            m_abort_signal = nullptr;
        }
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
    AbortSignal* m_abort_signal = nullptr;
};

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpMessage_base>* retVal, AsyncEvent* ac, bool)
{
    class asyncRequest : public AsyncState {
    public:
        asyncRequest(HttpClient* hc, Stream_base* conn, HttpRequest_base* req,
            obj_ptr<HttpMessage_base>* retVal, AsyncEvent* ac)
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
            if (m_retVal)
                *m_retVal = m_response;

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
                        if (!m_response->m_message->m_trailers)
                            m_response->m_message->m_trailers = new Headers();
                        m_response->m_message->m_trailers->m_lowercase_keys = true;
                        stm = new ChunkedStream(m_bs, m_hc->m_maxChunkSize, m_hc->m_maxBodySize, m_response->m_message->m_trailers);
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

            if (!m_retVal) {
                static_cast<HttpRequest*>(m_req.get())->_set_response(m_response);
                Variant resp_var = m_response;
                static_cast<HttpRequest*>(m_req.get())->_emit("response", &resp_var, 1);
            }
            return next();
        }

    private:
        obj_ptr<HttpClient> m_hc;
        obj_ptr<Stream_base> m_conn;
        obj_ptr<HttpRequest_base> m_req;
        obj_ptr<BufferedStream> m_bs;
        obj_ptr<Stream_base> m_body;
        obj_ptr<HttpMessage_base>* m_retVal;
        obj_ptr<HttpResponse> m_response;
        bool m_bNoBody;
        bool m_bConnect;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, conn, req, retVal, ac))->post(0);
}

// Public virtual implementation — delegates to the streaming overload.
result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return request(conn, req, retVal);
    return 0;
}

class asyncRequest : public AsyncState {
public:
    ~asyncRequest()
    {
        // Only clear abort callbacks on error paths; on success,
        // complete() already cleared them before do_wrap could
        // register new callbacks on the body stream.
        if (!m_completed && m_o && m_o->signal)
            m_o->abort_signal()->clearAbort();
    }

    asyncRequest(HttpRequest::Options* o,
        obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
        : AsyncState(ac)
        , m_o(o)
        , m_retVal(m_tempRetVal)
        , m_respRetVal(&retVal)
        , m_req_holder(o->req_holder)
        , m_hc((HttpClient*)(HttpClient_base*)o->agent)
    {
        init();
    }

    asyncRequest(HttpRequest::Options* o, AsyncEvent* ac)
        : AsyncState(ac)
        , m_o(o)
        , m_retVal(m_tempRetVal)
        , m_req_holder(o->req_holder)
        , m_hc((HttpClient*)(HttpClient_base*)o->agent)
    {
        init();
    }

    void init()
    {
        m_o->u->toString(m_url);

        if (m_o->signal) {
            // D.3: abort callback calls req->abort() to close the socket,
            // cancelling any pending operation (connect, read, or write).
            // For HTTP/2, it also resets the H2 stream.
            bool aborted;
            m_o->signal->get_aborted(aborted);
            if (!aborted) {
                auto pthis = this;
                m_o->abort_signal()->addAbortCallback([pthis]() {
                    if (pthis->m_h2stream)
                        pthis->m_h2stream->onClose(NGHTTP2_CANCEL);
                    if (pthis->m_req)
                        pthis->m_req->abort();
                });
            }
        }

        if (m_o->is_async && m_o->req)
            next(wait_end);
        else
            next(prepare);
    }

    ON_STATE(asyncRequest, wait_end)
    {
        m_o->req->m_asyncState = this;
        next(prepare);
        return CALL_E_PENDDING;
    }

    ON_STATE(asyncRequest, prepare)
    {
        if (!m_req) {
            if (m_o->req)
                m_req = m_o->req;
            else
                m_req = new HttpRequest();
        } else
            m_req->clear();

        // Reject immediately if signal is already aborted
        if (m_o->signal) {
            bool aborted;
            m_o->signal->get_aborted(aborted);
            if (aborted)
                return CHECK_ERROR(Runtime::setError("AbortError"));
        }

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
            } else {
                m_connUrl = "tcp://";
            }
        } else
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol: '%s'.", protocol.c_str()));

        if (host.empty())
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown host"));

        m_connUrl.append(host);

        if (!_domain && m_o->u->port().empty())
            m_connUrl.append(m_ssl ? ":443" : ":80");

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

        if (m_hc->m_enableEncoding) {
            bool hasAE = false;
            m_req->hasHeader("Accept-Encoding", hasAE);
            if (!hasAE)
                m_req->appendHeader("Accept-Encoding", "gzip, deflate");
        }

        if (m_o->body)
            m_req->set_body(m_o->body);

        if (m_ssl)
            m_sslhost = m_o->u->hostname();
        else
            m_sslhost.clear();

        m_reuse = false;
        m_h2PoolKey.clear();

        // Check for existing H2 session before creating a new TCP connection
        if (m_ssl && m_hc->m_enableH2) {
            m_h2PoolKey = build_h2_pool_key(m_connUrl, m_http_proxy, m_sslhost, m_hc->m_context.get());
            m_h2session = m_hc->get_h2session(m_h2PoolKey);
            if (m_h2session && !m_h2session->m_destroyed && !m_h2session->m_closed) {
                m_conn = m_h2session->m_conn;
                return next(h2_wait_settings);
            }
            m_h2session.Release();

            // Try to become the H2 handshake leader for this URL.
            // If another fiber is already doing the handshake, queue up and wait.
            if (!m_hc->h2_acquire(m_h2PoolKey, &m_h2session, &m_conn, this)) {
                next(h2_wait_settings);
                return CALL_E_PENDDING;
            }
            m_is_h2_leader = true;
        }

        while (m_hc->get_conn(m_connUrl, m_conn)) {
            if (http_conn_looks_alive(m_conn)) {
                m_reuse = true;
                return next(connected);
            }

            m_conn.Release();
        }

        if (m_http_proxy.empty()) {
            if (m_ssl) {
                // Split into TCP connect + TLS handshake so m_conn holds
                // the TCP socket during connect, enabling abort callbacks
                // to find and cancel the pending connection.
                exlib::string tcpUrl = "tcp://";
                tcpUrl.append(m_connUrl.substr(6));
                return net_base::connect(tcpUrl, timeout(), m_conn, next(ssl_handshake));
            } else
                return net_base::connect(m_connUrl, timeout(), m_conn, next(connected));
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

            while (m_hc->get_conn(m_http_proxy, m_conn)) {
                if (http_conn_looks_alive(m_conn)) {
                    m_reuse = true;
                    return next(m_ssl ? ssl_connect : connected);
                }

                m_conn.Release();
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

            return net_base::connect(connUrl, timeout(), m_conn,
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

        m_conn.As<Socket_base>()->set_timeout(timeout());
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
        m_conn.As<Socket_base>()->set_timeout(timeout());
        return m_hc->request(m_conn, m_reqConn, &m_retVal, next(ssl_handshake), true);
    }

    ON_STATE(asyncRequest, ssl_connected)
    {
        int32_t status;
        result_t hr;

        HttpResponse_base* resp = static_cast<HttpResponse_base*>(m_retVal.get());
        hr = resp->get_statusCode(status);
        if (hr < 0)
            return hr;

        if (status != 200) {
            exlib::string msg;

            resp->get_statusMessage(msg);
            return CHECK_ERROR(Runtime::setError("HttpClient: " + msg));
        }

        m_reqConn.Release();
        m_retVal.Release();

        return next(ssl_handshake);
    }

    ON_STATE(asyncRequest, ssl_handshake)
    {
        // m_conn is the raw TCP socket from net_base::connect.
        // Sync it to req so abort() can close it during TLS handshake.
        m_req->_set_socket(m_conn);

        obj_ptr<TLSSocket> ss = new TLSSocket();
        ss->init(m_hc->m_context);

        // Auto-negotiate ALPN: if user hasn't set ALPN on the context,
        // set ["h2", "http/1.1"] or ["http/1.1"] based on enableH2
        SecureContext* ctx = static_cast<SecureContext*>(m_hc->m_context.get());
        if (!ctx->hasAlpn()) {
            if (m_hc->m_enableH2) {
                static const unsigned char alpn[] = {
                    2, 'h', '2',
                    8, 'h', 't', 't', 'p', '/', '1', '.', '1'
                };
                SSL_set_alpn_protos(ss->m_tls, alpn, sizeof(alpn));
            } else {
                static const unsigned char alpn[] = {
                    8, 'h', 't', 't', 'p', '/', '1', '.', '1'
                };
                SSL_set_alpn_protos(ss->m_tls, alpn, sizeof(alpn));
            }
        }

        obj_ptr<Stream_base> conn = m_conn;
        m_conn = ss;
        m_req->_set_socket(m_conn);

        return ss->connect(conn, m_sslhost, next(connected));
    }

    ON_STATE(asyncRequest, connected)
    {
        m_req->_set_socket(m_conn);
        if (!m_ssl)
            m_conn.As<Socket_base>()->set_timeout(timeout());

        // Check ALPN negotiation result for HTTP/2 auto-upgrade
        if (m_ssl && !m_reuse) {
            TLSSocket* tls = static_cast<TLSSocket*>(TLSSocket_base::getInstance(m_conn));
            if (tls) {
                exlib::string alpn;
                tls->get_alpnProtocol(alpn);
                if (alpn == "h2")
                    return next(h2_init_session);
            }
        }

        // If this fiber is the H2 leader but taking the HTTP/1.1 path
        // (either ALPN was not h2, or reusing an existing connection),
        // clean up the pending entry so future requests don't hang as waiters.
        if (m_is_h2_leader) {
            m_hc->h2_fail(m_h2PoolKey, CALL_E_INVALID_CALL);
            m_is_h2_leader = false;
        }

        return m_hc->request(m_conn, m_req, &m_retVal, next(requested), true);
    }

    ON_STATE(asyncRequest, h2_init_session)
    {
        // Only the leader fiber reaches here (guarded by h2_acquire in prepare).

        // Create new H2 session (internal mode: no V8 access)
        m_h2session = new Http2Session(false);
        m_h2session->m_internal = true;
        m_h2session->m_scheme = "https";
        m_h2session->m_authority = m_sslhost;

        result_t hr = m_h2session->init(m_conn);
        if (hr < 0) {
            m_hc->h2_fail(m_h2PoolKey, hr);
            m_is_h2_leader = false;
            return hr;
        }

        m_hc->save_h2session(m_h2PoolKey, m_h2session);
        m_h2session->startLoops();

        // Wake all queued waiters with the new session
        m_hc->h2_complete(m_h2PoolKey, m_h2session, m_conn);
        m_is_h2_leader = false;

        return next(h2_wait_settings);
    }

    ON_STATE(asyncRequest, h2_wait_settings)
    {
        if (!m_h2session->m_remote_settings_ready.load(std::memory_order_acquire))
            m_h2session->m_remote_settings_event.wait();

        if (m_h2session->m_closed || m_h2session->m_destroyed)
            return CHECK_ERROR(Runtime::setError("Http2Session: session closed during SETTINGS exchange"));

        return next(h2_request);
    }

    ON_STATE(asyncRequest, h2_request)
    {
        // Build headers for H2 request from HttpRequest
        exlib::string method, path, host;
        m_req->get_method(method);
        m_req->get_address(path);
        m_h2_path = path;
        m_req->firstHeader("Host", host);

        m_h2_hdrs.clear();
        m_h2_hdrs.push_back({ ":method", method });
        m_h2_hdrs.push_back({ ":path", path });
        m_h2_hdrs.push_back({ ":scheme", "https" });
        m_h2_hdrs.push_back({ ":authority", host.empty() ? m_sslhost : host });

        // Copy regular headers from request (skip Host, it's in :authority)
        obj_ptr<Headers_base> req_headers;
        m_req->get_headers(req_headers);
        Headers* hc = static_cast<Headers*>(req_headers.get());
        for (size_t i = 0; i < hc->m_map.size(); i++) {
            auto& p = hc->m_map[i];
            exlib::string name = p.first;
            // Skip pseudo-headers and Host (already in :authority)
            if (name.length() > 0 && name[0] != ':'
                && qstricmp(name.c_str(), "Host")) {
                // HTTP/2 requires lowercase header names
                for (size_t j = 0; j < name.length(); j++)
                    name[j] = tolower(name[j]);
                m_h2_hdrs.push_back({ name, p.second.string() });
            }
        }

        m_h2_endStream = (method == "GET" || method == "HEAD");
        if (!m_h2_endStream) {
            obj_ptr<Stream_base> bodyStream;
            m_req->get_body(bodyStream);
            if (!bodyStream)
                m_h2_endStream = true;
        }

        // Read body BEFORE submitting request to avoid a race where the
        // readLoop's asyncFlushOutput sends the HEADERS frame before the
        // DATA frame is ready, causing out-of-order frames if a fiber
        // switch occurs during readAll.
        if (!m_h2_endStream) {
            obj_ptr<Stream_base> bodyStream;
            m_req->get_body(bodyStream);
            obj_ptr<SeekableStream_base> seekable = SeekableStream_base::getInstance(bodyStream);
            if (seekable) {
                seekable->rewind();
                return seekable->readAll(m_buffer, next(h2_submit_with_body));
            }
        }

        return next(h2_submit_no_body);
    }

    ON_STATE(asyncRequest, h2_submit_no_body)
    {
        result_t hr = m_h2session->request(m_h2_hdrs, m_h2_endStream, m_h2stream);
        if (hr < 0)
            return hr;

        // No asyncFlushOutput needed: request() atomically collects
        // and enqueues output for endStream=true requests.
        if (!m_h2_endStream)
            m_h2session->asyncFlushOutput();
        return next(h2_request_sent);
    }

    ON_STATE(asyncRequest, h2_submit_with_body)
    {
        // Body data is now in m_buffer. Prepare send buffer BEFORE
        // submitting request so that submit + resume_data + collect
        // can happen atomically under one lock.
        exlib::string body_data;
        if (n != CALL_RETURN_NULL && m_buffer) {
            Buffer* buf = Buffer::Cast(m_buffer);
            body_data.append((const char*)buf->data(), buf->length());
            m_buffer.Release();
        }

        // Build nghttp2_nv array
        std::vector<nghttp2_nv> nva(m_h2_hdrs.size());
        for (size_t i = 0; i < m_h2_hdrs.size(); i++) {
            nva[i] = {
                (uint8_t*)m_h2_hdrs[i].first.c_str(), (uint8_t*)m_h2_hdrs[i].second.c_str(),
                m_h2_hdrs[i].first.length(), m_h2_hdrs[i].second.length(),
                NGHTTP2_NV_FLAG_NO_COPY_NAME | NGHTTP2_NV_FLAG_NO_COPY_VALUE
            };
        }

        nghttp2_data_provider data_prd;
        data_prd.source.ptr = nullptr;
        data_prd.read_callback = Http2Session::data_source_read_callback;

        // Pre-create the stream to hold body data
        obj_ptr<Http2Stream> stream = new Http2Stream(m_h2session, 0);
        if (!body_data.empty()) {
            stream->m_send_data = std::move(body_data);
        }
        stream->m_send_end = true;

        // Atomically submit + addStream + resume + collect under one lock
        int32_t stream_id;
        exlib::string pending;

        // Lock m_request_lock to ensure HPACK-encoded frames are enqueued
        // in the same order they were encoded (prevents out-of-order delivery).
        m_h2session->m_request_lock.lock();

        m_h2session->m_nghttp2.submit_request_with_data(
            nva.data(), nva.size(), &data_prd,
            stream_id, pending,
            [&](int32_t sid) {
                stream->m_stream_id = sid;
                m_h2session->addStream(sid, stream);
            });

        if (stream_id < 0) {
            m_h2session->m_request_lock.unlock();
            return Runtime::setError(exlib::string("Http2Session: submit request failed: ") + nghttp2_strerror(stream_id));
        }

        m_h2stream = stream;

        // Send the atomically collected output
        if (!pending.empty()) {
            obj_ptr<Buffer_base> buf = new Buffer(pending.c_str(), pending.length());
            m_h2session->enqueueFlush(new AsyncFlushItem(buf));
        }

        m_h2session->m_request_lock.unlock();

        return next(h2_request_sent);
    }

    ON_STATE(asyncRequest, h2_request_sent)
    {
        m_h2buf.Release();

        return next(h2_wait_headers);
    }

    ON_STATE(asyncRequest, h2_wait_headers)
    {
        // Wait for response headers
        return m_h2stream->waitHeaders(next(h2_response));
    }

    ON_STATE(asyncRequest, h2_response)
    {
        // Build HttpResponse from H2 stream headers and body
        obj_ptr<HttpResponse> resp = new HttpResponse();

        // Get response headers from the H2 stream
        obj_ptr<NObject> h2_headers = m_h2stream->m_headers;
        if (h2_headers) {
            // Extract :status
            Variant status_var;
            if (h2_headers->get(":status", status_var) == 0) {
                exlib::string status_str = status_var.string();
                resp->set_statusCode(atoi(status_str.c_str()));
            }

            // Copy regular headers to HttpResponse
            for (auto& kv : h2_headers->m_keys) {
                const exlib::string& key = kv.first;
                if (key.length() > 0 && key[0] != ':') {
                    int32_t idx = kv.second;
                    resp->appendHeader(key, h2_headers->m_values[idx].m_val.string());
                }
            }
        }

        // Set body stream to H2 stream for subsequent reads
        resp->set_keepAlive(true);
        resp->m_message->m_bodyStream = m_h2stream;

        m_retVal = resp;
        return next(requested);
    }

    ON_STATE(asyncRequest, requested)
    {
        HttpResponse_base* resp = static_cast<HttpResponse_base*>(m_retVal.get());

        bool enableCookie;
        m_hc->get_enableCookie(enableCookie);
        if (enableCookie) {
            obj_ptr<NArray> cookies;
            resp->get_cookies(cookies);
            m_hc->update_cookies(m_url, cookies);
        }

        bool upgrade;
        m_retVal->get_upgrade(upgrade);
        if (upgrade)
            return next(end);

        // Inner asyncRequest always returns a BodyStream (streaming path).
        // Wire up EOF-triggered connection pool return on the BodyStream.
        // For H2, session pooling is managed separately - skip BodyStream cleanup.
        bool keepAlive;
        m_retVal->get_keepAlive(keepAlive);
        if (keepAlive && !m_h2session) {
            obj_ptr<Stream_base> bodyStream;
            if (m_retVal->get_body(bodyStream) == 0 && bodyStream) {
                BodyStream* bs = static_cast<BodyStream*>(bodyStream.get());
                auto hc = m_hc;
                // Proxy connections are pooled under proxy URL; direct under connUrl.
                exlib::string connUrl = (!m_http_proxy.empty() && m_http_proxy[0] != 's' && m_sslhost.empty())
                    ? m_http_proxy
                    : m_connUrl;
                auto conn = m_conn;
                bs->setCleanup([hc, connUrl, conn](bool eof) {
                    if (eof)
                        hc->save_conn(connUrl, conn);
                });
            } else {
                // Empty body (e.g. Content-Length: 0): return connection immediately.
                exlib::string connUrl = (!m_http_proxy.empty() && m_http_proxy[0] != 's' && m_sslhost.empty())
                    ? m_http_proxy
                    : m_connUrl;
                m_hc->save_conn(connUrl, m_conn);
            }
        }
        return next(decode);
    }

    ON_STATE(asyncRequest, decode)
    {
        if (!m_hc->m_enableEncoding)
            return next(end);

        HttpResponse_base* resp = static_cast<HttpResponse_base*>(m_retVal.get());
        exlib::string encoding;
        if (resp->firstHeader("Content-Encoding", encoding) == CALL_RETURN_NULL)
            return next(end);

        obj_ptr<Stream_base> bodyStream;
        if (resp->get_body(bodyStream) != 0 || !bodyStream)
            return next(end);

        m_decodeStream = new MemoryStream();

        if (!qstricmp(encoding.c_str(), "gzip") || !qstricmp(encoding.c_str(), "x-gzip"))
            return zlib_base::gunzipTo(bodyStream, m_decodeStream, -1, next(decode_done));
        else if (!qstricmp(encoding.c_str(), "deflate") || !qstricmp(encoding.c_str(), "x-deflate"))
            return zlib_base::inflateTo(bodyStream, m_decodeStream, -1, next(decode_done));

        m_decodeStream.Release();
        return next(end);
    }

    ON_STATE(asyncRequest, decode_done)
    {
        m_decodeStream->rewind();

        HttpResponse* resp = static_cast<HttpResponse*>(static_cast<HttpResponse_base*>(m_retVal.get()));
        resp->m_message->m_bodyStream = m_decodeStream;
        resp->removeHeader("Content-Encoding");
        resp->removeHeader("Content-Length");

        m_decodeStream.Release();
        return next(end);
    }

    ON_STATE(asyncRequest, end)
    {
        result_t hr;
        int32_t status;
        exlib::string location;
        obj_ptr<UrlObject_base> u1;

        HttpResponse_base* resp = static_cast<HttpResponse_base*>(m_retVal.get());
        hr = resp->get_statusCode(status);
        if (hr < 0)
            return hr;

        bool isRedirect = (status == 301 || status == 302 || status == 303
            || status == 307 || status == 308);
        if (!isRedirect)
            return complete();

        // Fetch API redirect mode takes precedence over HttpClient autoRedirect
        if (m_o->redirect == "error")
            return CHECK_ERROR(Runtime::setError(kTypeError, "fetch: redirect not allowed"));
        if (m_o->redirect == "manual")
            return complete(); // return redirect response as-is
        if (!m_hc->m_autoRedirect)
            return complete();

        hr = m_retVal->firstHeader("location", location);
        if (hr < 0)
            return hr;

        m_o->u->resolve(location, u1);
        m_o->u = u1.As<Url>();
        m_url.resize(0);
        m_o->u->toString(m_url);

        if (m_urls.find(m_url) != m_urls.end())
            return CHECK_ERROR(Runtime::setError(kTypeError, "HttpClient: redirect cycle"));

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
        if (can_retry_transport_error(v) && m_reuse && (at(ssl_connect) || at(connected))) {
            m_reuse = false;
            next(prepare);
            return 0;
        }

        if (can_retry_transport_error(v) && can_retry_http1_request()) {
            m_http1_retry_count++;
            m_reuse = false;
            m_buffer.Release();
            m_decodeStream.Release();
            m_reqConn.Release();
            m_retVal.Release();
            m_conn.Release();
            next(prepare);
            return 0;
        }

        // H2 waiter woken with CALL_E_INVALID_CALL means ALPN was not h2.
        // Fall back to independent connection by retrying from prepare.
        if (v == CALL_E_INVALID_CALL && at(h2_wait_settings)) {
            m_h2session.Release();
            m_conn.Release();
            next(prepare);
            return 0;
        }

        if (can_retry_transport_error(v) && can_retry_h2_request()) {
            m_h2_retry_count++;
            if (m_h2session && !m_h2PoolKey.empty())
                m_hc->remove_h2session(m_h2PoolKey, m_h2session);
            m_h2stream.Release();
            m_h2session.Release();
            m_conn.Release();
            next(prepare);
            return 0;
        }

        // If this fiber is the H2 leader and hits an error during
        // TCP connect / TLS / session init, propagate to all waiters.
        if (m_is_h2_leader) {
            m_hc->h2_fail(m_h2PoolKey, v);
            m_is_h2_leader = false;
        }

        // D.3: if abort was requested, convert the connection error to AbortError
        if (m_o->signal) {
            bool aborted;
            m_o->signal->get_aborted(aborted);
            if (aborted) {
                Runtime::setError(kTypeError, "AbortError");
                v = CALL_E_EXCEPTION;
            }
        }

        // Emit 'error' event on the request when in async (callback/deferred) mode
        if (m_o->is_async && m_req) {
            exlib::string errMsg = Runtime::errMessage();
            if (errMsg.empty())
                errMsg = "Request failed";
            Variant err(errMsg);
            m_req->_emit("error", err);
        }

        return v;
    }

    bool can_retry_transport_error(int32_t v)
    {
        if (v == CALL_E_TIMEOUT || v == CALL_E_ABORT)
            return false;

        if (m_o->signal) {
            bool aborted;
            if (m_o->signal->get_aborted(aborted) >= 0 && aborted)
                return false;
        }

        return true;
    }

    bool can_retry_http1_request()
    {
        if (m_h2session || m_http1_retry_count > 0 || m_completed)
            return false;

        if (!(at(connected) || at(requested) || at(decode)))
            return false;

        exlib::string method;
        if (!m_req || m_req->get_method(method) < 0)
            return false;

        return !qstricmp(method.c_str(), "GET") || !qstricmp(method.c_str(), "HEAD");
    }

    bool can_retry_h2_request()
    {
        if (!m_h2session || m_h2_retry_count > 0)
            return false;

        if (!(at(h2_submit_no_body) || at(h2_submit_with_body)
                || at(h2_request_sent) || at(h2_wait_headers)))
            return false;

        exlib::string method;
        if (!m_req || m_req->get_method(method) < 0)
            return false;

        return !qstricmp(method.c_str(), "GET") || !qstricmp(method.c_str(), "HEAD");
    }

    int32_t complete()
    {
        // Clear request-phase abort callbacks but keep the timer alive,
        // so AbortSignal.timeout() can still fire during body consumption.
        if (m_o && m_o->signal)
            m_o->abort_signal()->clearCallbacks();
        m_completed = true;

        if (m_o->is_async) {
            m_req->_set_response(static_cast<HttpResponse_base*>(m_retVal.get()));
            m_req->_emit("response", m_retVal);
            m_retVal = m_req;
        } else if (m_respRetVal) {
            *m_respRetVal = static_cast<HttpResponse_base*>(m_retVal.get());
        }
        return next();
    }

private:
    obj_ptr<HttpRequest::Options> m_o;
    exlib::string m_url;
    exlib::string m_sslhost;
    bool m_ssl;
    exlib::string m_http_proxy;
    obj_ptr<HttpMessage_base> m_tempRetVal;
    obj_ptr<HttpMessage_base>& m_retVal;
    obj_ptr<HttpResponse_base>* m_respRetVal = nullptr;
    std::unordered_map<exlib::string, bool> m_urls;
    obj_ptr<Stream_base> m_conn;
    obj_ptr<HttpRequest> m_req;
    obj_ptr<ValueHolder> m_req_holder;
    obj_ptr<HttpRequest> m_reqConn;
    exlib::string m_connUrl;
    exlib::string m_h2PoolKey;
    obj_ptr<HttpClient> m_hc;
    obj_ptr<Buffer_base> m_buffer;
    bool m_reuse;
    bool m_completed = false;

    int32_t timeout() const
    {
        return m_o->has_timeout ? m_o->timeout : m_hc->m_timeout;
    }

    // Content-Encoding decompression state
    obj_ptr<MemoryStream> m_decodeStream;

    // HTTP/2 auto-upgrade state
    obj_ptr<Http2Session> m_h2session;
    obj_ptr<Http2Stream> m_h2stream;
    obj_ptr<Buffer_base> m_h2buf;
    exlib::string m_h2_path;
    std::vector<std::pair<exlib::string, exlib::string>> m_h2_hdrs;
    bool m_h2_endStream = true;
    bool m_is_h2_leader = false;
    int32_t m_http1_retry_count = 0;
    int32_t m_h2_retry_count = 0;
};

// AsyncEvent is the internal binding execution context. It is still used
// for sync-style JS APIs and callback-style APIs, and does not mean the
// external requestSync/getSync/... signatures are async-mode.
result_t HttpClient::requestSync(HttpRequest::Options* o, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Reject immediately if signal is already aborted
    if (o->signal) {
        bool aborted;
        o->signal->get_aborted(aborted);
        if (aborted)
            return CHECK_ERROR(Runtime::setError(kTypeError, "AbortError"));
    }

    if (o->u->protocol() == "file:")
        return build_file_fetch_response(o, retVal);

    return (new asyncRequest(o, retVal, ac))->post(0);
}

// Lightweight event that silently absorbs async completion,
// used by callback-mode HTTP methods to fire-and-forget.
class FireAndForgetEvent : public AsyncEvent {
public:
    FireAndForgetEvent(Isolate* isolate)
        : AsyncEvent(isolate)
    {
        setAsync();
        isolate->Ref();
    }

    virtual int32_t post(int32_t v) override
    {
        m_isolate->Unref();
        delete this;
        return 0;
    }
};

result_t HttpClient::request(HttpRequest::Options* o, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(o, ac))->post(0);
}

result_t HttpClient::request(exlib::string method, exlib::string url, SeekableStream_base* body,
    Headers_base* headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest::Options> o = new HttpRequest::Options();
    o->agent = this;
    o->method = method;
    o->keepAlive = m_keepAlive;
    o->headers = headers;
    o->body = body;

    obj_ptr<Url> u = new Url();
    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;
    o->u = u;

    return requestSync(o.get(), retVal, ac);
}

result_t HttpClient::get_request_opts(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, AsyncEvent* ac,
    v8::Local<v8::Function> callback, bool skip_body)
{
    ac->m_ctx.resize(1);

    obj_ptr<HttpRequest::Options> o = new HttpRequest::Options();
    o->agent = this;

    result_t hr = o->from_opts(method, url, opts, true, false, skip_body);
    if (hr < 0)
        return hr;

    // Object-only overload: build URL entirely from opts fields
    if (!o->u) {
        o->u = new Url();
        o->u->format(opts);
    }

    o->req = new HttpRequest();
    if (!callback.IsEmpty()) {
        o->is_async = true;
        o->req_holder = new ValueHolder(o->req->wrap());
        v8::Local<v8::Object> _r;
        hr = o->req->once(o->req->holder()->NewString("response"), callback, _r);
        if (hr < 0)
            return hr;
    }

    ac->m_ctx[0] = o;

    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t HttpClient::requestSync(exlib::string method, exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac, bool headerOnly)
{
    if (ac->isSync())
        return get_request_opts(method, url, opts, ac);

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return requestSync(o.get(), retVal, ac);
}

result_t HttpClient::requestSync(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return get_request_opts(method, url, opts, ac);

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return requestSync(o.get(), retVal, ac);
}

result_t HttpClient::requestSync(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return requestSync("GET", url, opts, retVal, ac);
}

result_t HttpClient::requestSync(v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return requestSync("GET", "", opts, retVal, ac);
}

result_t HttpClient::getSync(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return requestSync("GET", url, opts, retVal, ac);
}

result_t HttpClient::fire_request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    result_t hr = get_request_opts(method, url, opts, ac,
        v8::Local<v8::Function>(), true);
    if (hr != CALL_E_NOSYNC)
        return hr;

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    o->is_async = true;
    if (!o->req_holder)
        o->req_holder = new ValueHolder(o->req->wrap());
    retVal = o->req;

    (new asyncRequest(o.get(), new FireAndForgetEvent(ac->isolate())))->post(0);
    return 0;
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        obj_ptr<HttpMessage_base> msg;
        result_t hr = fire_request(method, url, opts, msg, ac);
        if (hr < 0)
            return hr;

        retVal = (HttpRequest_base*)msg.get();
        return 0;
    }
    return 0;
}

result_t HttpClient::request(v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("GET", "", opts, retVal, ac);
}

result_t HttpClient::request(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("GET", url, opts, retVal, ac);
}

result_t HttpClient::fire_callback_request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, v8::Local<v8::Function> callback,
    obj_ptr<HttpMessage_base>& retVal, AsyncEvent* ac)
{
    result_t hr = get_request_opts(method, url, opts, ac, callback, true);
    if (hr != CALL_E_NOSYNC)
        return hr;

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    retVal = o->req;

    (new asyncRequest(o.get(), new FireAndForgetEvent(ac->isolate())))->post(0);
    return 0;
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        obj_ptr<HttpMessage_base> msg;
        result_t hr = fire_callback_request(method, url, opts, callback, msg, ac);
        if (hr < 0)
            return hr;

        retVal = (HttpRequest_base*)msg.get();
        return 0;
    }
    return 0;
}

result_t HttpClient::request(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("GET", url, opts, callback, retVal, ac);
}

result_t HttpClient::request(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        obj_ptr<HttpMessage_base> msg;
        v8::Local<v8::Object> opts = v8::Object::New(Isolate::current()->m_isolate);
        result_t hr = fire_callback_request("GET", url, opts, callback, msg, ac);
        if (hr < 0)
            return hr;

        retVal = (HttpRequest_base*)msg.get();
        return 0;
    }
    return 0;
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request(method, url, v8::Object::New(Isolate::current()->m_isolate), callback, retVal, ac);
}

result_t HttpClient::request(v8::Local<v8::Object> opts, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("GET", "", opts, callback, retVal, ac);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("GET", url, opts, retVal, ac);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("GET", url, opts, callback, retVal, ac);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request(url, callback, retVal, ac);
}

result_t HttpClient::postSync(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return requestSync("POST", url, opts, retVal, ac);
}

result_t HttpClient::post(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("POST", url, opts, retVal, ac);
}

result_t HttpClient::post(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("POST", url, opts, callback, retVal, ac);
}

result_t HttpClient::post(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    obj_ptr<HttpMessage_base> msg;
    if (ac->isSync()) {
        v8::Local<v8::Object> opts = v8::Object::New(Isolate::current()->m_isolate);
        result_t hr = fire_callback_request("POST", url, opts, callback, msg, ac);
        if (hr < 0)
            return hr;

        retVal = (HttpRequest_base*)msg.get();
        return 0;
    }
    return 0;
}

result_t HttpClient::delSync(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return requestSync("DELETE", url, opts, retVal, ac);
}

result_t HttpClient::del(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("DELETE", url, opts, retVal, ac);
}

result_t HttpClient::del(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("DELETE", url, opts, callback, retVal, ac);
}

result_t HttpClient::del(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    obj_ptr<HttpMessage_base> msg;
    if (ac->isSync()) {
        v8::Local<v8::Object> opts = v8::Object::New(Isolate::current()->m_isolate);
        result_t hr = fire_callback_request("DELETE", url, opts, callback, msg, ac);
        if (hr < 0)
            return hr;

        retVal = (HttpRequest_base*)msg.get();
        return 0;
    }
    return 0;
}

result_t HttpClient::putSync(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return requestSync("PUT", url, opts, retVal, ac);
}

result_t HttpClient::put(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("PUT", url, opts, retVal, ac);
}

result_t HttpClient::put(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("PUT", url, opts, callback, retVal, ac);
}

result_t HttpClient::put(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    obj_ptr<HttpMessage_base> msg;
    if (ac->isSync()) {
        v8::Local<v8::Object> opts = v8::Object::New(Isolate::current()->m_isolate);
        result_t hr = fire_callback_request("PUT", url, opts, callback, msg, ac);
        if (hr < 0)
            return hr;

        retVal = (HttpRequest_base*)msg.get();
        return 0;
    }
    return 0;
}

result_t HttpClient::patchSync(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return requestSync("PATCH", url, opts, retVal, ac);
}

result_t HttpClient::patch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("PATCH", url, opts, retVal, ac);
}

result_t HttpClient::patch(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("PATCH", url, opts, callback, retVal, ac);
}

result_t HttpClient::patch(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    obj_ptr<HttpMessage_base> msg;
    if (ac->isSync()) {
        v8::Local<v8::Object> opts = v8::Object::New(Isolate::current()->m_isolate);
        result_t hr = fire_callback_request("PATCH", url, opts, callback, msg, ac);
        if (hr < 0)
            return hr;

        retVal = (HttpRequest_base*)msg.get();
        return 0;
    }
    return 0;
}

result_t HttpClient::headSync(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return requestSync("HEAD", url, opts, retVal, ac);
}

result_t HttpClient::head(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("HEAD", url, opts, retVal, ac);
}

result_t HttpClient::head(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    return request("HEAD", url, opts, callback, retVal, ac);
}

result_t HttpClient::head(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal, AsyncEvent* ac)
{
    obj_ptr<HttpMessage_base> msg;
    if (ac->isSync()) {
        v8::Local<v8::Object> opts = v8::Object::New(Isolate::current()->m_isolate);
        result_t hr = fire_callback_request("HEAD", url, opts, callback, msg, ac);
        if (hr < 0)
            return hr;

        retVal = (HttpRequest_base*)msg.get();
        return 0;
    }
    return 0;
}

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpRequest_base>& retVal)
{
    request(conn, req, nullptr, new FireAndForgetEvent(Isolate::current()), true);
    retVal = req;
    return 0;
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal)
{
    AsyncEvent ac(Isolate::current());
    obj_ptr<HttpMessage_base> msg;
    result_t hr = fire_request(method, url, opts, msg, &ac);
    if (hr < 0)
        return hr;

    retVal = (HttpRequest_base*)msg.get();
    return 0;
}

result_t HttpClient::request(v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("GET", "", opts, retVal);
}

result_t HttpClient::request(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("GET", url, opts, retVal);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    AsyncEvent ac(Isolate::current());
    obj_ptr<HttpMessage_base> msg;
    result_t hr = fire_callback_request(method, url, opts, callback, msg, &ac);
    if (hr < 0)
        return hr;

    retVal = (HttpRequest_base*)msg.get();
    return 0;
}

result_t HttpClient::request(v8::Local<v8::Object> opts, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("GET", "", opts, callback, retVal);
}

result_t HttpClient::request(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal)
{
    return request("GET", url, opts, callback, retVal);
}

result_t HttpClient::request(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request(url, v8::Object::New(Isolate::current()->m_isolate), callback, retVal);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal)
{
    return request(method, url, v8::Object::New(Isolate::current()->m_isolate), callback, retVal);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("GET", url, opts, retVal);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal)
{
    return request("GET", url, opts, callback, retVal);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request(url, callback, retVal);
}

result_t HttpClient::post(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("POST", url, opts, retVal);
}

result_t HttpClient::post(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal)
{
    return request("POST", url, opts, callback, retVal);
}

result_t HttpClient::post(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("POST", url, v8::Object::New(Isolate::current()->m_isolate), callback, retVal);
}

result_t HttpClient::del(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("DELETE", url, opts, retVal);
}

result_t HttpClient::del(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal)
{
    return request("DELETE", url, opts, callback, retVal);
}

result_t HttpClient::del(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("DELETE", url, v8::Object::New(Isolate::current()->m_isolate), callback, retVal);
}

result_t HttpClient::put(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("PUT", url, opts, retVal);
}

result_t HttpClient::put(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal)
{
    return request("PUT", url, opts, callback, retVal);
}

result_t HttpClient::put(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("PUT", url, v8::Object::New(Isolate::current()->m_isolate), callback, retVal);
}

result_t HttpClient::patch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("PATCH", url, opts, retVal);
}

result_t HttpClient::patch(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal)
{
    return request("PATCH", url, opts, callback, retVal);
}

result_t HttpClient::patch(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("PATCH", url, v8::Object::New(Isolate::current()->m_isolate), callback, retVal);
}

result_t HttpClient::head(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("HEAD", url, opts, retVal);
}

result_t HttpClient::head(exlib::string url, v8::Local<v8::Object> opts,
    v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal)
{
    return request("HEAD", url, opts, callback, retVal);
}

result_t HttpClient::head(exlib::string url, v8::Local<v8::Function> callback,
    obj_ptr<HttpRequest_base>& retVal)
{
    return request("HEAD", url, v8::Object::New(Isolate::current()->m_isolate), callback, retVal);
}

// Async state machine shared by both fetch(url) and fetch(request).
class asyncFetch : public AsyncState {
public:
    asyncFetch(obj_ptr<HttpRequest::Options> o,
        obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
        : AsyncState(ac)
        , m_hc((HttpClient*)(HttpClient_base*)o->agent)
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
        Runtime::setError(kTypeError, msg);
        return CALL_E_EXCEPTION;
    }

    ON_STATE(asyncFetch, do_request)
    {
        // D.2: reject immediately if signal is already aborted
        if (m_o->signal) {
            bool aborted;
            m_o->signal->get_aborted(aborted);
            if (aborted) {
                Runtime::setError(kTypeError, "AbortError");
                return CALL_E_EXCEPTION;
            }
        }
        return m_hc->requestSync(m_o.get(), m_httpMsg, next(do_wrap));
    }

    ON_STATE(asyncFetch, do_wrap)
    {
        exlib::string finalUrl;
        m_o->u->toString(finalUrl);

        HttpResponse* resp = (HttpResponse*)m_httpMsg.get();
        resp->m_fetchUrl = finalUrl;
        resp->m_redirected = m_o->redirected;
        resp->m_fetchType = "basic";

        // Transfer abort signal to the body stream so that abort during
        // body consumption (text/json/arrayBuffer/bytes) still works.
        if (m_o->signal) {
            obj_ptr<Stream_base> bodyStream;
            result_t body_hr = resp->get_body(bodyStream);
            if (body_hr == 0 && bodyStream) {
                Http2Stream_base* h2s = Http2Stream_base::getInstance(bodyStream);
                if (h2s) {
                    // HTTP/2: register abort callback to reset the stream
                    obj_ptr<Http2Stream_base> h2ref(h2s);
                    m_o->abort_signal()->addAbortCallback([h2ref]() {
                        ((Http2Stream*)h2ref.get())->onClose(NGHTTP2_CANCEL);
                    });
                } else {
                    // HTTP/1.1: register abort callback on BodyStream.
                    // After Content-Encoding decompression, body may be a
                    // MemoryStream (fully buffered) — nothing to hook in that case.
                    BodyStream* bs = dynamic_cast<BodyStream*>(bodyStream.get());
                    if (bs)
                        bs->setAbortSignal(m_o->abort_signal());
                }
            }
        }

        m_retVal = m_httpMsg;
        return next();
    }

private:
    obj_ptr<HttpClient> m_hc;
    obj_ptr<HttpRequest::Options> m_o;
    obj_ptr<HttpResponse_base> m_httpMsg;
    obj_ptr<HttpResponse_base>& m_retVal;
};

result_t HttpClient::fetch(exlib::string url, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    // Sync phase: parse v8::Local opts into ac->m_ctx as a single Options object.
    if (ac->isSync())
        return get_request_opts("GET", url, opts, ac);

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return (new asyncFetch(o, retVal, ac))->post(0);
}

result_t HttpClient::fetch(HttpRequest_base* request, v8::Local<v8::Object> opts,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    // Sync phase: extract url/method from the request, apply opts overrides.
    if (ac->isSync()) {
        exlib::string req_method;
        request->get_method(req_method);

        obj_ptr<HttpRequest::Options> o = new HttpRequest::Options();
        o->agent = this;

        result_t hr = o->from_opts(req_method, "", opts, true, true);
        if (hr < 0)
            return hr;

        hr = o->apply_from_request(request);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = o;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<HttpRequest::Options> o = (HttpRequest::Options*)ac->m_ctx[0].object();
    return (new asyncFetch(o, retVal, ac))->post(0);
}
}
