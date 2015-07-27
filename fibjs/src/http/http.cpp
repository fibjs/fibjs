/*
 * http.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "ifs/http.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "Url.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "map"
#include "ifs/zlib.h"

namespace fibjs
{

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

                std::string str = hdr.string();

                if (!qstrcmp(str.c_str(), "gzip"))
                    return zlib_base::gunzipTo(pThis->m_body,
                                               pThis->m_unzip, pThis);
                else if (!qstrcmp(str.c_str(), "deflate"))
                    return zlib_base::inflateTo(pThis->m_body,
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

result_t http_base::request(const char *method, const char *url,
                            SeekableStream_base *body, v8::Local<v8::Object> headers,
                            obj_ptr<HttpResponse_base> &retVal)
{
    std::string location;
    std::map<std::string, bool> urls;

    while (true)
    {
        urls[url] = true;

        result_t hr;
        int32_t status;
        bool ssl = false;
        obj_ptr<HttpRequest> req;
        Variant v;
        std::string connUrl;

        {
            obj_ptr<Url> u = new Url();
            std::string path;

            hr = u->parse(url);
            if (hr < 0)
                return hr;

            if (!qstrcmp(u->m_protocol.c_str(), "https:"))
            {
                ssl = true;
                connUrl = "ssl://";
            }
            else if (!qstrcmp(u->m_protocol.c_str(), "http:"))
                connUrl = "tcp://";
            else
                return CHECK_ERROR(Runtime::setError("http: unknown protocol"));

            if (u->m_host.empty())
                return CHECK_ERROR(Runtime::setError("http: unknown host"));

            connUrl.append(u->m_host);

            if (!u->m_port.empty())
            {
                connUrl.append(":", 1);
                connUrl.append(u->m_port);
            }
            else
                connUrl.append(ssl ? ":443" : ":80");

            req = new HttpRequest();

            req->set_method(method);

            u->get_path(path);
            req->set_address(path.c_str());

            req->addHeader("host", u->m_host.c_str());
            req->setHeader("Accept-Encoding", "gzip,deflate");
            req->addHeader(headers);

            if (body)
                req->set_body(body);
        }

        {
            obj_ptr<Stream_base> conn;
            hr = net_base::ac_connect(connUrl.c_str(), conn);
            if (hr < 0)
                return hr;

            hr = ac_request(conn, req, retVal);
            if (hr < 0)
                return hr;

            conn->ac_close();
        }

        hr = retVal->get_status(status);
        if (hr < 0)
            return hr;

        if (status != 302 && status != 301)
            return 0;

        hr = retVal->firstHeader("location", v);
        if (hr < 0)
            return hr;

        location = v.string();

        if (urls.find(location) != urls.end())
            return CHECK_ERROR(Runtime::setError("http: redirect cycle"));

        url = location.c_str();
    }

    return 0;
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
