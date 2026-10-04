/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Http2Stream is the object representing a single HTTP/2 stream in an Http2Session
 *
 * Each Http2Stream instance is a duplex stream. Data written to the stream is sent as DATA frames, and received data is provided as DATA frames from the remote end.
 *
 * Http2Stream instances are created by Http2Session and should not be constructed directly.
 *
 */
declare class Class_Http2Stream extends Class_Stream {
    /**
     * @description queries the numeric stream identifier of this Http2Stream instance
     */
    readonly id: number;

    /**
     * @description queries whether this Http2Stream instance is closed
     */
    readonly closed: boolean;

    /**
     * @description queries whether this Http2Stream instance is destroyed
     */
    readonly destroyed: boolean;

    /**
     * @description queries the headers object received by this stream
     */
    readonly headers: FIBJS.GeneralObject;

    /**
     * @description sends response headers to the remote end
     *      @param headers an object containing header name-value pairs
     *
     */
    respond(headers?: FIBJS.GeneralObject): void;

    /**
     * @description sends additional informational (1xx) headers to the remote end
     *      @param headers an object containing header name-value pairs
     *
     */
    additionalHeaders(headers: FIBJS.GeneralObject): void;

    /**
     * @description sends trailing headers to the remote end, marking the end of the stream
     *      @param headers an object containing trailing header name-value pairs
     *
     */
    sendTrailers(headers: FIBJS.GeneralObject): void;

    /**
     * @description sends an RST_STREAM frame to the remote end, closing the stream
     *      @param code RST_STREAM error code, default is NGHTTP2_NO_ERROR (0)
     *
     */
    rstStream(code?: number): void;

    /**
     * @description emitted when response headers are received (client)
     *      @param headers response headers object
     *
     */
    on(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    once(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    off(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    addListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    /**
     * @description emitted when response headers are received (client)
     *      @param headers response headers object
     *
     */
    onheaders: ((headers: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description emitted when trailing headers are received
     *      @param headers trailing headers object
     *
     */
    on(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    once(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    off(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    addListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    /**
     * @description emitted when trailing headers are received
     *      @param headers trailing headers object
     *
     */
    ontrailers: ((headers: FIBJS.GeneralObject)=>void) | null;

    on(event: "data", listener: (data: Class_Buffer)=>void): this;

    on(event: "close", listener: ()=>void): this;

    on(event: "error", listener: (code: number)=>void): this;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(event: "data", listener: (data: Class_Buffer)=>void): this;

    once(event: "close", listener: ()=>void): this;

    once(event: "error", listener: (code: number)=>void): this;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(event: "data", listener: (data: Class_Buffer)=>void): this;

    off(event: "close", listener: ()=>void): this;

    off(event: "error", listener: (code: number)=>void): this;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    addListener(event: "error", listener: (code: number)=>void): this;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    removeListener(event: "error", listener: (code: number)=>void): this;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependListener(event: "error", listener: (code: number)=>void): this;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "error", listener: (code: number)=>void): this;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * The promise variant of the Http2Stream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_Http2StreamPromise extends Class_StreamPromise {
    /**
     * @description queries the numeric stream identifier of this Http2Stream instance
     */
    readonly id: number;

    /**
     * @description queries whether this Http2Stream instance is closed
     */
    readonly closed: boolean;

    /**
     * @description queries whether this Http2Stream instance is destroyed
     */
    readonly destroyed: boolean;

    /**
     * @description queries the headers object received by this stream
     */
    readonly headers: FIBJS.GeneralObject;

    /**
     * @description sends response headers to the remote end
     *      @param headers an object containing header name-value pairs
     *
     */
    respond(headers?: FIBJS.GeneralObject): void;

    /**
     * @description sends additional informational (1xx) headers to the remote end
     *      @param headers an object containing header name-value pairs
     *
     */
    additionalHeaders(headers: FIBJS.GeneralObject): void;

    /**
     * @description sends trailing headers to the remote end, marking the end of the stream
     *      @param headers an object containing trailing header name-value pairs
     *
     */
    sendTrailers(headers: FIBJS.GeneralObject): void;

    /**
     * @description sends an RST_STREAM frame to the remote end, closing the stream
     *      @param code RST_STREAM error code, default is NGHTTP2_NO_ERROR (0)
     *
     */
    rstStream(code?: number): void;

    /**
     * @description emitted when response headers are received (client)
     *      @param headers response headers object
     *
     */
    onheaders: ((headers: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description emitted when trailing headers are received
     *      @param headers trailing headers object
     *
     */
    ontrailers: ((headers: FIBJS.GeneralObject)=>void) | null;

}


declare namespace Class_Http2Stream {
    const promises: FIBJS.GeneralObject;
}
