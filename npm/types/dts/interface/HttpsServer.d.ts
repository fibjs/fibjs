/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
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
     *     @param context the SecureContext secure context
     *     @param port specifies the port on which the http server listens
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object
     *
     */
    constructor(context: Class_SecureContext, port: number, hdlr: Class_Handler);

    /**
     * @description HttpsServer constructor
     *      @param context the SecureContext secure context
     *      @param addr specifies the address on which the http server listens; if "" it listens on all local addresses
     *      @param port specifies the port on which the http server listens
     *      @param hdlr the http built-in message handler: a handler function, chained handling array or routing object
     *
     */
    constructor(context: Class_SecureContext, addr: string, port: number, hdlr: Class_Handler);

    /**
     * @description HttpsServer constructor, listens on all local addresses
     *
     *      In addition to the properties used to create a SecureContext, options can also provide the following properties:
     *      - address: specifies the listening address, optional, by default listens on all addresses
     *      - port: specifies the listening port, optional; when not provided, listen() must be called to start
     *
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param hdlr the http built-in message handler: a handler function, chained handling array or routing object
     *
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler);

    /**
     * @description HttpsServer constructor, does not bind a port; listen() must be called to start
     *      @param context the SecureContext secure context
     *      @param hdlr the http built-in message handler: a handler function, chained handling array or routing object
     *
     */
    constructor(context: Class_SecureContext, hdlr: Class_Handler);

    /**
     * @description queries the SecureContext used by the current HttpsServer
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description sets the SecureContext used by the current HttpsServer
     *     @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext): void;

    /**
     * @description sets the SecureContext used by the current HttpsServer
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}

