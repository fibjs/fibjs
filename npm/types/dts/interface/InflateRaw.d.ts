/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description InflateRaw is the Node.js compatible codec that decompresses raw deflate data
 *
 *  `new zlib.InflateRaw(opts)` builds a synchronous codec; feed raw deflate chunks (no
 *  header, no checksum) with `_processChunk` and it returns the decoded bytes. It is the
 *  decoder for `zlib.deflateRaw`, `zlib.createDeflateRaw` and protocol payloads such as
 *  WebSocket permessage-deflate and the deflate entries inside zip archives; zlib-format
 *  input is rejected with `Z_DATA_ERROR`. For the fibjs stream API use
 *  `zlib.createInflateRaw(to)`; the codec class exists for packages such as minizlib and
 *  tar. It inherits the members of ZlibCodec.
 *
 *  Concepts:
 *  - **Options**: only `windowBits` (default -15 for raw deflate; 15 decodes the zlib
 *    format and 31 the gzip container) affects decoding; `level`, `memLevel` and `strategy`
 *    are accepted for compatibility and ignored.
 *  - **Behavior**: input that ends early is decoded up to its last complete output without
 *    an error, and a zero-byte result is an empty Buffer. Chunks can be fed incrementally
 *    with Z_NO_FLUSH; Z_FINISH ends the message and resets the codec.
 *
 *  Example 1 — decode the output of zlib.deflateRaw:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const raw = zlib.deflateRaw('hello, world');
 *  const inflate = new zlib.InflateRaw();
 *  console.log(inflate._processChunk(raw, C.Z_FINISH).toString()); // hello, world
 *  ```
 *
 *  Example 2 — feed the stream in two chunks:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const raw = zlib.deflateRaw('hello, world');
 *  const inflate = new zlib.InflateRaw();
 *  const first = inflate._processChunk(raw.slice(0, 4), C.Z_NO_FLUSH);
 *  const last = inflate._processChunk(raw.slice(4), C.Z_FINISH);
 *
 *  console.log(Buffer.concat([first, last]).toString()); // hello, world
 *  ```
 *
 */
declare class Class_InflateRaw extends Class_ZlibCodec {
    /**
     * @description Creates a raw-deflate decompression codec
     *
     *      `opts` may be omitted or empty. Only `windowBits` is used (default -15 for raw
     *      deflate; 15 decodes the zlib format and 31 the gzip container); `level`, `memLevel`
     *      and `strategy` are accepted for compatibility with the other codecs and ignored. An
     *      invalid windowBits leaves the codec closed and the first `_processChunk` throws
     *      ERR_ZLIB_BINDING_CLOSED.
     *
     *      @param opts decompression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

