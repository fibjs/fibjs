/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZipFile.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * The promise variant of the zip module: async members return a Promise as their primary form.
 */
declare module 'zip/promises' {
    /**
     * @description  Determines whether a file is in zip format
     * 	 @param filename file name
     * 	 @return returns true if the file is a zip file
     *
     */
    function isZipFile(filename: string): Promise<boolean>;

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
    function open(path: string, mod?: string, codec?: string): Promise<Class_ZipFilePromise>;

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
    function open(data: Class_Buffer, mod?: string, codec?: string): Promise<Class_ZipFilePromise>;

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
    function open(strm: Class_SeekableStream | Class_SeekableStreamPromise, mod?: string, codec?: string): Promise<Class_ZipFilePromise>;

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


declare module "zip" {
    const promises: typeof import("zip/promises");
}
