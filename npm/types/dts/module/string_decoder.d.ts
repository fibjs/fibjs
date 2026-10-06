/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/StringDecoder.d.ts" />
/**
 * @description Node.js-compatible alias module whose single export is the StringDecoder class
 *
 *  The module exists so that Node.js code doing `require('string_decoder')` keeps working:
 *  it exports the same class as the StringDecoder interface, reachable as
 *  `require('string_decoder').StringDecoder` and also as
 *  `require('node:string_decoder').StringDecoder`. There is no global `StringDecoder`, and
 *  the encoding module does not export the class; that module provides the whole-buffer
 *  codecs only.
 *
 *  Concepts:
 *
 *  - **Streaming decode**: StringDecoder converts a byte stream into text incrementally,
 *    holding the incomplete tail of a multibyte character until the next chunk; see the
 *    StringDecoder interface for the chunk-boundary model and the per-encoding state.
 *  - **Node compatibility**: the module name, the export name and the chunk semantics
 *    match Node.js; the error types and the behavior of calling `end` twice differ, see the
 *    StringDecoder interface notes.
 *
 *  Import:
 *  ```JavaScript
 *  const { StringDecoder } = require('string_decoder');
 *  ```
 *
 *  Example 1 — decode a utf8 stream whose chunk boundary splits a character:
 *  ```JavaScript
 *  const { StringDecoder } = require('string_decoder');
 *
 *  const decoder = new StringDecoder('utf8');
 *  console.log(decoder.write(Buffer.from([0xe4, 0xb8]))); // ''
 *  console.log(decoder.write(Buffer.from([0xad]))); // 中
 *  console.log(decoder.end()); // ''
 *  ```
 *
 *  Example 2 — feed one byte at a time and flush with end:
 *  ```JavaScript
 *  const { StringDecoder } = require('string_decoder');
 *
 *  const bytes = Buffer.from('a中👍', 'utf8');
 *  const decoder = new StringDecoder('utf8');
 *
 *  let text = '';
 *  for (let i = 0; i < bytes.length; i++)
 *      text += decoder.write(bytes.slice(i, i + 1));
 *
 *  console.log(text + decoder.end()); // a中👍
 *  ```
 *
 */
declare module 'string_decoder' {
    /**
     * @description The StringDecoder class; see the StringDecoder interface for details
     *
     *      This single export is the class itself: create an instance with `new` and choose the encoding of the stream at construction, as in `new StringDecoder('utf8')`. The class is exported rather than a ready-made instance because a decoder carries the state of one stream, so every stream needs its own object.
     *
     */
    const StringDecoder: typeof Class_StringDecoder;

}

