/*
 * BufferedStream.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "BufferedStream.h"
#include "Stream.h"
#include "Buffer.h"

namespace fibjs
{

class asyncBuffer: public asyncState
{
public:
    asyncBuffer(BufferedStream *pThis, exlib::AsyncEvent *ac) :
        asyncState(ac), m_streamEnd(false), m_pThis(pThis)
    {
        set(process);
    }

    virtual result_t process(bool end)
    {
        return CALL_RETURN_NULL;
    }

    static int process(asyncState *pState, int n)
    {
        asyncBuffer *pThis = (asyncBuffer *) pState;

        result_t hr = pThis->process(pThis->m_streamEnd);
        if (hr != CALL_E_PENDDING)
            return pThis->done(hr);

        pThis->set(ready);
        return pThis->m_pThis->m_stm->read(-1, pThis->m_buf, pThis);
    }

    static int ready(asyncState *pState, int n)
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
                                   obj_ptr<BufferedStream_base> &retVal)
{
    retVal = new BufferedStream(stm);
    return 0;
}

result_t BufferedStream::read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                              exlib::AsyncEvent *ac)
{
    class asyncRead: public asyncBuffer
    {
    public:
        asyncRead(BufferedStream *pThis, int32_t bytes,
                  obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac) :
            asyncBuffer(pThis, ac), m_bytes(bytes), m_retVal(retVal)
        {
        }

        static result_t process(BufferedStream *pThis, int32_t bytes,
                                obj_ptr<Buffer_base> &retVal, bool streamEnd)
        {
            int n = bytes - (int) pThis->m_strbuf.size();
            int n1 = (int) pThis->m_buf.length() - pThis->m_pos;

            if (n > n1)
                n = n1;

            pThis->append(n);

            if (streamEnd || bytes == (int) pThis->m_strbuf.size())
            {
                std::string s = pThis->m_strbuf.str();

                if (s.length() == 0)
                    return CALL_RETURN_NULL;

                retVal = new Buffer(s);

                return 0;
            }

            return CALL_E_PENDDING;
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
        int n = (int) m_buf.length() - m_pos;
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
        return CALL_E_NOSYNC;

    return (new asyncRead(this, bytes, retVal, ac))->post(0);
}

result_t BufferedStream::write(Buffer_base *data, exlib::AsyncEvent *ac)
{
    return m_stm->write(data, ac);
}

result_t BufferedStream::close(exlib::AsyncEvent *ac)
{
    return m_stm->close(ac);
}

result_t BufferedStream::copyTo(Stream_base *stm, int64_t bytes,
                                int64_t &retVal, exlib::AsyncEvent *ac)
{
    return copyStream(this, stm, bytes, retVal, ac);
}

result_t BufferedStream::readText(int32_t size, std::string &retVal,
                                  exlib::AsyncEvent *ac)
{
    class asyncRead: public asyncBuffer
    {
    public:
        asyncRead(BufferedStream *pThis, int32_t size,
                  std::string &retVal, exlib::AsyncEvent *ac) :
            asyncBuffer(pThis, ac), m_size(size), m_retVal(retVal)
        {
        }

        static result_t process(BufferedStream *pThis, int32_t size,
                                std::string &retVal, bool streamEnd)
        {
            int n = size - (int) pThis->m_strbuf.size();
            int n1 = (int) pThis->m_buf.length() - pThis->m_pos;

            if (n > n1)
                n = n1;

            pThis->append(n);

            if (streamEnd || size == (int) pThis->m_strbuf.size())
            {
                retVal = pThis->m_strbuf.str();

                if (retVal.length() == 0)
                    return CALL_RETURN_NULL;

                return 0;
            }

            return CALL_E_PENDDING;
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
        return CALL_E_NOSYNC;

    return (new asyncRead(this, size, retVal, ac))->post(0);
}

result_t BufferedStream::readLine(int32_t maxlen, std::string &retVal,
                                  exlib::AsyncEvent *ac)
{
    return readUntil(m_eol.c_str(), maxlen, retVal, ac);
}

result_t BufferedStream::readLines(int32_t maxlines, v8::Handle<v8::Array> &retVal)
{
    result_t hr = 0;
    std::string str;
    int32_t n = 0;
    retVal = v8::Array::New();

    if (maxlines == 0)
        return 0;

    while (true)
    {
        hr = ac_readLine(-1, str);

        if (hr < 0)
            return hr;
        if (hr > 0)
            return 0;

        retVal->Set(n ++, v8::String::New(str.c_str(), (int)str.length()));
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
                                   std::string &retVal, exlib::AsyncEvent *ac)
{
    class asyncRead: public asyncBuffer
    {
    public:
        asyncRead(BufferedStream *pThis, const char *mk, int32_t maxlen,
                  std::string &retVal, exlib::AsyncEvent *ac) :
            asyncBuffer(pThis, ac), m_mk(mk), m_maxlen(maxlen), m_retVal(
                retVal)
        {
        }

        static result_t process(BufferedStream *pThis, const char *mk,
                                int32_t maxlen, std::string &retVal, bool streamEnd)
        {
            int pos = pThis->m_pos;
            int mklen = (int) qstrlen(mk);

            if (mklen == 0)
                mklen = 1;

            while ((pos < (int) pThis->m_buf.length())
                    && (pThis->m_temp < mklen))
            {
                if (pThis->m_temp == 0)
                {
                    char ch = mk[0];

                    while (pos < (int) pThis->m_buf.length())
                        if (pThis->m_buf[pos++] == ch)
                        {
                            pThis->m_temp++;
                            break;
                        }
                }

                if (pThis->m_temp > 0)
                {
                    char ch;

                    while ((pos < (int) pThis->m_buf.length())
                            && (pThis->m_temp < mklen))
                    {
                        ch = mk[pThis->m_temp];

                        if (pThis->m_buf[pos] != ch)
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
                    && ((int) pThis->m_strbuf.size() + (pos - pThis->m_pos)
                        > maxlen + mklen))
                return CALL_E_INVALID_DATA;

            pThis->append(pos - pThis->m_pos);

            if (streamEnd || (pThis->m_temp >= mklen))
            {
                retVal = pThis->m_strbuf.str();

                if (pThis->m_temp)
                {
                    retVal.resize(retVal.length() - pThis->m_temp);
                    pThis->m_temp = 0;
                    return 0;
                }

                pThis->m_temp = 0;
                return streamEnd && (retVal.length() == 0) ?
                       CALL_RETURN_NULL : 0;
            }

            return CALL_E_PENDDING;
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
        return CALL_E_NOSYNC;

    return (new asyncRead(this, mk, maxlen, retVal, ac))->post(0);
}

result_t BufferedStream::readPacket(int32_t limit, obj_ptr<Buffer_base> &retVal,
                                    exlib::AsyncEvent *ac)
{
    class asyncReadPacket: public asyncBuffer
    {
    public:
        asyncReadPacket(BufferedStream *pThis, int32_t limit,
                        obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac) :
            asyncBuffer(pThis, ac), m_limit(limit), m_retVal(retVal)
        {
        }

        static result_t process(BufferedStream *pThis, int32_t limit,
                                obj_ptr<Buffer_base> &retVal, bool streamEnd)
        {
            int n1 = (int) pThis->m_buf.length() - pThis->m_pos;

            if (pThis->m_temp == 0)
            {
                int n2 = (int) pThis->m_strbuf.size();

                if (n1 + n2 < sizeof(int32_t))
                {
                    if (streamEnd)
                    {
                        pThis->m_temp = 0;
                        return CALL_RETURN_NULL;
                    }

                    pThis->append(n1);
                    return CALL_E_PENDDING;
                }

                if (n2 > 0)
                {
                    std::string s = pThis->m_strbuf.str();
                    memcpy(&pThis->m_temp, &s[0], n2);
                }

                memcpy((char *)&pThis->m_temp + n2, &pThis->m_buf[pThis->m_pos],
                       sizeof(int32_t) - n2);
                pThis->m_pos += sizeof(int32_t) - n2;
                n1 -= sizeof(int32_t) - n2;
            }

            if (pThis->m_temp <= 0)
            {
                retVal = new Buffer();
                return 0;
            }

            if (limit > 0 && pThis->m_temp > limit)
                return CALL_E_INVALID_DATA;

            int bytes = pThis->m_temp;
            int n = bytes - (int) pThis->m_strbuf.size();

            if (n > n1)
                n = n1;

            pThis->append(n);

            if (bytes == (int) pThis->m_strbuf.size())
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

            return CALL_E_PENDDING;
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
        return CALL_E_NOSYNC;

    return (new asyncReadPacket(this, limit, retVal, ac))->post(0);
}

result_t BufferedStream::writeText(const char *txt, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    std::string strBuf = txt;
    obj_ptr<Buffer_base> data = new Buffer(strBuf);
    return write(data, ac);
}

result_t BufferedStream::writeLine(const char *txt, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    std::string strBuf = txt;
    strBuf.append(m_eol);
    obj_ptr<Buffer_base> data = new Buffer(strBuf);
    return write(data, ac);
}

result_t BufferedStream::writePacket(Buffer_base *data, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    std::string strBuf;
    std::string strData;
    int32_t len;

    data->toString(strData);
    len = (int32_t)strData.length();

    strBuf.append((char *)&len, sizeof(len));
    strBuf.append(strData);

    obj_ptr<Buffer_base> data1 = new Buffer(strBuf);
    return write(data1, ac);
}

result_t BufferedStream::get_stream(obj_ptr<Stream_base> &retVal)
{
    retVal = m_stm;
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
        return CALL_E_INVALIDARG;

    return 0;
}

} /* namespace fibjs */
