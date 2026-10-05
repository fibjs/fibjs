/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description multibase is an encoding method that introduces multiple encoding prefixes on top of Base1x encoding
 *
 * multibase can encode the same data with different encodings and add a prefix indicating the encoding. multibase supports 15 encodings in total: base1, base2, base8, base10, base16, base32, base32hex, base32z, base36, base40, base56, base58flickr, base58btc, base64 and base64url. Among them, base16, base32 and base64 are the more commonly used.
 *
 * multibase can be used to change the presentation of binary data without changing the data itself. For example, encoding randomly generated binary data as a base32 string:
 * ```JavaScript
 * const {
 *     encode
 * } = require('multibase');
 *
 * const crypto = require('crypto');
 * const data = crypto.randomBytes(10); // generate 10 bytes random data
 * const encodedStr = encode(data, 'base32'); // encode data to base32 string
 * console.log(encodedStr); // ==> "bpgwnvztqmlbo5fy"
 * ```
 * Decoding the above string back to the original binary data:
 * ```JavaScript
 * const {
 *     decode
 * } = require('multibase');
 *
 * const data = decode('bpgwnvztqmlbo5fy', 'base32'); // decode base32 string to data
 * console.log(data); // ==> <Buffer a7 55 3d 33 ca 97 ac 0d aa 40>
 * ```
 * As can be seen, with multibase we encoded the original binary data as a base32 string, and we can decode this string back to the original binary data.
 *
 */
declare module 'multibase' {
    /**
     * @description Encodes data in multibase format; a string data is encoded as utf8
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to encode
     *      @param codec the encoding to use, a string is encoded as utf8
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string, codec: string): string;

    /**
     * @description Decodes a string into binary data in multibase format
     * 	 @param data the string to decode
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string): Class_Buffer;

}

