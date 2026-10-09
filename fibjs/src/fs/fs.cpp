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
#include "union_helpers.h"

namespace fibjs {

DECLARE_MODULE(fs);

// ---------------------------------------------------------------------------
// Node.js compatible argument helpers
// ---------------------------------------------------------------------------

// Node.js reports buffer offset/length violations as RangeError +
// ERR_OUT_OF_RANGE; the message keeps fibjs' own (more explicit) wording.
static result_t setRangeError(const char* msg)
{
    return Runtime::setError(ErrorPayload::make(errtype::kRangeError, CALL_E_OUTRANGE)
            .with_code("ERR_OUT_OF_RANGE")
            .with_message(msg));
}

// Node prints NaN / Infinity by name, integers without a fractional part.
static exlib::string node_number_text(double d)
{
    if (std::isnan(d))
        return "NaN";
    if (std::isinf(d))
        return d < 0 ? "-Infinity" : "Infinity";
    if (floor(d) == d && fabs(d) < 1e15)
        return std::to_string((long long)d);

    char buf[64];
    snprintf(buf, sizeof(buf), "%g", d);
    return buf;
}

// Node's "Received ..." description for a rejected argument value.
static exlib::string node_received_text(Variant& v)
{
    switch (v.type()) {
    case Variant::VT_Undefined:
        return "undefined";
    case Variant::VT_Null:
        return "null";
    case Variant::VT_Boolean:
        return exlib::string("type boolean (") + (v.boolVal() ? "true" : "false") + ")";
    case Variant::VT_Integer:
    case Variant::VT_Long:
    case Variant::VT_Number:
        return "type number (" + node_number_text(v.dblVal()) + ")";
    case Variant::VT_String:
        return "type string ('" + v.string() + "')";
    case Variant::VT_Object:
    case Variant::VT_JSValue: {
        v8::Local<v8::Value> jsv = (v8::Local<v8::Value>)v;

        if (!jsv.IsEmpty() && jsv->IsArray())
            return "an instance of Array";
        return "an instance of Object";
    }
    default:
        return "an instance of Object";
    }
}

// Node.js ERR_OUT_OF_RANGE: RangeError with the "must be <range>" wording.
static result_t setOutOfRange(const char* name, const char* range, exlib::string received)
{
    return Runtime::setError(ErrorPayload::make(errtype::kRangeError, CALL_E_OUTRANGE)
            .with_code("ERR_OUT_OF_RANGE")
            .format("The value of \"%s\" is out of range. It must be %s. Received %s",
                name, range, received.c_str()));
}

// Node.js ERR_INVALID_ARG_VALUE: TypeError with the "must be ..." wording.
static result_t setInvalidArgValue(const char* name, const char* must_be, exlib::string received)
{
    return Runtime::setError(ErrorPayload::make(errtype::kTypeError, CALL_E_INVALIDARG)
            .with_code("ERR_INVALID_ARG_VALUE")
            .format("The argument '%s' %s. Received %s", name, must_be, received.c_str()));
}

// Node.js ERR_INVALID_ARG_TYPE: TypeError with the "Received type ..." wording.
static result_t setInvalidArgType(const char* name, const char* must_be, exlib::string received)
{
    return Runtime::setError(ErrorPayload::make(errtype::kTypeError, CALL_E_INVALIDARG)
            .with_code("ERR_INVALID_ARG_TYPE")
            .format("The \"%s\" argument must be %s. Received %s", name, must_be, received.c_str()));
}

// Node.js time argument: Date object | unix timestamp in seconds | date string
static result_t to_unix_timestamp(Variant& v, double& retVal)
{
    switch (v.type()) {
    case Variant::VT_Date: {
        double ms = v.dateValue();

        if (std::isnan(ms))
            return setInvalidArgType("time", "an instance of Date or an Time in seconds", "an invalid Date");

        retVal = ms / 1000;
        return 0;
    }
    case Variant::VT_String: {
        // Node accepts a numeric string (seconds), nothing else.
        exlib::string s = v.string();
        char* end = NULL;
        double d = strtod(s.c_str(), &end);

        if (end == s.c_str() || *end != '\0')
            return setInvalidArgType("time", "an instance of Date or an Time in seconds", node_received_text(v));

        retVal = d;
        return 0;
    }
    case Variant::VT_Number: {
        double d = v.dblVal();

        if (std::isnan(d))
            return setInvalidArgType("time", "an instance of Date or an Time in seconds", node_received_text(v));

        retVal = d;
        return 0;
    }
    case Variant::VT_Integer:
    case Variant::VT_Long:
        retVal = v.dblVal();
        return 0;
    default:
        return setInvalidArgType("time", "an instance of Date or an Time in seconds", node_received_text(v));
    }
}

// Node.js validates an encoding before touching the file: an unknown label is
// ERR_INVALID_ARG_VALUE. 'buffer' stays valid - it is a fibjs extension (the
// API hands back raw Buffers instead of decoded strings).
static result_t check_encoding(const exlib::string& encoding)
{
    if (encoding.empty() || encoding == "buffer"
        || encoding_conv::is_buffer_codec(encoding) || encoding_conv::is_encoding(encoding))
        return 0;

    return setInvalidArgValue("encoding", "is invalid encoding", "'" + encoding + "'");
}

// appends through an open descriptor: the flag and mode of an options object
// do not apply, a descriptor writes where it is positioned
static result_t append_file_fd(FileHandle_base* fd, Buffer_base* data, int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
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
            return CHECK_ERROR(LastError("write"));
        pos += len;
    }

    retVal = n;

    return 0;
}


// fs-side wrapper around the shared encoder: validates the label first.
static result_t fs_common_encode(exlib::string codec, exlib::string data, exlib::string& retVal)
{
    result_t hr = check_encoding(codec);
    if (hr < 0)
        return hr;

    return commonEncode(codec, data, retVal);
}

// Node.js mode argument: an unsigned 32-bit integer, or an octal string
// matching /^[0-7]+$/ (no 0o prefix).
static result_t to_mode_value(Variant& v, int32_t& retVal)
{
    switch (v.type()) {
    case Variant::VT_Number:
    case Variant::VT_Integer:
    case Variant::VT_Long: {
        double d = v.dblVal();

        if (std::isnan(d) || floor(d) != d)
            return setOutOfRange("mode", "an integer", node_number_text(d));

        if (d < 0 || d > 4294967295.0)
            return setOutOfRange("mode", ">= 0 && <= 4294967295", node_number_text(d));

        retVal = (int32_t)d;
        return 0;
    }
    case Variant::VT_String: {
        exlib::string s = v.string();
        exlib::string quoted = "'" + s + "'";
        double mode = 0;

        if (s.empty())
            return setInvalidArgValue("mode", "must be a 32-bit unsigned integer or an octal string", quoted);

        for (size_t pos = 0; pos < s.length(); pos++) {
            char c = s[pos];

            if (c < '0' || c > '7')
                return setInvalidArgValue("mode", "must be a 32-bit unsigned integer or an octal string", quoted);

            mode = mode * 8 + (c - '0');
        }

        if (mode > 4294967295.0)
            return setOutOfRange("mode", ">= 0 && <= 4294967295", node_number_text(mode));

        retVal = (int32_t)mode;
        return 0;
    }
    default:
        return setInvalidArgType("mode", "a 32-bit unsigned integer or an octal string", node_received_text(v));
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

result_t FileHandle::chmod(int32_t mode, AsyncHandle ac)
{
    return fs_base::fchmod(this, mode, std::move(ac));
}

result_t FileHandle::stat(obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    return fs_base::fstat(this, retVal, std::move(ac));
}

result_t FileHandle::read(Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, obj_ptr<ReadType>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t bufLength = Buffer::Cast(buffer)->length();

    // Node.js validates the offset and length as integers first and then
    // reports the length bound relative to the offset (fs.read); the arguments
    // are checked before the handle state, so a bad range wins over a closed
    // handle.
    if (offset < 0)
        return setOutOfRange("offset", ">= 0 && <= 9007199254740991", std::to_string(offset));

    if (length < 0)
        return setOutOfRange("length", ">= 0", std::to_string(length));

    if (length > bufLength - offset)
        return setOutOfRange("length", ("<= " + std::to_string(bufLength - offset)).c_str(), std::to_string(length));

    if (m_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (position > -1) {
        if (_lseeki64(m_fd, position, SEEK_SET) < 0)
            return CHECK_ERROR(LastError("read"));
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
                return CHECK_ERROR(ReadError("read"));
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

result_t FileHandle::read(v8::Local<v8::Object> options, obj_ptr<ReadType>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(4);

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

        ac.ctxv()[0] = buffer;
        ac.ctxv()[1] = offset;
        ac.ctxv()[2] = length;
        ac.ctxv()[3] = position;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;
    ctx_hr = ac.ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;
    ctx_hr = ac.ctx(2);
    if (ctx_hr < 0)
        return ctx_hr;
    ctx_hr = ac.ctx(3);
    if (ctx_hr < 0)
        return ctx_hr;

    return read(Buffer_base::getInstance(ac.ctxv()[0].object()),
        ac.ctxv()[1].intVal(), ac.ctxv()[2].intVal(), ac.ctxv()[3].intVal(),
        retVal, std::move(ac));
}

result_t FileHandle::write(Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, obj_ptr<WriteType>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t n;
    result_t hr = fs_base::write(this, buffer, offset, length, position, n, std::move(ac));
    if (hr < 0)
        return hr;

    // Node.js: filehandle.write() resolves with { bytesWritten, buffer }
    obj_ptr<WriteType> result = new WriteType();
    result->bytesWritten = n;
    result->buffer = buffer;

    retVal = result;

    return 0;
}

result_t FileHandle::write(exlib::string string, int32_t position, exlib::string encoding, obj_ptr<WriteType>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<Buffer_base> buf;
    {
        exlib::string strData = string;
        result_t hr = fs_common_encode(encoding, strData, strData);
        if (hr < 0)
            return hr;

        buf = new Buffer(strData.c_str(), strData.length());
    }

    return write(buf, 0, -1, position, retVal, std::move(ac));
}


result_t FileHandle::readFile(Union_readFile_options options, Variant& retVal, AsyncHandle ac)
{
    exlib::string encoding;

    if (std::holds_alternative<exlib::string>(options))
        encoding = std::get<exlib::string>(options);
    else {
        // the options object is readable in the synchronous phase only: the
        // callback phase receives an empty handle
        if (ac.isSync()) {
            ac.ctxv().resize(1);

            GetConfigValue(std::get<v8::Local<v8::Object>>(options), "encoding", encoding);
            ac.ctxv()[0] = encoding;

            return CHECK_ERROR(CALL_E_NOSYNC);
        }

        // the object form is carried in ctx[0]; an entry that never ran the
        // sync phase (the async-only compile cache one) must report that
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        encoding = ac.ctxv()[0].string();
    }

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    // Node.js validates the encoding before reading.
    {
        result_t hr = check_encoding(encoding);
        if (hr < 0)
            return hr;
    }

    // seek to beginning
    if (_lseeki64(m_fd, 0, SEEK_SET) < 0)
        return CHECK_ERROR(LastError("read"));

    exlib::string strBuf;
    char tmp[STREAM_BUFF_SIZE];

    while (true) {
        int32_t n = (int32_t)::_read(m_fd, tmp, STREAM_BUFF_SIZE);
        if (n < 0)
            return CHECK_ERROR(ReadError("read"));
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


result_t FileHandle::writeFile(Union_writeFile_data data, Union_writeFile_opt opt, int32_t& retVal, AsyncHandle ac)
{
    bool bBuffer = std::holds_alternative<obj_ptr<Buffer_base>>(data);
    exlib::string encoding;

    if (std::holds_alternative<exlib::string>(opt))
        encoding = std::get<exlib::string>(opt);
    else {
        // the options object is readable in the synchronous phase only: the
        // callback phase receives an empty handle. A Buffer only validates the
        // empty label, a string takes its encoding from the options.
        if (ac.isSync()) {
            ac.ctxv().resize(1);

            if (!bBuffer) {
                encoding = "utf8";

                result_t hr = GetConfigValue(std::get<v8::Local<v8::Object>>(opt), "encoding", encoding, true);
                if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                    return hr;
            }

            ac.ctxv()[0] = encoding;

            return CHECK_ERROR(CALL_E_NOSYNC);
        }

        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        encoding = ac.ctxv()[0].string();
    }

    // Node.js validates the encoding even when the data is a Buffer.
    {
        result_t _e = check_encoding(encoding);
        if (_e < 0)
            return _e;
    }

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;

    if (bBuffer)
        buf = std::get<obj_ptr<Buffer_base>>(data);
    else {
        // the string is encoded before the handle is touched
        exlib::string strData = std::get<exlib::string>(data);
        result_t hr = fs_common_encode(encoding, strData, strData);
        if (hr < 0)
            return hr;

        buf = new Buffer(strData.c_str(), strData.length());
    }

    if (m_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    // seek to beginning and truncate
    if (_lseeki64(m_fd, 0, SEEK_SET) < 0)
        return CHECK_ERROR(LastError("write"));

    exlib::string strBuf;
    Buffer::Cast(buf)->toString(strBuf);

    const char* p = strBuf.c_str();
    int32_t sz = (int32_t)strBuf.length();
    retVal = sz;

    while (sz > 0) {
        int32_t n = (int32_t)::_write(m_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
        if (n < 0)
            return CHECK_ERROR(LastError("write"));
        sz -= n;
        p += n;
    }

    ftruncate64(m_fd, _lseeki64(m_fd, 0, SEEK_CUR));

    return 0;
}


result_t FileHandle::utimes(Variant atime, Variant mtime, AsyncHandle ac)
{
    return fs_base::futimes(this, atime, mtime, std::move(ac));
}

result_t FileHandle::chown(int32_t uid, int32_t gid, AsyncHandle ac)
{
    return fs_base::fchown(this, uid, gid, std::move(ac));
}

result_t FileHandle::sync(AsyncHandle ac)
{
    return fs_base::fsync(this, std::move(ac));
}

result_t FileHandle::datasync(AsyncHandle ac)
{
    return fs_base::fdatasync(this, std::move(ac));
}

result_t FileHandle::truncate(int32_t len, AsyncHandle ac)
{
    return fs_base::ftruncate(this, len, std::move(ac));
}


result_t FileHandle::appendFile(Union_appendFile_data data, int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;

    if (std::holds_alternative<obj_ptr<Buffer_base>>(data))
        buf = std::get<obj_ptr<Buffer_base>>(data);
    else {
        exlib::string strData = std::get<exlib::string>(data);
        buf = new Buffer(strData.c_str(), strData.length());
    }

    return append_file_fd(this, buf, retVal, std::move(ac));
}


result_t FileHandle::close(AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_fd == -1)
        return CHECK_ERROR(setSystemErrorPayload(UV_EBADF, "close"));

    int32_t fd = m_fd;
    m_fd = -1;

    // Node.js compatibility: the descriptor is released either way, and a failed
    // close(2) only means that it was not valid. On Windows the CRT sets errno
    // instead of the thread error, which would report whatever the previous call
    // left behind.
    if (::_close(fd))
        return CHECK_ERROR(setSystemErrorPayload(UV_EBADF, "close"));

    return 0;
}

result_t fs_base::open(exlib::string fname, exlib::string flags, int32_t mode,
    obj_ptr<FileHandle_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr < 0)
        return hr;

    int32_t _fd;
    hr = file_open(safe_name, flags, mode, _fd);
    if (hr < 0)
        return setSystemErrorPayload(hr, "open", fname);

    retVal = new FileHandle(_fd);

    return 0;
}

result_t fs_base::open(exlib::string fname, int32_t flags, int32_t mode,
    obj_ptr<FileHandle_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr < 0)
        return hr;

    int32_t _fd;
    hr = file_open(safe_name, flags, mode, _fd);
    if (hr < 0)
        return setSystemErrorPayload(hr, "open", fname);

    retVal = new FileHandle(_fd);

    return 0;
}

result_t fs_base::open(exlib::string fname, exlib::string flags, Variant mode,
    obj_ptr<FileHandle_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        int32_t _mode;
        result_t hr = to_mode_value(mode, _mode);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = _mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    return open(fname, flags, ac.ctxv()[0].intVal(), retVal, std::move(ac));
}

result_t fs_base::close(Union_close_fd fd, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<FileHandle_base> handle;
    result_t hr = filehandle_from_union(fd, handle);
    if (hr < 0)
        return hr;

    return handle->close(std::move(ac));
}

result_t fs_base::openTextStream(exlib::string fname, exlib::string flags,
    obj_ptr<BufferedStream_base>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> pFile;
    result_t hr = openFile(fname, flags, pFile, std::move(ac));
    if (hr < 0)
        return hr;

    return BufferedStream_base::_new(pFile, retVal);
}

result_t fs_base::readTextFile(exlib::string fname, exlib::string& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = openFile(fname, "r", f, std::move(ac));
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
    Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js reports an unknown encoding before it opens the file.
    {
        result_t hr = check_encoding(encoding);
        if (hr < 0)
            return hr;
    }

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = fs_base::openFile(fname, flag, f, std::move(ac));
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


result_t fs_base::readLines(exlib::string fname, int32_t maxlines,
    std::vector<exlib::string>& retVal)
{
    obj_ptr<BufferedStream_base> pFile;
    result_t hr;

    hr = ac_openTextStream(fname, "r", pFile);
    if (hr < 0)
        return hr;

    return pFile->readLines(maxlines, retVal);
}

static result_t open_file_for_write(exlib::string fname, exlib::string flag, int32_t mode,
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac)
{
    // keep the zip-aware / sandbox-checked path for the default creation mode
    if (mode == 0666)
        return fs_base::openFile(fname, flag, retVal, std::move(ac));

    // an explicit creation mode only applies to the real filesystem
    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr < 0)
        return hr;

    if (!ac.isolate()->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<FileStream> pFile = new FileStream();
    hr = pFile->open(safe_name, flag, mode);
    if (hr < 0)
        return hr;

    retVal = pFile;

    return 0;
}

static result_t write_file_ext(exlib::string fname, Buffer_base* data, exlib::string flag, int32_t mode,
    int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = open_file_for_write(fname, flag, mode, f, std::move(ac));
    if (hr < 0)
        return hr;

    retVal = Buffer::Cast(data)->length();
    hr = f->cc_writeBuffer(data);
    f->cc_close();

    return hr;
}

static result_t write_text_file_ext(exlib::string fname, exlib::string txt, exlib::string flag, int32_t mode,
    int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = open_file_for_write(fname, flag, mode, f, std::move(ac));
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(txt.c_str(), txt.length());

    retVal = (int32_t)txt.length();
    hr = f->cc_writeBuffer(buf);
    f->cc_close();

    return hr;
}

result_t fs_base::writeTextFile(exlib::string fname, exlib::string txt, int32_t& retVal,
    AsyncHandle ac)
{
    return write_text_file_ext(fname, txt, "w", 0666, retVal, std::move(ac));
}


static result_t append_file_ext(exlib::string fname, exlib::string flag, int32_t mode, Buffer_base* data,
    int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = open_file_for_write(fname, flag, mode, f, std::move(ac));
    if (hr < 0)
        return hr;

    retVal = Buffer::Cast(data)->length();
    hr = f->cc_writeBuffer(data);
    f->cc_close();

    return hr;
}

static result_t read_file_fd(FileHandle_base* fd, exlib::string encoding, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
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
            return CHECK_ERROR(ReadError("read"));
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

static result_t write_file_fd(FileHandle_base* fd, Buffer_base* data, int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
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
        return CHECK_ERROR(LastError("write"));

    {
        AutoReq req;
        int32_t ret = uv_fs_ftruncate(NULL, &req, _fd, 0, NULL);
        if (ret < 0)
            return setSystemErrorPayload(ret, "write");
    }

    size_t pos = 0;
    const uint8_t* p = (const uint8_t*)Buffer::Cast(data)->data();

    while (pos < (size_t)n) {
        int32_t len = (int32_t)::_write(_fd, p + pos, (size_t)n - pos);
        if (len < 0)
            return CHECK_ERROR(LastError("write"));
        pos += len;
    }

    retVal = n;

    return 0;
}


result_t fs_base::readFile(Union_readFile_fname fname, Union_readFile_options options,
    Variant& retVal, AsyncHandle ac)
{
    bool bFd = !std::holds_alternative<exlib::string>(fname);

    exlib::string encoding;
    exlib::string flag = "r";

    if (std::holds_alternative<exlib::string>(options))
        encoding = std::get<exlib::string>(options);
    else {
        // the options object is readable in the synchronous phase only: the
        // callback phase receives an empty handle
        if (ac.isSync()) {
            ac.ctxv().resize(2);

            v8::Local<v8::Object> opts = std::get<v8::Local<v8::Object>>(options);

            // a descriptor decodes as utf8 by default, a name returns a Buffer
            if (bFd)
                encoding = "utf8";

            GetConfigValue(opts, "encoding", encoding);
            ac.ctxv()[0] = encoding;

            if (!bFd) {
                GetConfigValue(opts, "flag", flag);
                ac.ctxv()[1] = flag;
            }

            return CHECK_ERROR(CALL_E_NOSYNC);
        }

        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        encoding = ac.ctxv()[0].string();

        if (!bFd) {
            ctx_hr = ac.ctx(1);
            if (ctx_hr < 0)
                return ctx_hr;

            flag = ac.ctxv()[1].string();
        }
    }

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js reports an unknown encoding before the file is opened; the
    // descriptor form leaves the label to the conversion
    if (!bFd) {
        result_t hr = check_encoding(encoding);
        if (hr < 0)
            return hr;
    }

    // Node.js: the descriptor is neither closed nor repositioned; the handle
    // is resolved here, off the sync phase (plans/async-phase-discipline-audit-2026-10-05.md §A1)
    if (bFd) {
        obj_ptr<FileHandle_base> fdHandle;
        result_t fd_hr = filehandle_from_union(fname, fdHandle);
        if (fd_hr < 0)
            return fd_hr;

        return read_file_fd(fdHandle, encoding, retVal, std::move(ac));
    }

    exlib::string strName = std::get<exlib::string>(fname);

    return setSystemErrorPayload(read_file_ext(strName, flag, encoding, retVal, std::move(ac)), "open", strName);
}


result_t fs_base::writeFile(Union_writeFile_fname fname, Union_writeFile_data data,
    Union_writeFile_opt opt, int32_t& retVal, AsyncHandle ac)
{
    bool bFd = !std::holds_alternative<exlib::string>(fname);
    bool bBuffer = std::holds_alternative<obj_ptr<Buffer_base>>(data);

    exlib::string encoding;
    exlib::string flag = "w";
    int32_t mode = 0666;

    if (std::holds_alternative<exlib::string>(opt))
        encoding = std::get<exlib::string>(opt);
    else if (!bFd) {
        // the name form takes the encoding, flag and mode from the options
        // object, which is readable in the synchronous phase only: the callback
        // phase receives an empty handle
        if (ac.isSync()) {
            ac.ctxv().resize(3);

            v8::Local<v8::Object> options = std::get<v8::Local<v8::Object>>(opt);

            // Node.js validates the encoding even when the data is a Buffer; a
            // value of another type is reported for a text write only, a binary
            // write ignores it
            if (!bBuffer)
                encoding = "utf8";

            result_t hr = GetConfigValue(options, "encoding", encoding, true);
            if (!bBuffer && hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                return hr;

            result_t _e = check_encoding(encoding);
            if (_e < 0)
                return _e;
            ac.ctxv()[0] = encoding;

            GetConfigValue(options, "flag", flag);
            ac.ctxv()[1] = flag;

            GetConfigValue(options, "mode", mode);
            if (mode < 0)
                return CHECK_ERROR(setOutOfRange("mode", ">= 0 && <= 4294967295", std::to_string(mode)));
            ac.ctxv()[2] = mode;

            return CHECK_ERROR(CALL_E_NOSYNC);
        }

        // the sync phase filled the three slots together
        result_t ctx_hr = ac.ctx(2);
        if (ctx_hr < 0)
            return ctx_hr;

        encoding = ac.ctxv()[0].string();
        flag = ac.ctxv()[1].string();
        mode = ac.ctxv()[2].intVal();
    }

    // the descriptor form ignores the options object: the encoding stays empty

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js validates the encoding even when the data is a Buffer
    {
        result_t hr = check_encoding(encoding);
        if (hr < 0)
            return hr;
    }

    obj_ptr<Buffer_base> buf;

    if (bBuffer)
        buf = std::get<obj_ptr<Buffer_base>>(data);
    else {
        exlib::string strData = std::get<exlib::string>(data);
        result_t hr = fs_common_encode(encoding, strData, strData);
        if (hr < 0)
            return hr;

        buf = new Buffer(strData.c_str(), strData.length());
    }

    if (bFd) {
        obj_ptr<FileHandle_base> fdHandle;
        result_t fd_hr = filehandle_from_union(fname, fdHandle);
        if (fd_hr < 0)
            return fd_hr;

        return write_file_fd(fdHandle, buf, retVal, std::move(ac));
    }

    exlib::string strName = std::get<exlib::string>(fname);

    return setSystemErrorPayload(write_file_ext(strName, buf, flag, mode, retVal, std::move(ac)), "open", strName);
}


result_t fs_base::appendFile(Union_appendFile_fname fname, Union_appendFile_data data,
    Union_appendFile_options options, int32_t& retVal, AsyncHandle ac)
{
    bool bFd = !std::holds_alternative<exlib::string>(fname);
    bool bBuffer = std::holds_alternative<obj_ptr<Buffer_base>>(data);

    exlib::string encoding;
    exlib::string flag = "a";
    int32_t mode = 0666;

    if (std::holds_alternative<exlib::string>(options))
        encoding = std::get<exlib::string>(options);
    else if (!bFd) {
        // the name form takes flag and mode from the options object; the
        // encoding of the object only validates the label, the data itself is
        // appended as it is. The object is readable in the synchronous phase
        // only: the callback phase receives an empty handle.
        if (ac.isSync()) {
            ac.ctxv().resize(3);

            v8::Local<v8::Object> opts = std::get<v8::Local<v8::Object>>(options);

            // Node.js validates the encoding even when the data is a Buffer
            GetConfigValue(opts, "encoding", encoding, true);
            result_t _e = check_encoding(encoding);
            if (_e < 0)
                return _e;
            ac.ctxv()[0] = encoding;

            GetConfigValue(opts, "flag", flag);
            ac.ctxv()[1] = flag;

            GetConfigValue(opts, "mode", mode);
            if (mode < 0)
                return CHECK_ERROR(setOutOfRange("mode", ">= 0 && <= 4294967295", std::to_string(mode)));
            ac.ctxv()[2] = mode;

            return CHECK_ERROR(CALL_E_NOSYNC);
        }

        // the sync phase filled the three slots together
        result_t ctx_hr = ac.ctx(2);
        if (ctx_hr < 0)
            return ctx_hr;

        encoding.clear();
        flag = ac.ctxv()[1].string();
        mode = ac.ctxv()[2].intVal();
    }

    // the descriptor form ignores the options object: the encoding stays empty

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js validates the encoding even when the data is a Buffer
    {
        result_t hr = check_encoding(encoding);
        if (hr < 0)
            return hr;
    }

    obj_ptr<Buffer_base> buf;

    if (bBuffer)
        buf = std::get<obj_ptr<Buffer_base>>(data);
    else {
        // the encoding string encodes the data, the options object does not
        exlib::string strData = std::get<exlib::string>(data);

        if (!encoding.empty()) {
            result_t hr = fs_common_encode(encoding, strData, strData);
            if (hr < 0)
                return hr;
        }

        buf = new Buffer(strData.c_str(), strData.length());
    }

    if (bFd) {
        obj_ptr<FileHandle_base> fdHandle;
        result_t fd_hr = filehandle_from_union(fname, fdHandle);
        if (fd_hr < 0)
            return fd_hr;

        return append_file_fd(fdHandle, buf, retVal, std::move(ac));
    }

    exlib::string strName = std::get<exlib::string>(fname);

    return setSystemErrorPayload(append_file_ext(strName, flag, mode, buf, retVal, std::move(ac)), "open", strName);
}


// The descriptor forms that only need the raw fd: the int alternative is taken
// as it is, a FileHandle is asked for it - no FileHandle object is built for
// the int alternative. Called after the sync guard (no work before it, see
// plans/async-phase-discipline-audit-2026-10-05.md §4.2).
template <typename Variant>
static result_t fd_from_union(Variant& fd, int32_t& _fd)
{
    if (std::holds_alternative<int32_t>(fd))
        _fd = std::get<int32_t>(fd);
    else {
        result_t hr = std::get<obj_ptr<FileHandle_base>>(fd)->get_fd(_fd);
        if (hr < 0)
            return hr;
    }

    if (_fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return 0;
}

result_t fs_base::read(Union_read_fd fd, Buffer_base* buffer, int32_t offset, int32_t length,
    int32_t position, int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    result_t hr = fd_from_union(fd, _fd);
    if (hr < 0)
        return hr;

    int32_t bufLength = Buffer::Cast(buffer)->length();

    // Node.js validates the offset and length as integers first and then
    // reports the length bound relative to the offset (fs.read).
    if (offset < 0)
        return setOutOfRange("offset", ">= 0 && <= 9007199254740991", std::to_string(offset));

    if (length < 0)
        return setOutOfRange("length", ">= 0", std::to_string(length));

    if (length > bufLength - offset)
        return setOutOfRange("length", ("<= " + std::to_string(bufLength - offset)).c_str(), std::to_string(length));

    if (position > -1) {
        if (_lseeki64(_fd, position, SEEK_SET) < 0)
            return CHECK_ERROR(LastError("read"));
    }

    exlib::string strBuf;
    if (length > 0) {
        strBuf.resize(length);
        int32_t sz = length;
        char* p = strBuf.data();

        while (sz) {
            int32_t n = (int32_t)::_read(_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
            if (n < 0)
                return CHECK_ERROR(ReadError("read"));
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

result_t fs_base::write(Union_write_fd fd, Buffer_base* buffer, int32_t offset, int32_t length,
    int32_t position, int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    result_t hr = fd_from_union(fd, _fd);
    if (hr < 0)
        return hr;

    int32_t bufLength = Buffer::Cast(buffer)->length();

    // Node.js validates the offset first (an out of range offset is reported
    // against the buffer length), then the length; a negative length means "to
    // the end" here (the JS binding passes -1 for an omitted length).
    if (offset < 0)
        return setOutOfRange("offset", ">= 0 && <= 9007199254740991", std::to_string(offset));

    if (offset > bufLength)
        return setOutOfRange("offset", ("<= " + std::to_string(bufLength)).c_str(), std::to_string(offset));

    if (length < 0)
        length = bufLength - offset;

    if (length > bufLength - offset)
        return setOutOfRange("length", ("<= " + std::to_string(bufLength - offset)).c_str(), std::to_string(length));

    if (position > -1) {
        if (_lseeki64(_fd, position, SEEK_SET) < 0)
            return CHECK_ERROR(LastError("write"));
    }

    if (length > 0) {
        Buffer* buf = Buffer::Cast(buffer);
        int32_t sz = length;
        const uint8_t* p = buf->data() + offset;

        while (sz) {
            int32_t n = (int32_t)::_write(_fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
            if (n < 0) {
                result_t hr = LastError();

                if (hr > CALL_E_MAX) {
                    ErrorPayload payload = ErrorPayload::from_system(hr)
                                               .with_syscall("write")
                                               .arg("buffer", buffer)
                                               .arg("length", length)
                                               .arg("position", position);
                    return CHECK_ERROR(setErrorPayload(hr, payload));
                }

                return CHECK_ERROR(hr);
            }

            sz -= n;
            p += n;
        }
    }

    retVal = length;

    return 0;
}

result_t fs_base::write(Union_write_fd fd, exlib::string string, int32_t position,
    exlib::string encoding, int32_t& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    result_t hr = fd_from_union(fd, _fd);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf;

    hr = encoding_conv(encoding).encode(string, buf);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return write(fd, buf, 0, -1, position, retVal, std::move(ac));
}

result_t fs_base::fstat(Union_fstat_fd fd, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t _fd;
    result_t hr = fd_from_union(fd, _fd);
    if (hr < 0)
        return hr;

    AutoReq req;
    int32_t ret = uv_fs_fstat(NULL, &req, _fd, NULL);
    if (ret < 0)
        return setSystemErrorPayload(ret, "fstat");

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill("", &req.statbuf);
    retVal = pStat;

    return 0;
}

result_t fs_base::fstat(Union_fstat_fd fd, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    return fstat(fd, retVal, std::move(ac));
}

result_t fs_base::exists(exlib::string path, bool& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    retVal = uv_fs_access(NULL, &req, path.c_str(), F_OK, NULL) == 0;
    return 0;
}

// Node.js compatibility: the options argument is accepted and ignored
result_t fs_base::exists(exlib::string path, v8::Local<v8::Object> options, bool& retVal, AsyncHandle ac)
{
    return exists(path, retVal, std::move(ac));
}

result_t fs_base::access(exlib::string path, int32_t mode, AsyncHandle ac)
{
    if (ac.isSync()) {
        // Node.js compatibility: mode is a bitmask of F_OK/R_OK/W_OK/X_OK
        if (mode < 0 || mode > 7)
            return CHECK_ERROR(setRangeError("mode is out of range: >= 0 && <= 7"));

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    return setSystemErrorPayload(uv_fs_access(NULL, &req, path.c_str(), mode, NULL), "access", path);
}

result_t fs_base::link(exlib::string oldPath, exlib::string newPath, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(oldPath, oldPath);
    if (hr < 0)
        return hr;

    hr = normalize_file_path_like(newPath, newPath);
    if (hr < 0)
        return hr;

    AutoReq req;
    return setSystemErrorPayload2(uv_fs_link(NULL, &req, oldPath.c_str(), newPath.c_str(), NULL),
        "link", oldPath, newPath);
}

result_t fs_base::unlink(exlib::string path, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    return setSystemErrorPayload(uv_fs_unlink(NULL, &req, path.c_str(), NULL), "unlink", path);
}

result_t fs_base::symlink(exlib::string target, exlib::string linkpath, exlib::string type, AsyncHandle ac)
{
    if (ac.isSync())
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
    return setSystemErrorPayload2(uv_fs_symlink(NULL, &req, target.c_str(), linkpath.c_str(), _type, NULL),
        "symlink", target, linkpath);
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

result_t fs_base::readlink(exlib::string path, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    int32_t ret = uv_fs_readlink(NULL, &req, path.c_str(), NULL);
    if (ret < 0)
        return setSystemErrorPayload(ret, "readlink", path);

    retVal = (const char*)req.ptr;
    return 0;
}

static result_t readlink_encoding(exlib::string path, exlib::string encoding, Variant& retVal, AsyncHandle ac);

// the options form: the encoding is read in the sync phase and travels to the
// async phase through m_ctx
static result_t readlink_options(exlib::string path, v8::Local<v8::Object> options, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        exlib::string encoding = "utf8";
        GetConfigValue(options, "encoding", encoding);
        ac.ctxv()[0] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    return readlink_encoding(path, ac.ctxv()[0].string(), retVal, std::move(ac));
}

// the encoding form
static result_t readlink_encoding(exlib::string path, exlib::string encoding, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    Variant link;
    result_t hr = fs_base::readlink(path, link, std::move(ac));
    if (hr < 0)
        return hr;

    return path_to_variant(link.string(), encoding, retVal);
}

result_t fs_base::readlink(exlib::string path, Union_readlink_options options, Variant& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<exlib::string>(options))
        return readlink_encoding(path, std::get<exlib::string>(options), retVal, std::move(ac));

    return readlink_options(path, std::get<v8::Local<v8::Object>>(options), retVal, std::move(ac));
}

result_t fs_base::realpath(exlib::string path, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
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
        hr = cc_lstat(testPath, stat, ac.isolate());
        if (hr < 0)
            return setSystemErrorPayload(hr, "lstat", testPath);

        bool isSymlink = false;
        stat->isSymbolicLink(isSymlink);

        if (isSymlink) {
            if (++linkCount > maxLinks)
                return CALL_E_FILE_NOT_FOUND; // Too many symlinks

            // Read the symlink target
            Variant linkValue;
            hr = cc_readlink(testPath, linkValue, ac.isolate());
            if (hr < 0)
                return setSystemErrorPayload(hr, "readlink", testPath);

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

static result_t realpath_encoding(exlib::string path, exlib::string encoding, Variant& retVal, AsyncHandle ac);

// the options form: the encoding is read in the sync phase and travels to the
// async phase through m_ctx
static result_t realpath_options(exlib::string path, v8::Local<v8::Object> options, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        exlib::string encoding = "utf8";
        GetConfigValue(options, "encoding", encoding);
        ac.ctxv()[0] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    return realpath_encoding(path, ac.ctxv()[0].string(), retVal, std::move(ac));
}

// the encoding form
static result_t realpath_encoding(exlib::string path, exlib::string encoding, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    Variant resolved;
    result_t hr = fs_base::realpath(path, resolved, std::move(ac));
    if (hr < 0)
        return hr;

    return path_to_variant(resolved.string(), encoding, retVal);
}

result_t fs_base::realpath(exlib::string path, Union_realpath_options options, Variant& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<exlib::string>(options))
        return realpath_encoding(path, std::get<exlib::string>(options), retVal, std::move(ac));

    return realpath_options(path, std::get<v8::Local<v8::Object>>(options), retVal, std::move(ac));
}

// the numeric form; the merged entry dispatches here, and so does the octal
// string form after parsing
static result_t mkdir_numeric(exlib::string path, int32_t mode, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js validates a numeric mode as an unsigned 32-bit integer; the IDL
    // binding types it as int32_t, so only the lower bound can be violated.
    if (mode < 0)
        return CHECK_ERROR(setOutOfRange("mode", ">= 0 && <= 4294967295", std::to_string(mode)));

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    return setSystemErrorPayload(uv_fs_mkdir(NULL, &req, path.c_str(), mode, NULL), "mkdir", path);
}

// the octal string form: parsed in the sync phase, the number travels to the
// async phase through m_ctx
static result_t mkdir_variant(exlib::string path, Variant mode, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        int32_t _mode;
        result_t hr = to_mode_value(mode, _mode);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = _mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    return mkdir_numeric(path, ac.ctxv()[0].intVal(), retVal, std::move(ac));
}

// the options form (recursive/mode), defined below: it reads its config in the
// sync phase
static result_t mkdir_options(exlib::string path, v8::Local<v8::Object> opt, Variant& retVal, AsyncHandle ac);

result_t fs_base::mkdir(exlib::string path, Union_mkdir_mode mode, Variant& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<int32_t>(mode))
        return mkdir_numeric(path, std::get<int32_t>(mode), retVal, std::move(ac));

    if (std::holds_alternative<v8::Local<v8::Object>>(mode))
        return mkdir_options(path, std::get<v8::Local<v8::Object>>(mode), retVal, std::move(ac));

    return mkdir_variant(path, std::get<Variant>(mode), retVal, std::move(ac));
}

static result_t mkdir_options(exlib::string path, v8::Local<v8::Object> opt, Variant& retVal, AsyncHandle ac)
{
    class AsyncUVMKDir : public uv_fs_t {
    public:
        AsyncUVMKDir(exlib::string path, int32_t mode, Variant* retVal, AsyncHandle ac)
            : m_ac(std::move(ac))
            , m_retVal(retVal)
            , m_requestPath(path)
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
                setSystemErrorPayload(pThis->m_last_err, "mkdir", pThis->m_requestPath);
                pThis->m_ac.apost(pThis->m_last_err);
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
                    pThis->m_ac.apost(0);
                    delete pThis;
                    return;
                }

                pThis->m_path = pThis->m_paths.back();
                pThis->m_paths.pop_back();
                break;
            case UV_EACCES:
            case UV_ENOTDIR:
            case UV_EPERM:
                setSystemErrorPayload(ret, "mkdir", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
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
                    setSystemErrorPayload(pThis->m_last_err, "mkdir", pThis->m_requestPath);
                    pThis->m_ac.apost(pThis->m_last_err);
                    delete pThis;
                }
                return;
            };

            uv_fs_req_cleanup(pThis);
            ret = uv_fs_mkdir(s_uv_loop, pThis, pThis->m_path.c_str(), pThis->m_mode, AsyncUVMKDir::cb_mkdir);
            if (ret != 0) {
                setSystemErrorPayload(ret, "mkdir", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
                delete pThis;
            }
        }

    private:
        AsyncHandle m_ac;
        Variant* m_retVal;
        exlib::string m_requestPath;
        exlib::string m_path;
        exlib::string m_first;
        bool m_existed = false;
        int32_t m_mode;
        std::vector<exlib::string> m_paths;
        int32_t m_last_err;
    };

    if (ac.isSync()) {
        ac.ctxv().resize(2);

        bool recursive = false;
        GetConfigValue(opt, "recursive", recursive);
        ac.ctxv()[0] = recursive;

        int32_t mode = 0777;
        GetConfigValue(opt, "mode", mode);
        ac.ctxv()[1] = mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;

    bool recursive = ac.ctxv()[0].boolVal();
    int32_t mode = ac.ctxv()[1].intVal();

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    if (!recursive)
        return mkdir_numeric(path, mode, retVal, std::move(ac));

    os_resolve(path);

    return uv_async([&] {
        return uv_fs_mkdir(s_uv_loop, new AsyncUVMKDir(path, mode, &retVal, std::move(ac)), path.c_str(), mode, AsyncUVMKDir::cb_mkdir);
    });
}

class AsyncUVRM : public uv_fs_t {
public:
    AsyncUVRM(exlib::string path, bool rmFile, bool force, AsyncHandle ac)
        : m_ac(std::move(ac))
        , m_requestPath(path)
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
                pThis->m_ac.apost(0);
            } else {
                setSystemErrorPayload(ret, "rmdir", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
            }
            delete pThis;
            return;
        }

        // Check if it's a regular file
        if (S_ISREG(pThis->statbuf.st_mode)) {
            if (!pThis->m_rmFile) {
                // rmdir does not delete files
                setSystemErrorPayload(UV_ENOTDIR, "rmdir", pThis->m_requestPath);
                pThis->m_ac.apost(UV_ENOTDIR);
                delete pThis;
                return;
            }
            // It's a file, remove it directly
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_unlink(s_uv_loop, pThis, pThis->m_path.c_str(), cb_unlink);
            if (ret != 0) {
                setSystemErrorPayload(ret, "rmdir", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
                delete pThis;
            }
            return;
        } else if (S_ISDIR(pThis->statbuf.st_mode)) {
            // It's a directory, scan contents first
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_scandir(s_uv_loop, pThis, pThis->m_path.c_str(), 0, cb_scandir);
            if (ret != 0) {
                setSystemErrorPayload(ret, "rmdir", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
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
                setSystemErrorPayload(ret, "rmdir", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
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
            setSystemErrorPayload(ret, "rmdir", pThis->m_requestPath);
            pThis->m_ac.apost(ret);
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
            setSystemErrorPayload(ret, "rmdir", pThis->m_requestPath);
            pThis->m_ac.apost(ret);
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

        pThis->m_ac.apost(ret);
        delete pThis;
    }

    static void cb_unlink(uv_fs_t* req)
    {
        AsyncUVRM* pThis = (AsyncUVRM*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        pThis->m_ac.apost(ret);
        delete pThis;
    }

    void remove_next_entry()
    {
        if (m_entries.empty()) {
            // All entries removed, now remove the directory itself
            uv_fs_req_cleanup(this);
            int32_t ret = uv_fs_rmdir(s_uv_loop, this, m_path.c_str(), cb_rmdir);
            if (ret != 0) {
                setSystemErrorPayload(ret, "rmdir", m_requestPath);
                m_ac.apost(ret);
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
                setSystemErrorPayload(ret, "rmdir", m_requestPath);
                m_ac.apost(ret);
                delete subRemover;
                delete this;
            }
        } else {
            // For files, unlink directly
            int32_t ret = uv_fs_unlink(s_uv_loop, this, entry_path.c_str(), cb_entry_removed);
            if (ret != 0) {
                setSystemErrorPayload(ret, "rmdir", m_requestPath);
                m_ac.apost(ret);
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

        int32_t post(int32_t hr) override
        {
            if (hr < 0) {
                m_parent->m_ac.apost(hr);
                delete m_parent;
            } else {
                m_parent->remove_next_entry();
            }
            delete this;
            return 0;
        }

    private:
        AsyncUVRM* m_parent;
    };

private:
    AsyncHandle m_ac;
    exlib::string m_requestPath;
    exlib::string m_path;
    bool m_rmFile;
    bool m_force;
    std::vector<std::pair<exlib::string, uv_dirent_type_t>> m_entries;
};

result_t fs_base::mkdtemp(exlib::string prefix, exlib::string& retVal, AsyncHandle ac)
{
    if (ac.isSync())
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

result_t fs_base::rmdir(exlib::string path, v8::Local<v8::Object> opt, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        bool recursive = false;
        GetConfigValue(opt, "recursive", recursive);
        ac.ctxv()[0] = recursive;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    bool recursive = ac.ctxv()[0].boolVal();

    if (!recursive) {
        AutoReq req;
        return setSystemErrorPayload(uv_fs_rmdir(NULL, &req, path.c_str(), NULL), "rmdir", path);
    }

    os_resolve(path);

    return uv_async([&] {
        return uv_fs_lstat(s_uv_loop, new AsyncUVRM(path, false, false, std::move(ac)), path.c_str(), AsyncUVRM::cb_stat);
    });
}

result_t fs_base::rm(exlib::string path, v8::Local<v8::Object> opt, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(2);

        bool recursive = false;
        GetConfigValue(opt, "recursive", recursive);
        ac.ctxv()[0] = recursive;

        bool force = false;
        GetConfigValue(opt, "force", force);
        ac.ctxv()[1] = force;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;

    bool recursive = ac.ctxv()[0].boolVal();
    bool force = ac.ctxv()[1].boolVal();

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
            return CHECK_ERROR(setSystemErrorPayload(ret, "lstat", path));
        }

        if (S_ISDIR(req.statbuf.st_mode)) {
            uv_fs_req_cleanup(&req);
            // Node.js: SystemError + ERR_FS_EISDIR, errno is the raw value
            return setErrorPayload(UV_EISDIR,
                ErrorPayload::make(errtype::kError)
                    .with_type_name("SystemError")
                    .with_code("ERR_FS_EISDIR")
                    .with_errno(-UV_EISDIR)
                    .with_syscall("rm")
                    .with_path(path)
                    .format("Path is a directory: rm returned EISDIR (is a directory) %s", path.c_str()));
        }

        uv_fs_req_cleanup(&req);
        return CHECK_ERROR(setSystemErrorPayload(uv_fs_unlink(NULL, &req, path.c_str(), NULL), "unlink", path));
    }

    os_resolve(path);

    return uv_async([&] {
        return uv_fs_lstat(s_uv_loop, new AsyncUVRM(path, true, force, std::move(ac)), path.c_str(), AsyncUVRM::cb_stat);
    });
}

result_t fs_base::fchmod(Union_fchmod_fd fd, int32_t mode, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js validates a numeric mode as an unsigned 32-bit integer.
    if (mode < 0)
        return CHECK_ERROR(setOutOfRange("mode", ">= 0 && <= 4294967295", std::to_string(mode)));

    obj_ptr<FileHandle_base> handle;
    result_t hr = filehandle_from_union(fd, handle);
    if (hr < 0)
        return hr;

    int32_t _fd;
    handle->get_fd(_fd);

    AutoReq req;
    return setSystemErrorPayload(uv_fs_fchmod(NULL, &req, _fd, mode, NULL), "fchmod");
}

result_t fs_base::fchown(Union_fchown_fd fd, int32_t uid, int32_t gid, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<FileHandle_base> handle;
    result_t hr = filehandle_from_union(fd, handle);
    if (hr < 0)
        return hr;

    int32_t _fd;
    handle->get_fd(_fd);

    AutoReq req;
    return setSystemErrorPayload(uv_fs_fchown(NULL, &req, _fd, uid, gid, NULL), "fchown");
}

result_t fs_base::fsync(Union_fsync_fd fd, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<FileHandle_base> handle;
    result_t hr = filehandle_from_union(fd, handle);
    if (hr < 0)
        return hr;

    int32_t _fd;
    handle->get_fd(_fd);

    AutoReq req;
    return setSystemErrorPayload(uv_fs_fsync(NULL, &req, _fd, NULL), "fsync");
}

result_t fs_base::ftruncate(Union_ftruncate_fd fd, int32_t len, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<FileHandle_base> handle;
    result_t hr = filehandle_from_union(fd, handle);
    if (hr < 0)
        return hr;

    int32_t _fd;
    hr = handle->get_fd(_fd);
    if (hr < 0)
        return hr;

    // Node.js compatibility: a negative length is treated as zero
    if (len < 0)
        len = 0;

    AutoReq req;
    return setSystemErrorPayload(uv_fs_ftruncate(NULL, &req, _fd, len, NULL), "ftruncate");
}

result_t fs_base::statfs(exlib::string path, obj_ptr<StatfsType>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    int32_t ret = uv_fs_statfs(NULL, &req, path.c_str(), NULL);
    if (ret < 0)
        return setSystemErrorPayload(ret, "statfs", path);

    uv_statfs_t* st = (uv_statfs_t*)req.ptr;

    retVal = new StatfsType();
    retVal->type = (double)st->f_type;
    retVal->bsize = (double)st->f_bsize;
    retVal->blocks = (double)st->f_blocks;
    retVal->bfree = (double)st->f_bfree;
    retVal->bavail = (double)st->f_bavail;
    retVal->files = (double)st->f_files;
    retVal->ffree = (double)st->f_ffree;

    return 0;
}

// the numeric form; the merged entry dispatches here, and so does the string
// form's async phase (which carries the parsed mode through m_ctx)
static result_t chmod_numeric(exlib::string path, int32_t mode, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js validates a numeric mode as an unsigned 32-bit integer.
    if (mode < 0)
        return CHECK_ERROR(setOutOfRange("mode", ">= 0 && <= 4294967295", std::to_string(mode)));

    AutoReq req;
    return setSystemErrorPayload(uv_fs_chmod(NULL, &req, path.c_str(), mode, NULL), "chmod", path);
}

result_t fs_base::chmod(exlib::string path, Union_chmod_mode mode, AsyncHandle ac)
{
    if (std::holds_alternative<int32_t>(mode))
        return chmod_numeric(path, std::get<int32_t>(mode), std::move(ac));

    // a string mode (or any other value) is parsed in the sync phase; the
    // number travels to the async phase through m_ctx
    Variant mode_value = std::get<Variant>(mode);

    if (ac.isSync()) {
        ac.ctxv().resize(1);

        int32_t _mode;
        result_t hr = to_mode_value(mode_value, _mode);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = _mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    return chmod_numeric(path, ac.ctxv()[0].intVal(), std::move(ac));
}

// the platform-specific numeric lchmod: fs_posix.cpp implements it, fs_win32.cpp
// reports that it is unsupported; the merged entry below and the string form's
// async phase both dispatch here
result_t lchmod_platform(exlib::string path, int32_t mode, AsyncHandle ac);

result_t fs_base::lchmod(exlib::string path, Union_lchmod_mode mode, AsyncHandle ac)
{
    if (std::holds_alternative<int32_t>(mode))
        return lchmod_platform(path, std::get<int32_t>(mode), std::move(ac));

    // a string mode (or any other value) is parsed in the sync phase; the
    // number travels to the async phase through m_ctx
    Variant mode_value = std::get<Variant>(mode);

    if (ac.isSync()) {
        ac.ctxv().resize(1);

        int32_t _mode;
        result_t hr = to_mode_value(mode_value, _mode);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = _mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    return lchmod_platform(path, ac.ctxv()[0].intVal(), std::move(ac));
}

result_t fs_base::chown(exlib::string path, int32_t uid, int32_t gid, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    AutoReq req;
    return setSystemErrorPayload(uv_fs_chown(NULL, &req, path.c_str(), uid, gid, NULL), "chown", path);
}

result_t fs_base::lchown(exlib::string path, int32_t uid, int32_t gid, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    AutoReq req;
    return setSystemErrorPayload(uv_fs_lchown(NULL, &req, path.c_str(), uid, gid, NULL), "lchown", path);
}

result_t fs_base::utimes(exlib::string path, Variant atime, Variant mtime, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(2);

        double _atime, _mtime;
        result_t hr = to_unix_timestamp(atime, _atime);
        if (hr < 0)
            return hr;

        hr = to_unix_timestamp(mtime, _mtime);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = _atime;
        ac.ctxv()[1] = _mtime;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;

    AutoReq req;
    return setSystemErrorPayload(uv_fs_utime(NULL, &req, path.c_str(), ac.ctxv()[0].dblVal(), ac.ctxv()[1].dblVal(), NULL), "utime", path);
}

result_t fs_base::lutimes(exlib::string path, Variant atime, Variant mtime, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(2);

        double _atime, _mtime;
        result_t hr = to_unix_timestamp(atime, _atime);
        if (hr < 0)
            return hr;

        hr = to_unix_timestamp(mtime, _mtime);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = _atime;
        ac.ctxv()[1] = _mtime;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;

    AutoReq req;
    return setSystemErrorPayload(uv_fs_lutime(NULL, &req, path.c_str(), ac.ctxv()[0].dblVal(), ac.ctxv()[1].dblVal(), NULL), "lutime", path);
}

result_t fs_base::futimes(Union_futimes_fd fd, Variant atime, Variant mtime, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(2);

        double _atime, _mtime;
        result_t hr = to_unix_timestamp(atime, _atime);
        if (hr < 0)
            return hr;

        hr = to_unix_timestamp(mtime, _mtime);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = _atime;
        ac.ctxv()[1] = _mtime;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    int32_t _fd;
    result_t hr = fd_from_union(fd, _fd);
    if (hr < 0)
        return hr;

    result_t ctx_hr = ac.ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;

    AutoReq req;
    return setSystemErrorPayload(uv_fs_futime(NULL, &req, _fd, ac.ctxv()[0].dblVal(), ac.ctxv()[1].dblVal(), NULL), "futime");
}

result_t fs_base::rename(exlib::string from, exlib::string to, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(from, from);
    if (hr < 0)
        return hr;

    hr = normalize_file_path_like(to, to);
    if (hr < 0)
        return hr;

    AutoReq req;
    return setSystemErrorPayload2(uv_fs_rename(NULL, &req, from.c_str(), to.c_str(), NULL),
        "rename", from, to);
}

result_t fs_base::fdatasync(Union_fdatasync_fd fd, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<FileHandle_base> handle;
    result_t hr = filehandle_from_union(fd, handle);
    if (hr < 0)
        return hr;

    int32_t _fd;
    handle->get_fd(_fd);

    AutoReq req;
    return setSystemErrorPayload(uv_fs_fdatasync(NULL, &req, _fd, NULL), "fdatasync");
}

result_t fs_base::copyFile(exlib::string from, exlib::string to, int32_t mode, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(from, from);
    if (hr < 0)
        return hr;

    hr = normalize_file_path_like(to, to);
    if (hr < 0)
        return hr;

    AutoReq req;
    return setSystemErrorPayload2(uv_fs_copyfile(NULL, &req, from.c_str(), to.c_str(), mode, NULL),
        "copyfile", from, to);
}

class AsyncUVCP : public uv_fs_t {
public:
    AsyncUVCP(exlib::string src, exlib::string dest, bool recursive, bool force, bool errorOnExist, int32_t mode, AsyncHandle ac)
        : m_ac(std::move(ac))
        , m_requestPath(src)
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
            setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);
            pThis->m_ac.apost(ret);
            delete pThis;
            return;
        }

        if (S_ISDIR(pThis->statbuf.st_mode)) {
            if (!pThis->m_recursive) {
                // Node.js: a plain Error with ERR_FS_EISDIR and no errno (it is
                // not a system error): the description travels through the
                // exception channel instead of the raw uv code.
                Runtime::setError(ErrorPayload::make(errtype::kError)
                        .with_code("ERR_FS_EISDIR")
                        .format("Recursive option not enabled, cannot copy a directory: %s/",
                            pThis->m_requestPath.c_str()));
                pThis->m_ac.apost(CALL_E_EXCEPTION);
                delete pThis;
                return;
            }

            // Create destination directory
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_mkdir(s_uv_loop, pThis, pThis->m_dest.c_str(), pThis->statbuf.st_mode & 0777, cb_mkdir);
            if (ret != 0) {
                setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
                delete pThis;
            }
        } else if (S_ISREG(pThis->statbuf.st_mode)) {
            // Copy file directly
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_copyfile(s_uv_loop, pThis, pThis->m_src.c_str(), pThis->m_dest.c_str(),
                pThis->m_mode, cb_copyfile);
            if (ret != 0) {
                setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
                delete pThis;
            }
        } else {
            // Symlinks and other types: try copyfile
            uv_fs_req_cleanup(pThis);
            ret = uv_fs_copyfile(s_uv_loop, pThis, pThis->m_src.c_str(), pThis->m_dest.c_str(),
                pThis->m_mode, cb_copyfile);
            if (ret != 0) {
                setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);
                pThis->m_ac.apost(ret);
                delete pThis;
            }
        }
    }

    static void cb_mkdir(uv_fs_t* req)
    {
        AsyncUVCP* pThis = (AsyncUVCP*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        if (ret < 0 && ret != UV_EEXIST) {
            setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);
            pThis->m_ac.apost(ret);
            delete pThis;
            return;
        }

        // Scan source directory
        uv_fs_req_cleanup(pThis);
        ret = uv_fs_scandir(s_uv_loop, pThis, pThis->m_src.c_str(), 0, cb_scandir);
        if (ret != 0) {
            setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);
            pThis->m_ac.apost(ret);
            delete pThis;
        }
    }

    static void cb_scandir(uv_fs_t* req)
    {
        AsyncUVCP* pThis = (AsyncUVCP*)req;
        int32_t ret = (int32_t)uv_fs_get_result(req);

        if (ret < 0) {
            setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);
            pThis->m_ac.apost(ret);
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

        if (ret == UV_EEXIST && pThis->m_errorOnExist) {
            // Node.js: SystemError + ERR_FS_CP_EEXIST
            setErrorPayload(ret,
                ErrorPayload::make(errtype::kError)
                    .with_type_name("SystemError")
                    .with_code("ERR_FS_CP_EEXIST")
                    .with_errno(-UV_EEXIST)
                    .with_syscall("cp")
                    .with_path(pThis->m_dest)
                    .format("Target already exists: cp returned EEXIST (%s already exists) %s",
                        pThis->m_dest.c_str(), pThis->m_dest.c_str()));
        } else if (ret < 0)
            setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);

        pThis->m_ac.apost(ret);
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
            setSystemErrorPayload(ret, "copyfile", pThis->m_requestPath);
            pThis->m_ac.apost(ret);
            delete pThis;
            return;
        }

        pThis->copy_next_entry();
    }

    void copy_next_entry()
    {
        if (m_entries.empty()) {
            // All entries copied
            m_ac.apost(0);
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
                setSystemErrorPayload(ret, "copyfile", m_requestPath);
                m_ac.apost(ret);
                delete subCopier;
                delete this;
            }
        } else {
            // For files, copy directly
            int32_t ret = uv_fs_copyfile(s_uv_loop, this, src_path.c_str(), dest_path.c_str(),
                m_mode, cb_entry_copied);
            if (ret != 0) {
                setSystemErrorPayload(ret, "copyfile", m_requestPath);
                m_ac.apost(ret);
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

        int32_t post(int32_t hr) override
        {
            if (hr < 0) {
                m_parent->m_ac.apost(hr);
                delete m_parent;
            } else {
                m_parent->copy_next_entry();
            }
            delete this;
            return 0;
        }

    private:
        AsyncUVCP* m_parent;
    };

private:
    AsyncHandle m_ac;
    exlib::string m_requestPath;
    exlib::string m_src;
    exlib::string m_dest;
    bool m_recursive;
    bool m_force;
    bool m_errorOnExist;
    int32_t m_mode;
    std::vector<std::pair<exlib::string, uv_dirent_type_t>> m_entries;
};

result_t fs_base::cp(exlib::string src, exlib::string dest, v8::Local<v8::Object> opts, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(4);

        // Node.js: copying onto itself is refused up front.
        if (src == dest)
            return CHECK_ERROR(Runtime::setError(ErrorPayload::make(errtype::kError)
                    .with_code("ERR_FS_CP_EINVAL")
                    .format("src and dest cannot be the same %s", src.c_str())));

        bool recursive = false;
        GetConfigValue(opts, "recursive", recursive);
        ac.ctxv()[0] = recursive;

        bool force = true;
        GetConfigValue(opts, "force", force);
        ac.ctxv()[1] = force;

        bool errorOnExist = false;
        GetConfigValue(opts, "errorOnExist", errorOnExist);
        ac.ctxv()[2] = errorOnExist;

        int32_t mode = 0;
        GetConfigValue(opts, "mode", mode);
        ac.ctxv()[3] = mode;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(3);
    if (ctx_hr < 0)
        return ctx_hr;

    bool recursive = ac.ctxv()[0].boolVal();
    bool force = ac.ctxv()[1].boolVal();
    bool errorOnExist = ac.ctxv()[2].boolVal();
    int32_t mode = ac.ctxv()[3].intVal();

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
        return uv_fs_stat(s_uv_loop, new AsyncUVCP(src, dest, recursive, force, errorOnExist, mode, std::move(ac)), src.c_str(), AsyncUVCP::cb_stat);
    });
}

result_t fs_base::opendir(exlib::string path, obj_ptr<Dir_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new Dir(path);

    return 0;
}

result_t fs_base::readdir(exlib::string path, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = normalize_file_path_like(path, path);
    if (hr < 0)
        return hr;

    AutoReq req;
    int32_t ret = uv_fs_scandir(NULL, &req, path.c_str(), 0, NULL);
    if (ret < 0)
        return setSystemErrorPayload(ret, "scandir", path);

    retVal = new NArray();
    uv_dirent_t dirent;

    while (uv_fs_scandir_next(&req, &dirent) != UV_EOF)
        retVal->append(dirent.name);

    return 0;
}

// Node.js: readdir's `encoding` option controls how entry names are returned
static result_t append_dirent_name(obj_ptr<NArray>& list, const char* name, exlib::string& encoding)
{
    // Node.js validates the encoding up front, before adding any entry.
    {
        result_t hr = check_encoding(encoding);
        if (hr < 0)
            return hr;
    }

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
    obj_ptr<NArray>& retVal, AsyncHandle ac);

// the options form: recursive/withFileTypes/encoding are read in the sync
// phase and travel to the async phase through m_ctx
static result_t readdir_options(exlib::string path, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(3);

        bool recursive = false;
        GetConfigValue(opts, "recursive", recursive);
        ac.ctxv()[0] = recursive;

        bool withFileTypes = false;
        GetConfigValue(opts, "withFileTypes", withFileTypes);
        ac.ctxv()[1] = withFileTypes;

        exlib::string encoding;
        GetConfigValue(opts, "encoding", encoding);
        ac.ctxv()[2] = encoding;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(2);
    if (ctx_hr < 0)
        return ctx_hr;

    return readdir_ext(path, ac.ctxv()[0].boolVal(), ac.ctxv()[1].boolVal(), ac.ctxv()[2].string(), retVal, std::move(ac));
}

// Node.js: readdir(path, encoding) - names are returned in the given encoding
static result_t readdir_encoding(exlib::string path, exlib::string encoding, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return readdir_ext(path, false, false, encoding, retVal, std::move(ac));
}

result_t fs_base::readdir(exlib::string path, Union_readdir_opts opts, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<exlib::string>(opts))
        return readdir_encoding(path, std::get<exlib::string>(opts), retVal, std::move(ac));

    return readdir_options(path, std::get<v8::Local<v8::Object>>(opts), retVal, std::move(ac));
}

// shared readdir implementation
static result_t readdir_ext(exlib::string path, bool recursive, bool withFileTypes, exlib::string encoding,
    obj_ptr<NArray>& retVal, AsyncHandle ac)
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

    // The open failure (description and payload) belongs to the calling thread;
    // the emit runs on the JS thread, so both are carried across.
    Runtime::ErrorDescription desc = Runtime::captureErrorDescription(hr);
    ErrorPayload payload = takeErrorPayload();

    async([stm, hr, desc, payload, isolate]() {
        // the emit must run on the JS thread with a valid context
        isolate->sync([stm, hr, desc, payload]() -> int {
            Runtime::applyErrorDescription(desc, payload);

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
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(3);

        exlib::string flags = "r";
        GetConfigValue(options, "flags", flags);
        ac.ctxv()[0] = flags;

        int64_t start = -1;
        GetConfigValue(options, "start", start);
        ac.ctxv()[1] = start;

        int64_t end = -1;
        GetConfigValue(options, "end", end);
        ac.ctxv()[2] = end;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(2);
    if (ctx_hr < 0)
        return ctx_hr;

    exlib::string flags = ac.ctxv()[0].string();
    int64_t start = ac.ctxv()[1].longVal();
    int64_t end = ac.ctxv()[2].longVal();

    obj_ptr<SeekableStream_base> stm;
    result_t hr = openFile(fname, flags, stm, std::move(ac));
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
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(2);

        exlib::string flags = "w";
        GetConfigValue(options, "flags", flags);
        ac.ctxv()[0] = flags;

        int64_t start = -1;
        GetConfigValue(options, "start", start);
        ac.ctxv()[1] = start;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;

    exlib::string flags = ac.ctxv()[0].string();
    int64_t start = ac.ctxv()[1].longVal();

    obj_ptr<SeekableStream_base> stm;
    result_t hr = openFile(fname, flags, stm, std::move(ac));
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
