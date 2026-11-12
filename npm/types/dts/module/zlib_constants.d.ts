/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The zlib_constants module enumerates the constants of the zlib library bundled
 *  with fibjs: flush codes, status and error codes, compression levels, strategies, window
 *  and memory bounds, codec type ids, and the library version
 *
 *  The module is not requireable on its own; load it through the `constants` property of the
 *  zlib module. The values describe the zlib C library version 1.3.1 linked into this build
 *  and are accepted by the `flush`, `level`, `windowBits`, `memLevel` and `strategy`
 * parameters of the compression classes; the codec type ids (`DEFLATE`, `GZIP`, ...) are
 * also exported.
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
 *    Z_MAX_MEMLEVEL bound the codec options; Z_MIN_CHUNK, Z_MAX_CHUNK (-1 means unlimited) and Z_DEFAULT_CHUNK describe chunk sizes.
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
     * @description Maximum chunk size; -1 means unlimited
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

    /**
     * @description zstd compression codec type id
     */
    export const ZSTD_COMPRESS: 10;

    /**
     * @description zstd decompression codec type id
     */
    export const ZSTD_DECOMPRESS: 11;

    /**
     * @description zstd end directive: keep buffering input
     */
    export const ZSTD_e_continue: 0;

    /**
     * @description zstd end directive: emit everything produced so far
     */
    export const ZSTD_e_flush: 1;

    /**
     * @description zstd end directive: finish the frame and write its trailer
     */
    export const ZSTD_e_end: 2;

    /**
     * @description zstd compression strategy: fastest, no matching
     */
    export const ZSTD_fast: 1;

    /**
     * @description zstd compression strategy: double fast
     */
    export const ZSTD_dfast: 2;

    /**
     * @description zstd compression strategy: greedy
     */
    export const ZSTD_greedy: 3;

    /**
     * @description zstd compression strategy: lazy
     */
    export const ZSTD_lazy: 4;

    /**
     * @description zstd compression strategy: lazy2
     */
    export const ZSTD_lazy2: 5;

    /**
     * @description zstd compression strategy: binary tree with lazy2
     */
    export const ZSTD_btlazy2: 6;

    /**
     * @description zstd compression strategy: binary tree with optimal parsing
     */
    export const ZSTD_btopt: 7;

    /**
     * @description zstd compression strategy: binary tree, ultra
     */
    export const ZSTD_btultra: 8;

    /**
     * @description zstd compression strategy: binary tree, ultra 2
     */
    export const ZSTD_btultra2: 9;

    /**
     * @description zstd parameter: compression level
     */
    export const ZSTD_c_compressionLevel: 100;

    /**
     * @description zstd parameter: maximum back-reference distance, as a power of 2
     */
    export const ZSTD_c_windowLog: 101;

    /**
     * @description zstd parameter: size of the initial probe table, as a power of 2
     */
    export const ZSTD_c_hashLog: 102;

    /**
     * @description zstd parameter: size of the multi-probe search table, as a power of 2
     */
    export const ZSTD_c_chainLog: 103;

    /**
     * @description zstd parameter: number of search attempts, as a power of 2
     */
    export const ZSTD_c_searchLog: 104;

    /**
     * @description zstd parameter: minimum size of searched matches
     */
    export const ZSTD_c_minMatch: 105;

    /**
     * @description zstd parameter: impact of this field depends on the strategy
     */
    export const ZSTD_c_targetLength: 106;

    /**
     * @description zstd parameter: compression strategy, see the ZSTD_* strategy constants
     */
    export const ZSTD_c_strategy: 107;

    /**
     * @description zstd parameter: enable long distance matching
     */
    export const ZSTD_c_enableLongDistanceMatching: 160;

    /**
     * @description zstd parameter: long distance matcher table size, as a power of 2
     */
    export const ZSTD_c_ldmHashLog: 161;

    /**
     * @description zstd parameter: minimum match size of the long distance matcher
     */
    export const ZSTD_c_ldmMinMatch: 162;

    /**
     * @description zstd parameter: log size of each long distance matcher bucket
     */
    export const ZSTD_c_ldmBucketSizeLog: 163;

    /**
     * @description zstd parameter: long distance matcher hash rate, as a power of 2
     */
    export const ZSTD_c_ldmHashRateLog: 164;

    /**
     * @description zstd parameter: write the content size into the frame header when known
     */
    export const ZSTD_c_contentSizeFlag: 200;

    /**
     * @description zstd parameter: write a 32-bit content checksum at the end of the frame
     */
    export const ZSTD_c_checksumFlag: 201;

    /**
     * @description zstd parameter: write the dictionary id into the frame header
     */
    export const ZSTD_c_dictIDFlag: 202;

    /**
     * @description zstd parameter: number of worker threads for multi-threaded compression
     */
    export const ZSTD_c_nbWorkers: 400;

    /**
     * @description zstd parameter: size of one compression job, enforced when nbWorkers is set
     */
    export const ZSTD_c_jobSize: 401;

    /**
     * @description zstd parameter: overlap size between jobs, as a fraction of the window
     */
    export const ZSTD_c_overlapLog: 402;

    /**
     * @description zstd parameter: largest window the decompressor reserves memory for
     */
    export const ZSTD_d_windowLogMax: 100;

    /**
     * @description zstd default compression level
     */
    export const ZSTD_CLEVEL_DEFAULT: 3;

    /**
     * @description zstd error: no error
     */
    export const ZSTD_error_no_error: 0;

    /**
     * @description zstd error: an unspecified error occurred
     */
    export const ZSTD_error_GENERIC: 1;

    /**
     * @description zstd error: the frame header is not recognized
     */
    export const ZSTD_error_prefix_unknown: 10;

    /**
     * @description zstd error: the frame requires an unsupported library version
     */
    export const ZSTD_error_version_unsupported: 12;

    /**
     * @description zstd error: a frame header parameter is not supported
     */
    export const ZSTD_error_frameParameter_unsupported: 14;

    /**
     * @description zstd error: the frame window is larger than the decoder allows
     */
    export const ZSTD_error_frameParameter_windowTooLarge: 16;

    /**
     * @description zstd error: the input is corrupted
     */
    export const ZSTD_error_corruption_detected: 20;

    /**
     * @description zstd error: the content checksum does not match
     */
    export const ZSTD_error_checksum_wrong: 22;

    /**
     * @description zstd error: the literals section header is malformed
     */
    export const ZSTD_error_literals_headerWrong: 24;

    /**
     * @description zstd error: the dictionary is corrupted
     */
    export const ZSTD_error_dictionary_corrupted: 30;

    /**
     * @description zstd error: the dictionary does not match the frame
     */
    export const ZSTD_error_dictionary_wrong: 32;

    /**
     * @description zstd error: the dictionary could not be created
     */
    export const ZSTD_error_dictionaryCreation_failed: 34;

    /**
     * @description zstd error: a parameter is not supported
     */
    export const ZSTD_error_parameter_unsupported: 40;

    /**
     * @description zstd error: this combination of parameters is not supported
     */
    export const ZSTD_error_parameter_combination_unsupported: 41;

    /**
     * @description zstd error: a parameter value is out of range
     */
    export const ZSTD_error_parameter_outOfBound: 42;

    /**
     * @description zstd error: a table log is too large
     */
    export const ZSTD_error_tableLog_tooLarge: 44;

    /**
     * @description zstd error: a maximum symbol value is too large
     */
    export const ZSTD_error_maxSymbolValue_tooLarge: 46;

    /**
     * @description zstd error: a maximum symbol value is too small
     */
    export const ZSTD_error_maxSymbolValue_tooSmall: 48;

    /**
     * @description zstd error: a stability condition is not respected
     */
    export const ZSTD_error_stabilityCondition_notRespected: 50;

    /**
     * @description zstd error: the call is made at the wrong stage
     */
    export const ZSTD_error_stage_wrong: 60;

    /**
     * @description zstd error: the context is not initialized
     */
    export const ZSTD_error_init_missing: 62;

    /**
     * @description zstd error: memory allocation failed
     */
    export const ZSTD_error_memory_allocation: 64;

    /**
     * @description zstd error: the workspace is too small
     */
    export const ZSTD_error_workSpace_tooSmall: 66;

    /**
     * @description zstd error: the destination buffer is too small
     */
    export const ZSTD_error_dstSize_tooSmall: 70;

    /**
     * @description zstd error: the source size does not match the pledged size
     */
    export const ZSTD_error_srcSize_wrong: 72;

    /**
     * @description zstd error: the destination buffer is null
     */
    export const ZSTD_error_dstBuffer_null: 74;

    /**
     * @description zstd error: the destination filled up and no progress was made
     */
    export const ZSTD_error_noForwardProgress_destFull: 80;

    /**
     * @description zstd error: the input is empty and no progress was made
     */
    export const ZSTD_error_noForwardProgress_inputEmpty: 82;

}

