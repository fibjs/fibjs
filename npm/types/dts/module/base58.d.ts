/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The base58 module encodes binary data with the Bitcoin base58 alphabet
 *
 *  Main capabilities:
 *
 *  - **Encoding**: `encode` turns a Buffer or a string into base58 text; the two-argument
 *    form produces a base58check value that carries a version byte and a checksum;
 *  - **Decoding**: `decode` returns the bytes of a base58 text, and the two-argument form
 *    verifies and strips a base58check value.
 *
 *  Concepts:
 *
 *  - **Alphabet**: 123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz. The digit 0,
 *    the letters O, I and l and all punctuation are absent, so a transcription error is less
 *    likely.
 *  - **Leading zeros**: there is no padding character; every leading zero byte of the input
 *    becomes one leading "1", so the text length depends on the value. An empty input encodes
 *    to the empty string, and `encode(Buffer.from([0, 0, 1]))` is "112".
 *  - **base58check**: encode(data, chk_ver) prefixes the payload with the one-byte version
 *    chk_ver and appends four checksum bytes derived from a double SHA-256 over the version
 *    byte and the payload. decode(data, chk_ver) recomputes the checksum, requires the stored
 *    version byte to equal chk_ver and returns the payload without the version byte and the
 *    checksum; this is the format used by Bitcoin addresses.
 *  - **Strict decoding**: unlike base32 and base64, `decode` rejects characters outside the
 *    alphabet instead of skipping them. The one-argument form reports `base58: encode error.`
 *    (the message is fibjs' own) for a bad character; the base58check form reports
 *    `base58: decode error.` for the same case and `base58: check error.` when the checksum
 *    or the version byte does not match.
 *  - **Relation to multibase**: the base58btc codec of the multibase module is the letter "z"
 *    followed by this payload.
 *
 *  Import:
 *  ```JavaScript
 *  const base58 = require('base58');
 *  ```
 *
 *  Example 1 — round-trip a text:
 *  ```JavaScript
 *  const base58 = require('base58');
 *
 *  const data = Buffer.from('Hello, World!');
 *  const encoded = base58.encode(data);
 *  console.log(encoded); // 72k1xXWG59fYdzSNoA
 *  console.log(base58.decode(encoded).toString()); // Hello, World!
 *  ```
 *
 *  Example 2 — a base58check value with version byte 0:
 *  ```JavaScript
 *  const base58 = require('base58');
 *
 *  const address = base58.encode(Buffer.from('hello'), 0);
 *  console.log(address); // 12L5B5yqsf7vwb
 *  console.log(base58.decode(address, 0).toString()); // hello
 *
 *  try {
 *      base58.decode(address, 1); // the version byte does not match
 *  } catch (e) {
 *      console.log(e.message); // base58: check error.
 *  }
 *  ```
 *
 *  Notes:
 *
 *  - Base58 is case-sensitive and has no fixed length; do not compare encoded values by
 *    length or pad them by hand.
 *  - Node.js has no base58 in its standard library; in fibjs the bare payload is also
 *    reachable through `encoding.encode(data, 'base58')`.
 *
 */
declare module 'base58' {
    /**
     * @description Encodes data in base58 format
     *
     *      data may be a Buffer or a string; a string is encoded as utf8. Each leading zero byte
     *      of the input produces one leading "1"; the other bytes yield as many characters as
     *      their value needs, so the output length is not a fixed multiple of the input length.
     *      An empty input returns the empty string. This form performs no checksumming and no
     *      version marking; use the chk_ver overload for base58check.
     *
     *      Example — leading zero bytes survive the round-trip:
     *      ```JavaScript
     *      const base58 = require('base58');
     *
     *      console.log(base58.encode(Buffer.from([0, 0, 1]))); // 112
     *      console.log(base58.decode('112').toString('hex')); // 000001
     *      ```
     *      @param data the data to encode
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string): string;

    /**
     * @description Encodes data in base58check format
     *
     *      data may be a Buffer or a string; a string is encoded as utf8. The result carries the
     *      version byte chk_ver and a four-byte double-SHA-256 checksum; only the low eight bits
     *      of chk_ver are stored, so pass a value in the range 0 to 255. The check form is longer
     *      than the bare form: 14 characters against 7 for "hello".
     *
     *      Example — the check form is verified and stripped by the two-argument decode:
     *      ```JavaScript
     *      const base58 = require('base58');
     *
     *      const address = base58.encode('hello', 0);
     *      console.log(address); // 12L5B5yqsf7vwb
     *      console.log(base58.decode(address, 0).toString()); // hello
     *      ```
     *      @param data the data to encode
     *      @param chk_ver the check version to use
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string, chk_ver: number): string;

    /**
     * @description Decodes a string into binary data in base58 format
     *
     *      The text is decoded with the base58 alphabet; spaces, line breaks and punctuation are
     *      not skipped, and any character outside the alphabet fails the call with
     *      `base58: encode error.`. An empty string returns an empty Buffer, and each leading "1"
     *      becomes one leading zero byte.
     *
     *      Example — a stray character fails the whole call:
     *      ```JavaScript
     *      const base58 = require('base58');
     *
     *      console.log(base58.decode('Cn8eVZg').toString()); // hello
     *      try {
     *          base58.decode('Cn8eVZg0'); // "0" is not in the alphabet
     *      } catch (e) {
     *          console.log(e.message); // base58: encode error.
     *      }
     *      ```
     * 	 @param data the string to decode
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string): Class_Buffer;

    /**
     * @description Decodes a string into binary data in base58check format
     *
     *      The checksum and the stored version byte are verified before the payload is returned:
     *      the version byte must equal chk_ver, which is compared as an integer, so pass a value
     *      in the range 0 to 255. The returned Buffer holds only the payload; the version byte
     *      and the four checksum bytes are removed. A bad character reports
     *      `base58: decode error.`; a checksum or version mismatch reports
     *      `base58: check error.`
     * 	 @param data the string to decode
     * 	 @param chk_ver the check version to use
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string, chk_ver: number): Class_Buffer;

}

