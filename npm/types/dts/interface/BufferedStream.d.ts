/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description Buffered reading object
 *
 *  The BufferedStream object is a buffered stream object for binary stream reading. It can buffer its underlying stream and provides text reading capability. To use a BufferedStream object, simply pass the stream object to process as the constructor parameter. Creation method:
 *  ```JavaScript
 *  var reader = new io.BufferedStream(stream);
 *  ```
 *
 *  BufferedStream inherits from the Stream object and has all the methods and properties of the Stream object. The stream property queries the stream object used when the buffer object was created. The BufferedStream object also supports the EOL property to query and set the line ending marker (by default posix:\"\n\"; windows:\"\r\n\") and the charset property to query and set the charset used when processing text, default is utf-8.
 *
 *  When reading stream data, the BufferedStream object uses a chunked approach: it first reads data into the buffer and then fetches data from the buffer, which effectively reduces the number of network interactions when reading stream data and improves reading efficiency.
 *
 *  The BufferedStream object also provides a write method that writes the given data to the stream and, when the underlying stream object is blocked for writing, waits until it can accept data before proceeding. The Flush method writes the file buffer content to the physical device. The close method closes the current stream object. The concrete implementation of some methods can be provided in subclasses.
 *
 *  When using a BufferedStream object, take care not to mix it with other underlying stream objects already in use, otherwise data may be read twice or read incorrectly.
 *
 *  Below is an example of using a BufferedStream object to read file content:
 *   ```JavaScript
 *   var fs = require('fs');
 *   var io = require('io');
 *
 *   var filename = "test.txt";
 *
 *   // open file
 *   var file = fs.openFile(filename);
 *
 *   // create BufferedStream object
 *   var reader = new io.BufferedStream(file);
 *
 *   // read file content
 *   var lines = reader.readLines();
 *
 *   for(var i = 0; i < lines.length; i ++)
 *       console.log(lines[i]);
 *
 *   // close file
 *   file.close();
 *   ```
 *
 */
declare class Class_BufferedStream extends Class_Stream {
    /**
     * @description BufferedStream constructor
     *       @param stm the binary underlying stream object of the BufferedStream
     *
     */
    constructor(stm: Class_Stream | Class_StreamPromise);

    /**
     * @description Reads text of the specified number of characters
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readText(size: number): string;

    readText(size: number, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Reads text of the specified number of characters
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readTextSync(size: number): string;

    /**
     * @description Reads text of the specified number of characters
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readTextAsync(size: number): Promise<string>;

    /**
     * @description Reads one line of text; the line ending is based on the EOL property setting, by default posix:\"\\n\"; windows:\"\\r\\n\"
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLine(maxlen?: number): string;

    readLine(maxlen?: number, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Reads one line of text; the line ending is based on the EOL property setting, by default posix:\"\\n\"; windows:\"\\r\\n\"
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLineSync(maxlen?: number): string;

    /**
     * @description Reads one line of text; the line ending is based on the EOL property setting, by default posix:\"\\n\"; windows:\"\\r\\n\"
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLineAsync(maxlen?: number): Promise<string>;

    /**
     * @description Reads a group of text lines as an array; the line ending is based on the EOL property setting, by default posix:\"\\n\"; windows:\"\\r\\n\"
     *      @param maxlines the maximum number of lines to read this time; by default all text lines are read
     *      @return returns the array of text lines read; an empty array if there is no data to read, or the connection is interrupted
     *
     */
    readLines(maxlines?: number): string[];

    /**
     * @description Reads a text string ending with the specified bytes
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntil(mk: string, maxlen?: number): string;

    readUntil(mk: string, maxlen?: number, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Reads a text string ending with the specified bytes
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntilSync(mk: string, maxlen?: number): string;

    /**
     * @description Reads a text string ending with the specified bytes
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntilAsync(mk: string, maxlen?: number): Promise<string>;

    /**
     * @description Writes a string
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeText(txt: string): number;

    writeText(txt: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes a string
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeTextSync(txt: string): number;

    /**
     * @description Writes a string
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeTextAsync(txt: string): Promise<number>;

    /**
     * @description Writes a string and a newline character
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLine(txt: string): number;

    writeLine(txt: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes a string and a newline character
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLineSync(txt: string): number;

    /**
     * @description Writes a string and a newline character
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLineAsync(txt: string): Promise<number>;

    /**
     * @description Queries the stream object used when the buffer was created
     */
    readonly stream: Class_Stream;

    /**
     * @description Queries and sets the charset used when processing text, default is utf-8
     */
    charset: string;

    /**
     * @description Queries and sets the line ending marker, by default posix:\"\\n\"; windows:\"\\r\\n\"
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
     *       @param stm the binary underlying stream object of the BufferedStream
     *
     */
    constructor(stm: Class_Stream | Class_StreamPromise);

    /**
     * @description Reads text of the specified number of characters
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readText(size: number): Promise<string>;

    /**
     * @description Reads text of the specified number of characters
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readTextSync(size: number): string;

    /**
     * @description Reads text of the specified number of characters
     *      @param size the number of text characters to read, measured in utf8 or the specified encoding bytes
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readTextAsync(size: number): Promise<string>;

    /**
     * @description Reads one line of text; the line ending is based on the EOL property setting, by default posix:\"\\n\"; windows:\"\\r\\n\"
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLine(maxlen?: number): Promise<string>;

    /**
     * @description Reads one line of text; the line ending is based on the EOL property setting, by default posix:\"\\n\"; windows:\"\\r\\n\"
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLineSync(maxlen?: number): string;

    /**
     * @description Reads one line of text; the line ending is based on the EOL property setting, by default posix:\"\\n\"; windows:\"\\r\\n\"
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readLineAsync(maxlen?: number): Promise<string>;

    /**
     * @description Reads a group of text lines as an array; the line ending is based on the EOL property setting, by default posix:\"\\n\"; windows:\"\\r\\n\"
     *      @param maxlines the maximum number of lines to read this time; by default all text lines are read
     *      @return returns the array of text lines read; an empty array if there is no data to read, or the connection is interrupted
     *
     */
    readLines(maxlines?: number): string[];

    /**
     * @description Reads a text string ending with the specified bytes
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntil(mk: string, maxlen?: number): Promise<string>;

    /**
     * @description Reads a text string ending with the specified bytes
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntilSync(mk: string, maxlen?: number): string;

    /**
     * @description Reads a text string ending with the specified bytes
     *      @param mk the ending string
     *      @param maxlen the maximum string to read this time, measured in utf8 encoded bytes; by default the number of characters is not limited
     *      @return returns the text string read; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readUntilAsync(mk: string, maxlen?: number): Promise<string>;

    /**
     * @description Writes a string
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeText(txt: string): Promise<number>;

    /**
     * @description Writes a string
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeTextSync(txt: string): number;

    /**
     * @description Writes a string
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeTextAsync(txt: string): Promise<number>;

    /**
     * @description Writes a string and a newline character
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLine(txt: string): Promise<number>;

    /**
     * @description Writes a string and a newline character
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLineSync(txt: string): number;

    /**
     * @description Writes a string and a newline character
     *      @param txt the string to write
     *      @return the number of bytes actually written
     *
     */
    writeLineAsync(txt: string): Promise<number>;

    /**
     * @description Queries the stream object used when the buffer was created
     */
    readonly stream: Class_StreamPromise;

    /**
     * @description Queries and sets the charset used when processing text, default is utf-8
     */
    charset: string;

    /**
     * @description Queries and sets the line ending marker, by default posix:\"\\n\"; windows:\"\\r\\n\"
     */
    EOL: string;

}


declare namespace Class_BufferedStream {
    const promises: FIBJS.GeneralObject;
}
