/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Gunzip is the Node.js compatible codec that decompresses gzip data
 *
 *  `new zlib.Gunzip(opts)` builds a synchronous codec; feed gzip chunks with `_processChunk`
 *  and the zlib flush flags and it returns the decoded bytes. It decodes the container
 *  written by `zlib.gzip` and `zlib.createGzip`. For the fibjs stream API use
 *  `zlib.createGunzip(to)`; the codec class exists for packages such as minizlib and tar.
 *  It inherits the members of ZlibCodec.
 *
 *  Concepts:
 *  - **Options**: only `windowBits` (default 31, the gzip container) affects decoding;
 *    `level`, `memLevel` and `strategy` are accepted for constructor compatibility and
 *    ignored, because the level is recorded in the compressed header. An invalid windowBits
 *    leaves the codec closed.
 *  - **Behavior**: the codec decodes one gzip member per message: when the member ends the
 *    codec resets and ignores the remaining bytes, so feed concatenated members member by
 *    member. The module-level helpers (`zlib.gunzip`, `gunzipTo` and `createGunzip`) decode
 *    all members at once. Input that ends early is decoded up to its last complete output
 *    without an error, and a zero-byte result is an empty Buffer (the module-level helpers
 *    return null instead). Data that is not gzip fails with `Z_DATA_ERROR`.
 *
 *  Example 1 — decode the output of zlib.gzip:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const packed = zlib.gzip('hello, world');
 *  const gunzip = new zlib.Gunzip();
 *  console.log(gunzip._processChunk(packed, C.Z_FINISH).toString()); // hello, world
 *  ```
 *
 *  Example 2 — decode a member written in several pieces:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const gzip = new zlib.Gzip();
 *  const part1 = gzip._processChunk('hello, ', C.Z_NO_FLUSH);
 *  const part2 = gzip._processChunk('world', C.Z_FINISH);
 *
 *  const gunzip = new zlib.Gunzip();
 *  console.log(gunzip._processChunk(Buffer.concat([part1, part2]), C.Z_FINISH).toString());
 *  // hello, world
 *  ```
 *
 */
declare class Class_Gunzip extends Class_ZlibCodec {
    /**
     * @description Creates a gunzip codec
     *
     *      `opts` may be omitted or empty. Only `windowBits` is used (default 31 for the gzip
     *      container); `level`, `memLevel` and `strategy` are accepted for compatibility with
     *      the other codecs and ignored. An invalid windowBits leaves the codec closed and the
     *      first `_processChunk` throws ERR_ZLIB_BINDING_CLOSED.
     *
     *      @param opts decompression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

