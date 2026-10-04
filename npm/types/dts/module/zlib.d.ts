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
 * @description zlib is a built-in compression module that supports multiple compression formats and modes such as gzip, deflate and zlib
 *
 *  zlib mainly consists of the following 3 functions:
 *
 * - deflate: compresses data;
 * - inflate: decompresses data;
 * - gzip: the gzip compression format.
 *
 * Before using zlib, you need to choose one of the compression algorithms according to your needs. You can refer to the constants of zlib to select the corresponding compression algorithm. For example, we use the deflate compression algorithm to explain the module:
 *
 * ```JavaScript
 * const zlib = require('zlib');
 * const { NO_COMPRESSION, BEST_SPEED, BEST_COMPRESSION, DEFAULT_COMPRESSION } = require('zlib');
 *
 * // compress data
 * const deflated = zlib.deflate('hello, world', BEST_SPEED);
 * console.log(deflated.toString());
 *
 * // decompress data
 * const inflated = zlib.inflate(deflated);
 * console.log(inflated.toString());
 * ```
 *
 * The above code shows how to compress and decompress data: first use the `zlib.deflate` method to compress the string `hello, world` and pass `BEST_SPEED` as the compression level option, then use the `zlib.inflate` method to decompress the data; the output should be the same as the original string.
 *
 * Both `zlib.deflate` and `zlib.inflate` support defining the compression level. The compression level is a number in the range `[NO_COMPRESSION, BEST_SPEED, DEFAULT_COMPRESSION, BEST_COMPRESSION]`, with a default value of `DEFAULT_COMPRESSION`. For the meaning of these 4 compression levels, see the following table:
 *
 * | Compression Level | Meaning                                                                      |
 * | ----------------- | ---------------------------------------------------------------------------- |
 * | zlib.NO_COMPRESSION | Does not compress the data (a complete compression header is included)                                    |
 * | zlib.BEST_SPEED     | The fastest compression speed, but the compression ratio is correspondingly worse                           |
 * | zlib.DEFAULT_COMPRESSION | The default value of the compression algorithm; usually slower than BEST_SPEED but with a higher compression ratio |
 * | zlib.BEST_COMPRESSION   | The best compression ratio, but the compression speed is correspondingly slower.                                   |
 *
 * Note when using the `zlib` module: if you want to compress and decompress data at the same time, it is recommended to use `deflate` to compress the data and then use `inflate` to decompress it, to avoid errors. For different compression formats and algorithms, there are other classes and methods for compression and decompression; see the following documents for their usage.
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
     * ! The constant object of the zlib module, see zlib_constants
     */
    const constants: typeof import ('zlib_constants');

    /**
     * ! Gzip compression class
     */
    const Gzip: typeof Class_Gzip;

    /**
     * ! Gunzip decompression class
     */
    const Gunzip: typeof Class_Gunzip;

    /**
     * ! Deflate compression class
     */
    const Deflate: typeof Class_Deflate;

    /**
     * ! Inflate decompression class
     */
    const Inflate: typeof Class_Inflate;

    /**
     * ! DeflateRaw compression class
     */
    const DeflateRaw: typeof Class_DeflateRaw;

    /**
     * ! InflateRaw decompression class
     */
    const InflateRaw: typeof Class_InflateRaw;

    /**
     * ! Unzip auto-detect decompression class
     */
    const Unzip: typeof Class_Unzip;

    /**
     * @description Creates a deflate stream object
     *      @param to the stream used to store the processing result
     *      @return returns the wrapped stream object
     */
    function createDeflate(to: Class_Stream | Class_StreamPromise): Class_Stream;

    /**
     * @description Creates a deflateRaw stream object
     *      @param to the stream used to store the processing result
     *      @return returns the wrapped stream object
     */
    function createDeflateRaw(to: Class_Stream | Class_StreamPromise): Class_Stream;

    /**
     * @description Creates a gunzip stream object
     *      @param to the stream used to store the processing result
     *      @param maxSize the decompression size limit, default -1, no limit
     *      @return returns the wrapped stream object
     */
    function createGunzip(to: Class_Stream | Class_StreamPromise, maxSize?: number): Class_Stream;

    /**
     * @description Creates a gzip stream object
     *      @param to the stream used to store the processing result
     *      @return returns the wrapped stream object
     */
    function createGzip(to: Class_Stream | Class_StreamPromise): Class_Stream;

    /**
     * @description Creates an inflate stream object
     *      @param to the stream used to store the processing result
     *      @param maxSize the decompression size limit, default -1, no limit
     *      @return returns the wrapped stream object
     */
    function createInflate(to: Class_Stream | Class_StreamPromise, maxSize?: number): Class_Stream;

    /**
     * @description Creates an inflateRaw stream object
     *      @param to the stream used to store the processing result
     *      @param maxSize the decompression size limit, default -1, no limit
     *      @return returns the wrapped stream object
     */
    function createInflateRaw(to: Class_Stream | Class_StreamPromise, maxSize?: number): Class_Stream;

    /**
     * @description Compresses data with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function deflate(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    function deflate(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Compresses data with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function deflateSync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Compresses data with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function deflateAsync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    function deflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

    /**
     * @description Decompresses data compressed with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function inflate(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    function inflate(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function inflateSync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Decompresses data compressed with the deflate algorithm (zlib format); a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function inflateAsync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object (zlib format); a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    function inflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object (zlib format); a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object (zlib format); a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

    /**
     * @description Compresses data with the gzip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param options the compression options, read as { level }
     *      @return returns the compressed binary data
     *
     */
    function gzip(data: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Buffer;

    function gzip(data: Class_Buffer | string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Compresses data with the gzip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param options the compression options, read as { level }
     *      @return returns the compressed binary data
     *
     */
    function gzipSync(data: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Compresses data with the gzip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param options the compression options, read as { level }
     *      @return returns the compressed binary data
     *
     */
    function gzipAsync(data: Class_Buffer | string, options?: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Compresses data into a stream object with the gzip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *
     */
    function gzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise): void;

    function gzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Compresses data into a stream object with the gzip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *
     */
    function gzipToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise): void;

    /**
     * @description Compresses data into a stream object with the gzip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *
     */
    function gzipToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise): Promise<void>;

    /**
     * @description Decompresses gzip data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function gunzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    function gunzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Decompresses gzip data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function gunzipSync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Decompresses gzip data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function gunzipAsync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decompresses data compressed with the gzip algorithm into a stream object; a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function gunzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    function gunzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses data compressed with the gzip algorithm into a stream object; a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function gunzipToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    /**
     * @description Decompresses data compressed with the gzip algorithm into a stream object; a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function gunzipToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

    /**
     * @description Compresses data with the deflateRaw algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function deflateRaw(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    function deflateRaw(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Compresses data with the deflateRaw algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function deflateRawSync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Compresses data with the deflateRaw algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function deflateRawAsync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (deflateRaw); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    function deflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (deflateRaw); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateRawToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    /**
     * @description Compresses data into a stream object with the deflate algorithm (deflateRaw); a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function deflateRawToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

    /**
     * @description Decompresses deflateRaw data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function inflateRaw(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    function inflateRaw(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Decompresses deflateRaw data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function inflateRawSync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Decompresses deflateRaw data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function inflateRawAsync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object (inflateRaw); a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    function inflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object (inflateRaw); a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateRawToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    /**
     * @description Decompresses data compressed with the deflate algorithm into a stream object (inflateRaw); a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function inflateRawToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

    /**
     * @description Creates a zip stream object
     *      @param to the stream used to store the processing result
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *      @return returns the wrapped stream object
     */
    function createZip(to: Class_Stream | Class_StreamPromise, level?: number): Class_Stream;

    /**
     * @description Creates a unzip stream object
     *      @param to the stream used to store the processing result
     *      @param maxSize the decompression size limit, default -1, no limit
     *      @return returns the wrapped stream object
     */
    function createUnzip(to: Class_Stream | Class_StreamPromise, maxSize?: number): Class_Stream;

    /**
     * @description Compresses data with the zip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function zip(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    function zip(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Compresses data with the zip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function zipSync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Compresses data with the zip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param level the compression level, default DEFAULT_COMPRESSION; an object is read as the options, supporting: { level }
     *      @return returns the compressed binary data
     *
     */
    function zipAsync(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Compresses data into a stream object with the zip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function zipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    function zipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Compresses data into a stream object with the zip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function zipToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): void;

    /**
     * @description Compresses data into a stream object with the zip algorithm; a string data is encoded as utf8
     *      @param data the data to compress, a string is encoded as utf8
     *      @param stm the stream that stores the compressed data
     *      @param level the compression level, default DEFAULT_COMPRESSION
     *
     */
    function zipToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

    /**
     * @description Decompresses zip data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function unzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    function unzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Decompresses zip data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function unzipSync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Decompresses zip data; a string data is encoded as utf8
     *      @param data the compressed data, a string is encoded as utf8
     *      @param maxSize the decompression size limit, default -1, no limit; an object is read as the options, supporting: { maxOutputLength }
     *      @return returns the decompressed binary data
     *
     */
    function unzipAsync(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decompresses data compressed with the zip algorithm into a stream object; a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function unzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    function unzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses data compressed with the zip algorithm into a stream object; a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function unzipToSync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): void;

    /**
     * @description Decompresses data compressed with the zip algorithm into a stream object; a string data is encoded as utf8
     *      @param data the data to decompress, a string is encoded as utf8
     *      @param stm the stream that stores the decompressed data
     *      @param maxSize the decompression size limit, default -1, no limit
     *
     */
    function unzipToAsync(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

}

