/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/// <reference path="../interface/Headers.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FormData.d.ts" />
/**
 * @description http base message object
 */
declare class Class_HttpMessage extends Class_Message {
    /**
     * @description protocol version information, the allowed format is: HTTP/#.#
     */
    protocol: string;

    /**
     * @description container holding the http headers of the message, read-only property
     */
    readonly headers: Class_Headers;

    /**
     * @description queries and sets whether to keep the connection alive
     */
    keepAlive: boolean;

    /**
     * @description queries and sets whether the protocol is upgraded
     */
    upgrade: boolean;

    /**
     * @description queries and sets the maximum number of request headers, default is 128
     */
    maxHeadersCount: number;

    /**
     * @description queries and sets the maximum request header length, default is 8192
     */
    maxHeaderSize: number;

    /**
     * @description queries and sets the maximum chunk size in MB, default is 2
     */
    maxChunkSize: number;

    /**
     * @description queries and sets the maximum body size in MB, default is 64
     */
    maxBodySize: number;

    /**
     * @description queries the source socket of the current object
     */
    readonly socket: Class_Stream;

    /**
     * @description checks whether a header of the specified key exists
     *      @param name specifies the key to check
     *      @return returns whether the key exists
     *
     */
    hasHeader(name: string): boolean;

    /**
     * @description queries the first header of the specified key
     *      @param name specifies the key to query
     *      @return returns the value corresponding to the key, or undefined if it does not exist
     *
     */
    firstHeader(name: string): string;

    /**
     * @description queries all headers of the specified key
     *      @param name specifies the key to query; passing an empty string returns the result of all keys
     *      @return returns an array of all values corresponding to the key, or null if the data does not exist
     *
     */
    allHeader(name?: string): FIBJS.GeneralObject;

    /**
     * @description appends a header; appending data does not modify the headers of an existing key
     *      @param map specifies the key-value data dictionary to append
     *
     */
    appendHeader(map: FIBJS.GeneralObject): void;

    /**
     * @description appends headers; appending data does not modify the headers of an existing key
     *      @param headers specifies the Headers object to append
     *
     */
    appendHeader(headers: Class_Headers): void;

    /**
     * @description appends a group of headers with the specified name; appending data does not modify the headers of an existing key
     *      @param name specifies the key to append
     *      @param values specifies the group of data to append
     *
     */
    appendHeader(name: string, values: any[]): void;

    /**
     * @description appends a header; appending data does not modify the headers of an existing key
     *      @param name specifies the key to append
     *      @param value specifies the data to append
     *
     */
    appendHeader(name: string, value: string): void;

    /**
     * @description sets a header; setting data modifies the first value of the key and clears the remaining headers with the same key
     *      @param map specifies the key-value data dictionary to set
     *
     */
    setHeader(map: FIBJS.GeneralObject): void;

    /**
     * @description sets headers; setting data modifies the value of the key and clears the remaining headers with the same key
     *      @param headers specifies the Headers object to set
     *
     */
    setHeader(headers: Class_Headers): void;

    /**
     * @description sets a group of headers with the specified name; setting data modifies the value of the key and clears the remaining headers with the same key
     *      @param name specifies the key to set
     *      @param values specifies the group of data to set
     *
     */
    setHeader(name: string, values: any[]): void;

    /**
     * @description sets a header; setting data modifies the first value of the key and clears the remaining headers with the same key
     *      @param name specifies the key to set
     *      @param value specifies the data to set
     *
     */
    setHeader(name: string, value: string): void;

    /**
     * @description deletes all headers of the specified key
     *      @param name specifies the key to delete
     *
     */
    removeHeader(name: string): void;

    /**
     * @description queries the first header of the specified key
     *      @param name specifies the key to query
     *      @return returns the value corresponding to the key, or undefined if it does not exist
     *
     */
    getHeader(name: string): any;

    /**
     * @description queries all headers
     *      @return returns the key-value pairs of all headers
     *
     */
    getHeaders(): FIBJS.GeneralObject;

    /**
     * @description queries whether the headers have been sent
     */
    readonly headersSent: boolean;

    /**
     * @description container holding the http trailer headers of the message, read-only property
     */
    readonly trailers: Class_Headers;

    /**
     * @description adds trailer headers, which will be sent after the body
     *      @param headers specifies the trailer headers to add
     *
     */
    addTrailers(headers: FIBJS.GeneralObject): void;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; other types throw a TypeError.
     *
     *      @return returns the parsed FormData object
     *
     */
    formData(): Class_FormData;

    formData(callback: (err: Error | undefined | null, retVal: Class_FormData)=>any): void;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; other types throw a TypeError.
     *
     *      @return returns the parsed FormData object
     *
     */
    formDataSync(): Class_FormData;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; other types throw a TypeError.
     *
     *      @return returns the parsed FormData object
     *
     */
    formDataAsync(): Promise<Class_FormData>;

}

