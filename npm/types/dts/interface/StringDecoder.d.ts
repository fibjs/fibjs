/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Decodes a byte stream into text chunk by chunk, holding back the incomplete tail of a multibyte character so that characters split across chunk boundaries survive
 *
 *  A StringDecoder is the streaming counterpart of the whole-buffer conversions in the encoding module. Write each chunk as it arrives and the decoder returns the text that is complete, buffering up to three bytes of a partial UTF-8 sequence, half of a UTF-16 surrogate pair, or one or two bytes of a Base64 quantum until the next chunk completes it; `end` flushes the tail when the stream is over. Use it whenever a socket, file stream, decompressor or any other source can split the data at arbitrary byte offsets; decoding each chunk with `toString` would turn every split character into replacement characters.
 *
 *  Concepts:
 *
 *  - **Character boundaries**: a JavaScript string is a sequence of Unicode code points, but a UTF-8 character occupies one to four bytes (and a UTF-16 surrogate pair four bytes). A chunk boundary can fall in the middle of a character, and the bytes seen so far are meaningless until the character is complete; the decoder remembers them (`lastNeed`, `lastTotal` and `lastChar` expose that state) and prepends them to the next chunk.
 *  - **Per-encoding state**: `utf8` holds at most three bytes; `utf16le`/`ucs2` holds an odd byte or a high surrogate; `base64` holds one or two bytes and adds the missing padding on end; `hex`, `latin1`, `ascii` and the other pass-through codecs have no state at all, so their write is just a per-chunk `Buffer#toString`.
 *  - **Error behavior**: a malformed UTF-8 sequence yields U+FFFD for the offending part instead of throwing; an unsupported encoding name is rejected by the constructor with an Error (number 20024), and a non-string argument with a TypeError (number 20005).
 *
 *  Obtained from:
 *  - `new StringDecoder(encoding)` — create a decoder for one byte stream;
 *  - `require('string_decoder').StringDecoder` — the export of the string_decoder module; `require('node:string_decoder').StringDecoder` is the same class, and there is no global `StringDecoder`. The encoding module does not export it: that module provides the whole-buffer conversions only.
 *
 *  Example 1 — a multibyte character split across three chunks:
 *  ```JavaScript
 *  const { StringDecoder } = require('string_decoder');
 *
 *  const decoder = new StringDecoder('utf8');
 *  const euro = Buffer.from('€', 'utf8'); // e2 82 ac
 *
 *  console.log(decoder.write(euro.slice(0, 1))); // ''
 *  console.log(decoder.write(euro.slice(1, 2))); // ''
 *  console.log(decoder.write(euro.slice(2))); // €
 *  console.log(decoder.end()); // ''
 *  ```
 *
 *  Example 2 — chunk-by-chunk decoding versus per-chunk Buffer#toString:
 *  ```JavaScript
 *  const { StringDecoder } = require('string_decoder');
 *
 *  const chunks = [Buffer.from('price: '), Buffer.from([0xe2]), Buffer.from([0x82, 0xac])];
 *
 *  // Decoding every chunk on its own loses the split character
 *  console.log(JSON.stringify(chunks.map((c) => c.toString('utf8')).join('')));
 *  // "price: \ufffd\ufffd\ufffd"
 *
 *  // The decoder keeps the incomplete bytes and assembles the character
 *  const decoder = new StringDecoder('utf8');
 *  console.log(decoder.write(chunks[0]) + decoder.write(chunks[1]) +
 *      decoder.write(chunks[2]) + decoder.end()); // price: €
 *  ```
 *
 *  Example 3 — the same chunking rules for utf16le and base64 streams:
 *  ```JavaScript
 *  const { StringDecoder } = require('string_decoder');
 *
 *  // A surrogate pair split between chunks is reassembled
 *  const utf16 = new StringDecoder('utf16le');
 *  const thumbs = Buffer.from('👍', 'utf16le');
 *  console.log(utf16.write(thumbs.slice(0, 2))); // ''
 *  console.log(utf16.write(thumbs.slice(2))); // 👍
 *
 *  // A base64 decoder turns the byte stream into Base64 text, whatever the chunking
 *  const base64 = new StringDecoder('base64');
 *  const parts = [Buffer.from('hel'), Buffer.from('lo')];
 *  console.log(base64.write(parts[0]) + base64.write(parts[1]) + base64.end());
 *  // aGVsbG8=, the same as Buffer.from('hello').toString('base64')
 *  ```
 *
 *  Notes:
 *
 *  - The class follows Node.js's `string_decoder.StringDecoder` for the supported encodings and the chunk semantics. The errors differ: fibjs throws an Error with number 20024 for an unknown encoding where Node throws a TypeError, and `end` does not clear the pending state, so calling it twice returns the flush text twice while Node returns '' the second time.
 *  - `base32`, `base58` and `base64url` are rejected by the constructor even though `encoding.isEncoding` accepts them: they have no incremental decoder here.
 *
 */
declare class Class_StringDecoder extends Class_object {
    /**
     * @description Creates a decoder for the given encoding
     *
     *      The accepted names are `utf8`/`utf-8`, `utf16le`/`utf-16le`/`ucs2`, `base64`, `hex`, `latin1`/`binary` and `ascii`; the name is normalized (for example `ucs-2` becomes `utf16le`) and the canonical form is exposed by the `encoding` property. A name outside that list throws an Error with number 20024, even when `encoding.isEncoding` reports it as supported, because there is no incremental decoder for it; a non-string argument throws a TypeError with number 20005.
     *      @param encoding the encoding of the byte stream: "utf8", "utf16le", "ucs2", "base64", "hex", "latin1", "binary" or "ascii", default "utf8"
     *
     */
    constructor(encoding?: string);

    /**
     * @description Flushes the decoder, decoding one final chunk first when one is given
     *
     *      Once the stream is over, bytes still held for an incomplete character can no longer be completed: for utf8 they are returned as U+FFFD, for utf16le a held high surrogate is returned as a lone surrogate and a single trailing byte is dropped, and for base64 the missing padding is added. The pending state is not cleared, so calling end twice returns the same flush text; create a new decoder for a new stream.
     *
     *      Example — flush a truncated utf8 character:
     *      ```JavaScript
     *      const { StringDecoder } = require('string_decoder');
     *
     *      const decoder = new StringDecoder('utf8');
     *      decoder.write(Buffer.from([0xe2, 0x82])); // the first two bytes of €
     *
     *      // The stream ends before the third byte arrives: one replacement character.
     *      console.log(JSON.stringify(decoder.end())); // "\ufffd"
     *      ```
     *      @param buf the final chunk to decode before flushing, optional
     *      @return the remaining text, with U+FFFD for a truncated utf8 character
     *
     */
    end(buf?: Class_Buffer | string): string;

    /**
     * @description Decodes a chunk, holding back an incomplete trailing character
     *
     *      buf is appended to whatever the decoder kept from the previous chunk, and as much text as possible is returned. If the chunk ends in the middle of a character the incomplete bytes are kept and the return value is '' (or the complete part before them); the next write continues from there. A string argument is first converted to its utf8 bytes, so pass a Buffer when the stream is not utf8.
     *
     *      Example — a euro sign arriving byte by byte:
     *      ```JavaScript
     *      const { StringDecoder } = require('string_decoder');
     *
     *      const decoder = new StringDecoder('utf8');
     *      const euro = Buffer.from('€', 'utf8'); // e2 82 ac
     *
     *      console.log(decoder.write(euro.slice(0, 1))); // ''
     *      console.log(decoder.write(euro.slice(1, 2))); // ''
     *      console.log(decoder.write(euro.slice(2))); // €
     *      ```
     *      @param buf the chunk to decode: a Buffer, or a string read as utf8 bytes
     *      @return the decoded text available so far, '' while a character is incomplete
     *
     */
    write(buf: Class_Buffer | string): string;

    /**
     * @description Decodes a chunk starting at the given offset; this is the internal per-chunk routine
     *
     *      The write method uses it after consuming the bytes held from the previous chunk; offset selects where the still-undecoded part of buf starts. It reads and updates the same state as write (`lastNeed`, `lastTotal`, `lastChar`), so calling it by hand on a decoder that is also being written leaves the state inconsistent. It is exposed for compatibility with code that mirrors the Node.js internals; use write instead.
     *      @param buf the chunk to decode
     *      @param offset the index in buf where decoding starts
     *      @return the decoded part of the chunk, '' while a character is incomplete
     *
     */
    text(buf: Class_Buffer | string, offset: number): string;

    /**
     * @description Completes the held character with the leading bytes of a chunk; this is the internal routine
     *
     *      It is the other half of the internal state machine: write calls it when bytes are pending, then continues with text from the offset it consumed. When the chunk is too short it stores what it can, decreases `lastNeed` and returns null so write knows that no text is available yet; with enough bytes it returns the completed character. Direct calls leave the state partially updated and are not a supported way to drive the decoder; use write instead.
     *      @param buf the new chunk whose leading bytes complete the pending character
     *      @return the completed character, or null when more bytes are needed
     *
     */
    fillLast(buf: Class_Buffer | string): string;

    /**
     * @description Number of bytes still needed to complete the character held from the previous chunk
     *
     *      Zero when no partial character is pending, otherwise 1-3 for utf8, 1-2 for base64 and 1-2 for utf16le. The value is maintained by write and exposed for compatibility with the Node.js internals; it is not cleared by end, and changing it by hand breaks the decoder.
     *
     */
    lastNeed: number;

    /**
     * @description Total number of bytes of the character held from the previous chunk
     *
     *      Pairs with `lastNeed`: for utf8 a character that needs 3 bytes in total and still needs 2 reports `lastTotal` 3 and `lastNeed` 2; for utf16le a split surrogate reports 4 and 2; for base64 a held two-byte group reports 3. Internal state exposed for compatibility; do not modify.
     *
     */
    lastTotal: number;

    /**
     * @description Scratch buffer holding the bytes of the character kept for the next chunk
     *
     *      The Buffer is allocated once with the maximum size of the encoding's state (4 bytes for utf8 and utf16le, 3 for base64), and only the bytes counted by `lastNeed` are meaningful while a character is pending; the rest is uninitialized. Internal state exposed for compatibility; do not modify.
     *
     */
    lastChar: Class_Buffer;

    /**
     * @description Canonical name of the decoder's encoding
     *
     *      The constructor normalizes the name it was given: `utf-8` becomes `utf8`, `ucs-2` and `utf-16le` become `utf16le`, and `binary` becomes `latin1`. Read it to see which codec the decoder selects; assigning a new name changes later conversions but does not re-select the internal state machine, so only set it while the decoder is empty and the new name belongs to the same family.
     *
     */
    encoding: string;

}

