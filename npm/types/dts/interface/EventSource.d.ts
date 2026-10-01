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

    /**
     * @description error event callback
     */
    on(event: "error", listener: ()=>void): this;

    /**
     * @description message event callback
     */
    on(event: "message", listener: ()=>void): this;

    /**
     * @description close event callback
     */
    on(event: "close", listener: ()=>void): this;

}

