/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description base58 encoding and decoding module
 *
 *  The `base58` module is a module for Base58 encoding and decoding of data. Base58 is a representation combining digits and letters that excludes easily confused characters such as the digit 0, the letters O and I, and the letter l, giving it the characteristic of being less error-prone.
 *
 *  The module provides two methods, `encode` and `decode`. The `encode` method Base58-encodes the given data and returns the encoded string. The `decode` method decodes the given Base58-encoded string and returns the decoded binary data.
 *
 *  Below is example code for the `base58` module:
 *  ```JavaScript
 *  var base58 = require('base58');
 *
 *  var data = "Hello, World!";
 *  var encoded = base58.encode(data);
 *  console.log(encoded); // => 'StV1DL6CwTryKyV'
 *
 *  var decoded = base58.decode(encoded);
 *  console.log(decoded.toString()); // => 'hello world'
 *  ```
 *
 */
declare module 'base58' {
    /**
     * @description Encodes data in base58 format
     *
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to encode
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string): string;

    /**
     * @description Encodes data in base58check format
     *
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to encode
     *      @param chk_ver the check version to use
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string, chk_ver: number): string;

    /**
     * @description Decodes a string into binary data in base58 format
     * 	 @param data the string to decode
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string): Class_Buffer;

    /**
     * @description Decodes a string into binary data in base58check format
     * 	 @param data the string to decode
     * 	 @param chk_ver the check version to use
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string, chk_ver: number): Class_Buffer;

}

