/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description network socket object
 *
 *  Socket belongs to the net module and provides connection, listening and data transfer capabilities for TCP, unix socket and Windows pipe. It can be created with:
 *  ```JavaScript
 *  var s = new net.Socket();
 *  ```
 *
 *  Socket inherits from Stream and provides streaming read/write capabilities, plus the following network features:
 *
 *  - **Connection**: `connect` establishes connections in multiple forms; after connecting, `send`/`recv` can be used to transfer data;
 *  - **Server**: `bind` binds an address and port, `listen` starts listening, `accept` accepts connections;
 *  - **Tuning**: `setKeepAlive` keeps the connection alive, `setNoDelay` disables the Nagle algorithm, `setTimeout`/`timeout` control timeouts;
 *  - **Status**: `remoteAddress`/`remotePort`/`localAddress`/`localPort` query connection address information, `isAlive` checks connection availability.
 *
 */
declare class Class_Socket extends Class_Stream {
    /**
     * @description Socket constructor, creates a new Socket object
     *      @param family specifies the address family, default is AF_INET, ipv4
     *
     */
    constructor(family?: number);

    /**
     * @description queries the address family of the current Socket object
     */
    readonly family: number;

    /**
     * @description queries the remote address of the current connection
     */
    readonly remoteAddress: string;

    /**
     * @description queries the remote port of the current connection
     */
    readonly remotePort: number;

    /**
     * @description queries the local address of the current connection
     */
    readonly localAddress: string;

    /**
     * @description queries the local port of the current connection
     */
    readonly localPort: number;

    /**
     * @description queries and sets the timeout in milliseconds
     */
    timeout: number;

    /**
     * @description establishes a tcp connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connect(port: number, host?: string, timeout?: number): Class_Stream;

    connect(port: number, host?: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description establishes a tcp connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connectSync(port: number, host?: string, timeout?: number): Class_Stream;

    /**
     * @description establishes a tcp connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connectAsync(port: number, host?: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description establishes a unix socket or Windows pipe connection
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connect(path: string, timeout?: number): Class_Stream;

    connect(path: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description establishes a unix socket or Windows pipe connection
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connectSync(path: string, timeout?: number): Class_Stream;

    /**
     * @description establishes a unix socket or Windows pipe connection
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connectAsync(path: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection
     *
     *      The options parameter can contain the following properties:
     *       - port: specifies the remote port
     *       - host: specifies the remote address or host name
     *       - timeout: specifies the timeout in milliseconds, default is 0
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    connect(options: FIBJS.GeneralObject): Class_Stream;

    connect(options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description establishes a connection
     *
     *      The options parameter can contain the following properties:
     *       - port: specifies the remote port
     *       - host: specifies the remote address or host name
     *       - timeout: specifies the timeout in milliseconds, default is 0
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    connectSync(options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description establishes a connection
     *
     *      The options parameter can contain the following properties:
     *       - port: specifies the remote port
     *       - host: specifies the remote address or host name
     *       - timeout: specifies the timeout in milliseconds, default is 0
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    connectAsync(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(port: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(port: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(port: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(port: number, host: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(port: number, host: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(port: number, host: string, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(port: number, host: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(port: number, host: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(port: number, host: string, timeout: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(path: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(path: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(path: string, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(path: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(path: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(path: string, timeout: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object, which can contain the following properties:
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(options: FIBJS.GeneralObject, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object, which can contain the following properties:
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(options: FIBJS.GeneralObject, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object, which can contain the following properties:
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(options: FIBJS.GeneralObject, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description binds the current Socket to the specified port on all local addresses
     *      @param port specifies the port to bind
     *      @param allowIPv4 specifies whether to accept ipv4 connections, default is true. This parameter is effective for ipv6 and depends on the operating system
     *
     */
    bind(port: number, allowIPv4?: boolean): void;

    /**
     * @description binds the current Socket to the specified port on the specified address
     *      @param addr specifies the address to bind, which can also refer to a unix socket or Windows pipe path
     *      @param port specifies the port to bind; this parameter is ignored when binding a unix socket or Windows pipe
     *      @param allowIPv4 specifies whether to accept ipv4 connections, default is true. This parameter is effective for ipv6 and depends on the operating system
     *
     */
    bind(addr: string, port?: number, allowIPv4?: boolean): void;

    /**
     * @description starts listening for connection requests
     *      @param backlog specifies the request queue length; requests beyond it will be rejected, default is 120
     *
     */
    listen(backlog?: number): void;

    /**
     * @description waits for and accepts a connection
     *      @return returns the accepted connection object
     *
     */
    accept(): Class_Socket;

    accept(callback: (err: Error | undefined | null, retVal: Class_Socket)=>any): void;

    /**
     * @description waits for and accepts a connection
     *      @return returns the accepted connection object
     *
     */
    acceptSync(): Class_Socket;

    /**
     * @description waits for and accepts a connection
     *      @return returns the accepted connection object
     *
     */
    acceptAsync(): Promise<Class_SocketPromise>;

    /**
     * @description enables or disables the TCP keep-alive mechanism
     *      @param enable specifies whether to enable the keep-alive mechanism, default is false
     *      @param initialDelay specifies the initial delay in seconds, default is 0
     *
     */
    setKeepAlive(enable?: boolean, initialDelay?: number): void;

    /**
     * @description enables or disables the Nagle algorithm
     *      @param noDelay specifies whether to disable the Nagle algorithm, default is true
     *
     */
    setNoDelay(noDelay?: boolean): void;

    /**
     * @description checks whether the socket currently appears to be still usable
     *
     *      This method performs a best-effort non-blocking check and does not consume any received data.
     *      Returning false means the socket is definitely unusable; returning true only means no closed state has been detected so far.
     *      @return returns whether the socket currently appears to be still usable
     *
     */
    isAlive(): boolean;

    /**
     * @description reads the specified amount of data from the connection; unlike the read method, recv does not guarantee reading all the requested data, but returns immediately after data is read
     *      @param bytes specifies the amount of data to read; by default any size of data is read
     *      @return returns the data read from the connection
     *
     */
    recv(bytes?: number): Class_Buffer;

    recv(bytes?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description reads the specified amount of data from the connection; unlike the read method, recv does not guarantee reading all the requested data, but returns immediately after data is read
     *      @param bytes specifies the amount of data to read; by default any size of data is read
     *      @return returns the data read from the connection
     *
     */
    recvSync(bytes?: number): Class_Buffer;

    /**
     * @description reads the specified amount of data from the connection; unlike the read method, recv does not guarantee reading all the requested data, but returns immediately after data is read
     *      @param bytes specifies the amount of data to read; by default any size of data is read
     *      @return returns the data read from the connection
     *
     */
    recvAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description writes the given data to the connection, equivalent to the write method
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    send(data: Class_Buffer): number;

    send(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description writes the given data to the connection, equivalent to the write method
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    sendSync(data: Class_Buffer): number;

    /**
     * @description writes the given data to the connection, equivalent to the write method
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    sendAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description aborts all ongoing operations on the current socket
     *
     *      This method cancels all pending asynchronous operations (connect, recv, send, etc.),
     *      and the canceled operations return an error. The socket itself is not closed and can continue to be used.
     *
     */
    abort(): void;

    /**
     * @description sets the socket timeout
     *      @param timeout the timeout in milliseconds. Setting it to 0 disables the timeout.
     *      @return returns the current Socket object
     *
     */
    setTimeout(timeout: number): Class_Socket;

    /**
     * @description sets the socket timeout and registers a one-time 'timeout' event listener
     *      @param timeout the timeout in milliseconds. Setting it to 0 disables the timeout.
     *      @param callback the callback function, called once when the socket times out
     *      @return returns the current Socket object
     *
     */
    setTimeout(timeout: number, callback: (...args: any[])=>any): Class_Socket;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the Socket class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_SocketPromise extends Class_StreamPromise {
    /**
     * @description Socket constructor, creates a new Socket object
     *      @param family specifies the address family, default is AF_INET, ipv4
     *
     */
    constructor(family?: number);

    /**
     * @description queries the address family of the current Socket object
     */
    readonly family: number;

    /**
     * @description queries the remote address of the current connection
     */
    readonly remoteAddress: string;

    /**
     * @description queries the remote port of the current connection
     */
    readonly remotePort: number;

    /**
     * @description queries the local address of the current connection
     */
    readonly localAddress: string;

    /**
     * @description queries the local port of the current connection
     */
    readonly localPort: number;

    /**
     * @description queries and sets the timeout in milliseconds
     */
    timeout: number;

    /**
     * @description establishes a tcp connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connect(port: number, host?: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description establishes a tcp connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connectSync(port: number, host?: string, timeout?: number): Class_Stream;

    /**
     * @description establishes a tcp connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connectAsync(port: number, host?: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description establishes a unix socket or Windows pipe connection
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connect(path: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description establishes a unix socket or Windows pipe connection
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connectSync(path: string, timeout?: number): Class_Stream;

    /**
     * @description establishes a unix socket or Windows pipe connection
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    connectAsync(path: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection
     *
     *      The options parameter can contain the following properties:
     *       - port: specifies the remote port
     *       - host: specifies the remote address or host name
     *       - timeout: specifies the timeout in milliseconds, default is 0
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    connect(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection
     *
     *      The options parameter can contain the following properties:
     *       - port: specifies the remote port
     *       - host: specifies the remote address or host name
     *       - timeout: specifies the timeout in milliseconds, default is 0
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    connectSync(options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description establishes a connection
     *
     *      The options parameter can contain the following properties:
     *       - port: specifies the remote port
     *       - host: specifies the remote address or host name
     *       - timeout: specifies the timeout in milliseconds, default is 0
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    connectAsync(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(port: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(port: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(port: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(port: number, host: string, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(port: number, host: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(port: number, host: string, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(port: number, host: string, timeout: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(port: number, host: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(port: number, host: string, timeout: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(path: string, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(path: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(path: string, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(path: string, timeout: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(path: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(path: string, timeout: number, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object, which can contain the following properties:
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connect(options: FIBJS.GeneralObject, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object, which can contain the following properties:
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectSync(options: FIBJS.GeneralObject, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object, which can contain the following properties:
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    connectAsync(options: FIBJS.GeneralObject, connectListener: (...args: any[])=>any): Promise<Class_StreamPromise>;

    /**
     * @description binds the current Socket to the specified port on all local addresses
     *      @param port specifies the port to bind
     *      @param allowIPv4 specifies whether to accept ipv4 connections, default is true. This parameter is effective for ipv6 and depends on the operating system
     *
     */
    bind(port: number, allowIPv4?: boolean): void;

    /**
     * @description binds the current Socket to the specified port on the specified address
     *      @param addr specifies the address to bind, which can also refer to a unix socket or Windows pipe path
     *      @param port specifies the port to bind; this parameter is ignored when binding a unix socket or Windows pipe
     *      @param allowIPv4 specifies whether to accept ipv4 connections, default is true. This parameter is effective for ipv6 and depends on the operating system
     *
     */
    bind(addr: string, port?: number, allowIPv4?: boolean): void;

    /**
     * @description starts listening for connection requests
     *      @param backlog specifies the request queue length; requests beyond it will be rejected, default is 120
     *
     */
    listen(backlog?: number): void;

    /**
     * @description waits for and accepts a connection
     *      @return returns the accepted connection object
     *
     */
    accept(): Promise<Class_SocketPromise>;

    /**
     * @description waits for and accepts a connection
     *      @return returns the accepted connection object
     *
     */
    acceptSync(): Class_Socket;

    /**
     * @description waits for and accepts a connection
     *      @return returns the accepted connection object
     *
     */
    acceptAsync(): Promise<Class_SocketPromise>;

    /**
     * @description enables or disables the TCP keep-alive mechanism
     *      @param enable specifies whether to enable the keep-alive mechanism, default is false
     *      @param initialDelay specifies the initial delay in seconds, default is 0
     *
     */
    setKeepAlive(enable?: boolean, initialDelay?: number): void;

    /**
     * @description enables or disables the Nagle algorithm
     *      @param noDelay specifies whether to disable the Nagle algorithm, default is true
     *
     */
    setNoDelay(noDelay?: boolean): void;

    /**
     * @description checks whether the socket currently appears to be still usable
     *
     *      This method performs a best-effort non-blocking check and does not consume any received data.
     *      Returning false means the socket is definitely unusable; returning true only means no closed state has been detected so far.
     *      @return returns whether the socket currently appears to be still usable
     *
     */
    isAlive(): boolean;

    /**
     * @description reads the specified amount of data from the connection; unlike the read method, recv does not guarantee reading all the requested data, but returns immediately after data is read
     *      @param bytes specifies the amount of data to read; by default any size of data is read
     *      @return returns the data read from the connection
     *
     */
    recv(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description reads the specified amount of data from the connection; unlike the read method, recv does not guarantee reading all the requested data, but returns immediately after data is read
     *      @param bytes specifies the amount of data to read; by default any size of data is read
     *      @return returns the data read from the connection
     *
     */
    recvSync(bytes?: number): Class_Buffer;

    /**
     * @description reads the specified amount of data from the connection; unlike the read method, recv does not guarantee reading all the requested data, but returns immediately after data is read
     *      @param bytes specifies the amount of data to read; by default any size of data is read
     *      @return returns the data read from the connection
     *
     */
    recvAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description writes the given data to the connection, equivalent to the write method
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    send(data: Class_Buffer): Promise<number>;

    /**
     * @description writes the given data to the connection, equivalent to the write method
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    sendSync(data: Class_Buffer): number;

    /**
     * @description writes the given data to the connection, equivalent to the write method
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    sendAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description aborts all ongoing operations on the current socket
     *
     *      This method cancels all pending asynchronous operations (connect, recv, send, etc.),
     *      and the canceled operations return an error. The socket itself is not closed and can continue to be used.
     *
     */
    abort(): void;

    /**
     * @description sets the socket timeout
     *      @param timeout the timeout in milliseconds. Setting it to 0 disables the timeout.
     *      @return returns the current Socket object
     *
     */
    setTimeout(timeout: number): Class_Socket;

    /**
     * @description sets the socket timeout and registers a one-time 'timeout' event listener
     *      @param timeout the timeout in milliseconds. Setting it to 0 disables the timeout.
     *      @param callback the callback function, called once when the socket times out
     *      @return returns the current Socket object
     *
     */
    setTimeout(timeout: number, callback: (...args: any[])=>any): Class_Socket;

}


declare namespace Class_Socket {
    const promises: FIBJS.GeneralObject;
}
