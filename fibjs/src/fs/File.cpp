/*
 * File.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"

#ifndef _WIN32
#include <sys/ioctl.h>
#endif

#include <fcntl.h>

#include "ifs/io.h"
#include "File.h"
#include "Buffer.h"
#include "Stat.h"
#include "utf8.h"

#ifdef _WIN32
#define pclose _pclose
#endif

namespace fibjs {

File::~File()
{
    if (m_fd != -1) {
        if (m_pipe)
            asyncCall(pclose, m_pipe);
        else
            asyncCall(::_close, m_fd);
    }
}

result_t File::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strBuf;

    if (bytes < 0) {
        if (m_pipe)
            bytes = STREAM_BUFF_SIZE;
        else {
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
    }

    if (bytes > 0) {
        strBuf.resize(bytes);
        int32_t sz = bytes;
        char* p = &strBuf[0];

        while (sz) {
            int32_t n = (int32_t)::_read(m_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
            if (n < 0)
                return CHECK_ERROR(LastError());
            if (n == 0)
                break;

            sz -= n;
            p += n;

            if (m_pipe)
                break;
        }

        strBuf.resize(bytes - sz);
    }

    if (strBuf.length() == 0)
        return CALL_RETURN_NULL;

    retVal = new Buffer(strBuf);

    return 0;
}

result_t File::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strBuf;

    int32_t bytes;
    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return CHECK_ERROR(LastError());

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return CHECK_ERROR(LastError());

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    sz -= p;

    bytes = (int32_t)sz;

    if (bytes > 0) {
        strBuf.resize(bytes);
        int32_t sz = bytes;
        char* p = &strBuf[0];

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

    retVal = new Buffer(strBuf);

    return 0;
}

result_t File::Write(const char* p, int32_t sz)
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

result_t File::write(Buffer_base* data, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strBuf;
    data->toString(strBuf);

    return Write(strBuf);
}

result_t File::copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal,
    AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return io_base::copyStream(this, stm, bytes, retVal, ac);
}

result_t File::open(exlib::string fname, exlib::string flags)
{
#ifdef _WIN32
    int32_t _flags = _O_BINARY;
#else
    int32_t _flags = 0;
#endif

    if (flags == "r")
        _flags |= O_RDONLY;
    else if (flags == "r+")
        _flags |= O_RDWR;
    else if (flags == "w")
        _flags |= O_TRUNC | O_CREAT | O_WRONLY;
    else if (flags == "w+")
        _flags |= O_TRUNC | O_CREAT | O_RDWR;
    else if (flags == "a")
        _flags |= O_APPEND | O_CREAT | O_WRONLY;
    else if (flags == "a+")
        _flags |= O_APPEND | O_CREAT | O_RDWR;

    close();

#ifdef _WIN32
    m_fd = _wopen(UTF8_W(fname), _flags, _S_IREAD | _S_IWRITE);
#else
    m_fd = ::open(fname.c_str(), _flags, 0666);
#endif
    if (m_fd < 0)
        return CHECK_ERROR(LastError());

#ifndef _WIN32
    struct stat64 st;
    fstat64(m_fd, &st);

    if (S_IFDIR & st.st_mode) {
        ::_close(m_fd);
        m_fd = -1;
        return CHECK_ERROR(CALL_E_FILE_NOT_FOUND);
    }

    if (::fcntl(m_fd, F_SETFD, FD_CLOEXEC))
        return CHECK_ERROR(LastError());
#endif

    name = fname;

    return 0;
}

result_t File::get_name(exlib::string& retVal)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = name;
    return 0;
}

#ifdef _WIN32

result_t File::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    BY_HANDLE_FILE_INFORMATION fdata;

    if (!GetFileInformationByHandle((HANDLE)_get_osfhandle(m_fd), &fdata))
        return CHECK_ERROR(LastError());

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill(name, fdata);

    retVal = pStat;

    return 0;
}
#else
result_t File::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    struct stat64 st;
    fstat64(m_fd, &st);

    obj_ptr<Stat> pStat = new Stat();
    pStat->fill(name, st);
    retVal = pStat;

    return 0;
}
#endif

result_t File::size(int64_t& retVal)
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

result_t File::eof(bool& retVal)
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

result_t File::seek(int64_t offset, int32_t whence)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (_lseeki64(m_fd, offset, whence) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t File::tell(int64_t& retVal)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = _lseeki64(m_fd, 0, SEEK_CUR);
    if (retVal < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t File::rewind()
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (_lseeki64(m_fd, 0, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t File::flush(AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    //    fflush(m_file);

    //    if (ferror(m_file))
    //        return CHECK_ERROR(LastError());

    return 0;
}

result_t File::close()
{
    if (m_fd != -1) {
        if (m_pipe)
            pclose(m_pipe);
        else
            ::_close(m_fd);

        m_fd = -1;
        m_pipe = NULL;
    }

    return 0;
}

result_t File::close(AsyncEvent* ac)
{
    if (m_fd != -1) {
        if (!ac)
            return CHECK_ERROR(CALL_E_NOSYNC);

        return close();
    }

    return 0;
}

result_t File::truncate(int64_t bytes, AsyncEvent* ac)
{
    if (m_fd == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (ftruncate64(m_fd, bytes) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t File::chmod(int32_t mode, AsyncEvent* ac)
{
#ifdef _WIN32
    return CHECK_ERROR(CALL_E_INVALID_CALL);
#else
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::fchmod(m_fd, mode))
        return CHECK_ERROR(LastError());

    return 0;
#endif
}
}
