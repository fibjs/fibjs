/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description Module that defines the commonly used constants of the zlib module
 *
 *  How to require it:
 *  ```JavaScript
 *  var constants = require('zlib').constants
 *  ```
 *
 */
declare module 'zlib_constants' {
    /**
     * @description Performs no flush operation
     */
    export const Z_NO_FLUSH: 0;

    /**
     * @description Performs a partial flush operation
     */
    export const Z_PARTIAL_FLUSH: 1;

    /**
     * @description Synchronous flush, waits for all pending output to be flushed
     */
    export const Z_SYNC_FLUSH: 2;

    /**
     * @description Full flush, waits for all output to be flushed and resets the internal state
     */
    export const Z_FULL_FLUSH: 3;

    /**
     * @description Finishes the compression or decompression operation
     */
    export const Z_FINISH: 4;

    /**
     * @description Stops compression at the end of the current block
     */
    export const Z_BLOCK: 5;

    /**
     * @description The operation completed successfully
     */
    export const Z_OK: 0;

    /**
     * @description End of the compression or decompression stream
     */
    export const Z_STREAM_END: 1;

    /**
     * @description A dictionary is required to continue the operation
     */
    export const Z_NEED_DICT: 2;

    /**
     * @description A system error occurred
     */
    export const Z_ERRNO: -1;

    /**
     * @description Inconsistent stream state or invalid parameter
     */
    export const Z_STREAM_ERROR: -2;

    /**
     * @description The input data is corrupted
     */
    export const Z_DATA_ERROR: -3;

    /**
     * @description Memory allocation failed
     */
    export const Z_MEM_ERROR: -4;

    /**
     * @description Buffer error
     */
    export const Z_BUF_ERROR: -5;

    /**
     * @description Version mismatch
     */
    export const Z_VERSION_ERROR: -6;

    /**
     * @description No compression
     */
    export const Z_NO_COMPRESSION: 0;

    /**
     * @description Fastest compression speed
     */
    export const Z_BEST_SPEED: 1;

    /**
     * @description Best compression ratio
     */
    export const Z_BEST_COMPRESSION: 9;

    /**
     * @description Default compression level
     */
    export const Z_DEFAULT_COMPRESSION: -1;

    /**
     * @description Filtered compression strategy
     */
    export const Z_FILTERED: 1;

    /**
     * @description Huffman coding only
     */
    export const Z_HUFFMAN_ONLY: 2;

    /**
     * @description Run-length encoding
     */
    export const Z_RLE: 3;

    /**
     * @description Fixed Huffman coding
     */
    export const Z_FIXED: 4;

    /**
     * @description Default compression strategy
     */
    export const Z_DEFAULT_STRATEGY: 0;

    /**
     * @description zlib version number
     */
    export const ZLIB_VERNUM: 4800;

    /**
     * @description deflate compression
     */
    export const DEFLATE: 1;

    /**
     * @description inflate decompression
     */
    export const INFLATE: 2;

    /**
     * @description gzip compression
     */
    export const GZIP: 3;

    /**
     * @description gunzip decompression
     */
    export const GUNZIP: 4;

    /**
     * @description deflateRaw compression
     */
    export const DEFLATERAW: 5;

    /**
     * @description inflateRaw decompression
     */
    export const INFLATERAW: 6;

    /**
     * @description unzip decompression
     */
    export const UNZIP: 7;

    /**
     * @description Brotli decoding
     */
    export const BROTLI_DECODE: 8;

    /**
     * @description Brotli encoding
     */
    export const BROTLI_ENCODE: 9;

    /**
     * @description Minimum window size
     */
    export const Z_MIN_WINDOWBITS: 8;

    /**
     * @description Maximum window size
     */
    export const Z_MAX_WINDOWBITS: 15;

    /**
     * @description Default window size
     */
    export const Z_DEFAULT_WINDOWBITS: 15;

    /**
     * @description Minimum chunk size
     */
    export const Z_MIN_CHUNK: 64;

    /**
     * @description Maximum chunk size
     */
    export const Z_MAX_CHUNK: -1;

    /**
     * @description Default chunk size
     */
    export const Z_DEFAULT_CHUNK: 16384;

    /**
     * @description Minimum memory level
     */
    export const Z_MIN_MEMLEVEL: 1;

    /**
     * @description Maximum memory level
     */
    export const Z_MAX_MEMLEVEL: 9;

    /**
     * @description Default memory level
     */
    export const Z_DEFAULT_MEMLEVEL: 8;

    /**
     * @description Minimum compression level
     */
    export const Z_MIN_LEVEL: -1;

    /**
     * @description Maximum compression level
     */
    export const Z_MAX_LEVEL: 9;

    /**
     * @description Default compression level
     */
    export const Z_DEFAULT_LEVEL: -1;

}

