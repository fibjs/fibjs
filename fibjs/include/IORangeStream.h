/**
 * @author Richard
 * @email richardo2016@gmail.com
 * @create date 2020-08-24 17:49:40
 * @modify date 2020-08-24 17:49:40
 * @desc File Range R/W strem
 */
#include "ifs/fs.h"
#include "ifs/io.h"
#include "ifs/IORangeStream.h"
#include "Stat.h"

#ifndef IORANGESTREAM_H_
#define IORANGESTREAM_H_

namespace fibjs {

class IORangeStream : public IORangeStream_base {
public:
    IORangeStream(obj_ptr<SeekableStream_base> stream, int64_t begin, int64_t end)
    {
        m_stream = stream;
        b_pos = begin;
        e_pos = end;
        m_stream->seek(b_pos, fs_base::_SEEK_SET);
        c_pos = b_pos;
    }

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        int64_t sz = e_pos - c_pos;
        if (bytes < 0 || bytes > sz)
            return m_stream->read(sz, retVal, ac);
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
    virtual result_t seek(int64_t offset, int32_t whence)
    {
        if (whence == fs_base::_SEEK_SET)
            c_pos = b_pos + offset;
        else if (whence == fs_base::_SEEK_CUR)
            c_pos = c_pos + offset;
        else if (whence == fs_base::_SEEK_END)
            c_pos = e_pos + offset;
        else
            return CHECK_ERROR(CALL_E_INVALIDARG);

        if (c_pos < b_pos)
            c_pos = b_pos;
        else if (c_pos > e_pos)
            c_pos = e_pos;

        return 0;
    }
    virtual result_t tell(int64_t& retVal)
    {
        retVal = c_pos - b_pos;

        return 0;
    }
    virtual result_t rewind()
    {
        m_stream->seek(b_pos, fs_base::_SEEK_SET);
        c_pos = b_pos;

        return 0;
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
        retVal = c_pos >= e_pos;

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

private:
    obj_ptr<SeekableStream_base> m_stream;
    int64_t b_pos; // begin pos
    int64_t c_pos; // current pos
    int64_t e_pos; // end pos
};

result_t IORangeStream_base::_new(SeekableStream_base* stm, int32_t begin, int32_t end, obj_ptr<IORangeStream_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new IORangeStream(stm, begin, end);
    return 0;
}

}

#endif // IORANGESTREAM_H_