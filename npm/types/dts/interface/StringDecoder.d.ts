/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Stream decoding object
 *
 */
declare class Class_StringDecoder extends Class_object {
    /**
     * @description Decoder constructor
     *      @param encoding the decoding encoding. Default 'utf8'.
     *
     */
    constructor(encoding?: string);

    /**
     * @description Returns the internally retained buffer as characters. Incomplete UTF-8 and UTF-16 bytes are completed when possible
     *      buf may be a Buffer or a string to decode first; when it is omitted, only the internally retained bytes are returned.
     *      @param buf the data to decode first, optional
     *      @return the decoded string.
     *
     */
    end(buf?: Class_Buffer | string): string;

    /**
     * @description Returns a decoded string, ensuring any incomplete trailing characters are omitted from this return and stored internally for the next write or end call
     *
     *      buf may be a Buffer, or a string encoded as utf8.
     *      @param buf the data to decode
     *      @return the decoded string.
     *
     */
    write(buf: Class_Buffer | string): string;

    /**
     * @description Internal use.
     *
     *      buf may be a Buffer, or a string encoded as utf8.
     *      @param buf the data to decode
     *      @param offset the decoding offset
     *      @return the decoded string.
     *
     */
    text(buf: Class_Buffer | string, offset: number): string;

    /**
     * @description Internal use.
     *      buf may be a Buffer, or a string encoded as utf8.
     *      @param buf the bytes to decode
     *      @return the decoded string.
     *
     */
    fillLast(buf: Class_Buffer | string): string;

    /**
     * @description Internal use.
     */
    lastNeed: number;

    /**
     * @description Internal use.
     */
    lastTotal: number;

    /**
     * @description Internal use.
     */
    lastChar: Class_Buffer;

    /**
     * @description Decoding encoding. Internal use.
     */
    encoding: string;

}

