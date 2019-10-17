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
#include "ifs/net.h"
#include "ifs/zlib.h"
#include "ifs/json.h"
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

result_t HttpClient::get_proxyAgent(exlib::string& retVal)
{
    retVal = m_proxyAgent;
    return 0;
}

result_t HttpClient::set_proxyAgent(exlib::string newVal)
{
    if (newVal.empty()) {
        m_proxyAgent.clear();
        m_proxyConnUrl.clear();
        m_proxyAgentUrl.Release();
    } else {
        obj_ptr<Url> u = new Url();
        result_t hr;
        exlib::string connUrl;
        bool ssl = false;

        hr = u->parse(newVal);
        if (hr < 0)
            return hr;

        if (u->m_protocol == "https:") {
            ssl = true;
            connUrl = "ssl://";
        } else if (u->m_protocol == "http:")
            connUrl = "tcp://";
        else
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol"));

        if (u->m_host.empty())
            return CHECK_ERROR(Runtime::setError("HttpClient: unknown host"));

        connUrl.append(u->m_host);

        if (!u->m_port.empty()) {
            connUrl.append(":", 1);
            connUrl.append(u->m_port);
        } else
            connUrl.append(ssl ? ":443" : ":80");

        m_proxyAgent = newVal;
        m_proxyConnUrl = connUrl;
        m_proxyAgentUrl = u;
    }

    date_t d;

    d.now();
    d.add(m_poolTimeout, date_t::_MICROSECOND);
    clean_coon(d);

    return 0;
}

result_t HttpClient::update(HttpCookie_base* cookie)
{
    int32_t length, i;
    exlib::string str, str1;
    bool b = false, b1 = false;
    date_t t, t1;

    m_cookies->get_length(length);
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

    cookies->get_length(length);
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
    m_cookies->get_length(length);
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

result_t HttpClient::request(Stream_base* conn, HttpRequest_base* req,
    obj_ptr<HttpResponse_base>& retVal,
    AsyncEvent* ac)
{
    class asyncRequest : public AsyncState {
    public:
        asyncRequest(Stream_base* conn, HttpRequest_base* req, int32_t maxBodySize, bool enableEncoding,
            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_conn(conn)
            , m_req(req)
            , m_maxBodySize(maxBodySize)
            , m_enableEncoding(enableEncoding)
            , m_retVal(retVal)
        {
            next(send);

            exlib::string method;
            m_req->get_method(method);
            m_bNoBody = !qstricmp(method.c_str(), "head", 4);
        }

        static int32_t send(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;
            return pThis->m_req->sendTo(pThis->m_conn, pThis->next(recv));
        }

        static int32_t recv(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;

            obj_ptr<HttpResponse> resp = new HttpResponse();
            resp->m_message->m_bNoBody = pThis->m_bNoBody;

            pThis->m_retVal = resp;
            pThis->m_retVal->set_maxBodySize(pThis->m_maxBodySize);
            pThis->m_bs = new BufferedStream(pThis->m_conn);
            pThis->m_bs->set_EOL("\r\n");

            return pThis->m_retVal->readFrom(pThis->m_bs, pThis->next(pThis->m_enableEncoding ? unzip : close));
        }

        static int32_t unzip(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;

            exlib::string hdr;

            if (pThis->m_retVal->firstHeader("Content-Encoding", hdr) != CALL_RETURN_NULL) {
                pThis->m_retVal->removeHeader("Content-Encoding");

                pThis->m_retVal->get_body(pThis->m_body);
                pThis->m_unzip = new MemoryStream();

                if (hdr == "gzip")
                    return zlib_base::gunzipTo(pThis->m_body, pThis->m_unzip,
                        pThis->m_maxBodySize, pThis->next(close));
                else if (hdr == "deflate")
                    return zlib_base::inflateRawTo(pThis->m_body, pThis->m_unzip,
                        pThis->m_maxBodySize, pThis->next(close));
            }

            return pThis->next(close);
        }

        static int32_t close(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;

            if (pThis->m_unzip) {
                pThis->m_unzip->rewind();
                pThis->m_retVal->set_body(pThis->m_unzip);
            }

            return pThis->next();
        }

    private:
        Stream_base* m_conn;
        HttpRequest_base* m_req;
        int32_t m_maxBodySize;
        bool m_enableEncoding;
        obj_ptr<HttpResponse_base>& m_retVal;
        obj_ptr<BufferedStream> m_bs;
        obj_ptr<MemoryStream> m_unzip;
        obj_ptr<SeekableStream_base> m_body;
        bool m_bNoBody;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(conn, req, m_maxBodySize, m_enableEncoding, retVal, ac))->post(0);
}

result_t HttpClient::request(exlib::string method, exlib::string url, SeekableStream_base* body,
    NObject* headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    class asyncRequest : public AsyncState {
    public:
        asyncRequest(HttpClient* hc, exlib::string method, exlib::string url,
            SeekableStream_base* body, NObject* headers,
            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_method(method)
            , m_url(url)
            , m_body(body)
            , m_headers(headers)
            , m_retVal(retVal)
            , m_hc(hc)
        {
            next(prepare);
        }

        static int32_t prepare(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;

            result_t hr;
            bool ssl = false;

            pThis->m_urls[pThis->m_url] = true;

            obj_ptr<Url> u = new Url();
            exlib::string path;
            exlib::string cookie;

            hr = u->parse(pThis->m_url);
            if (hr < 0)
                return hr;

            if (u->m_protocol == "https:") {
                ssl = true;
                pThis->m_connUrl = "ssl://";
            } else if (u->m_protocol == "http:")
                pThis->m_connUrl = "tcp://";
            else
                return CHECK_ERROR(Runtime::setError("HttpClient: unknown protocol"));

            if (u->m_host.empty())
                return CHECK_ERROR(Runtime::setError("HttpClient: unknown host"));

            pThis->m_connUrl.append(u->m_host);

            if (!u->m_port.empty()) {
                pThis->m_connUrl.append(":", 1);
                pThis->m_connUrl.append(u->m_port);
            } else
                pThis->m_connUrl.append(ssl ? ":443" : ":80");

            pThis->m_req = new HttpRequest();

            pThis->m_req->set_method(pThis->m_method);

            if (pThis->m_hc->m_proxyConnUrl.empty()) {
                u->get_path(path);
                pThis->m_req->set_address(path);
            } else
                pThis->m_req->set_address(pThis->m_url);

            bool enableEncoding = false;
            pThis->m_hc->get_enableEncoding(enableEncoding);
            if (enableEncoding)
                pThis->m_req->addHeader("Accept-Encoding", "gzip,deflate");

            bool enableCookie = false;
            pThis->m_hc->get_enableCookie(enableCookie);
            if (enableCookie) {
                pThis->m_hc->get_cookie(pThis->m_url, cookie);
                if (cookie.length() > 0)
                    pThis->m_req->addHeader("Cookie", cookie);
            }

            if (pThis->m_headers)
                pThis->m_req->addHeader(pThis->m_headers);

            exlib::string a = pThis->m_hc->agent();
            if (!a.empty()) {
                bool bCheck = false;
                pThis->m_req->hasHeader("User-Agent", bCheck);
                if (!bCheck)
                    pThis->m_req->addHeader("User-Agent", a);
            }

            bool bHost = false;
            pThis->m_req->hasHeader("Host", bHost);
            if (!bHost)
                pThis->m_req->addHeader("Host", u->m_host);

            if (pThis->m_body)
                pThis->m_req->set_body(pThis->m_body);

            if (pThis->m_hc->get_conn(pThis->m_connUrl, pThis->m_conn))
                return pThis->next(connected);

            if (pThis->m_hc->m_proxyConnUrl.empty())
                return net_base::connect(pThis->m_connUrl, pThis->m_hc->m_timeout, pThis->m_conn, pThis->next(connected));
            else {
                if (ssl) {
                    exlib::string host = pThis->m_connUrl.substr(6);
                    pThis->m_reqConn = new HttpRequest();

                    pThis->m_reqConn->set_method("CONNECT");
                    pThis->m_reqConn->set_address(host);
                    pThis->m_reqConn->addHeader("Host", host);
                    pThis->m_host = u->m_hostname;

                    exlib::string a = pThis->m_hc->agent();
                    if (!a.empty())
                        pThis->m_reqConn->addHeader("User-Agent", a);
                }

                if (pThis->m_hc->get_conn(pThis->m_hc->m_proxyConnUrl, pThis->m_conn))
                    return pThis->next(ssl ? ssl_connect : connected);

                return net_base::connect(pThis->m_hc->m_proxyConnUrl,
                    pThis->m_hc->m_timeout, pThis->m_conn, pThis->next(ssl ? ssl_connect : connected));
            }
        }

        static int32_t ssl_connect(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;
            return pThis->m_hc->request(pThis->m_conn, pThis->m_reqConn, pThis->m_retVal, pThis->next(ssl_handshake));
        }

        static int32_t ssl_handshake(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;
            int32_t status;
            result_t hr;

            hr = pThis->m_retVal->get_statusCode(status);
            if (hr < 0)
                return hr;

            if (status != 200) {
                exlib::string msg;

                pThis->m_retVal->get_statusMessage(msg);
                return CHECK_ERROR(Runtime::setError("HttpClient: " + msg));
            }

            pThis->m_reqConn.Release();
            pThis->m_retVal.Release();

            obj_ptr<SslSocket> ss = new SslSocket();
            obj_ptr<Stream_base> conn = pThis->m_conn;
            pThis->m_conn = ss;

            if (g_ssl.m_crt && g_ssl.m_key) {
                result_t hr = ss->setCert("", g_ssl.m_crt, g_ssl.m_key);
                if (hr < 0)
                    return hr;
            }

            return ss->connect(conn, pThis->m_host, pThis->m_temp, pThis->next(connected));
        }

        static int32_t connected(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;
            return pThis->m_hc->request(pThis->m_conn, pThis->m_req, pThis->m_retVal, pThis->next(requested));
        }

        static int32_t requested(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;
            bool enableCookie = false;

            bool upgrade;
            pThis->m_retVal->get_upgrade(upgrade);
            if (upgrade)
                return pThis->next(closed);

            pThis->m_hc->get_enableCookie(enableCookie);
            if (enableCookie) {
                obj_ptr<NArray> cookies;
                pThis->m_retVal->get_cookies(cookies);
                pThis->m_hc->update_cookies(pThis->m_url, cookies);
            }

            bool keepalive;
            pThis->m_retVal->get_keepAlive(keepalive);
            if (keepalive) {
                if (pThis->m_hc->m_proxyConnUrl.empty() || !pThis->m_host.empty())
                    pThis->m_hc->save_conn(pThis->m_connUrl, pThis->m_conn);
                else
                    pThis->m_hc->save_conn(pThis->m_hc->m_proxyConnUrl, pThis->m_conn);

                return pThis->next(closed);
            }

            return pThis->m_conn->close(pThis->next(closed));
        }

        static int32_t closed(AsyncState* pState, int32_t n)
        {
            asyncRequest* pThis = (asyncRequest*)pState;

            result_t hr;
            int32_t status;
            exlib::string location;
            obj_ptr<Url> u = new Url();
            obj_ptr<UrlObject_base> u1;

            hr = pThis->m_retVal->get_statusCode(status);
            if (hr < 0)
                return hr;

            if (!pThis->m_hc->m_autoRedirect || (status != 302 && status != 301))
                return pThis->next();

            hr = pThis->m_retVal->firstHeader("location", location);
            if (hr < 0)
                return hr;

            u->parse(pThis->m_url);
            u->resolve(location, u1);
            location.resize(0);
            u1->toString(location);

            if (pThis->m_urls.find(location) != pThis->m_urls.end())
                return CHECK_ERROR(Runtime::setError("HttpClient: redirect cycle"));

            pThis->m_url = location;

            return pThis->next(prepare);
        }

    private:
        exlib::string m_method;
        exlib::string m_url;
        exlib::string m_host;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<NObject> m_headers;
        obj_ptr<HttpResponse_base>& m_retVal;
        std::map<exlib::string, bool> m_urls;
        obj_ptr<Stream_base> m_conn;
        obj_ptr<HttpRequest> m_req;
        obj_ptr<HttpRequest> m_reqConn;
        exlib::string m_connUrl;
        obj_ptr<HttpClient> m_hc;
        int32_t m_temp;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, method, url, body, headers, retVal, ac))->post(0);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
    v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    static const char* s_keys[] = {
        "query", "headers", "body", "json", NULL
    };

    if (ac->isSync()) {
        Isolate* isolate = holder();
        obj_ptr<NObject> map = new NObject();
        obj_ptr<SeekableStream_base> stm;
        v8::Local<v8::Object> o;
        JSValue v;
        result_t hr;

        hr = CheckConfig(opts, s_keys);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(3);

        v = opts->Get(isolate->NewString("query", 5));
        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;
        hr = GetArgumentValue(v, o);
        if (hr >= 0) {
            exlib::string s;
            hr = querystring_base::stringify(o, "&", "=", v8::Local<v8::Object>(), s);
            if (hr < 0)
                return hr;
            ac->m_ctx[0] = s;
        } else
            ac->m_ctx[0] = "";

        ac->m_ctx[1] = map;

        o.Clear();
        v = opts->Get(isolate->NewString("headers", 7));
        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;
        hr = GetArgumentValue(v, o);
        if (hr >= 0) {
            JSArray ks = o->GetPropertyNames();
            int32_t len = ks->Length();
            int32_t i;

            for (i = 0; i < len; i++) {
                JSValue k = ks->Get(i);
                JSValue v = o->Get(k);

                if (v.IsEmpty())
                    return CALL_E_JAVASCRIPT;

                if (v->IsArray()) {
                    obj_ptr<NArray> arr = new NArray();
                    v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(v);
                    int32_t len1 = a->Length();
                    int32_t i1;

                    for (i1 = 0; i1 < len1; i1++)
                        arr->append(ToCString(v8::String::Utf8Value(a->Get(i1))));

                    map->add(ToCString(v8::String::Utf8Value(k)), arr);
                } else
                    map->add(ToCString(v8::String::Utf8Value(k)),
                        ToCString(v8::String::Utf8Value(v)));
            }
        }

        v = opts->Get(isolate->NewString("body", 4));
        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!v->IsUndefined()) {
            stm = SeekableStream_base::getInstance(v);
            if (!stm) {
                obj_ptr<Buffer_base> buf;

                stm = new MemoryStream();

                o.Clear();
                hr = GetArgumentValue(v, o);
                if (hr >= 0) {
                    exlib::string s;
                    hr = querystring_base::stringify(o, "&", "=", v8::Local<v8::Object>(), s);
                    if (hr < 0)
                        return hr;

                    buf = new Buffer(s);
                    map->add("Content-Type", "application/x-www-form-urlencoded");
                } else {
                    hr = GetArgumentValue(isolate->m_isolate, v, buf);
                    if (hr < 0)
                        return hr;
                }

                stm->cc_write(buf);
            }
        } else {
            v = opts->Get(isolate->NewString("json", 4));
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;
            if (!v->IsUndefined()) {
                obj_ptr<Buffer_base> buf;

                stm = new MemoryStream();

                exlib::string s;
                hr = json_base::encode(v, s);
                if (hr < 0)
                    return hr;

                buf = new Buffer(s);
                stm->cc_write(buf);
                map->add("Content-Type", "application/json");
            }
        }
        ac->m_ctx[2] = stm;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string query = ac->m_ctx[0].string();
    obj_ptr<NObject> map = (NObject*)ac->m_ctx[1].object();
    obj_ptr<SeekableStream_base> stm = SeekableStream_base::getInstance(ac->m_ctx[2].object());

    if (!query.empty())
        url = url + '?' + query;

    return request(method, url, stm, map, retVal, ac);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("GET", url, headers, retVal, ac);
}

result_t HttpClient::post(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("POST", url, headers, retVal, ac);
}

result_t HttpClient::del(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("DELETE", url, headers, retVal, ac);
}

result_t HttpClient::put(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("PUT", url, headers, retVal, ac);
}

result_t HttpClient::patch(exlib::string url, v8::Local<v8::Object> headers,
    obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{
    return request("PATCH", url, headers, retVal, ac);
}
}
