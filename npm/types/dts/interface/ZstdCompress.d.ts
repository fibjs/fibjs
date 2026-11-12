/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description ZstdCompress is the codec that compresses data to the zstd frame format
 *
 *  `new zlib.ZstdCompress(opts)` builds a synchronous codec; feed chunks with `_processChunk`
 *  and the zlib flush flags. The output is a standard zstd frame (magic 28 b5 2f fd), the
 *  same bytes `zlib.zstdCompress` writes and `zlib.zstdDecompress` reads. For the fibjs
 *  stream API use `zlib.createZstdCompress(to)` instead; the codec class exists for packages
 *  that drive a codec directly. It inherits the members of ZlibCodec, so `_processChunk`,
 *  `close`, `reset` and `params` behave the way they do for the deflate codecs.
 *
 *  Concepts:
 *  - **Options**: `level` is the zstd compression level (default 3, `ZSTD_CLEVEL_DEFAULT`;
 *    negative values select zstd's fast levels), `windowBits` maps to `ZSTD_c_windowLog`,
 *    `strategy` to `ZSTD_c_strategy` and `nbWorkers` to `ZSTD_c_nbWorkers` (0 keeps the
 *    single-threaded path). `pledgedSrcSize` declares the total input length so the frame
 *    header can carry it; omitting it writes a frame without a content size, which is what a
 *    streaming producer has to do. `params` is an object whose keys are zstd parameter
 *    constants and whose values are passed to `ZSTD_CCtx_setParameter`, so any parameter the
 *    bundled library accepts can be set (for example `ZSTD_c_checksumFlag`). Other keys are
 *    ignored.
 *  - **Flush flags**: the codec takes the zlib flush flags and maps them to zstd end
 *    directives, so generic compression code does not need a second set of flags:
 *    `Z_NO_FLUSH` is `ZSTD_e_continue`, `Z_SYNC_FLUSH` (and the other flush modes) is
 *    `ZSTD_e_flush`, `Z_FINISH` is `ZSTD_e_end`.
 *  - **Streaming**: each `_processChunk` call appends to the current frame; `Z_FINISH` ends
 *    it and resets the codec for the next message. A call may return an empty Buffer when
 *    the compressor absorbed the input without producing output.
 *
 *  Example 1 — compress and decompress through concrete codecs:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const zstd = new zlib.ZstdCompress({ level: 9 });
 *  const packed = zstd._processChunk('hello, world', C.Z_FINISH);
 *  console.log(packed[0].toString(16), packed[1].toString(16)); // 28 b5
 *
 *  const unzstd = new zlib.ZstdDecompress();
 *  console.log(unzstd._processChunk(packed, C.Z_FINISH).toString()); // hello, world
 *  ```
 *
 *  Example 2 — the codec and the module-level helper produce interchangeable frames:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const packed = new zlib.ZstdCompress()._processChunk('hello, world', C.Z_FINISH);
 *  console.log(zlib.zstdDecompress(packed).toString()); // hello, world
 *  ```
 *
 */
declare class Class_ZstdCompress extends Class_ZlibCodec {
    /**
     * @description Creates a zstd compressor
     *
     *      `opts` may be omitted or empty. The recognized options are `level` (default 3),
     *      `windowBits` (ZSTD_c_windowLog), `strategy` (ZSTD_c_strategy), `nbWorkers`
     *      (ZSTD_c_nbWorkers, 0 keeps the single-threaded path), `pledgedSrcSize` (declares
     *      the total input length for the frame header) and `params` (an object of zstd
     *      parameter constants to values). Other keys are ignored.
     *
     *      @param opts compression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

