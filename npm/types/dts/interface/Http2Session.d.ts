/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Http2Stream.d.ts" />
/**
 * @description Http2Session represents an active HTTP/2 session, managing the connection and all streams
 *
 * Http2Session instances should not be constructed directly by users. The server creates one when receiving a new HTTP/2 connection. Clients use http2.connect() to create a session.
 *
 */
declare class Class_Http2Session extends Class_EventEmitter {
    /**
     * @description queries the remote settings of this session
     */
    readonly remoteSettings: {
        headerTableSize: number;
        enablePush: boolean;
        maxConcurrentStreams: number;
        initialWindowSize: number;
        maxFrameSize: number;
        maxHeaderListSize: number;
    };

    /**
     * @description queries the local settings of this session
     */
    readonly localSettings: {
        headerTableSize: number;
        enablePush: boolean;
        maxConcurrentStreams: number;
        initialWindowSize: number;
        maxFrameSize: number;
        maxHeaderListSize: number;
    };

    /**
     * @description queries whether the session is destroyed
     */
    readonly destroyed: boolean;

    /**
     * @description queries whether the session is closed
     */
    readonly closed: boolean;

    /**
     * @description queries the ALPN protocol negotiated by this session
     */
    readonly alpnProtocol: string;

    /**
     * @description queries the underlying TLSSocket of this session
     */
    readonly socket: Class_Stream;

    /**
     * @description initiates a new HTTP/2 stream to send a request (client only)
     *      @param headers an object containing request headers; must contain the :method and :path pseudo-headers
     *      @param options optional stream creation options
     *      @return returns the Http2Stream object of the new request
     *
     */
    request(headers: FIBJS.GeneralObject, options?: FIBJS.GeneralObject): Class_Http2Stream;

    /**
     * @description sends a GOAWAY frame to the remote end and gracefully closes the session
     *      @param code HTTP/2 error code, default is NGHTTP2_NO_ERROR (0)
     *      @param lastStreamId the last locally processed stream ID, default is 0
     *
     */
    goaway(code?: number, lastStreamId?: number): void;

    /**
     * @description sends a PING frame to the remote end
     *      @return returns the round-trip time (milliseconds)
     *
     */
    ping(): number;

    ping(callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description sends a PING frame to the remote end
     *      @return returns the round-trip time (milliseconds)
     *
     */
    pingSync(): number;

    /**
     * @description sends a PING frame to the remote end
     *      @return returns the round-trip time (milliseconds)
     *
     */
    pingAsync(): Promise<number>;

    /**
     * @description updates the local settings of this session
     *      @param settings an object containing the settings to update
     *
     */
    settings(settings: FIBJS.GeneralObject): void;

    /**
     * @description gracefully closes the session, allowing existing streams to complete
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description gracefully closes the session, allowing existing streams to complete
     */
    closeSync(): void;

    /**
     * @description gracefully closes the session, allowing existing streams to complete
     */
    closeAsync(): Promise<void>;

    /**
     * @description immediately destroys the session, aborting all streams
     */
    destroy(): void;

    /**
     * @description emitted when a new stream is created (server)
     *      @param stream the newly created Http2Stream
     *      @param headers request headers object
     *
     */
    on(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void): this;

    once(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void): this;

    off(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void): this;

    addListener(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "stream", listener: (stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void): this;

    /**
     * @description emitted when a new stream is created (server)
     *      @param stream the newly created Http2Stream
     *      @param headers request headers object
     *
     */
    onstream: ((stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description emitted when the session receives a GOAWAY frame
     */
    on(event: "goaway", listener: ()=>void): this;

    once(event: "goaway", listener: ()=>void): this;

    off(event: "goaway", listener: ()=>void): this;

    addListener(event: "goaway", listener: ()=>void): this;

    removeListener(event: "goaway", listener: ()=>void): this;

    addEventListener(event: "goaway", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "goaway", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "goaway", listener: ()=>void): this;

    prependOnceListener(event: "goaway", listener: ()=>void): this;

    /**
     * @description emitted when the session receives a GOAWAY frame
     */
    ongoaway: (()=>void) | null;

    /**
     * @description emitted when an error occurs on the session
     *      @param err error object
     *
     */
    on(event: "error", listener: (err: FIBJS.GeneralObject)=>void): this;

    once(event: "error", listener: (err: FIBJS.GeneralObject)=>void): this;

    off(event: "error", listener: (err: FIBJS.GeneralObject)=>void): this;

    addListener(event: "error", listener: (err: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "error", listener: (err: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "error", listener: (err: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (err: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: (err: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "error", listener: (err: FIBJS.GeneralObject)=>void): this;

    /**
     * @description emitted when an error occurs on the session
     *      @param err error object
     *
     */
    onerror: ((err: FIBJS.GeneralObject)=>void) | null;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Http2Stream.d.ts" />
/**
 * The promise variant of the Http2Session class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_Http2SessionPromise extends Class_EventEmitter {
    /**
     * @description queries the remote settings of this session
     */
    readonly remoteSettings: {
        headerTableSize: number;
        enablePush: boolean;
        maxConcurrentStreams: number;
        initialWindowSize: number;
        maxFrameSize: number;
        maxHeaderListSize: number;
    };

    /**
     * @description queries the local settings of this session
     */
    readonly localSettings: {
        headerTableSize: number;
        enablePush: boolean;
        maxConcurrentStreams: number;
        initialWindowSize: number;
        maxFrameSize: number;
        maxHeaderListSize: number;
    };

    /**
     * @description queries whether the session is destroyed
     */
    readonly destroyed: boolean;

    /**
     * @description queries whether the session is closed
     */
    readonly closed: boolean;

    /**
     * @description queries the ALPN protocol negotiated by this session
     */
    readonly alpnProtocol: string;

    /**
     * @description queries the underlying TLSSocket of this session
     */
    readonly socket: Class_StreamPromise;

    /**
     * @description initiates a new HTTP/2 stream to send a request (client only)
     *      @param headers an object containing request headers; must contain the :method and :path pseudo-headers
     *      @param options optional stream creation options
     *      @return returns the Http2Stream object of the new request
     *
     */
    request(headers: FIBJS.GeneralObject, options?: FIBJS.GeneralObject): Class_Http2Stream;

    /**
     * @description sends a GOAWAY frame to the remote end and gracefully closes the session
     *      @param code HTTP/2 error code, default is NGHTTP2_NO_ERROR (0)
     *      @param lastStreamId the last locally processed stream ID, default is 0
     *
     */
    goaway(code?: number, lastStreamId?: number): void;

    /**
     * @description sends a PING frame to the remote end
     *      @return returns the round-trip time (milliseconds)
     *
     */
    ping(): Promise<number>;

    /**
     * @description sends a PING frame to the remote end
     *      @return returns the round-trip time (milliseconds)
     *
     */
    pingSync(): number;

    /**
     * @description sends a PING frame to the remote end
     *      @return returns the round-trip time (milliseconds)
     *
     */
    pingAsync(): Promise<number>;

    /**
     * @description updates the local settings of this session
     *      @param settings an object containing the settings to update
     *
     */
    settings(settings: FIBJS.GeneralObject): void;

    /**
     * @description gracefully closes the session, allowing existing streams to complete
     */
    close(): Promise<void>;

    /**
     * @description gracefully closes the session, allowing existing streams to complete
     */
    closeSync(): void;

    /**
     * @description gracefully closes the session, allowing existing streams to complete
     */
    closeAsync(): Promise<void>;

    /**
     * @description immediately destroys the session, aborting all streams
     */
    destroy(): void;

    /**
     * @description emitted when a new stream is created (server)
     *      @param stream the newly created Http2Stream
     *      @param headers request headers object
     *
     */
    onstream: ((stream: Class_Http2Stream, headers: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description emitted when the session receives a GOAWAY frame
     */
    ongoaway: (()=>void) | null;

    /**
     * @description emitted when an error occurs on the session
     *      @param err error object
     *
     */
    onerror: ((err: FIBJS.GeneralObject)=>void) | null;

}


declare namespace Class_Http2Session {
    const promises: FIBJS.GeneralObject;
}
