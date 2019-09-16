/*
 * HttpRepeater.cpp
 *
 *  Created on: Sep 14, 2019
 *      Author: lion
 */

#include "object.h"
#include "HttpRepeater.h"
#include "HttpResponse.h"

namespace fibjs {

result_t add_url(std::vector<obj_ptr<Url>>& urls, exlib::string& url)
{
    obj_ptr<Url> u = new Url();
    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    if (u->m_hostname.empty())
        return CHECK_ERROR(Runtime::setError("HttpRepeater: hostname is empty."));

    if (!u->m_query.empty())
        return CHECK_ERROR(Runtime::setError("HttpRepeater: query is not empty."));

    if (!u->m_hash.empty())
        return CHECK_ERROR(Runtime::setError("HttpRepeater: hash is not empty."));

    urls.push_back(u);

    return 0;
}

result_t HttpRepeater_base::_new(exlib::string url, obj_ptr<HttpRepeater_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<HttpRepeater> repeater = new HttpRepeater();
    result_t hr = add_url(repeater->m_urls, url);
    if (hr < 0)
        return hr;

    retVal = repeater;
    return 0;
}

result_t HttpRepeater_base::_new(v8::Local<v8::Array> urls, obj_ptr<HttpRepeater_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<HttpRepeater> repeater = new HttpRepeater();
    result_t hr = repeater->load(urls);
    if (hr < 0)
        return hr;

    retVal = repeater;
    return 0;
}

HttpRepeater::HttpRepeater()
{
    m_client = new HttpClient();
    m_client->set_enableCookie(false);
    m_client->set_autoRedirect(false);
    m_client->set_enableEncoding(false);
    m_client->set_userAgent("");

    m_idx = 0;
}

result_t HttpRepeater::load(v8::Local<v8::Array> urls)
{
    std::vector<obj_ptr<Url>> _urls;
    result_t hr;
    int64_t len = urls->Length();

    if (len == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    for (int64_t i = 0; i < len; i++) {
        v8::Local<v8::Value> v = urls->Get(i);
        exlib::string url;

        hr = GetArgumentValue(v, url, true);
        if (hr < 0)
            return hr;

        hr = add_url(_urls, url);
        if (hr < 0)
            return hr;
    }

    m_lock.lock();
    m_urls = _urls;
    m_idx = 0;
    m_lock.unlock();

    return 0;
}

result_t HttpRepeater::get_urls(obj_ptr<NArray>& retVal)
{
    obj_ptr<NArray> a = new NArray();
    exlib::string s;

    m_lock.lock();
    for (int32_t i = 0; i < (int32_t)m_urls.size(); i++) {
        m_urls[i]->toString(s);
        a->append(s);
    }
    m_lock.unlock();

    retVal = a;

    return 0;
}

result_t HttpRepeater::get_client(obj_ptr<HttpClient_base>& retVal)
{
    retVal = m_client;
    return 0;
}

result_t HttpRepeater::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(HttpRepeater* pThis, HttpRequest_base* req, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
        {
            exlib::string v;
            req->get_value(v);

            obj_ptr<Url> u;

            pThis->m_lock.lock();
            u = new Url(*pThis->m_urls[pThis->m_idx++]);
            if (pThis->m_idx >= (int32_t)pThis->m_urls.size())
                pThis->m_idx = 0;
            pThis->m_lock.unlock();

            if (!isUrlSlash(v[0]))
                u->m_pathname.append(1, '/');

            u->m_pathname.append(v);
            u->normalize();

            u->toString(m_url);

            req->get_method(m_method);
            req->get_body(m_body);

            obj_ptr<HttpCollection_base> headers;
            req->get_headers(headers);

            headers->remove("Host");
            headers->remove("Connection");

            headers->all("", m_headers);

            req->get_response(m_rep);

            set(request);
        }

        static int32_t request(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            pThis->set(response);
            return pThis->m_pThis->m_client->request(pThis->m_method, pThis->m_url,
                pThis->m_body, pThis->m_headers, pThis->m_ret, pThis);
        }

        static int32_t response(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
            int32_t code;
            exlib::string msg;
            obj_ptr<NObject> headers;
            obj_ptr<SeekableStream_base> body;

            pThis->m_ret->get_statusCode(code);
            pThis->m_rep->set_statusCode(code);

            pThis->m_ret->get_statusMessage(msg);
            pThis->m_rep->set_statusMessage(msg);

            pThis->m_ret->allHeader("", headers);
            ((HttpResponse*)(HttpResponse_base*)pThis->m_rep)->addHeader(headers);

            pThis->m_ret->get_body(body);
            pThis->m_rep->set_body(body);

            return pThis->done(CALL_RETURN_NULL);
        }

    public:
        obj_ptr<HttpRepeater> m_pThis;
        exlib::string m_method;
        exlib::string m_url;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<NObject> m_headers;
        obj_ptr<HttpResponse_base> m_ret;
        obj_ptr<HttpResponse_base> m_rep;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);

    if (req == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, req, ac))->post(0);
}
}