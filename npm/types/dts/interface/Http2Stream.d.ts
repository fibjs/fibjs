/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
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
    on(event: "headers", listener: ()=>void): this;

    /**
     * @description emitted when trailing headers are received
     *      @param headers trailing headers object
     *
     */
    on(event: "trailers", listener: ()=>void): this;

}

