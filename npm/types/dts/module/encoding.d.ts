/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/base32.d.ts" />
/// <reference path="../module/base64.d.ts" />
/// <reference path="../module/base58.d.ts" />
/// <reference path="../module/hex.d.ts" />
/// <reference path="../module/multibase.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../module/json.d.ts" />
/// <reference path="../module/msgpack.d.ts" />
/**
 * @description The `encoding` module is a built-in FibJS module used to convert between various data encoding formats and binary. These data encoding formats include some commonly used ones such as `base64`, `base32`, `hex`, `json`, `msgpack`, `multibase` and `base58`.
 *
 * Below is a brief introduction to each submodule provided by the `encoding` module:
 *
 * - `base64`: provides Base64 encoding and decoding support; it can encode a string as Base64 and decode a Base64-encoded string back to a string.
 * - `base32`: provides Base32 encoding and decoding support; it can encode the given data as Base32 and return the encoded string, and can decode a Base32-encoded string back to the original data.
 * - `hex`: provides hexadecimal encoding and decoding support; it can encode the given data as hexadecimal and return the encoded string, and can decode a hexadecimal-encoded string back to the original data.
 * - `json`: provides JSON encoding and decoding support; it can serialize JavaScript objects into JSON strings and deserialize JSON strings back into JavaScript objects.
 * - `multibase`: provides Multibase encoding support. Multibase is an encoding scheme that introduces multiple encoding prefixes on top of Base1x encodings.
 * - `msgpack`: provides Msgpack encoding and decoding support. Msgpack is a lighter-weight data interchange format than JSON; it can serialize JSON objects into binary data for faster and more efficient data interchange.
 * - `base58`: provides Base58 encoding and decoding support. Base58 is a representation combining digits and letters that excludes easily confused characters such as the digit 0 and the letters O, I and L, making it less error-prone.
 *
 * Most submodules of the `encoding` module contain two functions, one for encoding and one for decoding; these functions can encode or decode data in a specific format. When using these encoding/decoding modules, choose the most suitable module according to its type and characteristics to ensure correct encoding and decoding.
 *
 *  How to reference the `encoding` module:
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  ```
 *
 *
 *
 *
 */
declare module 'encoding' {
    /**
     * @description base32 encoding and decoding module
     */
    const base32: typeof import ('base32');

    /**
     * @description base64 encoding and decoding module
     */
    const base64: typeof import ('base64');

    /**
     * @description base58 encoding and decoding module
     */
    const base58: typeof import ('base58');

    /**
     * @description hex encoding and decoding module
     */
    const hex: typeof import ('hex');

    /**
     * @description multibase encoding and decoding module
     */
    const multibase: typeof import ('multibase');

    /**
     * @description Determines whether the specified encoding is supported
     *      @param codec the encoding format, allowed values: "hex", "base32", "base58", "base64", "utf8", or any charset supported by ICU
     *      @return returns whether the encoding is supported
     *
     */
    function isEncoding(codec: string): boolean;

    /**
     * @description Encodes the Buffer as a string; a string data is encoded as utf8
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the initial string, written in utf-8 format
     *      @param codec the encoding format, allowed values: "hex", "base32", "base58", "base64", "utf8", or any charset supported by ICU, default is "utf8", a string is encoded as utf8
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string, codec?: string): string;

    /**
     * @description Decodes the string as a Buffer
     *      @param str the initial string, written in utf-8 format
     *      @param codec the encoding format, allowed values: "hex", "base32", "base58", "base64", "utf8", or any charset supported by ICU, default is "utf8"
     *      @return returns the decoded Buffer
     *
     */
    function decode(str: string, codec?: string): Class_Buffer;

    /**
     * @description json encoding and decoding module
     */
    const json: typeof import ('json');

    /**
     * @description msgpack encoding and decoding module
     */
    const msgpack: typeof import ('msgpack');

    /**
     * @description Encodes a string as a javascript escaped string, for embedding text in javascript code
     *      @param str the string to encode
     *      @param json whether to generate a json-compatible string
     *      @return returns the encoded string
     *
     */
    function jsstr(str: string, json?: boolean): string;

    /**
     * @description url string safe encoding
     *      @param url the url to encode
     *      @return returns the encoded string
     *
     */
    function encodeURI(url: string): string;

    /**
     * @description url component string safe encoding
     *      @param url the url to encode
     *      @param formEncoded whether to encode in application/x-www-form-urlencoded format (spaces are encoded as +), default is false
     *      @return returns the encoded string
     *
     */
    function encodeURIComponent(url: string, formEncoded?: boolean): string;

    /**
     * @description url safe string decoding
     *      @param url the url to decode
     *      @return returns the decoded string
     *
     */
    function decodeURI(url: string): string;

}

