/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Inflate is the Node.js compatible codec that decompresses zlib-format data
 *
 *  `new zlib.Inflate(opts)` builds a synchronous codec; feed zlib-format chunks with
 *  `_processChunk` and the zlib flush flags and it returns the decoded bytes. The zlib
 *  format starts with 0x78 and is what `zlib.deflate` and `zlib.createDeflate` write; gzip
 *  and raw deflate input is rejected with `Z_DATA_ERROR`. For the fibjs stream API use
 *  `zlib.createInflate(to)`; the codec class exists for packages such as minizlib and tar.
 *  It inherits the members of ZlibCodec.
 *
 *  Concepts:
 *  - **Options**: only `windowBits` (default 15 for the zlib format; 31 decodes the gzip
 *    container and -15 raw deflate) affects decoding; `level`, `memLevel` and `strategy`
 *    are accepted for compatibility and ignored.
 *  - **Behavior**: input that ends early is decoded up to its last complete output without
 *    an error, and a zero-byte result is an empty Buffer. The codec resets itself when the
 *    stream ends, so a new message can follow immediately.
 *
 *  Example 1 — decode the output of zlib.deflate:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const packed = zlib.deflate('hello, world');
 *  const inflate = new zlib.Inflate();
 *  console.log(inflate._processChunk(packed, C.Z_FINISH).toString()); // hello, world
 *  ```
 *
 *  Example 2 — wrapped formats are rejected:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  try {
 *      new zlib.Inflate()._processChunk(zlib.gzip('hello'), C.Z_FINISH);
 *  } catch (e) {
 *      console.log(e.code); // Z_DATA_ERROR
 *  }
 *  ```
 *
 */
declare class Class_Inflate extends Class_ZlibCodec {
    /**
     * @description Creates an inflate codec
     *
     *      `opts` may be omitted or empty. Only `windowBits` is used (default 15 for the zlib
     *      format; 31 decodes the gzip container and -15 raw deflate); `level`, `memLevel` and
     *      `strategy` are accepted for compatibility with the other codecs and ignored. An
     *      invalid windowBits leaves the codec closed and the first `_processChunk` throws
     *      ERR_ZLIB_BINDING_CLOSED.
     *
     *      @param opts decompression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

