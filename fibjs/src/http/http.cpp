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
#include "Map.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "map"
#include "ifs/zlib.h"

namespace fibjs
{

DECLARE_MODULE(http);

result_t http_base::request(Stream_base *conn, HttpRequest_base *req,
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

result_t http_base::request(const char* method, const char* url,
                            SeekableStream_base* body, Map_base* headers,
                            obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac)
{

    class asyncRequest: public AsyncState
    {
    public:
        asyncRequest(const char* method, const char* url,
                     SeekableStream_base* body, Map_base* headers,
                     obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) :
            AsyncState(ac), m_method(method), m_url(url), m_body(body),
            m_headers(headers), m_retVal(retVal)
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

            hr = u->parse(pThis->m_url.c_str());
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

            pThis->m_req->set_method(pThis->m_method.c_str());

            u->get_path(path);
            pThis->m_req->set_address(path.c_str());

            pThis->m_req->addHeader("host", u->m_host.c_str());
            pThis->m_req->setHeader("Accept-Encoding", "gzip,deflate");
            pThis->m_req->addHeader(pThis->m_headers);

            if (pThis->m_body)
                pThis->m_req->set_body(pThis->m_body);

            pThis->set(connected);
            return net_base::connect(pThis->m_connUrl.c_str(), pThis->m_conn, pThis);
        }

        static int32_t connected(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(requested);
            return request(pThis->m_conn, pThis->m_req, pThis->m_retVal, pThis);
        }

        static int32_t requested(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(closed);

            bool upgrade;
            pThis->m_retVal->get_upgrade(upgrade);
            if (upgrade)
                return 0;

            return pThis->m_conn->close(pThis);
        }

        static int32_t closed(AsyncState *pState, int32_t n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            result_t hr;
            int32_t status;
            exlib::string location;
            Variant v;

            hr = pThis->m_retVal->get_status(status);
            if (hr < 0)
                return hr;

            if (status != 302 && status != 301)
                return pThis->done(0);

            hr = pThis->m_retVal->firstHeader("location", v);
            if (hr < 0)
                return hr;

            location = v.string();

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
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRequest(method, url, body, headers, retVal, ac))->post(0);
}

result_t http_base::request(const char *method, const char *url,
                            SeekableStream_base *body, v8::Local<v8::Object> headers,
                            obj_ptr<HttpResponse_base> &retVal)
{
    obj_ptr<Map_base> map = new Map();
    map->put(headers);
    return ac_request(method, url, body, map, retVal);
}

result_t http_base::request(const char *method, const char *url,
                            v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base> &retVal)
{
    return request(method, url, (SeekableStream_base *) NULL, headers, retVal);
}

result_t http_base::request(const char *method, const char *url,
                            Buffer_base *body, v8::Local<v8::Object> headers,
                            obj_ptr<HttpResponse_base> &retVal)
{
    obj_ptr<SeekableStream_base> stm = new MemoryStream();
    stm->write(body, NULL);
    return request(method, url, stm, headers, retVal);
}

result_t http_base::get(const char *url, v8::Local<v8::Object> headers,
                        obj_ptr<HttpResponse_base> &retVal)
{
    return request("GET", url, headers, retVal);
}

result_t http_base::post(const char *url, Buffer_base *body,
                         v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base> &retVal)
{
    return request("POST", url, body, headers, retVal);
}

result_t http_base::post(const char *url, SeekableStream_base *body,
                         v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base> &retVal)
{
    return request("POST", url, body, headers, retVal);
}

}
