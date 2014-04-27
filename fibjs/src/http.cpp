/*
 * http.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "HttpHandler.h"
#include "HttpFileHandler.h"
#include "JSHandler.h"
#include "ifs/http.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "Socket.h"
#include "Url.h"
#include "HttpRequest.h"
#include "SslSocket.h"
#include "BufferedStream.h"
#include "map"

namespace fibjs
{

result_t http_base::fileHandler(const char *root, obj_ptr<Handler_base> &retVal)
{
    retVal = new HttpFileHandler(root);
    return 0;
}

result_t http_base::request(const char *host, int32_t port, HttpRequest_base *req,
                            bool ssl, obj_ptr<HttpResponse_base> &retVal,
                            exlib::AsyncEvent *ac)
{
    class asyncRequest: public asyncState
    {
    public:
        asyncRequest(const char *host, int32_t port, HttpRequest_base *req, bool ssl,
                     obj_ptr<HttpResponse_base> &retVal, exlib::AsyncEvent *ac) :
            asyncState(ac), m_host(host), m_port(port), m_req(req), m_ssl(ssl), m_retVal(
                retVal)
        {
            set(connect);
        }

        static int connect(asyncState *pState, int n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->m_sock = new Socket();
            pThis->m_strm = pThis->m_sock;

            pThis->m_sock->create(net_base::_AF_INET, net_base::_SOCK_STREAM);

            pThis->set(handshake);
            return pThis->m_sock->connect(pThis->m_host, pThis->m_port, pThis);
        }

        static int handshake(asyncState *pState, int n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(send);

            if (!pThis->m_ssl)
                return 0;

            pThis->m_ssl_sock = new SslSocket();
            pThis->m_strm = pThis->m_ssl_sock;
            return pThis->m_ssl_sock->connect(pThis->m_sock, "", pThis->m_temp, pThis);
        }

        static int send(asyncState *pState, int n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(recv);
            return pThis->m_req->sendTo(pThis->m_strm, pThis);
        }

        static int recv(asyncState *pState, int n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->m_retVal = new HttpResponse();
            pThis->m_bs = new BufferedStream(pThis->m_strm);
            pThis->m_bs->set_EOL("\r\n");

            pThis->set(close);
            return pThis->m_retVal->readFrom(pThis->m_bs, pThis);
        }

        static int close(asyncState *pState, int n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(ok);
            if (!pThis->m_ssl)
                return 0;

            return pThis->m_ssl_sock->close(pThis);
        }

        static int ok(asyncState *pState, int n)
        {
            return pState->done();
        }

    private:
        const char *m_host;
        int32_t m_port;
        HttpRequest_base *m_req;
        bool m_ssl;
        obj_ptr<HttpResponse_base> &m_retVal;
        obj_ptr<Socket> m_sock;
        obj_ptr<SslSocket> m_ssl_sock;
        obj_ptr<Stream_base> m_strm;
        obj_ptr<BufferedStream> m_bs;
        int m_temp;
    };

    if (!ac)
        return CALL_E_NOSYNC;

    return (new asyncRequest(host, port, req, ssl, retVal, ac))->post(0);
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
        std::string hostname;
        int nPort = 80;
        int32_t status;
        bool ssl = false;
        obj_ptr<HttpRequest> req;
        Variant v;

        {
            obj_ptr<Url> u = new Url();

            hr = u->parse(url);
            if (hr < 0)
                return hr;

            std::string host, port, path;
            std::string protocol;

            u->get_protocol(protocol);

            if (!qstrcmp(protocol.c_str(), "https:"))
                ssl = true;
            else if (qstrcmp(protocol.c_str(), "http:"))
                return Runtime::setError("unknown protocol");

            u->get_host(host);
            if (host.empty())
                return Runtime::setError("unknown host");

            u->get_hostname(hostname);
            u->get_port(port);
            u->get_path(path);

            if (!port.empty())
                nPort = atoi(port.c_str());
            else
                nPort = ssl ? 443 : 80;

            req = new HttpRequest();

            req->set_address(path.c_str());
            req->addHeader("host", host.c_str());
            req->addHeader(headers);

            if (body)
                req->set_body(body);
        }

        hr = ac_request(hostname.c_str(), nPort, req, ssl, retVal);
        if (hr < 0)
            return hr;

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
            return Runtime::setError("redirect cycle");

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

result_t http_base::request(const char *method, const char *url,
                            const char *body, v8::Local<v8::Object> headers,
                            obj_ptr<HttpResponse_base> &retVal)
{
    obj_ptr<Buffer_base> buf = new Buffer(body);
    return request(method, url, buf, headers, retVal);
}

result_t http_base::get(const char *url, v8::Local<v8::Object> headers,
                        obj_ptr<HttpResponse_base> &retVal)
{
    return request("GET", url, headers, retVal);
}

result_t http_base::post(const char *url, v8::Local<v8::Object> headers,
                         obj_ptr<HttpResponse_base> &retVal)
{
    return request("POST", url, headers, retVal);
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

result_t http_base::post(const char *url, const char *body,
                         v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base> &retVal)
{
    return request("POST", url, body, headers, retVal);
}

}
