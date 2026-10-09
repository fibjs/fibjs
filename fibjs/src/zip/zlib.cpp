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

// The `level` / `maxOutputLength` options of the union overloads are parsed in
// the sync phase (they need the JS options object) and carried to the async
// phase in ac.ctxv()[0].
static result_t parse_level_option(v8::Local<v8::Object> options, AsyncHandle ac)
{
    int32_t level = zlib_base::C_DEFAULT_COMPRESSION;
    result_t hr = GetConfigValue(options, "level", level, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    ac.ctxv().resize(1);
    ac.ctxv()[0] = level;
    return CALL_E_NOSYNC;
}

// Same for `maxOutputLength`, carried in ac.ctxv()[0].
static result_t parse_max_output_option(v8::Local<v8::Object> options, AsyncHandle ac)
{
    int32_t maxOutputLength = -1;
    result_t hr = GetConfigValue(options, "maxOutputLength", maxOutputLength, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    ac.ctxv().resize(1);
    ac.ctxv()[0] = maxOutputLength;
    return CALL_E_NOSYNC;
}

// The union of the `*To` families: a string is encoded as utf8, a buffer is
// passed as it is; both end up as the buffer form the process object takes.
template <typename DataVariant>
static result_t union_to_buffer(DataVariant& data, obj_ptr<Buffer_base>& buf)
{
    if (std::holds_alternative<exlib::string>(data)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", buf);
        return hr;
    }

    buf = std::get<obj_ptr<Buffer_base>>(data);
    return 0;
}

// The `*To` families share one shape: `Buffer|Stream|String data` written into
// `stm`, with the process object (def/inf/gz/...) chosen as the template
// argument.
template <typename StreamT, typename DataVariant>
static result_t process_to(DataVariant& data, Stream_base* stm, int32_t param, AsyncHandle ac)
{
    if (std::holds_alternative<obj_ptr<Stream_base>>(data))
        return (new StreamT(stm, param))->process(std::get<obj_ptr<Stream_base>>(data), std::move(ac));

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    return (new StreamT(stm, param))->process(buf, std::move(ac));
}

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

// One implementation for the whole union family: the sync phase extracts the
// options object, the async phase normalises both union parameters and then
// runs the original logic (plans/idl-union-types-2026-10-02.md §3.4).
result_t zlib_base::deflate(Union_deflate_data data, Union_deflate_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(level)
            ? parse_level_option(std::get<v8::Local<v8::Object>>(level), std::move(ac))
            : CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    int32_t lv;

    if (std::holds_alternative<int32_t>(level))
        lv = std::get<int32_t>(level);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        lv = ac.ctxv()[0].intVal();
    }

    return (new def(NULL, lv))->process(buf, retVal, std::move(ac));
}

result_t zlib_base::deflateTo(Union_deflateTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process_to<def>(data, stm, level, std::move(ac));
}


result_t zlib_base::inflate(Union_inflate_data data, Union_inflate_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(maxSize)
            ? parse_max_output_option(std::get<v8::Local<v8::Object>>(maxSize), std::move(ac))
            : CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    int32_t ms;

    if (std::holds_alternative<int32_t>(maxSize))
        ms = std::get<int32_t>(maxSize);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        ms = ac.ctxv()[0].intVal();
    }

    return (new inf(NULL, ms))->process(buf, retVal, std::move(ac));
}

result_t zlib_base::inflateTo(Union_inflateTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process_to<inf>(data, stm, maxSize, std::move(ac));
}


result_t zlib_base::gzip(Union_gzip_data data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return parse_level_option(options, std::move(ac));

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    // the compile-cache entry (cc_gzip) runs the async phase without the sync
    // one, so no options were parsed: fall back to the default level
    int32_t lv = ac.ctxv().size() > 0 ? ac.ctxv()[0].intVal() : zlib_base::C_DEFAULT_COMPRESSION;

    return (new gz(NULL, lv))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::gzipTo(Union_gzipTo_data data, Stream_base* stm, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process_to<gz>(data, stm, -1, std::move(ac));
}



result_t zlib_base::gunzip(Union_gunzip_data data, Union_gunzip_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(maxSize)
            ? parse_max_output_option(std::get<v8::Local<v8::Object>>(maxSize), std::move(ac))
            : CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    int32_t ms;

    if (std::holds_alternative<int32_t>(maxSize))
        ms = std::get<int32_t>(maxSize);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        ms = ac.ctxv()[0].intVal();
    }

    return (new gunz(NULL, ms))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::gunzipTo(Union_gunzipTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process_to<gunz>(data, stm, maxSize, std::move(ac));
}


result_t zlib_base::deflateRaw(Union_deflateRaw_data data, Union_deflateRaw_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(level)
            ? parse_level_option(std::get<v8::Local<v8::Object>>(level), std::move(ac))
            : CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    int32_t lv;

    if (std::holds_alternative<int32_t>(level))
        lv = std::get<int32_t>(level);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        lv = ac.ctxv()[0].intVal();
    }

    return (new defraw(NULL, lv))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::deflateRawTo(Union_deflateRawTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process_to<defraw>(data, stm, level, std::move(ac));
}



result_t zlib_base::inflateRaw(Union_inflateRaw_data data, Union_inflateRaw_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(maxSize)
            ? parse_max_output_option(std::get<v8::Local<v8::Object>>(maxSize), std::move(ac))
            : CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    int32_t ms;

    if (std::holds_alternative<int32_t>(maxSize))
        ms = std::get<int32_t>(maxSize);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        ms = ac.ctxv()[0].intVal();
    }

    return (new infraw(NULL, ms))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::inflateRawTo(Union_inflateRawTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process_to<infraw>(data, stm, maxSize, std::move(ac));
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

result_t zlib_base::zip(Union_zip_data data, Union_zip_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(level)
            ? parse_level_option(std::get<v8::Local<v8::Object>>(level), std::move(ac))
            : CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    int32_t lv;

    if (std::holds_alternative<int32_t>(level))
        lv = std::get<int32_t>(level);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        lv = ac.ctxv()[0].intVal();
    }

    return (new fibjs::zip(NULL, lv))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::zipTo(Union_zipTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process_to<fibjs::zip>(data, stm, level, std::move(ac));
}



result_t zlib_base::unzip(Union_unzip_data data, Union_unzip_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(maxSize)
            ? parse_max_output_option(std::get<v8::Local<v8::Object>>(maxSize), std::move(ac))
            : CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr < 0)
        return hr;

    int32_t ms;

    if (std::holds_alternative<int32_t>(maxSize))
        ms = std::get<int32_t>(maxSize);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr < 0)
            return ctx_hr;

        ms = ac.ctxv()[0].intVal();
    }

    return (new fibjs::unzip(NULL, ms))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::unzipTo(Union_unzipTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process_to<fibjs::unzip>(data, stm, maxSize, std::move(ac));
}



}
