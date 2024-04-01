/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description zlib 模块常用常量定义模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var constants = require('zlib').constants
 *  ```
 *  
 */
declare module 'zlib_constants' {
    export const Z_NO_FLUSH: 0;

    export const Z_PARTIAL_FLUSH: 1;

    export const Z_SYNC_FLUSH: 2;

    export const Z_FULL_FLUSH: 3;

    export const Z_FINISH: 4;

    export const Z_BLOCK: 5;

    export const Z_OK: 0;

    export const Z_STREAM_END: 1;

    export const Z_NEED_DICT: 2;

    export const Z_ERRNO: -1;

    export const Z_STREAM_ERROR: -2;

    export const Z_DATA_ERROR: -3;

    export const Z_MEM_ERROR: -4;

    export const Z_BUF_ERROR: -5;

    export const Z_VERSION_ERROR: -6;

    export const Z_NO_COMPRESSION: 0;

    export const Z_BEST_SPEED: 1;

    export const Z_BEST_COMPRESSION: 9;

    export const Z_DEFAULT_COMPRESSION: -1;

    export const Z_FILTERED: 1;

    export const Z_HUFFMAN_ONLY: 2;

    export const Z_RLE: 3;

    export const Z_FIXED: 4;

    export const Z_DEFAULT_STRATEGY: 0;

    export const ZLIB_VERNUM: 4800;

    export const DEFLATE: 1;

    export const INFLATE: 2;

    export const GZIP: 3;

    export const GUNZIP: 4;

    export const DEFLATERAW: 5;

    export const INFLATERAW: 6;

    export const UNZIP: 7;

    export const BROTLI_DECODE: 8;

    export const BROTLI_ENCODE: 9;

    export const Z_MIN_WINDOWBITS: 8;

    export const Z_MAX_WINDOWBITS: 15;

    export const Z_DEFAULT_WINDOWBITS: 15;

    export const Z_MIN_CHUNK: 64;

    export const Z_MAX_CHUNK: -1;

    export const Z_DEFAULT_CHUNK: 16384;

    export const Z_MIN_MEMLEVEL: 1;

    export const Z_MAX_MEMLEVEL: 9;

    export const Z_DEFAULT_MEMLEVEL: 8;

    export const Z_MIN_LEVEL: -1;

    export const Z_MAX_LEVEL: 9;

    export const Z_DEFAULT_LEVEL: -1;

}

