/*
 * zlib.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: lion
 */

#include "object.h"
#include "ZlibStream.h"
#include "ZstdStream.h"
#include "BrotliStream.h"
#include "MemoryStream.h"
#include "utils.h"
#include <cmath>

namespace fibjs {

DECLARE_MODULE(zlib);

// The `level` / `maxOutputLength` options of the union overloads are parsed in
// the sync phase (they need the JS options object) and carried to the async
// phase in ac.ctxv()[0].
static result_t parse_level_option(v8::Local<v8::Object> options, AsyncHandle ac)
{
    int32_t level = zlib_base::C_DEFAULT_COMPRESSION;
    result_t hr = GetConfigValue(options, "level", level, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    // Node.js rejects a level outside -1..9 rather than clamping it: a level
    // the caller did not ask for silently produces a different stream. The
    // check runs in the sync phase, so the error is thrown (or handed to the
    // callback) before any work is scheduled.
    hr = check_level(level);
    if (hr.failed())
        return hr;

    // The two other deflateInit2 knobs Node exposes on the one-shot forms.
    int32_t strategy = Z_DEFAULT_STRATEGY;
    hr = GetConfigValue(options, "strategy", strategy, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    int32_t memLevel = DEFAULT_MEM_LEVEL;
    hr = GetConfigValue(options, "memLevel", memLevel, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    if (memLevel < 1)
        memLevel = 1;
    else if (memLevel > 9)
        memLevel = 9;

    ac.ctxv().resize(3);
    ac.ctxv()[0] = level;
    ac.ctxv()[1] = strategy;
    ac.ctxv()[2] = memLevel;
    return result_t::nosync();
}

// Same for `maxOutputLength`, carried in ac.ctxv()[0].
static result_t parse_max_output_option(v8::Local<v8::Object> options, AsyncHandle ac)
{
    int32_t maxOutputLength = -1;
    result_t hr = GetConfigValue(options, "maxOutputLength", maxOutputLength, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    ac.ctxv().resize(1);
    ac.ctxv()[0] = maxOutputLength;
    return result_t::nosync();
}

// One named integer option of the zstd forms; a missing key reads 0, which the
// callers treat as "not set". The value is validated in the sync phase, so a
// mistyped option is reported before the work is scheduled.
static int32_t zstd_int_option(v8::Local<v8::Object> options, const char* name)
{
    int32_t value = 0;
    GetConfigValue(options, name, value, true);
    return value;
}

// `strategy` is a zstd strategy constant, but -1 means "not given": zstd then
// derives one from the compression level, which is what keeps a high level
// better than a low one.

static int32_t zstd_strategy_option(v8::Local<v8::Object> options)
{
    int32_t strategy = -1;
    GetConfigValue(options, "strategy", strategy, true);
    return strategy;
}

// The options of the zstd forms cross the two phases the way every other async
// function carries an options object (see crypto_base::sign): the sync phase -
// the only one that may read a v8 object - flattens `options` into the integer
// ctxv() slots, and the async phase reads the slots back without touching a
// handle the callback phase no longer owns.
//
// The named knobs take six slots; `params` is an arbitrary object of zstd
// parameter constants to values, so its entries follow as constant/value pairs
// and slot five holds their count.
#define ZSTD_OPT_SLOTS 6 // level, windowBits, nbWorkers, strategy, pledgedSrcSize, paramsCount

// The options of one zstd form, read from the JS object.
struct zstd_options {
    int32_t level = ZSTD_CLEVEL_DEFAULT;
    int32_t windowBits = 0;
    int32_t nbWorkers = 0;
    int32_t strategy = -1;
    int64_t pledgedSrcSize = -1;
    std::vector<std::pair<int32_t, int32_t>> params;
};

// One reader for both entry points: the synchronous factories build the stream
// directly, the async forms flatten the result into ctxv() and rebuild it. A
// single reader means the two can never disagree about what an option means.
static result_t zstd_options_from_options(v8::Local<v8::Object> options, zstd_options& opts)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    // 0 means "not given" for the level, and zstd's own default applies then.
    int32_t level = zstd_int_option(options, "level");
    opts.level = level != 0 ? level : ZSTD_CLEVEL_DEFAULT;
    opts.windowBits = zstd_int_option(options, "windowBits");
    opts.nbWorkers = zstd_int_option(options, "nbWorkers");
    opts.strategy = zstd_strategy_option(options);

    result_t hr = GetConfigValue(options, "pledgedSrcSize", opts.pledgedSrcSize, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    JSValue params_val = options->Get(context, isolate->NewString("params"));
    if (params_val->IsUndefined() || params_val->IsNull())
        return result_t();

    if (!params_val->IsObject())
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_TYPE")
            .with_message("The \"options.params\" property must be of type object");

    v8::Local<v8::Object> obj = params_val.As<v8::Object>();
    JSArray keys = obj->GetOwnPropertyNames(context);
    int32_t len = keys->Length();

    for (int32_t i = 0; i < len; i++) {
        JSValue k = keys->Get(context, i);
        exlib::string name = isolate->toString(k);

        // A key that is not a number names one of the codec options (or is
        // anything else the caller put there); it is not a parameter id.
        if (name.empty() || name[0] < '0' || name[0] > '9')
            continue;

        int32_t param = 0;
        if (GetArgumentValue(isolate, k, param, false).failed())
            continue;

        v8::Local<v8::Value> raw;
        if (!obj->Get(context, k).ToLocal(&raw))
            continue;

        int32_t value = 0;
        hr = GetArgumentValue(isolate, raw, value, true);
        if (hr.is_param_missing())
            continue;
        if (hr.failed())
            return hr;

        opts.params.push_back({ param, value });
    }

    return result_t();
}

static result_t zstd_options_to_ctx(v8::Local<v8::Object> options, AsyncHandle& ac)
{
    zstd_options opts;
    result_t hr = zstd_options_from_options(options, opts);
    if (hr.failed())
        return hr;

    ac.ctxv().resize(ZSTD_OPT_SLOTS + opts.params.size() * 2);

    ac.ctxv()[0] = opts.level;
    ac.ctxv()[1] = opts.windowBits;
    ac.ctxv()[2] = opts.nbWorkers;
    ac.ctxv()[3] = opts.strategy;
    ac.ctxv()[4] = opts.pledgedSrcSize;
    ac.ctxv()[5] = (int32_t)opts.params.size();

    for (size_t i = 0; i < opts.params.size(); i++) {
        ac.ctxv()[ZSTD_OPT_SLOTS + i * 2] = opts.params[i].first;
        ac.ctxv()[ZSTD_OPT_SLOTS + i * 2 + 1] = opts.params[i].second;
    }

    return result_t::nosync();
}

// Reads the slots written by zstd_options_to_ctx.
static result_t zstd_options_from_ctx(AsyncHandle& ac, zstd_options& opts)
{
    if (ac.ctxv().size() < ZSTD_OPT_SLOTS)
        return result_t::make(CALL_E_INTERNAL)
            .with_type_name(errtype::kError)
            .with_code("ERR_INVALID_STATE")
            .with_message("zlib: the zstd options were not read");

    opts.level = ac.ctxv()[0].intVal();
    opts.windowBits = ac.ctxv()[1].intVal();
    opts.nbWorkers = ac.ctxv()[2].intVal();
    opts.strategy = ac.ctxv()[3].intVal();
    opts.pledgedSrcSize = ac.ctxv()[4].longVal();

    int32_t count = ac.ctxv()[5].intVal();
    for (int32_t i = 0; i < count; i++) {
        size_t at = ZSTD_OPT_SLOTS + i * 2;
        if (ac.ctxv().size() <= at + 1)
            break;
        opts.params.push_back({ ac.ctxv()[at].intVal(), ac.ctxv()[at + 1].intVal() });
    }

    return result_t();
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
    return result_t();
}

// The `*To` families share one shape: `Buffer|Stream|String data` written into
// `stm`, with the process object (def/inf/gz/...) chosen as the template
// argument.
//
// The deflating streams clamp an out-of-range level to the zlib range, because
// a constructor has no way to report a failure. The callers that take a level
// from the user validate it in their sync phase, so a mistyped level is
// reported instead of silently applied - the behaviour Node.js has.
template <typename StreamT, typename DataVariant>
static result_t process_to(DataVariant& data, Stream_base* stm, int32_t param, AsyncHandle ac)
{
    if (std::holds_alternative<obj_ptr<Stream_base>>(data))
        return (new StreamT(stm, param))->process(std::get<obj_ptr<Stream_base>>(data), std::move(ac));

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    return (new StreamT(stm, param))->process(buf, std::move(ac));
}

// The create* factories accept a missing destination: the stream then writes into
// an internal MemoryStream so the call still produces a usable object, the way
// `zlib.createGzip()` does in Node.  The bytes stay inside the returned stream's
// destination, which `process()`/`readAll()` on the codec class exposes; a
// write-only stream alone cannot hand them back, so callers that need the output
// pass a destination (as before).
static Stream_base* default_dest(Stream_base* to)
{
    return to ? to : new MemoryStream();
}

result_t zlib_base::createDeflate(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new def(default_dest(to), -1);
    return result_t();
}

result_t zlib_base::createDeflateRaw(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new defraw(default_dest(to));
    return result_t();
}

result_t zlib_base::createGunzip(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal)
{
    retVal = new gunz(default_dest(to), maxSize);
    return result_t();
}

result_t zlib_base::createGzip(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new gz(default_dest(to));
    return result_t();
}

result_t zlib_base::createInflate(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal)
{
    retVal = new inf(default_dest(to), maxSize);
    return result_t();
}

result_t zlib_base::createInflateRaw(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal)
{
    retVal = new infraw(default_dest(to), maxSize);
    return result_t();
}

// One implementation for the whole union family: the sync phase extracts the
// options object, the async phase normalises both union parameters and then
// runs the original logic (plans/idl-union-types-2026-10-02.md §3.4).
result_t zlib_base::deflate(Union_deflate_data data, Union_deflate_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(level)
            ? parse_level_option(std::get<v8::Local<v8::Object>>(level), std::move(ac))
            : result_t::nosync();

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    int32_t lv;

    if (std::holds_alternative<int32_t>(level))
        lv = std::get<int32_t>(level);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr.failed())
            return ctx_hr;

        lv = ac.ctxv()[0].intVal();
    }

    return (new def(NULL, lv))->process(buf, retVal, std::move(ac));
}

result_t zlib_base::deflateTo(Union_deflateTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac)
{
    if (ac.isSync()) {
        result_t lhr = check_level(level);
        return lhr.failed() ? lhr : result_t::nosync();
    }

    return process_to<def>(data, stm, level, std::move(ac));
}


result_t zlib_base::inflate(Union_inflate_data data, Union_inflate_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(maxSize)
            ? parse_max_output_option(std::get<v8::Local<v8::Object>>(maxSize), std::move(ac))
            : result_t::nosync();

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    int32_t ms;

    if (std::holds_alternative<int32_t>(maxSize))
        ms = std::get<int32_t>(maxSize);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr.failed())
            return ctx_hr;

        ms = ac.ctxv()[0].intVal();
    }

    return (new inf(NULL, ms))->process(buf, retVal, std::move(ac));
}

result_t zlib_base::inflateTo(Union_inflateTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    return process_to<inf>(data, stm, maxSize, std::move(ac));
}


result_t zlib_base::gzip(Union_gzip_data data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return parse_level_option(options, std::move(ac));

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    // the compile-cache entry (cc_gzip) runs the async phase without the sync
    // one, so no options were parsed: fall back to the defaults
    int32_t lv = ac.ctxv().size() > 0 ? ac.ctxv()[0].intVal() : zlib_base::C_DEFAULT_COMPRESSION;
    int32_t strategy = ac.ctxv().size() > 1 ? ac.ctxv()[1].intVal() : Z_DEFAULT_STRATEGY;
    int32_t memLevel = ac.ctxv().size() > 2 ? ac.ctxv()[2].intVal() : DEFAULT_MEM_LEVEL;

    return (new gz(NULL, lv, strategy, memLevel))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::gzipTo(Union_gzipTo_data data, Stream_base* stm, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    return process_to<gz>(data, stm, -1, std::move(ac));
}



result_t zlib_base::gunzip(Union_gunzip_data data, Union_gunzip_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(maxSize)
            ? parse_max_output_option(std::get<v8::Local<v8::Object>>(maxSize), std::move(ac))
            : result_t::nosync();

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    int32_t ms;

    if (std::holds_alternative<int32_t>(maxSize))
        ms = std::get<int32_t>(maxSize);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr.failed())
            return ctx_hr;

        ms = ac.ctxv()[0].intVal();
    }

    return (new gunz(NULL, ms))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::gunzipTo(Union_gunzipTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    return process_to<gunz>(data, stm, maxSize, std::move(ac));
}


result_t zlib_base::deflateRaw(Union_deflateRaw_data data, Union_deflateRaw_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(level)
            ? parse_level_option(std::get<v8::Local<v8::Object>>(level), std::move(ac))
            : result_t::nosync();

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    int32_t lv;

    if (std::holds_alternative<int32_t>(level))
        lv = std::get<int32_t>(level);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr.failed())
            return ctx_hr;

        lv = ac.ctxv()[0].intVal();
    }

    return (new defraw(NULL, lv))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::deflateRawTo(Union_deflateRawTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac)
{
    if (ac.isSync()) {
        result_t lhr = check_level(level);
        return lhr.failed() ? lhr : result_t::nosync();
    }

    return process_to<defraw>(data, stm, level, std::move(ac));
}



result_t zlib_base::inflateRaw(Union_inflateRaw_data data, Union_inflateRaw_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(maxSize)
            ? parse_max_output_option(std::get<v8::Local<v8::Object>>(maxSize), std::move(ac))
            : result_t::nosync();

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    int32_t ms;

    if (std::holds_alternative<int32_t>(maxSize))
        ms = std::get<int32_t>(maxSize);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr.failed())
            return ctx_hr;

        ms = ac.ctxv()[0].intVal();
    }

    return (new infraw(NULL, ms))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::inflateRawTo(Union_inflateRawTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    return process_to<infraw>(data, stm, maxSize, std::move(ac));
}


result_t zlib_base::createZip(Stream_base* to, int32_t level, obj_ptr<Stream_base>& retVal)
{
    retVal = new class zip(default_dest(to), level);
    return result_t();
}

result_t zlib_base::createUnzip(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal)
{
    retVal = new class unzip(default_dest(to), maxSize);
    return result_t();
}

result_t zlib_base::zip(Union_zip_data data, Union_zip_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(level)
            ? parse_level_option(std::get<v8::Local<v8::Object>>(level), std::move(ac))
            : result_t::nosync();

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    int32_t lv;

    if (std::holds_alternative<int32_t>(level))
        lv = std::get<int32_t>(level);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr.failed())
            return ctx_hr;

        lv = ac.ctxv()[0].intVal();
    }

    return (new fibjs::zip(NULL, lv))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::zipTo(Union_zipTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac)
{
    if (ac.isSync()) {
        result_t lhr = check_level(level);
        return lhr.failed() ? lhr : result_t::nosync();
    }

    return process_to<fibjs::zip>(data, stm, level, std::move(ac));
}



result_t zlib_base::unzip(Union_unzip_data data, Union_unzip_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return std::holds_alternative<v8::Local<v8::Object>>(maxSize)
            ? parse_max_output_option(std::get<v8::Local<v8::Object>>(maxSize), std::move(ac))
            : result_t::nosync();

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    int32_t ms;

    if (std::holds_alternative<int32_t>(maxSize))
        ms = std::get<int32_t>(maxSize);
    else {
        // the options object was read in the sync phase
        result_t ctx_hr = ac.ctx(0);
        if (ctx_hr.failed())
            return ctx_hr;

        ms = ac.ctxv()[0].intVal();
    }

    return (new fibjs::unzip(NULL, ms))->process(buf, retVal, std::move(ac));
}


result_t zlib_base::unzipTo(Union_unzipTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    return process_to<fibjs::unzip>(data, stm, maxSize, std::move(ac));
}

result_t zlib_base::crc32(Union_crc32_data data, double value, int64_t& retVal)
{
    // The seed is a 32-bit register, and Node rejects anything that is not a
    // whole number in that range instead of truncating it: a truncated seed
    // silently continues a checksum the caller did not ask for, which is the
    // kind of defect that only shows up as a bad trailer much later.
    if (std::isnan(value) || std::trunc(value) != value || value < 0 || value > 4294967295.0)
        return result_t::make(CALL_E_OVERFLOW)
            .with_type_name(errtype::kRangeError)
            .with_code("ERR_OUT_OF_RANGE")
            .with_message("The value of \"value\" is out of range. It must be an integer >= 0 and <= 4294967295");

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    obj_ptr<Buffer> input = Buffer::Cast(buf);

    // The seed is the raw running register, not its complement: crc32's value
    // defaults to 0, which is what the first call starts from, and feeding a
    // returned value back continues the same checksum.
    uLong crc = (uLong)(uint32_t)value;
    crc = ::crc32(crc, (const Bytef*)input->data(), (uInt)input->length());

    retVal = (int64_t)(uint32_t)crc;
    return result_t();
}

result_t zlib_base::createZstdCompress(Stream_base* to, v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal)
{
    zstd_options opts;
    result_t hr = zstd_options_from_options(options, opts);
    if (hr.failed())
        return hr;

    zdstream* stm = new zdstream(default_dest(to), opts.level, opts.windowBits,
        opts.nbWorkers, -1, opts.strategy);
    stm->set_params(opts.params);

    retVal = stm;
    return result_t();
}

result_t zlib_base::createZstdDecompress(Stream_base* to, int32_t maxSize, v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal)
{
    zstd_options opts;
    result_t hr = zstd_options_from_options(options, opts);
    if (hr.failed())
        return hr;

    zistream* stm = new zistream(default_dest(to), maxSize, opts.windowBits);
    stm->set_params(opts.params);

    retVal = stm;
    return result_t();
}

result_t zlib_base::zstdCompress(Union_zstdCompress_data data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        result_t hr = zstd_options_to_ctx(options, ac);
        if (hr.failed())
            return hr;

        // The one-shot form knows the whole input, so its length becomes the
        // pledged source size unless the caller set one explicitly; that is
        // what lets the frame header carry the content size.
        if (ac.ctxv()[4].longVal() < 0) {
            obj_ptr<Buffer_base> tmp;
            result_t bhr = union_to_buffer(data, tmp);
            if (bhr.failed())
                return bhr;

            int32_t len = 0;
            result_t lhr = tmp->get_length(len);
            if (lhr.failed())
                return lhr;

            ac.ctxv()[4] = (int64_t)len;
        }

        return result_t::nosync();
    }

    zstd_options opts;
    result_t ctx_hr = zstd_options_from_ctx(ac, opts);
    if (ctx_hr.failed())
        return ctx_hr;

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    zdstream* stm = new zdstream(NULL, opts.level, opts.windowBits, opts.nbWorkers,
        opts.pledgedSrcSize, opts.strategy);
    stm->set_params(opts.params);

    return stm->process(buf, retVal, std::move(ac));
}

result_t zlib_base::zstdDecompress(Union_zstdDecompress_data data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return zstd_options_to_ctx(options, ac);

    zstd_options opts;
    result_t ctx_hr = zstd_options_from_ctx(ac, opts);
    if (ctx_hr.failed())
        return ctx_hr;

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    zistream* stm = new zistream(NULL, -1, opts.windowBits);
    stm->set_params(opts.params);

    return stm->process(buf, retVal, std::move(ac));
}

result_t zlib_base::zstdCompressTo(Union_zstdCompressTo_data data, Stream_base* stm, v8::Local<v8::Object> options, AsyncHandle ac)
{
    if (ac.isSync())
        return zstd_options_to_ctx(options, ac);

    zstd_options opts;
    result_t ctx_hr = zstd_options_from_ctx(ac, opts);
    if (ctx_hr.failed())
        return ctx_hr;

    zdstream* coder = new zdstream(stm, opts.level, opts.windowBits, opts.nbWorkers,
        -1, opts.strategy);
    coder->set_params(opts.params);

    if (std::holds_alternative<obj_ptr<Stream_base>>(data))
        return coder->process(std::get<obj_ptr<Stream_base>>(data), std::move(ac));

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    return coder->process(buf, std::move(ac));
}

result_t zlib_base::zstdDecompressTo(Union_zstdDecompressTo_data data, Stream_base* stm, int32_t maxSize, v8::Local<v8::Object> options, AsyncHandle ac)
{
    if (ac.isSync())
        return zstd_options_to_ctx(options, ac);

    zstd_options opts;
    result_t ctx_hr = zstd_options_from_ctx(ac, opts);
    if (ctx_hr.failed())
        return ctx_hr;

    zistream* coder = new zistream(stm, maxSize, opts.windowBits);
    coder->set_params(opts.params);

    if (std::holds_alternative<obj_ptr<Stream_base>>(data))
        return coder->process(std::get<obj_ptr<Stream_base>>(data), std::move(ac));

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    return coder->process(buf, std::move(ac));
}

result_t zlib_base::createBrotliCompress(Stream_base* to, v8::Local<v8::Object> options,
    obj_ptr<Stream_base>& retVal)
{
    // The stream lives in user code and is created synchronously, so the
    // options can be applied right away - no phase bridge is needed here.
    brotli_params params;
    result_t hr = brotli_options_from_options(options, false, params);
    if (hr.failed())
        return hr;

    retVal = new brstream(default_dest(to), params);
    return result_t();
}

result_t zlib_base::createBrotliDecompress(Stream_base* to, int32_t maxSize,
    v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal)
{
    brotli_params params;
    result_t hr = brotli_options_from_options(options, true, params);
    if (hr.failed())
        return hr;

    retVal = new bdstream(default_dest(to), maxSize, params);
    return result_t();
}

// Brotli takes one options object whose `params` keys are parameter ids; it is
// read in the sync phase and flattened into the handle, because the async phase
// runs on a worker fiber where v8 handles are not reachable.
result_t zlib_base::brotliCompress(Union_brotliCompress_data data,
    v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return brotli_options_to_ctx(options, false, ac);

    brotli_params params;
    result_t ctx_hr = brotli_options_from_ctx(ac, params);
    if (ctx_hr.failed())
        return ctx_hr;

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    return (new brstream(NULL, params))->process(buf, retVal, std::move(ac));
}

result_t zlib_base::brotliDecompress(Union_brotliDecompress_data data,
    v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return brotli_options_to_ctx(options, true, ac);

    brotli_params params;
    result_t ctx_hr = brotli_options_from_ctx(ac, params);
    if (ctx_hr.failed())
        return ctx_hr;

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    return (new bdstream(NULL, -1, params))->process(buf, retVal, std::move(ac));
}

result_t zlib_base::brotliCompressTo(Union_brotliCompressTo_data data,
    Stream_base* stm, v8::Local<v8::Object> options, AsyncHandle ac)
{
    if (ac.isSync())
        return brotli_options_to_ctx(options, false, ac);

    brotli_params params;
    result_t ctx_hr = brotli_options_from_ctx(ac, params);
    if (ctx_hr.failed())
        return ctx_hr;

    brstream* coder = new brstream(stm, params);

    if (std::holds_alternative<obj_ptr<Stream_base>>(data))
        return coder->process(std::get<obj_ptr<Stream_base>>(data), std::move(ac));

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    return coder->process(buf, std::move(ac));
}

result_t zlib_base::brotliDecompressTo(Union_brotliDecompressTo_data data,
    Stream_base* stm, int32_t maxSize, v8::Local<v8::Object> options, AsyncHandle ac)
{
    if (ac.isSync())
        return brotli_options_to_ctx(options, true, ac);

    brotli_params params;
    result_t ctx_hr = brotli_options_from_ctx(ac, params);
    if (ctx_hr.failed())
        return ctx_hr;

    bdstream* coder = new bdstream(stm, maxSize, params);

    if (std::holds_alternative<obj_ptr<Stream_base>>(data))
        return coder->process(std::get<obj_ptr<Stream_base>>(data), std::move(ac));

    obj_ptr<Buffer_base> buf;
    result_t hr = union_to_buffer(data, buf);
    if (hr.failed())
        return hr;

    return coder->process(buf, std::move(ac));
}

}
