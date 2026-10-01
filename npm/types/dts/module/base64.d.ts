/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description base64 encoding and decoding module
 *
 *  `base64` is a method of encoding binary data as ASCII strings so that it can be transmitted over the network. The `base64` module provides support for Base64 encoding and decoding.
 *
 *  With the `base64` module, a string can be encoded to Base64 format, and Base64 format can be decoded to a string. For example, encoding a string to Base64 format:
 *  ```JavaScript
 *  const { encode } = require('base64');
 *  const str = 'hello, world';
 *  const encodedStr = encode(str);
 *  console.log(encodedStr); // ==> "aGVsbG8sIHdvcmxk"
 *  ```
 *  Decoding a Base64-format string to a string:
 *  ```JavaScript
 *  const { decode } = require('base64');
 *  const encodedStr = 'aGVsbG8sIHdvcmxk';
 *  const str = decode(encodedStr);
 *  console.log(str); // ==> "hello, world"
 *  ```
 *  When handling data containing sensitive information, Base64 encoding does not provide security. Because Base64 encoding can easily be broken, other more secure methods should be used to handle such data.
 *
 */
declare module 'base64' {
    /**
     * @description Encodes data in base64 format
     * 	 @param data the data to encode
     * 	 @param url specifies whether to use url-safe character encoding
     * 	 @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer, url?: boolean): string;

    /**
     * @description Decodes a string into binary data in base64 format
     * 	 @param data the string to decode
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string): Class_Buffer;

}

