/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#ifndef _WIN32
#define _FILE_OFFSET_BITS 64
#endif

#include "ifs/fs.h"
#include "ifs/zip.h"
#include "encoding_conv.h"
#include "file_path.h"
#include "path.h"
#include "Buffer.h"
#include "Stat.h"
#include "DirEntry.h"
#include "Dir.h"
#include "FileStream.h"
#include "RangeStream.h"
#include "AsyncUV.h"
#include "EventEmitter.h"
#include "utils.h"
#include "encoding.h"

namespace fibjs {

DECLARE_MODULE(fs);

// ---------------------------------------------------------------------------
// Node.js compatible argument helpers
// ---------------------------------------------------------------------------

// Node.js time argument: Date object | unix timestamp in seconds | date string
static result_t to_unix_timestamp(Variant& v, double& retVal)
{
    switch (v.type()) {
    case Variant::VT_Date: {
        double ms = v.dateValue();

        if (std::isnan(ms))
            return CHECK_ERROR(CALL_E_INVALIDARG);

        retVal = ms / 1000;
        return 0;
    }
    case Variant::VT_String: {
        date_t d;
        d.parse(v.string());

        double ms = d.date();
        if (std::isnan(ms))
            return CHECK_ERROR(CALL_E_INVALIDARG);

        retVal = ms / 1000;
        return 0;
    }
    case Variant::VT_Number:
    case Variant::VT_Integer:
    case Variant::VT_Long:
        retVal = v.dblVal();
        return 0;
    default:
        return CHECK_ERROR(CALL_E_BADVARTYPE);
    }
}

// Node.js mode argument: an integer, or an octal string ('755' / '0755' / '0o755')
static result_t to_mode_value(Variant& v, int32_t& retVal)
{
    switch (v.type()) {
    case Variant::VT_Number:
    case Variant::VT_Integer:
    case Variant::VT_Long: {
        double d = v.dblVal();

        if (std::isnan(d) || d < 0 || d > 07777 || floor(d) != d)
            return CHECK_ERROR(CALL_E_INVALIDARG);

        retVal = (int32_t)d;
        return 0;
    }
    case Variant::VT_String: {
        exlib::string s = v.string();
        size_t pos = 0;

        while (pos < s.length() && (s[pos] == ' ' || s[pos] == '\t'))
            pos++;

        if (pos + 1 < s.length() && s[pos] == '0' && (s[pos + 1] == 'o' || s[pos + 1] == 'O'))
            pos += 2;

        if (pos >= s.length())
            return CHECK_ERROR(CALL_E_INVALIDARG);

        int32_t mode = 0;
        for (; pos < s.length(); pos++) {
            char c = s[pos];

            if (c < '0' || c > '7')
                return CHECK_ERROR(CALL_E_INVALIDARG);

            mode = mode * 8 + (c - '0');

            if (mode > 07777)
                return CHECK_ERROR(CALL_E_INVALIDARG);
        }

        retVal = mode;
        return 0;
    }
    default:
        return CHECK_ERROR(CALL_E_BADVARTYPE);
    }
}

result_t FileHandle_base::_new(int32_t fd, obj_ptr<FileHandle_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new FileHandle(fd);
    return 0;
}

result_t FileHandle::get_fd(int32_t& retVal)
{
    retVal = m_fd;
    return 0;
}

result_t FileHandle::chmod(int32_t mode, AsyncEvent* ac)
{
    return fs_base::fchmod(this, mode, ac);
}

result_t FileHandle::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    return fs_base::fstat(this, retVal, ac);
}

result_t FileHandle::read(Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, obj_ptr<ReadType>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t bufLength = Buffer::Cast(buffer)->length();

    if (offset < 0 || offset >= bufLength)
        return Runtime::setError("fs: Offset is out of bounds");

    if (length < 0 || (offset + length > bufLength))
        return Runtime::setError("fs: Length extends beyond buffer");

    if (position > -1) {
        if (_lseeki64(m_fd, position, SEEK_SET) < 0)
            return CHECK_ERROR(LastError());
    }

    int32_t bytesRead = 0;
    if (length > 0) {
        exlib::string strBuf;
        strBuf.resize(length);
        int32_t sz = length;
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

        strBuf.resize(length - sz);

        if (strBuf.length() > 0) {
            int32_t written = 0;
            result_t hr = buffer->write(strBuf, offset, (int32_t)strBuf.length(), "utf8", written);
            if (hr < 0)
                return hr;
            bytesRead = written;
        }
    }

    obj_ptr<ReadType> obj = new ReadType();
    obj->bytesRead = bytesRead;
    obj->buffer = buffer;
    retVal = obj;

    return 0;
}

result_t FileHandle::read(v8::Local<v8::Object> options, obj_ptr<ReadType>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(4);

        obj_ptr<Buffer_base> buffer;
        GetConfigValue(options, "buffer", buffer);
        if (buffer == NULL)
            buffer = new Buffer(NULL, 16384);

        int32_t offset = 0;
        GetConfigValue(options, "offset", offset, true);

        int32_t length = Buffer::Cast(buffer)->length() - offset;
        GetConfigValue(options, "length", length, true);

        int32_t position = -1;
        GetConfigValue(options, "position", position, true);

        ac->m_ctx[0] = buffer;
        ac->m_ctx[1] = offset;
        ac->m_ctx[2] = length;
        ac->m_ctx[3] = position;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return read(Buffer_base::getInstance(ac->m_ctx[0].object()),
        ac->m_ctx[1].intVal(), ac->m_ctx[2].intVal(), ac->m_ctx[3].intVal(),
        retVal, ac);
}

result_t FileHandle::write(Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, obj_ptr<WriteType>& retVal, AsyncEvent* ac)
{
    setErrorContext("write");

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t n;
    result_t hr = fs_base::write(this, buffer, offset, length, position, n, ac);
    if (hr < 0)
        return hr;

    // Node.js: filehandle.write() resolves with { bytesWritten, buffer }
    obj_ptr<WriteType> result = new WriteType();
    result->bytesWritten = n;
    result->buffer = buffer;

    retVal = result;

    return 0;
}

result_t FileHandle::write(exlib::string string, int32_t position, exlib::string encoding, obj_ptr<WriteType>& retVal, AsyncEvent* ac)
{
    setErrorContext("write");

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<Buffer_base> buf;
    {
        exlib::string strData = string;
        result_t hr = commonEncode(encoding, strData, strData);
        if (hr < 0)
            return hr;

        buf = new Buffer(strData.c_str(), strData.length());
    }

    return write(buf, 0, -1, position, retVal, ac);
}

result_t FileHandle::readFile(exlib::string encoding, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    // seek to beginning
    if (_lseeki64(m_fd, 0, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    exlib::string strBuf;
    char tmp[STREAM_BUFF_SIZE];

    while (true) {
        int32_t n = (int32_t)::_read(m_fd, tmp, STREAM_BUFF_SIZE);
        if (n < 0)
            return CHECK_ERROR(LastError());
        if (n == 0)
            break;
        strBuf.append(tmp, n);
    }

    if (encoding != "") {
        obj_ptr<Buffer_base> buf = new Buffer(strBuf.c_str(), strBuf.length());
        return Buffer::Cast(buf)->toValue(encoding, retVal);
    }

    retVal = new Buffer(strBuf.c_str(), strBuf.length());
    return 0;
}

result_t FileHandle::readFile(v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        exlib::string encoding;
        GetConfigValue(options, "encoding", encoding);
        ac->m_ctx[0] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return readFile(ac->m_ctx[0].string(), retVal, ac);
}

result_t FileHandle::writeFile(Buffer_base* data, exlib::string opt, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    // seek to beginning and truncate
    if (_lseeki64(m_fd, 0, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    exlib::string strBuf;
    Buffer::Cast(data)->toString(strBuf);

    const char* p = strBuf.c_str();
    int32_t sz = (int32_t)strBuf.length();
    retVal = sz;

    while (sz > 0) {
        int32_t n = (int32_t)::_write(m_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
        if (n < 0)
            return CHECK_ERROR(LastError());
        sz -= n;
        p += n;
    }

    ftruncate64(m_fd, _lseeki64(m_fd, 0, SEEK_CUR));

    return 0;
}

result_t FileHandle::writeFile(exlib::string data, exlib::string opt, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = commonEncode(opt, data, data);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(data.c_str(), data.length());
    return writeFile(buf, "", retVal, ac);
}

result_t FileHandle::writeFile(Buffer_base* data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return writeFile(data, "", retVal, ac);
}

result_t FileHandle::writeFile(exlib::string data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        exlib::string encoding = "utf8";
        result_t hr = GetConfigValue(options, "encoding", encoding, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx[0] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return writeFile(data, ac->m_ctx[0].string(), retVal, ac);
}

result_t FileHandle::utimes(Variant atime, Variant mtime, AsyncEvent* ac)
{
    return fs_base::futimes(this, atime, mtime, ac);
}

result_t FileHandle::chown(int32_t uid, int32_t gid, AsyncEvent* ac)
{
    return fs_base::fchown(this, uid, gid, ac);
}

result_t FileHandle::sync(AsyncEvent* ac)
{
    return fs_base::fsync(this, ac);
}

result_t FileHandle::datasync(AsyncEvent* ac)
{
    return fs_base::fdatasync(this, ac);
}

result_t FileHandle::truncate(int32_t len, AsyncEvent* ac)
{
    return fs_base::ftruncate(this, len, ac);
}

result_t FileHandle::appendFile(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    return fs_base::appendFile(this, data, retVal, ac);
}

result_t FileHandle::appendFile(exlib::string data, int32_t& retVal, AsyncEvent* ac)
{
    return fs_base::appendFile(this, data, retVal, ac);
}

result_t FileHandle::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_fd == -1)
        return UV_EBADF;

    int32_t fd = m_fd;
    m_fd = -1;

    if (::_close(fd))
        return LastError();

    return 0;
}

result_t fs_base::open(exlib::string fname, exlib::string flags, int32_t mode,
    obj_ptr<FileHandle_base>& retVal, AsyncEvent* ac)
{
    setErrorContext("open", fname);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr < 0)
        return hr;

    int32_t _fd;
    hr = file_open(safe_name, flags, mode, _fd);
    if (hr < 0)
        return hr;

    retVal = new FileHandle(_fd);

    return 0;
}

result_t fs_base::open(exlib::string fname, int32_t flags, int32_t mode,
    obj_ptr<FileHandle_base>& retVal, AsyncEvent* ac)
{
    setErrorContext("open", fname);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr < 0)
        return hr;

    int32_t _fd;
    hr = file_open(safe_name, flags, mode, _fd);
    if (hr < 0)
        return hr;

    retVal = new FileHandle(_fd);

    return 0;
}

result_t fs_base::open(exlib::string fname, exlib::string flags, Variant mode,
    obj_ptr<FileHandle_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        int32_t _mode;
        result_t hr = to_mode_value(mode, _mode);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return open(fname, flags, ac->m_ctx[0].intVal(), retVal, ac);
}

result_t fs_base::close(FileHandle_base* fd, AsyncEvent* ac)
{
    return fd->close(ac);
}

result_t fs_base::openTextStream(exlib::string fname, exlib::string flags,
    obj_ptr<BufferedStream_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> pFile;
    result_t hr = openFile(fname, flags, pFile, ac);
    if (hr < 0)
        return hr;

    return BufferedStream_base::_new(pFile, retVal);
}

result_t fs_base::readTextFile(exlib::string fname, exlib::string& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = openFile(fname, "r", f, ac);
    if (hr < 0)
        return hr;

    hr = f->cc_readAll(buf);
    f->cc_close();

    if (hr == CALL_RETURN_NULL) {
        retVal.clear();
        return 0;
    }

    if (hr < 0)
        return hr;

    return buf->toString(retVal);
}

static result_t read_file_ext(exlib::string fname, exlib::string flag, exlib::string encoding,
    Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = fs_base::openFile(fname, flag, f, ac);
    if (hr < 0)
        return hr;

    // Fast path for utf8: read directly to string to avoid Buffer intermediate
    if (encoding == "utf8" || encoding == "utf-8") {
        FileStream_base* pFileBase = FileStream_base::getInstance(f);
        if (pFileBase) {
            // FileStream: use readAllText for zero-copy string reading
            FileStream* pFile = static_cast<FileStream*>(pFileBase);
            exlib::string strBuf;
            hr = pFile->readAllText(strBuf);
            f->cc_close();

            if (hr < 0)
                return hr;

            retVal = strBuf;
            return 0;
        }
    }

    // Normal path: read to Buffer first
    obj_ptr<Buffer_base> buf;
    hr = f->cc_readAll(buf);
    f->cc_close();

    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_NULL) {
        if (encoding != "") {
            exlib::string str;
            retVal = str;
        } else {
            buf = new Buffer();
            retVal = buf;
        }
    } else {
        if (encoding != "") {
            Buffer* pBuf = Buffer::Cast(buf);
            return pBuf->toValue(encoding, retVal);
        } else
            retVal = buf;
    }

    return 0;
}

result_t fs_base::readFile(exlib::string fname, exlib::string encoding,
    Variant& retVal, AsyncEvent* ac)
{
    setErrorContext("open", fname);
    return read_file_ext(fname, "r", encoding, retVal, ac);
}

result_t fs_base::readFile(exlib::string fname, v8::Local<v8::Object> options,
    Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        exlib::string encoding;
        GetConfigValue(options, "encoding", encoding);
        ac->m_ctx[0] = encoding;

        exlib::string flag = "r";
        GetConfigValue(options, "flag", flag);
        ac->m_ctx[1] = flag;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return read_file_ext(fname, ac->m_ctx[1].string(), ac->m_ctx[0].string(), retVal, ac);
}

result_t fs_base::readLines(exlib::string fname, int32_t maxlines,
    v8::Local<v8::Array>& retVal)
{
    obj_ptr<BufferedStream_base> pFile;
    result_t hr;

    hr = ac_openTextStream(fname, "r", pFile);
    if (hr < 0)
        return hr;

    return pFile->readLines(maxlines, retVal);
}

static result_t open_file_for_write(exlib::string fname, exlib::string flag, int32_t mode,
    obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac)
{
    // keep the zip-aware / sandbox-checked path for the default creation mode
    if (mode == 0666)
        return fs_base::openFile(fname, flag, retVal, ac);

    // an explicit creation mode only applies to the real filesystem
    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr < 0)
        return hr;

    if (!ac->isolate()->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<FileStream> pFile = new FileStream();
    hr = pFile->open(safe_name, flag, mode);
    if (hr < 0)
        return hr;

    retVal = pFile;

    return 0;
}

static result_t write_file_ext(exlib::string fname, Buffer_base* data, exlib::string flag, int32_t mode,
    int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = open_file_for_write(fname, flag, mode, f, ac);
    if (hr < 0)
        return hr;

    retVal = Buffer::Cast(data)->length();
    hr = f->cc_writeBuffer(data);
    f->cc_close();

    return hr;
}

static result_t write_text_file_ext(exlib::string fname, exlib::string txt, exlib::string flag, int32_t mode,
    int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = open_file_for_write(fname, flag, mode, f, ac);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(txt.c_str(), txt.length());

    retVal = (int32_t)txt.length();
    hr = f->cc_writeBuffer(buf);
    f->cc_close();

    return hr;
}

result_t fs_base::writeTextFile(exlib::string fname, exlib::string txt, int32_t& retVal,
    AsyncEvent* ac)
{
    return write_text_file_ext(fname, txt, "w", 0666, retVal, ac);
}

result_t fs_base::writeFile(exlib::string fname, Buffer_base* data, exlib::string opt, int32_t& retVal,
    AsyncEvent* ac)
{
    setErrorContext("open", fname);
    return write_file_ext(fname, data, "w", 0666, retVal, ac);
}

result_t fs_base::writeFile(exlib::string fname, Buffer_base* data, v8::Local<v8::Object> options, int32_t& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        exlib::string flag = "w";
        GetConfigValue(options, "flag", flag);
        ac->m_ctx[0] = flag;

        int32_t mode = 0666;
        GetConfigValue(options, "mode", mode);
        if (mode < 0 || mode > 07777)
            return CHECK_ERROR(CALL_E_INVALIDARG);
        ac->m_ctx[1] = mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return write_file_ext(fname, data, ac->m_ctx[0].string(), ac->m_ctx[1].intVal(), retVal, ac);
}

result_t fs_base::writeFile(exlib::string fname, exlib::string data, exlib::string opt, int32_t& retVal, AsyncEvent* ac)
{
    setErrorContext("open", fname);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = commonEncode(opt, data, data);
    if (hr < 0)
        return hr;

    return write_text_file_ext(fname, data, "w", 0666, retVal, ac);
}

result_t fs_base::writeFile(exlib::string fname, exlib::string data, v8::Local<v8::Object> options, int32_t& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr;

        ac->m_ctx.resize(3);

        exlib::string encoding = "utf8";
        hr = GetConfigValue(options, "encoding", encoding, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx[0] = encoding;

        exlib::string flag = "w";
        GetConfigValue(options, "flag", flag);
        ac->m_ctx[1] = flag;

        int32_t mode = 0666;
        GetConfigValue(options, "mode", mode);
        if (mode < 0 || mode > 07777)
            return CHECK_ERROR(CALL_E_INVALIDARG);
        ac->m_ctx[2] = mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string strData = data;
    result_t hr = commonEncode(ac->m_ctx[0].string(), strData, strData);
    if (hr < 0)
        return hr;

    return write_text_file_ext(fname, strData, ac->m_ctx[1].string(), ac->m_ctx[2].intVal(), retVal, ac);
}

static result_t append_file_ext(exlib::string fname, exlib::string flag, int32_t mode, Buffer_base* data,
    int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = open_file_for_write(fname, flag, mode, f, ac);
    if (hr < 0)
        return hr;

    retVal = Buffer::Cast(data)->length();
    hr = f->cc_writeBuffer(data);
    f->cc_close();

    return hr;
}

static result_t read_file_fd(FileHandle_base* fd, exlib::string encoding, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    result_t hr = fd->get_fd(_fd);
    if (hr < 0)
        return hr;

    if (_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    // Node.js: read the whole content, the descriptor is neither closed nor repositioned
    exlib::string strBuf;
    char tmp[STREAM_BUFF_SIZE];

    while (true) {
        int32_t n = (int32_t)::_read(_fd, tmp, STREAM_BUFF_SIZE);
        if (n < 0)
            return CHECK_ERROR(LastError());
        if (n == 0)
            break;
        strBuf.append(tmp, n);
    }

    if (encoding == "") {
        retVal = new Buffer(strBuf.c_str(), strBuf.length());
        return 0;
    }

    obj_ptr<Buffer_base> buf = new Buffer(strBuf.c_str(), strBuf.length());
    return Buffer::Cast(buf)->toValue(encoding, retVal);
}

static result_t write_file_fd(FileHandle_base* fd, Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    result_t hr = fd->get_fd(_fd);
    if (hr < 0)
        return hr;

    if (_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t n = Buffer::Cast(data)->length();

    // Node.js: writeFile(fd) replaces the content of the file
    if (_lseeki64(_fd, 0, SEEK_SET) < 0)
        return CHECK_ERROR(LastError());

    {
        AutoReq req;
        int32_t ret = uv_fs_ftruncate(NULL, &req, _fd, 0, NULL);
        if (ret < 0)
            return ret;
    }

    size_t pos = 0;
    const uint8_t* p = (const uint8_t*)Buffer::Cast(data)->data();

    while (pos < (size_t)n) {
        int32_t len = (int32_t)::_write(_fd, p + pos, (size_t)n - pos);
        if (len < 0)
            return CHECK_ERROR(LastError());
        pos += len;
    }

    retVal = n;

    return 0;
}

result_t fs_base::readFile(FileHandle_base* fd, exlib::string encoding, Variant& retVal, AsyncEvent* ac)
{
    return read_file_fd(fd, encoding, retVal, ac);
}

result_t fs_base::readFile(FileHandle_base* fd, v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        exlib::string encoding = "utf8";
        GetConfigValue(options, "encoding", encoding);
        ac->m_ctx[0] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return read_file_fd(fd, ac->m_ctx[0].string(), retVal, ac);
}

result_t fs_base::writeFile(FileHandle_base* fd, Buffer_base* data, exlib::string opt, int32_t& retVal, AsyncEvent* ac)
{
    return write_file_fd(fd, data, retVal, ac);
}

result_t fs_base::writeFile(FileHandle_base* fd, Buffer_base* data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    return write_file_fd(fd, data, retVal, ac);
}

result_t fs_base::writeFile(FileHandle_base* fd, exlib::string data, exlib::string opt, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = commonEncode(opt, data, data);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(data.c_str(), data.length());

    return write_file_fd(fd, buf, retVal, ac);
}

result_t fs_base::writeFile(FileHandle_base* fd, exlib::string data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr;

        ac->m_ctx.resize(1);

        exlib::string encoding = "utf8";
        hr = GetConfigValue(options, "encoding", encoding, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx[0] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string strData = data;
    result_t hr = commonEncode(ac->m_ctx[0].string(), strData, strData);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(strData.c_str(), strData.length());

    return write_file_fd(fd, buf, retVal, ac);
}

result_t fs_base::appendFile(exlib::string fname, Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    setErrorContext("open", fname);
    return append_file_ext(fname, "a", 0666, data, retVal, ac);
}

result_t fs_base::appendFile(exlib::string fname, Buffer_base* data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        exlib::string flag = "a";
        GetConfigValue(options, "flag", flag);
        ac->m_ctx[0] = flag;

        int32_t mode = 0666;
        GetConfigValue(options, "mode", mode);
        if (mode < 0 || mode > 07777)
            return CHECK_ERROR(CALL_E_INVALIDARG);
        ac->m_ctx[1] = mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return append_file_ext(fname, ac->m_ctx[0].string(), ac->m_ctx[1].intVal(), data, retVal, ac);
}

result_t fs_base::appendFile(exlib::string fname, Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return append_file_ext(fname, "a", 0666, data, retVal, ac);
}

result_t fs_base::appendFile(FileHandle_base* fd, Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    result_t hr = fd->get_fd(_fd);
    if (hr < 0)
        return hr;

    if (_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t n = Buffer::Cast(data)->length();

    size_t pos = 0;
    const uint8_t* p = (const uint8_t*)Buffer::Cast(data)->data();

    while (pos < (size_t)n) {
        int32_t len = (int32_t)::_write(_fd, p + pos, (size_t)n - pos);
        if (len < 0)
            return CHECK_ERROR(LastError());
        pos += len;
    }

    retVal = n;

    return 0;
}

result_t fs_base::appendFile(FileHandle_base* fd, Buffer_base* data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    return appendFile(fd, data, retVal, ac);
}

result_t fs_base::appendFile(FileHandle_base* fd, Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    return appendFile(fd, data, retVal, ac);
}

result_t fs_base::appendFile(FileHandle_base* fd, exlib::string data, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf = new Buffer(data.c_str(), data.length());

    return appendFile(fd, buf, retVal, ac);
}

result_t fs_base::appendFile(FileHandle_base* fd, exlib::string data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    return appendFile(fd, data, retVal, ac);
}

result_t fs_base::appendFile(FileHandle_base* fd, exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strData = data;
    result_t hr = commonEncode(encoding, strData, strData);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(strData.c_str(), strData.length());

    return appendFile(fd, buf, retVal, ac);
}

result_t fs_base::read(FileHandle_base* fd, Buffer_base* buffer, int32_t offset, int32_t length,
    int32_t position, int32_t& retVal, AsyncEvent* ac)
{
    int32_t _fd;
    fd->get_fd(_fd);

    if (_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t bufLength = Buffer::Cast(buffer)->length();

    if (offset < 0 || offset >= bufLength)
        return Runtime::setError("fs: Offset is out of bounds");

    if (length < 0 || (offset + length > bufLength)) {
        return Runtime::setError("fs: Length extends beyond buffer");
    }

    if (position > -1) {
        if (_lseeki64(_fd, position, SEEK_SET) < 0)
            return CHECK_ERROR(LastError());
    }

    exlib::string strBuf;
    if (length > 0) {
        strBuf.resize(length);
        int32_t sz = length;
        char* p = strBuf.data();

        while (sz) {
            int32_t n = (int32_t)::_read(_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
            if (n < 0)
                return CHECK_ERROR(LastError());
            if (n == 0)
                break;

            sz -= n;
            p += n;
        }

        strBuf.resize(length - sz);
    }

    if (strBuf.length() == 0) {
        retVal = 0;
        return 0;
    }

    return buffer->write(strBuf, offset, (int32_t)strBuf.length(), "utf8", retVal);
}

result_t fs_base::write(FileHandle_base* fd, Buffer_base* buffer, int32_t offset, int32_t length,
    int32_t position, int32_t& retVal, AsyncEvent* ac)
{
    int32_t _fd;
    fd->get_fd(_fd);

    if (_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t bufLength = Buffer::Cast(buffer)->length();

    if (offset < 0 || offset >= bufLength)
        return Runtime::setError("fs: Offset is out of bounds");

    if (offset + length > bufLength)
        return Runtime::setError("fs: Length extends beyond buffer");

    if (length < 0)
        length = bufLength - offset;

    if (position > -1) {
        if (_lseeki64(_fd, position, SEEK_SET) < 0)
            return CHECK_ERROR(LastError());
    }

    if (length > 0) {
        Buffer* buf = Buffer::Cast(buffer);
        int32_t sz = length;
        const uint8_t* p = buf->data() + offset;

        while (sz) {
            int32_t n = (int32_t)::_write(_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
            if (n < 0)
                return CHECK_ERROR(LastError());

            sz -= n;
            p += n;
        }
    }

    retVal = length;

    return 0;
}

result_t fs_base::write(FileHandle_base* fd, exlib::string string, int32_t position,
    exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    int32_t _fd;
    fd->get_fd(_fd);

    if (_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;

    result_t hr = encoding_conv(encoding).encode(string, buf);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return write(fd, buf, 0, -1, position, retVal, ac);
}

result_t fs_base::fstat(FileHandle_base* fd, obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    setErrorContext("fstat");
    int32_t _fd;
    fd->get_fd(_fd);

    if (_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    AutoReq req;
    int32_t ret = uv_fs_fstat(NULL, &req, _fd, NULL);
    if (ret < 0)
        return ret;

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill("", &req.statbuf);
    retVal = pStat;

    return 0;
}

result_t fs_base::fstat(FileHandle_base* fd, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    return fstat(fd, retVal, ac);
}

result_t fs_base::exists(exlib::string path, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    retVal = uv_fs_access(NULL, &req, path.c_str(), F_OK, NULL) == 0;
    return 0;
}

// Node.js compatibility: the options argument is accepted and ignored
result_t fs_base::exists(exlib::string path, v8::Local<v8::Object> options, bool& retVal, AsyncEvent* ac)
{
    return exists(path, retVal, ac);
}

result_t fs_base::access(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    setErrorContext("access", path);
    if (ac->isSync()) {
        // Node.js compatibility: mode is a bitmask of F_OK/R_OK/W_OK/X_OK
        if (mode < 0 || mode > 7)
            return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG,
                "mode is out of range: >= 0 && <= 7"));

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    return uv_fs_access(NULL, &req, path.c_str(), mode, NULL);
}

result_t fs_base::link(exlib::string oldPath, exlib::string newPath, AsyncEvent* ac)
{
    setErrorContext("link", oldPath);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(oldPath, oldPath);
    if (hr < 0)
        return hr;

    hr = normalize_file_path_like(newPath, newPath);
    if (hr < 0)
        return hr;

    AutoReq req;
    return uv_fs_link(NULL, &req, oldPath.c_str(), newPath.c_str(), NULL);
}

result_t fs_base::unlink(exlib::string path, AsyncEvent* ac)
{
    setErrorContext("unlink", path);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    return uv_fs_unlink(NULL, &req, path.c_str(), NULL);
}

result_t fs_base::symlink(exlib::string target, exlib::string linkpath, exlib::string type, AsyncEvent* ac)
{
    setErrorContext("symlink", linkpath);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(target, target);
    if (hr < 0)
        return hr;

    hr = normalize_file_path_like(linkpath, linkpath);
    if (hr < 0)
        return hr;

    int _type = 0;

    if (type == "dir")
        _type = 1;
    else if (type == "junction")
        _type = 2;

    AutoReq req;
    return uv_fs_symlink(NULL, &req, target.c_str(), linkpath.c_str(), _type, NULL);
}

// Node.js: convert a raw path (utf8 bytes) to the requested encoding
static result_t path_to_variant(const exlib::string& path, const exlib::string& encoding, Variant& retVal)
{
    if (encoding.empty() || encoding == "utf8" || encoding == "utf-8") {
        retVal = path;
        return 0;
    }

    obj_ptr<Buffer_base> buf = new Buffer(path.c_str(), path.length());

    if (encoding == "buffer") {
        retVal = buf;
        return 0;
    }

    exlib::string str;
    result_t hr = buf->toString(encoding, 0, str);
    if (hr < 0)
        return hr;

    retVal = str;

    return 0;
}

result_t fs_base::readlink(exlib::string path, Variant& retVal, AsyncEvent* ac)
{
    setErrorContext("readlink", path);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    int32_t ret = uv_fs_readlink(NULL, &req, path.c_str(), NULL);
    if (ret < 0)
        return ret;

    retVal = (const char*)req.ptr;
    return 0;
}

result_t fs_base::readlink(exlib::string path, v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        exlib::string encoding = "utf8";
        GetConfigValue(options, "encoding", encoding);
        ac->m_ctx[0] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return readlink(path, ac->m_ctx[0].string(), retVal, ac);
}

result_t fs_base::readlink(exlib::string path, exlib::string encoding, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    Variant link;
    result_t hr = readlink(path, link, ac);
    if (hr < 0)
        return hr;

    return path_to_variant(link.string(), encoding, retVal);
}

result_t fs_base::realpath(exlib::string path, Variant& retVal, AsyncEvent* ac)
{
    setErrorContext("lstat", path);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = coerce_file_path_like(path, path);
    if (hr < 0)
        return hr;

    exlib::string resolved;

    // First resolve to absolute path and normalize
    bool isAbs = false;
    path_base::isAbsolute(path, isAbs);
    if (!isAbs)
        os_resolve(path);
    path_base::normalize(path, resolved);

    // Loop to resolve symlinks (with a limit to prevent infinite loops)
    const int maxLinks = 40;
    int linkCount = 0;

    // Start from root and walk down the path, resolving symlinks at each step
    exlib::string current;
    size_t pos = 0;

#ifdef _WIN32
    // Handle Windows drive letter or UNC path
    if (resolved.length() >= 2 && resolved[1] == ':') {
        current = resolved.substr(0, 3); // e.g., "C:\"
        pos = 3;
    } else if (resolved.length() >= 2 && resolved[0] == '\\' && resolved[1] == '\\') {
        // UNC path
        size_t slash = resolved.find('\\', 2);
        if (slash != exlib::string::npos) {
            slash = resolved.find('\\', slash + 1);
            if (slash != exlib::string::npos) {
                current = resolved.substr(0, slash + 1);
                pos = slash + 1;
            }
        }
    }
#else
    // Unix: start from root
    if (resolved.length() > 0 && resolved[0] == '/') {
        current = "/";
        pos = 1;
    }
#endif

    while (pos < resolved.length()) {
        // Find next path separator
        size_t nextSep = resolved.find(PATH_SLASH, pos);
        exlib::string component;

        if (nextSep == exlib::string::npos) {
            component = resolved.substr(pos);
            pos = resolved.length();
        } else {
            component = resolved.substr(pos, nextSep - pos);
            pos = nextSep + 1;
        }

        if (component.empty() || component == ".")
            continue;

        // Build path to current component
        exlib::string testPath = current;
        if (!testPath.empty() && testPath[testPath.length() - 1] != PATH_SLASH)
            testPath += PATH_SLASH;
        testPath += component;

        // Check if this component is a symlink
        obj_ptr<Stat_base> stat;
        hr = cc_lstat(testPath, stat, ac->isolate());
        if (hr < 0)
            return hr;

        bool isSymlink = false;
        stat->isSymbolicLink(isSymlink);

        if (isSymlink) {
            if (++linkCount > maxLinks)
                return CALL_E_FILE_NOT_FOUND; // Too many symlinks

            // Read the symlink target
            Variant linkValue;
            hr = cc_readlink(testPath, linkValue, ac->isolate());
            if (hr < 0)
                return hr;

            exlib::string linkTarget = linkValue.string();

            // Resolve the link target
            path_base::isAbsolute(linkTarget, isAbs);
            if (isAbs) {
                current = linkTarget;
            } else {
                resolvePath(current, linkTarget);
            }
            path_base::normalize(current, current);

            // Append remaining path and restart resolution
            if (pos < resolved.length()) {
                exlib::string remaining = resolved.substr(pos);
                resolvePath(current, remaining);
                path_base::normalize(current, resolved);
                pos = 0;
#ifdef _WIN32
                if (resolved.length() >= 2 && resolved[1] == ':') {
                    current = resolved.substr(0, 3);
                    pos = 3;
                }
#else
                if (resolved.length() > 0 && resolved[0] == '/') {
                    current = "/";
                    pos = 1;
                }
#endif
            }
        } else {
            current = testPath;
        }
    }

    // Remove trailing slash (except for root paths)
    size_t len = current.length();
#ifdef _WIN32
    // On Windows, keep trailing slash only for drive root like "C:\"
    if (len > 3 && (current[len - 1] == '\\' || current[len - 1] == '/'))
        current.resize(len - 1);
#else
    // On Unix, keep trailing slash only for root "/"
    if (len > 1 && current[len - 1] == '/')
        current.resize(len - 1);
#endif

    retVal = current;
    return 0;
}

result_t fs_base::realpath(exlib::string path, v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        exlib::string encoding = "utf8";
        GetConfigValue(options, "encoding", encoding);
        ac->m_ctx[0] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return realpath(path, ac->m_ctx[0].string(), retVal, ac);
}

result_t fs_base::realpath(exlib::string path, exlib::string encoding, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    Variant resolved;
    result_t hr = realpath(path, resolved, ac);
    if (hr < 0)
        return hr;

    return path_to_variant(resolved.string(), encoding, retVal);
}

result_t fs_base::mkdir(exlib::string path, int32_t mode, Variant& retVal, AsyncEvent* ac)
{
    setErrorContext("mkdir", path);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    return uv_fs_mkdir(NULL, &req, path.c_str(), mode, NULL);
}

result_t fs_base::mkdir(exlib::string path, Variant mode, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        int32_t _mode;
        result_t hr = to_mode_value(mode, _mode);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return mkdir(path, ac->m_ctx[0].intVal(), retVal, ac);
}

result_t fs_base::mkdir(exlib::string path, v8::Local<v8::Object> opt, Variant& retVal, AsyncEvent* ac)
{
    setErrorContext("mkdir", path);
    class AsyncUVMKDir : public uv_fs_t {
    public:
        AsyncUVMKDir(exlib::string path, int32_t mode, Variant* retVal, AsyncEvent* ac)
            : m_ac(ac)
            , m_retVal(retVal)
            , m_path(path)
            , m_mode(mode)
        {
        }

        ~AsyncUVMKDir()
        {
            uv_fs_req_cleanup(this);
        }

    public:
        static void cb_stat(uv_fs_t* req)
        {
            AsyncUVMKDir* pThis = (AsyncUVMKDir*)req;

            int32_t ret = (int32_t)uv_fs_get_result(req);
            if (ret < 0 || !S_ISDIR(pThis->statbuf.st_mode)) {
                pThis->m_ac->apost(pThis->m_last_err);
                delete pThis;
                return;
            }

            // the directory already existed, it must not be reported as created
            pThis->m_existed = true;
            pThis->result = 0;
            cb_mkdir(req);
        }

        static void cb_mkdir(uv_fs_t* req)
        {
            AsyncUVMKDir* pThis = (AsyncUVMKDir*)req;

            int32_t ret = (int32_t)uv_fs_get_result(req);
            switch (ret) {
            case 0:
                if (pThis->m_existed) {
                    pThis->m_existed = false;
                } else if (pThis->m_first.empty()) {
                    // Node.js: the first directory created is returned to the caller
                    pThis->m_first = pThis->m_path;
                }

                if (pThis->m_paths.size() == 0) {
                    // nothing was created: Node.js reports undefined
                    if (pThis->m_retVal && !pThis->m_first.empty())
                        *pThis->m_retVal = pThis->m_first;
                    pThis->m_ac->apost(0);
                    delete pThis;
                    return;
                }

                pThis->m_path = pThis->m_paths.back();
                pThis->m_paths.pop_back();
                break;
            case UV_EACCES:
            case UV_ENOTDIR:
            case UV_EPERM:
                pThis->m_ac->apost(ret);
                delete pThis;
                return;
            case UV_ENOENT:
                pThis->m_paths.push_back(pThis->m_path);
                os_dirname(pThis->m_path, pThis->m_path);
                break;
            default:
                pThis->m_last_err = ret;
                uv_fs_req_cleanup(pThis);
                ret = uv_fs_stat(s_uv_loop, pThis, pThis->m_path.c_str(), cb_stat);
                if (ret != 0) {
                    pThis->m_ac->apost(pThis->m_last_err);
                    delete pThis;
                }
                return;
            };

            uv_fs_req_cleanup(pThis);
            ret = uv_fs_mkdir(s_uv_loop, pThis, pThis->m_path.c_str(), pThis->m_mode, AsyncUVMKDir::cb_mkdir);
            if (ret != 0) {
                pThis->m_ac->apost(ret);
                delete pThis;
            }
        }

    private:
        AsyncEvent* m_ac;
        Variant* m_retVal;
        exlib::string m_path;
        exlib::string m_first;
        bool m_existed = false;
        int32_t m_mode;
        std::vector<exlib::string> m_paths;
        int32_t m_last_err;
    };

    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        bool recursive = false;
        GetConfigValue(opt, "recursive", recursive);
        ac->m_ctx[0] = recursive;

        int32_t mode = 0777;
        GetConfigValue(opt, "mode", mode);
        ac->m_ctx[1] = mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    bool recursive = ac->m_ctx[0].boolVal();
    int32_t mode = ac->m_ctx[1].intVal();

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    if (!recursive)
        return mkdir(path, mode, retVal, ac);

    os_resolve(path);

    return uv_async([&] {
        return uv_fs_mkdir(s_uv_loop, new AsyncUVMKDir(path, mode, &retVal, ac), path.c_str(), mode, AsyncUVMKDir::cb_mkdir);
    });
}

class AsyncUVRM : public uv_fs_t {
public:
    AsyncUVRM(exlib::string path, bool rmFile, bool force, AsyncEvent* ac)
        : m_ac(ac)
        , m_path(path)
        , m_rmFile(rmFile)
        , m_force(force)
    {
    }

    ~AsyncUVRM()
    {
        uv_fs_req_cleanup(this);
    }

public:
    static void cb_stat(uv_fs_t* req)
    {
        AsyncUVRM* pThis = (AsyncUVRM*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        if (ret < 0) {
            // Path doesn't exist or other error
            if (pThis->m_force && ret == UV_ENOENT) {
                // force: silently ignore nonexistent paths (Node.js behavior)
                pThis->m_ac->apost(0);
            } else {
                pThis->m_ac->apost(ret);
            }
            delete pThis;
            return;
        }

        // Check if it's a regular file
        if (S_ISREG(pThis->statbuf.st_mode)) {
            if (!pThis->m_rmFile) {
                // rmdir does not delete files
                pThis->m_ac->apost(UV_ENOTDIR);
                delete pThis;
                return;
            }
            // It's a file, remove it directly
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_unlink(s_uv_loop, pThis, pThis->m_path.c_str(), cb_unlink);
            if (ret != 0) {
                pThis->m_ac->apost(ret);
                delete pThis;
            }
            return;
        } else if (S_ISDIR(pThis->statbuf.st_mode)) {
            // It's a directory, scan contents first
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_scandir(s_uv_loop, pThis, pThis->m_path.c_str(), 0, cb_scandir);
            if (ret != 0) {
                pThis->m_ac->apost(ret);
                delete pThis;
            }
            return;
        } else {
            // Symlinks and other special files: unlink directly.
            // Node.js lstats entries and removes symlinks without
            // following them, so recursive removal must not touch
            // the symlink target.
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_unlink(s_uv_loop, pThis, pThis->m_path.c_str(), cb_unlink);
            if (ret != 0) {
                pThis->m_ac->apost(ret);
                delete pThis;
            }
            return;
        }
    }

    static void cb_scandir(uv_fs_t* req)
    {
        AsyncUVRM* pThis = (AsyncUVRM*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        if (ret < 0) {
            pThis->m_ac->apost(ret);
            delete pThis;
            return;
        }

        // Collect all entries that need to be removed
        uv_dirent_t dirent;
        while (uv_fs_scandir_next(req, &dirent) != UV_EOF) {
            pThis->m_entries.push_back(std::make_pair(dirent.name, dirent.type));
        }

        // Start removing entries
        pThis->remove_next_entry();
    }

    static void cb_entry_removed(uv_fs_t* req)
    {
        AsyncUVRM* pThis = (AsyncUVRM*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        if (ret < 0) {
            pThis->m_ac->apost(ret);
            delete pThis;
            return;
        }

        // Continue removing next entry
        pThis->remove_next_entry();
    }

    static void cb_rmdir(uv_fs_t* req)
    {
        AsyncUVRM* pThis = (AsyncUVRM*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        pThis->m_ac->apost(ret);
        delete pThis;
    }

    static void cb_unlink(uv_fs_t* req)
    {
        AsyncUVRM* pThis = (AsyncUVRM*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        pThis->m_ac->apost(ret);
        delete pThis;
    }

    void remove_next_entry()
    {
        if (m_entries.empty()) {
            // All entries removed, now remove the directory itself
            uv_fs_req_cleanup(this);
            int32_t ret = uv_fs_rmdir(s_uv_loop, this, m_path.c_str(), cb_rmdir);
            if (ret != 0) {
                m_ac->apost(ret);
                delete this;
            }
            return;
        }

        // Get next entry to remove
        auto entry = m_entries.back();
        m_entries.pop_back();

        exlib::string entry_path = m_path + PATH_SLASH + entry.first;

        uv_fs_req_cleanup(this);

        if (entry.second == UV_DIRENT_DIR) {
            // For directories, create a new AsyncUVRM instance
            AsyncUVRM* subRemover = new AsyncUVRM(entry_path, m_rmFile, m_force, new SubDirEvent(this));
            int32_t ret = uv_fs_stat(s_uv_loop, subRemover, entry_path.c_str(), cb_stat);
            if (ret != 0) {
                m_ac->apost(ret);
                delete subRemover;
                delete this;
            }
        } else {
            // For files, unlink directly
            int32_t ret = uv_fs_unlink(s_uv_loop, this, entry_path.c_str(), cb_entry_removed);
            if (ret != 0) {
                m_ac->apost(ret);
                delete this;
            }
        }
    }

    class SubDirEvent : public AsyncEvent {
    public:
        SubDirEvent(AsyncUVRM* parent)
            : m_parent(parent)
        {
        }

        virtual void apost(int32_t hr) override
        {
            if (hr < 0) {
                m_parent->m_ac->apost(hr);
                delete m_parent;
            } else {
                m_parent->remove_next_entry();
            }
            delete this;
        }

    private:
        AsyncUVRM* m_parent;
    };

private:
    AsyncEvent* m_ac;
    exlib::string m_path;
    bool m_rmFile;
    bool m_force;
    std::vector<std::pair<exlib::string, uv_dirent_type_t>> m_entries;
};

result_t fs_base::mkdtemp(exlib::string prefix, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Append XXXXXX template suffix as required by mkdtemp()
    exlib::string tpl = prefix + "XXXXXX";
    AutoReq req;
    int32_t ret = uv_fs_mkdtemp(NULL, &req, tpl.c_str(), NULL);
    if (ret < 0)
        return ret;

    retVal = uv_fs_get_path(&req);
    return 0;
}

result_t fs_base::rmdir(exlib::string path, v8::Local<v8::Object> opt, AsyncEvent* ac)
{
    setErrorContext("rmdir", path);
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        bool recursive = false;
        GetConfigValue(opt, "recursive", recursive);
        ac->m_ctx[0] = recursive;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    bool recursive = ac->m_ctx[0].boolVal();

    if (!recursive) {
        AutoReq req;
        return uv_fs_rmdir(NULL, &req, path.c_str(), NULL);
    }

    os_resolve(path);

    return uv_async([&] {
        return uv_fs_lstat(s_uv_loop, new AsyncUVRM(path, false, false, ac), path.c_str(), AsyncUVRM::cb_stat);
    });
}

result_t fs_base::rm(exlib::string path, v8::Local<v8::Object> opt, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        bool recursive = false;
        GetConfigValue(opt, "recursive", recursive);
        ac->m_ctx[0] = recursive;

        bool force = false;
        GetConfigValue(opt, "force", force);
        ac->m_ctx[1] = force;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    bool recursive = ac->m_ctx[0].boolVal();
    bool force = ac->m_ctx[1].boolVal();

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    if (!recursive) {
        // Non-recursive rm only removes files and symlinks.
        // Match Node.js: directories throw EISDIR and nonexistent
        // paths are ignored when force is true.
        AutoReq req;
        int32_t ret = uv_fs_lstat(NULL, &req, path.c_str(), NULL);
        if (ret < 0) {
            if (force && ret == UV_ENOENT)
                return 0;
            return ret;
        }

        if (S_ISDIR(req.statbuf.st_mode)) {
            uv_fs_req_cleanup(&req);
            return UV_EISDIR;
        }

        uv_fs_req_cleanup(&req);
        return uv_fs_unlink(NULL, &req, path.c_str(), NULL);
    }

    os_resolve(path);

    return uv_async([&] {
        return uv_fs_lstat(s_uv_loop, new AsyncUVRM(path, true, force, ac), path.c_str(), AsyncUVRM::cb_stat);
    });
}

result_t fs_base::fchmod(FileHandle_base* fd, int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    fd->get_fd(_fd);

    AutoReq req;
    return uv_fs_fchmod(NULL, &req, _fd, mode, NULL);
}

result_t fs_base::fchown(FileHandle_base* fd, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    fd->get_fd(_fd);

    AutoReq req;
    return uv_fs_fchown(NULL, &req, _fd, uid, gid, NULL);
}

result_t fs_base::fsync(FileHandle_base* fd, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    fd->get_fd(_fd);

    AutoReq req;
    return uv_fs_fsync(NULL, &req, _fd, NULL);
}

result_t fs_base::ftruncate(FileHandle_base* fd, int32_t len, AsyncEvent* ac)
{
    setErrorContext("ftruncate");

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    result_t hr = fd->get_fd(_fd);
    if (hr < 0)
        return hr;

    // Node.js compatibility: a negative length is treated as zero
    if (len < 0)
        len = 0;

    AutoReq req;
    return uv_fs_ftruncate(NULL, &req, _fd, len, NULL);
}

result_t fs_base::statfs(exlib::string path, obj_ptr<NObject>& retVal, AsyncEvent* ac)
{
    setErrorContext("statfs", path);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    int32_t ret = uv_fs_statfs(NULL, &req, path.c_str(), NULL);
    if (ret < 0)
        return ret;

    uv_statfs_t* st = (uv_statfs_t*)req.ptr;

    obj_ptr<NObject> obj = new NObject();
    obj->add("type", (double)st->f_type);
    obj->add("bsize", (double)st->f_bsize);
    obj->add("blocks", (double)st->f_blocks);
    obj->add("bfree", (double)st->f_bfree);
    obj->add("bavail", (double)st->f_bavail);
    obj->add("files", (double)st->f_files);
    obj->add("ffree", (double)st->f_ffree);

    retVal = obj;

    return 0;
}

result_t fs_base::chmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    setErrorContext("chmod", path);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    AutoReq req;
    return uv_fs_chmod(NULL, &req, path.c_str(), mode, NULL);
}

result_t fs_base::chmod(exlib::string path, Variant mode, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        int32_t _mode;
        result_t hr = to_mode_value(mode, _mode);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return chmod(path, ac->m_ctx[0].intVal(), ac);
}

result_t fs_base::lchmod(exlib::string path, Variant mode, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        int32_t _mode;
        result_t hr = to_mode_value(mode, _mode);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return lchmod(path, ac->m_ctx[0].intVal(), ac);
}

result_t fs_base::chown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    setErrorContext("chown", path);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    AutoReq req;
    return uv_fs_chown(NULL, &req, path.c_str(), uid, gid, NULL);
}

result_t fs_base::lchown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    AutoReq req;
    return uv_fs_lchown(NULL, &req, path.c_str(), uid, gid, NULL);
}

result_t fs_base::utimes(exlib::string path, Variant atime, Variant mtime, AsyncEvent* ac)
{
    setErrorContext("utime", path);
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        double _atime, _mtime;
        result_t hr = to_unix_timestamp(atime, _atime);
        if (hr < 0)
            return hr;

        hr = to_unix_timestamp(mtime, _mtime);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _atime;
        ac->m_ctx[1] = _mtime;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    AutoReq req;
    return uv_fs_utime(NULL, &req, path.c_str(), ac->m_ctx[0].dblVal(), ac->m_ctx[1].dblVal(), NULL);
}

result_t fs_base::lutimes(exlib::string path, Variant atime, Variant mtime, AsyncEvent* ac)
{
    setErrorContext("lutime", path);
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        double _atime, _mtime;
        result_t hr = to_unix_timestamp(atime, _atime);
        if (hr < 0)
            return hr;

        hr = to_unix_timestamp(mtime, _mtime);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _atime;
        ac->m_ctx[1] = _mtime;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    AutoReq req;
    return uv_fs_lutime(NULL, &req, path.c_str(), ac->m_ctx[0].dblVal(), ac->m_ctx[1].dblVal(), NULL);
}

result_t fs_base::futimes(FileHandle_base* fd, Variant atime, Variant mtime, AsyncEvent* ac)
{
    setErrorContext("futime");
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        double _atime, _mtime;
        result_t hr = to_unix_timestamp(atime, _atime);
        if (hr < 0)
            return hr;

        hr = to_unix_timestamp(mtime, _mtime);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _atime;
        ac->m_ctx[1] = _mtime;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    int32_t _fd;
    result_t hr = fd->get_fd(_fd);
    if (hr < 0)
        return hr;

    AutoReq req;
    return uv_fs_futime(NULL, &req, _fd, ac->m_ctx[0].dblVal(), ac->m_ctx[1].dblVal(), NULL);
}

result_t fs_base::rename(exlib::string from, exlib::string to, AsyncEvent* ac)
{
    setErrorContext("rename", from);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(from, from);
    if (hr < 0)
        return hr;

    hr = normalize_file_path_like(to, to);
    if (hr < 0)
        return hr;

    AutoReq req;
    return uv_fs_rename(NULL, &req, from.c_str(), to.c_str(), NULL);
}

result_t fs_base::fdatasync(FileHandle_base* fd, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    fd->get_fd(_fd);

    AutoReq req;
    return uv_fs_fdatasync(NULL, &req, _fd, NULL);
}

result_t fs_base::copyFile(exlib::string from, exlib::string to, int32_t mode, AsyncEvent* ac)
{
    setErrorContext("copyfile", from);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(from, from);
    if (hr < 0)
        return hr;

    hr = normalize_file_path_like(to, to);
    if (hr < 0)
        return hr;

    AutoReq req;
    return uv_fs_copyfile(NULL, &req, from.c_str(), to.c_str(), mode, NULL);
}

class AsyncUVCP : public uv_fs_t {
public:
    AsyncUVCP(exlib::string src, exlib::string dest, bool recursive, bool force, bool errorOnExist, int32_t mode, AsyncEvent* ac)
        : m_ac(ac)
        , m_src(src)
        , m_dest(dest)
        , m_recursive(recursive)
        , m_force(force)
        , m_errorOnExist(errorOnExist)
        , m_mode(mode)
    {
    }

    ~AsyncUVCP()
    {
        uv_fs_req_cleanup(this);
    }

public:
    static void cb_stat(uv_fs_t* req)
    {
        AsyncUVCP* pThis = (AsyncUVCP*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        if (ret < 0) {
            pThis->m_ac->apost(ret);
            delete pThis;
            return;
        }

        if (S_ISDIR(pThis->statbuf.st_mode)) {
            if (!pThis->m_recursive) {
                pThis->m_ac->apost(UV_EISDIR);
                delete pThis;
                return;
            }

            // Create destination directory
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_mkdir(s_uv_loop, pThis, pThis->m_dest.c_str(), pThis->statbuf.st_mode & 0777, cb_mkdir);
            if (ret != 0) {
                pThis->m_ac->apost(ret);
                delete pThis;
            }
        } else if (S_ISREG(pThis->statbuf.st_mode)) {
            // Copy file directly
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_copyfile(s_uv_loop, pThis, pThis->m_src.c_str(), pThis->m_dest.c_str(),
                pThis->m_mode, cb_copyfile);
            if (ret != 0) {
                pThis->m_ac->apost(ret);
                delete pThis;
            }
        } else {
            // Symlinks and other types: try copyfile
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_copyfile(s_uv_loop, pThis, pThis->m_src.c_str(), pThis->m_dest.c_str(),
                pThis->m_mode, cb_copyfile);
            if (ret != 0) {
                pThis->m_ac->apost(ret);
                delete pThis;
            }
        }
    }

    static void cb_mkdir(uv_fs_t* req)
    {
        AsyncUVCP* pThis = (AsyncUVCP*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        if (ret < 0 && ret != UV_EEXIST) {
            pThis->m_ac->apost(ret);
            delete pThis;
            return;
        }

        // Scan source directory
        uv_fs_req_cleanup(pThis);
        ret = uv_fs_scandir(s_uv_loop, pThis, pThis->m_src.c_str(), 0, cb_scandir);
        if (ret != 0) {
            pThis->m_ac->apost(ret);
            delete pThis;
        }
    }

    static void cb_scandir(uv_fs_t* req)
    {
        AsyncUVCP* pThis = (AsyncUVCP*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        if (ret < 0) {
            pThis->m_ac->apost(ret);
            delete pThis;
            return;
        }

        uv_dirent_t dirent;
        while (uv_fs_scandir_next(req, &dirent) != UV_EOF) {
            pThis->m_entries.push_back(std::make_pair(dirent.name, dirent.type));
        }

        pThis->copy_next_entry();
    }

    static void cb_copyfile(uv_fs_t* req)
    {
        AsyncUVCP* pThis = (AsyncUVCP*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        // Node.js: with force:false an existing destination is skipped silently
        // unless errorOnExist is set
        if (ret == UV_EEXIST && !pThis->m_force && !pThis->m_errorOnExist)
            ret = 0;

        pThis->m_ac->apost(ret);
        delete pThis;
    }

    static void cb_entry_copied(uv_fs_t* req)
    {
        AsyncUVCP* pThis = (AsyncUVCP*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        // Node.js: with force:false an existing destination is skipped silently
        // unless errorOnExist is set
        if (ret == UV_EEXIST && !pThis->m_force && !pThis->m_errorOnExist) {
            pThis->copy_next_entry();
            return;
        }

        if (ret < 0) {
            pThis->m_ac->apost(ret);
            delete pThis;
            return;
        }

        pThis->copy_next_entry();
    }

    void copy_next_entry()
    {
        if (m_entries.empty()) {
            // All entries copied
            m_ac->apost(0);
            delete this;
            return;
        }

        auto entry = m_entries.back();
        m_entries.pop_back();

        exlib::string src_path = m_src + PATH_SLASH + entry.first;
        exlib::string dest_path = m_dest + PATH_SLASH + entry.first;

        uv_fs_req_cleanup(this);

        if (entry.second == UV_DIRENT_DIR) {
            // For directories, create a new AsyncUVCP instance recursively
            AsyncUVCP* subCopier = new AsyncUVCP(src_path, dest_path, m_recursive, m_force, m_errorOnExist, m_mode, new SubDirEvent(this));
            int32_t ret = uv_fs_stat(s_uv_loop, subCopier, src_path.c_str(), cb_stat);
            if (ret != 0) {
                m_ac->apost(ret);
                delete subCopier;
                delete this;
            }
        } else {
            // For files, copy directly
            int32_t ret = uv_fs_copyfile(s_uv_loop, this, src_path.c_str(), dest_path.c_str(),
                m_mode, cb_entry_copied);
            if (ret != 0) {
                m_ac->apost(ret);
                delete this;
            }
        }
    }

    class SubDirEvent : public AsyncEvent {
    public:
        SubDirEvent(AsyncUVCP* parent)
            : m_parent(parent)
        {
        }

        virtual void apost(int32_t hr) override
        {
            if (hr < 0) {
                m_parent->m_ac->apost(hr);
                delete m_parent;
            } else {
                m_parent->copy_next_entry();
            }
            delete this;
        }

    private:
        AsyncUVCP* m_parent;
    };

private:
    AsyncEvent* m_ac;
    exlib::string m_src;
    exlib::string m_dest;
    bool m_recursive;
    bool m_force;
    bool m_errorOnExist;
    int32_t m_mode;
    std::vector<std::pair<exlib::string, uv_dirent_type_t>> m_entries;
};

result_t fs_base::cp(exlib::string src, exlib::string dest, v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    setErrorContext("copyfile", src);
    if (ac->isSync()) {
        ac->m_ctx.resize(4);

        bool recursive = false;
        GetConfigValue(opts, "recursive", recursive);
        ac->m_ctx[0] = recursive;

        bool force = true;
        GetConfigValue(opts, "force", force);
        ac->m_ctx[1] = force;

        bool errorOnExist = false;
        GetConfigValue(opts, "errorOnExist", errorOnExist);
        ac->m_ctx[2] = errorOnExist;

        int32_t mode = 0;
        GetConfigValue(opts, "mode", mode);
        ac->m_ctx[3] = mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    bool recursive = ac->m_ctx[0].boolVal();
    bool force = ac->m_ctx[1].boolVal();
    bool errorOnExist = ac->m_ctx[2].boolVal();
    int32_t mode = ac->m_ctx[3].intVal();

    result_t hr = normalize_file_path_like(src, src);
    if (hr < 0)
        return hr;

    hr = normalize_file_path_like(dest, dest);
    if (hr < 0)
        return hr;

    if (!force)
        mode |= UV_FS_COPYFILE_EXCL;

    os_resolve(src);
    os_resolve(dest);

    return uv_async([&] {
        return uv_fs_stat(s_uv_loop, new AsyncUVCP(src, dest, recursive, force, errorOnExist, mode, ac), src.c_str(), AsyncUVCP::cb_stat);
    });
}

result_t fs_base::opendir(exlib::string path, obj_ptr<Dir_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new Dir(path);

    return 0;
}

result_t fs_base::readdir(exlib::string path, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    setErrorContext("scandir", path);
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    int32_t ret = uv_fs_scandir(NULL, &req, path.c_str(), 0, NULL);
    if (ret < 0)
        return ret;

    retVal = new NArray();
    uv_dirent_t dirent;

    while (uv_fs_scandir_next(&req, &dirent) != UV_EOF)
        retVal->append(dirent.name);

    return 0;
}

// Node.js: readdir's `encoding` option controls how entry names are returned
static result_t append_dirent_name(obj_ptr<NArray>& list, const char* name, exlib::string& encoding)
{
    if (encoding.empty() || encoding == "utf8" || encoding == "utf-8") {
        list->append(name);
        return 0;
    }

    obj_ptr<Buffer_base> buf = new Buffer(name, qstrlen(name));

    if (encoding == "buffer") {
        list->append(buf);
        return 0;
    }

    exlib::string str;
    result_t hr = buf->toString(encoding, 0, str);
    if (hr < 0)
        return hr;

    list->append(str);

    return 0;
}

static result_t readdir_ext(exlib::string path, bool recursive, bool withFileTypes, exlib::string encoding,
    obj_ptr<NArray>& retVal, AsyncEvent* ac);

result_t fs_base::readdir(exlib::string path, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(3);

        bool recursive = false;
        GetConfigValue(opts, "recursive", recursive);
        ac->m_ctx[0] = recursive;

        bool withFileTypes = false;
        GetConfigValue(opts, "withFileTypes", withFileTypes);
        ac->m_ctx[1] = withFileTypes;

        exlib::string encoding;
        GetConfigValue(opts, "encoding", encoding);
        ac->m_ctx[2] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return readdir_ext(path, ac->m_ctx[0].boolVal(), ac->m_ctx[1].boolVal(), ac->m_ctx[2].string(), retVal, ac);
}

// Node.js: readdir(path, encoding) - names are returned in the given encoding
result_t fs_base::readdir(exlib::string path, exlib::string encoding, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return readdir_ext(path, false, false, encoding, retVal, ac);
}

// shared readdir implementation
static result_t readdir_ext(exlib::string path, bool recursive, bool withFileTypes, exlib::string encoding,
    obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    os_normalize(path, path, true);

    if (withFileTypes && encoding != "" && encoding != "utf8" && encoding != "utf-8")
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG,
            "withFileTypes is not supported with encoding '%s'.", encoding.c_str()));

    QuickArray<exlib::string> paths;
    retVal = new NArray();

    {
        AutoReq req;
        int32_t ret = uv_fs_scandir(NULL, &req, path.c_str(), 0, NULL);
        if (ret < 0)
            return ret;

        uv_dirent_t dirent;
        while (uv_fs_scandir_next(&req, &dirent) != UV_EOF) {
            if (withFileTypes) {
                obj_ptr<DirEntry> pDirEntry = new DirEntry();
                pDirEntry->fill(dirent.name, path, dirent_type_to_mode(dirent.type));
                retVal->append(pDirEntry);
            } else {
                hr = append_dirent_name(retVal, dirent.name, encoding);
                if (hr < 0)
                    return hr;
            }
            if (dirent.type == UV_DIRENT_DIR && recursive)
                paths.append(dirent.name);
        }
    }

    if (recursive) {
        size_t pos = 0;
        while (pos < paths.size()) {
            exlib::string& _path = paths[pos++];

            AutoReq req;
            int32_t ret = uv_fs_scandir(NULL, &req, (path + PATH_SLASH + _path).c_str(), 0, NULL);
            if (ret < 0)
                return ret;

            uv_dirent_t dirent;

            while (uv_fs_scandir_next(&req, &dirent) != UV_EOF) {
                exlib::string full_path = _path + PATH_SLASH + dirent.name;
                if (withFileTypes) {
                    obj_ptr<DirEntry> pDirEntry = new DirEntry();
                    pDirEntry->fill(dirent.name, path + PATH_SLASH + _path, dirent_type_to_mode(dirent.type));
                    retVal->append(pDirEntry);
                } else {
                    hr = append_dirent_name(retVal, full_path.c_str(), encoding);
                    if (hr < 0)
                        return hr;
                }
                if (dirent.type == UV_DIRENT_DIR)
                    paths.append(full_path);
            }
        }
    }

    return 0;
}

// Node.js compatibility: createReadStream/createWriteStream never throw synchronously,
// failures are reported through the 'error' event instead.
static result_t stream_open_error(exlib::string fname, exlib::string flags, result_t hr,
    obj_ptr<SeekableStream_base>& retVal)
{
    obj_ptr<FileStream> failed = new FileStream();
    // record the path and the failure state, the result is ignored on purpose
    failed->open(fname, flags);

    retVal = failed;

    obj_ptr<SeekableStream_base> stm = failed;
    Isolate* isolate = failed->holder();

    async([stm, hr, isolate]() {
        // the emit must run on the JS thread with a valid context
        isolate->sync([stm, hr]() -> int {
            v8::Local<v8::Value> err = FillError(hr);
            bool retVal;

            JSTrigger stmTrigger(stm);
            stmTrigger._emit("error", &err, 1, retVal);

            return 0;
        });
    });

    return 0;
}

result_t fs_base::createReadStream(exlib::string fname, v8::Local<v8::Object> options,
    obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(3);

        exlib::string flags = "r";
        GetConfigValue(options, "flags", flags);
        ac->m_ctx[0] = flags;

        int64_t start = -1;
        GetConfigValue(options, "start", start);
        ac->m_ctx[1] = start;

        int64_t end = -1;
        GetConfigValue(options, "end", end);
        ac->m_ctx[2] = end;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string flags = ac->m_ctx[0].string();
    int64_t start = ac->m_ctx[1].longVal();
    int64_t end = ac->m_ctx[2].longVal();

    obj_ptr<SeekableStream_base> stm;
    result_t hr = openFile(fname, flags, stm, ac);
    if (hr < 0)
        return stream_open_error(fname, flags, hr, retVal);

    if (start >= 0 || end >= 0) {
        int64_t sz;
        stm->size(sz);

        int64_t begin = (start >= 0) ? start : 0;
        int64_t e = (end >= 0) ? (end + 1) : sz;

        retVal = new RangeStream(stm, begin, e);
    } else {
        retVal = stm;
    }

    return 0;
}

result_t fs_base::createWriteStream(exlib::string fname, v8::Local<v8::Object> options,
    obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        exlib::string flags = "w";
        GetConfigValue(options, "flags", flags);
        ac->m_ctx[0] = flags;

        int64_t start = -1;
        GetConfigValue(options, "start", start);
        ac->m_ctx[1] = start;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string flags = ac->m_ctx[0].string();
    int64_t start = ac->m_ctx[1].longVal();

    obj_ptr<SeekableStream_base> stm;
    result_t hr = openFile(fname, flags, stm, ac);
    if (hr < 0)
        return stream_open_error(fname, flags, hr, retVal);

    // Node.js: options.start sets the offset of the first write
    if (start > 0) {
        FileStream_base* pFileBase = FileStream_base::getInstance(stm);
        if (pFileBase) {
            FileStream* pFile = static_cast<FileStream*>(pFileBase);
            hr = pFile->seek(start, SEEK_SET);
            if (hr < 0)
                return stream_open_error(fname, flags, hr, retVal);
        }
    }

    retVal = stm;

    return 0;
}

}
