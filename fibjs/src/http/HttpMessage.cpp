/*
 * HttpMessage.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpMessage.h"
#include "parse.h"
#include "Buffer.h"
#include <string.h>

namespace fibjs {

#define TINY_SIZE 32768

class asyncSendTo : public AsyncState {
public:
    asyncSendTo(HttpMessage* pThis, Stream_base* stm,
        exlib::string& strCommand, AsyncEvent* ac,
        bool headerOnly = false)
        : AsyncState(ac)
        , m_pThis(pThis)
        , m_stm(stm)
        , m_strCommand(
              strCommand)
        , m_headerOnly(headerOnly)
    {
        m_contentLength = 0;
        m_pThis->get_length(m_contentLength);

        if (!m_headerOnly && m_contentLength > 0 && m_contentLength < TINY_SIZE)
            next(tinybody);
        else
            next(header);
    }

    ON_STATE(asyncSendTo, tinybody)
    {
        m_pThis->body()->rewind();
        return m_pThis->body()->read((int32_t)m_contentLength, m_buffer, next(header));
    }

    ON_STATE(asyncSendTo, header)
    {
        size_t sz = m_strCommand.length();
        size_t sz1;
        exlib::string m_strBuf;
        char* pBuf;

        if (m_buffer != NULL) {
            m_body_buf = Buffer::Cast(m_buffer);
            m_body_length = m_body_buf->length();
            m_buffer.Release();

            if (m_contentLength != (int32_t)m_body_length)
                return CHECK_ERROR(Runtime::setError("HttpMessage: body is not complete."));
        }

        sz1 = m_pThis->size();
        m_strBuf = m_strCommand;
        m_strBuf.resize(sz + sz1 + 2 + m_body_length);

        pBuf = m_strBuf.c_buffer() + sz;
        *pBuf++ = '\r';
        *pBuf++ = '\n';

        pBuf += m_pThis->getData(pBuf, sz1);

        if (m_body_length > 0)
            memcpy(pBuf, m_body_buf->data(), m_body_length);

        m_buffer = new Buffer(m_strBuf.c_str(), m_strBuf.length());
        return m_stm->write(m_buffer, next(body));
    }

    ON_STATE(asyncSendTo, body)
    {
        if (m_headerOnly || m_contentLength == 0 || m_body_length > 0)
            return next();

        m_pThis->body()->rewind();
        return m_pThis->body()->copyTo(m_stm, m_contentLength, m_copySize, next(body_ok));
    }

    ON_STATE(asyncSendTo, body_ok)
    {
        if (m_contentLength != m_copySize)
            return CHECK_ERROR(Runtime::setError("HttpMessage: body is not complete."));
        return next();
    }

public:
    HttpMessage* m_pThis;
    obj_ptr<Stream_base> m_stm;
    obj_ptr<Buffer_base> m_buffer;
    int64_t m_contentLength;
    int64_t m_copySize;
    size_t m_body_length = 0;
    obj_ptr<Buffer> m_body_buf;
    exlib::string m_strCommand;
    const char* m_strStatus;
    int32_t m_nStatus;
    bool m_headerOnly;
};

result_t HttpMessage::get_data(v8::Local<v8::Value>& retVal)
{
    exlib::string strType;

    if (firstHeader("Content-Type", strType) == CALL_RETURN_NULL)
        return Message::get_data(retVal);

    size_t pos = strType.find(';');
    if (pos != exlib::string::npos)
        strType = strType.substr(0, pos);

    if (strType.find("json") != exlib::string::npos)
        return Message::json(retVal);

    if (strType == "application/msgpack")
        return Message::pack(retVal);

    return Message::get_data(retVal);
}

result_t HttpMessage::json(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal)
{
    setHeader("Content-Type", "application/json");
    return Message::json(data, retVal);
}

result_t HttpMessage::json(v8::Local<v8::Value>& retVal)
{
    exlib::string strType;

    if (firstHeader("Content-Type", strType) == CALL_RETURN_NULL)
        return CHECK_ERROR(Runtime::setError("HttpRequest: Content-Type is missing."));

    if (strType.find("json") == exlib::string::npos)
        return CHECK_ERROR(Runtime::setError("HttpMessage: Invalid content type."));

    return Message::json(retVal);
}

result_t HttpMessage::pack(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal)
{
    setHeader("Content-Type", "application/msgpack");
    return Message::pack(data, retVal);
}

result_t HttpMessage::pack(v8::Local<v8::Value>& retVal)
{
    exlib::string strType;

    if (firstHeader("Content-Type", strType) == CALL_RETURN_NULL)
        return CHECK_ERROR(Runtime::setError("HttpRequest: Content-Type is missing."));

    size_t pos = strType.find(';');
    if (pos != exlib::string::npos)
        strType = strType.substr(0, pos);

    if (strType != "application/msgpack")
        return CHECK_ERROR(Runtime::setError("HttpMessage: Invalid content type."));

    return Message::pack(retVal);
}

result_t HttpMessage::sendTo(Stream_base* stm, exlib::string& strCommand,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSendTo(this, stm, strCommand, ac))->post(0);
}

result_t HttpMessage::sendHeader(Stream_base* stm, exlib::string& strCommand,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSendTo(this, stm, strCommand, ac, true))->post(0);
}

result_t HttpMessage::readFrom(Stream_base* stm, AsyncEvent* ac)
{
    class asyncReadFrom : public AsyncState {
    public:
        asyncReadFrom(HttpMessage* pThis, BufferedStream_base* stm,
            AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_contentLength(-1)
            , m_bChunked(false)
            , m_headCount(0)
        {
            next(begin);
        }

        ON_STATE(asyncReadFrom, begin)
        {
            return m_stm->readLine(HTTP_MAX_LINE, m_strLine, next(header));
        }

        ON_STATE(asyncReadFrom, header)
        {
            if (m_strLine.length() > 0) {
                if (!qstricmp(m_strLine.c_str(), "content-length:", 15)) {
                    m_contentLength = atoi(m_strLine.c_str() + 15);

                    if ((m_contentLength < 0)
                        || (m_pThis->m_maxBodySize >= 0
                            && m_contentLength > (int64_t)m_pThis->m_maxBodySize * 1024 * 1024))
                        return CHECK_ERROR(Runtime::setError("HttpMessage: body is too huge."));

                    if (m_pThis->m_bNoBody) {
                        result_t hr = m_pThis->addHeader(m_strLine);
                        if (hr < 0)
                            return hr;

                        m_headCount++;
                    }
                } else if (!qstricmp(m_strLine.c_str(),
                               "transfer-encoding:", 18)) {
                    _parser p(m_strLine.c_str() + 18,
                        (int32_t)m_strLine.length() - 18);

                    p.skipSpace();
                    if (qstricmp(p.now(), "chunked"))
                        return CHECK_ERROR(Runtime::setError("HttpMessage: unknown transfer-encoding."));

                    m_bChunked = true;
                } else {
                    result_t hr = m_pThis->addHeader(m_strLine);
                    if (hr < 0)
                        return hr;

                    m_headCount++;
                }

                if (m_headCount > m_pThis->m_maxHeadersCount)
                    return CHECK_ERROR(Runtime::setError("HttpMessage: too many headers."));

                return m_stm->readLine(HTTP_MAX_LINE, m_strLine, this);
            }

            if (m_bChunked) {
                if (m_pThis->m_maxBodySize == 0)
                    return next();

                if (m_contentLength > 0)
                    return CHECK_ERROR(CALL_E_INVALID_DATA);
                m_contentLength = 0;

                m_pThis->get_body(m_body);
                return next(chunk_head);
            }

            if (!m_pThis->m_bNoBody && (m_contentLength > 0 || (m_pThis->m_bResponse && !m_pThis->m_keepAlive && m_contentLength == -1))) {
                m_pThis->get_body(m_body);
                return m_stm->copyTo(m_body, m_contentLength, m_copySize, next(body));
            }

            return next();
        }

        ON_STATE(asyncReadFrom, body)
        {
            if (!m_pThis->m_bNoBody && m_contentLength > 0 && m_contentLength != m_copySize)
                return CHECK_ERROR(Runtime::setError("HttpMessage: body is not complete."));

            m_body->rewind();
            return next();
        }

        ON_STATE(asyncReadFrom, chunk_head)
        {
            return m_stm->readLine(HTTP_MAX_LINE, m_strLine, next(chunk_body));
        }

        ON_STATE(asyncReadFrom, chunk_body)
        {
            _parser p(m_strLine);
            char ch;
            int64_t sz = 0;

            p.skipSpace();

            if (!qisxdigit(p.get()))
                return CHECK_ERROR(Runtime::setError("HttpMessage: bad chunk size."));

            while (qisxdigit(ch = p.get())) {
                sz = (sz << 4) + qhex(ch);
                p.skip();
            }

            if (sz) {
                if (m_pThis->m_maxBodySize >= 0
                    && sz + m_contentLength > (int64_t)m_pThis->m_maxBodySize * 1024 * 1024)
                    return CHECK_ERROR(Runtime::setError("HttpMessage: body is too huge."));
                return m_stm->copyTo(m_body, sz, m_copySize, next(chunk_body_end));
            }

            return m_stm->readLine(HTTP_MAX_LINE, m_strLine, next(chunk_end));
        }

        ON_STATE(asyncReadFrom, chunk_body_end)
        {
            m_contentLength += m_copySize;
            return m_stm->readLine(HTTP_MAX_LINE, m_strLine, next(chunk_head));
        }

        ON_STATE(asyncReadFrom, chunk_end)
        {
            m_body->rewind();
            return next();
        }

    public:
        HttpMessage* m_pThis;
        obj_ptr<BufferedStream_base> m_stm;
        obj_ptr<SeekableStream_base> m_body;
        exlib::string m_strLine;
        int64_t m_contentLength;
        bool m_bChunked;
        int32_t m_headCount;
        int64_t m_copySize;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<BufferedStream_base> _stm = BufferedStream_base::getInstance(stm);
    if (!_stm)
        return CHECK_ERROR(Runtime::setError("HttpMessage: only accept BufferedStream object."));

    _stm->get_stream(m_socket);
    m_stm = _stm;

    return (new asyncReadFrom(this, _stm, ac))->post(0);
}

void HttpMessage::addHeader(const char* name, int32_t szName, const char* value,
    int32_t szValue)
{
    if (szName == 10 && !qstricmp(name, "connection", szName)) {
        if (qstristr(value, "upgrade")) {
            m_upgrade = true;
            m_keepAlive = true;
        } else
            m_keepAlive = !!qstristr(value, "keep-alive");
    } else
        m_headers->add(name, szName, value, szValue);
}

result_t HttpMessage::addHeader(exlib::string& strLine)
{
    int32_t p2;
    _parser p(strLine);

    p.skipWord(':');
    p2 = p.pos;
    if (0 == p2 || !p.want(':'))
        return CHECK_ERROR(Runtime::setError("HttpMessage: bad header: " + strLine));

    p.skipSpace();
    addHeader(p.string, p2, p.now(), p.left());

    return 0;
}

size_t HttpMessage::size()
{
    size_t sz = 2 + m_headers->size();
    int64_t l;

    // connection 10
    sz += 10 + 4 + (m_upgrade ? 7 : (m_keepAlive ? 10 : 5));

    // content-length 14
    get_length(l);
    if (l > 0) {
        sz += 14 + 4;
        while (l > 0) {
            l /= 10;
            sz++;
        }
    } else if (m_bResponse)
        sz += 19;

    return sz;
}

inline void cp(char* buf, size_t sz, size_t& pos, const char* str, size_t szStr)
{
    buf += pos;

    pos += szStr;
    if (pos > sz) {
        szStr -= pos - sz;
        pos = sz;
    }

    memcpy(buf, str, szStr);
}

size_t HttpMessage::getData(char* buf, size_t sz)
{
    size_t pos = m_headers->getData(buf, sz);
    int64_t l;

    // connection 10
    cp(buf, sz, pos, "Connection: ", 12);
    if (m_upgrade)
        cp(buf, sz, pos, "upgrade\r\n", 9);
    else if (m_keepAlive)
        cp(buf, sz, pos, "keep-alive\r\n", 12);
    else
        cp(buf, sz, pos, "close\r\n", 7);

    // content-length 14
    get_length(l);
    if (l > 0) {
        char s[32];
        char* p;
        int32_t n;

        cp(buf, sz, pos, "Content-Length: ", 16);
        p = s + 32;
        *--p = 0;
        *--p = '\n';
        *--p = '\r';
        n = 2;

        while (l > 0) {
            *--p = l % 10 + '0';
            n++;
            l /= 10;
        }

        cp(buf, sz, pos, p, n);
    } else if (m_bResponse)
        cp(buf, sz, pos, "Content-Length: 0\r\n", 19);

    cp(buf, sz, pos, "\r\n", 2);

    return pos;
}

result_t HttpMessage::get_protocol(exlib::string& retVal)
{
    retVal = m_protocol;
    return 0;
}

result_t HttpMessage::set_protocol(exlib::string newVal)
{
    const char* c_str = newVal.c_str();

    if (qstrcmp(c_str, "HTTP/", 5) || !qisdigit(c_str[5]) || c_str[6] != '.'
        || !qisdigit(c_str[7]) || c_str[8])
        return CHECK_ERROR(Runtime::setError("HttpMessage: bad protocol version."));

    m_keepAlive = ((c_str[5] - '0') * 10 + c_str[7] - '0') > 10;

    m_protocol = newVal;
    return 0;
}

result_t HttpMessage::get_headers(obj_ptr<HttpCollection_base>& retVal)
{
    retVal = m_headers;
    return 0;
}

result_t HttpMessage::get_keepAlive(bool& retVal)
{
    retVal = m_keepAlive;
    return 0;
}

result_t HttpMessage::set_keepAlive(bool newVal)
{
    m_keepAlive = newVal;
    return 0;
}

result_t HttpMessage::get_upgrade(bool& retVal)
{
    retVal = m_upgrade;
    return 0;
}

result_t HttpMessage::set_upgrade(bool newVal)
{
    m_upgrade = newVal;
    return 0;
}

result_t HttpMessage::get_maxHeadersCount(int32_t& retVal)
{
    retVal = m_maxHeadersCount;
    return 0;
}

result_t HttpMessage::set_maxHeadersCount(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxHeadersCount = newVal;
    return 0;
}

result_t HttpMessage::get_maxBodySize(int32_t& retVal)
{
    retVal = m_maxBodySize;
    return 0;
}

result_t HttpMessage::set_maxBodySize(int32_t newVal)
{
    m_maxBodySize = newVal;
    return 0;
}

result_t HttpMessage::get_socket(obj_ptr<Stream_base>& retVal)
{
    if (!m_socket)
        return CALL_RETURN_NULL;

    retVal = m_socket;
    return 0;
}

result_t HttpMessage::hasHeader(exlib::string name, bool& retVal)
{
    return m_headers->has(name, retVal);
}

result_t HttpMessage::firstHeader(exlib::string name, exlib::string& retVal)
{
    return m_headers->first(name, retVal);
}

result_t HttpMessage::allHeader(exlib::string name, obj_ptr<NObject>& retVal)
{
    return m_headers->all(name, retVal);
}

result_t HttpMessage::addHeader(v8::Local<v8::Object> map)
{
    return m_headers->add(map);
}

result_t HttpMessage::addHeader(exlib::string name, exlib::string value)
{
    return m_headers->add(name, value);
}

result_t HttpMessage::addHeader(exlib::string name, v8::Local<v8::Array> values)
{
    return m_headers->add(name, values);
}

result_t HttpMessage::setHeader(v8::Local<v8::Object> map)
{
    return m_headers->set(map);
}

result_t HttpMessage::setHeader(exlib::string name, exlib::string value)
{
    return m_headers->set(name, value);
}

result_t HttpMessage::setHeader(exlib::string name, v8::Local<v8::Array> values)
{
    return m_headers->set(name, values);
}

result_t HttpMessage::removeHeader(exlib::string name)
{
    return m_headers->remove(name);
}

result_t HttpMessage::get_stream(obj_ptr<Stream_base>& retVal)
{
    if (!m_stm)
        return CALL_RETURN_NULL;

    retVal = m_stm;
    return 0;
}

result_t HttpMessage::clear()
{
    Message::clear();

    m_protocol.assign("HTTP/1.1", 8);
    m_keepAlive = true;
    m_upgrade = false;

    m_origin.clear();
    m_encoding.clear();

    m_headers->clear();

    m_stm.Release();
    m_socket.Release();

    return 0;
}

} /* namespace fibjs */
