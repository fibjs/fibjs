/*
 * BufferedStream.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "BufferedStream.h"
#include "Stream.h"
#include "Buffer.h"
#include "PacketMessage.h"

namespace fibjs
{

class asyncBuffer: public AsyncState
{
public:
    asyncBuffer(BufferedStream *pThis, AsyncEvent *ac) :
        AsyncState(ac), m_streamEnd(false), m_pThis(pThis)
    {
        set(process);
    }

    virtual result_t process(bool end)
    {
        return CALL_RETURN_NULL;
    }

    static int32_t process(AsyncState *pState, int32_t n)
    {
        asyncBuffer *pThis = (asyncBuffer *) pState;

        result_t hr = pThis->process(pThis->m_streamEnd);
        if (hr != CALL_E_PENDDING)
            return pThis->done(hr);

        pThis->set(ready);
        return pThis->m_pThis->m_stm->read(-1, pThis->m_buf, pThis);
    }

    static int32_t ready(AsyncState *pState, int32_t n)
    {
        asyncBuffer *pThis = (asyncBuffer *) pState;

        pThis->m_pThis->m_buf.clear();
        pThis->m_pThis->m_pos = 0;

        if (n != CALL_RETURN_NULL)
        {
            pThis->m_buf->toString(pThis->m_pThis->m_buf);
            pThis->m_buf.Release();
        }
        else
            pThis->m_streamEnd = true;

        return process(pState, n);
    }

public:
    bool m_streamEnd;
    obj_ptr<BufferedStream> m_pThis;
    obj_ptr<Buffer_base> m_buf;
};

result_t BufferedStream_base::_new(Stream_base *stm,
                                   obj_ptr<BufferedStream_base> &retVal,
                                   v8::Local<v8::Object> This)
{
    retVal = new BufferedStream(stm);
    return 0;
}

result_t BufferedStream::read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                              AsyncEvent *ac)
{
    class asyncRead: public asyncBuffer
    {
    public:
        asyncRead(BufferedStream *pThis, int32_t bytes,
                  obj_ptr<Buffer_base> &retVal, AsyncEvent *ac) :
            asyncBuffer(pThis, ac), m_bytes(bytes), m_retVal(retVal)
        {
        }

        static result_t process(BufferedStream *pThis, int32_t bytes,
                                obj_ptr<Buffer_base> &retVal, bool streamEnd)
        {
            int32_t n = bytes - (int32_t) pThis->m_strbuf.size();
            int32_t n1 = (int32_t) pThis->m_buf.length() - pThis->m_pos;

            if (n > n1)
                n = n1;

            pThis->append(n);

            if (streamEnd || bytes == (int32_t) pThis->m_strbuf.size())
            {
                std::string s = pThis->m_strbuf.str();

                if (s.length() == 0)
                    return CALL_RETURN_NULL;

                retVal = new Buffer(s);

                return 0;
            }

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual result_t process(bool streamEnd)
        {
            return process(m_pThis, m_bytes, m_retVal, streamEnd);
        }

    public:
        int32_t m_bytes;
        obj_ptr<Buffer_base> &m_retVal;
    };

    if (bytes < 0)
    {
        int32_t n = (int32_t) m_buf.length() - m_pos;
        if (n > 0)
        {
            if (m_pos == 0)
                retVal = new Buffer(m_buf);
            else
            {
                std::string s1(&m_buf[m_pos], n);
                retVal = new Buffer(s1);
            }
            m_pos += n;

            return 0;
        }
        else
            return m_stm->read(bytes, retVal, ac);
    }

    result_t hr = asyncRead::process(this, bytes, retVal, false);
    if (hr != CALL_E_PENDDING)
        return hr;

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRead(this, bytes, retVal, ac))->post(0);
}

result_t BufferedStream::write(Buffer_base *data, AsyncEvent *ac)
{
    return m_stm->write(data, ac);
}

result_t BufferedStream::close(AsyncEvent *ac)
{
    return m_stm->close(ac);
}

result_t BufferedStream::copyTo(Stream_base *stm, int64_t bytes,
                                int64_t &retVal, AsyncEvent *ac)
{
    return copyStream(this, stm, bytes, retVal, ac);
}

result_t BufferedStream::readText(int32_t size, std::string &retVal,
                                  AsyncEvent *ac)
{
    class asyncRead: public asyncBuffer
    {
    public:
        asyncRead(BufferedStream *pThis, int32_t size,
                  std::string &retVal, AsyncEvent *ac) :
            asyncBuffer(pThis, ac), m_size(size), m_retVal(retVal)
        {
        }

        static result_t process(BufferedStream *pThis, int32_t size,
                                std::string &retVal, bool streamEnd)
        {
            int32_t n = size - (int32_t) pThis->m_strbuf.size();
            int32_t n1 = (int32_t) pThis->m_buf.length() - pThis->m_pos;

            if (n > n1)
                n = n1;

            pThis->append(n);

            if (streamEnd || size == (int32_t) pThis->m_strbuf.size())
            {
                result_t hr = pThis->m_iconv.decode(pThis->m_strbuf.str(), retVal);
                if (hr < 0)
                    return hr;

                if (retVal.length() == 0)
                    return CALL_RETURN_NULL;

                return 0;
            }

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual result_t process(bool streamEnd)
        {
            return process(m_pThis, m_size, m_retVal, streamEnd);
        }

    public:
        int32_t m_size;
        std::string &m_retVal;
    };

    result_t hr = asyncRead::process(this, size, retVal, false);
    if (hr != CALL_E_PENDDING)
        return hr;

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRead(this, size, retVal, ac))->post(0);
}

result_t BufferedStream::readLine(int32_t maxlen, std::string &retVal,
                                  AsyncEvent *ac)
{
    return readUntil(m_eol.c_str(), maxlen, retVal, ac);
}

result_t BufferedStream::readLines(int32_t maxlines, v8::Local<v8::Array> &retVal)
{
    result_t hr = 0;
    std::string str;
    int32_t n = 0;
    Isolate &isolate = Isolate::now();
    retVal = v8::Array::New(isolate.isolate);

    if (maxlines == 0)
        return 0;

    while (true)
    {
        hr = ac_readLine(-1, str);

        if (hr < 0)
            return hr;
        if (hr > 0)
            return 0;

        retVal->Set(n ++, v8::String::NewFromUtf8(isolate.isolate, str.c_str(),
                    v8::String::kNormalString, (int32_t)str.length()));
        if (maxlines > 0)
        {
            maxlines --;
            if (maxlines == 0)
                break;
        }
    }

    return 0;
}

result_t BufferedStream::readUntil(const char *mk, int32_t maxlen,
                                   std::string &retVal, AsyncEvent *ac)
{
    class asyncRead: public asyncBuffer
    {
    public:
        asyncRead(BufferedStream *pThis, const char *mk, int32_t maxlen,
                  std::string &retVal, AsyncEvent *ac) :
            asyncBuffer(pThis, ac), m_mk(mk), m_maxlen(maxlen), m_retVal(
                retVal)
        {
        }

        static result_t process(BufferedStream *pThis, const char *mk,
                                int32_t maxlen, std::string &retVal, bool streamEnd)
        {
            int32_t pos = pThis->m_pos;
            int32_t mklen = (int32_t) qstrlen(mk);

            if (mklen == 0)
                mklen = 1;

            while ((pos < (int32_t) pThis->m_buf.length())
                    && (pThis->m_temp < mklen))
            {
                if (pThis->m_temp == 0)
                {
                    char ch = mk[0];

                    while (pos < (int32_t) pThis->m_buf.length())
                        if (pThis->m_buf[pos++] == ch)
                        {
                            pThis->m_temp++;
                            break;
                        }
                }

                if (pThis->m_temp > 0)
                {
                    while ((pos < (int32_t) pThis->m_buf.length())
                            && (pThis->m_temp < mklen))
                    {
                        if (pThis->m_buf[pos] != mk[pThis->m_temp])
                        {
                            pThis->m_temp = 0;
                            break;
                        }

                        pos++;
                        pThis->m_temp++;
                    }

                }
            }

            if (maxlen > 0
                    && ((int32_t) pThis->m_strbuf.size() + (pos - pThis->m_pos)
                        > maxlen + mklen))
                return CHECK_ERROR(CALL_E_INVALID_DATA);

            pThis->append(pos - pThis->m_pos);

            if (streamEnd || (pThis->m_temp == mklen))
            {
                result_t hr = pThis->m_iconv.decode(pThis->m_strbuf.str(), retVal);
                if (hr < 0)
                    return hr;

                if (pThis->m_temp == mklen)
                {
                    retVal.resize(retVal.length() - pThis->m_temp);
                    pThis->m_temp = 0;
                    return 0;
                }

                pThis->m_temp = 0;
                return streamEnd && (retVal.length() == 0) ?
                       CALL_RETURN_NULL : 0;
            }

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual result_t process(bool streamEnd)
        {
            return process(m_pThis, m_mk, m_maxlen, m_retVal, streamEnd);
        }

    public:
        const char *m_mk;
        int32_t m_maxlen;
        std::string &m_retVal;
    };

    result_t hr = asyncRead::process(this, mk, maxlen, retVal, false);
    if (hr != CALL_E_PENDDING)
        return hr;

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRead(this, mk, maxlen, retVal, ac))->post(0);
}

result_t BufferedStream::readPacket(int32_t limit, obj_ptr<Buffer_base> &retVal,
                                    AsyncEvent *ac)
{
    class asyncReadPacket: public asyncBuffer
    {
    public:
        asyncReadPacket(BufferedStream *pThis, int32_t limit,
                        obj_ptr<Buffer_base> &retVal, AsyncEvent *ac) :
            asyncBuffer(pThis, ac), m_limit(limit), m_retVal(retVal)
        {
        }

        static result_t process(BufferedStream *pThis, int32_t limit,
                                obj_ptr<Buffer_base> &retVal, bool streamEnd)
        {
            int32_t n1 = (int32_t) pThis->m_buf.length() - pThis->m_pos;

            if (pThis->m_temp == 0)
            {
                int32_t n2 = (int32_t) pThis->m_strbuf.size();
                int32_t n3 = 0;

                while (n3 < n1 && ((unsigned char)pThis->m_buf[pThis->m_pos + n3] & 0x80))
                    n3 ++;

                if (n2 + n3 > (int32_t)sizeof(int32_t))
                    return CHECK_ERROR(CALL_E_INVALID_DATA);

                if (n3 == n1)
                {
                    if (streamEnd)
                    {
                        pThis->m_temp = 0;
                        return CALL_RETURN_NULL;
                    }

                    pThis->append(n3);
                    return CHECK_ERROR(CALL_E_PENDDING);
                }

                n3 ++;

                unsigned char buf[5];

                if (n2 > 0)
                    memcpy(&buf, pThis->m_strbuf.str().c_str(), n2);

                memcpy((char *)&buf + n2, pThis->m_buf.c_str() + pThis->m_pos, n3);
                pThis->m_pos += n3;
                n1 -= n3;

                n2 += n3;
                n3 = 0;
                while (n3 < n2)
                {
                    pThis->m_temp = (pThis->m_temp << 7) | (buf[n3] & 0x7f);
                    n3 ++;
                }
            }

            if (pThis->m_temp <= 0)
            {
                retVal = new Buffer();
                return 0;
            }

            if (limit > 0 && pThis->m_temp > limit)
            {
                pThis->m_temp = 0;
                return CHECK_ERROR(CALL_E_INVALID_DATA);
            }

            int32_t bytes = pThis->m_temp;
            int32_t n = bytes - (int32_t) pThis->m_strbuf.size();

            if (n > n1)
                n = n1;

            pThis->append(n);

            if (bytes == (int32_t) pThis->m_strbuf.size())
            {
                std::string s = pThis->m_strbuf.str();
                retVal = new Buffer(s);
                pThis->m_temp = 0;
                return 0;
            }

            if (streamEnd)
            {
                pThis->m_temp = 0;
                return CALL_RETURN_NULL;
            }

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual result_t process(bool streamEnd)
        {
            return process(m_pThis, m_limit, m_retVal, streamEnd);
        }

    public:
        int32_t m_limit;
        obj_ptr<Buffer_base> &m_retVal;
    };

    result_t hr = asyncReadPacket::process(this, limit, retVal, false);
    if (hr != CALL_E_PENDDING)
        return hr;

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncReadPacket(this, limit, retVal, ac))->post(0);
}

result_t BufferedStream::writeText(const char *txt, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    std::string strBuf;

    result_t hr = m_iconv.encode(txt, strBuf);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> data = new Buffer(strBuf);
    return write(data, ac);
}

result_t BufferedStream::writeLine(const char *txt, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    std::string strBuf;

    result_t hr = m_iconv.encode(txt, strBuf);
    if (hr < 0)
        return hr;

    strBuf.append(m_eol);
    obj_ptr<Buffer_base> data = new Buffer(strBuf);
    return write(data, ac);
}

result_t BufferedStream::writePacket(Buffer_base *data, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    std::string strBuf;
    std::string strData;
    int32_t len;

    data->toString(strData);
    len = (int32_t)strData.length();

    PacketMessage::packetSize(len, strBuf);
    strBuf.append(strData);

    obj_ptr<Buffer_base> data1 = new Buffer(strBuf);
    return write(data1, ac);
}

result_t BufferedStream::get_stream(obj_ptr<Stream_base> &retVal)
{
    retVal = m_stm;
    return 0;
}

result_t BufferedStream::get_charset(std::string &retVal)
{
    retVal = m_iconv.charset();
    return 0;
}

result_t BufferedStream::set_charset(const char *newVal)
{
    m_iconv.open(newVal);
    return 0;
}

result_t BufferedStream::get_EOL(std::string &retVal)
{
    retVal = m_eol;
    return 0;
}

result_t BufferedStream::set_EOL(const char *newVal)
{
    if (newVal[0] == '\r' && newVal[1] == '\n')
        m_eol.assign(newVal, 2);
    else if (newVal[1] == '\0' && (newVal[0] == '\r' || newVal[0] == '\n'))
        m_eol.assign(newVal, 1);
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return 0;
}

} /* namespace fibjs */
