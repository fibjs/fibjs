/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
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
     *     @param hdlr the built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
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

    /**
     * @description the current event handling interface object of the http protocol conversion handler
     */
    handler: Class_Handler;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the HttpHandler class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpHandlerPromise extends Class_HandlerPromise {
    /**
     * @description creates an http protocol handler object, converting the data of a stream object into http message objects
     *     @param hdlr the built-in message handler: a handler function, chained handling array or routing object; see mq.Handler for details
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

    /**
     * @description the current event handling interface object of the http protocol conversion handler
     */
    handler: Class_HandlerPromise;

}


declare namespace Class_HttpHandler {
    const promises: FIBJS.GeneralObject;
}
