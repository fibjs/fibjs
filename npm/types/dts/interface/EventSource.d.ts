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
     *      @param ev the event object of the connection
     *
     */
    on(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description open event callback
     *      @param ev the event object of the connection
     *
     */
    onopen: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description error event callback
     *      @param ev the event object, carrying the error code and reason
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
     * @description error event callback
     *      @param ev the event object, carrying the error code and reason
     *
     */
    onerror: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description message event callback
     *      @param ev the event object, carrying the message data, id and retry interval
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
     * @description message event callback
     *      @param ev the event object, carrying the message data, id and retry interval
     *
     */
    onmessage: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description close event callback
     *      @param ev the event object of the closed connection
     *
     */
    on(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description close event callback
     *      @param ev the event object of the closed connection
     *
     */
    onclose: ((ev: FIBJS.GeneralObject)=>void) | null;

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
     *      @param ev the event object of the connection
     *
     */
    onopen: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description error event callback
     *      @param ev the event object, carrying the error code and reason
     *
     */
    onerror: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description message event callback
     *      @param ev the event object, carrying the message data, id and retry interval
     *
     */
    onmessage: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description close event callback
     *      @param ev the event object of the closed connection
     *
     */
    onclose: ((ev: FIBJS.GeneralObject)=>void) | null;

}


declare namespace Class_EventSource {
    const promises: FIBJS.GeneralObject;
}
