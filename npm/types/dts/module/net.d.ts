/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Socket.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Smtp.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/**
 * @description the net module provides basic network capabilities, including establishing TCP connections, domain name resolution, IP address detection and creating TCP servers; it is the foundation of network modules such as http, tls and smtp
 *
 *  Main capabilities of the module:
 *
 *  - **Connection**: `connect` establishes TCP connections in multiple forms, supporting `tcp://`, `ssl://`, `unix:` and `pipe://` protocols;
 *  - **Resolution**: `resolve`, `ip` and `ipv6` query the addresses of host names;
 *  - **Server**: `createServer` creates a TCP server;
 *  - **Detection**: `isIP`, `isIPv4` and `isIPv6` detect IP address formats;
 *  - **Object aliases**: `Socket`, `Smtp`, `TcpServer` and `Url`.
 *
 *  Referenced as:
 *
 *  ```JavaScript
 *  var net = require('net');
 *  ```
 *
 *  Example of establishing a TCP connection:
 *
 *  ```JavaScript
 *  var net = require('net');
 *
 *  // specify the port and host
 *  var sock = net.connect(80, 'example.com');
 *  sock.send('GET / HTTP/1.0\r\n\r\n');
 *  console.log(sock.recv());
 *  sock.close();
 *
 *  // use the URL form, supporting tcp:// and ssl:// protocols
 *  var ssl = net.connect('ssl://example.com:443');
 *  ```
 *
 */
declare module 'net' {
    /**
     * @description address family constant, specifies unix socket
     */
    export const AF_UNIX: 1;

    /**
     * @description address family constant, specifies Windows pipe
     */
    export const AF_PIPE: 1;

    /**
     * @description address family constant, specifies ipv4
     */
    export const AF_INET: 2;

    /**
     * @description address family constant, specifies ipv6
     */
    export const AF_INET6: 10;

    /**
     * @description queries and sets whether the socket backend uses uv, default is false
     */
    var use_uv_socket: boolean;

    /**
     * @description queries the network information of the current runtime environment
     *      @return returns the network interface information
     *
     */
    function info(): FIBJS.GeneralObject;

    /**
     * @description queries the address of the given host name
     *
     *      family specifies the address family to return, with values AF_INET or AF_INET6; other values throw an exception.
     *      @param name specifies the host name
     *      @param family specifies the type returned by the query, default is AF_INET
     *      @return returns the queried ip string
     *
     */
    function resolve(name: string, family?: number): string;

    function resolve(name: string, family?: number, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description queries the address of the given host name
     *
     *      family specifies the address family to return, with values AF_INET or AF_INET6; other values throw an exception.
     *      @param name specifies the host name
     *      @param family specifies the type returned by the query, default is AF_INET
     *      @return returns the queried ip string
     *
     */
    function resolveSync(name: string, family?: number): string;

    /**
     * @description queries the address of the given host name
     *
     *      family specifies the address family to return, with values AF_INET or AF_INET6; other values throw an exception.
     *      @param name specifies the host name
     *      @param family specifies the type returned by the query, default is AF_INET
     *      @return returns the queried ip string
     *
     */
    function resolveAsync(name: string, family?: number): Promise<string>;

    /**
     * @description quickly queries the host address, equivalent to resolve(name)
     *      @param name specifies the host name
     *      @return returns the queried ip string
     *
     */
    function ip(name: string): string;

    function ip(name: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description quickly queries the host address, equivalent to resolve(name)
     *      @param name specifies the host name
     *      @return returns the queried ip string
     *
     */
    function ipSync(name: string): string;

    /**
     * @description quickly queries the host address, equivalent to resolve(name)
     *      @param name specifies the host name
     *      @return returns the queried ip string
     *
     */
    function ipAsync(name: string): Promise<string>;

    /**
     * @description quickly queries the host ipv6 address, equivalent to resolve(name, net.AF_INET6)
     *      @param name specifies the host name
     *      @return returns the queried ipv6 string
     *
     */
    function ipv6(name: string): string;

    function ipv6(name: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description quickly queries the host ipv6 address, equivalent to resolve(name, net.AF_INET6)
     *      @param name specifies the host name
     *      @return returns the queried ipv6 string
     *
     */
    function ipv6Sync(name: string): string;

    /**
     * @description quickly queries the host ipv6 address, equivalent to resolve(name, net.AF_INET6)
     *      @param name specifies the host name
     *      @return returns the queried ipv6 string
     *
     */
    function ipv6Async(name: string): Promise<string>;

    /**
     * @description creates a Socket object, see Socket
     */
    const Socket: typeof Class_Socket;

    /**
     * @description creates a Socket object and establishes a connection
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
    function connect(options: FIBJS.GeneralObject): Class_Stream;

    function connect(options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description creates a Socket object and establishes a connection
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
    function connectSync(options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description creates a Socket object and establishes a connection
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
    function connectAsync(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object (with the port/host/timeout properties), the remote port, or the unix socket path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object (with the port/host/timeout properties), the remote port, or the unix socket path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param options specifies the connection options object (with the port/host/timeout properties), the remote port, or the unix socket path
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a Socket object and establishes a connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host?: string, timeout?: number): Class_Stream;

    function connect(port: number, host?: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description creates a Socket object and establishes a connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host?: string, timeout?: number): Class_Stream;

    /**
     * @description creates a Socket object and establishes a connection
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host?: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description creates a Socket or SslSocket object and establishes a connection
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port, or unix:/usr/local/proc1 or pipe://./pipe/proc1; when connecting to a pipe, replace `\` with `/`
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket or SslSocket object
     *
     */
    function connect(url: string, timeout?: number): Class_Stream;

    function connect(url: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description creates a Socket or SslSocket object and establishes a connection
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port, or unix:/usr/local/proc1 or pipe://./pipe/proc1; when connecting to a pipe, replace `\` with `/`
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket or SslSocket object
     *
     */
    function connectSync(url: string, timeout?: number): Class_Stream;

    /**
     * @description creates a Socket or SslSocket object and establishes a connection
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port, or unix:/usr/local/proc1 or pipe://./pipe/proc1; when connecting to a pipe, replace `\` with `/`
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket or SslSocket object
     *
     */
    function connectAsync(url: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(path: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(path: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description establishes a connection and triggers the connect event after the connection is established
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(path: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a Smtp object, see Smtp
     */
    const Smtp: typeof Class_Smtp;

    /**
     * @description creates a Smtp object and establishes a connection, see Smtp
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Smtp object
     *
     */
    function openSmtp(url: string, timeout?: number): Class_Smtp;

    function openSmtp(url: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Smtp)=>any): void;

    /**
     * @description creates a Smtp object and establishes a connection, see Smtp
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Smtp object
     *
     */
    function openSmtpSync(url: string, timeout?: number): Class_Smtp;

    /**
     * @description creates a Smtp object and establishes a connection, see Smtp
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Smtp object
     *
     */
    function openSmtpAsync(url: string, timeout?: number): Promise<Class_SmtpPromise>;

    /**
     * @description creates a TcpServer object, see TcpServer
     */
    const TcpServer: typeof Class_TcpServer;

    /**
     * @description creates a TCP server
     *      @param options the server options object, which can contain the following properties:
     *       - address: specifies the listening address, default is all addresses
     *       - port: specifies the listening port, optional. When not provided, listen() must be called to start
     *      @param listener the connection handler function
     *      @return returns the TcpServer object
     *
     */
    function createServer(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise): Class_TcpServer;

    /**
     * @description creates a TCP server
     *      @param listener the connection handler function
     *      @return returns a TcpServer object not bound to a port; listen() must be called to start it
     *
     */
    function createServer(listener: Class_Handler | Class_HandlerPromise): Class_TcpServer;

    /**
     * @description creates a UrlObject object, see UrlObject
     */
    const Url: typeof Class_UrlObject;

    /**
     * @description queries the asynchronous network engine of the current system
     *      @return returns the network engine name
     *
     */
    function backend(): string;

    /**
     * @description detects whether the input is an IP address
     *      @param ip the value to detect; a non-string value is not an IP address
     *      @return returns 0 for an invalid IP address, 4 for IPv4 and 6 for IPv6
     *
     */
    function isIP(ip?: any): number;

    /**
     * @description detects whether the input is an IPv4 address
     *      @param ip the value to detect; a non-string value is not an IP address
     *      @return returns true if it is IPv4, otherwise returns false
     *
     */
    function isIPv4(ip?: any): boolean;

    /**
     * @description detects whether the input is an IPv6 address
     *      @param ip the value to detect; a non-string value is not an IP address
     *      @return returns true if it is IPv6, otherwise returns false
     *
     */
    function isIPv6(ip?: any): boolean;

    /**
     * @description queries whether net.connect enables automatic address family selection by default, compatible with Node.js >= 18.13
     *      @return returns the current default value, default is true
     *
     */
    function getDefaultAutoSelectFamily(): boolean;

    /**
     * @description sets whether net.connect enables automatic address family selection by default, compatible with Node.js >= 18.13
     *      @param enabled specifies the default value, which must be a boolean
     *
     */
    function setDefaultAutoSelectFamily(enabled: boolean): void;

    /**
     * @description queries the default automatic address family selection timeout, compatible with Node.js >= 18.13
     *      @return returns the current default timeout in milliseconds, default is 250
     *
     */
    function getDefaultAutoSelectFamilyAttemptTimeout(): number;

    /**
     * @description sets the default automatic address family selection timeout, compatible with Node.js >= 18.13
     *      @param milliseconds specifies the default timeout in milliseconds, which must be an integer greater than or equal to 10
     *
     */
    function setDefaultAutoSelectFamilyAttemptTimeout(milliseconds: number): void;

}

