/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Http2Server.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Http2Session.d.ts" />
/// <reference path="../interface/Http2Stream.d.ts" />
/// <reference path="../module/http2_constants.d.ts" />
/**
 * The promise variant of the http2 module: async members return a Promise as their primary form.
 */
declare module 'http2/promises' {
    /**
     * @description creates an Http2Server object, see Http2Server
     */
    const Server: typeof Class_Http2Server;

    /**
     * @description creates an Http2 server
     *
     *      options may be the TLS options object, used to create the SecureContext with
     *      tls.createSecureContext, or the SecureContext object itself.
     *
     *      hdlr accepts the same forms as http.createServer:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *      - a path/address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *      @param options the TLS options or the SecureContext object
     *      @param hdlr the request handler
     *      @return returns an Http2Server object; call listen() then start() to start serving
     *
     */
    function createServer(options: FIBJS.GeneralObject | Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string): Class_Http2Server;

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
    function connect(authority: string, options?: FIBJS.GeneralObject): Promise<Class_Http2SessionPromise>;

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
    function connectAsync(authority: string, options?: FIBJS.GeneralObject): Promise<Class_Http2SessionPromise>;

    /**
     * @description returns the default HTTP/2 settings object
     *      @return returns an object containing the default settings
     *
     */
    function getDefaultSettings(): {
        headerTableSize: number;
        enablePush: boolean;
        maxConcurrentStreams: number;
        initialWindowSize: number;
        maxFrameSize: number;
        maxHeaderListSize: number;
    };

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


declare module "http2" {
    const promises: typeof import("http2/promises");
}
