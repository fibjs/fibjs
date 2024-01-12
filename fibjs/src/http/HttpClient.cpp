/*
 * HttpClient.cpp
 *
 *  Created on: Aug 12, 2016
 */

#include "object.h"
#include "HttpClient.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "HttpRequest.h"
#include "SslSocket.h"
#include "BufferedStream.h"
#include "inetAddr.h"
#include "ifs/net.h"
#include "ifs/zlib.h"
#include "ifs/json.h"
#include "ifs/msgpack.h"
#include "ifs/querystring.h"
#include <string.h>

namespace fibjs {

result_t HttpClient_base::_new(obj_ptr<HttpClient_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpClient();
    return 0;
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

result_t HttpClient::get_maxHeaderLength(int32_t& retVal)
{
    retVal = m_maxHeaderLength;
    return 0;
}

result_t HttpClient::set_maxHeaderLength(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxHeaderLength = newVal;
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

        if (u->m_protocol != "https:" && u->m_protocol != "http:" && u->m_protocol != "socks5:")
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol"));

        if (u->m_host.empty())
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

        if (u->m_protocol != "https:" && u->m_protocol != "http:" && u->m_protocol != "socks5:")
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol"));

        if (u->m_host.empty())
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown host"));

        m_https_proxy = newVal;
    }

    date_t d;

    d.now();
    d.add(m_poolTimeout, date_t::_MICROSECOND);
    clean_coon(d);

    return 0;
}

result_t HttpClient::get_sslVerification(int32_t& retVal)
{
    retVal = m_sslVerification;

    if (retVal < 0)
        return CALL_RETURN_NULL;

    return 0;
}

result_t HttpClient::set_sslVerification(int32_t newVal)
{
    if (newVal < ssl_base::C_VERIFY_NONE || newVal > ssl_base::C_VERIFY_REQUIRED)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    m_sslVerification = newVal;
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
            hc->set_domain(u->m_hostname);

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
        if (secure && u->m_protocol != "https:")
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

result_t HttpClient::setClientCert(X509Cert_base* crt, PKey_base* key)
{
    m_crt = crt;
    m_key = key;

    return 0;
}

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    class asyncRequest : public AsyncState {
    public:
        asyncRequest(HttpClient* hc, Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body,
            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_hc(hc)
            , m_conn(conn)
            , m_req(req)
            , m_response_body(response_body)
            , m_retVal(retVal)
        {
            next(send);

            exlib::string method;
            m_req->get_method(method);
            m_bNoBody = !qstricmp(method.c_str(), "head", 4);
        }

        ON_STATE(asyncRequest, send)
        {
            return m_req->sendTo(m_conn, next(recv));
        }

        ON_STATE(asyncRequest, recv)
        {
            obj_ptr<HttpResponse> resp = new HttpResponse();
            resp->m_message->m_bNoBody = m_bNoBody;

            if (m_response_body)
                resp->set_body(m_response_body);

            m_retVal = resp;
            m_retVal->set_maxHeadersCount(m_hc->m_maxHeadersCount);
            m_retVal->set_maxHeaderLength(m_hc->m_maxHeaderLength);
            m_retVal->set_maxBodySize(m_hc->m_maxBodySize);
            m_bs = new BufferedStream(m_conn);
            m_bs->set_EOL("\r\n");

            return m_retVal->readFrom(m_bs, next(m_hc->m_enableEncoding ? unzip : close));
        }

        ON_STATE(asyncRequest, unzip)
        {
            exlib::string hdr;

            if (!m_response_body && m_retVal->firstHeader("Content-Encoding", hdr) != CALL_RETURN_NULL) {
                m_retVal->removeHeader("Content-Encoding");

                m_retVal->get_body(m_body);
                m_unzip = new MemoryStream();

                if (hdr == "gzip")
                    return zlib_base::gunzipTo(m_body, m_unzip,
                        m_hc->m_maxBodySize, next(close));
                else if (hdr == "deflate")
                    return zlib_base::inflateRawTo(m_body, m_unzip,
                        m_hc->m_maxBodySize, next(close));
            }

            return next(close);
        }

        ON_STATE(asyncRequest, close)
        {
            if (m_unzip) {
                m_unzip->rewind();
                m_retVal->set_body(m_unzip);
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
        obj_ptr<SeekableStream_base> m_response_body;
        obj_ptr<HttpResponse_base>& m_retVal;
        bool m_bNoBody;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, conn, req, response_body, retVal, ac))->post(0);
}

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpResponse_base>& retVal,
    AsyncEvent* ac)
{
    return request(conn, req, NULL, retVal, ac);
}

result_t HttpClient::request(exlib::string method, obj_ptr<Url>& u, SeekableStream_base* body,
    SeekableStream_base* response_body, NObject* opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    class asyncRequest : public AsyncState {
    public:
        asyncRequest(HttpClient* hc, exlib::string method, obj_ptr<Url>& u,
            SeekableStream_base* body, SeekableStream_base* response_body, NObject* opts,
            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_method(method)
            , m_u(u)
            , m_body(body)
            , m_response_body(response_body)
            , m_opts(opts)
            , m_retVal(retVal)
            , m_hc(hc)
        {
            m_u->toString(m_url);
            if (m_response_body)
                m_response_body->tell(m_response_pos);
            next(prepare);
        }

        ON_STATE(asyncRequest, prepare)
        {
            bool _domain = false;

            m_urls[m_url] = true;

            exlib::string path;
            exlib::string cookie;

            m_ssl = false;
            if (m_u->m_protocol == "https:") {
                m_ssl = true;
                m_connUrl = "ssl://";
            } else if (m_u->m_protocol == "http:") {
                if (m_u->m_host.c_str()[0] == '/') {
                    _domain = true;
                    m_connUrl = "unix:";
                } else
                    m_connUrl = "tcp://";
            } else
                return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol"));

            if (m_u->m_host.empty())
                return CHECK_ERROR(Runtime::setError("HttpClient: unknown host"));

            m_connUrl.append(m_u->m_host);

            if (!_domain && m_u->m_port.empty())
                m_connUrl.append(m_ssl ? ":443" : ":80");

            m_req = new HttpRequest();

            m_req->set_method(m_method);

            m_http_proxy = m_hc->m_http_proxy;
            if (m_ssl && !m_hc->m_https_proxy.empty())
                m_http_proxy = m_hc->m_https_proxy;

            if (m_http_proxy.empty() || m_ssl) {
                m_u->get_path(path);
                m_req->set_address(path);
            } else
                m_req->set_address(m_url);

            bool enableEncoding = false;
            m_hc->get_enableEncoding(enableEncoding);
            if (enableEncoding)
                m_req->addHeader("Accept-Encoding", "gzip,deflate");

            bool enableCookie = false;
            m_hc->get_enableCookie(enableCookie);
            if (enableCookie) {
                m_hc->get_cookie(m_url, cookie);
                if (cookie.length() > 0)
                    m_req->addHeader("Cookie", cookie);
            }

            if (m_opts)
                m_req->addHeader(m_opts);

            exlib::string a = m_hc->agent();
            if (!a.empty()) {
                bool bCheck = false;
                m_req->hasHeader("User-Agent", bCheck);
                if (!bCheck)
                    m_req->addHeader("User-Agent", a);
            }

            bool bHost = false;
            m_req->hasHeader("Host", bHost);
            if (!bHost)
                m_req->addHeader("Host", m_u->m_host);

            if (m_body)
                m_req->set_body(m_body);

            if (m_ssl)
                m_sslhost = m_u->m_hostname;
            else
                m_sslhost.clear();

            m_reuse = false;
            if (m_hc->get_conn(m_connUrl, m_conn)) {
                m_reuse = true;
                return next(connected);
            }

            if (m_http_proxy.empty()) {
                if (m_ssl)
                    return ssl_base::connect(m_connUrl, m_hc->m_sslVerification,
                        m_hc->m_crt, m_hc->m_key, m_hc->m_timeout, m_conn, next(connected));
                else
                    return net_base::connect(m_connUrl, m_hc->m_timeout, m_conn, next(connected));
            } else {
                bool socks = m_http_proxy.c_str()[0] == 's';

                if (m_ssl && !socks) {
                    exlib::string host = m_connUrl.substr(6);
                    m_reqConn = new HttpRequest();

                    m_reqConn->set_method("CONNECT");
                    m_reqConn->set_address(host);
                    m_reqConn->addHeader("Host", host);

                    exlib::string a = m_hc->agent();
                    if (!a.empty())
                        m_reqConn->addHeader("User-Agent", a);
                }

                if (m_hc->get_conn(m_http_proxy, m_conn)) {
                    m_reuse = true;
                    return next(m_ssl ? ssl_connect : connected);
                }

                obj_ptr<Url> u = new Url();
                exlib::string connUrl;
                const char* def_port;

                u->parse(m_http_proxy);

                if (u->m_protocol == "https:") {
                    connUrl = "ssl://";
                    def_port = "443";
                } else if (u->m_protocol == "http:") {
                    connUrl = "tcp://";
                    def_port = "80";
                } else if (u->m_protocol == "socks5:") {
                    connUrl = "tcp://";
                    def_port = "1080";
                }

                connUrl.append(u->m_host);

                if (u->m_port.empty())
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

            ((Socket_base*)(Stream_base*)m_conn)->set_timeout(m_hc->m_timeout);
            return m_conn->write(buf, next(socks_hello_response));
        }

        ON_STATE(asyncRequest, socks_hello_response)
        {
            return m_conn->read(2, m_buffer, next(socks_connect));
        }

        ON_STATE(asyncRequest, socks_connect)
        {
            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("HttpClient: connection reset by socks 5 server."));

            exlib::string strBuffer;

            m_buffer->toString(strBuffer);
            if (strBuffer.length() != 2 || strBuffer.c_str()[0] != 5 || strBuffer.c_str()[1] != 0)
                return CHECK_ERROR(Runtime::setError("HttpClient: socks 5 handshake failed."));

            obj_ptr<Url> u = new Url();
            u->parse(m_connUrl);

            sockaddr_in dst;
            sockaddr_in6 dst6;

            if (!uv_ip4_addr(u->m_hostname.c_str(), 0, &dst)) {
                strBuffer.assign("\5\1\0\1", 4);
                strBuffer.append((char*)&dst.sin_addr, 4);
            } else if (!uv_ip6_addr(u->m_hostname.c_str(), 0, &dst6)) {
                strBuffer.assign("\5\1\0\4", 4);
                strBuffer.append((char*)&dst6.sin6_addr, 16);
            } else {
                strBuffer.assign("\5\1\0\3", 4);
                strBuffer.append(1, (char)u->m_hostname.length());
                strBuffer.append(u->m_hostname);
            }

            int16_t port = htons(atoi(u->m_port.c_str()));
            strBuffer.append((char*)&port, 2);

            obj_ptr<Buffer_base> buf = new Buffer(strBuffer.c_str(), strBuffer.length());
            return m_conn->write(buf, next(socks_connect_req_5_bytes));
        }

        ON_STATE(asyncRequest, socks_connect_req_5_bytes)
        {
            return m_conn->read(5, m_buffer, next(socks_connect_res_5_bytes));
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

            return m_conn->read(len, m_buffer, next(socks_connected));
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
            ((Socket_base*)(Stream_base*)m_conn)->set_timeout(m_hc->m_timeout);
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
            obj_ptr<SslSocket> ss = new SslSocket();

            if (m_hc->m_sslVerification >= 0)
                ss->set_verification(m_hc->m_sslVerification);

            obj_ptr<Stream_base> conn = m_conn;
            m_conn = ss;

            if (m_hc->m_crt && m_hc->m_key) {
                result_t hr = ss->setCert("", m_hc->m_crt, m_hc->m_key);
                if (hr < 0)
                    return hr;
            } else if (g_ssl.m_crt && g_ssl.m_key) {
                result_t hr = ss->setCert("", g_ssl.m_crt, g_ssl.m_key);
                if (hr < 0)
                    return hr;
            }

            return ss->connect(conn, m_sslhost, m_temp, next(connected));
        }

        ON_STATE(asyncRequest, connected)
        {
            if (!m_ssl)
                ((Socket_base*)(Stream_base*)m_conn)->set_timeout(m_hc->m_timeout);

            return m_hc->request(m_conn, m_req, m_response_body, m_retVal, next(requested));
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
                return next(closed);

            bool keepalive;
            m_retVal->get_keepAlive(keepalive);
            if (keepalive) {
                if (m_http_proxy.empty() || m_http_proxy.c_str()[0] == 's' || !m_sslhost.empty())
                    m_hc->save_conn(m_connUrl, m_conn);
                else
                    m_hc->save_conn(m_http_proxy, m_conn);

                return next(closed);
            }

            return m_conn->close(next(closed));
        }

        ON_STATE(asyncRequest, closed)
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

            m_u->resolve(location, u1);
            m_u = (Url*)(UrlObject_base*)u1;
            m_url.resize(0);
            m_u->toString(m_url);

            if (m_urls.find(m_url) != m_urls.end())
                return CHECK_ERROR(Runtime::setError("HttpClient: redirect cycle"));

            if (m_response_body)
                m_response_body->seek(m_response_pos, fs_base::C_SEEK_SET);

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
        exlib::string m_method;
        obj_ptr<Url> m_u;
        exlib::string m_url;
        exlib::string m_sslhost;
        bool m_ssl;
        exlib::string m_http_proxy;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<SeekableStream_base> m_response_body;
        int64_t m_response_pos;
        obj_ptr<NObject> m_opts;
        obj_ptr<HttpResponse_base>& m_retVal;
        std::unordered_map<exlib::string, bool> m_urls;
        obj_ptr<Stream_base> m_conn;
        obj_ptr<HttpRequest> m_req;
        obj_ptr<HttpRequest> m_reqConn;
        exlib::string m_connUrl;
        obj_ptr<HttpClient> m_hc;
        obj_ptr<Buffer_base> m_buffer;
        int32_t m_temp;
        bool m_reuse;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, method, u, body, response_body, opts, retVal, ac))->post(0);
}

result_t HttpClient::request(exlib::string method, exlib::string url, SeekableStream_base* body,
    SeekableStream_base* response_body, NObject* opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Url> u = new Url();
    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    return request(method, u, body, response_body, opts, retVal, ac);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = holder();
        v8::Local<v8::Context> context = isolate->context();
        obj_ptr<NObject> map = new NObject();
        obj_ptr<SeekableStream_base> stm;
        v8::Local<v8::Object> o;
        JSValue v;
        Variant ct;
        result_t hr;

        ac->m_ctx.resize(5);

        exlib::string _method(method);
        GetConfigValue(isolate, opts, "method", _method, true);
        ac->m_ctx[0] = _method;

        obj_ptr<Url> u = new Url();
        hr = u->parse(url);
        if (hr < 0)
            return hr;

        obj_ptr<Url> u1 = new Url();
        u1->format(opts);

        obj_ptr<UrlObject_base> uo;
        u->resolve(u1, uo);

        u = (Url*)(UrlObject_base*)uo;
        ac->m_ctx[1] = u;

        ac->m_ctx[2] = map;

        hr = GetConfigValue(isolate, opts, "headers", o);
        if (hr >= 0) {
            JSArray ks = o->GetPropertyNames(context);
            int32_t len = ks->Length();
            int32_t i;

            for (i = 0; i < len; i++) {
                JSValue k = ks->Get(context, i);
                JSValue v = o->Get(context, k);

                if (v.IsEmpty())
                    return CALL_E_JAVASCRIPT;

                if (v->IsArray()) {
                    obj_ptr<NArray> arr = new NArray();
                    v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(v);
                    int32_t len1 = a->Length();
                    int32_t i1;

                    for (i1 = 0; i1 < len1; i1++)
                        arr->append(isolate->toString(JSValue(a->Get(context, i1))));

                    map->add(isolate->toString(k), arr);
                } else
                    map->add(isolate->toString(k), isolate->toString(v));
            }
        }

        v = opts->Get(context, isolate->NewString("body", 4));
        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!v->IsUndefined()) {
            stm = SeekableStream_base::getInstance(v);
            if (!stm) {
                obj_ptr<Buffer_base> buf;

                stm = new MemoryStream();

                o.Clear();
                hr = GetArgumentValue(isolate, v, o);
                if (hr >= 0) {
                    exlib::string s;
                    hr = querystring_base::stringify(o, "&", "=", v8::Local<v8::Object>(), s);
                    if (hr < 0)
                        return hr;

                    buf = new Buffer(s.c_str(), s.length());
                    if (map->get("Content-Type", ct) == CALL_RETURN_NULL)
                        map->add("Content-Type", "application/x-www-form-urlencoded");
                } else {
                    hr = GetArgumentValue(isolate, v, buf);
                    if (hr < 0)
                        return hr;
                }

                stm->cc_write(buf);
            }
        } else {
            v = opts->Get(context, isolate->NewString("json", 4));
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            if (v->IsUndefined()) {
                v = opts->Get(context, isolate->NewString("pack", 4));
                if (v.IsEmpty())
                    return CALL_E_JAVASCRIPT;

                if (!v->IsUndefined()) {
                    obj_ptr<Buffer_base> buf;
                    stm = new MemoryStream();

                    hr = msgpack_base::encode(v, buf);
                    if (hr < 0)
                        return hr;

                    stm->cc_write(buf);
                    if (map->get("Content-Type", ct) == CALL_RETURN_NULL)
                        map->add("Content-Type", "application/msgpack");
                }
            } else {
                obj_ptr<Buffer_base> buf;
                stm = new MemoryStream();

                exlib::string s;
                hr = json_base::encode(v, s);
                if (hr < 0)
                    return hr;

                buf = new Buffer(s.c_str(), s.length());
                stm->cc_write(buf);
                if (map->get("Content-Type", ct) == CALL_RETURN_NULL)
                    map->add("Content-Type", "application/json");
            }
        }
        ac->m_ctx[3] = stm;

        obj_ptr<SeekableStream_base> rsp_stm;
        hr = GetConfigValue(isolate, opts, "response_body", rsp_stm);
        if (hr >= 0)
            ac->m_ctx[4] = rsp_stm;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string _method = ac->m_ctx[0].string();
    obj_ptr<Url> u = (Url*)ac->m_ctx[1].object();
    obj_ptr<NObject> map = (NObject*)ac->m_ctx[2].object();
    obj_ptr<SeekableStream_base> stm = SeekableStream_base::getInstance(ac->m_ctx[3].object());
    obj_ptr<SeekableStream_base> rsp_stm = SeekableStream_base::getInstance(ac->m_ctx[4].object());

    return request(_method, u, stm, rsp_stm, map, retVal, ac);
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
}
