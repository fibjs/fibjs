/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Socket.d.ts" />
/**
 * @description TcpServer` is a high-concurrency TCP Socket server, which can be used to create a TCP server that already has TCP connections established with clients in its initial state
 *
 * The `TcpServer` object can be used to quickly create a multi-fiber concurrent TCP server. When a client connects to the listened address, the callback function is invoked and returns a new connected `Socket` object, which we can use to send or receive TCP messages to or from the client.
 *
 * The following is a concrete example of echoing client TCP messages based on the `TcpServer` object:
 *
 * ```JavaScript
 * const net = require("net");
 *
 * function onConnect(conn) {
 *   console.log(`new client accepted! local:${conn.localAddress}, remote:${conn.remoteAddress}`);
 *   const data = conn.read();
 *   if (data) {
 *     console.log(`recv data on fn onConnect: ${data}`);
 *     conn.write(data);
 *   }
 *   conn.close();
 * }
 *
 * new net.TcpServer('0.0.0.0', 8080, onConnect).start();
 * console.log('server is running on port: 8080');
 * ```
 * In the above code, we create a `TcpServer` object and use the callback function `onConnect` to handle received client request information, writing its data back to the client.
 *
 * When this service is started, it will listen for all IP addresses and requests on port `8080`. When you connect to the service via `telnet` or other client tools, you will see the service print connection information and send every request you send back unchanged.
 *
 */
declare class Class_TcpServer extends Class_EventEmitter {
    /**
     * @description TcpServer constructor, listening on all local addresses
     *     @param port specifies the tcp server listening port
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(port: number, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description TcpServer constructor
     *     @param addr specifies the tcp server listening address; "" means listening on all local addresses
     *     @param port specifies the tcp server listening port
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(addr: string, port: number, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description TcpServer constructor
     *
     *      options supports the following properties:
     *      - address: specifies the listening address, optional, defaults to listening on all addresses
     *      - port: specifies the listening port, optional, listen() must be called to start when not provided
     *
     *     @param options server options
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description TcpServer constructor
     *     @param addr specifies the unix socket or Windows pipe server listening address
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(addr: string, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description TcpServer constructor, does not bind a port, listen() must be called to start
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description starts the current server
     */
    start(): void;

    /**
     * @description binds the address and port and starts listening for connections
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listen(port: number, addr?: string, backlog?: number): void;

    listen(port: number, addr?: string, backlog?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description binds the address and port and starts listening for connections
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listenSync(port: number, addr?: string, backlog?: number): void;

    /**
     * @description binds the address and port and starts listening for connections
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listenAsync(port: number, addr?: string, backlog?: number): Promise<void>;

    /**
     * @description closes the socket and aborts the running server
     */
    stop(): void;

    stop(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description closes the socket and aborts the running server
     */
    stopSync(): void;

    /**
     * @description closes the socket and aborts the running server
     */
    stopAsync(): Promise<void>;

    /**
     * @description closes the socket and aborts the running server; an alias of stop()
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description closes the socket and aborts the running server; an alias of stop()
     */
    closeSync(): void;

    /**
     * @description closes the socket and aborts the running server; an alias of stop()
     */
    closeAsync(): Promise<void>;

    /**
     * @description returns an object containing the server bound address, address family and port. Used to look up the actual port when the OS assigns the address.
     *      @return returns the address, address family and port bound by the server
     *
     */
    address(): {
        address: string;
        family: string;
        port: number;
    };

    /**
     * @description the Socket object the server is currently listening on
     */
    readonly socket: Class_Socket;

    /**
     * @description queries and sets the timeout in milliseconds; this timeout is used for newly accepted connections
     */
    timeout: number;

    /**
     * @description the current event handling interface object of the server
     */
    handler: Class_Handler;

    /**
     * @description emitted after start() is called and binding completes
     */
    on(event: "listening", listener: ()=>void): this;

    once(event: "listening", listener: ()=>void): this;

    off(event: "listening", listener: ()=>void): this;

    addListener(event: "listening", listener: ()=>void): this;

    removeListener(event: "listening", listener: ()=>void): this;

    addEventListener(event: "listening", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "listening", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "listening", listener: ()=>void): this;

    prependOnceListener(event: "listening", listener: ()=>void): this;

    /**
     * @description emitted after start() is called and binding completes
     */
    onlistening: (()=>void) | null;

    /**
     * @description emitted when a new TCP connection is established
     *      @param socket the newly established Socket connection object
     *
     */
    on(event: "connection", listener: (socket: Class_Socket)=>void): this;

    once(event: "connection", listener: (socket: Class_Socket)=>void): this;

    off(event: "connection", listener: (socket: Class_Socket)=>void): this;

    addListener(event: "connection", listener: (socket: Class_Socket)=>void): this;

    removeListener(event: "connection", listener: (socket: Class_Socket)=>void): this;

    addEventListener(event: "connection", listener: (socket: Class_Socket)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "connection", listener: (socket: Class_Socket)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "connection", listener: (socket: Class_Socket)=>void): this;

    prependOnceListener(event: "connection", listener: (socket: Class_Socket)=>void): this;

    /**
     * @description emitted when a new TCP connection is established
     *      @param socket the newly established Socket connection object
     *
     */
    onconnection: ((socket: Class_Socket)=>void) | null;

    /**
     * @description emitted when an error occurs
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
     * @description emitted when an error occurs
     */
    onerror: (()=>void) | null;

    /**
     * @description emitted after the server is closed
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
     * @description emitted after the server is closed
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
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Socket.d.ts" />
/**
 * The promise variant of the TcpServer class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TcpServerPromise extends Class_EventEmitter {
    /**
     * @description TcpServer constructor, listening on all local addresses
     *     @param port specifies the tcp server listening port
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(port: number, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description TcpServer constructor
     *     @param addr specifies the tcp server listening address; "" means listening on all local addresses
     *     @param port specifies the tcp server listening port
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(addr: string, port: number, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description TcpServer constructor
     *
     *      options supports the following properties:
     *      - address: specifies the listening address, optional, defaults to listening on all addresses
     *      - port: specifies the listening port, optional, listen() must be called to start when not provided
     *
     *     @param options server options
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description TcpServer constructor
     *     @param addr specifies the unix socket or Windows pipe server listening address
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(addr: string, listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description TcpServer constructor, does not bind a port, listen() must be called to start
     *     @param listener specifies the built-in message handler for received tcp connections: handling function, chain handling array, routing object, see mq.Handler for details
     *
     */
    constructor(listener: Class_Handler | Class_HandlerPromise);

    /**
     * @description starts the current server
     */
    start(): void;

    /**
     * @description binds the address and port and starts listening for connections
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listen(port: number, addr?: string, backlog?: number): Promise<void>;

    /**
     * @description binds the address and port and starts listening for connections
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listenSync(port: number, addr?: string, backlog?: number): void;

    /**
     * @description binds the address and port and starts listening for connections
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listenAsync(port: number, addr?: string, backlog?: number): Promise<void>;

    /**
     * @description closes the socket and aborts the running server
     */
    stop(): Promise<void>;

    /**
     * @description closes the socket and aborts the running server
     */
    stopSync(): void;

    /**
     * @description closes the socket and aborts the running server
     */
    stopAsync(): Promise<void>;

    /**
     * @description closes the socket and aborts the running server; an alias of stop()
     */
    close(): Promise<void>;

    /**
     * @description closes the socket and aborts the running server; an alias of stop()
     */
    closeSync(): void;

    /**
     * @description closes the socket and aborts the running server; an alias of stop()
     */
    closeAsync(): Promise<void>;

    /**
     * @description returns an object containing the server bound address, address family and port. Used to look up the actual port when the OS assigns the address.
     *      @return returns the address, address family and port bound by the server
     *
     */
    address(): {
        address: string;
        family: string;
        port: number;
    };

    /**
     * @description the Socket object the server is currently listening on
     */
    readonly socket: Class_SocketPromise;

    /**
     * @description queries and sets the timeout in milliseconds; this timeout is used for newly accepted connections
     */
    timeout: number;

    /**
     * @description the current event handling interface object of the server
     */
    handler: Class_HandlerPromise;

    /**
     * @description emitted after start() is called and binding completes
     */
    onlistening: (()=>void) | null;

    /**
     * @description emitted when a new TCP connection is established
     *      @param socket the newly established Socket connection object
     *
     */
    onconnection: ((socket: Class_Socket)=>void) | null;

    /**
     * @description emitted when an error occurs
     */
    onerror: (()=>void) | null;

    /**
     * @description emitted after the server is closed
     */
    onclose: (()=>void) | null;

}


declare namespace Class_TcpServer {
    const promises: FIBJS.GeneralObject;
}
