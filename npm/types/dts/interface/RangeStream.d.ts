/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description Range query stream reading object
 *
 *  The RangeStream object is used to cut data from a SeekableStream object. Creation method:
 *  ```JavaScript
 *  var stm = new io.RangeStream(stream, '0-10');
 *  stm.end // 11
 *
 *  var stm = new io.RangeStream(stream, 0, 10);
 *  stm.end // 10
 *  ```
 *
 *  A plain Stream object can also be read with a length limit; in this case begin is fixed to 0 and only the number of bytes read is limited:
 *  ```JavaScript
 *  var stm = new io.RangeStream(stream, 1024);
 *  stm.begin // 0
 *  stm.end   // 1024
 *  ```
 *  If the passed stm is a SeekableStream, it is equivalent to RangeStream(stm, 0, end).
 *
 */
declare class Class_RangeStream extends Class_SeekableStream {
    /**
     * @description RangeStream constructor
     *       @param stm the binary underlying stream object of the RangeStream, must be a SeekableStream
     *       @param range the string describing the range, in the format 'begin-[end]' or '[begin]-end'
     *
     */
    constructor(stm: Class_SeekableStream | Class_SeekableStreamPromise, range: string);

    /**
     * @description RangeStream constructor
     *       @param stm the binary underlying stream object of the RangeStream, must be a SeekableStream
     *       @param begin the start position of the content read from stm
     *       @param end the end position of the content read from stm
     *
     */
    constructor(stm: Class_SeekableStream | Class_SeekableStreamPromise, begin: number, end: number);

    /**
     * @description RangeStream constructor, used for length-limited reading of a plain Stream
     *       @param stm the underlying stream object; if it is a SeekableStream, this is equivalent to RangeStream(stm, 0, end)
     *       @param end the maximum number of bytes read from stm
     *
     */
    constructor(stm: Class_Stream | Class_StreamPromise, end: number);

    /**
     * @description Queries the range begin value
     */
    readonly begin: number;

    /**
     * @description Queries the range end value
     */
    readonly end: number;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * The promise variant of the RangeStream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_RangeStreamPromise extends Class_SeekableStreamPromise {
    /**
     * @description RangeStream constructor
     *       @param stm the binary underlying stream object of the RangeStream, must be a SeekableStream
     *       @param range the string describing the range, in the format 'begin-[end]' or '[begin]-end'
     *
     */
    constructor(stm: Class_SeekableStream | Class_SeekableStreamPromise, range: string);

    /**
     * @description RangeStream constructor
     *       @param stm the binary underlying stream object of the RangeStream, must be a SeekableStream
     *       @param begin the start position of the content read from stm
     *       @param end the end position of the content read from stm
     *
     */
    constructor(stm: Class_SeekableStream | Class_SeekableStreamPromise, begin: number, end: number);

    /**
     * @description RangeStream constructor, used for length-limited reading of a plain Stream
     *       @param stm the underlying stream object; if it is a SeekableStream, this is equivalent to RangeStream(stm, 0, end)
     *       @param end the maximum number of bytes read from stm
     *
     */
    constructor(stm: Class_Stream | Class_StreamPromise, end: number);

    /**
     * @description Queries the range begin value
     */
    readonly begin: number;

    /**
     * @description Queries the range end value
     */
    readonly end: number;

}


declare namespace Class_RangeStream {
    const promises: FIBJS.GeneralObject;
}
