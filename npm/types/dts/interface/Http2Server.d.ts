/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description Http2Server is a high-concurrency HTTP/2 server
 *
 * Http2Server handles HTTP/2 connections over TLS (h2). When a client connects, the server creates an Http2Session for each connection and emits a 'stream' event for each request.
 *
 * ```JavaScript
 * const http2 = require('http2');
 *
 * const server = http2.createServer({
 *     key: ...,
 *     cert: ...
 * }, function(req) {
 *     req.response.write('Hello, HTTP/2!');
 * });
 * server.listen(8443);
 * server.start();
 * ```
 *
 */
declare class Class_Http2Server extends Class_TcpServer {
    /**
     * @description Http2Server constructor
     *      @param context SecureContext secure context
     *      @param hdlr http built-in message handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description Http2Server constructor
     *      @param context SecureContext secure context
     *      @param port listening port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description Http2Server constructor
     *      @param context SecureContext secure context
     *      @param addr listening address
     *      @param port listening port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description Http2Server constructor, creates the SecureContext from options
     *      @param options the options for creating the SecureContext, may contain address and port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description queries the SecureContext used by the current Http2Server
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description sets the SecureContext used by the current Http2Server
     *      @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description sets the SecureContext used by the current Http2Server
     *      @param options the options for creating a new SecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the Http2Server class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_Http2ServerPromise extends Class_TcpServerPromise {
    /**
     * @description Http2Server constructor
     *      @param context SecureContext secure context
     *      @param hdlr http built-in message handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description Http2Server constructor
     *      @param context SecureContext secure context
     *      @param port listening port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description Http2Server constructor
     *      @param context SecureContext secure context
     *      @param addr listening address
     *      @param port listening port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description Http2Server constructor, creates the SecureContext from options
     *      @param options the options for creating the SecureContext, may contain address and port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description queries the SecureContext used by the current Http2Server
     */
    readonly secureContext: Class_SecureContextPromise;

    /**
     * @description sets the SecureContext used by the current Http2Server
     *      @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description sets the SecureContext used by the current Http2Server
     *      @param options the options for creating a new SecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


declare namespace Class_Http2Server {
    const promises: FIBJS.GeneralObject;
}
