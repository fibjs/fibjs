/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The base64 module encodes binary data with the RFC 4648 alphabet, URL-safe or not
 *
 *  Main capabilities:
 *
 *  - **Encoding**: `encode` turns a Buffer or a string into base64 text, standard or
 *    URL-safe;
 *  - **Decoding**: `decode` turns base64 text back into a Buffer.
 *
 *  Concepts:
 *
 *  - **Alphabet and padding**: standard base64 maps six bits to one character of
 *    A-Z a-z 0-9 + / and pads the last group with "=" so that the text length is a multiple
 *    of 4. With url = true the encoder switches to - and _ and omits the padding, which keeps
 *    the text usable in URLs, query strings and file names without escaping.
 *  - **Decoding both forms**: `decode` accepts "+" and "-" as the same character and "/" and
 *    "_" as the same character; padding is optional and characters outside the alphabet are
 *    skipped instead of rejected, so the standard and URL-safe forms decode with one call and
 *    a string with stray whitespace still decodes.
 *  - **Relation to Buffer**: `base64.encode(data)` equals data.toString('base64') and
 *    `decode` accepts everything Buffer.from(str, 'base64url') accepts; the module is the
 *    functional form for code that does not hold a Buffer method call.
 *  - **Relation to multibase**: the base64, base64pad, base64url and base64urlpad codecs of
 *    the multibase module are these payloads behind one prefix character.
 *  - **Not encryption**: base64 is reversible without a key, so it hides nothing and must
 *    not be used to protect data.
 *
 *  Import:
 *  ```JavaScript
 *  const base64 = require('base64');
 *  ```
 *
 *  Example 1 — encode and decode standard base64 text:
 *  ```JavaScript
 *  const base64 = require('base64');
 *
 *  const data = Buffer.from('hello, world');
 *  console.log(base64.encode(data)); // aGVsbG8sIHdvcmxk
 *  console.log(base64.decode('aGVsbG8sIHdvcmxk').toString()); // hello, world
 *  ```
 *
 *  Example 2 — the URL-safe variant of two bytes that need "+" and "/" in standard form:
 *  ```JavaScript
 *  const base64 = require('base64');
 *
 *  const data = Buffer.from([0xfb, 0xff]);
 *  console.log(base64.encode(data)); // +/8=
 *  console.log(base64.encode(data, true)); // -_8
 *  console.log(base64.decode('-_8').toString('hex')); // fbff
 *  ```
 *
 *  Notes:
 *
 *  - `decode` never reports malformed input: an empty or fully invalid string returns an
 *    empty Buffer; use a stricter parser when the text must be validated.
 *  - Node.js exposes base64 through Buffer encodings rather than a module; fibjs also
 *    accepts the alias `encoding.encode(data, 'base64url')` for the URL-safe form.
 *
 */
declare module 'base64' {
    /**
     * @description Encodes data in base64 format
     *
     *      data may be a Buffer or a string; a string is encoded as utf8. Without url the
     *      standard alphabet (A-Z a-z 0-9 + /) is used and the result is padded with "=" to a
     *      multiple of four; with url the URL-safe alphabet (- and _) is used and no padding is
     *      added.
     *
     *      Example — the same two bytes in both forms:
     *      ```JavaScript
     *      const base64 = require('base64');
     *
     *      const data = Buffer.from([0xfb, 0xff]);
     *      console.log(base64.encode(data)); // +/8=
     *      console.log(base64.encode(data, true)); // -_8
     *      ```
     *      @param data the data to encode
     *      @param url specifies whether to use url-safe character encoding
     *      @return returns the encoded string
     *
     */
    function encode(data: Class_Buffer | string, url?: boolean): string;

    /**
     * @description Decodes a string into binary data in base64 format
     *
     *      The padding of the last group is optional and both alphabets are accepted in the same
     *      call: "+" and "-" decode alike, as do "/" and "_". Characters outside the alphabet,
     *      including whitespace, are ignored, so an empty or fully invalid string returns an
     *      empty Buffer instead of reporting the malformed input.
     *
     *      Example — padded, unpadded and URL-safe text:
     *      ```JavaScript
     *      const base64 = require('base64');
     *
     *      console.log(base64.decode('aGVsbG8=').toString()); // hello
     *      console.log(base64.decode('aGVsbG8').toString()); // hello
     *      console.log(base64.decode('-_8').toString('hex')); // fbff
     *      ```
     * 	 @param data the string to decode
     * 	 @return returns the decoded binary data
     *
     */
    function decode(data: string): Class_Buffer;

}

