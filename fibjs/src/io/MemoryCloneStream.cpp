/*
 * MemoryCloneStream.cpp
 *
 *  Created on: Jul 4, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "MemoryStream.h"
#include "Stat.h"
#include "Buffer.h"

namespace fibjs {

result_t MemoryStream::CloneStream::get_fd(int32_t& retVal)
{
    return result_t::invalid_call();
}

result_t MemoryStream::CloneStream::readBuffer(int32_t bytes,
    obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    exlib::string strBuf;

    int32_t sz;
    sz = (int32_t)m_buffer.length() - m_pos;

    if (bytes < 0 || bytes > sz)
        bytes = sz;

    if (bytes > 0) {
        if (m_pos == 0 && bytes == sz) {
            strBuf = m_buffer;
            m_pos = (int32_t)m_buffer.length();
        } else {
            strBuf = m_buffer.substr(m_pos, bytes);
            m_pos += bytes;
        }
    }

    if (strBuf.length() == 0)
        return result_t::return_null();

    retVal = new Buffer(strBuf.c_str(), strBuf.length());

    return result_t();
}

result_t MemoryStream::CloneStream::readAll(obj_ptr<Buffer_base>& retVal,
    AsyncHandle ac)
{
    return readBuffer(-1, retVal, std::move(ac));
}

result_t MemoryStream::CloneStream::truncate(int64_t bytes, AsyncHandle ac)
{
    return result_t::invalid_call();
}

result_t MemoryStream::CloneStream::eof(bool& retVal)
{
    retVal = m_pos == (int32_t)m_buffer.length();
    return result_t();
}

result_t MemoryStream::CloneStream::flush(AsyncHandle ac)
{
    return result_t::invalid_call();
}

result_t MemoryStream::CloneStream::writeBuffer(Buffer_base* data,
    AsyncHandle ac)
{
    return result_t::invalid_call();
}

result_t MemoryStream::CloneStream::close(AsyncHandle ac)
{
    return result_t();
}

result_t MemoryStream::CloneStream::stat(obj_ptr<Stat_base>& retVal,
    AsyncHandle ac)
{
    obj_ptr<Stat> st = new Stat();

    st->init();
    st->m_isMemory = true;
    st->mtime = st->ctime = m_time;
    size(st->size);

    retVal = st;

    return result_t();
}

result_t MemoryStream::CloneStream::seek(int64_t offset, int32_t whence)
{
    if (whence == fs_base::C_SEEK_SET)
        m_pos = (int32_t)offset;
    else if (whence == fs_base::C_SEEK_CUR)
        m_pos += (int32_t)offset;
    else if (whence == fs_base::C_SEEK_END)
        m_pos = (int32_t)offset + (int32_t)m_buffer.length();
    else
        return result_t::invalid_arg();

    if (m_pos < 0)
        m_pos = 0;
    else if (m_pos > (int32_t)m_buffer.length())
        m_pos = (int32_t)m_buffer.length();

    return result_t();
}

result_t MemoryStream::CloneStream::tell(int64_t& retVal)
{
    retVal = m_pos;
    return result_t();
}

result_t MemoryStream::CloneStream::rewind()
{
    m_pos = 0;
    return result_t();
}

result_t MemoryStream::CloneStream::size(int64_t& retVal)
{
    retVal = m_buffer.length();
    return result_t();
}

result_t MemoryStream::CloneStream::setTime(date_t d)
{
    return result_t::invalid_call();
}

result_t MemoryStream::CloneStream::clone(obj_ptr<MemoryStream_base>& retVal)
{
    retVal = new CloneStream(m_buffer, m_time);
    return result_t();
}

result_t MemoryStream::CloneStream::clear()
{
    rewind();
    m_buffer.clear();

    m_time.now();

    return result_t();
}
}
