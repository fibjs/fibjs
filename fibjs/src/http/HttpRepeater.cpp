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

    if (u->hostname().empty())
        return CHECK_ERROR(Runtime::setError("HttpRepeater: hostname is empty."));

    if (!u->search().empty())
        return CHECK_ERROR(Runtime::setError("HttpRepeater: query is not empty."));

    if (!u->hash().empty())
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

result_t HttpRepeater_base::_new(std::vector<exlib::string>& urls, obj_ptr<HttpRepeater_base>& retVal, v8::Local<v8::Object> This)
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
    Isolate* isolate = holder();
    m_client = new HttpClient(isolate->m_ctx);
    m_client->set_enableCookie(false);
    m_client->set_autoRedirect(false);
    m_client->set_enableEncoding(false);
    m_client->set_userAgent("");

    m_idx = 0;
}

result_t HttpRepeater::load(std::vector<exlib::string>& urls)
{
    std::vector<obj_ptr<Url>> _urls;
    result_t hr;
    int32_t len = urls.size();
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    if (len == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    for (int32_t i = 0; i < len; i++) {
        hr = add_url(_urls, urls[i]);
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

result_t HttpRepeater::isRouting(bool& retVal)
{
    retVal = true;
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

            exlib::string pathname = u->pathname();
            if (!isUrlSlash(v[0]))
                pathname.append(1, '/');

            pathname.append(v);
            u->set_pathname(pathname);

            exlib::string query;
            req->get_queryString(query);
            u->set_search(query);

            u->toString(m_url);

            req->get_method(m_method);
            req->get_body(m_body);

            req->get_headers(m_headers);

            m_headers->remove("Host");
            m_headers->remove("Connection");

            req->get_response(m_rep);

            next(request);
        }

        ON_STATE(asyncInvoke, request)
        {
            return m_pThis->m_client->request(m_method, m_url,
                m_body, NULL, m_headers, m_ret, next(response));
        }

        ON_STATE(asyncInvoke, response)
        {
            int32_t code;
            exlib::string msg;
            obj_ptr<Headers_base> headers;
            obj_ptr<SeekableStream_base> body;

            m_ret->get_statusCode(code);
            m_rep->set_statusCode(code);

            m_ret->get_statusMessage(msg);
            m_rep->set_statusMessage(msg);

            m_ret->get_headers(headers);
            m_rep.As<HttpResponse>()->appendHeader(headers);

            m_ret->get_body(body);
            m_rep->set_body(body);

            return next(CALL_RETURN_NULL);
        }

    public:
        obj_ptr<HttpRepeater> m_pThis;
        exlib::string m_method;
        exlib::string m_url;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<Headers_base> m_headers;
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