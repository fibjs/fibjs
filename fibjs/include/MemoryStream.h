/*
 * MemoryStream.h
 *
 *  Created on: Jun 29, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/os.h"
#include "ifs/fs.h"
#include "ifs/MemoryStream.h"
#include <sstream>
#include "AsyncStream.h"

namespace fibjs {

class MemoryStream : public AsyncStream<MemoryStream_base> {
public:
    class CloneStream : public AsyncStream<MemoryStream_base> {
    public:
        CloneStream(exlib::string buffer, date_t tm)
            : m_buffer(std::move(buffer))
            , m_time(tm)
            , m_pos(0)
        {
            extMemory((int32_t)m_buffer.length());
        }

        // Share an existing buffer instead of copying it: the zip VFS keeps
        // each entry's bytes in the cache and hands out a stream over them.
        CloneStream(obj_ptr<Buffer_base> buffer, date_t tm)
            : m_data(buffer)
            , m_time(tm)
            , m_pos(0)
        {
        }

    public:
        // Stream_base
        virtual result_t get_fd(int32_t& retVal);
        virtual result_t readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
        virtual result_t writeBuffer(Buffer_base* data, AsyncHandle ac);
        virtual result_t flush(AsyncHandle ac);
        virtual result_t close(AsyncHandle ac);

    public:
        // SeekableStream_base
        virtual result_t seek(int64_t offset, int32_t whence);
        virtual result_t tell(int64_t& retVal);
        virtual result_t rewind();
        virtual result_t size(int64_t& retVal);
        virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
        virtual result_t truncate(int64_t bytes, AsyncHandle ac);
        virtual result_t eof(bool& retVal);
        virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncHandle ac);

    public:
        // MemoryStream_base
        virtual result_t setTime(date_t d);
        virtual result_t clone(obj_ptr<MemoryStream_base>& retVal);
        virtual result_t clear();

    private:
        int64_t buffer_size() const
        {
            return m_data ? (int64_t)Buffer::Cast(m_data)->length() : (int64_t)m_buffer.length();
        }

    private:
        exlib::string m_buffer;
        obj_ptr<Buffer_base> m_data;
        date_t m_time;
        int32_t m_pos;
    };

public:
    MemoryStream()
    {
        m_time.now();
    }

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal);
    virtual result_t readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    virtual result_t writeBuffer(Buffer_base* data, AsyncHandle ac);
    virtual result_t flush(AsyncHandle ac);
    virtual result_t close(AsyncHandle ac);

public:
    // SeekableStream_base
    virtual result_t seek(int64_t offset, int32_t whence);
    virtual result_t tell(int64_t& retVal);
    virtual result_t rewind();
    virtual result_t size(int64_t& retVal);
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    virtual result_t truncate(int64_t bytes, AsyncHandle ac);
    virtual result_t eof(bool& retVal);
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncHandle ac);

public:
    // MemoryStream_base
    virtual result_t setTime(date_t d);
    virtual result_t clone(obj_ptr<MemoryStream_base>& retVal);
    virtual result_t clear();

private:
    std::stringstream m_buffer;
    date_t m_time;
    // Cached length of m_buffer. size() must not seek: the read position of the
    // stream is shared with concurrent readers (a buffered body is read from an
    // AsyncStreamReader running on a worker thread), and a size() that seeks to
    // the end and back corrupts the position under that concurrency.
    int64_t m_size = 0;
};

} /* namespace fibjs */
