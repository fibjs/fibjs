/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description HttpServer is one of the built-in objects; it is the object used to create an HTTP server. An HttpServer object contains two required parameters: a port and an event handling interface object. In the event handling interface object, the concrete implementation can be a simple callback function, or complex routing, a chained handling array, etc.
 *
 *  The http server object combines and encapsulates TcpServer and HttpHandler to make it easy to build a server quickly; logically, it is equivalent to:
 *  ```JavaScript
 *  var svr = new net.TcpServer(addr, port, new http.Handler(function(req){
 *     ...
 *  }));
 *  ```
 *
 * The following is a minimal HttpServer application example; it simply returns the string hello world for all requests.
 * ```JavaScript
 * const http = require('http');
 * var svr = new http.Server(8080, (req) => {
 *     req.response.write('hello, world');
 * });
 * svr.start();
 * ```
 *
 * As can be seen from the code, first we import the built-in http module. Then we create a new HttpServer object and pass two necessary parameters: a port number and a concrete event handling interface object. In this example, we use a simple callback function as the event handling interface to respond to data from HTTP requests. `req.response.write('hello, world')` is used to respond with our string hello world to the client.
 *
 * After creating the HttpServer object, use `svr.start()` to start the server, so that we can receive HTTP requests from the Internet through this server.
 *
 */
declare class Class_HttpServer extends Class_TcpServer {
    /**
     * @description HttpServer constructor, listens on all local addresses
     *     @param port specifies the port on which the http server listens
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
     *
     */
    constructor(port: number, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description HttpServer constructor
     *     @param addr specifies the address on which the http server listens; if "" it listens on all local addresses
     *     @param port specifies the port on which the http server listens
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
     *
     */
    constructor(addr: string, port: number, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description HttpServer constructor
     *     @param addr specifies the address on which the http server listens; if "" it listens on all local addresses
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
     *
     */
    constructor(addr: string, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description HttpServer constructor, does not bind a port; listen() must be called to start
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
     *
     */
    constructor(hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description enables cross-origin requests
     *      @param allowHeaders specifies the accepted http header fields
     *
     */
    enableCrossOrigin(allowHeaders?: string): void;

    /**
     * @description queries and sets the maximum number of request headers, default is 128
     */
    maxHeadersCount: number;

    /**
     * @description queries and sets the maximum request header length, default is 8192
     */
    maxHeaderSize: number;

    /**
     * @description queries and sets the maximum body size in MB, default is 64
     */
    maxBodySize: number;

    /**
     * @description switch for the automatic decompression feature, disabled by default
     */
    enableEncoding: boolean;

    /**
     * @description queries and sets the server name, default is: fibjs/0.x.0
     */
    serverName: string;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the HttpServer class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpServerPromise extends Class_TcpServerPromise {
    /**
     * @description HttpServer constructor, listens on all local addresses
     *     @param port specifies the port on which the http server listens
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
     *
     */
    constructor(port: number, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description HttpServer constructor
     *     @param addr specifies the address on which the http server listens; if "" it listens on all local addresses
     *     @param port specifies the port on which the http server listens
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
     *
     */
    constructor(addr: string, port: number, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description HttpServer constructor
     *     @param addr specifies the address on which the http server listens; if "" it listens on all local addresses
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
     *
     */
    constructor(addr: string, hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description HttpServer constructor, does not bind a port; listen() must be called to start
     *     @param hdlr the http built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
     *
     */
    constructor(hdlr: Class_Handler | Class_HandlerPromise);

    /**
     * @description enables cross-origin requests
     *      @param allowHeaders specifies the accepted http header fields
     *
     */
    enableCrossOrigin(allowHeaders?: string): void;

    /**
     * @description queries and sets the maximum number of request headers, default is 128
     */
    maxHeadersCount: number;

    /**
     * @description queries and sets the maximum request header length, default is 8192
     */
    maxHeaderSize: number;

    /**
     * @description queries and sets the maximum body size in MB, default is 64
     */
    maxBodySize: number;

    /**
     * @description switch for the automatic decompression feature, disabled by default
     */
    enableEncoding: boolean;

    /**
     * @description queries and sets the server name, default is: fibjs/0.x.0
     */
    serverName: string;

}


declare namespace Class_HttpServer {
    const promises: FIBJS.GeneralObject;
}
