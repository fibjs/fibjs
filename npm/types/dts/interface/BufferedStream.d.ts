/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description A buffered reader over any Stream, with text helpers
 *
 *  BufferedStream wraps a binary stream and reads it ahead in chunks, so text
 *  operations such as readLine cost one underlying read per chunk instead of one
 *  per line. It adds text reading and writing on top of Stream, while `read`,
 *  `readAll`, `write` and `close` pass through to the wrapped stream (`stream`).
 *  The buffer belongs to the BufferedStream: once it has read ahead, the wrapped
 *  stream has already advanced, so never read the wrapped stream directly while
 *  this object is in use.
 *
 *  Concepts:
 *
 *  - **Read ahead**: the first read pulls a chunk (up to 64 KiB) from the wrapped
 *    stream into an internal buffer; later reads are served from that buffer
 *    until it is empty, then another chunk is pulled. The buffered data is
 *    implicitly shared by the binary and text reads of this object.
 *  - **Text encoding**: `charset` selects the encoding used by readText, readLine,
 *    readUntil and the write helpers (default "utf-8"). readText and readUntil
 *    count encoded bytes, not characters: a multi-byte character straddling the
 *    boundary decodes to the replacement character U+FFFD. writeText/writeLine
 *    return the encoded byte count.
 *  - **Line ending**: `EOL` selects the separator for readLine/readLines and is
 *    appended by writeLine. Its default empty value auto-detects on every line:
 *    "\r\n" is preferred, a lone "\n" is accepted, a lone "\r" is not. Assign
 *    "\n", "\r\n" or "\r" to fix it; any other non-empty value throws [20004].
 *  - **maxlen**: the maxlen argument of readLine/readUntil is a byte budget that
 *    includes the separator; exceeding it throws [20024]. A value of 0 or less
 *    means unlimited (note that this differs from readText where the size is
 *    mandatory and non-positive sizes read nothing).
 *  - **Writes and close**: writes go straight to the wrapped stream, so nothing
 *    is buffered on the write path; `flush` is a no-op here. `close` closes the
 *    wrapped stream, after which this object cannot read from it anymore.
 *
 *  Obtained from:
 *  - `new io.BufferedStream(stream)` — wrap any Stream (MemoryStream, FileStream,
 *    Socket, http body);
 *  - `fs.openTextStream(path[, flags])` — open a file and wrap it in one call.
 *
 *  Example 1 — read a file line by line with mixed line endings:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const io = require('io');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-buffered-'));
 *  const file = path.join(dir, 'lines.txt');
 *  fs.writeFile(file, 'first\nsecond\r\nthird\n');
 *
 *  const reader = new io.BufferedStream(fs.openFile(file));
 *  while (true) {
 *      const line = reader.readLine();
 *      if (line === null) break;
 *      console.log(line); // first, then second, then third
 *  }
 *
 *  reader.close();
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — write text and read it back through the same buffer class:
 *  ```JavaScript
 *  const io = require('io');
 *
 *  const stm = new io.MemoryStream();
 *  const writer = new io.BufferedStream(stm);
 *  console.log(writer.writeText('a'), writer.writeLine('b')); // 1 2
 *
 *  stm.rewind();
 *  const reader = new io.BufferedStream(stm);
 *  console.log(reader.readLines()); // ['a', 'b']
 *  ```
 *
 */
declare class Class_BufferedStream extends Class_Stream {
    /**
     * @description BufferedStream constructor
     *
     *      Wraps an already open binary stream. Any Stream is accepted, including
     *      MemoryStream, FileStream, Socket and http bodies. The wrapped stream is
     *      exposed as `stream`: this object reads ahead from it and close() closes
     *      it, so it must not be read directly while the buffer is in use (a direct
     *      read would consume data the buffer is going to serve).
     *
     *      Example — wrap a memory stream and read its first line:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('hello\nworld\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLine()); // hello
     *      console.log(reader.stream === stm); // true
     *      ```
     *
     *      @param stm the binary underlying stream object of the BufferedStream
     *
     */
    constructor(stm: Class_Stream | Class_StreamPromise);

    /**
     * @description Reads text of the specified number of characters
     *
     *      size counts encoded bytes, not Unicode characters (a multi-byte character
     *      that straddles the boundary decodes to U+FFFD and its remaining bytes are
     *      consumed). The text is decoded with the current charset. At the end of the
     *      stream the remaining text is returned once, then null; size 0 or a
     *      negative size read nothing and return null — to read everything left use
     *      `readAll` instead. The call blocks until size bytes are available or the
     *      stream ends, so readText never returns a short string before the end.
     *
     *      Example — read four bytes, then one more:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('hello world'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readText(4), reader.readText(1)); // hell o
     *      ```
     *
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readText(size: number): string;

    readText(size: number, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Reads text of the specified number of characters
     *
     *      size counts encoded bytes, not Unicode characters (a multi-byte character
     *      that straddles the boundary decodes to U+FFFD and its remaining bytes are
     *      consumed). The text is decoded with the current charset. At the end of the
     *      stream the remaining text is returned once, then null; size 0 or a
     *      negative size read nothing and return null — to read everything left use
     *      `readAll` instead. The call blocks until size bytes are available or the
     *      stream ends, so readText never returns a short string before the end.
     *
     *      Example — read four bytes, then one more:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('hello world'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readText(4), reader.readText(1)); // hell o
     *      ```
     *
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readTextSync(size: number): string;

    /**
     * @description Reads text of the specified number of characters
     *
     *      size counts encoded bytes, not Unicode characters (a multi-byte character
     *      that straddles the boundary decodes to U+FFFD and its remaining bytes are
     *      consumed). The text is decoded with the current charset. At the end of the
     *      stream the remaining text is returned once, then null; size 0 or a
     *      negative size read nothing and return null — to read everything left use
     *      `readAll` instead. The call blocks until size bytes are available or the
     *      stream ends, so readText never returns a short string before the end.
     *
     *      Example — read four bytes, then one more:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('hello world'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readText(4), reader.readText(1)); // hell o
     *      ```
     *
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readTextAsync(size: number): Promise<string>;

    /**
     * @description Reads one line of text
     *
     *      The line ends at the current EOL setting: with the default auto-detection
     *      "\r\n" or a lone "\n", whichever appears first; the separator is consumed
     *      and not part of the result. At the end of the stream the last unterminated
     *      line is returned once, then null. maxlen is a byte budget for the line
     *      including its separator; exceeding it throws [20024] and leaves the
     *      buffered data untouched. A maxlen of 0 or less means unlimited.
     *
     *      Example — read two lines and then detect the end:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('one\ntwo\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLine(), reader.readLine()); // one two
     *      console.log(reader.readLine()); // null
     *      ```
     *
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLine(maxlen?: number): string;

    readLine(maxlen?: number, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Reads one line of text
     *
     *      The line ends at the current EOL setting: with the default auto-detection
     *      "\r\n" or a lone "\n", whichever appears first; the separator is consumed
     *      and not part of the result. At the end of the stream the last unterminated
     *      line is returned once, then null. maxlen is a byte budget for the line
     *      including its separator; exceeding it throws [20024] and leaves the
     *      buffered data untouched. A maxlen of 0 or less means unlimited.
     *
     *      Example — read two lines and then detect the end:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('one\ntwo\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLine(), reader.readLine()); // one two
     *      console.log(reader.readLine()); // null
     *      ```
     *
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLineSync(maxlen?: number): string;

    /**
     * @description Reads one line of text
     *
     *      The line ends at the current EOL setting: with the default auto-detection
     *      "\r\n" or a lone "\n", whichever appears first; the separator is consumed
     *      and not part of the result. At the end of the stream the last unterminated
     *      line is returned once, then null. maxlen is a byte budget for the line
     *      including its separator; exceeding it throws [20024] and leaves the
     *      buffered data untouched. A maxlen of 0 or less means unlimited.
     *
     *      Example — read two lines and then detect the end:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('one\ntwo\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLine(), reader.readLine()); // one two
     *      console.log(reader.readLine()); // null
     *      ```
     *
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLineAsync(maxlen?: number): Promise<string>;

    /**
     * @description Reads a group of text lines as an array
     *
     *      Reads up to maxlines lines (all of them by default) and returns them
     *      without their separators, using the current EOL setting. The end of the
     *      stream yields an empty array, never null, and maxlines = 0 returns an
     *      empty array without reading. The array can be empty even when the stream
     *      had data that was already consumed by other reads.
     *
     *      Example — read at most two lines, then the rest:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('1\n2\n3\n4\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLines(2)); // ['1', '2']
     *      console.log(reader.readLines()); // ['3', '4']
     *      console.log(reader.readLines()); // []
     *      ```
     *
     *      @param maxlines the maximum number of lines to read this time; by default all text lines are read
     *      @return returns the array of text lines read; an empty array if there is no data to read, or the connection is interrupted
     *
     */
    readLines(maxlines?: number): string[];

    /**
     * @description Reads a text string ending with the specified bytes
     *
     *      Reads until the marker mk appears and returns the text before it; the
     *      marker itself is consumed and not included. mk can be any string, not
     *      just a line ending (for example a delimiter or a boundary token). If the
     *      stream ends before the marker, the remaining text is returned and the next
     *      read returns null; maxlen is a byte budget including the marker and
     *      exceeding it throws [20024]. A maxlen of 0 or less means unlimited.
     *
     *      Example — split a stream on a custom separator:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('a::b::c'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readUntil('::'), reader.readUntil('::')); // a b
     *      console.log(reader.readText(1)); // c
     *      ```
     *
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntil(mk: string, maxlen?: number): string;

    readUntil(mk: string, maxlen?: number, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Reads a text string ending with the specified bytes
     *
     *      Reads until the marker mk appears and returns the text before it; the
     *      marker itself is consumed and not included. mk can be any string, not
     *      just a line ending (for example a delimiter or a boundary token). If the
     *      stream ends before the marker, the remaining text is returned and the next
     *      read returns null; maxlen is a byte budget including the marker and
     *      exceeding it throws [20024]. A maxlen of 0 or less means unlimited.
     *
     *      Example — split a stream on a custom separator:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('a::b::c'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readUntil('::'), reader.readUntil('::')); // a b
     *      console.log(reader.readText(1)); // c
     *      ```
     *
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntilSync(mk: string, maxlen?: number): string;

    /**
     * @description Reads a text string ending with the specified bytes
     *
     *      Reads until the marker mk appears and returns the text before it; the
     *      marker itself is consumed and not included. mk can be any string, not
     *      just a line ending (for example a delimiter or a boundary token). If the
     *      stream ends before the marker, the remaining text is returned and the next
     *      read returns null; maxlen is a byte budget including the marker and
     *      exceeding it throws [20024]. A maxlen of 0 or less means unlimited.
     *
     *      Example — split a stream on a custom separator:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('a::b::c'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readUntil('::'), reader.readUntil('::')); // a b
     *      console.log(reader.readText(1)); // c
     *      ```
     *
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntilAsync(mk: string, maxlen?: number): Promise<string>;

    /**
     * @description Writes a string
     *
     *      The string is encoded with the current charset and written to the wrapped
     *      stream immediately (no write buffer is involved); the returned value is
     *      the encoded byte count, not the character count. A read-only wrapped
     *      stream rejects the write with [20009], and a closed one reports its own
     *      error.
     *
     *      Example — write text and inspect the underlying stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      console.log(writer.writeText('你好')); // 6
     *
     *      stm.rewind();
     *      console.log(stm.readAll().toString()); // 你好
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeText(txt: string): number;

    writeText(txt: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes a string
     *
     *      The string is encoded with the current charset and written to the wrapped
     *      stream immediately (no write buffer is involved); the returned value is
     *      the encoded byte count, not the character count. A read-only wrapped
     *      stream rejects the write with [20009], and a closed one reports its own
     *      error.
     *
     *      Example — write text and inspect the underlying stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      console.log(writer.writeText('你好')); // 6
     *
     *      stm.rewind();
     *      console.log(stm.readAll().toString()); // 你好
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeTextSync(txt: string): number;

    /**
     * @description Writes a string
     *
     *      The string is encoded with the current charset and written to the wrapped
     *      stream immediately (no write buffer is involved); the returned value is
     *      the encoded byte count, not the character count. A read-only wrapped
     *      stream rejects the write with [20009], and a closed one reports its own
     *      error.
     *
     *      Example — write text and inspect the underlying stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      console.log(writer.writeText('你好')); // 6
     *
     *      stm.rewind();
     *      console.log(stm.readAll().toString()); // 你好
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeTextAsync(txt: string): Promise<number>;

    /**
     * @description Writes a string and a newline character
     *
     *      Encodes txt with the current charset, appends the EOL setting (a single
     *      "\n" while EOL keeps its auto-detected default value) and writes both
     *      parts. The returned count includes the separator, so it is the number of
     *      bytes to skip when computing offsets.
     *
     *      Example — write CRLF-terminated lines:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      writer.EOL = '\r\n';
     *      console.log(writer.writeLine('ok')); // 4
     *
     *      stm.rewind();
     *      console.log(JSON.stringify(stm.readAll().toString())); // "ok\r\n"
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLine(txt: string): number;

    writeLine(txt: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes a string and a newline character
     *
     *      Encodes txt with the current charset, appends the EOL setting (a single
     *      "\n" while EOL keeps its auto-detected default value) and writes both
     *      parts. The returned count includes the separator, so it is the number of
     *      bytes to skip when computing offsets.
     *
     *      Example — write CRLF-terminated lines:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      writer.EOL = '\r\n';
     *      console.log(writer.writeLine('ok')); // 4
     *
     *      stm.rewind();
     *      console.log(JSON.stringify(stm.readAll().toString())); // "ok\r\n"
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLineSync(txt: string): number;

    /**
     * @description Writes a string and a newline character
     *
     *      Encodes txt with the current charset, appends the EOL setting (a single
     *      "\n" while EOL keeps its auto-detected default value) and writes both
     *      parts. The returned count includes the separator, so it is the number of
     *      bytes to skip when computing offsets.
     *
     *      Example — write CRLF-terminated lines:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      writer.EOL = '\r\n';
     *      console.log(writer.writeLine('ok')); // 4
     *
     *      stm.rewind();
     *      console.log(JSON.stringify(stm.readAll().toString())); // "ok\r\n"
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLineAsync(txt: string): Promise<number>;

    /**
     * @description Queries the stream object used when the buffer was created
     *
     *      The wrapped stream is the read/write target of this object: writes pass
     *      through to it, close() closes it and its `fd` (when it has one) is the
     *      file descriptor behind this stream. Do not read it directly while the
     *      buffer holds read-ahead data.
     *
     *      Example — compare the wrapped stream with the original object:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('x\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readText(1), reader.stream === stm); // x true
     *      ```
     *
     */
    readonly stream: Class_Stream;

    /**
     * @description Queries and sets the charset used when processing text, default is utf-8
     *
     *      The value is an encoding name accepted by the encoding module, such as
     *      "utf-8", "gbk" or "windows-1252"; the getter returns the canonical name
     *      (for example "latin1" is reported as "windows-1252"). Assigning it
     *      re-opens the converter used by all following text reads and writes; the
     *      binary reads are not affected.
     *
     *      Example — switch the text encoding of an existing reader:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const reader = new io.BufferedStream(stm);
     *      reader.charset = 'latin1';
     *      console.log(reader.charset); // windows-1252
     *
     *      reader.writeText('café');
     *      stm.rewind();
     *      console.log(reader.readText(4)); // café
     *      ```
     *
     */
    charset: string;

    /**
     * @description Queries and sets the line ending marker; the default auto-detects "\r\n" or "\n"
     *
     *      The default value is an empty string, which means auto-detect: every line
     *      is scanned for "\r\n" and for a lone "\n" (whichever comes first; a lone
     *      "\r" is not a separator). Assigning "\n", "\r\n" or "\r" fixes the
     *      separator for readLine/readLines/readUntil, and writeLine appends it;
     *      assigning any other non-empty value throws [20004]. The getter returns the
     *      assigned value, or "" while auto-detection is active.
     *
     *      Example — force CRLF and read both lines:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('a\r\nb\r\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      reader.EOL = '\r\n';
     *      console.log(reader.readLines()); // ['a', 'b']
     *      ```
     *
     */
    EOL: string;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * The promise variant of the BufferedStream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_BufferedStreamPromise extends Class_StreamPromise {
    /**
     * @description BufferedStream constructor
     *
     *      Wraps an already open binary stream. Any Stream is accepted, including
     *      MemoryStream, FileStream, Socket and http bodies. The wrapped stream is
     *      exposed as `stream`: this object reads ahead from it and close() closes
     *      it, so it must not be read directly while the buffer is in use (a direct
     *      read would consume data the buffer is going to serve).
     *
     *      Example — wrap a memory stream and read its first line:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('hello\nworld\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLine()); // hello
     *      console.log(reader.stream === stm); // true
     *      ```
     *
     *      @param stm the binary underlying stream object of the BufferedStream
     *
     */
    constructor(stm: Class_Stream | Class_StreamPromise);

    /**
     * @description Reads text of the specified number of characters
     *
     *      size counts encoded bytes, not Unicode characters (a multi-byte character
     *      that straddles the boundary decodes to U+FFFD and its remaining bytes are
     *      consumed). The text is decoded with the current charset. At the end of the
     *      stream the remaining text is returned once, then null; size 0 or a
     *      negative size read nothing and return null — to read everything left use
     *      `readAll` instead. The call blocks until size bytes are available or the
     *      stream ends, so readText never returns a short string before the end.
     *
     *      Example — read four bytes, then one more:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('hello world'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readText(4), reader.readText(1)); // hell o
     *      ```
     *
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readText(size: number): Promise<string>;

    /**
     * @description Reads text of the specified number of characters
     *
     *      size counts encoded bytes, not Unicode characters (a multi-byte character
     *      that straddles the boundary decodes to U+FFFD and its remaining bytes are
     *      consumed). The text is decoded with the current charset. At the end of the
     *      stream the remaining text is returned once, then null; size 0 or a
     *      negative size read nothing and return null — to read everything left use
     *      `readAll` instead. The call blocks until size bytes are available or the
     *      stream ends, so readText never returns a short string before the end.
     *
     *      Example — read four bytes, then one more:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('hello world'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readText(4), reader.readText(1)); // hell o
     *      ```
     *
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readTextSync(size: number): string;

    /**
     * @description Reads text of the specified number of characters
     *
     *      size counts encoded bytes, not Unicode characters (a multi-byte character
     *      that straddles the boundary decodes to U+FFFD and its remaining bytes are
     *      consumed). The text is decoded with the current charset. At the end of the
     *      stream the remaining text is returned once, then null; size 0 or a
     *      negative size read nothing and return null — to read everything left use
     *      `readAll` instead. The call blocks until size bytes are available or the
     *      stream ends, so readText never returns a short string before the end.
     *
     *      Example — read four bytes, then one more:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('hello world'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readText(4), reader.readText(1)); // hell o
     *      ```
     *
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readTextAsync(size: number): Promise<string>;

    /**
     * @description Reads one line of text
     *
     *      The line ends at the current EOL setting: with the default auto-detection
     *      "\r\n" or a lone "\n", whichever appears first; the separator is consumed
     *      and not part of the result. At the end of the stream the last unterminated
     *      line is returned once, then null. maxlen is a byte budget for the line
     *      including its separator; exceeding it throws [20024] and leaves the
     *      buffered data untouched. A maxlen of 0 or less means unlimited.
     *
     *      Example — read two lines and then detect the end:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('one\ntwo\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLine(), reader.readLine()); // one two
     *      console.log(reader.readLine()); // null
     *      ```
     *
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLine(maxlen?: number): Promise<string>;

    /**
     * @description Reads one line of text
     *
     *      The line ends at the current EOL setting: with the default auto-detection
     *      "\r\n" or a lone "\n", whichever appears first; the separator is consumed
     *      and not part of the result. At the end of the stream the last unterminated
     *      line is returned once, then null. maxlen is a byte budget for the line
     *      including its separator; exceeding it throws [20024] and leaves the
     *      buffered data untouched. A maxlen of 0 or less means unlimited.
     *
     *      Example — read two lines and then detect the end:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('one\ntwo\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLine(), reader.readLine()); // one two
     *      console.log(reader.readLine()); // null
     *      ```
     *
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLineSync(maxlen?: number): string;

    /**
     * @description Reads one line of text
     *
     *      The line ends at the current EOL setting: with the default auto-detection
     *      "\r\n" or a lone "\n", whichever appears first; the separator is consumed
     *      and not part of the result. At the end of the stream the last unterminated
     *      line is returned once, then null. maxlen is a byte budget for the line
     *      including its separator; exceeding it throws [20024] and leaves the
     *      buffered data untouched. A maxlen of 0 or less means unlimited.
     *
     *      Example — read two lines and then detect the end:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('one\ntwo\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLine(), reader.readLine()); // one two
     *      console.log(reader.readLine()); // null
     *      ```
     *
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLineAsync(maxlen?: number): Promise<string>;

    /**
     * @description Reads a group of text lines as an array
     *
     *      Reads up to maxlines lines (all of them by default) and returns them
     *      without their separators, using the current EOL setting. The end of the
     *      stream yields an empty array, never null, and maxlines = 0 returns an
     *      empty array without reading. The array can be empty even when the stream
     *      had data that was already consumed by other reads.
     *
     *      Example — read at most two lines, then the rest:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('1\n2\n3\n4\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readLines(2)); // ['1', '2']
     *      console.log(reader.readLines()); // ['3', '4']
     *      console.log(reader.readLines()); // []
     *      ```
     *
     *      @param maxlines the maximum number of lines to read this time; by default all text lines are read
     *      @return returns the array of text lines read; an empty array if there is no data to read, or the connection is interrupted
     *
     */
    readLines(maxlines?: number): string[];

    /**
     * @description Reads a text string ending with the specified bytes
     *
     *      Reads until the marker mk appears and returns the text before it; the
     *      marker itself is consumed and not included. mk can be any string, not
     *      just a line ending (for example a delimiter or a boundary token). If the
     *      stream ends before the marker, the remaining text is returned and the next
     *      read returns null; maxlen is a byte budget including the marker and
     *      exceeding it throws [20024]. A maxlen of 0 or less means unlimited.
     *
     *      Example — split a stream on a custom separator:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('a::b::c'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readUntil('::'), reader.readUntil('::')); // a b
     *      console.log(reader.readText(1)); // c
     *      ```
     *
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntil(mk: string, maxlen?: number): Promise<string>;

    /**
     * @description Reads a text string ending with the specified bytes
     *
     *      Reads until the marker mk appears and returns the text before it; the
     *      marker itself is consumed and not included. mk can be any string, not
     *      just a line ending (for example a delimiter or a boundary token). If the
     *      stream ends before the marker, the remaining text is returned and the next
     *      read returns null; maxlen is a byte budget including the marker and
     *      exceeding it throws [20024]. A maxlen of 0 or less means unlimited.
     *
     *      Example — split a stream on a custom separator:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('a::b::c'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readUntil('::'), reader.readUntil('::')); // a b
     *      console.log(reader.readText(1)); // c
     *      ```
     *
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntilSync(mk: string, maxlen?: number): string;

    /**
     * @description Reads a text string ending with the specified bytes
     *
     *      Reads until the marker mk appears and returns the text before it; the
     *      marker itself is consumed and not included. mk can be any string, not
     *      just a line ending (for example a delimiter or a boundary token). If the
     *      stream ends before the marker, the remaining text is returned and the next
     *      read returns null; maxlen is a byte budget including the marker and
     *      exceeding it throws [20024]. A maxlen of 0 or less means unlimited.
     *
     *      Example — split a stream on a custom separator:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('a::b::c'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readUntil('::'), reader.readUntil('::')); // a b
     *      console.log(reader.readText(1)); // c
     *      ```
     *
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntilAsync(mk: string, maxlen?: number): Promise<string>;

    /**
     * @description Writes a string
     *
     *      The string is encoded with the current charset and written to the wrapped
     *      stream immediately (no write buffer is involved); the returned value is
     *      the encoded byte count, not the character count. A read-only wrapped
     *      stream rejects the write with [20009], and a closed one reports its own
     *      error.
     *
     *      Example — write text and inspect the underlying stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      console.log(writer.writeText('你好')); // 6
     *
     *      stm.rewind();
     *      console.log(stm.readAll().toString()); // 你好
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeText(txt: string): Promise<number>;

    /**
     * @description Writes a string
     *
     *      The string is encoded with the current charset and written to the wrapped
     *      stream immediately (no write buffer is involved); the returned value is
     *      the encoded byte count, not the character count. A read-only wrapped
     *      stream rejects the write with [20009], and a closed one reports its own
     *      error.
     *
     *      Example — write text and inspect the underlying stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      console.log(writer.writeText('你好')); // 6
     *
     *      stm.rewind();
     *      console.log(stm.readAll().toString()); // 你好
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeTextSync(txt: string): number;

    /**
     * @description Writes a string
     *
     *      The string is encoded with the current charset and written to the wrapped
     *      stream immediately (no write buffer is involved); the returned value is
     *      the encoded byte count, not the character count. A read-only wrapped
     *      stream rejects the write with [20009], and a closed one reports its own
     *      error.
     *
     *      Example — write text and inspect the underlying stream:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      console.log(writer.writeText('你好')); // 6
     *
     *      stm.rewind();
     *      console.log(stm.readAll().toString()); // 你好
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeTextAsync(txt: string): Promise<number>;

    /**
     * @description Writes a string and a newline character
     *
     *      Encodes txt with the current charset, appends the EOL setting (a single
     *      "\n" while EOL keeps its auto-detected default value) and writes both
     *      parts. The returned count includes the separator, so it is the number of
     *      bytes to skip when computing offsets.
     *
     *      Example — write CRLF-terminated lines:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      writer.EOL = '\r\n';
     *      console.log(writer.writeLine('ok')); // 4
     *
     *      stm.rewind();
     *      console.log(JSON.stringify(stm.readAll().toString())); // "ok\r\n"
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLine(txt: string): Promise<number>;

    /**
     * @description Writes a string and a newline character
     *
     *      Encodes txt with the current charset, appends the EOL setting (a single
     *      "\n" while EOL keeps its auto-detected default value) and writes both
     *      parts. The returned count includes the separator, so it is the number of
     *      bytes to skip when computing offsets.
     *
     *      Example — write CRLF-terminated lines:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      writer.EOL = '\r\n';
     *      console.log(writer.writeLine('ok')); // 4
     *
     *      stm.rewind();
     *      console.log(JSON.stringify(stm.readAll().toString())); // "ok\r\n"
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLineSync(txt: string): number;

    /**
     * @description Writes a string and a newline character
     *
     *      Encodes txt with the current charset, appends the EOL setting (a single
     *      "\n" while EOL keeps its auto-detected default value) and writes both
     *      parts. The returned count includes the separator, so it is the number of
     *      bytes to skip when computing offsets.
     *
     *      Example — write CRLF-terminated lines:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const writer = new io.BufferedStream(stm);
     *      writer.EOL = '\r\n';
     *      console.log(writer.writeLine('ok')); // 4
     *
     *      stm.rewind();
     *      console.log(JSON.stringify(stm.readAll().toString())); // "ok\r\n"
     *      ```
     *
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLineAsync(txt: string): Promise<number>;

    /**
     * @description Queries the stream object used when the buffer was created
     *
     *      The wrapped stream is the read/write target of this object: writes pass
     *      through to it, close() closes it and its `fd` (when it has one) is the
     *      file descriptor behind this stream. Do not read it directly while the
     *      buffer holds read-ahead data.
     *
     *      Example — compare the wrapped stream with the original object:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('x\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      console.log(reader.readText(1), reader.stream === stm); // x true
     *      ```
     *
     */
    readonly stream: Class_StreamPromise;

    /**
     * @description Queries and sets the charset used when processing text, default is utf-8
     *
     *      The value is an encoding name accepted by the encoding module, such as
     *      "utf-8", "gbk" or "windows-1252"; the getter returns the canonical name
     *      (for example "latin1" is reported as "windows-1252"). Assigning it
     *      re-opens the converter used by all following text reads and writes; the
     *      binary reads are not affected.
     *
     *      Example — switch the text encoding of an existing reader:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      const reader = new io.BufferedStream(stm);
     *      reader.charset = 'latin1';
     *      console.log(reader.charset); // windows-1252
     *
     *      reader.writeText('café');
     *      stm.rewind();
     *      console.log(reader.readText(4)); // café
     *      ```
     *
     */
    charset: string;

    /**
     * @description Queries and sets the line ending marker; the default auto-detects "\r\n" or "\n"
     *
     *      The default value is an empty string, which means auto-detect: every line
     *      is scanned for "\r\n" and for a lone "\n" (whichever comes first; a lone
     *      "\r" is not a separator). Assigning "\n", "\r\n" or "\r" fixes the
     *      separator for readLine/readLines/readUntil, and writeLine appends it;
     *      assigning any other non-empty value throws [20004]. The getter returns the
     *      assigned value, or "" while auto-detection is active.
     *
     *      Example — force CRLF and read both lines:
     *      ```JavaScript
     *      const io = require('io');
     *
     *      const stm = new io.MemoryStream();
     *      stm.write(Buffer.from('a\r\nb\r\n'));
     *      stm.rewind();
     *
     *      const reader = new io.BufferedStream(stm);
     *      reader.EOL = '\r\n';
     *      console.log(reader.readLines()); // ['a', 'b']
     *      ```
     *
     */
    EOL: string;

}


declare namespace Class_BufferedStream {
    const promises: FIBJS.GeneralObject;
}
