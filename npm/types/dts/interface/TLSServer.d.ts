/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description tls server object, makes it easy to create a standard multi-fiber tls/ssl server
 *
 * TLSServer is an object that combines and wraps TcpServer and TLSHandler, making it easy to quickly build servers, logically equivalent to:
 *  ```JavaScript
 *  var svr = new net.TLSServer(addr, port, new tls.Handler(ctx, function(conn){
 *     ...
 *  }));
 *  ```
 *
 *  Creation:
 *  ```JavaScript
 *  var tls = require("tls");
 *  var svr = new tls.Server(ctx, function(conn){
 *      ...
 *  });
 *  ```
 *
 */
declare class Class_TLSServer extends Class_TcpServer {
    /**
     * @description creates a new TLSServer object
     *      @param context specifies the secure context used to create TLSServer
     *      @param port specifies the listening port
     *      @param listener the event handling interface object
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description creates a new TLSServer object
     *      @param context specifies the secure context used to create TLSServer
     *      @param addr specifies the listening address
     *      @param port specifies the listening port
     *      @param listener the event handling interface object
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description creates a new TLSServer object
     *
     *      In addition to the properties used to create the SecureContext, options also supports the following properties:
     *      - address: specifies the listening address, optional, defaults to listening on all addresses
     *      - port: specifies the listening port, optional, listen() must be called to start when not provided
     *
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param listener the event handling interface object
     *
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description creates a new TLSServer object without binding a port; listen() must be called to start
     *      @param context specifies the secure context used to create TLSServer
     *      @param listener the event handling interface object
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description queries the SecureContext used by the current TLSServer
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description sets the SecureContext used by the current TLSServer
     *     @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description sets the SecureContext used by the current TLSServer
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the TLSServer class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TLSServerPromise extends Class_TcpServerPromise {
    /**
     * @description creates a new TLSServer object
     *      @param context specifies the secure context used to create TLSServer
     *      @param port specifies the listening port
     *      @param listener the event handling interface object
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description creates a new TLSServer object
     *      @param context specifies the secure context used to create TLSServer
     *      @param addr specifies the listening address
     *      @param port specifies the listening port
     *      @param listener the event handling interface object
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description creates a new TLSServer object
     *
     *      In addition to the properties used to create the SecureContext, options also supports the following properties:
     *      - address: specifies the listening address, optional, defaults to listening on all addresses
     *      - port: specifies the listening port, optional, listen() must be called to start when not provided
     *
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param listener the event handling interface object
     *
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description creates a new TLSServer object without binding a port; listen() must be called to start
     *      @param context specifies the secure context used to create TLSServer
     *      @param listener the event handling interface object
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description queries the SecureContext used by the current TLSServer
     */
    readonly secureContext: Class_SecureContextPromise;

    /**
     * @description sets the SecureContext used by the current TLSServer
     *     @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description sets the SecureContext used by the current TLSServer
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


declare namespace Class_TLSServer {
    const promises: FIBJS.GeneralObject;
}
