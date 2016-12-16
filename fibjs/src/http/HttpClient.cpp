/*
 * HttpClient.cpp
 *
 *  Created on: Aug 12, 2016
 */


#include "object.h"
#include "HttpClient.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "Url.h"
#include "Map.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "map"
#include "ifs/net.h"
#include "ifs/zlib.h"

namespace fibjs
{

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

result_t HttpClient::get_cookies(obj_ptr<List_base>& retVal)
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

result_t HttpClient::update(obj_ptr<HttpCookie> cookie)
{
    int32_t length, i;
    exlib::string str, str1;
    bool b = false, b1 = false;
    date_t t, t1;

    m_cookies->get_length(length);
    if (length == 0)
        return 0;

    for (i = 0; i < length; i++)
    {
        Variant v;
        obj_ptr<HttpCookie> hc;

        m_cookies->_indexed_getter(i, v);
        hc = (HttpCookie*)v.object();

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

result_t HttpClient::update_cookies(exlib::string url, obj_ptr<List_base> cookies)
{
    result_t hr;
    int32_t length, i;
    bool match = false;
    exlib::string domain;

    obj_ptr<Url> u = new Url();
    hr = u->parse(url);
    if (hr < 0)
        return hr;

    cookies->get_length(length);
    if (length == 0)
        return 0;

    for (i = 0; i < length; i++)
    {
        Variant v;
        obj_ptr<HttpCookie> hc;

        cookies->_indexed_getter(i, v);
        hc = (HttpCookie*)v.object();

        hc->match(url, match);
        if (!match)
            continue;

        hc->get_domain(domain);
        if (domain.empty())
            hc->set_domain(u->m_hostname);

        if (update(hc) == 0)
            m_cookies->append(hc);
    }
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

    m_cookies->get_length(length);
    if (length == 0)
        return 0;

    obj_ptr<Url> u = new Url();
    hr = u->parse(url);
    if (hr < 0)
        return hr;

    now.now();

    for (i = 0; i < length; i++)
    {
        Variant v;
        obj_ptr<HttpCookie> hc;
        date_t date;

        m_cookies->_indexed_getter(i, v);
        hc = (HttpCookie*)v.object();
        hc->get_expires(date);
        if (!date.empty() && date.diff(now) < 0)
            continue;

        hc->get_secure(secure);
        if (secure && u->m_protocol != "https:")
            continue;

        hc->match(url, match);
        if (match)
        {
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

    return 0;
}


result_t HttpClient::request(Stream_base *conn, HttpRequest_base *req,
                             obj_ptr<HttpResponse_base> &retVal,
                             AsyncEvent *ac)
{
    class asyncRequest: public AsyncState
    {
    public:
        asyncRequest(Stream_base *conn, HttpRequest_base *req,
                     obj_ptr<HttpResponse_base> &retVal, AsyncEvent *ac) :
            AsyncState(ac), m_conn(conn), m_req(req), m_retVal(retVal)
        {
            set(send);
        }

        static int32_t send(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(recv);
            return pThis->m_req->sendTo(pThis->m_conn, pThis);
        }

        static int32_t recv(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->m_retVal = new HttpResponse();
            pThis->m_bs = new BufferedStream(pThis->m_conn);
            pThis->m_bs->set_EOL("\r\n");

            pThis->set(unzip);
            return pThis->m_retVal->readFrom(pThis->m_bs, pThis);
        }

        static int32_t unzip(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(close);

            Variant hdr;

            if (pThis->m_retVal->firstHeader("Content-Encoding",
                                             hdr) != CALL_RETURN_NULL)
            {
                pThis->m_retVal->removeHeader("Content-Encoding");

                pThis->m_retVal->get_body(pThis->m_body);
                pThis->m_unzip = new MemoryStream();

                exlib::string str = hdr.string();

                if (str == "gzip")
                    return zlib_base::gunzipTo(pThis->m_body,
                                               pThis->m_unzip, pThis);
                else if (str == "deflate")
                    return zlib_base::inflateRawTo(pThis->m_body,
                                                   pThis->m_unzip, pThis);
            }

            return 0;
        }

        static int32_t close(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            if (pThis->m_unzip)
            {
                pThis->m_unzip->rewind();
                pThis->m_retVal->set_body(pThis->m_unzip);
            }

            return pThis->done();
        }

    private:
        Stream_base *m_conn;
        HttpRequest_base *m_req;
        obj_ptr<HttpResponse_base> &m_retVal;
        obj_ptr<BufferedStream> m_bs;
        obj_ptr<MemoryStream> m_unzip;
        obj_ptr<SeekableStream_base> m_body;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(conn, req, retVal, ac))->post(0);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
                             SeekableStream_base* body, Map_base* headers,
                             obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{

    class asyncRequest: public AsyncState
    {
    public:
        asyncRequest(HttpClient *hc, exlib::string method, exlib::string url,
                     SeekableStream_base* body, Map_base* headers,
                     obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) :
            AsyncState(ac), m_method(method), m_url(url), m_body(body),
            m_headers(headers), m_retVal(retVal), m_hc(hc)
        {
            set(prepare);
        }

        static int32_t prepare(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            result_t hr;
            bool ssl = false;

            pThis->m_urls[pThis->m_url] = true;

            obj_ptr<Url> u = new Url();
            exlib::string path;
            exlib::string cookie;

            hr = u->parse(pThis->m_url);
            if (hr < 0)
                return hr;

            if (u->m_protocol == "https:")
            {
                ssl = true;
                pThis->m_connUrl = "ssl://";
            }
            else if (u->m_protocol == "http:")
                pThis->m_connUrl = "tcp://";
            else
                return CHECK_ERROR(Runtime::setError("http: unknown protocol"));

            if (u->m_host.empty())
                return CHECK_ERROR(Runtime::setError("http: unknown host"));

            pThis->m_connUrl.append(u->m_host);

            if (!u->m_port.empty())
            {
                pThis->m_connUrl.append(":", 1);
                pThis->m_connUrl.append(u->m_port);
            }
            else
                pThis->m_connUrl.append(ssl ? ":443" : ":80");

            pThis->m_req = new HttpRequest();

            pThis->m_req->set_method(pThis->m_method);

            u->get_path(path);
            pThis->m_req->set_address(path);

            pThis->m_req->addHeader("host", u->m_host);
            pThis->m_req->setHeader("Accept-Encoding", "gzip,deflate");
            pThis->m_hc->get_cookie(pThis->m_url, cookie);
            if (cookie.length() > 0)
                pThis->m_req->setHeader("Cookie", cookie);
            pThis->m_req->addHeader(pThis->m_headers);
            pThis->m_hc->setAgent(pThis->m_req);

            if (pThis->m_body)
                pThis->m_req->set_body(pThis->m_body);

            pThis->set(connected);
            return net_base::connect(pThis->m_connUrl, pThis->m_hc->m_timeout, pThis->m_conn, pThis);
        }

        static int32_t connected(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(requested);
            return pThis->m_hc->request(pThis->m_conn, pThis->m_req, pThis->m_retVal, pThis);
        }

        static int32_t requested(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;
            bool enableCookie = false;

            pThis->set(closed);

            bool upgrade;
            pThis->m_retVal->get_upgrade(upgrade);
            if (upgrade)
                return 0;

            pThis->m_hc->get_enableCookie(enableCookie);
            if (enableCookie)
            {
                obj_ptr<List_base> cookies;
                pThis->m_retVal->get_cookies(cookies);
                pThis->m_hc->update_cookies(pThis->m_url, cookies);
            }

            return pThis->m_conn->close(pThis);
        }

        static int32_t closed(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            result_t hr;
            int32_t status;
            exlib::string location;
            Variant v;
            obj_ptr<Url> u = new Url();
            obj_ptr<UrlObject_base> u1;

            hr = pThis->m_retVal->get_status(status);
            if (hr < 0)
                return hr;

            if (!pThis->m_hc->m_autoRedirect || (status != 302 && status != 301))
                return pThis->done(0);

            hr = pThis->m_retVal->firstHeader("location", v);
            if (hr < 0)
                return hr;

            location = v.string();
            u->parse(pThis->m_url);
            u->resolve(location, u1);
            location.resize(0);
            u1->toString(location);

            if (pThis->m_urls.find(location) != pThis->m_urls.end())
                return CHECK_ERROR(Runtime::setError("http: redirect cycle"));

            pThis->m_url = location;

            pThis->set(prepare);
            return 0;
        }

    private:
        exlib::string m_method;
        exlib::string m_url;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<Map_base> m_headers;
        obj_ptr<HttpResponse_base>& m_retVal;
        std::map<exlib::string, bool> m_urls;
        obj_ptr<Stream_base> m_conn;
        obj_ptr<HttpRequest> m_req;
        exlib::string m_connUrl;
        obj_ptr<HttpClient> m_hc;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(this, method, url, body, headers, retVal, ac))->post(0);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
                             SeekableStream_base *body, v8::Local<v8::Object> headers,
                             obj_ptr<HttpResponse_base> &retVal)
{
    obj_ptr<Map_base> map = new Map();
    map->put(headers);
    return ac_request(method, url, body, map, retVal);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
                             v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base> &retVal)
{
    return request(method, url, (SeekableStream_base *) NULL, headers, retVal);
}

result_t HttpClient::request(exlib::string method, exlib::string url,
                             Buffer_base *body, v8::Local<v8::Object> headers,
                             obj_ptr<HttpResponse_base> &retVal)
{
    obj_ptr<SeekableStream_base> stm = new MemoryStream();
    stm->write(body, NULL);
    return request(method, url, stm, headers, retVal);
}

result_t HttpClient::get(exlib::string url, v8::Local<v8::Object> headers,
                         obj_ptr<HttpResponse_base> &retVal)
{
    return request("GET", url, headers, retVal);
}

result_t HttpClient::post(exlib::string url, Buffer_base *body,
                          v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base> &retVal)
{
    return request("POST", url, body, headers, retVal);
}

result_t HttpClient::post(exlib::string url, SeekableStream_base *body,
                          v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base> &retVal)
{
    return request("POST", url, body, headers, retVal);
}

}
