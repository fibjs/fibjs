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

        ON_STATE(asyncInvoke, read)
        {
            bool bKeepAlive = false;

            m_rep->get_keepAlive(bKeepAlive);

            if (!bKeepAlive)
                return next(CALL_RETURN_NULL);

            m_options = false;

            m_zip.Release();
            m_body.Release();

            return m_req->readFrom(m_stmBuffered, next(invoke));
        }

        ON_STATE(asyncInvoke, invoke)
        {
            if (n == CALL_RETURN_NULL)
                return next(CALL_RETURN_NULL);

            exlib::string str;

            m_req->get_protocol(str);
            m_rep->set_protocol(str);

            m_rep->addHeader("Server", m_pThis->m_serverName);

            bool bKeepAlive;

            m_req->get_keepAlive(bKeepAlive);
            m_rep->set_keepAlive(bKeepAlive);

            m_d.now();

            if (m_pThis->m_crossDomain) {
                m_req->get_address(str);

                exlib::string origin;

                if (m_req->firstHeader("origin", origin)
                    != CALL_RETURN_NULL) {
                    m_rep->setHeader("Access-Control-Allow-Credentials", "true");
                    m_rep->setHeader("Access-Control-Allow-Origin", origin);

                    m_req->get_method(str);

                    if (!qstricmp(str.c_str(), "options")) {
                        m_options = true;

                        m_rep->setHeader("Access-Control-Allow-Methods", "*");
                        m_rep->setHeader("Access-Control-Allow-Headers",
                            m_pThis->m_allowHeaders);
                        m_rep->setHeader("Access-Control-Max-Age", "1728000");

                        return next(send);
                    }
                }
            }

            return mq_base::invoke(m_pThis->m_hdlr, m_req, next(send));
        }

        ON_STATE(asyncInvoke, send)
        {
            int32_t s;
            bool t = false;
            date_t d;

            d.now();

            m_rep->get_statusCode(s);
            if (s == 200 && !m_options) {
                m_rep->hasHeader("Last-Modified", t);
                if (!t) {
                    m_rep->addHeader("Cache-Control", "no-cache, no-store");
                    m_rep->addHeader("Expires", "-1");
                }
            }

            exlib::string str;

            m_req->get_method(str);
            bool headOnly = !qstricmp(str.c_str(), "head");

            if (headOnly) {
                m_rep->set_keepAlive(false);
                return m_rep->sendHeader(m_stm, next(end));
            }

            int64_t len;

            m_rep->get_length(len);

            if (len > 128 && len < 1024 * 1024 * 64) {
                exlib::string hdr;

                if (m_req->firstHeader("Accept-Encoding", hdr)
                    != CALL_RETURN_NULL) {
                    int32_t type = 0;

                    if (qstristr(hdr.c_str(), "gzip"))
                        type = 1;
                    else if (qstristr(hdr.c_str(), "deflate"))
                        type = 2;

                    if (type != 0) {
                        if (m_rep->firstHeader("Content-Type", hdr)
                            != CALL_RETURN_NULL) {

                            if (qstricmp(hdr.c_str(), "text/", 5)
                                && qstricmp(hdr.c_str(), "application/x-javascript")
                                && qstricmp(hdr.c_str(), "application/json"))
                                type = 0;
                        } else
                            type = 0;
                    }

                    if (type != 0) {
                        if (m_rep->firstHeader("Content-Encoding", hdr)
                            != CALL_RETURN_NULL)
                            type = 0;
                    }

                    if (type != 0) {
                        m_rep->addHeader("Content-Encoding", type == 1 ? "gzip" : "deflate");

                        m_rep->get_body(m_body);
                        m_body->rewind();

                        m_zip = new MemoryStream();

                        if (type == 1)
                            return zlib_base::gzipTo(m_body, m_zip, next(zip));
                        else
                            return zlib_base::deflateTo(m_body, m_zip, -1, next(zip));
                    }
                }
            }

            return m_rep->sendTo(m_stm, next(end));
        }

        ON_STATE(asyncInvoke, zip)
        {
            m_rep->set_body(m_zip);
            return m_rep->sendTo(m_stm, next(end));
        }

        ON_STATE(asyncInvoke, end)
        {
            if (!m_body)
                m_rep->get_body(m_body);

            if (!m_body)
                return next(read);

            return m_body->close(next(read));
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
