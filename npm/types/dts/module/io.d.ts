/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/MemoryStream.d.ts" />
/// <reference path="../interface/BufferedStream.d.ts" />
/// <reference path="../interface/RangeStream.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description The io module provides stream creation and data movement between streams
 *
 *  Main capabilities:
 *
 *  - **Stream objects**: `MemoryStream` (in-memory read/write), `BufferedStream`
 *    (buffered reader with text helpers) and `RangeStream` (a window over another
 *    stream);
 *  - **Data movement**: `copyStream` copies a bounded number of bytes in one
 *    direction, `bridge` copies both directions at once.
 *
 *  Concepts:
 *
 *  - **Position**: every stream has a current position used by both reads and
 *    writes; `SeekableStream` exposes it through tell/seek/rewind and a length
 *    through size(). Reads and writes advance the position, so rewind before
 *    reading the same bytes again.
 *  - **Buffered streams**: BufferedStream reads ahead in chunks from the stream
 *    it wraps, so the underlying stream has already advanced before the data is
 *    consumed; never read the underlying stream directly while a BufferedStream
 *    is using it.
 *  - **Back pressure**: `write` returns false when the stream write queue is
 *    full; wait for the `drain` event before writing more (see the Stream class).
 *    The io helpers do not need it: copyStream and bridge keep one chunk in
 *    flight per direction.
 *  - **Read forms**: `read(bytes)` returns up to bytes and null at the end;
 *    `readAll` loops until the stream ends. Every async member also works
 *    synchronously without a callback, with a trailing callback, and through the
 *    `io.promises` namespace.
 *
 *  Usage:
 *  ```JavaScript
 *  const io = require('io');
 *  ```
 *
 *  Example 1 — read and write an in-memory stream:
 *  ```JavaScript
 *  const io = require('io');
 *
 *  const stm = new io.MemoryStream();
 *  stm.write(Buffer.from('hello world'));
 *  stm.rewind();
 *  console.log(stm.readAll().toString()); // hello world
 *  console.log(stm.size()); // 11
 *  ```
 *
 *  Example 2 — copy part of one stream into another:
 *  ```JavaScript
 *  const io = require('io');
 *
 *  const from = new io.MemoryStream();
 *  from.write(Buffer.from('0123456789'));
 *  from.rewind();
 *
 *  const to = new io.MemoryStream();
 *  console.log(io.copyStream(from, to, 4)); // 4
 *
 *  to.rewind();
 *  console.log(to.readAll().toString()); // 0123
 *  ```
 *
 *  Example 3 — read text lines through a buffered stream:
 *  ```JavaScript
 *  const io = require('io');
 *
 *  const stm = new io.MemoryStream();
 *  stm.write(Buffer.from('one\ntwo\n'));
 *  stm.rewind();
 *
 *  const reader = new io.BufferedStream(stm);
 *  console.log(reader.readLines()); // ['one', 'two']
 *  ```
 *
 */
declare module 'io' {
    /**
     * @description Creates a memory stream, see MemoryStream
     *
     *      The MemoryStream constructor exposed as a module static, so
     *      `new io.MemoryStream()` builds the same class that `clone()` returns. The
     *      stream lives in memory, supports random access and never blocks; see
     *      MemoryStream for its members.
     *
     */
    const MemoryStream: typeof Class_MemoryStream;

    /**
     * @description Creates a buffered stream, see BufferedStream
     *
     *      The BufferedStream constructor exposed as a module static; it wraps
     *      another stream and adds buffered text reading on top of it. See
     *      BufferedStream for the text members and the buffering model.
     *
     */
    const BufferedStream: typeof Class_BufferedStream;

    /**
     * @description Creates a range stream, see RangeStream
     *
     *      The RangeStream constructor exposed as a module static; it restricts
     *      another stream to a byte range. See RangeStream for the range forms and
     *      the seekable/length-limited modes.
     *
     */
    const RangeStream: typeof Class_RangeStream;

    /**
     * @description Copies the data of a stream into a target stream
     *
     *      Bytes are moved from the current position of `from` to the current
     *      position of `to`, in chunks of up to 64 KiB, and both positions advance by
     *      the number of bytes copied. `bytes` limits the transfer: -1 (the default)
     *      copies until the source ends, 0 copies nothing and returns 0. The copy
     *      also stops when the source has no more data (read returns null) or when
     *      the target reports an error; it never closes either stream. The return
     *      value is the number of bytes actually written, and the call works
     *      synchronously, with a trailing callback and through `io.promises`.
     *
     *      Example — copy a four-byte prefix into another stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const from = new io.MemoryStream();
     *      from.write(Buffer.from('0123456789'));
     *      from.rewind();
     *
     *      const to = new io.MemoryStream();
     *      console.log(io.copyStream(from, to, 4)); // 4
     *
     *      to.rewind();
     *      console.log(to.readAll().toString()); // 0123
     *      ```
     *
     *      @param from the source stream
     *      @param to the target stream
     *      @param bytes the number of bytes to copy
     *      @return the number of bytes copied
     *
     */
    function copyStream(from: Class_Stream | Class_StreamPromise, to: Class_Stream | Class_StreamPromise, bytes?: number): number;

    function copyStream(from: Class_Stream | Class_StreamPromise, to: Class_Stream | Class_StreamPromise, bytes?: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Copies the data of a stream into a target stream
     *
     *      Bytes are moved from the current position of `from` to the current
     *      position of `to`, in chunks of up to 64 KiB, and both positions advance by
     *      the number of bytes copied. `bytes` limits the transfer: -1 (the default)
     *      copies until the source ends, 0 copies nothing and returns 0. The copy
     *      also stops when the source has no more data (read returns null) or when
     *      the target reports an error; it never closes either stream. The return
     *      value is the number of bytes actually written, and the call works
     *      synchronously, with a trailing callback and through `io.promises`.
     *
     *      Example — copy a four-byte prefix into another stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const from = new io.MemoryStream();
     *      from.write(Buffer.from('0123456789'));
     *      from.rewind();
     *
     *      const to = new io.MemoryStream();
     *      console.log(io.copyStream(from, to, 4)); // 4
     *
     *      to.rewind();
     *      console.log(to.readAll().toString()); // 0123
     *      ```
     *
     *      @param from the source stream
     *      @param to the target stream
     *      @param bytes the number of bytes to copy
     *      @return the number of bytes copied
     *
     */
    function copyStreamSync(from: Class_Stream | Class_StreamPromise, to: Class_Stream | Class_StreamPromise, bytes?: number): number;

    /**
     * @description Copies the data of a stream into a target stream
     *
     *      Bytes are moved from the current position of `from` to the current
     *      position of `to`, in chunks of up to 64 KiB, and both positions advance by
     *      the number of bytes copied. `bytes` limits the transfer: -1 (the default)
     *      copies until the source ends, 0 copies nothing and returns 0. The copy
     *      also stops when the source has no more data (read returns null) or when
     *      the target reports an error; it never closes either stream. The return
     *      value is the number of bytes actually written, and the call works
     *      synchronously, with a trailing callback and through `io.promises`.
     *
     *      Example — copy a four-byte prefix into another stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const from = new io.MemoryStream();
     *      from.write(Buffer.from('0123456789'));
     *      from.rewind();
     *
     *      const to = new io.MemoryStream();
     *      console.log(io.copyStream(from, to, 4)); // 4
     *
     *      to.rewind();
     *      console.log(to.readAll().toString()); // 0123
     *      ```
     *
     *      @param from the source stream
     *      @param to the target stream
     *      @param bytes the number of bytes to copy
     *      @return the number of bytes copied
     *
     */
    function copyStreamAsync(from: Class_Stream | Class_StreamPromise, to: Class_Stream | Class_StreamPromise, bytes?: number): Promise<number>;

    /**
     * @description Copies data in both directions until no data is left or a stream is closed
     *
     *      stm1 and stm2 are the input and output of each other: two copies run at
     *      the same time, stm1 to stm2 and stm2 to stm1, until one direction reaches
     *      the end of its input. That direction then closes the other stream, which
     *      ends the opposite copy as well; a write error or an externally closed
     *      stream stops the whole call in the same way. Use it to connect two
     *      endpoints of a connection pair (a client socket and its upstream, or two
     *      memory streams related by a protocol) so that each side sees the other's
     *      data.
     *
     *      Example — forward a client socket to an upstream socket (fragment; a
     *      bridge needs two live bidirectional streams):
     *      ```JavaScript
     *      // fragment: a bridge needs two live bidirectional streams
     *      const io = require('io');
     *
     *      // everything the client sends goes upstream, and vice versa
     *      io.bridge(clientSocket, upstreamSocket);
     *      ```
     *
     *      @param stm1 the first stream
     *      @param stm2 the second stream
     *
     */
    function bridge(stm1: Class_Stream | Class_StreamPromise, stm2: Class_Stream | Class_StreamPromise): void;

    function bridge(stm1: Class_Stream | Class_StreamPromise, stm2: Class_Stream | Class_StreamPromise, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Copies data in both directions until no data is left or a stream is closed
     *
     *      stm1 and stm2 are the input and output of each other: two copies run at
     *      the same time, stm1 to stm2 and stm2 to stm1, until one direction reaches
     *      the end of its input. That direction then closes the other stream, which
     *      ends the opposite copy as well; a write error or an externally closed
     *      stream stops the whole call in the same way. Use it to connect two
     *      endpoints of a connection pair (a client socket and its upstream, or two
     *      memory streams related by a protocol) so that each side sees the other's
     *      data.
     *
     *      Example — forward a client socket to an upstream socket (fragment; a
     *      bridge needs two live bidirectional streams):
     *      ```JavaScript
     *      // fragment: a bridge needs two live bidirectional streams
     *      const io = require('io');
     *
     *      // everything the client sends goes upstream, and vice versa
     *      io.bridge(clientSocket, upstreamSocket);
     *      ```
     *
     *      @param stm1 the first stream
     *      @param stm2 the second stream
     *
     */
    function bridgeSync(stm1: Class_Stream | Class_StreamPromise, stm2: Class_Stream | Class_StreamPromise): void;

    /**
     * @description Copies data in both directions until no data is left or a stream is closed
     *
     *      stm1 and stm2 are the input and output of each other: two copies run at
     *      the same time, stm1 to stm2 and stm2 to stm1, until one direction reaches
     *      the end of its input. That direction then closes the other stream, which
     *      ends the opposite copy as well; a write error or an externally closed
     *      stream stops the whole call in the same way. Use it to connect two
     *      endpoints of a connection pair (a client socket and its upstream, or two
     *      memory streams related by a protocol) so that each side sees the other's
     *      data.
     *
     *      Example — forward a client socket to an upstream socket (fragment; a
     *      bridge needs two live bidirectional streams):
     *      ```JavaScript
     *      // fragment: a bridge needs two live bidirectional streams
     *      const io = require('io');
     *
     *      // everything the client sends goes upstream, and vice versa
     *      io.bridge(clientSocket, upstreamSocket);
     *      ```
     *
     *      @param stm1 the first stream
     *      @param stm2 the second stream
     *
     */
    function bridgeAsync(stm1: Class_Stream | Class_StreamPromise, stm2: Class_Stream | Class_StreamPromise): Promise<void>;

}

