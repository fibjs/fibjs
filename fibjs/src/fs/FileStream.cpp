/*
 * FileStream.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#ifndef _WIN32
#define _FILE_OFFSET_BITS 64
#endif

#include "object.h"

#include "ifs/io.h"
#include "ifs/fs.h"
#include "FileStream.h"
#include "Buffer.h"
#include "v8_api.h"

#ifdef _WIN32
#define pclose _pclose
#endif

namespace fibjs {

FileStream::~FileStream()
{
    if (m_fd != -1)
        async([fd = m_fd]() {
            ::_close(fd);
        });
}

result_t FileStream::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strBuf;

    if (bytes < 0) {
        int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
        if (p < 0)
            return CHECK_ERROR(LastError());

        int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
        if (sz < 0)
            return CHECK_ERROR(LastError());

        if (_lseeki64(m_fd, p, SEEK_SET) < 0)
            return CHECK_ERROR(LastError());

        sz -= p;

        if (sz > STREAM_BUFF_SIZE)
            sz = STREAM_BUFF_SIZE;

        bytes = (int32_t)sz;
    }

    if (bytes > 0) {
        strBuf.resize(bytes);
        int32_t sz = bytes;
        char* p = strBuf.data();

        while (sz) {
            int32_t n = (int32_t)::_read(m_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
            if (n < 0)
                return CHECK_ERROR(LastError());
            if (n == 0)
                break;

            sz -= n;
            p += n;
        }

        strBuf.resize(bytes - sz);
    }

    if (strBuf.length() == 0)
        return CALL_RETURN_NULL;

    retVal = new Buffer(strBuf.c_str(), strBuf.length());

    return 0;
}

result_t FileStream::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return CHECK_ERROR(LastError());

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return CHECK_ERROR(LastError());

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    sz -= p;

    int32_t bytes = (int32_t)sz;

    if (bytes <= 0)
        return CALL_RETURN_NULL;

    // Allocate Buffer directly and read into it to avoid extra copy
    std::shared_ptr<v8::BackingStore> store = NewBackingStore(bytes);
    char* buf = (char*)store->Data();
    int32_t remaining = bytes;

    while (remaining > 0) {
        int32_t n = (int32_t)::_read(m_fd, buf, remaining > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : remaining);
        if (n < 0)
            return CHECK_ERROR(LastError());
        if (n == 0)
            break;

        remaining -= n;
        buf += n;
    }

    int32_t actualBytes = bytes - remaining;
    if (actualBytes == 0)
        return CALL_RETURN_NULL;

    retVal = new Buffer(store, 0, actualBytes);

    return 0;
}

result_t FileStream::readAllText(exlib::string& retVal)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return CHECK_ERROR(LastError());

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return CHECK_ERROR(LastError());

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    sz -= p;

    int32_t bytes = (int32_t)sz;

    if (bytes <= 0) {
        retVal.clear();
        return 0;
    }

    // Allocate string buffer directly and read into it
    retVal.resize(bytes);
    char* buf = retVal.data();
    int32_t remaining = bytes;

    while (remaining > 0) {
        int32_t n = (int32_t)::_read(m_fd, buf, remaining > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : remaining);
        if (n < 0)
            return CHECK_ERROR(LastError());
        if (n == 0)
            break;

        remaining -= n;
        buf += n;
    }

    int32_t actualBytes = bytes - remaining;
    retVal.resize(actualBytes);

    return 0;
}

result_t FileStream::Write(const char* p, int32_t sz)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    while (sz) {
        int32_t n = (int32_t)::_write(m_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
        if (n < 0)
            return CHECK_ERROR(LastError());

        sz -= n;
        p += n;
    }

    return 0;
}

result_t FileStream::write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer> buf = Buffer::Cast(data);
    retVal = (int32_t)buf->length();

    return Write(data);
}

result_t FileStream::open(exlib::string fname, exlib::string flags)
{
    close();
    name = fname;
    return file_open(fname, flags, 0666, m_fd);
}

result_t FileStream::get_name(exlib::string& retVal)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = name;
    return 0;
}

result_t FileStream::get_fd(int32_t& retVal)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_fd;
    return 0;
}

result_t FileStream::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return fs_base::stat(name, retVal, ac);
}

result_t FileStream::size(int64_t& retVal)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return CHECK_ERROR(LastError());

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return CHECK_ERROR(LastError());

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    retVal = sz;
    return 0;
}

result_t FileStream::eof(bool& retVal)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return CHECK_ERROR(LastError());

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return CHECK_ERROR(LastError());

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    retVal = sz == p;

    return 0;
}

result_t FileStream::seek(int64_t offset, int32_t whence)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (_lseeki64(m_fd, offset, whence) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t FileStream::tell(int64_t& retVal)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = _lseeki64(m_fd, 0, SEEK_CUR);
    if (retVal < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t FileStream::rewind()
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (_lseeki64(m_fd, 0, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t FileStream::flush(AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    //    fflush(m_file);

    //    if (ferror(m_file))
    //        return CHECK_ERROR(LastError());

    return 0;
}

result_t FileStream::close()
{
    if (m_fd != -1) {
        ::_close(m_fd);
        m_fd = -1;
    }

    return 0;
}

result_t FileStream::close(AsyncEvent* ac)
{
    if (m_fd != -1) {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return close();
    }

    return 0;
}

result_t FileStream::truncate(int64_t bytes, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (ftruncate64(m_fd, bytes) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t FileStream::chmod(int32_t mode, AsyncEvent* ac)
{
#ifdef _WIN32
    return CHECK_ERROR(CALL_E_INVALID_CALL);
#else
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::fchmod(m_fd, mode))
        return CHECK_ERROR(LastError());

    return 0;
#endif
}
}
