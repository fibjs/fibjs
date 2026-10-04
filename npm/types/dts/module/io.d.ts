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
 *  - **Stream objects**: `MemoryStream`, `BufferedStream` and `RangeStream`;
 *  - **Data movement**: `copyStream` copies data into a target stream, `bridge` copies both ways.
 *
 *  Usage:
 *  ```JavaScript
 *  var io = require('io');
 *  ```
 *
 *  Example of copying a stream:
 *
 *  ```JavaScript
 *  var io = require('io');
 *
 *  var src = new io.MemoryStream();
 *  src.write(new Buffer('hello world'));
 *  src.rewind();
 *
 *  var dst = new io.MemoryStream();
 *  io.copyStream(src, dst);
 *  ```
 *
 */
declare module 'io' {
    /**
     * @description Creates a memory stream, see MemoryStream
     */
    const MemoryStream: typeof Class_MemoryStream;

    /**
     * @description Creates a buffered stream, see BufferedStream
     */
    const BufferedStream: typeof Class_BufferedStream;

    /**
     * @description Creates a range stream, see RangeStream
     */
    const RangeStream: typeof Class_RangeStream;

    /**
     * @description Copies the data of a stream into a target stream
     *
     *      bytes specifies the number of bytes to copy; -1 by default, which copies all the data of
     *      the source stream. The number of bytes actually copied is returned once the copy is done.
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
     *      bytes specifies the number of bytes to copy; -1 by default, which copies all the data of
     *      the source stream. The number of bytes actually copied is returned once the copy is done.
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
     *      bytes specifies the number of bytes to copy; -1 by default, which copies all the data of
     *      the source stream. The number of bytes actually copied is returned once the copy is done.
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
     *      stm1 and stm2 are the input and output of each other; the copy stops as a whole when
     *      either direction ends.
     *      @param stm1 the first stream
     *      @param stm2 the second stream
     *
     */
    function bridge(stm1: Class_Stream | Class_StreamPromise, stm2: Class_Stream | Class_StreamPromise): void;

    function bridge(stm1: Class_Stream | Class_StreamPromise, stm2: Class_Stream | Class_StreamPromise, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Copies data in both directions until no data is left or a stream is closed
     *
     *      stm1 and stm2 are the input and output of each other; the copy stops as a whole when
     *      either direction ends.
     *      @param stm1 the first stream
     *      @param stm2 the second stream
     *
     */
    function bridgeSync(stm1: Class_Stream | Class_StreamPromise, stm2: Class_Stream | Class_StreamPromise): void;

    /**
     * @description Copies data in both directions until no data is left or a stream is closed
     *
     *      stm1 and stm2 are the input and output of each other; the copy stops as a whole when
     *      either direction ends.
     *      @param stm1 the first stream
     *      @param stm2 the second stream
     *
     */
    function bridgeAsync(stm1: Class_Stream | Class_StreamPromise, stm2: Class_Stream | Class_StreamPromise): Promise<void>;

}

