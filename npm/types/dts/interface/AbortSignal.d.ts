/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 *  brief This object allows you to communicate with asynchronous operations (such as fetch requests) and abort them through an AbortController object when needed
 */
declare class Class_AbortSignal extends Class_EventEmitter {
    /**
     * @description Aborts one or more Web requests
     *      @param reason an optional string describing the reason for aborting the request
     *      @return returns an AbortSignal object
     *
     */
    static abort(reason?: string): Class_AbortSignal;

    /**
     * @description Aborts one or more Web requests
     *      @param reason a value of any type describing the reason for aborting the request
     *      @return returns an AbortSignal object
     *
     */
    static abort(reason: any): Class_AbortSignal;

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
     */
    on(event: "abort", listener: ()=>void): this;

}

