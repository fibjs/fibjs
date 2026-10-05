/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description HttpsServer is the object used to create an https server; an HttpsServer object can use all the interface functions and properties of HttpServer. An HttpsServer object can be provided with a certificate object (X509Cert type) and a key object (PKey type) generated earlier with openssl when it is created, thereby providing tls/ssl encrypted services for clients
 *
 *  The https server object combines and encapsulates TLSServer and HttpHandler to make it easy to build a server quickly; logically, it is equivalent to:
 *  ```JavaScript
 *  var svr = new tls.Server({
 *         crt,
 *         key
 *     }, addr, port, new http.Handler(function(req){
 *     ...
 *  }));
 *  ```
 *
 * The following is a sample code using HttpsServer:
 * ```JavaScript
 * const http = require("http");
 *
 * // create https server
 * const server = new http.HttpsServer({
 *         cert,
 *         key
 *     }, 8443, function(req) {
 *     resp.response.write(`Hello, Fibjs!`);
 * });
 * server.start();
 * ```
 *
 * In the example above, we load a certificate file named "server.crt" and a private key file named "server.key", then create a service using the HttpsServer object and start listening on port 8443; when a client accesses the service via "https://localhost:8443/", it is protected by ssl encryption.
 *
 * Note that if external access is required, the certificate must be issued by a trusted authority; otherwise clients cannot verify it, which reduces performance and security and may trigger security warnings.
 *
 */
declare class Class_HttpsServer extends Class_HttpServer {
    /**
     * @description HttpsServer constructor, listens on all local addresses
     *
     *     hdlr may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *     - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *     @param context the SecureContext secure context
     *     @param port specifies the port on which the http server listens
     *     @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description HttpsServer constructor
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *      - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *      @param context the SecureContext secure context
     *      @param addr specifies the address on which the http server listens; if "" it listens on all local addresses
     *      @param port specifies the port on which the http server listens
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description HttpsServer constructor, listens on all local addresses
     *
     *      In addition to the properties used to create a SecureContext, options can also provide the following properties:
     *      - address: specifies the listening address, optional, by default listens on all addresses
     *      - port: specifies the listening port, optional; when not provided, listen() must be called to start
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *      - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param hdlr the request handler
     *
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description HttpsServer constructor, does not bind a port; listen() must be called to start
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *      - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *      @param context the SecureContext secure context
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description queries the SecureContext used by the current HttpsServer
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description sets the SecureContext used by the current HttpsServer
     *     @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description sets the SecureContext used by the current HttpsServer
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the HttpsServer class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpsServerPromise extends Class_HttpServerPromise {
    /**
     * @description HttpsServer constructor, listens on all local addresses
     *
     *     hdlr may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *     - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *     @param context the SecureContext secure context
     *     @param port specifies the port on which the http server listens
     *     @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description HttpsServer constructor
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *      - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *      @param context the SecureContext secure context
     *      @param addr specifies the address on which the http server listens; if "" it listens on all local addresses
     *      @param port specifies the port on which the http server listens
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description HttpsServer constructor, listens on all local addresses
     *
     *      In addition to the properties used to create a SecureContext, options can also provide the following properties:
     *      - address: specifies the listening address, optional, by default listens on all addresses
     *      - port: specifies the listening port, optional; when not provided, listen() must be called to start
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *      - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param hdlr the request handler
     *
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description HttpsServer constructor, does not bind a port; listen() must be called to start
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *      - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *      @param context the SecureContext secure context
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description queries the SecureContext used by the current HttpsServer
     */
    readonly secureContext: Class_SecureContextPromise;

    /**
     * @description sets the SecureContext used by the current HttpsServer
     *     @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description sets the SecureContext used by the current HttpsServer
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


declare namespace Class_HttpsServer {
    const promises: FIBJS.GeneralObject;
}
