/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The File object represents a file in the file system, compatible with the Web standard File API.
 *
 * File inherits from Blob; besides all the binary data capabilities of Blob, it adds properties such as the file name (name) and last modification time (lastModified), and is commonly used in scenarios such as file upload, download and Web API interaction.
 *
 * Key features:
 * 1. Inherits from Blob and supports all binary data operations, slicing, asynchronous reading, etc.
 * 2. Read-only property name: represents the file name, typically used when displaying, uploading or saving the file.
 * 3. Read-only property lastModified: represents the last modification time of the file (in milliseconds since 1970-01-01 00:00:00 UTC).
 * 4. The constructor requires the file name parameter, otherwise it throws a TypeError.
 * 5. Options such as type, endings and lastModified can be specified at construction; the behavior is consistent with the Web File API.
 *
 * Common usage examples:
 * ```JavaScript
 * // Create File object
 * const file = new File(["hello"], "greeting.txt", { type: "text/plain" });
 *
 * // Read filename and type
 * console.log(file.name); // "greeting.txt"
 * console.log(file.type); // "text/plain"
 *
 * // Get last modified time
 * console.log(file.lastModified);
 *
 * // Slice
 * const part = file.slice(0, 2);
 *
 * // Read content asynchronously
 * file.text().then(txt => console.log(txt));
 * ```
 *
 */
declare class Class_File extends Class_Blob {
    /**
     * !@description File constructor, creates a new File instance. File inherits from Blob and supports all Blob data types.
     *
     *     options supports the following properties:
     *        - type: the MIME type (e.g. "text/plain"), default is an empty string.
     *        - lastModified: the last modification time (timestamp in milliseconds), default is the current time.
     *
     *      @param blobParts the initial data array, may contain strings, ArrayBuffers, TypedArrays, Blobs, etc.
     *      @param name the file name, must be a string and cannot be empty, e.g. "a.txt".
     *      @param options optional parameter object
     *
     */
    constructor(blobParts: any[], name: string, options?: FIBJS.GeneralObject);

    /**
     * !@description File constructor, creates a new File instance. File inherits from Blob and supports all Blob data types.
     *
     *     options supports the following properties:
     *        - type: the MIME type (e.g. "text/plain"), default is an empty string.
     *        - lastModified: the last modification time (timestamp in milliseconds), default is the current time.
     *
     *      @param blobData the initial binary data, can be a Buffer or another binary data type.
     *      @param name the file name, must be a string and cannot be empty, e.g. "a.txt".
     *      @param options optional parameter object
     *
     */
    constructor(blobData: Class_Buffer, name: string, options?: FIBJS.GeneralObject);

    /**
     * !@description File constructor, creates a new File instance. File inherits from Blob and supports all Blob data types.
     *
     *     options supports the following properties:
     *        - data: the initial binary data, can be a Buffer or another binary data type.
     *        - name: the file name, must be a string and cannot be empty, e.g. "a.txt".
     *        - type: the MIME type (e.g. "text/plain"), default is an empty string.
     *        - lastModified: the last modification time (timestamp in milliseconds), default is the current time.
     *
     *      @param options optional parameter object
     *
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * !@description File name, read-only property, returns the name of the file.
     *
     *      This property identifies the file, and is typically used when displaying, uploading or saving it.
     *
     */
    readonly name: string;

    /**
     * !@description Last modification timestamp, read-only property, returns the last modification time of the file (in milliseconds).
     *
     *      This property represents the last modification time of the file, in milliseconds since 1970-01-01 00:00:00 UTC.
     *
     */
    readonly lastModified: number;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the File class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_FilePromise extends Class_BlobPromise {
    /**
     * !@description File constructor, creates a new File instance. File inherits from Blob and supports all Blob data types.
     *
     *     options supports the following properties:
     *        - type: the MIME type (e.g. "text/plain"), default is an empty string.
     *        - lastModified: the last modification time (timestamp in milliseconds), default is the current time.
     *
     *      @param blobParts the initial data array, may contain strings, ArrayBuffers, TypedArrays, Blobs, etc.
     *      @param name the file name, must be a string and cannot be empty, e.g. "a.txt".
     *      @param options optional parameter object
     *
     */
    constructor(blobParts: any[], name: string, options?: FIBJS.GeneralObject);

    /**
     * !@description File constructor, creates a new File instance. File inherits from Blob and supports all Blob data types.
     *
     *     options supports the following properties:
     *        - type: the MIME type (e.g. "text/plain"), default is an empty string.
     *        - lastModified: the last modification time (timestamp in milliseconds), default is the current time.
     *
     *      @param blobData the initial binary data, can be a Buffer or another binary data type.
     *      @param name the file name, must be a string and cannot be empty, e.g. "a.txt".
     *      @param options optional parameter object
     *
     */
    constructor(blobData: Class_Buffer, name: string, options?: FIBJS.GeneralObject);

    /**
     * !@description File constructor, creates a new File instance. File inherits from Blob and supports all Blob data types.
     *
     *     options supports the following properties:
     *        - data: the initial binary data, can be a Buffer or another binary data type.
     *        - name: the file name, must be a string and cannot be empty, e.g. "a.txt".
     *        - type: the MIME type (e.g. "text/plain"), default is an empty string.
     *        - lastModified: the last modification time (timestamp in milliseconds), default is the current time.
     *
     *      @param options optional parameter object
     *
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * !@description File name, read-only property, returns the name of the file.
     *
     *      This property identifies the file, and is typically used when displaying, uploading or saving it.
     *
     */
    readonly name: string;

    /**
     * !@description Last modification timestamp, read-only property, returns the last modification time of the file (in milliseconds).
     *
     *      This property represents the last modification time of the file, in milliseconds since 1970-01-01 00:00:00 UTC.
     *
     */
    readonly lastModified: number;

}


declare namespace Class_File {
    const promises: FIBJS.GeneralObject;
}
