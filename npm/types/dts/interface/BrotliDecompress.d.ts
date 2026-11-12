/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description BrotliDecompress is the codec that decompresses Brotli streams
 *
 *  `new zlib.BrotliDecompress(opts)` builds a synchronous codec; feed chunks with
 *  `_processChunk` and the zlib flush flags. It reads the standard Brotli stream format, the
 *  same bytes `zlib.brotliCompress` writes and the brotli command line tool produces. For the
 *  fibjs stream API use `zlib.createBrotliDecompress(to)` instead. It inherits the members of
 *  ZlibCodec, so `_processChunk`, `close` and `reset` behave the way they do for the inflate
 *  codecs.
 *
 *  Concepts:
 *  - **Options**: `params` is an object whose keys are Brotli decoder parameter constants
 *    (`BROTLI_DECODER_PARAM_DISABLE_RING_BUFFER_REALLOCATION`,
 *    `BROTLI_DECODER_PARAM_LARGE_WINDOW`) and whose values are applied through
 *    `BrotliDecoderSetParameter`. Other keys are ignored.
 *  - **Errors**: corrupt input throws an Error whose `code` is the Brotli decoder error name
 *    (for example `BROTLI_DECODER_ERROR_FORMAT_RESERVED` or
 *    `BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET`), read from `BrotliDecoderGetErrorCode`.
 *  - **Streaming**: a Brotli stream may be split across several `_processChunk` calls; the
 *    decoder keeps the state between them and finishes when the input ends.
 *
 *  Example 1 — decode a stream built by the codec:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const packed = new zlib.BrotliCompress()._processChunk('hello, brotli', C.Z_FINISH);
 *  const unbr = new zlib.BrotliDecompress();
 *  console.log(unbr._processChunk(packed, C.Z_FINISH).toString()); // hello, brotli
 *  ```
 *
 *  Example 2 — a corrupt stream reports the decoder error name in err.code:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const unbr = new zlib.BrotliDecompress();
 *  try {
 *      unbr._processChunk(Buffer.from([0xff, 0xff, 0xff, 0xff]), C.Z_FINISH);
 *  } catch (e) {
 *      console.log(e.code); // BROTLI_DECODER_ERROR_FORMAT_*
 *  }
 *  ```
 *
 */
declare class Class_BrotliDecompress extends Class_ZlibCodec {
    /**
     * @description Creates a Brotli decompressor
     *
     *      `opts` may be omitted or empty. The recognized option is `params`, an object of
     *      `BROTLI_DECODER_PARAM_*` constants to values; other keys are ignored.
     *
     *      @param opts decompression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

