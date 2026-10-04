/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description the RTCDataChannel interface defines a bidirectional data channel
 */
declare class Class_RTCDataChannel extends Class_EventEmitter {
    /**
     * @description sends data to the remote end; a Buffer is sent as binary data and a string as text data
     *      @param data the data to send; a string is encoded as utf8
     *
     */
    send(data: Class_Buffer | string): void;

    /**
     * @description closes the channel; this method is used to close the channel
     */
    close(): void;

    /**
     * @description returns the ID number that uniquely identifies the RTCDataChannel
     */
    readonly id: number;

    /**
     * @description returns a string containing the name describing the data channel
     */
    readonly label: string;

    /**
     * @description returns a string containing the name of the sub-protocol in use
     */
    readonly protocol: string;

    /**
     * @description returns the number of bytes of data currently queued to be sent over the data channel
     */
    readonly bufferedAmount: number;

    /**
     * @description channel open event, emitted when the channel is opened
     */
    on(event: "open", listener: ()=>void): this;

    once(event: "open", listener: ()=>void): this;

    off(event: "open", listener: ()=>void): this;

    addListener(event: "open", listener: ()=>void): this;

    removeListener(event: "open", listener: ()=>void): this;

    addEventListener(event: "open", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "open", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "open", listener: ()=>void): this;

    prependOnceListener(event: "open", listener: ()=>void): this;

    /**
     * @description channel open event, emitted when the channel is opened
     */
    onopen: (()=>void) | null;

    /**
     * @description channel message event, emitted when a message is received
     *      @param ev the event object, carrying the received data in its data property
     *
     */
    on(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description channel message event, emitted when a message is received
     *      @param ev the event object, carrying the received data in its data property
     *
     */
    onmessage: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description channel close event, emitted when the channel is closed
     */
    on(event: "close", listener: ()=>void): this;

    once(event: "close", listener: ()=>void): this;

    off(event: "close", listener: ()=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    /**
     * @description channel close event, emitted when the channel is closed
     */
    onclose: (()=>void) | null;

    /**
     * @description channel error event, emitted when an error occurs on the channel
     *      @param ev the event object, carrying the error message in its error property
     *
     */
    on(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description channel error event, emitted when an error occurs on the channel
     *      @param ev the event object, carrying the error message in its error property
     *
     */
    onerror: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description channel buffered amount low event, emitted when the channel buffered amount is low
     */
    on(event: "bufferedamountlow", listener: ()=>void): this;

    once(event: "bufferedamountlow", listener: ()=>void): this;

    off(event: "bufferedamountlow", listener: ()=>void): this;

    addListener(event: "bufferedamountlow", listener: ()=>void): this;

    removeListener(event: "bufferedamountlow", listener: ()=>void): this;

    addEventListener(event: "bufferedamountlow", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "bufferedamountlow", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "bufferedamountlow", listener: ()=>void): this;

    prependOnceListener(event: "bufferedamountlow", listener: ()=>void): this;

    /**
     * @description channel buffered amount low event, emitted when the channel buffered amount is low
     */
    onbufferedamountlow: (()=>void) | null;

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

