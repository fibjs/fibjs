/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZipFile.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description The zip module is used for file compression and decompression. It provides operations such as compressing, decompressing, searching and enumerating the file list in a zip file
 *
 * With the zip module, we can pack multiple files into a zip file, and also decompress a zip file to restore the original files.
 *
 * The following are some examples:
 *
 * 1. Compress files:
 *
 * ```JavaScript
 * var zip = require('zip');
 * var zipfile = zip.open('/path/to/dest.zip', 'w');
 *
 * zipfile.write('/path/to/src1', 'src1');
 * zipfile.write('/path/to/src2', 'src2');
 * zipfile.close();
 * ```
 *
 * 2. Decompress files:
 *
 * ```JavaScript
 * var zip = require('zip');
 * var zipfile = zip.open('/path/to/src.zip', 'r');
 *
 * var filenames = zipfile.namelist();
 * for (var i = 0; i < filenames.length; ++i) {
 *   var filename = filenames[i];
 *   var data = zipfile.read(filename);
 *   console.log(filename + ': ' + data.length + ' bytes');
 * }
 * zipfile.close();
 * ```
 *
 */
declare module 'zip' {
    /**
     * @description  Determines whether a file is in zip format
     * 	 @param filename file name
     * 	 @return returns true if the file is a zip file
     *
     */
    function isZipFile(filename: string): boolean;

    function isZipFile(filename: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description  Determines whether a file is in zip format
     * 	 @param filename file name
     * 	 @return returns true if the file is a zip file
     *
     */
    function isZipFileSync(filename: string): boolean;

    /**
     * @description  Determines whether a file is in zip format
     * 	 @param filename file name
     * 	 @return returns true if the file is a zip file
     *
     */
    function isZipFileAsync(filename: string): Promise<boolean>;

    /**
     * @description Opens a zip file
     * 	 @param path file path
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function open(path: string, mod?: string, codec?: string): Class_ZipFile;

    function open(path: string, mod?: string, codec?: string, callback: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

    /**
     * @description Opens a zip file
     * 	 @param path file path
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function openSync(path: string, mod?: string, codec?: string): Class_ZipFile;

    /**
     * @description Opens a zip file
     * 	 @param path file path
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function openAsync(path: string, mod?: string, codec?: string): Promise<Class_ZipFilePromise>;

    /**
     * @description Opens a zip file
     * 	 @param data zip file data
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function open(data: Class_Buffer, mod?: string, codec?: string): Class_ZipFile;

    function open(data: Class_Buffer, mod?: string, codec?: string, callback: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

    /**
     * @description Opens a zip file
     * 	 @param data zip file data
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function openSync(data: Class_Buffer, mod?: string, codec?: string): Class_ZipFile;

    /**
     * @description Opens a zip file
     * 	 @param data zip file data
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function openAsync(data: Class_Buffer, mod?: string, codec?: string): Promise<Class_ZipFilePromise>;

    /**
     * @description Opens a zip file
     * 	 @param strm zip file stream
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function open(strm: Class_SeekableStream | Class_SeekableStreamPromise, mod?: string, codec?: string): Class_ZipFile;

    function open(strm: Class_SeekableStream | Class_SeekableStreamPromise, mod?: string, codec?: string, callback: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

    /**
     * @description Opens a zip file
     * 	 @param strm zip file stream
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function openSync(strm: Class_SeekableStream | Class_SeekableStreamPromise, mod?: string, codec?: string): Class_ZipFile;

    /**
     * @description Opens a zip file
     * 	 @param strm zip file stream
     * 	 @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     * 	 @param codec sets the encoding of the zip file, default "utf8"
     * 	 @return returns the zip file object
     *
     */
    function openAsync(strm: Class_SeekableStream | Class_SeekableStreamPromise, mod?: string, codec?: string): Promise<Class_ZipFilePromise>;

}

