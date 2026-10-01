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
    constructor(context: Class_SecureContext, hdlr: Class_Handler);

    /**
     * @description Http2Server constructor
     *      @param context SecureContext secure context
     *      @param port listening port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(context: Class_SecureContext, port: number, hdlr: Class_Handler);

    /**
     * @description Http2Server constructor
     *      @param context SecureContext secure context
     *      @param addr listening address
     *      @param port listening port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(context: Class_SecureContext, addr: string, port: number, hdlr: Class_Handler);

    /**
     * @description Http2Server constructor, creates the SecureContext from options
     *      @param options the options for creating the SecureContext, may contain address and port
     *      @param hdlr http built-in message handler
     *
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler);

    /**
     * @description queries the SecureContext used by the current Http2Server
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description sets the SecureContext used by the current Http2Server
     *      @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext): void;

    /**
     * @description sets the SecureContext used by the current Http2Server
     *      @param options the options for creating a new SecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}

