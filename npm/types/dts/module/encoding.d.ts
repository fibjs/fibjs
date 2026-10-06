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
 * @description The `encoding` module is fibjs's byte-and-text conversion toolbox: it converts between Buffer bytes and JavaScript strings in the representations used by files, network protocols and storage formats, going beyond the encodings that Buffer alone provides
 *
 *  Main capabilities:
 *
 *  - **Binary-to-text codecs**: `encode` and `decode` handle `hex`, `base32`, `base58`,
 *    `base64` and `base64url` in one call, and the same codecs are exported as the
 *    `base32`, `base58`, `base64`, `hex` and `multibase` submodules;
 *  - **Charset conversion**: any charset available from ICU can be named as a codec, such
 *    as `gbk`, `big5`, `shift_jis`, `euc-jp`, `koi8-r`, `iso-8859-*` and `windows-125*`;
 *  - **Unicode codecs**: `utf8`, `utf16le`/`ucs2`, `utf16be`, `utf32*`, `ascii` and
 *    `binary`/`latin1` turn raw bytes into text and back;
 *  - **Structured data**: the `json` and `msgpack` members reference the json and msgpack
 *    modules;
 *  - **Escaping**: `jsstr` escapes a string for embedding in JavaScript source, while
 *    `encodeURI`, `encodeURIComponent` and `decodeURI` percent-encode and decode URLs;
 *  - **Support queries**: `isEncoding` reports whether a codec label is usable, including
 *    whether the runtime ICU data provides a charset.
 *
 *  Concepts:
 *
 *  - **Two sides of a conversion**: a JavaScript string is a sequence of Unicode code
 *    points while a Buffer is a sequence of bytes. `encode` always turns bytes into text
 *    and `decode` always turns text into bytes; the codec describes how the bytes are
 *    represented, and both directions share the same codec names.
 *  - **Buffer codecs**: the names in the `utf8`, `hex`, `base32`, `base58`, `base64`,
 *    `base64url`, `ascii`, `binary`/`latin1` and `utf16*`/`utf32*` families are converted
 *    natively. `base64url` uses the URL-safe alphabet (`-` and `_` instead of `+` and
 *    `/`); `ascii` clears the high bit of every byte; `binary` and `latin1` map byte
 *    values to code points 0-255.
 *  - **Charset codecs**: any other label is resolved through the ICU converter library, so
 *    the exact list depends on the ICU data built into the binary. `isEncoding` is the
 *    runtime probe; encode/decode throw on an unknown label instead of returning a partial
 *    result.
 *  - **Relationship to Buffer**: `buf.toString(codec)` and `Buffer.from(str, codec)` are
 *    the Buffer-native form of the same conversions and are preferable inside buffer-heavy
 *    code; the encoding module adds the BASE32/BASE58 codecs, the module-object form and
 *    the explicit direction of `encode`/`decode`. The dedicated
 *    base32/base58/base64/hex/multibase modules are the same objects as the members here,
 *    so pick one form instead of converting between them.
 *  - **Whole-buffer versus streaming**: a codec applied to a whole Buffer cannot assemble
 *    a multibyte character that was split across chunks; when chunks arrive separately
 *    use the string_decoder module, which keeps the incomplete tail between calls.
 *
 *  Import:
 *  ```JavaScript
 *  const encoding = require('encoding');
 *  ```
 *
 *  Example 1 — encode bytes as text and decode text back to bytes:
 *  ```JavaScript
 *  const encoding = require('encoding');
 *
 *  const bytes = Buffer.from('abc', 'utf8');
 *
 *  console.log(encoding.encode(bytes, 'hex')); // 616263
 *  console.log(encoding.encode(bytes, 'base64')); // YWJj
 *  console.log(encoding.encode(bytes, 'base58')); // ZiCa
 *
 *  console.log(encoding.decode('616263', 'hex').toString()); // abc
 *  console.log(encoding.decode('YWJj', 'base64').toString()); // abc
 *  ```
 *
 *  Example 2 — convert text to a GBK byte stream and back:
 *  ```JavaScript
 *  const encoding = require('encoding');
 *
 *  // decode turns text into the bytes of the named charset
 *  const gbk = encoding.decode('你好', 'gbk');
 *  console.log(gbk.hex()); // c4e3bac3
 *
 *  // encode interprets the bytes of that charset and returns text
 *  console.log(encoding.encode(gbk, 'gbk')); // 你好
 *  ```
 *
 *  Example 3 — escape a message for JavaScript source and for a URL:
 *  ```JavaScript
 *  const encoding = require('encoding');
 *
 *  console.log(encoding.jsstr("it's a\nnew line")); // it\'s a\nnew line
 *
 *  const query = encoding.encodeURIComponent('user name&role=admin');
 *  console.log(query); // user%20name%26role%3Dadmin
 *
 *  // encodeURI keeps the URL structure characters unescaped
 *  console.log(encoding.encodeURI('/search?q=a b#top')); // /search?q=a%20b#top
 *  console.log(encoding.decodeURI('/search?q=a%20b#top')); // /search?q=a b#top
 *  ```
 *
 *  Notes:
 *
 *  - The module is a fibjs extension: Node.js has no `encoding` module and spreads these
 *    helpers over `Buffer`, `url` and `querystring`. The shared conversions behave like
 *    their Buffer counterparts: `encoding.encode(data, codec)` matches `data.toString(codec)`
 *    and `encoding.decode(str, codec)` matches `Buffer.from(str, codec)`.
 *  - A string argument is first converted to its utf8 bytes, so when the input is not utf8
 *    pass a Buffer; for a charset codec a string argument would be reinterpreted as that
 *    charset and can produce mojibake.
 *  - Charset conversion depends on the ICU data in the build; `isEncoding` reports what is
 *    actually available.
 *
 */
declare module 'encoding' {
    /**
     * @description Base32 encoding and decoding module
     *
     *      The same object as `require('base32')`, provided as a member so that every codec can
     *      be reached from one module. `encode` accepts a Buffer or a utf8 string and returns
     *      the Base32 text; `decode` returns a Buffer. See the base32 module for the alphabet
     *      and padding details.
     *
     */
    const base32: typeof import ('base32');

    /**
     * @description Base64 encoding and decoding module
     *
     *      The same object as `require('base64')`. `encode(data, url)` returns standard Base64
     *      when url is false and the URL-safe alphabet (`-` and `_`) when it is true; `decode`
     *      accepts both alphabets and tolerates missing padding and whitespace.
     *      `encoding.encode(data, 'base64url')` is the one-call form of the URL-safe variant.
     *
     */
    const base64: typeof import ('base64');

    /**
     * @description Base58 encoding and decoding module
     *
     *      The same object as `require('base58')`. Base58 omits the characters `0`, `O`, `I`
     *      and `l` that are easy to confuse when a value is transcribed by hand, which is why
     *      Bitcoin addresses use it. The optional check version of encode/decode adds and
     *      verifies the Base58Check checksum.
     *
     */
    const base58: typeof import ('base58');

    /**
     * @description Hexadecimal encoding and decoding module
     *
     *      The same object as `require('hex')`. `encode` renders every byte as two lowercase
     *      hexadecimal digits; `decode` ignores characters that are not hexadecimal digits, so
     *      separators and whitespace in the input do not have to be removed first.
     *
     */
    const hex: typeof import ('hex');

    /**
     * @description Multibase encoding and decoding module
     *
     *      The same object as `require('multibase')`. Multibase prepends a one-character prefix
     *      that identifies the inner codec (`f` for base16, `b` for base32, `m` for base64,
     *      `z` for base58btc, and so on) so that a decoder can recover the format from the
     *      string itself; see the multibase module for the full prefix table.
     *
     */
    const multibase: typeof import ('multibase');

    /**
     * @description Determines whether the specified encoding is supported
     *
     *      The probe covers both codec families: the built-in Buffer codecs and the charsets
     *      the runtime ICU data provides. Label matching is case-insensitive and ASCII
     *      whitespace inside the label is ignored, so `'UTF-8'`, `'utf8 '` and `'ut f8'` all
     *      report true; `Buffer.isEncoding` is deliberately stricter and rejects whitespace, so
     *      the two helpers can disagree. A non-string argument is a TypeError.
     *
     *      Example — list the supported codec families:
     *      ```JavaScript
     *      const encoding = require('encoding');
     *
     *      ['utf8', 'hex', 'base32', 'base64url', 'latin1'].forEach((codec) => {
     *          console.log(codec, encoding.isEncoding(codec));
     *      });
     *
     *      console.log(encoding.isEncoding('gbk')); // true, an ICU charset
     *      console.log(encoding.isEncoding('no-such-charset')); // false
     *      ```
     *      @param codec the encoding label, such as "utf8", "base64" or "gbk"
     *      @return whether the label names a usable codec
     *
     */
    function isEncoding(codec: string): boolean;

    /**
     * @description Converts bytes into the text representation selected by the codec
     *
     *      The direction is bytes to text: data is treated as raw bytes and the result is the
     *      string form of those bytes under codec. For the Buffer codecs (`hex`, `base32`,
     *      `base58`, `base64`, `base64url`, `ascii`, `binary`/`latin1`) the bytes are simply
     *      re-represented; for a charset codec the bytes are decoded from that charset into
     *      Unicode text. A string argument is first converted to its utf8 bytes, so pass a
     *      Buffer when the input is not utf8 text. An unknown codec throws an Error with
     *      number 20024.
     *
     *      Example — the same bytes under several codecs:
     *      ```JavaScript
     *      const encoding = require('encoding');
     *
     *      const bytes = Buffer.from('abc', 'utf8');
     *      console.log(encoding.encode(bytes, 'hex')); // 616263
     *      console.log(encoding.encode(bytes, 'base32')); // mfrgg
     *      console.log(encoding.encode(bytes, 'base64')); // YWJj
     *      console.log(encoding.encode(bytes)); // abc, the default utf8 codec
     *      ```
     *      @param data the bytes to convert: a Buffer, or a string whose utf8 bytes are used
     *      @param codec the codec name, a Buffer codec or an ICU charset label, default "utf8"
     *      @return the text representation of the bytes
     *
     */
    function encode(data: Class_Buffer | string, codec?: string): string;

    /**
     * @description Converts text into bytes according to the codec
     *
     *      The direction is text to bytes: the returned Buffer holds the bytes the codec
     *      assigns to the string. With the default `utf8` codec the result is the utf8
     *      encoding of the string; with `hex`, `base32`, `base58`, `base64` and `base64url`
     *      the textual representation is parsed back into the bytes it stands for; with a
     *      charset codec the text is encoded into that charset, which is the way to prepare
     *      data for a legacy system. The textual codecs are lenient about trailing or invalid
     *      characters. An unknown codec throws an Error with number 20024.
     *
     *      Example — text back to bytes, including a charset:
     *      ```JavaScript
     *      const encoding = require('encoding');
     *
     *      console.log(encoding.decode('616263', 'hex').toString()); // abc
     *      console.log(encoding.decode('YWJj', 'base64').toString()); // abc
     *      console.log(encoding.decode('abc').toString()); // abc, the default utf8 codec
     *
     *      const gbk = encoding.decode('你好', 'gbk');
     *      console.log(gbk.hex()); // c4e3bac3
     *      ```
     *      @param str the text to convert
     *      @param codec the codec name, a Buffer codec or an ICU charset label, default "utf8"
     *      @return the bytes of the text in the requested codec
     *
     */
    function decode(str: string, codec?: string): Class_Buffer;

    /**
     * @description JSON encoding and decoding module
     *
     *      The same object as `require('json')`, provided here so that an application can reach
     *      every serialization format from the encoding module. `encode` serializes a value
     *      into a JSON string and `decode` parses one back; see the json module for the exact
     *      semantics.
     *
     */
    const json: typeof import ('json');

    /**
     * @description Msgpack encoding and decoding module
     *
     *      The same object as `require('msgpack')`. Msgpack is a binary interchange format that
     *      is usually more compact and faster to parse than JSON; `encode` returns a Buffer and
     *      `decode` restores the value. See the msgpack module for the supported types.
     *
     */
    const msgpack: typeof import ('msgpack');

    /**
     * @description Escapes a string so that it can be embedded in JavaScript source
     *
     *      The characters backslash, carriage return, line feed, tab and double quote are
     *      replaced with their backslash escapes; when json is true the single quote is left
     *      as it is, which makes the result valid inside a JSON string. Other characters,
     *      including non-ASCII text and other control characters, are copied unchanged, so the
     *      result is not a complete string literal on its own.
     *
     *      Example — escape a multi-line message for generated code:
     *      ```JavaScript
     *      const encoding = require('encoding');
     *
     *      const message = 'first line\nsecond line';
     *      console.log(encoding.jsstr(message)); // first line\nsecond line
     *      console.log(encoding.jsstr("it's", true)); // it's, JSON-compatible
     *      console.log('const msg = \'' + encoding.jsstr(message) + '\';');
     *      ```
     *      @param str the string to escape
     *      @param json whether to leave the single quote unescaped for JSON compatibility, default false
     *      @return the escaped string
     *
     */
    function jsstr(str: string, json?: boolean): string;

    /**
     * @description Percent-encodes a URL, leaving its structure characters intact
     *
     *      Characters that are not allowed in a URL are written as `%XX` (utf8 bytes,
     *      uppercase hex), but the characters that give a URL its structure stay literal, for
     *      example `/`, `?`, `=`, `&`, `#` and `:`. Use it for a complete URL; use
     *      `encodeURIComponent` for a single query value. `decodeURI` reverses both `%XX` and
     *      `%uXXXX` sequences.
     *
     *      Example — encode a full URL and decode it back:
     *      ```JavaScript
     *      const encoding = require('encoding');
     *
     *      const url = '/search?q=a b#top';
     *      console.log(encoding.encodeURI(url)); // /search?q=a%20b#top
     *      console.log(encoding.decodeURI('/search?q=a%20b#top')); // /search?q=a b#top
     *      ```
     *      @param url the URL to encode
     *      @return the percent-encoded URL
     *
     */
    function encodeURI(url: string): string;

    /**
     * @description Percent-encodes a string for use as one URL component
     *
     *      Everything except the unreserved characters and `!'()*` is percent-encoded, so the
     *      `/`, `&`, `?`, `=` and `#` characters that are part of the value cannot break the
     *      surrounding URL. When formEncoded is true a space becomes `+` instead of `%20`, the
     *      application/x-www-form-urlencoded convention.
     *
     *      Example — encode a query value, in both component forms:
     *      ```JavaScript
     *      const encoding = require('encoding');
     *
     *      console.log(encoding.encodeURIComponent('user name')); // user%20name
     *      console.log(encoding.encodeURIComponent('user name', true)); // user+name
     *      console.log(encoding.encodeURIComponent('中')); // %E4%B8%AD
     *      ```
     *      @param url the string to encode
     *      @param formEncoded whether to encode in application/x-www-form-urlencoded form, default false
     *      @return the percent-encoded string
     *
     */
    function encodeURIComponent(url: string, formEncoded?: boolean): string;

    /**
     * @description Decodes a percent-encoded string
     *
     *      Both `%XX` byte escapes and `%uXXXX`/`\uXXXX` code point escapes are decoded, so
     *      strings produced by `escape()` are accepted as well. A `+` is left unchanged because
     *      the function does not know whether the input is form data; use the querystring
     *      module when `+` must become a space.
     *
     *      @param url the string to decode
     *      @return the decoded string
     *
     */
    function decodeURI(url: string): string;

}

