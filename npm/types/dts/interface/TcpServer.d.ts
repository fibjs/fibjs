/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Socket.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description A fiber-per-connection TCP server: it binds an address and hands every accepted connection to a handler, one Socket and one fiber per client
 *
 *  TcpServer is the server side of the net module and the fibjs counterpart of the Node.js
 *  net.Server. A handler is a function (or Handler object) called with each accepted Socket; it runs
 *  in its own fiber, so it can read, process and answer the client sequentially while other
 *  connections are served in parallel. The typical shapes are:
 *
 *  - `new net.TcpServer(port, handler)` or `new net.TcpServer(addr, port, handler)`: bind in the
 *    constructor, then `start()` begins accepting;
 *  - `new net.TcpServer(handler)`: bind later with `listen(port[, addr[, backlog]])`;
 *  - `net.createServer(...)`: the factory form of the constructors.
 *
 *  Concepts:
 *
 *  - **Lifecycle**: a constructor with a port/address binds and listens but does not accept yet;
 *    start() starts the accept loop and emits 'listening'. listen(port, addr, backlog) binds and
 *    starts in one call and is the only way to bind the handler-only form. stop()/close() closes the
 *    listening socket and emits 'close' immediately; a second listen on the same server throws
 *    ERR_SERVER_ALREADY_LISTEN, and stop() on a server that never bound is a no-op.
 *  - **Handler model**: the listener is normalized by the Handler constructor. A function is called
 *    once per connection in a new fiber with the accepted Socket; an array of handlers is chained; a
 *    routing map or a path/address string is converted to a message handler, which cannot process a
 *    raw connection. An exception thrown by the handler is logged and the connection is closed, it
 *    does not propagate to the caller.
 *  - **Accepted sockets**: each accepted Socket is independent from the server. The server does not
 *    close them on stop(), so long-lived connections keep working after the listener is closed (the
 *    handler owns them). The server `timeout` is copied to each accepted socket before the handler
 *    runs.
 *  - **Address information**: address() returns { address, family, port }; with listen(0) it reports
 *    the OS-assigned port. For a unix socket the address is the path while family/port are
 *    placeholders (see address). address() throws before the server is bound; Node.js returns null
 *    instead and returns the path string for pipe servers.
 *  - **Events**: 'listening' after start()/listen(), 'connection' with the accepted Socket before
 *    the handler is invoked, 'error' with a plain message string for accept errors and 'close' on
 *    stop()/close(). Node.js passes an Error to 'error' and delays 'close' until all connections
 *    have ended, while fibjs emits 'close' immediately.
 *  - **Node.js differences**: there is no net.Server class (net.createServer returns a TcpServer);
 *    the fibjs constructor can bind while Node.js always requires listen(); the options object of
 *    the constructor is a bind specification ({address, port}), not the socket options of Node.js;
 *    and fibjs has no maxConnections/getConnections/ref/unref.
 *
 *  Obtained from:
 *  - `net.createServer(options, listener)` / `net.createServer(listener)` — the factory form;
 *  - `new net.TcpServer(port, listener)` and the other constructor forms — explicit creation.
 *
 *  Example 1 — an echo server on an OS-assigned port:
 *  ```JavaScript
 *  const net = require('net');
 *
 *  const server = net.createServer((conn) => {
 *      let data;
 *      while ((data = conn.recv()) !== null)
 *          conn.send(data);
 *      conn.close();
 *  });
 *
 *  server.listen(0, '127.0.0.1');
 *  console.log('listening on', server.address().port);
 *
 *  const client = net.connect(server.address().port, '127.0.0.1');
 *  client.send('echo');
 *  console.log(client.recv().toString()); // echo
 *  client.close();
 *
 *  server.stop();
 *  ```
 *
 *  Example 2 — lifecycle events and a constructor-created server:
 *  ```JavaScript
 *  const net = require('net');
 *
 *  const server = new net.TcpServer(0, (conn) => {
 *      conn.write('hi');
 *      conn.close();
 *  });
 *  server.on('listening', () => console.log('listening'));
 *  server.on('connection', () => console.log('connection'));
 *  server.on('close', () => console.log('closed'));
 *  server.start();
 *
 *  const client = net.connect(server.address().port, '127.0.0.1');
 *  console.log(client.read().toString()); // hi
 *
 *  server.stop(); // closed
 *  client.close();
 *  ```
 *
 */
declare class Class_TcpServer extends Class_EventEmitter {
    /**
     * @description TcpServer constructor, binds a port on all local addresses
     *
     *     listener may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(socket) => any`, called with each accepted connection (a Socket) in its
     *       own fiber, so the handler may read and write in a loop;
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these
     *       same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *     - a path/address string: a directory or an `http(s)://` address, converted through the Handler
     *       constructor.
     *     The server binds port on all local addresses and starts listening; start() begins accepting.
     *     Port 0 asks the operating system for an ephemeral port, read it from address() after start().
     *     An exception thrown by the handler is logged and the connection is closed.
     *     @param port specifies the tcp server listening port
     *     @param listener the connection handler
     *
     */
    constructor(port: number, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description TcpServer constructor, binds the given address and port
     *
     *      addr is an IP literal; '' means listening on all local addresses and '::' all IPv6 addresses.
     *      port may be 0 for an OS-assigned port. A unix socket or Windows pipe path is also accepted and
     *      the port is then ignored. The listener forms are described on the port-only overload; start()
     *      begins accepting after the constructor has bound and listened.
     *      @param addr specifies the tcp server listening address; "" means listening on all local addresses
     *      @param port specifies the tcp server listening port
     *      @param listener the connection handler
     *
     */
    constructor(addr: string, port: number, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description TcpServer constructor, does not bind a port, listen() must be called to start
     *
     *     The fully deferred form: only the handler is stored. Call listen(port[, addr[, backlog]]) or
     *     bind through another constructor later; address() and socket throw until the server is bound.
     *     The listener forms are described on the port-only overload.
     *     @param listener the connection handler
     *
     */
    constructor(listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description TcpServer constructor, binds from an options object
     *
     *      options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "address": "", // the listening address, defaults to all addresses
     *          "port": 0      // the listening port; without it nothing is bound and listen() is required
     *      })
     *      ```
     *      With both properties the server binds address:port immediately; with address only it binds
     *      that address with an OS-assigned port (or a unix path when the address is not an IP literal);
     *      with neither it only stores the handler and listen() must be called. These are bind options,
     *      not the socket options of Node.js net.createServer. The listener forms are described on the
     *      port-only overload.
     *      @param options server options
     *      @param listener the connection handler
     *
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description TcpServer constructor, binds a unix socket or Windows pipe
     *
     *     addr is the path to bind, for example '/tmp/app.sock' or '\\.\pipe\app' (watch the string
     *     escapes). An all-digit string is treated as a TCP port instead, so
     *     new net.TcpServer('8080', handler) listens on port 8080. The path is bound during construction
     *     and start() begins accepting. Node.js uses server.listen(path) for the same purpose.
     *     @param addr specifies the unix socket or Windows pipe server listening address
     *     @param listener the connection handler
     *
     */
    constructor(addr: string, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Starts the current server
     *
     *      Begins the accept loop and emits 'listening'. The server must already be bound: the
     *      constructors with a port/address bind in the constructor while the handler-only form needs
     *      listen(). Calling start on an unbound server or a second time fails with an invalid-call
     *      error. Each accepted client is passed to the 'connection' listeners and then to the handler.
     *
     */
    start(): void;

    /**
     * @description Binds the address and port and starts listening for connections
     *
     *      Binds and starts the accept loop in one call, emitting 'listening'. Port 0 asks the operating
     *      system for an ephemeral port, read it from address(). backlog -1 passes the system default to
     *      the operating system. A second listen on the same server throws ERR_SERVER_ALREADY_LISTEN,
     *      and a failed bind (for example EADDRINUSE) carries syscall 'listen' like Node.js.
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listen(port: number, addr?: string, backlog?: number): void;

    listen(port: number, addr?: string, backlog?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Binds the address and port and starts listening for connections
     *
     *      Binds and starts the accept loop in one call, emitting 'listening'. Port 0 asks the operating
     *      system for an ephemeral port, read it from address(). backlog -1 passes the system default to
     *      the operating system. A second listen on the same server throws ERR_SERVER_ALREADY_LISTEN,
     *      and a failed bind (for example EADDRINUSE) carries syscall 'listen' like Node.js.
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listenSync(port: number, addr?: string, backlog?: number): void;

    /**
     * @description Binds the address and port and starts listening for connections
     *
     *      Binds and starts the accept loop in one call, emitting 'listening'. Port 0 asks the operating
     *      system for an ephemeral port, read it from address(). backlog -1 passes the system default to
     *      the operating system. A second listen on the same server throws ERR_SERVER_ALREADY_LISTEN,
     *      and a failed bind (for example EADDRINUSE) carries syscall 'listen' like Node.js.
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listenAsync(port: number, addr?: string, backlog?: number): Promise<void>;

    /**
     * @description Closes the socket and aborts the running server
     *
     *      Closes the listening socket and emits 'close' immediately; the accepted connections are not
     *      closed and keep running in their handler fibers (the handler owns them). Stopping a server
     *      that was never bound is a no-op. Node.js server.close() instead waits for the active
     *      connections to end before emitting 'close'.
     *
     */
    stop(): void;

    stop(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the socket and aborts the running server
     *
     *      Closes the listening socket and emits 'close' immediately; the accepted connections are not
     *      closed and keep running in their handler fibers (the handler owns them). Stopping a server
     *      that was never bound is a no-op. Node.js server.close() instead waits for the active
     *      connections to end before emitting 'close'.
     *
     */
    stopSync(): void;

    /**
     * @description Closes the socket and aborts the running server
     *
     *      Closes the listening socket and emits 'close' immediately; the accepted connections are not
     *      closed and keep running in their handler fibers (the handler owns them). Stopping a server
     *      that was never bound is a no-op. Node.js server.close() instead waits for the active
     *      connections to end before emitting 'close'.
     *
     */
    stopAsync(): Promise<void>;

    /**
     * @description Closes the socket and aborts the running server; an alias of stop()
     *
     *      Identical to stop(), provided for the Node.js naming; both are awaitable.
     *
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the socket and aborts the running server; an alias of stop()
     *
     *      Identical to stop(), provided for the Node.js naming; both are awaitable.
     *
     */
    closeSync(): void;

    /**
     * @description Closes the socket and aborts the running server; an alias of stop()
     *
     *      Identical to stop(), provided for the Node.js naming; both are awaitable.
     *
     */
    closeAsync(): Promise<void>;

    /**
     * @description Returns an object containing the server bound address, address family and port. Used to look up the actual port when the OS assigns the address.
     *
     *      The result has the shape { address, family, port }, where family is 'IPv4' or 'IPv6' and port
     *      is the real port after listen(0). For a unix socket or Windows pipe the address is the bound
     *      path while family/port are placeholders ('IPv4'/0). The method throws before the server is
     *      bound (number 20009); Node.js returns null instead and returns the path string for pipe
     *      servers.
     *
     *      Example — discovering the port assigned to listen(0):
     *      ```JavaScript
     *      const net = require('net');
     *
     *      const server = net.createServer((conn) => conn.close());
     *      server.listen(0, '127.0.0.1');
     *
     *      const addr = server.address();
     *      console.log(addr.address, addr.family, addr.port > 0); // 127.0.0.1 IPv4 true
     *
     *      server.stop();
     *      ```
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
     *
     *      The underlying listening Socket exposes the bound family/localAddress/localPort; it is useful
     *      for diagnostics, but do not call accept on it because the server owns the accept loop. It
     *      throws when the server was created by the handler-only constructor and is not bound yet.
     *      Node.js hides the listening handle.
     *
     */
    readonly socket: Class_Socket;

    /**
     * @description Queries and sets the timeout in milliseconds; this timeout is used for newly accepted connections
     *
     *      The default 0 means no timeout. The value is copied to each accepted Socket when the server
     *      accepts it, before the handler runs, so it bounds every recv/send of the handler unless the
     *      handler changes it. It does not apply to the listening socket itself.
     *
     */
    timeout: number;

    /**
     * @description the current event handling interface object of the server
     *
     *      The normalized Handler invoked for every connection. Assigning a value runs it through the
     *      Handler constructor: a function becomes a message handler wrapper, an array becomes a Chain
     *      and a path/address string or routing map is converted accordingly (see net.createServer for
     *      the accepted forms). The getter returns the last assigned object.
     *
     */
    handler: Class_Handler;

    /**
     * @description Emitted after start() is called and binding completes
     *
     *      Emitted synchronously by start()/listen(), after the socket is listening and before the first
     *      accept; observe it with on('listening') or the onlistening shorthand. Node.js emits
     *      'listening' asynchronously once the bind completes.
     *
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
     * @description Emitted after start() is called and binding completes
     *
     *      Emitted synchronously by start()/listen(), after the socket is listening and before the first
     *      accept; observe it with on('listening') or the onlistening shorthand. Node.js emits
     *      'listening' asynchronously once the bind completes.
     *
     */
    onlistening: (()=>void) | null;

    /**
     * @description Emitted when a new TCP connection is established
     *
     *      Emitted before the handler is invoked for the same Socket, on the accepting fiber; a long
     *      listener delays further accepts, so offload work to the handler or to a new fiber. Node.js
     *      emits 'connection' with its socket in the same way.
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
     * @description Emitted when a new TCP connection is established
     *
     *      Emitted before the handler is invoked for the same Socket, on the accepting fiber; a long
     *      listener delays further accepts, so offload work to the handler or to a new fiber. Node.js
     *      emits 'connection' with its socket in the same way.
     *      @param socket the newly established Socket connection object
     *
     */
    onconnection: ((socket: Class_Socket)=>void) | null;

    /**
     * @description Emitted when an error occurs
     *
     *      Emitted for accept-loop failures with a plain message string, not with an Error object as in
     *      Node.js. An exception thrown by the handler does not emit this event: it is logged and the
     *      connection is closed.
     *      @param msg the error message
     *
     */
    on(event: "error", listener: (msg: string)=>void): this;

    once(event: "error", listener: (msg: string)=>void): this;

    off(event: "error", listener: (msg: string)=>void): this;

    addListener(event: "error", listener: (msg: string)=>void): this;

    removeListener(event: "error", listener: (msg: string)=>void): this;

    addEventListener(event: "error", listener: (msg: string)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (msg: string)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: (msg: string)=>void): this;

    prependOnceListener(event: "error", listener: (msg: string)=>void): this;

    /**
     * @description Emitted when an error occurs
     *
     *      Emitted for accept-loop failures with a plain message string, not with an Error object as in
     *      Node.js. An exception thrown by the handler does not emit this event: it is logged and the
     *      connection is closed.
     *      @param msg the error message
     *
     */
    onerror: ((msg: string)=>void) | null;

    /**
     * @description Emitted after the server is closed
     *
     *      Emitted by stop()/close() immediately after the listening socket is closed, even when
     *      connections are still open. Node.js emits 'close' only after the server has stopped
     *      accepting and all connections have ended.
     *
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
     * @description Emitted after the server is closed
     *
     *      Emitted by stop()/close() immediately after the listening socket is closed, even when
     *      connections are still open. Node.js emits 'close' only after the server has stopped
     *      accepting and all connections have ended.
     *
     */
    onclose: (()=>void) | null;

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
/// <reference path="../interface/Socket.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the TcpServer class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TcpServerPromise extends Class_EventEmitter {
    /**
     * @description TcpServer constructor, binds a port on all local addresses
     *
     *     listener may be given in any of these forms:
     *     - a Handler object, invoked as it is;
     *     - an array of handlers, wrapped in a Chain and invoked in order;
     *     - a handler function `(socket) => any`, called with each accepted connection (a Socket) in its
     *       own fiber, so the handler may read and write in a loop;
     *     - a routing map object, whose keys are match patterns and whose values are handlers in these
     *       same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *     - a path/address string: a directory or an `http(s)://` address, converted through the Handler
     *       constructor.
     *     The server binds port on all local addresses and starts listening; start() begins accepting.
     *     Port 0 asks the operating system for an ephemeral port, read it from address() after start().
     *     An exception thrown by the handler is logged and the connection is closed.
     *     @param port specifies the tcp server listening port
     *     @param listener the connection handler
     *
     */
    constructor(port: number, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description TcpServer constructor, binds the given address and port
     *
     *      addr is an IP literal; '' means listening on all local addresses and '::' all IPv6 addresses.
     *      port may be 0 for an OS-assigned port. A unix socket or Windows pipe path is also accepted and
     *      the port is then ignored. The listener forms are described on the port-only overload; start()
     *      begins accepting after the constructor has bound and listened.
     *      @param addr specifies the tcp server listening address; "" means listening on all local addresses
     *      @param port specifies the tcp server listening port
     *      @param listener the connection handler
     *
     */
    constructor(addr: string, port: number, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description TcpServer constructor, does not bind a port, listen() must be called to start
     *
     *     The fully deferred form: only the handler is stored. Call listen(port[, addr[, backlog]]) or
     *     bind through another constructor later; address() and socket throw until the server is bound.
     *     The listener forms are described on the port-only overload.
     *     @param listener the connection handler
     *
     */
    constructor(listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description TcpServer constructor, binds from an options object
     *
     *      options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "address": "", // the listening address, defaults to all addresses
     *          "port": 0      // the listening port; without it nothing is bound and listen() is required
     *      })
     *      ```
     *      With both properties the server binds address:port immediately; with address only it binds
     *      that address with an OS-assigned port (or a unix path when the address is not an IP literal);
     *      with neither it only stores the handler and listen() must be called. These are bind options,
     *      not the socket options of Node.js net.createServer. The listener forms are described on the
     *      port-only overload.
     *      @param options server options
     *      @param listener the connection handler
     *
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description TcpServer constructor, binds a unix socket or Windows pipe
     *
     *     addr is the path to bind, for example '/tmp/app.sock' or '\\.\pipe\app' (watch the string
     *     escapes). An all-digit string is treated as a TCP port instead, so
     *     new net.TcpServer('8080', handler) listens on port 8080. The path is bound during construction
     *     and start() begins accepting. Node.js uses server.listen(path) for the same purpose.
     *     @param addr specifies the unix socket or Windows pipe server listening address
     *     @param listener the connection handler
     *
     */
    constructor(addr: string, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Starts the current server
     *
     *      Begins the accept loop and emits 'listening'. The server must already be bound: the
     *      constructors with a port/address bind in the constructor while the handler-only form needs
     *      listen(). Calling start on an unbound server or a second time fails with an invalid-call
     *      error. Each accepted client is passed to the 'connection' listeners and then to the handler.
     *
     */
    start(): void;

    /**
     * @description Binds the address and port and starts listening for connections
     *
     *      Binds and starts the accept loop in one call, emitting 'listening'. Port 0 asks the operating
     *      system for an ephemeral port, read it from address(). backlog -1 passes the system default to
     *      the operating system. A second listen on the same server throws ERR_SERVER_ALREADY_LISTEN,
     *      and a failed bind (for example EADDRINUSE) carries syscall 'listen' like Node.js.
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listen(port: number, addr?: string, backlog?: number): Promise<void>;

    /**
     * @description Binds the address and port and starts listening for connections
     *
     *      Binds and starts the accept loop in one call, emitting 'listening'. Port 0 asks the operating
     *      system for an ephemeral port, read it from address(). backlog -1 passes the system default to
     *      the operating system. A second listen on the same server throws ERR_SERVER_ALREADY_LISTEN,
     *      and a failed bind (for example EADDRINUSE) carries syscall 'listen' like Node.js.
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listenSync(port: number, addr?: string, backlog?: number): void;

    /**
     * @description Binds the address and port and starts listening for connections
     *
     *      Binds and starts the accept loop in one call, emitting 'listening'. Port 0 asks the operating
     *      system for an ephemeral port, read it from address(). backlog -1 passes the system default to
     *      the operating system. A second listen on the same server throws ERR_SERVER_ALREADY_LISTEN,
     *      and a failed bind (for example EADDRINUSE) carries syscall 'listen' like Node.js.
     *     @param port specifies the TCP server listening port
     *     @param addr specifies the TCP server listening address; "" means listening on all local addresses
     *     @param backlog specifies the maximum length of the connection queue, -1 means using the system default
     *
     */
    listenAsync(port: number, addr?: string, backlog?: number): Promise<void>;

    /**
     * @description Closes the socket and aborts the running server
     *
     *      Closes the listening socket and emits 'close' immediately; the accepted connections are not
     *      closed and keep running in their handler fibers (the handler owns them). Stopping a server
     *      that was never bound is a no-op. Node.js server.close() instead waits for the active
     *      connections to end before emitting 'close'.
     *
     */
    stop(): Promise<void>;

    /**
     * @description Closes the socket and aborts the running server
     *
     *      Closes the listening socket and emits 'close' immediately; the accepted connections are not
     *      closed and keep running in their handler fibers (the handler owns them). Stopping a server
     *      that was never bound is a no-op. Node.js server.close() instead waits for the active
     *      connections to end before emitting 'close'.
     *
     */
    stopSync(): void;

    /**
     * @description Closes the socket and aborts the running server
     *
     *      Closes the listening socket and emits 'close' immediately; the accepted connections are not
     *      closed and keep running in their handler fibers (the handler owns them). Stopping a server
     *      that was never bound is a no-op. Node.js server.close() instead waits for the active
     *      connections to end before emitting 'close'.
     *
     */
    stopAsync(): Promise<void>;

    /**
     * @description Closes the socket and aborts the running server; an alias of stop()
     *
     *      Identical to stop(), provided for the Node.js naming; both are awaitable.
     *
     */
    close(): Promise<void>;

    /**
     * @description Closes the socket and aborts the running server; an alias of stop()
     *
     *      Identical to stop(), provided for the Node.js naming; both are awaitable.
     *
     */
    closeSync(): void;

    /**
     * @description Closes the socket and aborts the running server; an alias of stop()
     *
     *      Identical to stop(), provided for the Node.js naming; both are awaitable.
     *
     */
    closeAsync(): Promise<void>;

    /**
     * @description Returns an object containing the server bound address, address family and port. Used to look up the actual port when the OS assigns the address.
     *
     *      The result has the shape { address, family, port }, where family is 'IPv4' or 'IPv6' and port
     *      is the real port after listen(0). For a unix socket or Windows pipe the address is the bound
     *      path while family/port are placeholders ('IPv4'/0). The method throws before the server is
     *      bound (number 20009); Node.js returns null instead and returns the path string for pipe
     *      servers.
     *
     *      Example — discovering the port assigned to listen(0):
     *      ```JavaScript
     *      const net = require('net');
     *
     *      const server = net.createServer((conn) => conn.close());
     *      server.listen(0, '127.0.0.1');
     *
     *      const addr = server.address();
     *      console.log(addr.address, addr.family, addr.port > 0); // 127.0.0.1 IPv4 true
     *
     *      server.stop();
     *      ```
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
     *
     *      The underlying listening Socket exposes the bound family/localAddress/localPort; it is useful
     *      for diagnostics, but do not call accept on it because the server owns the accept loop. It
     *      throws when the server was created by the handler-only constructor and is not bound yet.
     *      Node.js hides the listening handle.
     *
     */
    readonly socket: Class_SocketPromise;

    /**
     * @description Queries and sets the timeout in milliseconds; this timeout is used for newly accepted connections
     *
     *      The default 0 means no timeout. The value is copied to each accepted Socket when the server
     *      accepts it, before the handler runs, so it bounds every recv/send of the handler unless the
     *      handler changes it. It does not apply to the listening socket itself.
     *
     */
    timeout: number;

    /**
     * @description the current event handling interface object of the server
     *
     *      The normalized Handler invoked for every connection. Assigning a value runs it through the
     *      Handler constructor: a function becomes a message handler wrapper, an array becomes a Chain
     *      and a path/address string or routing map is converted accordingly (see net.createServer for
     *      the accepted forms). The getter returns the last assigned object.
     *
     */
    handler: Class_HandlerPromise;

    /**
     * @description Emitted after start() is called and binding completes
     *
     *      Emitted synchronously by start()/listen(), after the socket is listening and before the first
     *      accept; observe it with on('listening') or the onlistening shorthand. Node.js emits
     *      'listening' asynchronously once the bind completes.
     *
     */
    onlistening: (()=>void) | null;

    /**
     * @description Emitted when a new TCP connection is established
     *
     *      Emitted before the handler is invoked for the same Socket, on the accepting fiber; a long
     *      listener delays further accepts, so offload work to the handler or to a new fiber. Node.js
     *      emits 'connection' with its socket in the same way.
     *      @param socket the newly established Socket connection object
     *
     */
    onconnection: ((socket: Class_Socket)=>void) | null;

    /**
     * @description Emitted when an error occurs
     *
     *      Emitted for accept-loop failures with a plain message string, not with an Error object as in
     *      Node.js. An exception thrown by the handler does not emit this event: it is logged and the
     *      connection is closed.
     *      @param msg the error message
     *
     */
    onerror: ((msg: string)=>void) | null;

    /**
     * @description Emitted after the server is closed
     *
     *      Emitted by stop()/close() immediately after the listening socket is closed, even when
     *      connections are still open. Node.js emits 'close' only after the server has stopped
     *      accepting and all connections have ended.
     *
     */
    onclose: (()=>void) | null;

}


declare namespace Class_TcpServer {
    const promises: FIBJS.GeneralObject;
}
