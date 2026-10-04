/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Message handler interface
 */
declare class Class_Handler extends Class_object {
    /**
     * @description Constructs a message handler chain object
     *      @param hdlrs handler array
     *
     */
    constructor(hdlrs: (Class_Handler | Class_HandlerPromise)[]);

    /**
     * @description Creates a message handler routing object
     *      @param map initialization routing parameters
     *
     */
    constructor(map: FIBJS.GeneralObject);

    /**
     * @description Creates a JavaScript message handler
     *      @param hdlr JavaScript handler function
     *
     */
    constructor(hdlr: (...args: any[])=>any);

    /**
     * @description Constructs a fileHandler or HttpRepeater
     *      @param hdlr the address parameter of the handler
     *
     */
    constructor(hdlr: string);

    /**
     * @description Queries whether the current handler supports routing
     *      @return returns whether the current handler supports routing
     *
     */
    isRouting(): boolean;

    /**
     * @description Processes a message or object
     *      @param v the message or object to process
     *      @return returns the next handler
     *
     */
    invoke(v: Class_object): Class_Handler;

    invoke(v: Class_object, callback: (err: Error | undefined | null, retVal: Class_Handler)=>any): void;

    /**
     * @description Processes a message or object
     *      @param v the message or object to process
     *      @return returns the next handler
     *
     */
    invokeSync(v: Class_object): Class_Handler;

    /**
     * @description Processes a message or object
     *      @param v the message or object to process
     *      @return returns the next handler
     *
     */
    invokeAsync(v: Class_object): Promise<Class_HandlerPromise>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * The promise variant of the Handler class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HandlerPromise extends Class_object {
    /**
     * @description Constructs a message handler chain object
     *      @param hdlrs handler array
     *
     */
    constructor(hdlrs: (Class_Handler | Class_HandlerPromise)[]);

    /**
     * @description Creates a message handler routing object
     *      @param map initialization routing parameters
     *
     */
    constructor(map: FIBJS.GeneralObject);

    /**
     * @description Creates a JavaScript message handler
     *      @param hdlr JavaScript handler function
     *
     */
    constructor(hdlr: (...args: any[])=>any);

    /**
     * @description Constructs a fileHandler or HttpRepeater
     *      @param hdlr the address parameter of the handler
     *
     */
    constructor(hdlr: string);

    /**
     * @description Queries whether the current handler supports routing
     *      @return returns whether the current handler supports routing
     *
     */
    isRouting(): boolean;

    /**
     * @description Processes a message or object
     *      @param v the message or object to process
     *      @return returns the next handler
     *
     */
    invoke(v: Class_object): Promise<Class_HandlerPromise>;

    /**
     * @description Processes a message or object
     *      @param v the message or object to process
     *      @return returns the next handler
     *
     */
    invokeSync(v: Class_object): Class_Handler;

    /**
     * @description Processes a message or object
     *      @param v the message or object to process
     *      @return returns the next handler
     *
     */
    invokeAsync(v: Class_object): Promise<Class_HandlerPromise>;

}


declare namespace Class_Handler {
    const promises: FIBJS.GeneralObject;
}
