/*
 * HttpHandler.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "JSHandler.h"
#include "ifs/mq.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "ifs/zlib.h"
#include "ifs/console.h"

namespace fibjs
{

static const char *s_staticCounter[] =
{ "total", "pendding" };
static const char *s_Counter[] =
{ "request", "response", "error", "error_400", "error_404", "error_500", "totalTime" };

enum
{
    HTTP_TOTAL = 0,
    HTTP_PENDDING,
    HTTP_REQUEST,
    HTTP_RESPONSE,
    HTTP_ERROR,
    HTTP_ERROR_400,
    HTTP_ERROR_404,
    HTTP_ERROR_500,
    HTTP_TOTAL_TIME
};

result_t HttpHandler_base::_new(v8::Local<v8::Value> hdlr,
                                obj_ptr<HttpHandler_base> &retVal,
                                v8::Local<v8::Object> This)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;

    obj_ptr<HttpHandler> ht_hdlr = new HttpHandler();
    ht_hdlr->wrap(This);
    ht_hdlr->set_handler(hdlr1);

    retVal = ht_hdlr;

    return 0;
}

HttpHandler::HttpHandler() :
    m_crossDomain(false), m_forceGZIP(false), m_maxHeadersCount(
        128), m_maxUploadSize(67108864)
{
    m_stats = new Stats();
    m_stats->init(s_staticCounter, 2, s_Counter, 7);
}

result_t HttpHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                             AsyncEvent *ac)
{
    class asyncInvoke: public AsyncState
    {
    public:
        asyncInvoke(HttpHandler *pThis, Stream_base *stm, AsyncEvent *ac) :
            AsyncState(ac), m_pThis(pThis), m_stm(stm)
        {
            m_stmBuffered = new BufferedStream(stm);
            m_stmBuffered->set_EOL("\r\n");

            m_req = new HttpRequest();

            obj_ptr<Message_base> m;
            m_req->get_response(m);
            m_rep = (HttpResponse_base *)(Message_base *)m;

            m_req->set_maxHeadersCount(pThis->m_maxHeadersCount);
            m_req->set_maxUploadSize(pThis->m_maxUploadSize);

            set(read);
        }

        static int32_t read(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;
            bool bKeepAlive = false;

            pThis->m_rep->get_keepAlive(bKeepAlive);
            if (!bKeepAlive)
                return pThis->done(CALL_RETURN_NULL);

            pThis->m_zip.Release();
            pThis->m_body.Release();

            pThis->set(invoke);
            return pThis->m_req->readFrom(pThis->m_stmBuffered, pThis);
        }

        static int32_t invoke(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            if (n == CALL_RETURN_NULL)
                return pThis->done(CALL_RETURN_NULL);

            pThis->m_pThis->m_stats->inc(HTTP_TOTAL);
            pThis->m_pThis->m_stats->inc(HTTP_REQUEST);
            pThis->m_pThis->m_stats->inc(HTTP_PENDDING);

            std::string str;

            pThis->m_req->get_protocol(str);
            pThis->m_rep->set_protocol(str.c_str());

            bool bKeepAlive;

            pThis->m_req->get_keepAlive(bKeepAlive);
            pThis->m_rep->set_keepAlive(bKeepAlive);

            pThis->set(check_error);
            pThis->m_d.now();

            if (pThis->m_pThis->m_crossDomain)
            {
                pThis->m_req->get_address(str);

                if (!qstrcmp(str.c_str(), "/crossdomain.xml"))
                {
                    obj_ptr<MemoryStream> body = new MemoryStream();
                    obj_ptr<Buffer> buf = new Buffer("<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>",
                                                     88);

                    pThis->m_rep->set_body(body);
                    body->write(buf, NULL);

                    pThis->m_rep->setHeader("Content-Type", "text/xml");

                    return 0;
                }
                else
                {
                    Variant origin;

                    if (pThis->m_req->firstHeader("origin",
                                                  origin) != CALL_RETURN_NULL)
                    {
                        pThis->m_rep->setHeader(
                            "Access-Control-Allow-Credentials", "true");
                        pThis->m_rep->setHeader("Access-Control-Allow-Origin",
                                                origin);

                        pThis->m_req->get_method(str);

                        if (!qstricmp(str.c_str(), "options"))
                        {
                            pThis->m_rep->setHeader(
                                "Access-Control-Allow-Methods", "*");
                            pThis->m_rep->setHeader(
                                "Access-Control-Allow-Headers",
                                "CONTENT-TYPE");
                            pThis->m_rep->setHeader("Access-Control-Max-Age",
                                                    "1728000");

                            return 0;
                        }
                    }
                }
            }

            if (pThis->m_pThis->m_forceGZIP)
            {
                bool v;

                pThis->m_req->hasHeader("Accept-Encoding", v);
                if (!v)
                    pThis->m_req->setHeader("Accept-Encoding", "gzip");
            }

            return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_req, pThis);
        }

        static int32_t check_error(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;
            int32_t s;
            int32_t err_idx = -1;

            pThis->m_rep->get_status(s);
            if (s == 400) {
                err_idx = 0;
                pThis->m_pThis->m_stats->inc(HTTP_ERROR_400);
            }
            else if (s == 404) {
                err_idx = 1;
                pThis->m_pThis->m_stats->inc(HTTP_ERROR_404);
            }
            else if (s == 500) {
                err_idx = 2;
                pThis->m_pThis->m_stats->inc(HTTP_ERROR_500);
            }

            pThis->set(send);
            if (err_idx == -1 || !pThis->m_pThis->m_err_hdlrs[err_idx])
                return 0;
            return mq_base::invoke(pThis->m_pThis->m_err_hdlrs[err_idx], pThis->m_req, pThis);
        }

        static int32_t send(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;
            int32_t s;
            bool t = false;
            date_t d;

            d.now();
            pThis->m_pThis->m_stats->add(HTTP_TOTAL_TIME, (int32_t)d.diff(pThis->m_d));

            pThis->m_rep->get_status(s);
            if (s == 200)
            {
                pThis->m_rep->hasHeader("Last-Modified", t);
                if (!t)
                {
                    pThis->m_rep->addHeader("Cache-Control",
                                            "no-cache, no-store");
                    pThis->m_rep->addHeader("Expires", "-1");
                }
            }

            std::string str;

            pThis->m_req->get_method(str);
            bool headOnly = !qstricmp(str.c_str(), "head");

            if (headOnly)
            {
                pThis->set(end);
                pThis->m_rep->set_keepAlive(false);
                return pThis->m_rep->sendHeader(pThis->m_stm, pThis);
            }

            int64_t len;

            pThis->m_rep->get_length(len);

            if (len > 128 && len < 1024 * 1024 * 5)
            {
                Variant hdr;

                if (pThis->m_req->firstHeader("Accept-Encoding",
                                              hdr) != CALL_RETURN_NULL)
                {
                    std::string str = hdr.string();
                    int32_t type = 0;

                    if (qstristr(str.c_str(), "gzip"))
                        type = 1;
                    else if (qstristr(str.c_str(), "deflate"))
                        type = 2;

                    if (type != 0)
                    {
                        if (pThis->m_rep->firstHeader("Content-Type",
                                                      hdr) != CALL_RETURN_NULL)
                        {
                            str = hdr.string();

                            if (qstricmp(str.c_str(), "text/", 5)
                                    && qstricmp(str.c_str(),
                                                "application/x-javascript")
                                    && qstricmp(str.c_str(),
                                                "application/json"))
                                type = 0;
                        }
                        else
                            type = 0;
                    }

                    if (type != 0)
                    {
                        if (pThis->m_rep->firstHeader("Content-Encoding",
                                                      hdr) != CALL_RETURN_NULL)
                            type = 0;
                    }

                    if (type != 0)
                    {
                        pThis->m_rep->addHeader("Content-Encoding",
                                                type == 1 ? "gzip" : "deflate");

                        pThis->m_rep->get_body(pThis->m_body);
                        pThis->m_body->rewind();

                        pThis->m_zip = new MemoryStream();

                        pThis->set(zip);

                        if (type == 1)
                            return zlib_base::gzipTo(pThis->m_body,
                                                     pThis->m_zip, pThis);
                        else
                            return zlib_base::deflateTo(pThis->m_body,
                                                        pThis->m_zip, -1, pThis);
                    }
                }
            }

            pThis->set(end);
            return pThis->m_rep->sendTo(pThis->m_stm, pThis);
        }

        static int32_t zip(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->m_rep->set_body(pThis->m_zip);

            pThis->set(end);
            return pThis->m_rep->sendTo(pThis->m_stm, pThis);
        }

        static int32_t end(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            if (!pThis->m_body)
                pThis->m_rep->get_body(pThis->m_body);

            pThis->m_pThis->m_stats->inc(HTTP_RESPONSE);
            pThis->m_pThis->m_stats->dec(HTTP_PENDDING);

            pThis->set(read);
            if (!pThis->m_body)
                return 0;

            return pThis->m_body->close(pThis);
        }

        virtual int32_t error(int32_t v)
        {
            m_pThis->m_stats->inc(HTTP_ERROR);

            if (is(check_error))
            {
                std::string err = getResultMessage(v);

                m_req->set_lastError(err.c_str());
                asyncLog(console_base::_ERROR, "HttpHandler: " + err);

                m_rep->set_status(500);
                return 0;
            }

            if (is(invoke))
            {
                m_pThis->m_stats->inc(HTTP_TOTAL);
                m_pThis->m_stats->inc(HTTP_REQUEST);
                m_pThis->m_stats->inc(HTTP_PENDDING);

                m_rep->set_keepAlive(false);
                m_rep->set_status(400);
                set(check_error);
                m_d.now();
                return 0;
            }

            if (is(send))
            {
                asyncLog(console_base::_ERROR, "HttpHandler: " + getResultMessage(v));
                return 0;
            }

            m_pThis->m_stats->dec(HTTP_PENDDING);
            return done(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<HttpHandler> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<BufferedStream_base> m_stmBuffered;
        obj_ptr<HttpRequest_base> m_req;
        obj_ptr<HttpResponse_base> m_rep;
        obj_ptr<MemoryStream> m_zip;
        obj_ptr<SeekableStream_base> m_body;
        date_t m_d;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Stream_base> stm = Stream_base::getInstance(v);
    if (stm == NULL)
    {
        obj_ptr<Message_base> msg = Message_base::getInstance(v);
        if (msg != NULL)
            msg->get_stream(stm);

    }

    if (stm == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, stm, ac))->post(0);
}

result_t HttpHandler::onerror(v8::Local<v8::Object> hdlrs)
{
    static const char* s_err_keys[] = {"400", "404", "500"};
    int32_t i;
    v8::Local<v8::Object> o = wrap();
    Isolate* isolate = holder();

    for (i = 0; i < 3; i ++)
    {
        v8::Local<v8::String> key = isolate->NewFromUtf8(s_err_keys[i]);
        v8::Local<v8::Value> hdlr = hdlrs->Get(key);

        if (!IsEmpty(hdlr))
        {
            obj_ptr<Handler_base> hdlr1;

            result_t hr = JSHandler::New(hdlr, hdlr1);
            if (hr < 0)
                return hr;

            isolate->SetPrivate(o, s_err_keys[i], hdlr1->wrap());
            m_err_hdlrs[i] = hdlr1;
        }
    }

    return 0;
}

result_t HttpHandler::get_crossDomain(bool &retVal)
{
    retVal = m_crossDomain;
    return 0;
}

result_t HttpHandler::set_crossDomain(bool newVal)
{
    m_crossDomain = newVal;
    return 0;
}

result_t HttpHandler::get_forceGZIP(bool &retVal)
{
    retVal = m_forceGZIP;
    return 0;
}

result_t HttpHandler::set_forceGZIP(bool newVal)
{
    m_forceGZIP = newVal;
    return 0;
}

result_t HttpHandler::get_maxHeadersCount(int32_t &retVal)
{
    retVal = m_maxHeadersCount;
    return 0;
}

result_t HttpHandler::set_maxHeadersCount(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxHeadersCount = newVal;
    return 0;
}

result_t HttpHandler::get_maxUploadSize(int32_t &retVal)
{
    retVal = m_maxUploadSize;
    return 0;
}

result_t HttpHandler::set_maxUploadSize(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxUploadSize = newVal;
    return 0;
}

result_t HttpHandler::get_handler(obj_ptr<Handler_base> &retVal)
{
    retVal = m_hdlr;
    return 0;
}

result_t HttpHandler::set_handler(Handler_base *newVal)
{
    obj_ptr<Handler_base> hdlr = (Handler_base*)m_hdlr;

    holder()->SetPrivate(wrap(), "handler", newVal->wrap());
    m_hdlr = newVal;

    if (hdlr)
        hdlr->dispose();

    return 0;
}

result_t HttpHandler::get_stats(obj_ptr<Stats_base> &retVal)
{
    retVal = m_stats;
    return 0;
}

}

