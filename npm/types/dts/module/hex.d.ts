/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The hex module converts binary data to and from hexadecimal text
 *
 *  Main capabilities:
 *
 *  - **Encoding**: `encode` turns a Buffer or a string into lower-case hexadecimal text;
 *  - **Decoding**: `decode` turns hexadecimal text back into a Buffer.
 *
 *  Concepts:
 *
 *  - **Two characters per byte**: each byte becomes exactly two characters of 0-9a-f, so the
 *    text is twice as long as the input and the mapping is one-to-one. `decode` accepts upper
 *    case as well.
 *  - **Lenient decoding**: every character outside 0-9a-f A-F is skipped wherever it
 *    appears, and a trailing single nibble is dropped. The call does not report malformed
 *    input for a string argument and can even return an empty Buffer. Buffer.from(str,
 *    'hex') accepts both cases too, but stops at the first invalid character instead of
 *    skipping it, which is the behaviour to use when the text must be validated.
 *  - **Relation to Buffer and multibase**: `hex.encode(data)` equals data.toString('hex');
 *    the f and F codecs of the multibase module are this payload behind one prefix
 *    character, and `encoding.encode(data, 'hex')` calls this module.
 *
 *  Import:
 *  ```JavaScript
 *  const hex = require('hex');
 *  ```
 *
 *  Example 1 — encode and decode four bytes:
 *  ```JavaScript
 *  const hex = require('hex');
 *
 *  const data = Buffer.from('NODE');
 *  const encoded = hex.encode(data);
 *  console.log(encoded); // 4e4f4445
 *  console.log(hex.decode(encoded).toString()); // NODE
 *  ```
 *
 *  Example 2 — decoding is case-insensitive and skips separators:
 *  ```JavaScript
 *  const hex = require('hex');
 *
 *  console.log(hex.decode('DEADBEEF').toString('hex')); // deadbeef
 *  console.log(hex.decode('4e 4f-44!45').toString()); // NODE
 *  console.log(hex.decode('4e4f4').toString('hex')); // 4e4f, the odd nibble is dropped
 *  ```
 *
 */
declare module 'hex' {
    /**
     * @description Encodes data in hex format
     *
     *      data may be a Buffer or a string; a string is encoded as utf8. Each byte is written as
     *      two characters of 0-9a-f, so the result is always lower case and exactly twice as long
     *      as the input; this matches Buffer.toString('hex').
     *
     *      Example — one byte and one short text:
     *      ```JavaScript
     *      const hex = require('hex');
     *
     *      console.log(hex.encode(Buffer.from([0xde, 0xad]))); // dead
     *      console.log(hex.encode('NODE')); // 4e4f4445
     *      ```
     *      @param data the data to encode
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string): string;

    /**
     * @description Decodes a string into binary data in hex format
     *
     *      Upper- and lower-case digits are both accepted. Characters outside 0-9a-f A-F are
     *      skipped, wherever they appear, and a single trailing nibble is dropped, so the call
     *      returns the bytes it can spell instead of reporting malformed input; an empty or fully
     *      invalid string returns an empty Buffer.
     *
     *      Example — mixed separators and a trailing nibble:
     *      ```JavaScript
     *      const hex = require('hex');
     *
     *      console.log(hex.decode('4E4F4445').toString()); // NODE
     *      console.log(hex.decode('4e 4f-44!45').toString()); // NODE
     *      console.log(hex.decode('4e4f4').toString('hex')); // 4e4f
     *      ```
     * 	 @param data the string to decode
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string): Class_Buffer;

}

