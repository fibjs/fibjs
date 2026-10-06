/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/zlib_constants.d.ts" />
/// <reference path="../interface/Gzip.d.ts" />
/// <reference path="../interface/Gunzip.d.ts" />
/// <reference path="../interface/Deflate.d.ts" />
/// <reference path="../interface/Inflate.d.ts" />
/// <reference path="../interface/DeflateRaw.d.ts" />
/// <reference path="../interface/InflateRaw.d.ts" />
/// <reference path="../interface/Unzip.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The zlib module is the built-in compression module of fibjs: it compresses and
 *  decompresses Buffers and streams with the deflate, gzip and raw deflate formats
 *
 *  Capabilities:
 *
 *  - **One-shot buffers**: `deflate`/`inflate` (zlib format), `gzip`/`gunzip` (gzip format),
 *    `deflateRaw`/`inflateRaw` (raw deflate) and `zip`/`unzip` (format detection) transform a
 *    Buffer or a string in a single call;
 *  - **Stream to stream**: `deflateTo`, `deflateRawTo`, `gzipTo`, `zipTo` and the matching
 *    `inflateTo`, `inflateRawTo`, `gunzipTo`, `unzipTo` copy a Buffer or a source Stream
 *    through the codec into a destination stream;
 *  - **Compression streams**: `createDeflate`, `createDeflateRaw`, `createGzip`, `createZip`
 *    wrap a destination stream with a write/flush/close interface, while `createInflate`,
 *    `createInflateRaw`, `createGunzip` and `createUnzip` decompress into another stream and
 *    accept an output size limit;
 *  - **Codec objects**: `Gzip`, `Gunzip`, `Deflate`, `Inflate`, `DeflateRaw`, `InflateRaw`
 *    and `Unzip` are synchronous, Node.js compatible codec classes with explicit flush
 *    control, kept for packages such as minizlib and tar;
 *  - **Constants**: `NO_COMPRESSION`, `BEST_SPEED`, `BEST_COMPRESSION` and
 *    `DEFAULT_COMPRESSION`, plus the `constants` object with the flush flags, strategies,
 *    format identifiers and error codes.
 *
 *  Concepts:
 *
 *  - **The three formats**: deflate (also called zlib format) is deflate data with a 2-byte
 *    header whose first byte is 0x78 and an Adler-32 trailer; gzip is the gzip container
 *    (magic 1f 8b, header, CRC-32 trailer); raw deflate is the bare compressed stream with
 *    no header or trailer, and protocols such as WebSocket permessage-deflate embed it.
 *    The formats are not interchangeable: decompressing with the wrong function fails with
 *    `Z_DATA_ERROR`. `unzip` is the exception and detects the format from the header.
 *  - **`zip` and `unzip`**: kept for compatibility with older fibjs code. In this build
 *    `zip` produces the same zlib-format stream as `deflate`, and `unzip` auto-detects gzip
 *    (first bytes 1f 8b, needs at least the 10-byte header) or zlib (first byte 0x78) and
 *    rejects raw deflate, matching Node.js. Format detection looks at the first bytes of the
 *    input, which matters for the streaming forms.
 *  - **Compression level**: accepted as an integer or as `{ level }`. `DEFAULT_COMPRESSION`
 *    (-1) balances speed and ratio, 0 stores without compressing, 1 is the fastest and 9
 *    gives the smallest output. The one-shot functions and `createZip` clamp out-of-range
 *    values, but the codec constructors pass them to zlib unchanged: an invalid level,
 *    windowBits, memLevel or strategy leaves the codec closed and the first `_processChunk`
 *    throws ERR_ZLIB_BINDING_CLOSED. A higher level costs CPU time and memory, and the gain
 *    shrinks as the level rises.
 *  - **Flush modes and streaming**: the codec classes and the stream wrappers expose the
 *    zlib flush flags. `Z_NO_FLUSH` (0) absorbs input and emits only what is convenient, so
 *    a call may return an empty Buffer; `Z_SYNC_FLUSH` (2) emits everything produced so far
 *    and keeps the stream decodable; `Z_FINISH` (4) ends the stream, writes the trailer and
 *    resets the codec for a new message. The stream wrappers map `write` to Z_NO_FLUSH,
 *    `flush` to Z_SYNC_FLUSH and `close` to Z_FINISH; forgetting `close` leaves the trailer
 *    missing and the data cannot be decompressed.
 *  - **Window, memory and strategy parameters**: the codec constructors read `windowBits`
 *    (gzip 31, zlib 15, raw -15, unzip 47 by default; an explicit value also selects the
 *    container, so `new zlib.Gzip({ windowBits: 15 })` writes zlib format), `memLevel`
 *    (1 to 9, default 8) and `strategy` (`Z_DEFAULT_STRATEGY` 0, `Z_FILTERED` 1,
 *    `Z_HUFFMAN_ONLY` 2, `Z_RLE` 3, `Z_FIXED` 4). The Node.js options `dictionary`,
 *    `chunkSize`, `flush` and `finishFlush` have no equivalent in fibjs.
 *  - **Results and errors**: decompressing to zero bytes returns null instead of an empty
 *    Buffer, and input that ends early is decoded up to its last complete output without an
 *    error, where Node.js raises Z_BUF_ERROR. Wrong-format or corrupt data throws an Error
 *    with `code` `Z_DATA_ERROR` (number 20024, message "data error"); a closed codec throws
 *    ERR_ZLIB_BINDING_CLOSED; exceeding maxSize/maxOutputLength throws a RangeError
 *    (number 20013).
 *  - **Call forms**: every `async` member blocks the calling fiber when called without a
 *    callback, accepts a trailing `(err, result)` callback, and has `...Sync`/`...Async`
 *    aliases plus a promise-returning form under `zlib.promises`. The codec classes are
 *    purely synchronous. See the coroutine module for the fiber model.
 *  - **Relation to HTTP and WebSocket**: HttpClient and HttpServer decompress gzip/deflate
 *    response bodies transparently when `enableEncoding` is on, and WebSocket negotiates
 *    permessage-deflate as raw deflate finished with Z_SYNC_FLUSH; see those modules for the
 *    protocol side.
 *
 *  Import:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  ```
 *
 *  Example 1 — one-shot round trip for every format:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *
 *  const text = 'hello, world';
 *
 *  // deflate/inflate use the zlib format, gzip/gunzip the gzip container.
 *  console.log(zlib.inflate(zlib.deflate(text)).toString()); // hello, world
 *  console.log(zlib.gunzip(zlib.gzip(text)).toString());     // hello, world
 *
 *  // Raw deflate has no header; unzip is the auto-detecting decompressor.
 *  console.log(zlib.inflateRaw(zlib.deflateRaw(text)).toString()); // hello, world
 *  console.log(zlib.unzip(zlib.zip(text)).toString());             // hello, world
 *
 *  // A higher level trades CPU time for a smaller result.
 *  const stored = zlib.deflate('abc'.repeat(1000), { level: 0 });
 *  const packed = zlib.deflate('abc'.repeat(1000), { level: 9 });
 *  console.log(stored.length > packed.length); // true
 *  ```
 *
 *  Example 2 — a streaming codec pipeline over memory streams:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const io = require('io');
 *
 *  // Compress into a memory stream. write() is Z_NO_FLUSH, flush() is Z_SYNC_FLUSH
 *  // and close() is Z_FINISH: only close() writes the gzip trailer.
 *  const packed = new io.MemoryStream();
 *  const gz = zlib.createGzip(packed);
 *  gz.write('hello ');
 *  gz.write('world');
 *  gz.close();
 *
 *  // Decompress the packed bytes into a second stream.
 *  packed.rewind();
 *  const plain = new io.MemoryStream();
 *  const gunzip = zlib.createGunzip(plain);
 *  gunzip.write(packed.readAll());
 *  gunzip.close();
 *
 *  plain.rewind();
 *  console.log(plain.readAll().toString()); // hello world
 *  ```
 *
 *  Example 3 — formats, headers and auto-detection:
 *  ```JavaScript
 *  const zlib = require('zlib');
 *
 *  const text = 'hello, world';
 *  const deflated = zlib.deflate(text);
 *  const gzipped = zlib.gzip(text);
 *  const raw = zlib.deflateRaw(text);
 *
 *  // The first bytes identify the container.
 *  console.log(deflated[0].toString(16));                         // 78
 *  console.log(gzipped[0].toString(16), gzipped[1].toString(16)); // 1f 8b
 *  console.log(raw[0].toString(16));                              // cb
 *
 *  // unzip detects gzip and zlib data, but raw deflate is rejected.
 *  console.log(zlib.unzip(gzipped).toString());  // hello, world
 *  console.log(zlib.unzip(deflated).toString()); // hello, world
 *  try {
 *      zlib.unzip(raw);
 *  } catch (e) {
 *      console.log(e.code); // Z_DATA_ERROR
 *  }
 *
 *  // zip is a compatibility alias: it produces the same bytes as deflate.
 *  console.log(zlib.zip(text).equals(deflated)); // true
 *  ```
 *
 *  Notes: Node.js `zlib.createGzip()` returns a Transform stream, while fibjs
 *  `zlib.createGzip(to)` wraps an existing destination stream and the returned object is
 *  write-only (reading from it throws [20009]). Node.js also accepts TypedArrays and
 *  DataViews in the one-shot functions, while fibjs accepts Buffer and string (utf8); the
 *  options `dictionary`, `chunkSize`, `info`, `flush` and `finishFlush` are not supported.
 *  In return fibjs adds `zip`/`unzip`, the blocking and Sync/Async call forms and the
 *  `zlib.promises` namespace.
 *
 */
declare module 'zlib' {
    /**
     * @description deflate compression level, sets no compression
     */
    export const NO_COMPRESSION: 0;

    /**
     * @description deflate compression level, sets the fastest compression
     */
    export const BEST_SPEED: 1;

    /**
     * @description deflate compression level, sets the best compression
     */
    export const BEST_COMPRESSION: 9;

    /**
     * @description deflate compression level, sets the default setting
     */
    export const DEFAULT_COMPRESSION: -1;

    /**
     * @description The constants object of the module, see zlib_constants
     *
     *      It carries the flush flags (`Z_NO_FLUSH`, `Z_SYNC_FLUSH`, `Z_FINISH`, ...), the
     *      compression levels (`Z_BEST_SPEED`, `Z_BEST_COMPRESSION`, `Z_DEFAULT_COMPRESSION`),
     *      the strategies (`Z_FILTERED`, `Z_HUFFMAN_ONLY`, `Z_RLE`, `Z_FIXED`), the format
     *      identifiers (`DEFLATE`, `GZIP`, `DEFLATERAW`, ...) and the error codes
     *      (`Z_DATA_ERROR`, `Z_STREAM_ERROR`, ...). `zlib.constants` is the same object as the
     *      `zlib_constants` module.
     *
     */
    const constants: typeof import ('zlib_constants');

    /**
     * @description The Gzip codec class, see Gzip
     *
     *      Build an instance with `new zlib.Gzip(opts)`; it compresses data to the gzip format
     *      synchronously through `_processChunk`. For the fibjs stream forms use `gzip`,
     *      `gzipTo` or `createGzip` instead.
     *
     */
    const Gzip: typeof Class_Gzip;

    /**
     * @description The Gunzip codec class, see Gunzip
     *
     *      Build an instance with `new zlib.Gunzip(opts)`; it decompresses gzip data
     *      synchronously through `_processChunk` and is the codec counterpart of `gunzip`,
     *      `gunzipTo` and `createGunzip`.
     *
     */
    const Gunzip: typeof Class_Gunzip;

    /**
     * @description The Deflate codec class, see Deflate
     *
     *      Build an instance with `new zlib.Deflate(opts)`; it compresses data to the zlib
     *      format synchronously through `_processChunk`. The module-level `deflate`, `deflateTo`
     *      and `createDeflate` are the equivalent fibjs stream APIs.
     *
     */
    const Deflate: typeof Class_Deflate;

    /**
     * @description The Inflate codec class, see Inflate
     *
     *      Build an instance with `new zlib.Inflate(opts)`; it decompresses zlib-format data
     *      synchronously through `_processChunk` and rejects gzip or raw deflate input with
     *      `Z_DATA_ERROR`.
     *
     */
    const Inflate: typeof Class_Inflate;

    /**
     * @description The DeflateRaw codec class, see DeflateRaw
     *
     *      Build an instance with `new zlib.DeflateRaw(opts)`; it compresses data to raw deflate
     *      (no header or trailer) synchronously through `_processChunk`. Raw deflate is the
     *      payload of protocols such as WebSocket permessage-deflate.
     *
     */
    const DeflateRaw: typeof Class_DeflateRaw;

    /**
     * @description The InflateRaw codec class, see InflateRaw
     *
     *      Build an instance with `new zlib.InflateRaw(opts)`; it decompresses raw deflate data
     *      synchronously through `_processChunk`. Feed it exactly the bytes that a raw deflate
     *      producer wrote.
     *
     */
    const InflateRaw: typeof Class_InflateRaw;

    /**
     * @description The Unzip codec class, see Unzip
     *
     *      Build an instance with `new zlib.Unzip(opts)`; it decompresses gzip or zlib-format
     *      data and detects the format from the first bytes of the input. Use it when the
     *      container is not known in advance.
     *
     */
    const Unzip: typeof Class_Unzip;

    /**
     * @description Creates a zlib-format compressor that writes into `to`
     *
     *      The returned stream is write-only: `write` adds data with Z_NO_FLUSH, `flush` emits
     *      the pending output with Z_SYNC_FLUSH and `close` finishes the stream with Z_FINISH.
     *      `read`/`readBuffer` and the `fd` property throw the invalid-call error [20009], and
     *      the compression level is the default, so use `createZip(to, level)` or the `Deflate`
     *      codec class to select one. There is no Node.js counterpart: `zlib.createDeflate()`
     *      returns a Transform stream and takes no destination.
     *
     *      @param to the destination stream that receives the compressed data
     *      @return the write-only compressor stream
     *
     */
    function createDeflate(to: Class_Stream | Class_StreamPromise): Class_Stream;

    /**
     * @description Creates a raw-deflate compressor that writes into `to`
     *
     *      Like `createDeflate`, but the stream carries raw deflate data with no zlib header or
     *      trailer; decode it with `createInflateRaw` or `zlib.inflateRaw`. The returned stream
     *      is write-only and follows the write/flush/close contract of `createDeflate`, with the
     *      default compression level.
     *
     *      @param to the destination stream that receives the raw compressed data
     *      @return the write-only compressor stream
     *
     */
    function createDeflateRaw(to: Class_Stream | Class_StreamPromise): Class_Stream;

    /**
     * @description Creates a gzip decompressor that writes into `to`
     *
     *      The returned stream is write-only: feed the gzip data with `write` and call `close`
     *      when the input ends. `maxSize` limits the total number of decompressed bytes; the
     *      default -1 means no limit and exceeding it throws a RangeError [20013]. Corrupt or
     *      non-gzip input fails with `Z_DATA_ERROR`, and input that ends early is decoded up to
     *      its last complete output without an error.
     *
     *      @param to the destination stream that receives the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *      @return the write-only decompressor stream
     *
     */
    function createGunzip(to: Class_Stream | Class_StreamPromise, maxSize?: number): Class_Stream;

    /**
     * @description Creates a gzip compressor that writes into `to`
     *
     *      The streaming form of `zlib.gzip`: the returned write-only stream compresses with the
     *      default level, maps `write` to Z_NO_FLUSH, `flush` to Z_SYNC_FLUSH and `close` to
     *      Z_FINISH. The gzip trailer is written by `close`, so output read before it is an
     *      incomplete container. Unlike the `Gzip` codec class and `gzipTo`, this factory has no
     *      level parameter.
     *
     *      @param to the destination stream that receives the compressed data
     *      @return the write-only compressor stream
     *
     */
    function createGzip(to: Class_Stream | Class_StreamPromise): Class_Stream;

    /**
     * @description Creates a zlib-format decompressor that writes into `to`
     *
     *      The stream accepts zlib-format data and rejects gzip or raw deflate input with
     *      `Z_DATA_ERROR`. `maxSize` limits the total number of decompressed bytes (default -1,
     *      no limit; over the limit a RangeError [20013] is thrown). Feed the whole input, call
     *      `close`, then read the destination from its beginning.
     *
     *      @param to the destination stream that receives the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *      @return the write-only decompressor stream
     *
     */
    function createInflate(to: Class_Stream | Class_StreamPromise, maxSize?: number): Class_Stream;

    /**
     * @description Creates a raw-deflate decompressor that writes into `to`
     *
     *      The stream accepts raw deflate data (no header or trailer) and rejects zlib-format
     *      input with `Z_DATA_ERROR`. `maxSize` limits the total number of decompressed bytes
     *      (default -1, no limit; over the limit a RangeError [20013] is thrown). Raw deflate is
     *      the payload of protocols such as WebSocket permessage-deflate.
     *
     *      @param to the destination stream that receives the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *      @return the write-only decompressor stream
     *
     */
    function createInflateRaw(to: Class_Stream | Class_StreamPromise, maxSize?: number): Class_Stream;

    /**
     * @description Compresses data with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer or a string; a string is encoded as utf8. The level is either
     *      an integer or an options object read as `{ level }`; the default is
     *      DEFAULT_COMPRESSION (-1) and values outside the -1..9 range are clamped. The result
     *      starts with the 0x78 byte of the zlib header and ends with an Adler-32 checksum, and
     *      `inflate` or `unzip` decode it. Other options are ignored, and secrets are not
     *      hidden: deflate is a compression algorithm, not an encryption one. Node.js accepts
     *      TypedArrays and DataViews here, while fibjs accepts Buffer and string. Like every
     *      async member it has blocking, callback, Sync/Async and `zlib.promises` forms.
     *
     *      Example — a fixed input produces a deterministic zlib stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      console.log(zlib.deflate('hello, world').toString('hex'));
     *      // 789ccb48cdc9c9d75128cf2fca4901001d540489
     *
     *      // The level trades speed for size; level 0 stores, level 9 compresses.
     *      const stored = zlib.deflate('abc'.repeat(1000), { level: 0 });
     *      const best = zlib.deflate('abc'.repeat(1000), { level: 9 });
     *      console.log(stored.length > best.length); // true
     *      console.log(zlib.inflate(best).toString() === 'abc'.repeat(1000)); // true
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function deflate(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    function deflate(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Compresses data with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer or a string; a string is encoded as utf8. The level is either
     *      an integer or an options object read as `{ level }`; the default is
     *      DEFAULT_COMPRESSION (-1) and values outside the -1..9 range are clamped. The result
     *      starts with the 0x78 byte of the zlib header and ends with an Adler-32 checksum, and
     *      `inflate` or `unzip` decode it. Other options are ignored, and secrets are not
     *      hidden: deflate is a compression algorithm, not an encryption one. Node.js accepts
     *      TypedArrays and DataViews here, while fibjs accepts Buffer and string. Like every
     *      async member it has blocking, callback, Sync/Async and `zlib.promises` forms.
     *
     *      Example — a fixed input produces a deterministic zlib stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      console.log(zlib.deflate('hello, world').toString('hex'));
     *      // 789ccb48cdc9c9d75128cf2fca4901001d540489
     *
     *      // The level trades speed for size; level 0 stores, level 9 compresses.
     *      const stored = zlib.deflate('abc'.repeat(1000), { level: 0 });
     *      const best = zlib.deflate('abc'.repeat(1000), { level: 9 });
     *      console.log(stored.length > best.length); // true
     *      console.log(zlib.inflate(best).toString() === 'abc'.repeat(1000)); // true
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function deflateSync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Compresses data with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer or a string; a string is encoded as utf8. The level is either
     *      an integer or an options object read as `{ level }`; the default is
     *      DEFAULT_COMPRESSION (-1) and values outside the -1..9 range are clamped. The result
     *      starts with the 0x78 byte of the zlib header and ends with an Adler-32 checksum, and
     *      `inflate` or `unzip` decode it. Other options are ignored, and secrets are not
     *      hidden: deflate is a compression algorithm, not an encryption one. Node.js accepts
     *      TypedArrays and DataViews here, while fibjs accepts Buffer and string. Like every
     *      async member it has blocking, callback, Sync/Async and `zlib.promises` forms.
     *
     *      Example — a fixed input produces a deterministic zlib stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      console.log(zlib.deflate('hello, world').toString('hex'));
     *      // 789ccb48cdc9c9d75128cf2fca4901001d540489
     *
     *      // The level trades speed for size; level 0 stores, level 9 compresses.
     *      const stored = zlib.deflate('abc'.repeat(1000), { level: 0 });
     *      const best = zlib.deflate('abc'.repeat(1000), { level: 9 });
     *      console.log(stored.length > best.length); // true
     *      console.log(zlib.inflate(best).toString() === 'abc'.repeat(1000)); // true
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function deflateAsync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (encoded as
     *      utf8). `stm` receives the compressed bytes at its current position and is left at the
     *      end, so rewind it before reading. The level must be an integer (an options object is
     *      a TypeError) and is clamped to -1..9. The call finishes the stream with Z_FINISH, so
     *      no separate `close` is needed, and it returns no value.
     *
     *      Example — compress into a memory stream and decode it:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      zlib.deflateTo('hello, world', stm, zlib.BEST_COMPRESSION);
     *      stm.rewind();
     *
     *      console.log(zlib.inflate(stm.readAll()).toString()); // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    function deflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (encoded as
     *      utf8). `stm` receives the compressed bytes at its current position and is left at the
     *      end, so rewind it before reading. The level must be an integer (an options object is
     *      a TypeError) and is clamped to -1..9. The call finishes the stream with Z_FINISH, so
     *      no separate `close` is needed, and it returns no value.
     *
     *      Example — compress into a memory stream and decode it:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      zlib.deflateTo('hello, world', stm, zlib.BEST_COMPRESSION);
     *      stm.rewind();
     *
     *      console.log(zlib.inflate(stm.readAll()).toString()); // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (encoded as
     *      utf8). `stm` receives the compressed bytes at its current position and is left at the
     *      end, so rewind it before reading. The level must be an integer (an options object is
     *      a TypeError) and is clamped to -1..9. The call finishes the stream with Z_FINISH, so
     *      no separate `close` is needed, and it returns no value.
     *
     *      Example — compress into a memory stream and decode it:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      zlib.deflateTo('hello, world', stm, zlib.BEST_COMPRESSION);
     *      stm.rewind();
     *
     *      console.log(zlib.inflate(stm.readAll()).toString()); // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

    /**
     * @description Decompresses data compressed with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer or a string (decoded as utf8). The size limit is either an
     *      integer or an options object read as `{ maxOutputLength }`; the default is -1 (no
     *      limit) and exceeding it throws a RangeError [20013]. Only zlib-format input is
     *      accepted: gzip or raw deflate data fails with `Z_DATA_ERROR` (use `gunzip` or
     *      `inflateRaw`). Corrupt data fails with `Z_DATA_ERROR`; truncated input is decoded up
     *      to its last complete output without an error, and a result of zero bytes is null
     *      instead of an empty Buffer. Node.js raises Z_BUF_ERROR for truncated input and
     *      returns an empty Buffer for an empty result.
     *
     *      Example — enforce an output limit:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const packed = zlib.deflate('hello, world');
     *      console.log(zlib.inflate(packed, { maxOutputLength: 1024 }).toString());
     *      // hello, world
     *
     *      try {
     *          zlib.inflate(packed, { maxOutputLength: 3 });
     *      } catch (e) {
     *          console.log(e.name, e.number); // RangeError 20013
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function inflate(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    function inflate(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer or a string (decoded as utf8). The size limit is either an
     *      integer or an options object read as `{ maxOutputLength }`; the default is -1 (no
     *      limit) and exceeding it throws a RangeError [20013]. Only zlib-format input is
     *      accepted: gzip or raw deflate data fails with `Z_DATA_ERROR` (use `gunzip` or
     *      `inflateRaw`). Corrupt data fails with `Z_DATA_ERROR`; truncated input is decoded up
     *      to its last complete output without an error, and a result of zero bytes is null
     *      instead of an empty Buffer. Node.js raises Z_BUF_ERROR for truncated input and
     *      returns an empty Buffer for an empty result.
     *
     *      Example — enforce an output limit:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const packed = zlib.deflate('hello, world');
     *      console.log(zlib.inflate(packed, { maxOutputLength: 1024 }).toString());
     *      // hello, world
     *
     *      try {
     *          zlib.inflate(packed, { maxOutputLength: 3 });
     *      } catch (e) {
     *          console.log(e.name, e.number); // RangeError 20013
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function inflateSync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Decompresses data compressed with the deflate algorithm (zlib format)
     *
     *      `data` may be a Buffer or a string (decoded as utf8). The size limit is either an
     *      integer or an options object read as `{ maxOutputLength }`; the default is -1 (no
     *      limit) and exceeding it throws a RangeError [20013]. Only zlib-format input is
     *      accepted: gzip or raw deflate data fails with `Z_DATA_ERROR` (use `gunzip` or
     *      `inflateRaw`). Corrupt data fails with `Z_DATA_ERROR`; truncated input is decoded up
     *      to its last complete output without an error, and a result of zero bytes is null
     *      instead of an empty Buffer. Node.js raises Z_BUF_ERROR for truncated input and
     *      returns an empty Buffer for an empty result.
     *
     *      Example — enforce an output limit:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const packed = zlib.deflate('hello, world');
     *      console.log(zlib.inflate(packed, { maxOutputLength: 1024 }).toString());
     *      // hello, world
     *
     *      try {
     *          zlib.inflate(packed, { maxOutputLength: 3 });
     *      } catch (e) {
     *          console.log(e.name, e.number); // RangeError 20013
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function inflateAsync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object
     *      (zlib format)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Wrong formats fail with
     *      `Z_DATA_ERROR`, and a result of zero bytes leaves the destination empty.
     *
     *      Example — copy the decompressed bytes into a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      zlib.inflateTo(zlib.deflate('hello, world'), stm);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    function inflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object
     *      (zlib format)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Wrong formats fail with
     *      `Z_DATA_ERROR`, and a result of zero bytes leaves the destination empty.
     *
     *      Example — copy the decompressed bytes into a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      zlib.inflateTo(zlib.deflate('hello, world'), stm);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object
     *      (zlib format)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Wrong formats fail with
     *      `Z_DATA_ERROR`, and a result of zero bytes leaves the destination empty.
     *
     *      Example — copy the decompressed bytes into a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      zlib.inflateTo(zlib.deflate('hello, world'), stm);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

    /**
     * @description Compresses data with the gzip algorithm
     *
     *      `data` may be a Buffer or a string (encoded as utf8). `options` must be an object and
     *      only `{ level }` is read, so `zlib.gzip(data, 9)` is a TypeError; the default is
     *      DEFAULT_COMPRESSION and the value is clamped to -1..9. The result is a gzip container
     *      that starts with 1f 8b, ends with a CRC-32 trailer and carries a zero mtime, so the
     *      same input and level produce the same bytes in this build. Decode it with `gunzip` or
     *      `unzip`; `inflate` rejects it. Node.js takes the options object in the same position
     *      and additionally supports a `dictionary`, which fibjs ignores.
     *
     *      Example — level differences and round trip:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const data = 'abc'.repeat(1000);
     *      const fast = zlib.gzip(data, { level: 1 });
     *      const best = zlib.gzip(data, { level: 9 });
     *
     *      // The gzip XFL byte records the level hint (4 = fastest, 2 = best, 0 = default).
     *      console.log(fast[8], best[8]); // 4 2
     *      console.log(zlib.gunzip(best).toString() === data); // true
     *      ```
     *
     *      @param data the data to compress
     *      @param options the compression options, read as { level }
     *      @return the compressed binary data
     *
     */
    function gzip(data: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Buffer;

    function gzip(data: Class_Buffer | string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Compresses data with the gzip algorithm
     *
     *      `data` may be a Buffer or a string (encoded as utf8). `options` must be an object and
     *      only `{ level }` is read, so `zlib.gzip(data, 9)` is a TypeError; the default is
     *      DEFAULT_COMPRESSION and the value is clamped to -1..9. The result is a gzip container
     *      that starts with 1f 8b, ends with a CRC-32 trailer and carries a zero mtime, so the
     *      same input and level produce the same bytes in this build. Decode it with `gunzip` or
     *      `unzip`; `inflate` rejects it. Node.js takes the options object in the same position
     *      and additionally supports a `dictionary`, which fibjs ignores.
     *
     *      Example — level differences and round trip:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const data = 'abc'.repeat(1000);
     *      const fast = zlib.gzip(data, { level: 1 });
     *      const best = zlib.gzip(data, { level: 9 });
     *
     *      // The gzip XFL byte records the level hint (4 = fastest, 2 = best, 0 = default).
     *      console.log(fast[8], best[8]); // 4 2
     *      console.log(zlib.gunzip(best).toString() === data); // true
     *      ```
     *
     *      @param data the data to compress
     *      @param options the compression options, read as { level }
     *      @return the compressed binary data
     *
     */
    function gzipSync(data: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Compresses data with the gzip algorithm
     *
     *      `data` may be a Buffer or a string (encoded as utf8). `options` must be an object and
     *      only `{ level }` is read, so `zlib.gzip(data, 9)` is a TypeError; the default is
     *      DEFAULT_COMPRESSION and the value is clamped to -1..9. The result is a gzip container
     *      that starts with 1f 8b, ends with a CRC-32 trailer and carries a zero mtime, so the
     *      same input and level produce the same bytes in this build. Decode it with `gunzip` or
     *      `unzip`; `inflate` rejects it. Node.js takes the options object in the same position
     *      and additionally supports a `dictionary`, which fibjs ignores.
     *
     *      Example — level differences and round trip:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const data = 'abc'.repeat(1000);
     *      const fast = zlib.gzip(data, { level: 1 });
     *      const best = zlib.gzip(data, { level: 9 });
     *
     *      // The gzip XFL byte records the level hint (4 = fastest, 2 = best, 0 = default).
     *      console.log(fast[8], best[8]); // 4 2
     *      console.log(zlib.gunzip(best).toString() === data); // true
     *      ```
     *
     *      @param data the data to compress
     *      @param options the compression options, read as { level }
     *      @return the compressed binary data
     *
     */
    function gzipAsync(data: Class_Buffer | string, options?: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Compresses data into a stream object with the gzip algorithm
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the gzip bytes at its current position and is left at the end, so rewind it
     *      before reading. There is no options parameter and the default compression level is
     *      used; use the `Gzip` codec class or `gzip` when a level or the other zlib parameters
     *      are needed. `close` is implicit: the gzip trailer is written before the call returns.
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *
     */
    function gzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise): void;

    function gzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Compresses data into a stream object with the gzip algorithm
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the gzip bytes at its current position and is left at the end, so rewind it
     *      before reading. There is no options parameter and the default compression level is
     *      used; use the `Gzip` codec class or `gzip` when a level or the other zlib parameters
     *      are needed. `close` is implicit: the gzip trailer is written before the call returns.
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *
     */
    function gzipToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise): void;

    /**
     * @description Compresses data into a stream object with the gzip algorithm
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the gzip bytes at its current position and is left at the end, so rewind it
     *      before reading. There is no options parameter and the default compression level is
     *      used; use the `Gzip` codec class or `gzip` when a level or the other zlib parameters
     *      are needed. `close` is implicit: the gzip trailer is written before the call returns.
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *
     */
    function gzipToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise): Promise<void>;

    /**
     * @description Decompresses gzip data
     *
     *      `data` may be a Buffer or a string (decoded as utf8; gzip data is binary, so a string
     *      only makes sense for already-binary text). The size limit is an integer or an options
     *      object read as `{ maxOutputLength }`; the default is -1 (no limit) and exceeding it
     *      throws a RangeError [20013]. Only gzip input is accepted, but concatenated gzip
     *      members are decoded as a single output; bytes after the last member that are not
     *      another valid member fail with `Z_DATA_ERROR`. Truncated input is decoded up to its
     *      last complete output without an error, and a zero-byte result is null. Node.js raises
     *      Z_BUF_ERROR for truncated input and returns an empty Buffer for an empty result.
     *
     *      Example — concatenated members form one output:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const members = Buffer.concat([zlib.gzip('hello, '), zlib.gzip('world')]);
     *      console.log(zlib.gunzip(members).toString()); // hello, world
     *
     *      try {
     *          zlib.gunzip(Buffer.concat([zlib.gzip('hello'), Buffer.from('junk')]));
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function gunzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    function gunzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Decompresses gzip data
     *
     *      `data` may be a Buffer or a string (decoded as utf8; gzip data is binary, so a string
     *      only makes sense for already-binary text). The size limit is an integer or an options
     *      object read as `{ maxOutputLength }`; the default is -1 (no limit) and exceeding it
     *      throws a RangeError [20013]. Only gzip input is accepted, but concatenated gzip
     *      members are decoded as a single output; bytes after the last member that are not
     *      another valid member fail with `Z_DATA_ERROR`. Truncated input is decoded up to its
     *      last complete output without an error, and a zero-byte result is null. Node.js raises
     *      Z_BUF_ERROR for truncated input and returns an empty Buffer for an empty result.
     *
     *      Example — concatenated members form one output:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const members = Buffer.concat([zlib.gzip('hello, '), zlib.gzip('world')]);
     *      console.log(zlib.gunzip(members).toString()); // hello, world
     *
     *      try {
     *          zlib.gunzip(Buffer.concat([zlib.gzip('hello'), Buffer.from('junk')]));
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function gunzipSync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Decompresses gzip data
     *
     *      `data` may be a Buffer or a string (decoded as utf8; gzip data is binary, so a string
     *      only makes sense for already-binary text). The size limit is an integer or an options
     *      object read as `{ maxOutputLength }`; the default is -1 (no limit) and exceeding it
     *      throws a RangeError [20013]. Only gzip input is accepted, but concatenated gzip
     *      members are decoded as a single output; bytes after the last member that are not
     *      another valid member fail with `Z_DATA_ERROR`. Truncated input is decoded up to its
     *      last complete output without an error, and a zero-byte result is null. Node.js raises
     *      Z_BUF_ERROR for truncated input and returns an empty Buffer for an empty result.
     *
     *      Example — concatenated members form one output:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const members = Buffer.concat([zlib.gzip('hello, '), zlib.gzip('world')]);
     *      console.log(zlib.gunzip(members).toString()); // hello, world
     *
     *      try {
     *          zlib.gunzip(Buffer.concat([zlib.gzip('hello'), Buffer.from('junk')]));
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function gunzipAsync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decompresses data compressed with the gzip algorithm into a stream object
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Concatenated gzip members
     *      are decoded as one output; non-gzip data fails with `Z_DATA_ERROR`.
     *
     *      Example — stream the gzip bytes from a source stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write(zlib.gzip('hello, world'));
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.gunzipTo(src, stm);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function gunzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    function gunzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses data compressed with the gzip algorithm into a stream object
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Concatenated gzip members
     *      are decoded as one output; non-gzip data fails with `Z_DATA_ERROR`.
     *
     *      Example — stream the gzip bytes from a source stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write(zlib.gzip('hello, world'));
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.gunzipTo(src, stm);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function gunzipToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    /**
     * @description Decompresses data compressed with the gzip algorithm into a stream object
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Concatenated gzip members
     *      are decoded as one output; non-gzip data fails with `Z_DATA_ERROR`.
     *
     *      Example — stream the gzip bytes from a source stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write(zlib.gzip('hello, world'));
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.gunzipTo(src, stm);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function gunzipToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

    /**
     * @description Compresses data with the deflateRaw algorithm
     *
     *      `data` may be a Buffer or a string (encoded as utf8). The level handling is the same
     *      as `deflate`: an integer or an options object read as `{ level }`, default
     *      DEFAULT_COMPRESSION, clamped to -1..9. Raw deflate carries no header and no
     *      checksum, which makes the output smaller than the zlib form and is what protocols
     *      such as WebSocket permessage-deflate embed; only `inflateRaw` decodes it. The result
     *      has no identifiable magic bytes, so it cannot be recognized by `unzip`.
     *
     *      Example — raw streams carry no identifying header:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const raw = zlib.deflateRaw('hello, world');
     *      console.log(raw[0].toString(16));              // cb, not 78 or 1f
     *      console.log(zlib.inflateRaw(raw).toString());  // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function deflateRaw(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    function deflateRaw(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Compresses data with the deflateRaw algorithm
     *
     *      `data` may be a Buffer or a string (encoded as utf8). The level handling is the same
     *      as `deflate`: an integer or an options object read as `{ level }`, default
     *      DEFAULT_COMPRESSION, clamped to -1..9. Raw deflate carries no header and no
     *      checksum, which makes the output smaller than the zlib form and is what protocols
     *      such as WebSocket permessage-deflate embed; only `inflateRaw` decodes it. The result
     *      has no identifiable magic bytes, so it cannot be recognized by `unzip`.
     *
     *      Example — raw streams carry no identifying header:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const raw = zlib.deflateRaw('hello, world');
     *      console.log(raw[0].toString(16));              // cb, not 78 or 1f
     *      console.log(zlib.inflateRaw(raw).toString());  // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function deflateRawSync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Compresses data with the deflateRaw algorithm
     *
     *      `data` may be a Buffer or a string (encoded as utf8). The level handling is the same
     *      as `deflate`: an integer or an options object read as `{ level }`, default
     *      DEFAULT_COMPRESSION, clamped to -1..9. Raw deflate carries no header and no
     *      checksum, which makes the output smaller than the zlib form and is what protocols
     *      such as WebSocket permessage-deflate embed; only `inflateRaw` decodes it. The result
     *      has no identifiable magic bytes, so it cannot be recognized by `unzip`.
     *
     *      Example — raw streams carry no identifying header:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const raw = zlib.deflateRaw('hello, world');
     *      console.log(raw[0].toString(16));              // cb, not 78 or 1f
     *      console.log(zlib.inflateRaw(raw).toString());  // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function deflateRawAsync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (deflateRaw)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the raw deflate bytes at its current position and is left at the end, so
     *      rewind it before reading. The level must be an integer (an options object is a
     *      TypeError) and is clamped to -1..9; the stream is finished before the call returns.
     *
     *      Example — compress a source stream into a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write('hello, world');
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.deflateRawTo(src, stm);
     *      stm.rewind();
     *
     *      console.log(zlib.inflateRaw(stm.readAll()).toString()); // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    function deflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (deflateRaw)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the raw deflate bytes at its current position and is left at the end, so
     *      rewind it before reading. The level must be an integer (an options object is a
     *      TypeError) and is clamped to -1..9; the stream is finished before the call returns.
     *
     *      Example — compress a source stream into a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write('hello, world');
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.deflateRawTo(src, stm);
     *      stm.rewind();
     *
     *      console.log(zlib.inflateRaw(stm.readAll()).toString()); // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateRawToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (deflateRaw)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the raw deflate bytes at its current position and is left at the end, so
     *      rewind it before reading. The level must be an integer (an options object is a
     *      TypeError) and is clamped to -1..9; the stream is finished before the call returns.
     *
     *      Example — compress a source stream into a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write('hello, world');
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.deflateRawTo(src, stm);
     *      stm.rewind();
     *
     *      console.log(zlib.inflateRaw(stm.readAll()).toString()); // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateRawToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

    /**
     * @description Decompresses deflateRaw data
     *
     *      `data` may be a Buffer or a string (decoded as utf8). The size limit is an integer or
     *      an options object read as `{ maxOutputLength }`; the default is -1 (no limit) and
     *      exceeding it throws a RangeError [20013]. Only raw deflate input is accepted:
     *      zlib-format data fails with `Z_DATA_ERROR` here (use `inflate`), gzip data with
     *      `gunzip`. Truncated input is decoded up to its last complete output without an error
     *      and corrupt data fails with `Z_DATA_ERROR`; a zero-byte result is null instead of the
     *      empty Buffer Node.js returns, and Node.js also raises Z_BUF_ERROR when the stream is
     *      truncated.
     *
     *      Example — decode raw data and reject the wrapped formats:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const raw = zlib.deflateRaw('hello, world');
     *      console.log(zlib.inflateRaw(raw).toString()); // hello, world
     *
     *      try {
     *          zlib.inflate(raw);
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function inflateRaw(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    function inflateRaw(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Decompresses deflateRaw data
     *
     *      `data` may be a Buffer or a string (decoded as utf8). The size limit is an integer or
     *      an options object read as `{ maxOutputLength }`; the default is -1 (no limit) and
     *      exceeding it throws a RangeError [20013]. Only raw deflate input is accepted:
     *      zlib-format data fails with `Z_DATA_ERROR` here (use `inflate`), gzip data with
     *      `gunzip`. Truncated input is decoded up to its last complete output without an error
     *      and corrupt data fails with `Z_DATA_ERROR`; a zero-byte result is null instead of the
     *      empty Buffer Node.js returns, and Node.js also raises Z_BUF_ERROR when the stream is
     *      truncated.
     *
     *      Example — decode raw data and reject the wrapped formats:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const raw = zlib.deflateRaw('hello, world');
     *      console.log(zlib.inflateRaw(raw).toString()); // hello, world
     *
     *      try {
     *          zlib.inflate(raw);
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function inflateRawSync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Decompresses deflateRaw data
     *
     *      `data` may be a Buffer or a string (decoded as utf8). The size limit is an integer or
     *      an options object read as `{ maxOutputLength }`; the default is -1 (no limit) and
     *      exceeding it throws a RangeError [20013]. Only raw deflate input is accepted:
     *      zlib-format data fails with `Z_DATA_ERROR` here (use `inflate`), gzip data with
     *      `gunzip`. Truncated input is decoded up to its last complete output without an error
     *      and corrupt data fails with `Z_DATA_ERROR`; a zero-byte result is null instead of the
     *      empty Buffer Node.js returns, and Node.js also raises Z_BUF_ERROR when the stream is
     *      truncated.
     *
     *      Example — decode raw data and reject the wrapped formats:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const raw = zlib.deflateRaw('hello, world');
     *      console.log(zlib.inflateRaw(raw).toString()); // hello, world
     *
     *      try {
     *          zlib.inflate(raw);
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function inflateRawAsync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object
     *      (inflateRaw)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Non-raw input fails with
     *      `Z_DATA_ERROR`, and a zero-byte result leaves the destination empty.
     *
     *      Example — decompress raw bytes from a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write(zlib.deflateRaw('hello, world'));
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.inflateRawTo(src, stm, 1024);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    function inflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object
     *      (inflateRaw)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Non-raw input fails with
     *      `Z_DATA_ERROR`, and a zero-byte result leaves the destination empty.
     *
     *      Example — decompress raw bytes from a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write(zlib.deflateRaw('hello, world'));
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.inflateRawTo(src, stm, 1024);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateRawToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object
     *      (inflateRaw)
     *
     *      `data` may be a Buffer, a source Stream (copied to its end) or a string (utf8). `stm`
     *      receives the decompressed bytes at its current position and is left at the end, so
     *      rewind it before reading. `maxSize` is an integer (an options object is a TypeError),
     *      default -1, and exceeding it throws a RangeError [20013]. Non-raw input fails with
     *      `Z_DATA_ERROR`, and a zero-byte result leaves the destination empty.
     *
     *      Example — decompress raw bytes from a memory stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *      const io = require('io');
     *
     *      const src = new io.MemoryStream();
     *      src.write(zlib.deflateRaw('hello, world'));
     *      src.rewind();
     *
     *      const stm = new io.MemoryStream();
     *      zlib.inflateRawTo(src, stm, 1024);
     *      stm.rewind();
     *
     *      console.log(stm.readAll().toString()); // hello, world
     *      ```
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateRawToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

    /**
     * @description Creates a zlib-format compressor that writes into `to`
     *
     *      Kept for compatibility: in this build `createZip` produces the same zlib-format bytes
     *      as `createDeflate`, so the consumers of either stream are interchangeable. Unlike
     *      `createDeflate` it accepts a compression level, which is clamped to the -1..9 range
     *      like the one-shot functions. The returned stream is write-only with the usual
     *      write/flush/close contract; it is unrelated to the `zip` archive module.
     *
     *      @param to the destination stream that receives the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *      @return the write-only compressor stream
     *
     */
    function createZip(to: Class_Stream | Class_StreamPromise, level?: number): Class_Stream;

    /**
     * @description Creates a decompressor that detects the input format and writes into `to`
     *
     *      The format is detected from the first chunk the stream receives: gzip (first bytes
     *      1f 8b, needs at least the 10-byte header) or zlib (first byte 0x78); raw deflate and
     *      unknown formats fail with `Z_DATA_ERROR`. Feed at least the complete header with the
     *      first `write`: unlike the one-shot `unzip`, the stream does not buffer bytes until it
     *      can decide. When the input arrives in small pieces, use `createGunzip` or
     *      `createInflate` instead. `maxSize` limits the total decompressed size (default -1, no
     *      limit; exceeding it throws a RangeError [20013]).
     *
     *      @param to the destination stream that receives the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *      @return the write-only decompressor stream
     *
     */
    function createUnzip(to: Class_Stream | Class_StreamPromise, maxSize?: number): Class_Stream;

    /**
     * @description Compresses data with the zip algorithm
     *
     *      Compatibility alias of `deflate`: in this build both produce the same zlib-format
     *      stream, so the result decodes with `inflate` or `unzip` (raw deflate is not what this
     *      member writes). The level handling is the same as `deflate` (an integer or an options
     *      object read as `{ level }`, default DEFAULT_COMPRESSION, clamped). Older fibjs code
     *      uses this name; the archive format handled by the `zip` module is unrelated.
     *
     *      Example — zip output is a deflate stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const data = 'hello, world';
     *      console.log(zlib.zip(data).equals(zlib.deflate(data))); // true
     *      console.log(zlib.unzip(zlib.zip(data)).toString());     // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function zip(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    function zip(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Compresses data with the zip algorithm
     *
     *      Compatibility alias of `deflate`: in this build both produce the same zlib-format
     *      stream, so the result decodes with `inflate` or `unzip` (raw deflate is not what this
     *      member writes). The level handling is the same as `deflate` (an integer or an options
     *      object read as `{ level }`, default DEFAULT_COMPRESSION, clamped). Older fibjs code
     *      uses this name; the archive format handled by the `zip` module is unrelated.
     *
     *      Example — zip output is a deflate stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const data = 'hello, world';
     *      console.log(zlib.zip(data).equals(zlib.deflate(data))); // true
     *      console.log(zlib.unzip(zlib.zip(data)).toString());     // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function zipSync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Compresses data with the zip algorithm
     *
     *      Compatibility alias of `deflate`: in this build both produce the same zlib-format
     *      stream, so the result decodes with `inflate` or `unzip` (raw deflate is not what this
     *      member writes). The level handling is the same as `deflate` (an integer or an options
     *      object read as `{ level }`, default DEFAULT_COMPRESSION, clamped). Older fibjs code
     *      uses this name; the archive format handled by the `zip` module is unrelated.
     *
     *      Example — zip output is a deflate stream:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      const data = 'hello, world';
     *      console.log(zlib.zip(data).equals(zlib.deflate(data))); // true
     *      console.log(zlib.unzip(zlib.zip(data)).toString());     // hello, world
     *      ```
     *
     *      @param data the data to compress
     *      @param level the compression level or the options
     *      @return the compressed binary data
     *
     */
    function zipAsync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Compresses data into a stream object with the zip algorithm
     *
     *      Alias of `deflateTo`: `data` may be a Buffer, a source Stream (copied to its end) or
     *      a string (utf8), and `stm` receives the zlib-format bytes at its current position and
     *      is left at the end, so rewind it before reading. The level must be an integer and is
     *      clamped to -1..9; the stream is finished before the call returns. The result is
     *      identical to the one `deflateTo` produces.
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function zipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    function zipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Compresses data into a stream object with the zip algorithm
     *
     *      Alias of `deflateTo`: `data` may be a Buffer, a source Stream (copied to its end) or
     *      a string (utf8), and `stm` receives the zlib-format bytes at its current position and
     *      is left at the end, so rewind it before reading. The level must be an integer and is
     *      clamped to -1..9; the stream is finished before the call returns. The result is
     *      identical to the one `deflateTo` produces.
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function zipToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    /**
     * @description Compresses data into a stream object with the zip algorithm
     *
     *      Alias of `deflateTo`: `data` may be a Buffer, a source Stream (copied to its end) or
     *      a string (utf8), and `stm` receives the zlib-format bytes at its current position and
     *      is left at the end, so rewind it before reading. The level must be an integer and is
     *      clamped to -1..9; the stream is finished before the call returns. The result is
     *      identical to the one `deflateTo` produces.
     *
     *      @param data the data to compress
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function zipToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

    /**
     * @description Decompresses zip data
     *
     *      `unzip` detects the container from the data itself: gzip (first bytes 1f 8b, needs a
     *      complete 10-byte header) or zlib format (first byte 0x78); raw deflate and unknown
     *      formats fail with `Z_DATA_ERROR`, matching Node.js `unzip`. The result is identical
     *      to `gunzip` for gzip data and to `inflate` for zlib data. The size limit is an
     *      integer or an options object read as `{ maxOutputLength }`; the default is -1 (no
     *      limit) and exceeding it throws a RangeError [20013]. A zero-byte result is null, and
     *      truncated input is decoded up to its last complete output without an error.
     *
     *      Example — one decompressor for both containers:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      console.log(zlib.unzip(zlib.gzip('gzip data')).toString());    // gzip data
     *      console.log(zlib.unzip(zlib.deflate('zlib data')).toString()); // zlib data
     *
     *      // Raw deflate has no recognizable header and is rejected.
     *      try {
     *          zlib.unzip(zlib.deflateRaw('raw data'));
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function unzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    function unzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Decompresses zip data
     *
     *      `unzip` detects the container from the data itself: gzip (first bytes 1f 8b, needs a
     *      complete 10-byte header) or zlib format (first byte 0x78); raw deflate and unknown
     *      formats fail with `Z_DATA_ERROR`, matching Node.js `unzip`. The result is identical
     *      to `gunzip` for gzip data and to `inflate` for zlib data. The size limit is an
     *      integer or an options object read as `{ maxOutputLength }`; the default is -1 (no
     *      limit) and exceeding it throws a RangeError [20013]. A zero-byte result is null, and
     *      truncated input is decoded up to its last complete output without an error.
     *
     *      Example — one decompressor for both containers:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      console.log(zlib.unzip(zlib.gzip('gzip data')).toString());    // gzip data
     *      console.log(zlib.unzip(zlib.deflate('zlib data')).toString()); // zlib data
     *
     *      // Raw deflate has no recognizable header and is rejected.
     *      try {
     *          zlib.unzip(zlib.deflateRaw('raw data'));
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function unzipSync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Decompresses zip data
     *
     *      `unzip` detects the container from the data itself: gzip (first bytes 1f 8b, needs a
     *      complete 10-byte header) or zlib format (first byte 0x78); raw deflate and unknown
     *      formats fail with `Z_DATA_ERROR`, matching Node.js `unzip`. The result is identical
     *      to `gunzip` for gzip data and to `inflate` for zlib data. The size limit is an
     *      integer or an options object read as `{ maxOutputLength }`; the default is -1 (no
     *      limit) and exceeding it throws a RangeError [20013]. A zero-byte result is null, and
     *      truncated input is decoded up to its last complete output without an error.
     *
     *      Example — one decompressor for both containers:
     *      ```JavaScript
     *      const zlib = require('zlib');
     *
     *      console.log(zlib.unzip(zlib.gzip('gzip data')).toString());    // gzip data
     *      console.log(zlib.unzip(zlib.deflate('zlib data')).toString()); // zlib data
     *
     *      // Raw deflate has no recognizable header and is rejected.
     *      try {
     *          zlib.unzip(zlib.deflateRaw('raw data'));
     *      } catch (e) {
     *          console.log(e.code); // Z_DATA_ERROR
     *      }
     *      ```
     *
     *      @param data the compressed data
     *      @param maxSize the decompression size limit or the options
     *      @return the decompressed binary data
     *
     */
    function unzipAsync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decompresses data compressed with the zip algorithm into a stream object
     *
     *      The stream form of `unzip`: `data` may be a Buffer, a source Stream (copied to its
     *      end) or a string (utf8), and `stm` receives the decompressed bytes at its current
     *      position and is left at the end, so rewind it before reading. `maxSize` is an
     *      integer, default -1, and exceeding it throws a RangeError [20013]; gzip and
     *      zlib-format input are both accepted, raw deflate fails with `Z_DATA_ERROR`.
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function unzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    function unzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses data compressed with the zip algorithm into a stream object
     *
     *      The stream form of `unzip`: `data` may be a Buffer, a source Stream (copied to its
     *      end) or a string (utf8), and `stm` receives the decompressed bytes at its current
     *      position and is left at the end, so rewind it before reading. `maxSize` is an
     *      integer, default -1, and exceeding it throws a RangeError [20013]; gzip and
     *      zlib-format input are both accepted, raw deflate fails with `Z_DATA_ERROR`.
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function unzipToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    /**
     * @description Decompresses data compressed with the zip algorithm into a stream object
     *
     *      The stream form of `unzip`: `data` may be a Buffer, a source Stream (copied to its
     *      end) or a string (utf8), and `stm` receives the decompressed bytes at its current
     *      position and is left at the end, so rewind it before reading. `maxSize` is an
     *      integer, default -1, and exceeding it throws a RangeError [20013]; gzip and
     *      zlib-format input are both accepted, raw deflate fails with `Z_DATA_ERROR`.
     *
     *      @param data the data to decompress
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function unzipToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

}

