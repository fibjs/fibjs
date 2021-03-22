/**
 * @author gmxyb
 * @email gmxyb@163.com
 * @create date 2020-08-24 17:49:40
 * @modify date 2020-08-24 17:49:40
 * @desc File Range R/W strem
 */

#pragma once

#include "ifs/fs.h"
#include "ifs/io.h"
#include "ifs/RangeStream.h"
#include "Stat.h"

namespace fibjs {

class RangeStream : public RangeStream_base {
public:
    RangeStream(SeekableStream_base* stream, int64_t begin, int64_t end);

private:
    int64_t get_c_pos();
    int64_t valid_end();
    int64_t valid_start();

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal);
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, AsyncEvent* ac);
    virtual result_t flush(AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac);

public:
    // SeekableStream_base
    virtual result_t seek(int64_t offset, int32_t whence);
    virtual result_t tell(int64_t& retVal);
    virtual result_t rewind();
    virtual result_t size(int64_t& retVal);
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t truncate(int64_t bytes, AsyncEvent* ac);
    virtual result_t eof(bool& retVal);
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac);

public:
    // RangeStream_base
    virtual result_t get_begin(int64_t& retVal);
    virtual result_t get_end(int64_t& retVal);

private:
    obj_ptr<SeekableStream_base> m_stream;
    int64_t b_pos; // begin position
    int64_t real_pos; // real position
    int64_t e_pos; // end position
};
}
