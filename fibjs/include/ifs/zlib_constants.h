/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class zlib_constants_base : public object_base {
    DECLARE_CLASS(zlib_constants_base);

public:
    enum {
        C_Z_NO_FLUSH = 0,
        C_Z_PARTIAL_FLUSH = 1,
        C_Z_SYNC_FLUSH = 2,
        C_Z_FULL_FLUSH = 3,
        C_Z_FINISH = 4,
        C_Z_BLOCK = 5,
        C_Z_OK = 0,
        C_Z_STREAM_END = 1,
        C_Z_NEED_DICT = 2,
        C_Z_ERRNO = -1,
        C_Z_STREAM_ERROR = -2,
        C_Z_DATA_ERROR = -3,
        C_Z_MEM_ERROR = -4,
        C_Z_BUF_ERROR = -5,
        C_Z_VERSION_ERROR = -6,
        C_Z_NO_COMPRESSION = 0,
        C_Z_BEST_SPEED = 1,
        C_Z_BEST_COMPRESSION = 9,
        C_Z_DEFAULT_COMPRESSION = -1,
        C_Z_FILTERED = 1,
        C_Z_HUFFMAN_ONLY = 2,
        C_Z_RLE = 3,
        C_Z_FIXED = 4,
        C_Z_DEFAULT_STRATEGY = 0,
        C_ZLIB_VERNUM = 4800,
        C_DEFLATE = 1,
        C_INFLATE = 2,
        C_GZIP = 3,
        C_GUNZIP = 4,
        C_DEFLATERAW = 5,
        C_INFLATERAW = 6,
        C_UNZIP = 7,
        C_BROTLI_DECODE = 8,
        C_BROTLI_ENCODE = 9,
        C_Z_MIN_WINDOWBITS = 8,
        C_Z_MAX_WINDOWBITS = 15,
        C_Z_DEFAULT_WINDOWBITS = 15,
        C_Z_MIN_CHUNK = 64,
        C_Z_MAX_CHUNK = -1,
        C_Z_DEFAULT_CHUNK = 16384,
        C_Z_MIN_MEMLEVEL = 1,
        C_Z_MAX_MEMLEVEL = 9,
        C_Z_DEFAULT_MEMLEVEL = 8,
        C_Z_MIN_LEVEL = -1,
        C_Z_MAX_LEVEL = 9,
        C_Z_DEFAULT_LEVEL = -1
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<zlib_constants_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }
};
}

namespace fibjs {
inline ClassInfo& zlib_constants_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "Z_NO_FLUSH", ClassData::CONST_Integer, { .intValue = C_Z_NO_FLUSH } },
        { "Z_PARTIAL_FLUSH", ClassData::CONST_Integer, { .intValue = C_Z_PARTIAL_FLUSH } },
        { "Z_SYNC_FLUSH", ClassData::CONST_Integer, { .intValue = C_Z_SYNC_FLUSH } },
        { "Z_FULL_FLUSH", ClassData::CONST_Integer, { .intValue = C_Z_FULL_FLUSH } },
        { "Z_FINISH", ClassData::CONST_Integer, { .intValue = C_Z_FINISH } },
        { "Z_BLOCK", ClassData::CONST_Integer, { .intValue = C_Z_BLOCK } },
        { "Z_OK", ClassData::CONST_Integer, { .intValue = C_Z_OK } },
        { "Z_STREAM_END", ClassData::CONST_Integer, { .intValue = C_Z_STREAM_END } },
        { "Z_NEED_DICT", ClassData::CONST_Integer, { .intValue = C_Z_NEED_DICT } },
        { "Z_ERRNO", ClassData::CONST_Integer, { .intValue = C_Z_ERRNO } },
        { "Z_STREAM_ERROR", ClassData::CONST_Integer, { .intValue = C_Z_STREAM_ERROR } },
        { "Z_DATA_ERROR", ClassData::CONST_Integer, { .intValue = C_Z_DATA_ERROR } },
        { "Z_MEM_ERROR", ClassData::CONST_Integer, { .intValue = C_Z_MEM_ERROR } },
        { "Z_BUF_ERROR", ClassData::CONST_Integer, { .intValue = C_Z_BUF_ERROR } },
        { "Z_VERSION_ERROR", ClassData::CONST_Integer, { .intValue = C_Z_VERSION_ERROR } },
        { "Z_NO_COMPRESSION", ClassData::CONST_Integer, { .intValue = C_Z_NO_COMPRESSION } },
        { "Z_BEST_SPEED", ClassData::CONST_Integer, { .intValue = C_Z_BEST_SPEED } },
        { "Z_BEST_COMPRESSION", ClassData::CONST_Integer, { .intValue = C_Z_BEST_COMPRESSION } },
        { "Z_DEFAULT_COMPRESSION", ClassData::CONST_Integer, { .intValue = C_Z_DEFAULT_COMPRESSION } },
        { "Z_FILTERED", ClassData::CONST_Integer, { .intValue = C_Z_FILTERED } },
        { "Z_HUFFMAN_ONLY", ClassData::CONST_Integer, { .intValue = C_Z_HUFFMAN_ONLY } },
        { "Z_RLE", ClassData::CONST_Integer, { .intValue = C_Z_RLE } },
        { "Z_FIXED", ClassData::CONST_Integer, { .intValue = C_Z_FIXED } },
        { "Z_DEFAULT_STRATEGY", ClassData::CONST_Integer, { .intValue = C_Z_DEFAULT_STRATEGY } },
        { "ZLIB_VERNUM", ClassData::CONST_Integer, { .intValue = C_ZLIB_VERNUM } },
        { "DEFLATE", ClassData::CONST_Integer, { .intValue = C_DEFLATE } },
        { "INFLATE", ClassData::CONST_Integer, { .intValue = C_INFLATE } },
        { "GZIP", ClassData::CONST_Integer, { .intValue = C_GZIP } },
        { "GUNZIP", ClassData::CONST_Integer, { .intValue = C_GUNZIP } },
        { "DEFLATERAW", ClassData::CONST_Integer, { .intValue = C_DEFLATERAW } },
        { "INFLATERAW", ClassData::CONST_Integer, { .intValue = C_INFLATERAW } },
        { "UNZIP", ClassData::CONST_Integer, { .intValue = C_UNZIP } },
        { "BROTLI_DECODE", ClassData::CONST_Integer, { .intValue = C_BROTLI_DECODE } },
        { "BROTLI_ENCODE", ClassData::CONST_Integer, { .intValue = C_BROTLI_ENCODE } },
        { "Z_MIN_WINDOWBITS", ClassData::CONST_Integer, { .intValue = C_Z_MIN_WINDOWBITS } },
        { "Z_MAX_WINDOWBITS", ClassData::CONST_Integer, { .intValue = C_Z_MAX_WINDOWBITS } },
        { "Z_DEFAULT_WINDOWBITS", ClassData::CONST_Integer, { .intValue = C_Z_DEFAULT_WINDOWBITS } },
        { "Z_MIN_CHUNK", ClassData::CONST_Integer, { .intValue = C_Z_MIN_CHUNK } },
        { "Z_MAX_CHUNK", ClassData::CONST_Integer, { .intValue = C_Z_MAX_CHUNK } },
        { "Z_DEFAULT_CHUNK", ClassData::CONST_Integer, { .intValue = C_Z_DEFAULT_CHUNK } },
        { "Z_MIN_MEMLEVEL", ClassData::CONST_Integer, { .intValue = C_Z_MIN_MEMLEVEL } },
        { "Z_MAX_MEMLEVEL", ClassData::CONST_Integer, { .intValue = C_Z_MAX_MEMLEVEL } },
        { "Z_DEFAULT_MEMLEVEL", ClassData::CONST_Integer, { .intValue = C_Z_DEFAULT_MEMLEVEL } },
        { "Z_MIN_LEVEL", ClassData::CONST_Integer, { .intValue = C_Z_MIN_LEVEL } },
        { "Z_MAX_LEVEL", ClassData::CONST_Integer, { .intValue = C_Z_MAX_LEVEL } },
        { "Z_DEFAULT_LEVEL", ClassData::CONST_Integer, { .intValue = C_Z_DEFAULT_LEVEL } }
    };

    static ClassData s_cd = {
        "zlib_constants", true, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
