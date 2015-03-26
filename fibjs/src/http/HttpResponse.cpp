/*
 * HttpResponse.cpp
 *
 *  Created on: Aug 13, 2012
 *      Author: lion
 */

#include "HttpResponse.h"
#include "HttpCookie.h"
#include "Buffer.h"
#include "List.h"

namespace fibjs
{

result_t HttpResponse_base::_new(obj_ptr<HttpResponse_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpResponse();
    return 0;
}

result_t HttpResponse::get_protocol(std::string &retVal)
{
    return m_message.get_protocol(retVal);
}

result_t HttpResponse::set_protocol(const char *newVal)
{
    return m_message.set_protocol(newVal);
}

result_t HttpResponse::get_headers(obj_ptr<HttpCollection_base> &retVal)
{
    return m_message.get_headers(retVal);
}

result_t HttpResponse::get_body(obj_ptr<SeekableStream_base> &retVal)
{
    return m_message.get_body(retVal);
}

result_t HttpResponse::set_body(SeekableStream_base *newVal)
{
    return m_message.set_body(newVal);
}

result_t HttpResponse::read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                            exlib::AsyncEvent *ac)
{
    return m_message.read(bytes, retVal, ac);
}

result_t HttpResponse::readAll(obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac)
{
    return m_message.readAll(retVal, ac);
}

result_t HttpResponse::write(Buffer_base *data, exlib::AsyncEvent *ac)
{
    return m_message.write(data, ac);
}

result_t HttpResponse::get_length(int64_t &retVal)
{
    return m_message.get_length(retVal);
}

result_t HttpResponse::get_keepAlive(bool &retVal)
{
    return m_message.get_keepAlive(retVal);
}

result_t HttpResponse::set_keepAlive(bool newVal)
{
    return m_message.set_keepAlive(newVal);
}

result_t HttpResponse::get_maxHeadersCount(int32_t &retVal)
{
    return m_message.get_maxHeadersCount(retVal);
}

result_t HttpResponse::set_maxHeadersCount(int32_t newVal)
{
    return m_message.set_maxHeadersCount(newVal);
}

result_t HttpResponse::get_maxUploadSize(int32_t &retVal)
{
    return m_message.get_maxUploadSize(retVal);
}

result_t HttpResponse::set_maxUploadSize(int32_t newVal)
{
    return m_message.get_maxUploadSize(newVal);
}

result_t HttpResponse::hasHeader(const char *name, bool &retVal)
{
    return m_message.hasHeader(name, retVal);
}

result_t HttpResponse::firstHeader(const char *name, Variant &retVal)
{
    return m_message.firstHeader(name, retVal);
}

result_t HttpResponse::allHeader(const char *name, obj_ptr<List_base> &retVal)
{
    return m_message.allHeader(name, retVal);
}

result_t HttpResponse::addHeader(v8::Local<v8::Object> map)
{
    return m_message.addHeader(map);
}

result_t HttpResponse::addHeader(const char *name, Variant value)
{
    return m_message.addHeader(name, value);
}

result_t HttpResponse::setHeader(v8::Local<v8::Object> map)
{
    return m_message.setHeader(map);
}

result_t HttpResponse::setHeader(const char *name, Variant value)
{
    return m_message.setHeader(name, value);
}

result_t HttpResponse::removeHeader(const char *name)
{
    return m_message.removeHeader(name);
}

result_t HttpResponse::get_value(std::string &retVal)
{
    return m_message.get_value(retVal);
}

result_t HttpResponse::set_value(const char *newVal)
{
    return m_message.set_value(newVal);
}

result_t HttpResponse::get_params(obj_ptr<List_base> &retVal)
{
    return m_message.get_params(retVal);
}

result_t HttpResponse::set_params(List_base *newVal)
{
    return m_message.set_params(newVal);
}

result_t HttpResponse::get_result(Variant &retVal)
{
    return m_message.get_result(retVal);
}

result_t HttpResponse::set_result(Variant newVal)
{
    return m_message.set_result(newVal);
}

result_t HttpResponse::clear()
{
    m_message.clear();

    m_cookies.Release();
    m_status = 200;

    return 0;
}

static const char *const status_lines[] =
{
#define LEVEL_100  0
    " 100 Continue", " 101 Switching Protocols", " 102 Processing",
#define LEVEL_200  3
    " 200 OK", " 201 Created", " 202 Accepted",
    " 203 Non-Authoritative Information", " 204 No Content",
    " 205 Reset Content", " 206 Partial Content", " 207 Multi-Status",
#define LEVEL_300 11
    " 300 Multiple Choices", " 301 Moved Permanently", " 302 Object moved",
    " 303 See Other", " 304 Not Modified", " 305 Use Proxy", " 306 unused",
    " 307 Temporary Redirect",
#define LEVEL_400 19
    " 400 Bad Request", " 401 Authorization Required",
    " 402 Payment Required", " 403 Forbidden", " 404 File Not Found",
    " 405 Method Not Allowed", " 406 Not Acceptable",
    " 407 Proxy Authentication Required", " 408 Request Time-out",
    " 409 Conflict", " 410 Gone", " 411 Length Required",
    " 412 Precondition Failed", " 413 Request Entity Too Large",
    " 414 Request-URI Too Large", " 415 Unsupported Media Type",
    " 416 Requested Range Not Satisfiable", " 417 Expectation Failed",
    " 418 Host Not Found", " 419 unused", " 420 unused", " 421 unused",
    " 422 Unprocessable Entity", " 423 Locked", " 424 Failed Dependency",
#define LEVEL_500 44
    " 500 Internal Server Error", " 501 Method Not Implemented",
    " 502 Bad Gateway", " 503 Service Temporarily Unavailable",
    " 504 Gateway Time-out", " 505 HTTP Version Not Supported",
    " 506 Variant Also Negotiates", " 507 Insufficient Storage",
    " 508 unused", " 509 unused", " 510 Not Extended"
#define RESPONSE_CODES 55
};
static int shortcut[6] =
{ LEVEL_100, LEVEL_200, LEVEL_300, LEVEL_400, LEVEL_500, RESPONSE_CODES };
static unsigned char status_lines_size[RESPONSE_CODES];

static class _init_status_line
{
public:
    _init_status_line()
    {
        int i;

        for (i = 0; i < RESPONSE_CODES; i++)
            status_lines_size[i] = (unsigned char) qstrlen(status_lines[i]);
    }
} s_init_status_line;

result_t HttpResponse::sendTo(Stream_base *stm, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_cookies)
    {
        int32_t len, i;

        m_cookies->get_length(len);

        for (i = 0; i < len; i ++)
        {
            Variant v;
            obj_ptr<object_base> cookie;
            std::string str;

            m_cookies->_indexed_getter(i, v);
            cookie = v.object();

            if (cookie)
            {
                cookie->toString(str);
                addHeader("Set-Cookie", str.c_str());
            }
        }

        m_cookies.Release();
    }

    int pos = shortcut[m_status / 100 - 1] + m_status % 100;
    std::string strCommand;

    get_protocol(strCommand);
    strCommand.append(status_lines[pos], status_lines_size[pos]);

    return m_message.sendTo(stm, strCommand, ac);
}

result_t HttpResponse::readFrom(BufferedStream_base *stm, exlib::AsyncEvent *ac)
{
    class asyncReadFrom: public asyncState
    {
    public:
        asyncReadFrom(HttpResponse *pThis, BufferedStream_base *stm,
                      exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_stm(stm)
        {
            m_pThis->clear();
            set(begin);
        }

        static int begin(asyncState *pState, int n)
        {
            asyncReadFrom *pThis = (asyncReadFrom *) pState;

            pThis->set(command);
            return pThis->m_stm->readLine(HTTP_MAX_LINE, pThis->m_strLine,
                                          pThis);
        }

        static int command(asyncState *pState, int n)
        {
            asyncReadFrom *pThis = (asyncReadFrom *) pState;
            result_t hr;

            if (pThis->m_strLine.length() < 12 || pThis->m_strLine[8] != ' '
                    || !qisdigit(pThis->m_strLine[9])
                    || !qisdigit(pThis->m_strLine[10])
                    || !qisdigit(pThis->m_strLine[11])
                    || (pThis->m_strLine.length() > 12
                        && qisdigit(pThis->m_strLine[12])))
                return CHECK_ERROR(Runtime::setError("bad protocol: " + pThis->m_strLine));

            pThis->m_pThis->set_status(atoi(pThis->m_strLine.c_str() + 8));
            pThis->m_strLine.resize(8);

            hr = pThis->m_pThis->set_protocol(pThis->m_strLine.c_str());
            if (hr < 0)
                return hr;

            pThis->done();
            return pThis->m_pThis->m_message.readFrom(pThis->m_stm, pThis);
        }

    public:
        obj_ptr<HttpResponse> m_pThis;
        obj_ptr<BufferedStream_base> m_stm;
        std::string m_strLine;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncReadFrom(this, stm, ac))->post(0);
}

result_t HttpResponse::get_stream(obj_ptr<Stream_base> &retVal)
{
    return m_message.get_stream(retVal);
}

result_t HttpResponse::get_response(obj_ptr<Message_base> &retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t HttpResponse::get_status(int32_t &retVal)
{
    retVal = m_status;
    return 0;
}

result_t HttpResponse::set_status(int32_t newVal)
{
    if (newVal < 100 || newVal >= 600)
        newVal = 500;
    else
    {
        int n = newVal / 100;
        if (shortcut[n - 1] + newVal % 100 >= shortcut[n])
            newVal = 500;
    }

    m_status = newVal;
    return 0;
}

result_t HttpResponse::get_cookies(obj_ptr<List_base> &retVal)
{
    if (!m_cookies)
    {
        obj_ptr<List> cookies = new List();

        int32_t len, i;
        obj_ptr<List_base> headers;

        allHeader("Set-Cookie", headers);
        removeHeader("Set-Cookie");

        headers->get_length(len);

        for (i = 0; i < len; i ++)
        {
            Variant v;
            std::string str;
            obj_ptr<HttpCookie> cookie;

            headers->_indexed_getter(i, v);
            str = v.string();

            cookie = new HttpCookie();
            if (cookie->parse(str.c_str()) >= 0)
                cookies->append(cookie);
        }

        m_cookies = cookies;
    }

    retVal = m_cookies;
    return 0;
}

result_t HttpResponse::addCookie(HttpCookie_base *cookie)
{
    obj_ptr<List_base> cookies;
    Variant v;

    v = cookie;
    get_cookies(cookies);
    cookies->push(v);

    return 0;
}

result_t HttpResponse::redirect(const char *url)
{
    clear();

    m_status = 302;
    setHeader("Location", url);
    return 0;
}

result_t HttpResponse::sendHeader(Stream_base* stm, exlib::AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_cookies)
    {
        int32_t len, i;

        m_cookies->get_length(len);

        for (i = 0; i < len; i ++)
        {
            Variant v;
            obj_ptr<object_base> cookie;
            std::string str;

            m_cookies->_indexed_getter(i, v);
            cookie = v.object();

            if (cookie)
            {
                cookie->toString(str);
                addHeader("Set-Cookie", str.c_str());
            }
        }

        m_cookies.Release();
    }

    int pos = shortcut[m_status / 100 - 1] + m_status % 100;
    std::string strCommand;

    get_protocol(strCommand);
    strCommand.append(status_lines[pos], status_lines_size[pos]);

    return m_message.sendHeader(stm, strCommand, ac);
}

} /* namespace fibjs */
