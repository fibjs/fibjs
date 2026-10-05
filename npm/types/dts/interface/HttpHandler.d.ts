/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * @description http protocol conversion handler
 *
 *   Used to convert a data stream into http protocol messages. It can be created with:
 *   ```JavaScript
 *   var hdlr = new mq.HttpHandler(...);
 *   ```
 *   or:
 *   ```JavaScript
 *   var hdlr = new http.Handler(...);
 *   ```
 *
 */
declare class Class_HttpHandler extends Class_Handler {
    /**
     * @description creates an http protocol handler object, converting the data of a stream object into http message objects
     *
     *     hdlr may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *     - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *     @param hdlr the request handler
     *
     */
    constructor(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

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

    /**
     * @description the current event handling interface object of the http protocol conversion handler
     */
    handler: Class_Handler;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * The promise variant of the HttpHandler class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpHandlerPromise extends Class_HandlerPromise {
    /**
     * @description creates an http protocol handler object, converting the data of a stream object into http message objects
     *
     *     hdlr may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(req, res) => any`, called with the HttpRequest and the HttpResponse of each request;
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); a function value is called as `(req, ...captures, res) => any`, with the captured groups between the request and the response (also readable as req.params);
     *     - a path or address string: a directory served as static files, or an `http(s)://` address forwarded by a repeater.
     *     @param hdlr the request handler
     *
     */
    constructor(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

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

    /**
     * @description the current event handling interface object of the http protocol conversion handler
     */
    handler: Class_HandlerPromise;

}


declare namespace Class_HttpHandler {
    const promises: FIBJS.GeneralObject;
}
