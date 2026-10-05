/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The hex module is a built-in module that provides a hexadecimal implementation for encoding and decoding between binary data and ASCII characters. The `hex` module provides encoding and decoding functions
 *
 * Using the encoding method, arbitrary binary data can be encoded as a hexadecimal string. For example:
 *
 * ```JavaScript
 * const hex = require('hex')
 *
 * const data = new Buffer([0x4e, 0x4f, 0x44, 0x45]) // [0x4e, 0x4f, 0x44, 0x45] => 'NODE'
 * const encodedData = hex.encode(data)
 * console.log(encodedData) // "4e4f4445"
 * ```
 *
 * Using the decoding method, a hexadecimal string can be decoded back to the original binary data. For example:
 *
 * ```JavaScript
 * const hex = require('hex')
 *
 * const encodedData = '4e4f4445' // 'NODE'
 * const decodedData = hex.decode(encodedData)
 * console.log(decodedData) // [0x4e, 0x4f, 0x44, 0x45]
 * ```
 *
 * As can be seen, `hex` is a very simple encoding and decoding module, suitable for simple conversion needs between binary data and strings.
 *
 */
declare module 'hex' {
    /**
     * @description Encodes data in hex format
     *
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to encode
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string): string;

    /**
     * @description Decodes a string into binary data in hex format
     * 	 @param data the string to decode
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string): Class_Buffer;

}

