/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * @description event source interface, used for server-sent events
 *
 *   Reference:
 *   ```JavaScript
 *     const http = require('http');
 *
 *     const es = new http.EventSource('http://localhost:8080');
 *   ```
 *
 */
declare class Class_EventSource extends Class_EventEmitter {
    /**
     * @description constructor
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *
     *       @param url server address
     *       @param options options
     *
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description closes the connection
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description closes the connection
     */
    closeSync(): void;

    /**
     * @description closes the connection
     */
    closeAsync(): Promise<void>;

    /**
     * @description sends an event to the client
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "event": "message", // Specify the event type, default is message
     *          "id": "", // Event ID
     *          "retry": 0 // Retry interval in milliseconds
     *      }
     *      ```
     *
     *      @param data event data
     *      @param options options
     *      @return returns the number of bytes sent
     *
     */
    send(data: string, options?: FIBJS.GeneralObject): number;

    send(data: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description sends an event to the client
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "event": "message", // Specify the event type, default is message
     *          "id": "", // Event ID
     *          "retry": 0 // Retry interval in milliseconds
     *      }
     *      ```
     *
     *      @param data event data
     *      @param options options
     *      @return returns the number of bytes sent
     *
     */
    sendSync(data: string, options?: FIBJS.GeneralObject): number;

    /**
     * @description sends an event to the client
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "event": "message", // Specify the event type, default is message
     *          "id": "", // Event ID
     *          "retry": 0 // Retry interval in milliseconds
     *      }
     *      ```
     *
     *      @param data event data
     *      @param options options
     *      @return returns the number of bytes sent
     *
     */
    sendAsync(data: string, options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description event source state, the value is CONNECTING, OPEN, CLOSED
     */
    readonly readyState: number;

    /**
     * @description server address
     */
    readonly url: string;

    /**
     * @description whether to carry credentials
     */
    readonly withCredentials: boolean;

    /**
     * @description http response object
     */
    readonly response: Class_HttpResponse;

    /**
     * @description open event callback
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
     * @description open event callback
     */
    onopen: (()=>void) | null;

    /**
     * @description error event callback
     */
    on(event: "error", listener: ()=>void): this;

    once(event: "error", listener: ()=>void): this;

    off(event: "error", listener: ()=>void): this;

    addListener(event: "error", listener: ()=>void): this;

    removeListener(event: "error", listener: ()=>void): this;

    addEventListener(event: "error", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: ()=>void): this;

    prependOnceListener(event: "error", listener: ()=>void): this;

    /**
     * @description error event callback
     */
    onerror: (()=>void) | null;

    /**
     * @description message event callback
     */
    on(event: "message", listener: ()=>void): this;

    once(event: "message", listener: ()=>void): this;

    off(event: "message", listener: ()=>void): this;

    addListener(event: "message", listener: ()=>void): this;

    removeListener(event: "message", listener: ()=>void): this;

    addEventListener(event: "message", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: ()=>void): this;

    prependOnceListener(event: "message", listener: ()=>void): this;

    /**
     * @description message event callback
     */
    onmessage: (()=>void) | null;

    /**
     * @description close event callback
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
     * @description close event callback
     */
    onclose: (()=>void) | null;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * The promise variant of the EventSource class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_EventSourcePromise extends Class_EventEmitter {
    /**
     * @description constructor
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *
     *       @param url server address
     *       @param options options
     *
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description closes the connection
     */
    close(): Promise<void>;

    /**
     * @description closes the connection
     */
    closeSync(): void;

    /**
     * @description closes the connection
     */
    closeAsync(): Promise<void>;

    /**
     * @description sends an event to the client
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "event": "message", // Specify the event type, default is message
     *          "id": "", // Event ID
     *          "retry": 0 // Retry interval in milliseconds
     *      }
     *      ```
     *
     *      @param data event data
     *      @param options options
     *      @return returns the number of bytes sent
     *
     */
    send(data: string, options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description sends an event to the client
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "event": "message", // Specify the event type, default is message
     *          "id": "", // Event ID
     *          "retry": 0 // Retry interval in milliseconds
     *      }
     *      ```
     *
     *      @param data event data
     *      @param options options
     *      @return returns the number of bytes sent
     *
     */
    sendSync(data: string, options?: FIBJS.GeneralObject): number;

    /**
     * @description sends an event to the client
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "event": "message", // Specify the event type, default is message
     *          "id": "", // Event ID
     *          "retry": 0 // Retry interval in milliseconds
     *      }
     *      ```
     *
     *      @param data event data
     *      @param options options
     *      @return returns the number of bytes sent
     *
     */
    sendAsync(data: string, options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description event source state, the value is CONNECTING, OPEN, CLOSED
     */
    readonly readyState: number;

    /**
     * @description server address
     */
    readonly url: string;

    /**
     * @description whether to carry credentials
     */
    readonly withCredentials: boolean;

    /**
     * @description http response object
     */
    readonly response: Class_HttpResponsePromise;

    /**
     * @description open event callback
     */
    onopen: (()=>void) | null;

    /**
     * @description error event callback
     */
    onerror: (()=>void) | null;

    /**
     * @description message event callback
     */
    onmessage: (()=>void) | null;

    /**
     * @description close event callback
     */
    onclose: (()=>void) | null;

}


declare namespace Class_EventSource {
    const promises: FIBJS.GeneralObject;
}
