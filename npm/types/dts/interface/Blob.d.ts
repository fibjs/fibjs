/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The Blob object represents an immutable raw data block, compatible with the Web standard Blob API.
 *
 * Blob can be used to store binary data, text, images, etc., and is commonly used for file upload, data processing and similar scenarios. Blob supports concatenation, slicing and reading of multiple data types and is widely used in modules such as Web, HTTP and the file system.
 *
 * Key features:
 * 1. Supports flexible construction via arrays and options objects; data types can be strings, ArrayBuffers, TypedArrays, Blobs, etc.
 * 2. Supports read-only properties such as type and size, making it easy to get the data type and size.
 * 3. Supports the slice method for efficient slicing and supports type conversion.
 * 4. Supports asynchronous reading as text or binary.
 *
 * Common usage examples:
 * ```JavaScript
 * // Create empty Blob
 * const blob = new Blob();
 *
 * // Create Blob containing string and binary
 * const blob = new Blob(["hello", new Uint8Array([1,2,3])], { type: "text/plain" });
 *
 * // Slice
 * const part = blob.slice(0, 5);
 *
 * // Read text content
 * blob.text().then(txt => console.log(txt));
 *
 * // Read binary content
 * blob.arrayBuffer().then(buf => ...);
 * ```
 *
 */
declare class Class_Blob extends Class_object {
    /**
     * @description Blob object constructor
     *
     *      Creates a new Blob instance with the specified data content and type.
     *      @param blobParts the initial data array, may contain strings, ArrayBuffers, TypedArrays, Blobs, etc.
     *      @param options the options object, containing the type (MIME type) and endings (newline handling) properties
     *
     */
    constructor(blobParts?: any[], options?: FIBJS.GeneralObject);

    /**
     * @description Blob object constructor; a string blobData is encoded as utf8
     *
     *      Creates a new Blob instance with the specified data content and type.
     *      blobData may be a Buffer or another binary data type; a string is encoded as utf8.
     *      @param blobData the initial binary data
     *      @param options the options object, containing the type (MIME type) and endings (newline handling) properties
     *
     */
    constructor(blobData: Class_Buffer | string, options?: FIBJS.GeneralObject);

    /**
     * @description The type of the Blob object, returns the MIME type of the Blob (e.g. "text/plain", "image/png", etc.), read-only property.
     *
     */
    readonly type: string;

    /**
     * @description The size of the Blob object, returns the number of bytes of the Blob data, read-only property.
     *
     */
    readonly size: number;

    /**
     * @description Returns a Blob slice of the specified range
     *
     *      Creates a new Blob containing the specified range of the original data.
     *
     *      @param start the start position (in bytes, default 0)
     *      @param end the end position (in bytes, default -1, meaning to the end)
     *      @param contentType the MIME type of the new Blob (optional)
     *      @return returns the new Blob object
     *
     */
    slice(start?: number, end?: number, contentType?: string): Class_Blob;

    /**
     * @description Reads the Blob content as text
     *
     *      Asynchronously reads the Blob data as a string, returns a Promise.
     *      @return returns a Promise containing the text content
     *
     */
    text(): Promise<string>;

    /**
     * @description Reads the Blob content as text
     *
     *      Asynchronously reads the Blob data as a string, returns a Promise.
     *      @return returns a Promise containing the text content
     *
     */
    textSync(): string;

    /**
     * @description Reads the Blob content as text
     *
     *      Asynchronously reads the Blob data as a string, returns a Promise.
     *      @return returns a Promise containing the text content
     *
     */
    textAsync(): Promise<string>;

    /**
     * @description Reads the Blob content as an ArrayBuffer
     *
     *      Asynchronously reads the Blob data as an ArrayBuffer, returns a Promise.
     *      @return returns a Promise containing the binary data
     *
     */
    arrayBuffer(): Promise<ArrayBuffer>;

    /**
     * @description Reads the Blob content as an ArrayBuffer
     *
     *      Asynchronously reads the Blob data as an ArrayBuffer, returns a Promise.
     *      @return returns a Promise containing the binary data
     *
     */
    arrayBufferSync(): ArrayBuffer;

    /**
     * @description Reads the Blob content as an ArrayBuffer
     *
     *      Asynchronously reads the Blob data as an ArrayBuffer, returns a Promise.
     *      @return returns a Promise containing the binary data
     *
     */
    arrayBufferAsync(): Promise<ArrayBuffer>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the Blob class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_BlobPromise extends Class_object {
    /**
     * @description Blob object constructor
     *
     *      Creates a new Blob instance with the specified data content and type.
     *      @param blobParts the initial data array, may contain strings, ArrayBuffers, TypedArrays, Blobs, etc.
     *      @param options the options object, containing the type (MIME type) and endings (newline handling) properties
     *
     */
    constructor(blobParts?: any[], options?: FIBJS.GeneralObject);

    /**
     * @description Blob object constructor; a string blobData is encoded as utf8
     *
     *      Creates a new Blob instance with the specified data content and type.
     *      blobData may be a Buffer or another binary data type; a string is encoded as utf8.
     *      @param blobData the initial binary data
     *      @param options the options object, containing the type (MIME type) and endings (newline handling) properties
     *
     */
    constructor(blobData: Class_Buffer | string, options?: FIBJS.GeneralObject);

    /**
     * @description The type of the Blob object, returns the MIME type of the Blob (e.g. "text/plain", "image/png", etc.), read-only property.
     *
     */
    readonly type: string;

    /**
     * @description The size of the Blob object, returns the number of bytes of the Blob data, read-only property.
     *
     */
    readonly size: number;

    /**
     * @description Returns a Blob slice of the specified range
     *
     *      Creates a new Blob containing the specified range of the original data.
     *
     *      @param start the start position (in bytes, default 0)
     *      @param end the end position (in bytes, default -1, meaning to the end)
     *      @param contentType the MIME type of the new Blob (optional)
     *      @return returns the new Blob object
     *
     */
    slice(start?: number, end?: number, contentType?: string): Class_Blob;

    /**
     * @description Reads the Blob content as text
     *
     *      Asynchronously reads the Blob data as a string, returns a Promise.
     *      @return returns a Promise containing the text content
     *
     */
    text(): Promise<string>;

    /**
     * @description Reads the Blob content as text
     *
     *      Asynchronously reads the Blob data as a string, returns a Promise.
     *      @return returns a Promise containing the text content
     *
     */
    textSync(): string;

    /**
     * @description Reads the Blob content as text
     *
     *      Asynchronously reads the Blob data as a string, returns a Promise.
     *      @return returns a Promise containing the text content
     *
     */
    textAsync(): Promise<string>;

    /**
     * @description Reads the Blob content as an ArrayBuffer
     *
     *      Asynchronously reads the Blob data as an ArrayBuffer, returns a Promise.
     *      @return returns a Promise containing the binary data
     *
     */
    arrayBuffer(): Promise<ArrayBuffer>;

    /**
     * @description Reads the Blob content as an ArrayBuffer
     *
     *      Asynchronously reads the Blob data as an ArrayBuffer, returns a Promise.
     *      @return returns a Promise containing the binary data
     *
     */
    arrayBufferSync(): ArrayBuffer;

    /**
     * @description Reads the Blob content as an ArrayBuffer
     *
     *      Asynchronously reads the Blob data as an ArrayBuffer, returns a Promise.
     *      @return returns a Promise containing the binary data
     *
     */
    arrayBufferAsync(): Promise<ArrayBuffer>;

}


declare namespace Class_Blob {
    const promises: FIBJS.GeneralObject;
}
