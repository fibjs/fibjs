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

        real_pos = get_c_pos();
    }

private:
    int64_t get_c_pos()
    {
        int64_t pos;
        m_stream->tell(pos);
        return pos;
    }

    int64_t valid_end()
    {
        int64_t sz;
        m_stream->size(sz);

        return sz > e_pos ? e_pos : sz;
    }

    int64_t valid_start()
    {
        return b_pos > 0 ? b_pos : 0;
    }

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        if (e_pos < real_pos || b_pos > real_pos)
            return CALL_RETURN_NULL;
        
        int64_t c_pos_snap = get_c_pos();
        if (c_pos_snap != real_pos)
            m_stream->seek(real_pos, fs_base::_SEEK_SET);

        int64_t rest_sz = e_pos - real_pos;

        result_t hr;
        if (bytes < 0 || bytes > rest_sz) {
            hr = m_stream->read(rest_sz, retVal, ac);
        } else {
            hr = m_stream->read(bytes, retVal, ac);
        }

        m_stream->seek(c_pos_snap, fs_base::_SEEK_SET);

        return hr;
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
        return CALL_E_INVALID_CALL;
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
        case fs_base::_SEEK_CUR:
            offset += real_pos;
            break;
        case fs_base::_SEEK_END:
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
        retVal = valid_end() - valid_start();

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
        retVal = get_c_pos() >= valid_end();

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
    int64_t real_pos; // real_pos pos
    int64_t e_pos; // end pos
};

}

#endif // IORANGESTREAM_H_