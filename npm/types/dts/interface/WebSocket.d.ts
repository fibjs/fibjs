/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/WebSocketMessage.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description WebSocket is a full-duplex communication protocol based on TCP; it establishes a persistent connection between browser and server, enabling real-time bidirectional data transmission and supporting data in any format. In fibjs, the WebSocket support module provides corresponding API interfaces for developing WebSocket servers and clients
 *
 * The WebSocket support module is only an implementation of the WebSocket protocol and needs to work on top of the HTTP protocol. On the server side, HTTP requests can be converted into WebSocket connections through the upgrade function; on the client side, the server address to connect is specified via a WebSocket protocol URL.
 *
 * Example of starting a WebSocket server:
 * ```JavaScript
 * var http = require('http');
 *
 * var svr = new http.Server(80, {
 *     '/ws': WebSocket.upgrade({
 *         protocols: ['json', 'text']
 *     }, conn => {
 *         conn.onmessage = e => {
 *             conn.send('fibjs:' + e.data);
 *         };
 *     })
 * });
 * svr.start();
 * ```
 * Example of establishing a connection to the above server from a client:
 * ```JavaScript
 * var conn = new WebSocket("ws://127.0.0.1/ws", ['json', 'text']);
 * // emit open event
 * conn.onopen = () => {
 *     console.log("websocket connected with protocol:", conn.protocol);
 *     conn.send("hi");
 * };
 * // emit close event
 * conn.onmessage = evt => {
 *     console.log("websocket receive: " + evt.data);
 * };
 * ```
 *
 */
declare class Class_WebSocket extends Class_EventEmitter {
    /**
     * @description WebSocket constructor
     *      @param url specifies the server to connect
     *      @param protocol specifies the handshake protocol, default is ""
     *      @param origin specifies the origin to simulate during the handshake, default is ""
     *
     */
    constructor(url: string, protocol?: string, origin?: string);

    /**
     * @description WebSocket constructor
     *      @param url specifies the server to connect
     *      @param protocols specifies the list of candidate sub-protocols for the handshake
     *      @param origin specifies the origin to simulate during the handshake, default is ""
     *
     */
    constructor(url: string, protocols: string[], origin?: string);

    /**
     * @description WebSocket constructor
     *      opts contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "protocol": "", // specify the sub-protocol, default is ""
     *          "protocols": [], // specify candidate sub-protocols, takes precedence over protocol when provided
     *          "origin": "", // specify the origin, default is ""
     *          "perMessageDeflate": false, // specify whether to enable permessage-deflate, default is false
     *          "maxPayload": 67108864, // specify the max payload size, default is 64MB
     *          "httpClient": hc, // specify the http client, default is null, use the global http client
     *          "headers": // specify the http headers, default is {}
     *      }
     *      ```
     *      @param url specifies the server to connect
     *      @param opts connection options, default is {}
     *
     */
    constructor(url: string, opts: FIBJS.GeneralObject);

    /**
     * @description specifies WebSocket message type 0, representing a continuation frame
     */
    static readonly CONTINUE: 0;

    /**
     * @description specifies WebSocket message type 1, representing a text frame
     */
    static readonly TEXT: 1;

    /**
     * @description specifies WebSocket message type 2, representing a binary frame
     */
    static readonly BINARY: 2;

    /**
     * @description specifies WebSocket message type 8, connection close
     */
    static readonly CLOSE: 8;

    /**
     * @description specifies WebSocket message type 9, representing a ping frame
     */
    static readonly PING: 9;

    /**
     * @description specifies WebSocket message type 10, representing a pong frame
     */
    static readonly PONG: 10;

    /**
     * @description specifies the WebSocket state, indicating connecting
     */
    static readonly CONNECTING: 0;

    /**
     * @description specifies the WebSocket state, indicating connected
     */
    static readonly OPEN: 1;

    /**
     * @description specifies the WebSocket state, indicating closing
     */
    static readonly CLOSING: 2;

    /**
     * @description specifies the WebSocket state, indicating closed
     */
    static readonly CLOSED: 3;

    /**
     * @description queries the server the current object is connected to
     */
    readonly url: string;

    /**
     * @description queries the protocol used when the current object connected
     */
    readonly protocol: string;

    /**
     * @description queries the origin the current object connected with
     */
    readonly origin: string;

    /**
     * @description queries the connection state of the current object, see ws
     */
    readonly readyState: number;

    /**
     * @description closes the current connection; this operation sends a CLOSE packet to the peer and waits for its response
     *      @param code specifies the close code, allowed values are 3000-4999 or 1000, default is 1000
     *      @param reason specifies the reason for closing, default is ""
     *
     */
    close(code?: number, reason?: string): void;

    /**
     * @description sends a piece of text to the peer
     *      @param data specifies the text to send
     *
     */
    send(data: string): void;

    /**
     * @description sends a piece of binary data to the peer
     *      @param data specifies the binary data to send
     *
     */
    send(data: Class_Buffer): void;

    send(data: string | Class_Buffer): void;

    /**
     * @description queries and binds the connection success event, equivalent to on("open", func);
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
     * @description queries and binds the connection success event, equivalent to on("open", func);
     */
    onopen: (()=>void) | null;

    /**
     * @description queries and binds the event of receiving a message from the peer, equivalent to on("message", func);
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
     * @description queries and binds the event of receiving a message from the peer, equivalent to on("message", func);
     */
    onmessage: (()=>void) | null;

    /**
     * @description queries and binds the connection close event, equivalent to on("close", func);
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
     * @description queries and binds the connection close event, equivalent to on("close", func);
     */
    onclose: (()=>void) | null;

    /**
     * @description queries and binds the error event, equivalent to on("error", func);
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
     * @description queries and binds the error event, equivalent to on("error", func);
     */
    onerror: (()=>void) | null;

    /**
     * @description keeps the fibjs process from exiting, preventing the fibjs process from exiting while the object is bound
     *      @return returns the current object
     *
     */
    ref(): Class_WebSocket;

    /**
     * @description allows the fibjs process to exit, allowing the fibjs process to exit while the object is bound
     *      @return returns the current object
     *
     */
    unref(): Class_WebSocket;

    /**
     * @description the WebSocketMessage class, used to create WebSocket protocol messages, see the WebSocketMessage object
     */
    static Message: Class_WebSocketMessage;

    /**
     * @description creates a WebSocket protocol handler that receives http upgrade requests and performs the handshake, generating a WebSocket object
     *      @param accept the connection success handler; the callback will receive two parameters, the first is the received WebSocket object and the second is the HttpRequest object of the handshake
     *      @return returns the protocol handler, which can be used with HttpServer, Chain, Routing, etc.
     *
     */
    static upgrade(accept: (...args: any[])=>any): Class_Handler;

    /**
     * @description creates a WebSocket protocol handler that receives http upgrade requests and performs the handshake, generating a WebSocket object
     *      opts supports using `protocol` or `protocols` to specify the sub-protocols acceptable to the server, and writes back `Sec-WebSocket-Protocol` when the handshake succeeds, for example:
     *      ```JavaScript
     *      WebSocket.upgrade({
     *          protocols: ['json', 'text']
     *      }, conn => {
     *          console.log(conn.protocol); // selected sub-protocol
     *      })
     *      ```
     *      @param opts connection options, default is {}
     *      @param accept the connection success handler; the callback will receive two parameters, the first is the received WebSocket object and the second is the HttpRequest object of the handshake
     *      @return returns the protocol handler, which can be used with HttpServer, Chain, Routing, etc.
     *
     */
    static upgrade(opts: FIBJS.GeneralObject, accept: (...args: any[])=>any): Class_Handler;

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

