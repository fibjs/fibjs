/*
 * zlib.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: lion
 */

#include "object.h"
#include "ZlibStream.h"
#include "utils.h"

namespace fibjs {

DECLARE_MODULE(zlib);

result_t zlib_base::createDeflate(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new def(to, -1);
    return 0;
}

result_t zlib_base::createDeflateRaw(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new defraw(to);
    return 0;
}

result_t zlib_base::createGunzip(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal)
{
    retVal = new gunz(to, maxSize);
    return 0;
}

result_t zlib_base::createGzip(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new gz(to);
    return 0;
}

result_t zlib_base::createInflate(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal)
{
    retVal = new inf(to, maxSize);
    return 0;
}

result_t zlib_base::createInflateRaw(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal)
{
    retVal = new infraw(to, maxSize);
    return 0;
}

result_t zlib_base::deflate(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(NULL, level))->process(data, retVal, ac);
}

result_t zlib_base::deflate(Buffer_base* data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        int32_t level = C_DEFAULT_COMPRESSION;
        result_t hr = GetConfigValue(options, "level", level, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = level;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new def(NULL, ac->m_ctx[0].intVal()))->process(data, retVal, ac);
}

result_t zlib_base::deflateTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(stm, level))->process(data, ac);
}

result_t zlib_base::deflateTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(stm, level))->process(src, ac);
}

result_t zlib_base::inflate(Buffer_base* data, int32_t maxSize, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(NULL, maxSize))->process(data, retVal, ac);
}

result_t zlib_base::inflate(Buffer_base* data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        int32_t maxOutputLength = -1;
        result_t hr = GetConfigValue(options, "maxOutputLength", maxOutputLength, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = maxOutputLength;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new inf(NULL, ac->m_ctx[0].intVal()))->process(data, retVal, ac);
}

result_t zlib_base::inflateTo(Buffer_base* data, Stream_base* stm, int32_t maxSize, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(stm, maxSize))->process(data, ac);
}

result_t zlib_base::inflateTo(Stream_base* src, Stream_base* stm, int32_t maxSize, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(stm, maxSize))->process(src, ac);
}

result_t zlib_base::gzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(NULL))->process(data, retVal, ac);
}

result_t zlib_base::gzip(Buffer_base* data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        int32_t level = C_DEFAULT_COMPRESSION;
        result_t hr = GetConfigValue(options, "level", level, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = level;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new gz(NULL, ac->m_ctx[0].intVal()))->process(data, retVal, ac);
}

result_t zlib_base::gzipTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(stm))->process(data, ac);
}

result_t zlib_base::gzipTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(stm))->process(src, ac);
}

result_t zlib_base::gunzip(Buffer_base* data, int32_t maxSize, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(NULL, maxSize))->process(data, retVal, ac);
}

result_t zlib_base::gunzip(Buffer_base* data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        int32_t maxOutputLength = -1;
        result_t hr = GetConfigValue(options, "maxOutputLength", maxOutputLength, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = maxOutputLength;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new gunz(NULL, ac->m_ctx[0].intVal()))->process(data, retVal, ac);
}

result_t zlib_base::gunzipTo(Buffer_base* data, Stream_base* stm, int32_t maxSize, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(stm, maxSize))->process(data, ac);
}

result_t zlib_base::gunzipTo(Stream_base* src, Stream_base* stm, int32_t maxSize, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(stm, maxSize))->process(src, ac);
}

result_t zlib_base::deflateRaw(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(NULL, level))->process(data, retVal, ac);
}

result_t zlib_base::deflateRaw(Buffer_base* data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        int32_t level = C_DEFAULT_COMPRESSION;
        result_t hr = GetConfigValue(options, "level", level, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = level;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new defraw(NULL, ac->m_ctx[0].intVal()))->process(data, retVal, ac);
}

result_t zlib_base::deflateRawTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(stm, level))->process(data, ac);
}

result_t zlib_base::deflateRawTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(stm, level))->process(src, ac);
}

result_t zlib_base::inflateRaw(Buffer_base* data, int32_t maxSize, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(NULL, maxSize))->process(data, retVal, ac);
}

result_t zlib_base::inflateRaw(Buffer_base* data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        int32_t maxOutputLength = -1;
        result_t hr = GetConfigValue(options, "maxOutputLength", maxOutputLength, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = maxOutputLength;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new infraw(NULL, ac->m_ctx[0].intVal()))->process(data, retVal, ac);
}

result_t zlib_base::inflateRawTo(Buffer_base* data, Stream_base* stm, int32_t maxSize, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(stm, maxSize))->process(data, ac);
}

result_t zlib_base::inflateRawTo(Stream_base* src, Stream_base* stm, int32_t maxSize, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(stm, maxSize))->process(src, ac);
}

result_t zlib_base::createZip(Stream_base* to, int32_t level, obj_ptr<Stream_base>& retVal)
{
    retVal = new class zip(to, level);
    return 0;
}

result_t zlib_base::createUnzip(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal)
{
    retVal = new class unzip(to, maxSize);
    return 0;
}

result_t zlib_base::zip(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new class zip(NULL, level))->process(data, retVal, ac);
}

result_t zlib_base::zip(Buffer_base* data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        int32_t level = C_DEFAULT_COMPRESSION;
        result_t hr = GetConfigValue(options, "level", level, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = level;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new class zip(NULL, ac->m_ctx[0].intVal()))->process(data, retVal, ac);
}

result_t zlib_base::zipTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new class zip(stm, level))->process(data, ac);
}

result_t zlib_base::zipTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new class zip(stm, level))->process(src, ac);
}

result_t zlib_base::unzip(Buffer_base* data, int32_t maxSize, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new class unzip(NULL, maxSize))->process(data, retVal, ac);
}

result_t zlib_base::unzip(Buffer_base* data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        int32_t maxOutputLength = -1;
        result_t hr = GetConfigValue(options, "maxOutputLength", maxOutputLength, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = maxOutputLength;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new class unzip(NULL, ac->m_ctx[0].intVal()))->process(data, retVal, ac);
}

result_t zlib_base::unzipTo(Buffer_base* data, Stream_base* stm, int32_t maxSize, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new class unzip(stm, maxSize))->process(data, ac);
}

result_t zlib_base::unzipTo(Stream_base* src, Stream_base* stm, int32_t maxSize, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new class unzip(stm, maxSize))->process(src, ac);
}
}
