/**
 * @author gmxyb
 * @email gmxyb@163.com
 * @create date 2020-08-24 17:49:40
 * @modify date 2020-08-24 17:49:40
 * @desc File Range R/W strem
 */

#include "object.h"
#include "RangeStream.h"
#include "Buffer.h"
#include "ifs/SeekableStream.h"

namespace fibjs {

// follow HTTP 206 Range Request RFC, but should cut off `bytes=` token firstly
// @see https://tools.ietf.org/html/rfc7233
result_t _parseRange(exlib::string range, int64_t fsize, int64_t& bpos, int64_t& epos)
{
    int32_t p;
    p = (int32_t)range.find(',', 0);
    if (p >= 0)
        return Runtime::setError("Parse Range-String Error, unexpected token ','!");
    p = (int32_t)range.find('-', 0);
    if (p < 0)
        return Runtime::setError("Parse Range-String Error, expect next token '-'!");

    exlib::string b = range.substr(0, p);
    exlib::string e = range.substr(p + 1);

    if (b.empty() && e.empty())
        return Runtime::setError("Parse Range-String Error, first-byte-pos required!");
    if (!b.empty() && e.empty()) {
        bpos = atoll(b.c_str());
        epos = fsize;
    }
    if (b.empty() && !e.empty()) {
        int64_t t = atoll(e.c_str());
        bpos = fsize - t;
        epos = fsize;
    }
    if (!b.empty() && !e.empty()) {
        bpos = atoll(b.c_str());
        epos = atoll(e.c_str()) + 1;
    }

    if (bpos < 0 || bpos > epos || epos > fsize)
        return Runtime::setError("Parse Range-String Error, invalid range string '" + range + "' which is out of file size range!");

    return 0;
}

result_t RangeStream_base::_new(SeekableStream_base* stm, exlib::string range, obj_ptr<RangeStream_base>& retVal, v8::Local<v8::Object> This)
{
    int64_t sz, begin, end;
    stm->size(sz);

    result_t hr = _parseRange(range, sz, begin, end);
    if (hr != 0)
        return hr;

    retVal = new RangeStream(stm, begin, end);

    return 0;
}

result_t RangeStream_base::_new(SeekableStream_base* stm, int64_t begin, int64_t end, obj_ptr<RangeStream_base>& retVal, v8::Local<v8::Object> This)
{
    if (begin < 0)
        return Runtime::setError("'begin' must be non-negative integer!");

    int64_t sz;
    stm->size(sz);

    if (begin > sz)
        return Runtime::setError("'begin' cannot be greater than stream’s size");

    retVal = new RangeStream(stm, begin, end);
    return 0;
}
result_t RangeStream_base::_new(Stream_base* stm, int64_t end, obj_ptr<RangeStream_base>& retVal, v8::Local<v8::Object> This)
{
    if (end < 0)
        return Runtime::setError("'end' must be non-negative integer!");

    // if the passed stream is a SeekableStream, use seekable mode with begin=0
    obj_ptr<SeekableStream_base> seekable = SeekableStream_base::getInstance(stm);
    if (seekable) {
        retVal = new RangeStream(seekable, 0, end);
    } else {
        retVal = new RangeStream(stm, end);
    }

    return 0;
}
RangeStream::RangeStream(SeekableStream_base* stream, int64_t begin, int64_t end)
    : m_seekable(true)
{
    m_stream = stream;
    b_pos = begin;
    e_pos = end;

    real_pos = get_c_pos();
}

RangeStream::RangeStream(Stream_base* stream, int64_t end)
    : m_seekable(false)
{
    m_raw_stream = stream;
    b_pos = 0;
    e_pos = end;
    real_pos = 0;
}

result_t RangeStream::get_fd(int32_t& retVal)
{
    if (m_seekable) {
        if (!m_stream)
            return CALL_E_CLOSED;
        return m_stream->get_fd(retVal);
    } else {
        if (!m_raw_stream)
            return CALL_E_CLOSED;
        return m_raw_stream->get_fd(retVal);
    }
}

result_t RangeStream::readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (m_seekable) {
        if (!m_stream)
            return CALL_E_CLOSED;

        if (e_pos < real_pos || b_pos > real_pos)
            return CALL_RETURN_NULL;

        class asyncRead : public AsyncState {
        public:
            asyncRead(RangeStream* pThis, AsyncEvent* ac, int32_t bytes, obj_ptr<Buffer_base>& retVal)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_bytes(bytes)
                , m_retVal(retVal)
            {
                m_c_pos_snap = m_pThis->get_c_pos();

                if (m_c_pos_snap != m_pThis->real_pos)
                    m_pThis->m_stream->seek(m_pThis->real_pos, fs_base::C_SEEK_SET);

                next(read);
            }

        public:
            ON_STATE(asyncRead, read)
            {
                int64_t rest_sz = m_pThis->e_pos - m_pThis->real_pos;

                if (m_bytes < 0 || m_bytes > rest_sz)
                    return m_pThis->m_stream->readBuffer((int32_t)rest_sz, m_retVal, next(ready));
                else
                    return m_pThis->m_stream->readBuffer(m_bytes, m_retVal, next(ready));
            }

            ON_STATE(asyncRead, ready)
            {
                if (n >= 0 && n != CALL_RETURN_NULL) {
                    int32_t len = Buffer::Cast(m_retVal)->length();
                    m_pThis->real_pos += len;
                }

                result_t hr = m_pThis->m_stream->seek(m_c_pos_snap, fs_base::C_SEEK_SET);

                if (n != CALL_RETURN_NULL || hr < 0)
                    return next(hr);
                else
                    return next(n);
            }

        private:
            obj_ptr<RangeStream> m_pThis;
            int32_t m_bytes;
            obj_ptr<Buffer_base>& m_retVal;

            int64_t m_c_pos_snap;
        };

        if (ac->isSync())
            return CALL_E_NOSYNC;

        return (new asyncRead(this, ac, bytes, retVal))->post(0);
    } else {
        if (!m_raw_stream)
            return CALL_E_CLOSED;

        int64_t rest_sz = e_pos - real_pos;
        if (rest_sz <= 0)
            return CALL_RETURN_NULL;

        class asyncReadRaw : public AsyncState {
        public:
            asyncReadRaw(RangeStream* pThis, AsyncEvent* ac, int32_t bytes, obj_ptr<Buffer_base>& retVal)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_bytes(bytes)
                , m_retVal(retVal)
            {
                next(read);
            }

        public:
            ON_STATE(asyncReadRaw, read)
            {
                int64_t rest_sz = m_pThis->e_pos - m_pThis->real_pos;
                if (rest_sz <= 0)
                    return next(CALL_RETURN_NULL);

                int32_t to_read = (m_bytes < 0 || m_bytes > rest_sz) ? (int32_t)rest_sz : m_bytes;
                return m_pThis->m_raw_stream->readBuffer(to_read, m_retVal, next(ready));
            }

            ON_STATE(asyncReadRaw, ready)
            {
                if (n >= 0 && n != CALL_RETURN_NULL) {
                    int32_t len = Buffer::Cast(m_retVal)->length();
                    m_pThis->real_pos += len;
                }
                return next(n);
            }

        private:
            obj_ptr<RangeStream> m_pThis;
            int32_t m_bytes;
            obj_ptr<Buffer_base>& m_retVal;
        };

        if (ac->isSync())
            return CALL_E_NOSYNC;

        return (new asyncReadRaw(this, ac, bytes, retVal))->post(0);
    }
}

int64_t RangeStream::get_c_pos()
{
    if (!m_seekable)
        return real_pos;

    int64_t pos;
    m_stream->tell(pos);
    return pos;
}

int64_t RangeStream::valid_end()
{
    if (!m_seekable)
        return e_pos;

    int64_t sz;
    m_stream->size(sz);

    return sz > e_pos ? e_pos : sz;
}

int64_t RangeStream::valid_start()
{
    return b_pos > 0 ? b_pos : 0;
}

result_t RangeStream::write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    return CALL_E_INVALID_CALL;
}

result_t RangeStream::flush(AsyncEvent* ac)
{
    return 0;
}

result_t RangeStream::close(AsyncEvent* ac)
{
    if (m_seekable)
        m_stream = NULL;
    else
        m_raw_stream = NULL;

    return 0;
}

result_t RangeStream::seek(int64_t offset, int32_t whence)
{
    if (!m_seekable)
        return CALL_E_INVALID_CALL;

    switch (whence) {
    case fs_base::C_SEEK_SET:
        offset += b_pos;
        break;
    case fs_base::C_SEEK_CUR:
        offset += real_pos;
        break;
    case fs_base::C_SEEK_END:
        offset += e_pos;
        break;
    default:
        return CALL_E_INVALIDARG;
    }

    if (offset < b_pos || offset > e_pos)
        return CALL_E_OUTRANGE;

    real_pos = offset;

    return 0;
}

result_t RangeStream::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (!m_seekable)
        return CALL_E_INVALID_CALL;

    if (!m_stream)
        return CALL_E_CLOSED;

    if (ac->isSync())
        return CALL_E_NOSYNC;

    class asyncStat : public AsyncState {
    public:
        asyncStat(RangeStream* pThis, AsyncEvent* ac, obj_ptr<Stat_base>& retVal)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_retVal(retVal)
        {
            next(stat);
        }

    public:
        ON_STATE(asyncStat, stat)
        {
            return m_pThis->m_stream->stat(m_retVal, next(stat_ok));
        }

        ON_STATE(asyncStat, stat_ok)
        {
            obj_ptr<Stat> st = new Stat();
            st->init(m_retVal);
            m_pThis->size(st->size);

            m_retVal = st;

            return next();
        }

    private:
        obj_ptr<RangeStream> m_pThis;
        obj_ptr<Stat_base>& m_retVal;
    };

    return (new asyncStat(this, ac, retVal))->post(0);
}

result_t RangeStream::tell(int64_t& retVal)
{
    if (m_seekable) {
        if (!m_stream)
            return CALL_E_CLOSED;
    } else {
        if (!m_raw_stream)
            return CALL_E_CLOSED;
    }

    retVal = real_pos - b_pos;

    return 0;
}

result_t RangeStream::rewind()
{
    if (!m_seekable)
        return CALL_E_INVALID_CALL;

    return seek(0, fs_base::C_SEEK_SET);
}

result_t RangeStream::size(int64_t& retVal)
{
    if (m_seekable) {
        if (!m_stream)
            return CALL_E_CLOSED;
        retVal = valid_end() - valid_start();
    } else {
        if (!m_raw_stream)
            return CALL_E_CLOSED;
        retVal = e_pos;
    }

    return 0;
}

result_t RangeStream::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return readBuffer(-1, retVal, ac);
}

result_t RangeStream::truncate(int64_t bytes, AsyncEvent* ac)
{
    return CALL_E_INVALID_CALL;
}

result_t RangeStream::eof(bool& retVal)
{
    if (m_seekable) {
        if (!m_stream)
            return CALL_E_CLOSED;
        retVal = get_c_pos() >= valid_end();
    } else {
        if (!m_raw_stream)
            return CALL_E_CLOSED;
        retVal = real_pos >= e_pos;
    }

    return 0;
}

result_t RangeStream::get_begin(int64_t& retVal)
{
    retVal = b_pos;
    return 0;
}

result_t RangeStream::get_end(int64_t& retVal)
{
    retVal = e_pos;
    return 0;
}
}
