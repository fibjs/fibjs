/*
 * HttpHandler.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpHandler.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "JSHandler.h"
#include "ifs/mq.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "version.h"
#include "ifs/zlib.h"
#include "ifs/console.h"

namespace fibjs {

result_t HttpHandler_base::_new(Handler_base* hdlr, obj_ptr<HttpHandler_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<HttpHandler> ht_hdlr = new HttpHandler();
    ht_hdlr->wrap(This);
    ht_hdlr->set_handler(hdlr);

    retVal = ht_hdlr;

    return 0;
}

HttpHandler::HttpHandler()
    : m_crossDomain(false)
    , m_maxHeadersCount(128)
    , m_maxBodySize(64)
{
    m_serverName = "fibjs/";
    m_serverName.append(fibjs_version);
}

result_t HttpHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(HttpHandler* pThis, Stream_base* stm, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_options(false)
        {
            m_stmBuffered = new BufferedStream(stm);
            m_stmBuffered->set_EOL("\r\n");

            m_req = new HttpRequest();
            m_req->get_response(m_rep);

            m_req->set_maxHeadersCount(pThis->m_maxHeadersCount);
            m_req->set_maxBodySize(pThis->m_maxBodySize);

            next(read);
        }

        static int32_t read(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
            bool bKeepAlive = false;

            pThis->m_rep->get_keepAlive(bKeepAlive);

            if (!bKeepAlive)
                return pThis->next(CALL_RETURN_NULL);

            pThis->m_options = false;

            pThis->m_zip.Release();
            pThis->m_body.Release();

            return pThis->m_req->readFrom(pThis->m_stmBuffered, pThis->next(invoke));
        }

        static int32_t invoke(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            if (n == CALL_RETURN_NULL)
                return pThis->next(CALL_RETURN_NULL);

            exlib::string str;

            pThis->m_req->get_protocol(str);
            pThis->m_rep->set_protocol(str);

            pThis->m_rep->addHeader("Server", pThis->m_pThis->m_serverName);

            bool bKeepAlive;

            pThis->m_req->get_keepAlive(bKeepAlive);
            pThis->m_rep->set_keepAlive(bKeepAlive);

            pThis->m_d.now();

            if (pThis->m_pThis->m_crossDomain) {
                pThis->m_req->get_address(str);

                exlib::string origin;

                if (pThis->m_req->firstHeader("origin", origin)
                    != CALL_RETURN_NULL) {
                    pThis->m_rep->setHeader("Access-Control-Allow-Credentials", "true");
                    pThis->m_rep->setHeader("Access-Control-Allow-Origin", origin);

                    pThis->m_req->get_method(str);

                    if (!qstricmp(str.c_str(), "options")) {
                        pThis->m_options = true;

                        pThis->m_rep->setHeader("Access-Control-Allow-Methods", "*");
                        pThis->m_rep->setHeader("Access-Control-Allow-Headers",
                            pThis->m_pThis->m_allowHeaders);
                        pThis->m_rep->setHeader("Access-Control-Max-Age", "1728000");

                        return pThis->next(send);
                    }
                }
            }

            return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_req, pThis->next(send));
        }

        static int32_t send(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
            int32_t s;
            bool t = false;
            date_t d;

            d.now();

            pThis->m_rep->get_statusCode(s);
            if (s == 200 && !pThis->m_options) {
                pThis->m_rep->hasHeader("Last-Modified", t);
                if (!t) {
                    pThis->m_rep->addHeader("Cache-Control", "no-cache, no-store");
                    pThis->m_rep->addHeader("Expires", "-1");
                }
            }

            exlib::string str;

            pThis->m_req->get_method(str);
            bool headOnly = !qstricmp(str.c_str(), "head");

            if (headOnly) {
                pThis->m_rep->set_keepAlive(false);
                return pThis->m_rep->sendHeader(pThis->m_stm, pThis->next(end));
            }

            int64_t len;

            pThis->m_rep->get_length(len);

            if (len > 128 && len < 1024 * 1024 * 64) {
                exlib::string hdr;

                if (pThis->m_req->firstHeader("Accept-Encoding", hdr)
                    != CALL_RETURN_NULL) {
                    int32_t type = 0;

                    if (qstristr(hdr.c_str(), "gzip"))
                        type = 1;
                    else if (qstristr(hdr.c_str(), "deflate"))
                        type = 2;

                    if (type != 0) {
                        if (pThis->m_rep->firstHeader("Content-Type", hdr)
                            != CALL_RETURN_NULL) {

                            if (qstricmp(hdr.c_str(), "text/", 5)
                                && qstricmp(hdr.c_str(), "application/x-javascript")
                                && qstricmp(hdr.c_str(), "application/json"))
                                type = 0;
                        } else
                            type = 0;
                    }

                    if (type != 0) {
                        if (pThis->m_rep->firstHeader("Content-Encoding", hdr)
                            != CALL_RETURN_NULL)
                            type = 0;
                    }

                    if (type != 0) {
                        pThis->m_rep->addHeader("Content-Encoding", type == 1 ? "gzip" : "deflate");

                        pThis->m_rep->get_body(pThis->m_body);
                        pThis->m_body->rewind();

                        pThis->m_zip = new MemoryStream();

                        if (type == 1)
                            return zlib_base::gzipTo(pThis->m_body, pThis->m_zip, pThis->next(zip));
                        else
                            return zlib_base::deflateTo(pThis->m_body, pThis->m_zip, -1, pThis->next(zip));
                    }
                }
            }

            return pThis->m_rep->sendTo(pThis->m_stm, pThis->next(end));
        }

        static int32_t zip(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            pThis->m_rep->set_body(pThis->m_zip);
            return pThis->m_rep->sendTo(pThis->m_stm, pThis->next(end));
        }

        static int32_t end(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            if (!pThis->m_body)
                pThis->m_rep->get_body(pThis->m_body);

            if (!pThis->m_body)
                return pThis->next(read);

            return pThis->m_body->close(pThis->next(read));
        }

        virtual int32_t error(int32_t v)
        {
            if (at(invoke)) {
                exlib::string err = getResultMessage(v);

                m_req->set_lastError(err);
                errorLog("HttpHandler: " + err);

                m_rep->set_statusCode(500);
                return 0;
            }

            if (at(read)) {
                m_rep->set_keepAlive(false);
                m_rep->set_statusCode(400);
                next(send);
                m_d.now();
                return 0;
            }

            return next(CALL_RETURN_NULL);
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
        bool m_options;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Stream_base> stm = Stream_base::getInstance(v);
    if (stm == NULL) {
        obj_ptr<Message_base> msg = Message_base::getInstance(v);
        if (msg != NULL)
            msg->get_stream(stm);
    }

    if (stm == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, stm, ac))->post(0);
}

result_t HttpHandler::enableCrossOrigin(exlib::string allowHeaders)
{
    m_crossDomain = true;
    m_allowHeaders = allowHeaders;
    return 0;
}

result_t HttpHandler::get_maxHeadersCount(int32_t& retVal)
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

result_t HttpHandler::get_maxBodySize(int32_t& retVal)
{
    retVal = m_maxBodySize;
    return 0;
}

result_t HttpHandler::set_maxBodySize(int32_t newVal)
{
    m_maxBodySize = newVal;
    return 0;
}

result_t HttpHandler::get_serverName(exlib::string& retVal)
{
    retVal = m_serverName;
    return 0;
}

result_t HttpHandler::set_serverName(exlib::string newVal)
{
    m_serverName = newVal;
    return 0;
}

result_t HttpHandler::get_handler(obj_ptr<Handler_base>& retVal)
{
    retVal = m_hdlr;
    return 0;
}

result_t HttpHandler::set_handler(Handler_base* newVal)
{
    SetPrivate("handler", newVal->wrap());
    m_hdlr = newVal;

    return 0;
}
}
