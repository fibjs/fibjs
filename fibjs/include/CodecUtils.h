/*
 * CodecUtils.h
 *
 * Helpers the compression codecs share: zlib, zstd and brotli all implement the
 * same ZlibCodec interface, report the same errors and expose the same `_handle`
 * shim, so that logic lives here instead of being repeated per backend.
 */

#pragma once

#include "ifs/zlib.h"
#include "utils.h"

namespace fibjs {

// Node.js stops a decompression that would grow past `maxOutputLength` with
// ERR_BUFFER_TOO_LARGE (a RangeError). fibjs keeps its own error number (20013,
// the historical grouping every overflow shares) but adopts the code so that
// code branching on err.code behaves the same on both runtimes.
inline result_t max_output_exceeded(int32_t maxSize)
{
    return result_t::make(CALL_E_OVERFLOW)
        .with_type_name(errtype::kRangeError)
        .with_code("ERR_BUFFER_TOO_LARGE")
        .with_format("Cannot create a Buffer larger than %d bytes", maxSize);
}

// The compression level range zlib accepts. Node.js rejects anything outside
// it with ERR_OUT_OF_RANGE instead of clamping, because a level the caller did
// not ask for silently produces a different stream; fibjs follows that here so
// a mistyped level is reported rather than quietly applied as 0 or 9.
inline result_t check_level(int32_t level)
{
    if (level >= zlib_base::C_DEFAULT_COMPRESSION && level <= zlib_base::C_BEST_COMPRESSION)
        return result_t();

    return result_t::make(CALL_E_OVERFLOW)
        .with_type_name(errtype::kRangeError)
        .with_code("ERR_OUT_OF_RANGE")
        .with_format("The value of \"options.level\" is out of range. It must be >= %d and <= %d. Received %d",
            (int32_t)zlib_base::C_DEFAULT_COMPRESSION, (int32_t)zlib_base::C_BEST_COMPRESSION, level);
}

// Every codec of the module reports work on a closed instance the same way.
inline result_t codec_closed()
{
    return result_t::make(CALL_E_INVALID_CALL)
        .with_type_name(errtype::kError)
        .with_code("ERR_ZLIB_BINDING_CLOSED")
        .with_message("zlib binding closed");
}

// `options.params` is an object of numeric codec constants in every backend;
// anything else is a caller mistake and reported like Node.js does.
inline result_t codec_bad_params()
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_code("ERR_INVALID_ARG_TYPE")
        .with_message("The \"options.params\" property must be of type object");
}

// The `_handle` property Node.js codecs expose: an object whose close() does
// nothing. Packages reach for it after finishing a stream, so it has to exist,
// but it is a shim and the three backends build it identically.
inline result_t codec_get_handle(v8::Global<v8::Value>& handle, Isolate* isolate,
    v8::Local<v8::Value>& retVal)
{
    if (handle.IsEmpty()) {
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);
        v8::Local<v8::Function> closeFn;

        if (!v8::Function::New(context,
                [](const v8::FunctionCallbackInfo<v8::Value>& info) {})
                 .ToLocal(&closeFn))
            return result_t(CALL_E_JAVASCRIPT);

        obj->Set(context, isolate->NewString("close"), closeFn).IsJust();
        handle.Reset(isolate->m_isolate, obj);
    }

    retVal = handle.Get(isolate->m_isolate);
    return result_t();
}

inline result_t codec_set_handle(v8::Global<v8::Value>& handle, Isolate* isolate,
    v8::Local<v8::Value> newVal)
{
    handle.Reset(isolate->m_isolate, newVal);
    return result_t();
}

}
