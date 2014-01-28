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
#include "BufferedStream.h"

namespace fibjs
{

result_t http_base::handler(v8::Local<v8::Value> hdlr,
                            obj_ptr<HttpHandler_base> &retVal)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;

    retVal = new HttpHandler(hdlr1);
    return 0;
}

result_t http_base::fileHandler(const char *root, obj_ptr<Handler_base> &retVal)
{
    retVal = new HttpFileHandler(root);
    return 0;
}

result_t http_base::request(const char *host, int32_t port,
                            HttpRequest_base *req, obj_ptr<HttpResponse_base> &retVal,
                            exlib::AsyncEvent *ac)
{
    class asyncRequest: public asyncState
    {
    public:
        asyncRequest(const char *host, int32_t port, HttpRequest_base *req,
                     obj_ptr<HttpResponse_base> &retVal, exlib::AsyncEvent *ac) :
            asyncState(ac), m_host(host), m_port(port), m_req(req), m_retVal(
                retVal)
        {
            set(connect);
        }

        static int connect(asyncState *pState, int n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->m_sock = new Socket();

            pThis->m_sock->create(net_base::_AF_INET, net_base::_SOCK_STREAM);

            pThis->set(send);
            return pThis->m_sock->connect(pThis->m_host, pThis->m_port, pThis);
        }

        static int send(asyncState *pState, int n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->set(recv);
            return pThis->m_req->sendTo(pThis->m_sock, pThis);
        }

        static int recv(asyncState *pState, int n)
        {
            asyncRequest *pThis = (asyncRequest *) pState;

            pThis->m_retVal = new HttpResponse();
            pThis->m_bs = new BufferedStream(pThis->m_sock);
            pThis->m_bs->set_EOL("\r\n");

            pThis->set(ok);
            return pThis->m_retVal->readFrom(pThis->m_bs, pThis);
        }

        static int ok(asyncState *pState, int n)
        {
            return pState->done(0);
        }

    private:
        const char *m_host;
        int32_t m_port;
        HttpRequest_base *m_req;
        obj_ptr<HttpResponse_base> &m_retVal;
        obj_ptr<Socket> m_sock;
        obj_ptr<BufferedStream> m_bs;
    };

    if (!ac)
        return CALL_E_NOSYNC;

    return (new asyncRequest(host, port, req, retVal, ac))->post(0);
}

result_t http_base::request(const char *method, const char *url,
                            SeekableStream_base *body, v8::Local<v8::Object> headers,
                            obj_ptr<HttpResponse_base> &retVal)
{
    result_t hr;
    std::string hostname;
    int nPort = 80;
    obj_ptr<HttpRequest> req;

    {
        obj_ptr<Url> u = new Url();

        hr = u->parse(url);
        if (hr < 0)
            return hr;

        std::string host, port, path;

        u->get_host(host);
        if (host.empty())
            return Runtime::setError("bad url.");

        u->get_hostname(hostname);
        u->get_port(port);
        u->get_path(path);

        if (!port.empty())
            nPort = atoi(port.c_str());

        req = new HttpRequest();

        req->set_address(path.c_str());
        req->addHeader("host", host.c_str());
        req->addHeader(headers);

        if (body)
            req->set_body(body);
    }

    return ac_request(hostname.c_str(), nPort, req, retVal);
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
