/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The zlib_constants module enumerates the constants of the zlib library bundled
 *  with fibjs: flush codes, status and error codes, compression levels, strategies, window
 *  and memory bounds, codec type ids, and the library version
 *
 *  The module is not requireable on its own; load it through the `constants` property of the
 *  zlib module. The values describe the zlib C library version 1.3.1 linked into this build
 *  and are accepted by the `flush`, `level`, `windowBits`, `memLevel` and `strategy`
 *  parameters of the compression classes; the codec type ids (`DEFLATE`, `GZIP`, ...) mirror
 *  Node.js's zlib.constants.
 *
 *  Concepts:
 *
 *  - **Flush modes**: Z_NO_FLUSH lets the compressor buffer and decide when to emit;
 *    Z_PARTIAL_FLUSH emits the pending output at a partial-block boundary; Z_SYNC_FLUSH emits
 *    all pending output aligned to a byte boundary and keeps the compression dictionary, so
 *    the peer can decode the piece immediately; Z_FULL_FLUSH additionally resets the
 *    dictionary, so decompression can restart from that point; Z_BLOCK stops at the next
 *    deflate block boundary; Z_FINISH ends the stream and writes the trailer.
 *  - **Status and error codes**: Z_OK (0) and Z_STREAM_END (1) are success states; Z_NEED_DICT
 *    asks for a preset dictionary; the negative values (Z_ERRNO, Z_STREAM_ERROR, Z_DATA_ERROR,
 *    Z_MEM_ERROR, Z_BUF_ERROR, Z_VERSION_ERROR) are zlib return codes. A failed call throws an
 *    Error whose `code` is the status name, for example 'Z_DATA_ERROR'; the numeric constant
 *    is the zlib return value, not the error number.
 *  - **Levels and strategies**: Z_NO_COMPRESSION (0) to Z_BEST_COMPRESSION (9) trade speed for
 *    size and Z_DEFAULT_COMPRESSION (-1) lets zlib choose (level 6 today). Z_DEFAULT_STRATEGY
 *    suits ordinary data, Z_FILTERED favors data produced by a filter, Z_HUFFMAN_ONLY and
 *    Z_RLE restrict LZ matching, and Z_FIXED disables dynamic Huffman codes.
 *  - **Bounds**: Z_MIN_WINDOWBITS through Z_MAX_WINDOWBITS and Z_MIN_MEMLEVEL through
 *    Z_MAX_MEMLEVEL bound the codec options; Z_MIN_CHUNK, Z_MAX_CHUNK (-1 means unlimited in
 *    fibjs, Node.js reports Infinity) and Z_DEFAULT_CHUNK describe chunk sizes.
 *  - **Where options apply**: the codec classes (`new zlib.Deflate(...)`, `new zlib.Gzip(...)`
 *    and the rest) read level, windowBits, memLevel and strategy; the one-shot functions only
 *    read level, so pass the other options to a codec instance.
 *
 *  Import:
 *  ```JavaScript
 *  const constants = require('zlib').constants;
 *  ```
 *
 *  Example 1 — flush control on a streaming codec:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  // _processChunk(data, flushFlag) drives one codec step at a time.
 *  const deflate = new zlib.Deflate();
 *  const head = deflate._processChunk('hello ', C.Z_NO_FLUSH);    // header only
 *  const middle = deflate._processChunk('world', C.Z_SYNC_FLUSH); // decodable piece
 *  const tail = deflate._processChunk('', C.Z_FINISH);            // trailer
 *
 *  console.log(head.length, middle.length, tail.length); // 2 17 6
 *  const packed = Buffer.concat([head, middle, tail]);
 *  console.log(zlib.inflate(packed).toString()); // hello world
 *  ```
 *
 *  Example 2 — compression levels:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *  const text = 'abc'.repeat(1000);
 *
 *  // Z_NO_COMPRESSION stores, Z_BEST_COMPRESSION spends CPU to shrink.
 *  const stored = zlib.deflate(text, { level: C.Z_NO_COMPRESSION });
 *  const packed = zlib.deflate(text, { level: C.Z_BEST_COMPRESSION });
 *  console.log(stored.length > packed.length); // true
 *
 *  // Without a level the one-shot uses Z_DEFAULT_COMPRESSION (-1).
 *  console.log(C.Z_DEFAULT_COMPRESSION, C.Z_MIN_LEVEL, C.Z_MAX_LEVEL); // -1 -1 9
 *  console.log(zlib.deflate(text).length ===
 *      zlib.deflate(text, { level: C.Z_DEFAULT_COMPRESSION }).length); // true
 *  ```
 *
 *  Example 3 — strategies and error codes:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *  const text = 'The quick brown fox jumps over the lazy dog. '.repeat(50);
 *
 *  // Strategies are codec-constructor options: HUFFMAN_ONLY gives up LZ matching.
 *  const plain = new zlib.Deflate({ strategy: C.Z_DEFAULT_STRATEGY });
 *  const huff = new zlib.Deflate({ strategy: C.Z_HUFFMAN_ONLY });
 *  const a = plain._processChunk(text, C.Z_FINISH);
 *  const b = huff._processChunk(text, C.Z_FINISH);
 *  console.log(a.length < b.length); // true
 *
 *  // A failed inflate reports the zlib status name in err.code.
 *  try {
 *      zlib.inflate(Buffer.from('not a zlib stream'));
 *  } catch (e) {
 *      console.log(e.code); // Z_DATA_ERROR
 *  }
 *  console.log(C.Z_OK, C.Z_STREAM_END, C.Z_DATA_ERROR, C.Z_VERSION_ERROR); // 0 1 -3 -6
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
     * @description zlib version number (4880 is zlib 1.3.1)
     */
    export const ZLIB_VERNUM: 4880;

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
     * @description Maximum chunk size; -1 means unlimited (Node reports Infinity)
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

