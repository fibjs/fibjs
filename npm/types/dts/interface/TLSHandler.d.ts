/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/TLSSocket.d.ts" />
/**
 * @description tls/ssl protocol conversion handler
 *
 *  Used to convert data streams into the tls/ssl stream protocol. TLSHandler is a wrapper around TLSSocket, used to build servers, logically equivalent to:
 *  ```JavaScript
 *
 *  function(s){
 *     var s1 = new tls.TLSSocket(ctx);
 *     s1.accept(s);
 *     hdlr.invoke(s1);
 *     s1.close();
 *  }
 *  ```
 *
 */
declare class Class_TLSHandler extends Class_Handler {
    /**
     * @description creates a new TLSHandler object
     *
     *     handler may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(socket) => any`, called with each accepted TLS connection (a TLSSocket; it extends Stream, not Socket);
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *     - a path/address string: a directory or an `http(s)://` address, converted through the Handler constructor.
     *     @param context specifies the secure context used to create TLSHandler
     *     @param handler the connection handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, handler: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description creates a new TLSHandler object
     *
     *     handler may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(socket) => any`, called with each accepted TLS connection (a TLSSocket; it extends Stream, not Socket);
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *     - a path/address string: a directory or an `http(s)://` address, converted through the Handler constructor.
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *     @param handler the connection handler
     *
     */
    constructor(options: FIBJS.GeneralObject, handler: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description queries the SecureContext used by the current TLSHandler
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description sets the SecureContext used by the current TLSHandler
     *     @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description sets the SecureContext used by the current TLSHandler
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

    /**
     * @description the current event handling interface object of the ssl protocol conversion handler
     */
    handler: Class_Handler;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/TLSSocket.d.ts" />
/**
 * The promise variant of the TLSHandler class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TLSHandlerPromise extends Class_HandlerPromise {
    /**
     * @description creates a new TLSHandler object
     *
     *     handler may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(socket) => any`, called with each accepted TLS connection (a TLSSocket; it extends Stream, not Socket);
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *     - a path/address string: a directory or an `http(s)://` address, converted through the Handler constructor.
     *     @param context specifies the secure context used to create TLSHandler
     *     @param handler the connection handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, handler: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description creates a new TLSHandler object
     *
     *     handler may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(socket) => any`, called with each accepted TLS connection (a TLSSocket; it extends Stream, not Socket);
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *     - a path/address string: a directory or an `http(s)://` address, converted through the Handler constructor.
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *     @param handler the connection handler
     *
     */
    constructor(options: FIBJS.GeneralObject, handler: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description queries the SecureContext used by the current TLSHandler
     */
    readonly secureContext: Class_SecureContextPromise;

    /**
     * @description sets the SecureContext used by the current TLSHandler
     *     @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description sets the SecureContext used by the current TLSHandler
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

    /**
     * @description the current event handling interface object of the ssl protocol conversion handler
     */
    handler: Class_HandlerPromise;

}


declare namespace Class_TLSHandler {
    const promises: FIBJS.GeneralObject;
}
