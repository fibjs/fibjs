/**
 * @author gmxyb
 * @email gmxyb@163.com
 * @create date 2020-08-24 17:49:40
 * @modify date 2020-08-24 17:49:40
 * @desc File Range R/W strem
 */
#include "ifs/fs.h"
#include "ifs/io.h"
#include "ifs/RangeStream.h"
#include "Stat.h"

#ifndef IORANGESTREAM_H_
#define IORANGESTREAM_H_

namespace fibjs {

static int32_t _parseRange(exlib::string range, int64_t fsize, int64_t& bpos, int64_t& epos)
{
    exlib::string r = range;
    if (0 != qstricmp(r.c_str(), "bytes=", 6))
        return -1;
    r = r.substr(6);

    int32_t p;
    p = (int32_t)r.find(',', 0);
    if (p >= 0)
        return -2;
    p = (int32_t)r.find('-', 0);
    if (p < 0)
        return -3;

    exlib::string b = r.substr(0, p);
    exlib::string e = r.substr(p + 1);

    if (b.empty() && e.empty())
        return -4;
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
        return -5;

    return 0;
}

class RangeStream : public RangeStream_base {
public:
    RangeStream(obj_ptr<SeekableStream_base> stream, int64_t begin, int64_t end)
    {
        m_stream = stream;
        b_pos = begin;
        e_pos = end;

        rewind();
    }

private:
    int64_t get_c_pos()
    {
        int64_t pos;
        m_stream->tell(pos);
        return pos;
    }

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        int64_t c_pos = get_c_pos();
        if (e_pos < c_pos || b_pos > c_pos)
            return CALL_RETURN_NULL;

        int64_t rest_sz = e_pos - c_pos;

        if (bytes < 0 || bytes > rest_sz)
            return m_stream->read(rest_sz, retVal, ac);

        return m_stream->read(bytes, retVal, ac);
    }
    virtual result_t write(Buffer_base* data, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }
    virtual result_t flush(AsyncEvent* ac)
    {
        return 0;
    }
    virtual result_t close(AsyncEvent* ac)
    {
        return m_stream->close(ac);
    }
    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
    {
        return io_base::copyStream(this, stm, bytes, retVal, ac);
    }

public:
    // SeekableStream_base
    virtual result_t seek(int64_t offset, int32_t whence = fs_base::_SEEK_SET)
    {
        switch (whence) {
        case fs_base::_SEEK_SET:
            offset += b_pos;
            break;
        case fs_base::_SEEK_END:
            offset += e_pos;
            break;
        default:
            return CALL_E_INVALIDARG;
        }

        return m_stream->seek(offset, whence);
    }
    virtual result_t tell(int64_t& retVal)
    {
        retVal = get_c_pos() - b_pos;

        return 0;
    }
    virtual result_t rewind()
    {
        return seek(0, fs_base::_SEEK_SET);
    }
    virtual result_t size(int64_t& retVal)
    {
        retVal = e_pos - b_pos;

        return 0;
    }
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return this->read(-1, retVal, ac);
    }
    virtual result_t truncate(int64_t bytes, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }
    virtual result_t eof(bool& retVal)
    {
        retVal = get_c_pos() >= e_pos;

        return 0;
    }
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
    {
        obj_ptr<Stat> st = new Stat();
        st->init();
        st->m_isFile = true;
        st->size = e_pos - b_pos;
        retVal = st;

        return 0;
    }

public:
    // RangeStream_base
    virtual result_t get_begin(int32_t& retVal)
    {
        retVal = b_pos;
        return 0;
    };
    virtual result_t get_end(int32_t& retVal)
    {
        retVal = e_pos;
        return 0;
    };

private:
    obj_ptr<SeekableStream_base> m_stream;
    int64_t b_pos; // begin pos
    int64_t e_pos; // end pos
};

result_t RangeStream_base::_new(SeekableStream_base* stm, exlib::string range, obj_ptr<RangeStream_base>& retVal, v8::Local<v8::Object> This)
{
    int64_t sz, begin, end;

    stm->size(sz);

    int32_t pr;
    if ((pr = _parseRange(range, sz, begin, end)) == -1)
        pr = _parseRange("bytes=" + range, sz, begin, end);

    if (pr != 0)
        return CALL_E_INVALIDARG;

    retVal = new RangeStream(stm, begin, end);
    return 0;
}

result_t RangeStream_base::_new(SeekableStream_base* stm, int32_t begin, int32_t end, obj_ptr<RangeStream_base>& retVal, v8::Local<v8::Object> This)
{
    if (begin < 0)
        return CALL_E_INVALIDARG;

    int64_t sz;
    stm->size(sz);

    if (begin > sz)
        return CALL_E_INVALIDARG;

    retVal = new RangeStream(stm, begin, end);
    return 0;
}

}

#endif // IORANGESTREAM_H_