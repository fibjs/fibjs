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
#include "Blob.h"
#include "MemoryStream.h"
#include "ChunkedStream.h"
#include "ifs/URLSearchParams.h"
#include "ifs/FormData.h"
#include <string.h>

namespace fibjs {

result_t body_to_stream(Isolate* isolate, v8::Local<v8::Value> body,
    obj_ptr<SeekableStream_base>& retVal, Headers_base* headers, bool defaultFormUrlEncoded)
{
    result_t hr;

    // null or undefined - no body
    if (body.IsEmpty() || body->IsNullOrUndefined())
        return CALL_RETURN_NULL;

    // Already a SeekableStream - use directly
    obj_ptr<SeekableStream_base> stm = SeekableStream_base::getInstance(body);
    if (stm) {
        retVal = stm;
        return 0;
    }

    // Convert various types to Buffer and write to MemoryStream
    stm = new MemoryStream();
    obj_ptr<Buffer_base> buf;
    obj_ptr<Blob_base> blob;
    obj_ptr<FormData_base> formData;
    obj_ptr<URLSearchParams_base> params;

    // Default content types based on context:
    // - HttpClient request: application/x-www-form-urlencoded (historical behavior)
    // - Response constructor: text/plain;charset=UTF-8 (Web API standard)
    const char* stringDefaultType = defaultFormUrlEncoded ? "application/x-www-form-urlencoded" : "text/plain;charset=UTF-8";
    const char* binaryDefaultType = defaultFormUrlEncoded ? "application/x-www-form-urlencoded" : "application/octet-stream";

    if (body->IsString()) {
        hr = GetArgumentValue(isolate, body, buf);
        if (hr < 0)
            return hr;

        if (headers) {
            Variant ct;
            if (headers->first("Content-Type", ct) == CALL_RETURN_NULL)
                headers->set("Content-Type", stringDefaultType);
        }
    } else if ((params = URLSearchParams_base::getInstance(body)) != NULL) {
        exlib::string s;
        hr = params->toString(s);
        if (hr < 0)
            return hr;

        buf = new Buffer(s.c_str(), s.length());
        if (headers) {
            Variant ct;
            if (headers->first("Content-Type", ct) == CALL_RETURN_NULL)
                headers->set("Content-Type", "application/x-www-form-urlencoded");
        }
    } else if (IsJSBuffer(body, false) && GetArgumentValue(isolate, body, buf) == 0) {
        // Handle Buffer type directly
        if (headers) {
            Variant ct;
            if (headers->first("Content-Type", ct) == CALL_RETURN_NULL)
                headers->set("Content-Type", "application/octet-stream");
        }
    } else {
        bool has_ContentType = false;

        if ((blob = Blob_base::getInstance(body)) == NULL && GetArgumentValue(isolate, body, formData) == 0) {
            exlib::string mimeType;
            if (headers) {
                Variant ct;
                if (headers->first("Content-Type", ct) != CALL_RETURN_NULL) {
                    has_ContentType = true;
                    mimeType = ct.string();
                } else {
                    mimeType = "application/x-www-form-urlencoded";
                }
            } else {
                mimeType = "application/x-www-form-urlencoded";
            }

            hr = formData->encode(mimeType, blob);
            if (hr < 0)
                return hr;
        }

        if (blob || (blob = Blob_base::getInstance(body)) != NULL) {
            buf = blob.As<Blob>()->m_impl.getBuffer();
            if (headers) {
                Variant ct;
                if (!has_ContentType || headers->first("Content-Type", ct) == CALL_RETURN_NULL) {
                    exlib::string mimeType;
                    blob->get_type(mimeType);
                    if (!mimeType.empty())
                        headers->set("Content-Type", mimeType);
                    else
                        headers->set("Content-Type", binaryDefaultType);
                }
            }
        }

        if (!buf) {
            hr = GetArgumentValue(isolate, body, buf);
            if (hr < 0)
                return hr;

            if (headers) {
                Variant ct;
                if (headers->first("Content-Type", ct) == CALL_RETURN_NULL)
                    headers->set("Content-Type", binaryDefaultType);
            }
        }
    }

    stm->writeBuffer(buf, nullptr);
    stm->rewind();
    retVal = stm;
    return 0;
}

#define TINY_SIZE 32768

class asyncSendTo : public AsyncState {
public:
    asyncSendTo(HttpMessage* pThis, Stream_base* stm,
        exlib::string& strCommand, AsyncEvent* ac,
        bool headerOnly = false, bool content_length = true)
        : AsyncState(ac)
        , m_pThis(pThis)
        , m_stm(stm)
        , m_strCommand(
              strCommand)
        , m_headerOnly(headerOnly)
        , m_content_length(content_length)
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
        return m_pThis->body()->readBuffer((int32_t)m_contentLength, m_buffer, next(header));
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

        sz1 = m_pThis->getData(nullptr, 0, m_content_length);
        m_strBuf = m_strCommand;
        m_strBuf.resize(sz + sz1 + 2 + m_body_length);

        pBuf = m_strBuf.data() + sz;
        *pBuf++ = '\r';
        *pBuf++ = '\n';

        pBuf += m_pThis->getData(pBuf, sz1, m_content_length);

        if (m_body_length > 0)
            memcpy(pBuf, m_body_buf->data(), m_body_length);

        m_buffer = new Buffer(m_strBuf.c_str(), m_strBuf.length());
        return m_stm->writeBuffer(m_buffer, next(body));
    }

    ON_STATE(asyncSendTo, body)
    {
        if (m_headerOnly || m_contentLength == 0 || m_body_length > 0)
            return next();

        if (!m_pThis->body())
            return next();

        obj_ptr<SeekableStream_base> seekable = SeekableStream_base::getInstance(m_pThis->body());
        if (!seekable)
            return CHECK_ERROR(Runtime::setError("HttpMessage: request body must be seekable."));
        seekable->rewind();
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
    bool m_content_length;
};

result_t HttpMessage::get_sent(bool& retVal)
{
    retVal = m_sent;
    return 0;
}

result_t HttpMessage::text(exlib::string data, exlib::string& retVal, AsyncEvent* ac)
{
    return Message::text(data, retVal, ac);
}

result_t HttpMessage::text(exlib::string& retVal, AsyncEvent* ac)
{
    return Message::text(retVal, ac);
}

result_t HttpMessage::arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    return Message::arrayBuffer(retVal, ac);
}

result_t HttpMessage::json(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac)
{
    setHeader("Content-Type", "application/json");
    return Message::json(data, retVal, ac);
}

result_t HttpMessage::json(Variant& retVal, AsyncEvent* ac)
{
    exlib::string strType;

    if (firstHeader("Content-Type", strType) == CALL_RETURN_NULL)
        return CHECK_ERROR(Runtime::setError("HttpRequest: Content-Type is missing."));

    if (strType.find("json") == exlib::string::npos)
        return CHECK_ERROR(Runtime::setError("HttpMessage: Invalid content type."));

    return Message::json(retVal, ac);
}

result_t HttpMessage::pack(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac)
{
    setHeader("Content-Type", "application/msgpack");
    return Message::pack(data, retVal, ac);
}

result_t HttpMessage::pack(Variant& retVal, AsyncEvent* ac)
{
    exlib::string strType;

    if (firstHeader("Content-Type", strType) == CALL_RETURN_NULL)
        return CHECK_ERROR(Runtime::setError("HttpRequest: Content-Type is missing."));

    size_t pos = strType.find(';');
    if (pos != exlib::string::npos)
        strType = strType.substr(0, pos);

    if (strType != "application/msgpack")
        return CHECK_ERROR(Runtime::setError("HttpMessage: Invalid content type."));

    return Message::pack(retVal, ac);
}

result_t HttpMessage::send(Stream_base* stm, exlib::string& strCommand,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_sent = true;
    return (new asyncSendTo(this, stm, strCommand, ac))->post(0);
}

result_t HttpMessage::sendHeader(Stream_base* stm, exlib::string& strCommand, bool content_length,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_sent = true;
    return (new asyncSendTo(this, stm, strCommand, ac, true, content_length))->post(0);
}

result_t HttpMessage::readHeader(Stream_base* stm, AsyncEvent* ac)
{
    class asyncReadHeader : public AsyncState {
    public:
        asyncReadHeader(HttpMessage* pThis, BufferedStream_base* stm,
            AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_headCount(0)
        {
            next(begin);
        }

        ON_STATE(asyncReadHeader, begin)
        {
            return m_stm->readLine(m_pThis->m_maxHeaderSize, m_strLine, next(header));
        }

        ON_STATE(asyncReadHeader, header)
        {
            if (m_strLine.length() > 0) {
                if (!qstricmp(m_strLine.c_str(), "content-length:", 15)) {
                    m_pThis->m_contentLength = atoi(m_strLine.c_str() + 15);

                    if (!m_pThis->m_bNoBody
                        && ((m_pThis->m_contentLength < 0)
                            || (m_pThis->m_maxBodySize >= 0
                                && m_pThis->m_contentLength > (int64_t)m_pThis->m_maxBodySize * 1024 * 1024)))
                        return CHECK_ERROR(Runtime::setError("HttpMessage: body is too huge."));

                    {
                        result_t hr = m_pThis->appendHeader(m_strLine);
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

                    m_pThis->m_bChunked = true;

                    result_t hr = m_pThis->appendHeader(m_strLine);
                    if (hr < 0)
                        return hr;

                    m_headCount++;
                } else {
                    result_t hr = m_pThis->appendHeader(m_strLine);
                    if (hr < 0)
                        return hr;

                    m_headCount++;
                }

                if (m_headCount > m_pThis->m_maxHeadersCount)
                    return CHECK_ERROR(Runtime::setError("HttpMessage: too many headers."));

                return m_stm->readLine(m_pThis->m_maxHeaderSize, m_strLine, this);
            }

            return next();
        }

    public:
        HttpMessage* m_pThis;
        obj_ptr<BufferedStream_base> m_stm;
        obj_ptr<SeekableStream_base> m_body;
        exlib::string m_strLine;
        int32_t m_headCount;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_headers->m_lowercase_keys = true;

    obj_ptr<BufferedStream_base> _stm = BufferedStream_base::getInstance(stm);
    if (!_stm)
        return CHECK_ERROR(Runtime::setError("HttpMessage: only accept BufferedStream object."));

    _stm->get_stream(m_socket);
    m_stm = _stm;

    return (new asyncReadHeader(this, _stm, ac))->post(0);
}

result_t HttpMessage::readBody(AsyncEvent* ac)
{
    class asyncReadBody : public AsyncState {
    public:
        asyncReadBody(HttpMessage* pThis, BufferedStream_base* stm,
            AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
        {
            next(begin);
        }

        ON_STATE(asyncReadBody, begin)
        {
            if (!m_pThis->m_bNoBody) {
                if (m_pThis->m_bChunked) {
                    if (m_pThis->m_maxBodySize == 0)
                        return next();

                    if (m_pThis->m_contentLength > 0)
                        return CHECK_ERROR(CALL_E_INVALID_DATA);
                    m_pThis->m_contentLength = 0;

                    if (!m_pThis->m_trailers)
                        m_pThis->m_trailers = new Headers();
                    m_pThis->m_trailers->m_lowercase_keys = true;

                    m_body = m_pThis->ensure_body();
                    m_chunked = new ChunkedStream(m_stm, m_pThis->m_maxChunkSize, m_pThis->m_maxBodySize, m_pThis->m_trailers);
                    return m_chunked->copyTo(m_body, -1, m_copySize, next(body));
                }

                if (m_pThis->m_contentLength > 0 || (m_pThis->m_bResponse && !m_pThis->m_keepAlive && m_pThis->m_contentLength == -1)) {
                    m_body = m_pThis->ensure_body();
                    return m_stm->copyTo(m_body, m_pThis->m_contentLength, m_copySize, next(body));
                }
            }

            return next();
        }

        ON_STATE(asyncReadBody, body)
        {
            if (m_pThis->m_contentLength > 0 && m_pThis->m_contentLength != m_copySize)
                return CHECK_ERROR(Runtime::setError("HttpMessage: body is not complete."));

            m_body->rewind();
            return next();
        }

    public:
        HttpMessage* m_pThis;
        obj_ptr<BufferedStream_base> m_stm;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<Stream_base> m_chunked;
        int64_t m_copySize;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_stm)
        return CHECK_ERROR(Runtime::setError("HttpMessage: stream is not set."));

    return (new asyncReadBody(this, m_stm.As<BufferedStream_base>(), ac))->post(0);
}

result_t HttpMessage::readFrom(Stream_base* stm, AsyncEvent* ac)
{
    class asyncReadFrom : public AsyncState {
    public:
        asyncReadFrom(HttpMessage* pThis, Stream_base* stm, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
        {
            next(begin);
        }

        ON_STATE(asyncReadFrom, begin)
        {
            return m_pThis->readHeader(m_stm, next(read_body));
        }

        ON_STATE(asyncReadFrom, read_body)
        {
            return m_pThis->readBody(next());
        }

    public:
        HttpMessage* m_pThis;
        obj_ptr<Stream_base> m_stm;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncReadFrom(this, stm, ac))->post(0);
}

void HttpMessage::appendHeader(const char* name, int32_t szName, const char* value,
    int32_t szValue)
{
    if (szName == 10 && !qstricmp(name, "connection", szName)) {
        if (qstristr(value, "upgrade")) {
            m_upgrade = true;
            m_keepAlive = true;
        } else
            m_keepAlive = !!qstristr(value, "keep-alive");
    } else
        m_headers->append_string(name, szName, value, szValue);
}

result_t HttpMessage::appendHeader(exlib::string& strLine)
{
    int32_t p2;
    _parser p(strLine);

    p.skipWord(':');
    p2 = p.pos;
    if (0 == p2 || !p.want(':'))
        return CHECK_ERROR(Runtime::setError("HttpMessage: bad header: " + strLine));

    p.skipSpace();
    appendHeader(p.string, p2, p.now(), p.left());

    return 0;
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

size_t HttpMessage::getData(char* buf, size_t sz, bool content_length)
{
    if (!buf) {
        size_t sz = 2 + m_headers->size();
        int64_t l;

        // connection 10
        sz += 10 + 4 + (m_upgrade ? 7 : (m_keepAlive ? 10 : 5));

        if (content_length) {
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
        }

        return sz;
    }

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

    if (content_length) {
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
    }

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

result_t HttpMessage::get_headers(obj_ptr<Headers_base>& retVal)
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
        return CHECK_ERROR(Runtime::setError(CALL_E_OUTRANGE, "maxHeadersCount must be non-negative, received %d.", newVal));

    m_maxHeadersCount = newVal;
    return 0;
}

result_t HttpMessage::get_maxHeaderSize(int32_t& retVal)
{
    retVal = m_maxHeaderSize;
    return 0;
}

result_t HttpMessage::set_maxHeaderSize(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(Runtime::setError(CALL_E_OUTRANGE, "maxHeaderSize must be non-negative, received %d.", newVal));

    m_maxHeaderSize = newVal;
    return 0;
}

result_t HttpMessage::get_maxChunkSize(int32_t& retVal)
{
    retVal = m_maxChunkSize;
    return 0;
}

result_t HttpMessage::set_maxChunkSize(int32_t newVal)
{
    m_maxChunkSize = newVal;
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
    return m_headers->first_string(name, retVal);
}

result_t HttpMessage::allHeader(exlib::string name, obj_ptr<NObject>& retVal)
{
    return m_headers->all(name, retVal);
}

result_t HttpMessage::appendHeader(v8::Local<v8::Object> map)
{
    return m_headers->append(map);
}

result_t HttpMessage::appendHeader(Headers_base* headers)
{
    Headers* hdrs = static_cast<Headers*>(headers);
    for (int32_t i = 0; i < (int32_t)hdrs->m_map.size(); i++) {
        auto& it = hdrs->m_map[i];
        exlib::string value = it.second.string();
        appendHeader(it.first.c_str(), (int32_t)it.first.length(),
            value.c_str(), (int32_t)value.length());
    }
    return 0;
}

result_t HttpMessage::appendHeader(exlib::string name, exlib::string value)
{
    return m_headers->append(name, value);
}

result_t HttpMessage::appendHeader(exlib::string name, v8::Local<v8::Array> values)
{
    return m_headers->append(name, values);
}

result_t HttpMessage::setHeader(v8::Local<v8::Object> map)
{
    return m_headers->set(map);
}

result_t HttpMessage::setHeader(Headers_base* headers)
{
    m_headers->clear();
    return m_headers->init(headers);
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

result_t HttpMessage::getHeader(exlib::string name, v8::Local<v8::Value>& retVal)
{
    bool has;
    result_t hr = m_headers->has(name, has);
    if (hr < 0)
        return hr;

    if (!has)
        return CALL_RETURN_UNDEFINED;

    Variant value;
    hr = m_headers->first(name, value);
    if (hr < 0)
        return hr;

    retVal = value;
    return 0;
}

result_t HttpMessage::getHeaders(obj_ptr<NObject>& retVal)
{
    return m_headers->all("", retVal);
}

result_t HttpMessage::get_headersSent(bool& retVal)
{
    retVal = m_sent;
    return 0;
}

result_t HttpMessage::get_trailers(obj_ptr<Headers_base>& retVal)
{
    if (!m_trailers)
        m_trailers = new Headers();
    retVal = m_trailers;
    return 0;
}

result_t HttpMessage::addTrailers(v8::Local<v8::Object> headers)
{
    if (!m_trailers)
        m_trailers = new Headers();
    return m_trailers->append(headers);
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
    m_headers->m_lowercase_keys = false;

    if (m_trailers)
        m_trailers->clear();

    m_stm.Release();
    m_socket.Release();

    m_contentLength = -1;
    m_bChunked = false;
    m_sent = false;

    return 0;
}

result_t HttpMessage::clone(obj_ptr<Message_base>& retVal)
{
    // HttpMessage is an abstract base class, cannot be cloned directly
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

void HttpMessage::copyTo(HttpMessage* target)
{
    // Copy base Message properties
    Message::copyTo(target);

    // Copy HttpMessage specific properties
    target->m_protocol = m_protocol;
    target->m_keepAlive = m_keepAlive;
    target->m_upgrade = m_upgrade;
    target->m_maxHeadersCount = m_maxHeadersCount;
    target->m_maxHeaderSize = m_maxHeaderSize;
    target->m_maxChunkSize = m_maxChunkSize;
    target->m_maxBodySize = m_maxBodySize;
    target->m_origin = m_origin;
    target->m_encoding = m_encoding;
    target->m_contentLength = m_contentLength;
    target->m_bChunked = m_bChunked;

    // Clone headers by iterating over the map directly
    if (m_headers) {
        target->m_headers = new Headers();
        for (size_t i = 0; i < m_headers->m_map.size(); i++) {
            auto& p = m_headers->m_map[i];
            target->m_headers->append(p.first, p.second.string());
        }
    }

    // Clone trailers
    if (m_trailers) {
        target->m_trailers = new Headers();
        for (size_t i = 0; i < m_trailers->m_map.size(); i++) {
            auto& p = m_trailers->m_map[i];
            target->m_trailers->append(p.first, p.second.string());
        }
    }
}

} /* namespace fibjs */
