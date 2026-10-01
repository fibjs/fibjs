/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Http2Server.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Http2Session.d.ts" />
/// <reference path="../interface/Http2Stream.d.ts" />
/// <reference path="../module/http2_constants.d.ts" />
/**
 * @description the http2 module provides HTTP/2 protocol support
 *
 * The http2 module allows creating HTTP/2 servers and clients, with full support for stream multiplexing, header compression and flow control.
 *
 * ```JavaScript
 * const http2 = require('http2');
 *
 * // client example
 * const session = http2.connect('https://example.com');
 * const stream = session.request({ ':path': '/' });
 * const response = stream.read();
 * session.close();
 *
 * // server example
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
declare module 'http2' {
    /**
     * @description creates an Http2Server object, see Http2Server
     */
    const Server: typeof Class_Http2Server;

    /**
     * @description creates an Http2 server
     *      @param options TLS options object or SecureContext configuration
     *      @param hdlr the request handling function
     *      @return returns an Http2Server object; call listen() then start() to start serving
     *
     */
    function createServer(options: FIBJS.GeneralObject, hdlr: Class_Handler): Class_Http2Server;

    /**
     * @description creates an Http2 server
     *      @param context SecureContext object used for TLS configuration
     *      @param hdlr the request handling function
     *      @return returns an Http2Server object; call listen() then start() to start serving
     *
     */
    function createServer(context: Class_SecureContext, hdlr: Class_Handler): Class_Http2Server;

    /**
     * @description creates an HTTP/2 client session to the specified target
     *
     *      authority should be a URL string such as 'https://example.com' or 'https://example.com:8443'.
     *
     *      options may contain:
     *      - all SecureContext options (key, cert, ca, etc.)
     *
     *      @param authority URL of the server to connect
     *      @param options connection options
     *      @return returns the Http2Session client session
     *
     */
    function connect(authority: string, options?: FIBJS.GeneralObject): Class_Http2Session;

    function connect(authority: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Http2Session)=>any): void;

    /**
     * @description creates an HTTP/2 client session to the specified target
     *
     *      authority should be a URL string such as 'https://example.com' or 'https://example.com:8443'.
     *
     *      options may contain:
     *      - all SecureContext options (key, cert, ca, etc.)
     *
     *      @param authority URL of the server to connect
     *      @param options connection options
     *      @return returns the Http2Session client session
     *
     */
    function connectSync(authority: string, options?: FIBJS.GeneralObject): Class_Http2Session;

    /**
     * @description creates an HTTP/2 client session to the specified target
     *
     *      authority should be a URL string such as 'https://example.com' or 'https://example.com:8443'.
     *
     *      options may contain:
     *      - all SecureContext options (key, cert, ca, etc.)
     *
     *      @param authority URL of the server to connect
     *      @param options connection options
     *      @return returns the Http2Session client session
     *
     */
    function connectAsync(authority: string, options?: FIBJS.GeneralObject): Promise<Class_Http2Session>;

    /**
     * @description returns the default HTTP/2 settings object
     *      @return returns an object containing the default settings
     *
     */
    function getDefaultSettings(): FIBJS.GeneralObject;

    /**
     * @description Http2Stream object, see Http2Stream
     */
    const Http2Stream: typeof Class_Http2Stream;

    /**
     * @description Http2Session object, see Http2Session
     */
    const Http2Session: typeof Class_Http2Session;

    /**
     * @description the constants object of the http2 module, see http2_constants
     */
    const constants: typeof import ('http2_constants');

}

