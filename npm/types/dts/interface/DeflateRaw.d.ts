/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description DeflateRaw is the Node.js compatible codec that compresses data to raw deflate
 *
 *  `new zlib.DeflateRaw(opts)` builds a synchronous codec; `_processChunk` returns raw
 *  deflate data with no header and no checksum. Raw deflate is the payload of protocols
 *  such as WebSocket permessage-deflate and of the deflate entries inside zip archives;
 *  only `InflateRaw` and `zlib.inflateRaw` decode it, while `inflate` and `unzip` reject
 *  it. For the fibjs stream API use `zlib.createDeflateRaw(to)`; the codec class exists for
 *  packages such as minizlib and tar. It inherits the members of ZlibCodec.
 *
 *  Concepts:
 *  - **Options**: `level` (default Z_DEFAULT_COMPRESSION, not clamped), `windowBits`
 *    (default -15 for raw deflate; 15 selects the zlib format and 31 the gzip container),
 *    `memLevel` (1 to 9, default 8) and `strategy` (default Z_DEFAULT_STRATEGY).
 *  - **Streaming**: Z_SYNC_FLUSH produces a piece the peer can decode without ending the
 *    message, which is exactly what permessage-deflate sends; Z_FINISH ends the stream and
 *    resets the codec.
 *
 *  Example 1 — round trip through DeflateRaw and InflateRaw:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const raw = new zlib.DeflateRaw()._processChunk('hello, world', C.Z_FINISH);
 *  console.log(raw.equals(zlib.deflateRaw('hello, world'))); // true
 *
 *  const inflate = new zlib.InflateRaw();
 *  console.log(inflate._processChunk(raw, C.Z_FINISH).toString()); // hello, world
 *  ```
 *
 *  Example 2 — permessage-deflate style: emit with Z_SYNC_FLUSH, then finish:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const C = zlib.constants;
 *
 *  const deflate = new zlib.DeflateRaw();
 *  const piece = deflate._processChunk('hello', C.Z_SYNC_FLUSH);
 *  const rest = deflate._processChunk(' world', C.Z_FINISH);
 *
 *  const inflate = new zlib.InflateRaw();
 *  console.log(inflate._processChunk(Buffer.concat([piece, rest]), C.Z_FINISH).toString());
 *  // hello world
 *  ```
 *
 */
declare class Class_DeflateRaw extends Class_ZlibCodec {
    /**
     * @description Creates a raw-deflate codec
     *
     *      `opts` may be omitted or empty. The recognized options are `level` (default
     *      Z_DEFAULT_COMPRESSION, not clamped), `windowBits` (default -15 for raw deflate; 15
     *      writes the zlib format and 31 the gzip container), `memLevel` (1 to 9, default 8)
     *      and `strategy` (default Z_DEFAULT_STRATEGY 0). Creation never throws for a bad
     *      option: the codec is left closed and the first `_processChunk` throws
     *      ERR_ZLIB_BINDING_CLOSED.
     *
     *      @param opts compression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

