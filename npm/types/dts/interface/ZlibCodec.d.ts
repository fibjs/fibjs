/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description ZlibCodec is the base class of zlib compression and decompression codecs, providing the constructors of zlib-like codecs
 *
 *  ZlibCodec inherits from EventEmitter and can be used by npm packages such as minizlib. Subclasses include Gzip, Gunzip, Deflate, Inflate, DeflateRaw, InflateRaw and Unzip.
 *
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const gzip = new zlib.Gzip({});
 *  const result = gzip._processChunk(Buffer.from('hello'), zlib.constants.Z_FINISH);
 *  ```
 *
 */
declare class Class_ZlibCodec extends Class_EventEmitter {
    /**
     * @description Processes a chunk of data synchronously; a string chunk is encoded as utf8
     *      @param chunk the data to process, a string is encoded as utf8
     *      @param flushFlag flush flag, see zlib.constants.Z_NO_FLUSH and others
     *      @return returns the processed data
     *
     */
    _processChunk(chunk: Class_Buffer | string, flushFlag: number): Class_Buffer;

    /**
     * @description Closes the codec and releases resources
     */
    close(): void;

    /**
     * @description Resets the codec state
     */
    reset(): void;

    /**
     * @description The underlying handle object, used for internal compatibility
     */
    _handle: any;

    /**
     * @description Dynamically updates compression parameters
     *      @param level compression level
     *      @param strategy compression strategy
     *
     */
    params(level: number, strategy: number): void;

}

