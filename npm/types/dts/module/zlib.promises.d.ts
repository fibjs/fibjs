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
 * The promise variant of the zlib module: async members return a Promise as their primary form.
 */
declare module 'zlib/promises' {
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
    function deflate(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

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
    function deflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

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
    function inflate(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

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
    function inflateTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

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
    function gzip(data: Class_Buffer | string, options?: FIBJS.GeneralObject): Promise<Class_Buffer>;

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
    function gzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise): Promise<void>;

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
    function gunzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

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
    function gunzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

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
    function deflateRaw(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

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
    function deflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

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
    function inflateRaw(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

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
    function inflateRawTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

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
    function zip(data: Class_Buffer | string, level?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

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
    function zipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, level?: number): Promise<void>;

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
    function unzip(data: Class_Buffer | string, maxSize?: number | FIBJS.GeneralObject): Promise<Class_Buffer>;

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
    function unzipTo(data: Class_Buffer | Class_Stream | Class_StreamPromise | string, stm: Class_Stream | Class_StreamPromise, maxSize?: number): Promise<void>;

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


declare module "zlib" {
    const promises: typeof import("zlib/promises");
}
