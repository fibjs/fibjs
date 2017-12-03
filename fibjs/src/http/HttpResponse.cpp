/*
 * HttpResponse.cpp
 *
 *  Created on: Aug 13, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpResponse.h"
#include "HttpCookie.h"
#include "Buffer.h"

namespace fibjs {

result_t HttpResponse_base::_new(obj_ptr<HttpResponse_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new HttpResponse();
    return 0;
}

result_t HttpResponse::get_protocol(exlib::string& retVal)
{
    return m_message->get_protocol(retVal);
}

result_t HttpResponse::set_protocol(exlib::string newVal)
{
    return m_message->set_protocol(newVal);
}

result_t HttpResponse::get_headers(obj_ptr<HttpCollection_base>& retVal)
{
    return m_message->get_headers(retVal);
}

result_t HttpResponse::get_body(obj_ptr<SeekableStream_base>& retVal)
{
    return m_message->get_body(retVal);
}

result_t HttpResponse::set_body(SeekableStream_base* newVal)
{
    return m_message->set_body(newVal);
}

result_t HttpResponse::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    return m_message->read(bytes, retVal, ac);
}

result_t HttpResponse::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return m_message->readAll(retVal, ac);
}

result_t HttpResponse::write(Buffer_base* data, AsyncEvent* ac)
{
    return m_message->write(data, ac);
}

result_t HttpResponse::json(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal)
{
    return m_message->json(data, retVal);
}

result_t HttpResponse::json(v8::Local<v8::Value>& retVal)
{
    return m_message->json(retVal);
}

result_t HttpResponse::get_length(int64_t& retVal)
{
    return m_message->get_length(retVal);
}

result_t HttpResponse::get_keepAlive(bool& retVal)
{
    return m_message->get_keepAlive(retVal);
}

result_t HttpResponse::set_keepAlive(bool newVal)
{
    return m_message->set_keepAlive(newVal);
}

result_t HttpResponse::get_upgrade(bool& retVal)
{
    return m_message->get_upgrade(retVal);
}

result_t HttpResponse::set_upgrade(bool newVal)
{
    return m_message->set_upgrade(newVal);
}

result_t HttpResponse::get_maxHeadersCount(int32_t& retVal)
{
    return m_message->get_maxHeadersCount(retVal);
}

result_t HttpResponse::set_maxHeadersCount(int32_t newVal)
{
    return m_message->set_maxHeadersCount(newVal);
}

result_t HttpResponse::get_maxBodySize(int32_t& retVal)
{
    return m_message->get_maxBodySize(retVal);
}

result_t HttpResponse::set_maxBodySize(int32_t newVal)
{
    return m_message->set_maxBodySize(newVal);
}

result_t HttpResponse::get_socket(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_socket(retVal);
}

result_t HttpResponse::hasHeader(exlib::string name, bool& retVal)
{
    return m_message->hasHeader(name, retVal);
}

result_t HttpResponse::firstHeader(exlib::string name, Variant& retVal)
{
    return m_message->firstHeader(name, retVal);
}

result_t HttpResponse::allHeader(exlib::string name, obj_ptr<NArray>& retVal)
{
    return m_message->allHeader(name, retVal);
}

result_t HttpResponse::addHeader(v8::Local<v8::Object> map)
{
    return m_message->addHeader(map);
}

result_t HttpResponse::addHeader(exlib::string name, Variant value)
{
    return m_message->addHeader(name, value);
}

result_t HttpResponse::setHeader(v8::Local<v8::Object> map)
{
    return m_message->setHeader(map);
}

result_t HttpResponse::setHeader(exlib::string name, Variant value)
{
    return m_message->setHeader(name, value);
}

result_t HttpResponse::removeHeader(exlib::string name)
{
    return m_message->removeHeader(name);
}

result_t HttpResponse::get_value(exlib::string& retVal)
{
    return m_message->get_value(retVal);
}

result_t HttpResponse::set_value(exlib::string newVal)
{
    return m_message->set_value(newVal);
}

result_t HttpResponse::get_params(obj_ptr<NArray>& retVal)
{
    return m_message->get_params(retVal);
}

result_t HttpResponse::get_type(int32_t& retVal)
{
    return m_message->get_type(retVal);
}

result_t HttpResponse::set_type(int32_t newVal)
{
    return m_message->set_type(newVal);
}

result_t HttpResponse::get_data(v8::Local<v8::Value>& retVal)
{
    return m_message->get_data(retVal);
}

result_t HttpResponse::get_lastError(exlib::string& retVal)
{
    return m_message->get_lastError(retVal);
}

result_t HttpResponse::set_lastError(exlib::string newVal)
{
    return m_message->set_lastError(newVal);
}

result_t HttpResponse::end()
{
    return m_message->end();
}

result_t HttpResponse::isEnded(bool& retVal)
{
    return m_message->isEnded(retVal);
}

result_t HttpResponse::clear()
{
    m_message->clear();

    m_cookies.Release();
    m_statusCode = 200;

    return 0;
}

static const char* const status_lines[] = {
#define LEVEL_100 0
    " 100 Continue", " 101 Switching Protocols", " 102 Processing",
#define LEVEL_200 3
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
static int32_t shortcut[6] = { LEVEL_100, LEVEL_200, LEVEL_300, LEVEL_400, LEVEL_500, RESPONSE_CODES };
static unsigned char status_lines_size[RESPONSE_CODES];

static class _init_status_line {
public:
    _init_status_line()
    {
        int32_t i;

        for (i = 0; i < RESPONSE_CODES; i++)
            status_lines_size[i] = (unsigned char)qstrlen(status_lines[i]);
    }
} s_init_status_line;

result_t HttpResponse::sendTo(Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_cookies) {
        int32_t len, i;

        m_cookies->get_length(len);

        for (i = 0; i < len; i++) {
            Variant v;
            obj_ptr<object_base> cookie;
            exlib::string str;

            m_cookies->_indexed_getter(i, v);
            cookie = v.object();

            if (cookie) {
                cookie->toString(str);
                addHeader("Set-Cookie", str);
            }
        }

        m_cookies.Release();
    }

    exlib::string strCommand;
    exlib::string statusMessage;

    if (m_statusMessage.empty()) {
        int32_t statusCode = m_statusCode;

        if (statusCode >= 100 && statusCode < 600) {
            int32_t n = statusCode / 100;
            if (shortcut[n - 1] + statusCode % 100 < shortcut[n]) {
                int32_t pos = shortcut[statusCode / 100 - 1] + statusCode % 100;
                statusMessage.assign(status_lines[pos], status_lines_size[pos]);
            }
        }

        if (statusMessage.empty()) {
            char buf[16];
            sprintf(buf, " %d Unknown", statusCode);
            statusMessage = buf;
        }
    } else {
        char buf[16];

        sprintf(buf, " %d ", m_statusCode);
        statusMessage = buf;
        statusMessage.append(m_statusMessage);
    }

    get_protocol(strCommand);
    strCommand.append(statusMessage);

    return m_message->sendTo(stm, strCommand, ac);
}

result_t HttpResponse::readFrom(Stream_base* stm, AsyncEvent* ac)
{
    class asyncReadFrom : public AsyncState {
    public:
        asyncReadFrom(HttpResponse* pThis, BufferedStream_base* stm,
            AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
        {
            m_pThis->clear();
            set(begin);
        }

        static int32_t begin(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            pThis->set(command);
            return pThis->m_stm->readLine(HTTP_MAX_LINE, pThis->m_strLine,
                pThis);
        }

        static int32_t command(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;
            result_t hr;
            const char* c_str = pThis->m_strLine.c_str();
            int32_t len = (int32_t)pThis->m_strLine.length();

            if (len < 12 || c_str[8] != ' '
                || !qisdigit(c_str[9]) || !qisdigit(c_str[10]) || !qisdigit(c_str[11])
                || qisdigit(c_str[12]))
                return CHECK_ERROR(Runtime::setError("HttpResponse: bad protocol: " + pThis->m_strLine));

            pThis->m_pThis->set_statusCode((c_str[9] - '0') * 100 + (c_str[10] - '0') * 10 + (c_str[11] - '0'));
            pThis->m_pThis->set_statusMessage(c_str[12] == ' ' ? c_str + 13 : c_str + 12);

            pThis->m_strLine.resize(8);
            hr = pThis->m_pThis->set_protocol(pThis->m_strLine);
            if (hr < 0)
                return hr;

            pThis->done();
            return pThis->m_pThis->m_message->readFrom(pThis->m_stm, pThis);
        }

    public:
        obj_ptr<HttpResponse> m_pThis;
        obj_ptr<BufferedStream_base> m_stm;
        exlib::string m_strLine;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<BufferedStream_base> _stm = BufferedStream_base::getInstance(stm);
    if (!_stm)
        return CHECK_ERROR(Runtime::setError("HttpResponse: only accept BufferedStream object."));

    return (new asyncReadFrom(this, _stm, ac))->post(0);
}

result_t HttpResponse::get_stream(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_stream(retVal);
}

result_t HttpResponse::get_statusCode(int32_t& retVal)
{
    retVal = m_statusCode;
    return 0;
}

result_t HttpResponse::set_statusCode(int32_t newVal)
{
    m_statusCode = newVal;
    return 0;
}

result_t HttpResponse::get_statusMessage(exlib::string& retVal)
{
    retVal = m_statusMessage;
    return 0;
}

result_t HttpResponse::set_statusMessage(exlib::string newVal)
{
    m_statusMessage = newVal;
    return 0;
}

result_t HttpResponse::writeHead(int32_t statusCode, exlib::string statusMessage, v8::Local<v8::Object> headers)
{
    set_statusCode(statusCode);
    set_statusMessage(statusMessage);
    addHeader(headers);
    return 0;
}

result_t HttpResponse::writeHead(int32_t statusCode, v8::Local<v8::Object> headers)
{
    set_statusCode(statusCode);
    addHeader(headers);
    return 0;
}

result_t HttpResponse::get_cookies(obj_ptr<NArray>& retVal)
{
    if (!m_cookies) {
        obj_ptr<NArray> cookies = new NArray();

        int32_t len, i;
        obj_ptr<NArray> headers;

        allHeader("Set-Cookie", headers);
        removeHeader("Set-Cookie");

        headers->get_length(len);

        for (i = 0; i < len; i++) {
            Variant v;
            exlib::string str;
            obj_ptr<HttpCookie> cookie;

            headers->_indexed_getter(i, v);
            str = v.string();

            cookie = new HttpCookie();
            if (cookie->parse(str) >= 0)
                cookies->append(cookie);
        }

        m_cookies = cookies;
    }

    retVal = m_cookies;
    return 0;
}

result_t HttpResponse::addCookie(HttpCookie_base* cookie)
{
    obj_ptr<NArray> cookies;
    Variant v;

    v = cookie;
    get_cookies(cookies);
    cookies->append(v);

    return 0;
}

result_t HttpResponse::redirect(exlib::string url)
{
    m_statusCode = 302;
    setHeader("Location", url);
    return 0;
}

result_t HttpResponse::sendHeader(Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_cookies) {
        int32_t len, i;

        m_cookies->get_length(len);

        for (i = 0; i < len; i++) {
            Variant v;
            obj_ptr<object_base> cookie;
            exlib::string str;

            m_cookies->_indexed_getter(i, v);
            cookie = v.object();

            if (cookie) {
                cookie->toString(str);
                addHeader("Set-Cookie", str);
            }
        }

        m_cookies.Release();
    }

    int32_t pos = shortcut[m_statusCode / 100 - 1] + m_statusCode % 100;
    exlib::string strCommand;

    get_protocol(strCommand);
    strCommand.append(status_lines[pos], status_lines_size[pos]);

    return m_message->sendHeader(stm, strCommand, ac);
}

} /* namespace fibjs */
