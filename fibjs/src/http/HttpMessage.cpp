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
            set(tinybody);
        else
            set(header);
    }

    static int32_t tinybody(AsyncState* pState, int32_t n)
    {
        asyncSendTo* pThis = (asyncSendTo*)pState;

        pThis->m_pThis->body()->rewind();

        pThis->set(header);
        return pThis->m_pThis->body()->read(
            (int32_t)pThis->m_contentLength, pThis->m_buffer, pThis);
    }

    static int32_t header(AsyncState* pState, int32_t n)
    {
        asyncSendTo* pThis = (asyncSendTo*)pState;
        size_t sz = pThis->m_strCommand.length();
        size_t sz1;
        exlib::string m_strBuf;
        char* pBuf;

        if (pThis->m_buffer != NULL) {
            pThis->m_buffer->toString(pThis->m_body);
            pThis->m_buffer.Release();

            if (pThis->m_contentLength != (int32_t)pThis->m_body.length())
                return CHECK_ERROR(Runtime::setError("HttpMessage: body is not complete."));
        }

        sz1 = pThis->m_pThis->size();
        m_strBuf = pThis->m_strCommand;
        m_strBuf.resize(sz + sz1 + 2 + pThis->m_body.length());

        pBuf = &m_strBuf[sz];
        *pBuf++ = '\r';
        *pBuf++ = '\n';

        pBuf += pThis->m_pThis->getData(pBuf, sz1);

        if (pThis->m_body.length() > 0)
            memcpy(pBuf, pThis->m_body.c_str(), pThis->m_body.length());

        pThis->m_buffer = new Buffer(m_strBuf);

        if (pThis->m_headerOnly || pThis->m_contentLength == 0 || pThis->m_body.length() > 0)
            pThis->done();
        else
            pThis->set(body);

        return pThis->m_stm->write(pThis->m_buffer, pThis);
    }

    static int32_t body(AsyncState* pState, int32_t n)
    {
        asyncSendTo* pThis = (asyncSendTo*)pState;

        if (pThis->m_contentLength == 0)
            return pThis->done();

        pThis->m_pThis->body()->rewind();

        pThis->set(body_ok);
        return pThis->m_pThis->body()->copyTo(pThis->m_stm,
            pThis->m_contentLength, pThis->m_copySize, pThis);
    }

    static int32_t body_ok(AsyncState* pState, int32_t n)
    {
        asyncSendTo* pThis = (asyncSendTo*)pState;

        if (pThis->m_contentLength != pThis->m_copySize)
            return CHECK_ERROR(Runtime::setError("HttpMessage: body is not complete."));

        return pThis->done();
    }

public:
    HttpMessage* m_pThis;
    obj_ptr<Stream_base> m_stm;
    obj_ptr<Buffer_base> m_buffer;
    int64_t m_contentLength;
    int64_t m_copySize;
    exlib::string m_body;
    exlib::string m_strCommand;
    const char* m_strStatus;
    int32_t m_nStatus;
    bool m_headerOnly;
};

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

    size_t pos = strType.find(';');
    if (pos != exlib::string::npos)
        strType = strType.substr(0, pos);

    if (strType != "application/json")
        return CHECK_ERROR(Runtime::setError("HttpMessage: Invalid content type."));

    return Message::json(retVal);
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
            , m_contentLength(0)
            , m_bChunked(false)
            , m_headCount(0)
        {
            set(begin);
        }

        static int32_t begin(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            pThis->set(header);
            return pThis->m_stm->readLine(HTTP_MAX_LINE, pThis->m_strLine,
                pThis);
        }

        static int32_t header(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            if (pThis->m_strLine.length() > 0) {
                if (!qstricmp(pThis->m_strLine.c_str(), "content-length:", 15)) {
                    pThis->m_contentLength = atoi(pThis->m_strLine.c_str() + 15);

                    if ((pThis->m_contentLength < 0)
                        || (pThis->m_pThis->m_maxBodySize >= 0
                               && pThis->m_contentLength > pThis->m_pThis->m_maxBodySize * 1024 * 1024))
                        return CHECK_ERROR(Runtime::setError("HttpMessage: body is too huge."));
                } else if (!qstricmp(pThis->m_strLine.c_str(),
                               "transfer-encoding:", 18)) {
                    _parser p(pThis->m_strLine.c_str() + 18,
                        (int32_t)pThis->m_strLine.length() - 18);

                    p.skipSpace();
                    if (qstricmp(p.now(), "chunked"))
                        return CHECK_ERROR(Runtime::setError("HttpMessage: unknown transfer-encoding."));

                    pThis->m_bChunked = true;
                } else {
                    result_t hr = pThis->m_pThis->addHeader(pThis->m_strLine);
                    if (hr < 0)
                        return hr;

                    pThis->m_headCount++;
                    if (pThis->m_headCount > pThis->m_pThis->m_maxHeadersCount)
                        return CHECK_ERROR(Runtime::setError("HttpMessage: too many headers."));
                }

                return pThis->m_stm->readLine(HTTP_MAX_LINE, pThis->m_strLine,
                    pThis);
            }

            if (pThis->m_bChunked) {
                if (pThis->m_pThis->m_maxBodySize == 0)
                    return pThis->done();

                if (pThis->m_contentLength)
                    return CHECK_ERROR(CALL_E_INVALID_DATA);

                pThis->m_pThis->get_body(pThis->m_body);
                return chunk_head(pState, n);
            }

            if (!pThis->m_pThis->m_bNoBody && pThis->m_contentLength > 0) {
                pThis->m_pThis->get_body(pThis->m_body);

                pThis->set(body);
                return pThis->m_stm->copyTo(pThis->m_body, pThis->m_contentLength, pThis->m_copySize, pThis);
            }

            return pThis->done();
        }

        static int32_t body(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            if (!pThis->m_pThis->m_bNoBody && pThis->m_contentLength != pThis->m_copySize)
                return CHECK_ERROR(Runtime::setError("HttpMessage: body is not complete."));

            pThis->m_body->rewind();

            return pThis->done();
        }

        static int32_t chunk_head(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            pThis->set(chunk_body);
            return pThis->m_stm->readLine(HTTP_MAX_LINE, pThis->m_strLine,
                pThis);
        }

        static int32_t chunk_body(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;
            _parser p(pThis->m_strLine);
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
                if (pThis->m_pThis->m_maxBodySize >= 0
                    && sz + pThis->m_contentLength > pThis->m_pThis->m_maxBodySize * 1024 * 1024)
                    return CHECK_ERROR(Runtime::setError("HttpMessage: body is too huge."));
                pThis->set(chunk_body_end);
                return pThis->m_stm->copyTo(pThis->m_body, sz,
                    pThis->m_copySize, pThis);
            }

            pThis->set(chunk_end);
            return pThis->m_stm->readLine(HTTP_MAX_LINE, pThis->m_strLine,
                pThis);
        }

        static int32_t chunk_body_end(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            pThis->m_contentLength += pThis->m_copySize;
            pThis->set(chunk_head);
            return pThis->m_stm->readLine(HTTP_MAX_LINE, pThis->m_strLine,
                pThis);
        }

        static int32_t chunk_end(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            pThis->m_body->rewind();

            return pThis->done();
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
