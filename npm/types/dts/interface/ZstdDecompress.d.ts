/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description ZstdDecompress is the codec that decompresses zstd frames
 *
 *  `new zlib.ZstdDecompress(opts)` builds a synchronous codec; feed chunks with
 *  `_processChunk` and the zlib flush flags. It reads the standard zstd frame format, the
 *  same bytes `zlib.zstdCompress` writes and the zstd command line tool produces. For the
 *  fibjs stream API use `zlib.createZstdDecompress(to)` instead. It inherits the members of
 *  ZlibCodec, so `_processChunk`, `close` and `reset` behave exactly as they do for the
 *  inflate codecs; `params` is rejected because a decompressor has no compression
 *  parameters.
 *
 *  Concepts:
 *  - **Options**: `windowBits` maps to `ZSTD_d_windowLogMax`, the largest window the
 *    decompressor allocates for, which bounds the memory a hostile frame can make it
 *    reserve. `params` is an object whose keys are zstd parameter constants and whose values
 *    are passed to `ZSTD_DCtx_setParameter`. Other keys are ignored.
 *  - **Concatenated frames**: a zstd stream may carry several frames back to back; the
 *    decompressor continues with the next one after a frame ends, so one call chain decodes
 *    all of them.
 *  - **Errors**: corrupt data, an unknown frame header or a frame that needs more memory
 *    than `windowBits` allows throws an Error whose `code` is the zstd error name (for
 *    example `ZSTD_error_prefix_unknown`), read from `ZSTD_getErrorName`.
 *
 *  Example 1 — decode a frame and its concatenated successor in one chain:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const packed = Buffer.concat([
 *      zlib.zstdCompress('first ', { pledgedSrcSize: 6 }),
 *      zlib.zstdCompress('second', { pledgedSrcSize: 6 }),
 *  ]);
 *
 *  const unzstd = new zlib.ZstdDecompress();
 *  console.log(unzstd._processChunk(packed, C.Z_FINISH).toString()); // first second
 *  ```
 *
 *  Example 2 — a corrupt frame reports the zstd error name in err.code:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const unzstd = new zlib.ZstdDecompress();
 *  try {
 *      unzstd._processChunk(Buffer.from('not a zstd frame'), C.Z_FINISH);
 *  } catch (e) {
 *      console.log(e.code); // ZSTD_error_prefix_unknown
 *  }
 *  ```
 *
 */
declare class Class_ZstdDecompress extends Class_ZlibCodec {
    /**
     * @description Creates a zstd decompressor
     *
     *      `opts` may be omitted or empty. The recognized options are `windowBits`
     *      (ZSTD_d_windowLogMax, the largest window the decompressor reserves memory for) and
     *      `params` (an object of zstd parameter constants to values). Other keys are ignored.
     *
     *      @param opts decompression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

