/*
 * BufferedStream.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "BufferedStream.h"
#include "Buffer.h"

namespace fibjs {

class asyncBuffer : public AsyncState {
public:
    asyncBuffer(BufferedStream* pThis, AsyncEvent* ac)
        : AsyncState(ac)
        , m_streamEnd(false)
        , m_pThis(pThis)
    {
        set(process);
    }

    virtual result_t process(bool end)
    {
        return CALL_RETURN_NULL;
    }

    static int32_t process(AsyncState* pState, int32_t n)
    {
        asyncBuffer* pThis = (asyncBuffer*)pState;

        result_t hr = pThis->process(pThis->m_streamEnd);
        if (pThis->m_pThis->m_pos == (int32_t)pThis->m_pThis->m_buf.length()) {
            pThis->m_pThis->m_buf.clear();
            pThis->m_pThis->m_pos = 0;
        }

        if (hr != CALL_E_PENDDING)
            return pThis->done(hr);

        pThis->set(ready);
        return pThis->m_pThis->m_stm->read(-1, pThis->m_buf, pThis);
    }

    static int32_t ready(AsyncState* pState, int32_t n)
    {
        asyncBuffer* pThis = (asyncBuffer*)pState;

        pThis->m_pThis->m_buf.clear();
        pThis->m_pThis->m_pos = 0;

        if (n != CALL_RETURN_NULL) {
            pThis->m_buf->toString(pThis->m_pThis->m_buf);
            pThis->m_buf.Release();
        } else
            pThis->m_streamEnd = true;

        return process(pState, n);
    }

public:
    bool m_streamEnd;
    obj_ptr<BufferedStream> m_pThis;
    obj_ptr<Buffer_base> m_buf;
};

result_t BufferedStream_base::_new(Stream_base* stm,
    obj_ptr<BufferedStream_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new BufferedStream(stm);
    return 0;
}

result_t BufferedStream::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    class asyncRead : public asyncBuffer {
    public:
        asyncRead(BufferedStream* pThis, int32_t bytes,
            obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
            : asyncBuffer(pThis, ac)
            , m_bytes(bytes)
            , m_retVal(retVal)
        {
        }

        static result_t process(BufferedStream* pThis, int32_t bytes,
            obj_ptr<Buffer_base>& retVal, bool streamEnd)
        {
            int32_t n = bytes - (int32_t)pThis->m_strbuf.size();
            int32_t n1 = (int32_t)pThis->m_buf.length() - pThis->m_pos;

            if (n > n1)
                n = n1;

            pThis->append(n);

            if (streamEnd || bytes == (int32_t)pThis->m_strbuf.size()) {
                exlib::string s = pThis->m_strbuf.str();

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
        obj_ptr<Buffer_base>& m_retVal;
    };

    if (bytes < 0) {
        int32_t n = (int32_t)m_buf.length() - m_pos;
        if (n > 0) {
            if (m_pos == 0)
                retVal = new Buffer(m_buf);
            else {
                exlib::string s1(&m_buf[m_pos], n);
                retVal = new Buffer(s1);
            }
            m_pos += n;

            return 0;
        } else
            return m_stm->read(bytes, retVal, ac);
    }

    result_t hr = asyncRead::process(this, bytes, retVal, false);
    if (hr != CALL_E_PENDDING)
        return hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRead(this, bytes, retVal, ac))->post(0);
}

result_t BufferedStream::write(Buffer_base* data, AsyncEvent* ac)
{
    return m_stm->write(data, ac);
}

result_t BufferedStream::close(AsyncEvent* ac)
{
    return m_stm->close(ac);
}

result_t BufferedStream::copyTo(Stream_base* stm, int64_t bytes,
    int64_t& retVal, AsyncEvent* ac)
{
    return io_base::copyStream(this, stm, bytes, retVal, ac);
}

result_t BufferedStream::readText(int32_t size, exlib::string& retVal,
    AsyncEvent* ac)
{
    class asyncRead : public asyncBuffer {
    public:
        asyncRead(BufferedStream* pThis, int32_t size,
            exlib::string& retVal, AsyncEvent* ac)
            : asyncBuffer(pThis, ac)
            , m_size(size)
            , m_retVal(retVal)
        {
        }

        static result_t process(BufferedStream* pThis, int32_t size,
            exlib::string& retVal, bool streamEnd)
        {
            int32_t n = size - (int32_t)pThis->m_strbuf.size();
            int32_t n1 = (int32_t)pThis->m_buf.length() - pThis->m_pos;

            if (n > n1)
                n = n1;

            pThis->append(n);

            if (streamEnd || size == (int32_t)pThis->m_strbuf.size()) {
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
        exlib::string& m_retVal;
    };

    result_t hr = asyncRead::process(this, size, retVal, false);
    if (hr != CALL_E_PENDDING)
        return hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRead(this, size, retVal, ac))->post(0);
}

result_t BufferedStream::readLine(int32_t maxlen, exlib::string& retVal,
    AsyncEvent* ac)
{
    return readUntil(m_eol, maxlen, retVal, ac);
}

result_t BufferedStream::readLines(int32_t maxlines, v8::Local<v8::Array>& retVal)
{
    result_t hr = 0;
    exlib::string str;
    int32_t n = 0;
    Isolate* isolate = holder();
    retVal = v8::Array::New(isolate->m_isolate);

    if (maxlines == 0)
        return 0;

    while (true) {
        hr = ac_readLine(-1, str);

        if (hr < 0)
            return hr;
        if (hr > 0)
            return 0;

        retVal->Set(n++, isolate->NewString(str));
        if (maxlines > 0) {
            maxlines--;
            if (maxlines == 0)
                break;
        }
    }

    return 0;
}

result_t BufferedStream::readUntil(exlib::string mk, int32_t maxlen,
    exlib::string& retVal, AsyncEvent* ac)
{
    class asyncRead : public asyncBuffer {
    public:
        asyncRead(BufferedStream* pThis, exlib::string mk, int32_t maxlen,
            exlib::string& retVal, AsyncEvent* ac)
            : asyncBuffer(pThis, ac)
            , m_mk(mk)
            , m_maxlen(maxlen)
            , m_retVal(
                  retVal)
        {
        }

        static result_t process(BufferedStream* pThis, exlib::string mk,
            int32_t maxlen, exlib::string& retVal, bool streamEnd)
        {
            int32_t pos = pThis->m_pos;
            int32_t mklen = (int32_t)mk.length();

            if (mklen == 0)
                mklen = 1;

            while ((pos < (int32_t)pThis->m_buf.length())
                && (pThis->m_temp < mklen)) {
                if (pThis->m_temp == 0) {
                    char ch = mk[0];

                    while (pos < (int32_t)pThis->m_buf.length())
                        if (pThis->m_buf[pos++] == ch) {
                            pThis->m_temp++;
                            break;
                        }
                }

                if (pThis->m_temp > 0) {
                    while ((pos < (int32_t)pThis->m_buf.length())
                        && (pThis->m_temp < mklen)) {
                        if (pThis->m_buf[pos] != mk[pThis->m_temp]) {
                            pThis->m_temp = 0;
                            break;
                        }

                        pos++;
                        pThis->m_temp++;
                    }
                }
            }

            if (maxlen > 0
                && ((int32_t)pThis->m_strbuf.size() + (pos - pThis->m_pos)
                       > maxlen + mklen))
                return CHECK_ERROR(CALL_E_INVALID_DATA);

            pThis->append(pos - pThis->m_pos);

            if (streamEnd || (pThis->m_temp == mklen)) {
                result_t hr = pThis->m_iconv.decode(pThis->m_strbuf.str(), retVal);
                if (hr < 0)
                    return hr;

                if (pThis->m_temp == mklen) {
                    retVal.resize(retVal.length() - pThis->m_temp);
                    pThis->m_temp = 0;
                    return 0;
                }

                pThis->m_temp = 0;
                return streamEnd && (retVal.length() == 0) ? CALL_RETURN_NULL : 0;
            }

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual result_t process(bool streamEnd)
        {
            return process(m_pThis, m_mk, m_maxlen, m_retVal, streamEnd);
        }

    public:
        exlib::string m_mk;
        int32_t m_maxlen;
        exlib::string& m_retVal;
    };

    result_t hr = asyncRead::process(this, mk, maxlen, retVal, false);
    if (hr != CALL_E_PENDDING)
        return hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRead(this, mk, maxlen, retVal, ac))->post(0);
}

result_t BufferedStream::writeText(exlib::string txt, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strBuf;

    result_t hr = m_iconv.encode(txt, strBuf);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> data = new Buffer(strBuf);
    return write(data, ac);
}

result_t BufferedStream::writeLine(exlib::string txt, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strBuf;

    result_t hr = m_iconv.encode(txt, strBuf);
    if (hr < 0)
        return hr;

    strBuf.append(m_eol);
    obj_ptr<Buffer_base> data = new Buffer(strBuf);
    return write(data, ac);
}

result_t BufferedStream::get_stream(obj_ptr<Stream_base>& retVal)
{
    retVal = m_stm;
    return 0;
}

result_t BufferedStream::get_charset(exlib::string& retVal)
{
    retVal = m_iconv.charset();
    return 0;
}

result_t BufferedStream::set_charset(exlib::string newVal)
{
    m_iconv.open(newVal.c_str());
    return 0;
}

result_t BufferedStream::get_EOL(exlib::string& retVal)
{
    retVal = m_eol;
    return 0;
}

result_t BufferedStream::set_EOL(exlib::string newVal)
{
    if (newVal[0] == '\r' && newVal[1] == '\n')
        m_eol.assign(newVal.c_str(), 2);
    else if (newVal[1] == '\0' && (newVal[0] == '\r' || newVal[0] == '\n'))
        m_eol.assign(newVal.c_str(), 1);
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return 0;
}

} /* namespace fibjs */
