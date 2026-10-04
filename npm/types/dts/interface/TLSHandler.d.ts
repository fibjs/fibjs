/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
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
     *     @param context specifies the secure context used to create TLSHandler
     *     @param handler the event handling interface object
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, handler: Class_Handler | Class_HandlerPromise);

    /**
     * @description creates a new TLSHandler object
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *     @param handler the event handling interface object
     *
     */
    constructor(options: FIBJS.GeneralObject, handler: Class_Handler | Class_HandlerPromise);

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
/**
 * The promise variant of the TLSHandler class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TLSHandlerPromise extends Class_HandlerPromise {
    /**
     * @description creates a new TLSHandler object
     *     @param context specifies the secure context used to create TLSHandler
     *     @param handler the event handling interface object
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, handler: Class_Handler | Class_HandlerPromise);

    /**
     * @description creates a new TLSHandler object
     *     @param options the options needed to create a secure context with tls.createSecureContext
     *     @param handler the event handling interface object
     *
     */
    constructor(options: FIBJS.GeneralObject, handler: Class_Handler | Class_HandlerPromise);

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
