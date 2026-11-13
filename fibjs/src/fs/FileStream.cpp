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
#include "errhelp.h"

#include "ifs/io.h"
#include "ifs/fs.h"
#include "file_path.h"
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

// The descriptor is already released: every method on a closed stream reports
// the same thing, and EBADF is what Node reports for a file descriptor that is
// no longer open. A local helper keeps the message and the code in one place,
// so the fifteen call sites cannot drift apart.
static result_t file_closed()
{
    return errhelp::closed("EBADF", "FileStream: file is closed.");
}

result_t FileStream::readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    if (m_fd == -1)
        return file_closed();

    exlib::string strBuf;

    if (bytes < 0) {
        int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
        if (p < 0)
            return LastError("lseek", name);

        int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
        if (sz < 0)
            return LastError("lseek", name);

        if (_lseeki64(m_fd, p, SEEK_SET) < 0)
            return LastError("lseek", name);

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
                return ReadError("read", name);
            if (n == 0)
                break;

            sz -= n;
            p += n;
        }

        strBuf.resize(bytes - sz);
    }

    if (strBuf.length() == 0)
        return result_t::return_null();

    retVal = new Buffer(strBuf.c_str(), strBuf.length());

    return result_t();
}

result_t FileStream::readAll(obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    if (m_fd == -1)
        return file_closed();

    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return LastError("lseek", name);

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return LastError("lseek", name);

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return LastError("lseek", name);

    sz -= p;

    int32_t bytes = (int32_t)sz;

    if (bytes <= 0)
        return result_t::return_null();

    // Allocate Buffer directly and read into it to avoid extra copy
    std::shared_ptr<v8::BackingStore> store = NewBackingStore(bytes);
    char* buf = (char*)store->Data();
    int32_t remaining = bytes;

    while (remaining > 0) {
        int32_t n = (int32_t)::_read(m_fd, buf, remaining > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : remaining);
        if (n < 0)
            return ReadError("read", name);
        if (n == 0)
            break;

        remaining -= n;
        buf += n;
    }

    int32_t actualBytes = bytes - remaining;
    if (actualBytes == 0)
        return result_t::return_null();

    retVal = new Buffer(store, 0, actualBytes);

    return result_t();
}

result_t FileStream::readAllText(exlib::string& retVal)
{
    if (m_fd == -1)
        return file_closed();

    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return LastError("lseek", name);

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return LastError("lseek", name);

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return LastError("lseek", name);

    sz -= p;

    int32_t bytes = (int32_t)sz;

    if (bytes <= 0) {
        retVal.clear();
        return result_t();
    }

    // Allocate string buffer directly and read into it
    retVal.resize(bytes);
    char* buf = retVal.data();
    int32_t remaining = bytes;

    while (remaining > 0) {
        int32_t n = (int32_t)::_read(m_fd, buf, remaining > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : remaining);
        if (n < 0)
            return ReadError("read", name);
        if (n == 0)
            break;

        remaining -= n;
        buf += n;
    }

    int32_t actualBytes = bytes - remaining;
    retVal.resize(actualBytes);

    return result_t();
}

result_t FileStream::Write(const char* p, int32_t sz, int64_t diag_buffer_len)
{
    if (m_fd == -1)
        return file_closed();

    while (sz) {
        int32_t n = (int32_t)::_write(m_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
        if (n < 0) {
            int32_t err = LastError();

            if (err < 0 && err > CALL_E_MAX) {
                // The failing layer builds the complete value: the write
                // context plus the buffer summary its caller handed down.
                result_t::builder b = result_t::from_system(err).with_syscall("write").with_path(name).arg("length", sz);

                if (diag_buffer_len >= 0)
                    b = std::move(b).arg_buffer("buffer", diag_buffer_len);

                return result_t(std::move(b));
            }

            return result_t(err);
        }

        sz -= n;
        p += n;
    }

    return result_t();
}

result_t FileStream::writeBuffer(Buffer_base* data, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    if (m_fd == -1)
        return file_closed();

    return Write(data);
}

result_t FileStream::open(exlib::string fname, exlib::string flags, int32_t mode)
{
    close();

    result_t hr = normalize_file_path_like(fname, name);
    if (hr.failed())
        return hr;

    return file_open(name, flags, mode, m_fd);
}

result_t FileStream::get_name(exlib::string& retVal)
{
    if (m_fd == -1)
        return file_closed();

    retVal = name;
    return result_t();
}

result_t FileStream::get_fd(int32_t& retVal)
{
    if (m_fd == -1)
        return file_closed();

    retVal = m_fd;
    return result_t();
}

result_t FileStream::stat(obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (m_fd == -1)
        return file_closed();

    return fs_base::stat(name, retVal, std::move(ac));
}

result_t FileStream::size(int64_t& retVal)
{
    if (m_fd == -1)
        return file_closed();

    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return LastError("lseek", name);

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return LastError("lseek", name);

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return LastError("lseek", name);

    retVal = sz;
    return result_t();
}

result_t FileStream::eof(bool& retVal)
{
    if (m_fd == -1)
        return file_closed();

    int64_t p = _lseeki64(m_fd, 0, SEEK_CUR);
    if (p < 0)
        return LastError("lseek", name);

    int64_t sz = _lseeki64(m_fd, 0, SEEK_END);
    if (sz < 0)
        return LastError("lseek", name);

    if (_lseeki64(m_fd, p, SEEK_SET) < 0)
        return LastError("lseek", name);

    retVal = sz == p;

    return result_t();
}

result_t FileStream::seek(int64_t offset, int32_t whence)
{
    if (m_fd == -1)
        return file_closed();

    if (_lseeki64(m_fd, offset, whence) < 0)
        return LastError("lseek", name);

    return result_t();
}

result_t FileStream::tell(int64_t& retVal)
{
    if (m_fd == -1)
        return file_closed();

    retVal = _lseeki64(m_fd, 0, SEEK_CUR);
    if (retVal < 0)
        return LastError("lseek", name);

    return result_t();
}

result_t FileStream::rewind()
{
    if (m_fd == -1)
        return file_closed();

    if (_lseeki64(m_fd, 0, SEEK_SET) < 0)
        return LastError("lseek", name);

    return result_t();
}

result_t FileStream::flush(AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    if (m_fd == -1)
        return file_closed();

    return result_t();
}

result_t FileStream::close()
{
    if (m_fd != -1) {
        ::_close(m_fd);
        m_fd = -1;
    }

    return result_t();
}

result_t FileStream::close(AsyncHandle ac)
{
    if (m_fd != -1) {
        if (ac.isSync())
            return result_t::nosync();

        return close();
    }

    return result_t();
}

result_t FileStream::truncate(int64_t bytes, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    if (m_fd == -1)
        return file_closed();

    if (ftruncate64(m_fd, bytes) < 0)
        return LastError("ftruncate", name);

    return result_t();
}

result_t FileStream::chmod(int32_t mode, AsyncHandle ac)
{
#ifdef _WIN32
    return result_t::invalid_call();
#else
    if (ac.isSync())
        return result_t::nosync();

    if (::fchmod(m_fd, mode))
        return LastError("fchmod", name);

    return result_t();
#endif
}

// The Node.js class form: `new fs.ReadStream(path[, options])` and
// `new fs.WriteStream(path[, options])` reach this constructor through the
// fs.ReadStream / fs.WriteStream aliases. Unlike createReadStream, which reports
// an open failure through an 'error' event, the constructor throws: a caller
// that builds the object itself has no place to listen yet.
result_t FileStream_base::_new(exlib::string path, Options_FileStream_FileStream_options* options,
    obj_ptr<FileStream_base>& retVal, v8::Local<v8::Object> This)
{
    exlib::string flags = options ? options->flags.value_or("r") : "r";
    int64_t start = options ? options->start.value_or(-1) : -1;

    // A bounded range needs the RangeStream that createReadStream builds when
    // `end` is set; this class is the whole file, so the option cannot be
    // honoured here and is reported instead of silently reading past it.
    int64_t end = options ? options->end.value_or(-1) : -1;
    if (end >= 0)
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_message("The \"options.end\" property is not supported by fs.ReadStream; use fs.createReadStream(path, { start, end }) for a bounded range");

    obj_ptr<FileStream> stm = new FileStream();

    result_t hr = stm->open(path, flags);
    if (hr.failed())
        return hr;

    if (start > 0) {
        hr = stm->seek(start, SEEK_SET);
        if (hr.failed()) {
            stm->close();
            return hr;
        }
    }

    retVal = stm;
    return result_t();
}
}
