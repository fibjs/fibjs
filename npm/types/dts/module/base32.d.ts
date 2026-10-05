/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description base32 encoding and decoding module
 *
 * The `base32` module is a module for base32 encoding and decoding. Base32 is an algorithm for encoding binary data as ASCII strings, used to transmit binary data in network protocols such as email and DNS.
 *
 * The module provides two methods: `encode` and `decode`. The `encode` method encodes binary data as a Base32 string, and the `decode` method decodes a Base32 string into binary data. Below is a usage example:
 *
 *  ```JavaScript
 *     const base32 = require('base32');
 *     const data = new Uint8Array([0x4e, 0x4f, 0x44, 0x45]); // 'NODE'
 *     const encoded = base32.encode(data); // 'KRUGKIDROV======'
 *     const decoded = base32.decode(encoded); // [0x4e, 0x4f, 0x44, 0x45]
 *     console.log(encoded, decoded); // KRUGKIDROV====== [78, 79, 68, 69]
 *  ```
 *  As can be seen, the `encode` method encoded the binary data as `KRUGKIDROV======`, and the `decode` method decoded it as `[0x4e, 0x4f, 0x44, 0x45]`.
 *
 *  Note that the resulting Base32 string is about 8/5 times the length of the original binary data, so it is not suitable for encoding large amounts of data. If large amounts of data need to be encoded, Base64 encoding is recommended.
 *
 */
declare module 'base32' {
    /**
     * @description Encodes data in base32 format
     *
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to encode
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string): string;

    /**
     * @description Decodes a string into binary data in base32 format
     * 	 @param data the string to decode
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string): Class_Buffer;

}

