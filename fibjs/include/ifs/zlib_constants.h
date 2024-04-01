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

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }
};
}

namespace fibjs {
inline ClassInfo& zlib_constants_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "Z_NO_FLUSH", C_Z_NO_FLUSH },
        { "Z_PARTIAL_FLUSH", C_Z_PARTIAL_FLUSH },
        { "Z_SYNC_FLUSH", C_Z_SYNC_FLUSH },
        { "Z_FULL_FLUSH", C_Z_FULL_FLUSH },
        { "Z_FINISH", C_Z_FINISH },
        { "Z_BLOCK", C_Z_BLOCK },
        { "Z_OK", C_Z_OK },
        { "Z_STREAM_END", C_Z_STREAM_END },
        { "Z_NEED_DICT", C_Z_NEED_DICT },
        { "Z_ERRNO", C_Z_ERRNO },
        { "Z_STREAM_ERROR", C_Z_STREAM_ERROR },
        { "Z_DATA_ERROR", C_Z_DATA_ERROR },
        { "Z_MEM_ERROR", C_Z_MEM_ERROR },
        { "Z_BUF_ERROR", C_Z_BUF_ERROR },
        { "Z_VERSION_ERROR", C_Z_VERSION_ERROR },
        { "Z_NO_COMPRESSION", C_Z_NO_COMPRESSION },
        { "Z_BEST_SPEED", C_Z_BEST_SPEED },
        { "Z_BEST_COMPRESSION", C_Z_BEST_COMPRESSION },
        { "Z_DEFAULT_COMPRESSION", C_Z_DEFAULT_COMPRESSION },
        { "Z_FILTERED", C_Z_FILTERED },
        { "Z_HUFFMAN_ONLY", C_Z_HUFFMAN_ONLY },
        { "Z_RLE", C_Z_RLE },
        { "Z_FIXED", C_Z_FIXED },
        { "Z_DEFAULT_STRATEGY", C_Z_DEFAULT_STRATEGY },
        { "ZLIB_VERNUM", C_ZLIB_VERNUM },
        { "DEFLATE", C_DEFLATE },
        { "INFLATE", C_INFLATE },
        { "GZIP", C_GZIP },
        { "GUNZIP", C_GUNZIP },
        { "DEFLATERAW", C_DEFLATERAW },
        { "INFLATERAW", C_INFLATERAW },
        { "UNZIP", C_UNZIP },
        { "BROTLI_DECODE", C_BROTLI_DECODE },
        { "BROTLI_ENCODE", C_BROTLI_ENCODE },
        { "Z_MIN_WINDOWBITS", C_Z_MIN_WINDOWBITS },
        { "Z_MAX_WINDOWBITS", C_Z_MAX_WINDOWBITS },
        { "Z_DEFAULT_WINDOWBITS", C_Z_DEFAULT_WINDOWBITS },
        { "Z_MIN_CHUNK", C_Z_MIN_CHUNK },
        { "Z_MAX_CHUNK", C_Z_MAX_CHUNK },
        { "Z_DEFAULT_CHUNK", C_Z_DEFAULT_CHUNK },
        { "Z_MIN_MEMLEVEL", C_Z_MIN_MEMLEVEL },
        { "Z_MAX_MEMLEVEL", C_Z_MAX_MEMLEVEL },
        { "Z_DEFAULT_MEMLEVEL", C_Z_DEFAULT_MEMLEVEL },
        { "Z_MIN_LEVEL", C_Z_MIN_LEVEL },
        { "Z_MAX_LEVEL", C_Z_MAX_LEVEL },
        { "Z_DEFAULT_LEVEL", C_Z_DEFAULT_LEVEL }
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
