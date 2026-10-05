/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 *  brief This object allows you to communicate with asynchronous operations (such as fetch requests) and abort them through an AbortController object when needed
 */
declare class Class_AbortSignal extends Class_EventEmitter {
    /**
     * @description Aborts one or more Web requests
     *      reason may be a string, or a value of any type.
     *      @param reason the reason for aborting the request
     *      @return returns an AbortSignal object
     *
     */
    static abort(reason?: string | any): Class_AbortSignal;

    /**
     * @description Creates an AbortSignal that automatically aborts after a timeout
     *      @param ms timeout in milliseconds
     *      @return returns an AbortSignal object that will abort after ms milliseconds
     *
     */
    static timeout(ms: number): Class_AbortSignal;

    /**
     * @description Creates an AbortSignal that aborts when any of the given signals aborts
     *      @param signals an array of AbortSignal objects
     *      @return returns a composite AbortSignal object
     *
     */
    static any(signals: any[]): Class_AbortSignal;

    /**
     * @description Throws an exception if the request has been aborted
     */
    throwIfAborted(): void;

    /**
     * @description Used to check whether an abort has been requested
     */
    readonly aborted: boolean;

    /**
     * @description Gets the reason for aborting the request
     */
    readonly reason: any;

    /**
     * @description Event handler triggered when the request is aborted
     *      @param ev the event object, carrying the abort reason
     *
     */
    on(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Event handler triggered when the request is aborted
     *      @param ev the event object, carrying the abort reason
     *
     */
    onabort: ((ev: FIBJS.GeneralObject)=>void) | null;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}

