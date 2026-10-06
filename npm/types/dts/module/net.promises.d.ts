/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Socket.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Smtp.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/**
 * The promise variant of the net module: async members return a Promise as their primary form.
 */
declare module 'net/promises' {
    /**
     * @description Address family constant that selects a unix socket or Windows named pipe; it has the same value as AF_PIPE
     */
    export const AF_UNIX: 1;

    /**
     * @description Address family constant that selects a Windows named pipe; it has the same value as AF_UNIX and is only meaningful on Windows
     */
    export const AF_PIPE: 1;

    /**
     * @description Address family constant that selects IPv4; it is the default of new net.Socket()
     */
    export const AF_INET: 2;

    /**
     * @description Address family constant that selects IPv6
     */
    export const AF_INET6: 10;

    /**
     * @description Queries and sets whether sockets created afterwards use the libuv backend instead of the platform engine; the default is false
     *
     *      The flag is process-wide and is read when a Socket object is created, so it only affects later
     *      sockets and existing sockets keep the engine they were created with. Setting it to true routes
     *      sockets through libuv (a single loop, the backend Node.js uses); leaving it false uses the
     *      platform engine reported by backend(). The backends differ in a few edge cases, for example a
     *      read on a socket closed by another fiber reports EBADF/EPERM on the platform engine but
     *      CALL_E_CLOSED_SOCKET on the libuv backend, and some timeouts use different error numbers.
     *
     */
    var use_uv_socket: boolean;

    /**
     * @description Queries the network information of the current runtime environment
     *
     *      The result has one property per interface name holding an array of address entries with the
     *      same shape as os.networkInterfaces(): address, family ('IPv4' or 'IPv6'), mac, internal and
     *      netmask. The call is synchronous and does not perform any lookup; Node.js exposes the same
     *      data through os.networkInterfaces().
     *      @return returns the network interface information
     *
     */
    function info(): FIBJS.GeneralObject;

    /**
     * @description Queries the address of the given host name
     *
     *      The lookup goes through the system resolver (getaddrinfo) and returns the first address that
     *      matches the requested family, so a name with both A and AAAA records returns its IPv4 address
     *      by default. The result is always a single address; there is no all/hints equivalent of the
     *      Node.js dns module. family must be net.AF_INET (default) or net.AF_INET6, any other value
     *      throws a TypeError. A name that cannot be resolved fails with an Error carrying syscall
     *      'getaddrinfo' and a resolver code such as ENOTFOUND, which distinguishes a missing host from a
     *      local resolver failure. The call yields the current fiber while the resolver works.
     *      @param name specifies the host name
     *      @param family specifies the type returned by the query, default is AF_INET
     *      @return returns the queried ip string
     *
     */
    function resolve(name: string, family?: number): Promise<string>;

    /**
     * @description Queries the address of the given host name
     *
     *      The lookup goes through the system resolver (getaddrinfo) and returns the first address that
     *      matches the requested family, so a name with both A and AAAA records returns its IPv4 address
     *      by default. The result is always a single address; there is no all/hints equivalent of the
     *      Node.js dns module. family must be net.AF_INET (default) or net.AF_INET6, any other value
     *      throws a TypeError. A name that cannot be resolved fails with an Error carrying syscall
     *      'getaddrinfo' and a resolver code such as ENOTFOUND, which distinguishes a missing host from a
     *      local resolver failure. The call yields the current fiber while the resolver works.
     *      @param name specifies the host name
     *      @param family specifies the type returned by the query, default is AF_INET
     *      @return returns the queried ip string
     *
     */
    function resolveSync(name: string, family?: number): string;

    /**
     * @description Queries the address of the given host name
     *
     *      The lookup goes through the system resolver (getaddrinfo) and returns the first address that
     *      matches the requested family, so a name with both A and AAAA records returns its IPv4 address
     *      by default. The result is always a single address; there is no all/hints equivalent of the
     *      Node.js dns module. family must be net.AF_INET (default) or net.AF_INET6, any other value
     *      throws a TypeError. A name that cannot be resolved fails with an Error carrying syscall
     *      'getaddrinfo' and a resolver code such as ENOTFOUND, which distinguishes a missing host from a
     *      local resolver failure. The call yields the current fiber while the resolver works.
     *      @param name specifies the host name
     *      @param family specifies the type returned by the query, default is AF_INET
     *      @return returns the queried ip string
     *
     */
    function resolveAsync(name: string, family?: number): Promise<string>;

    /**
     * @description Quickly queries the host IPv4 address, equivalent to resolve(name)
     *
     *      A convenience wrapper of resolve(name, AF_INET); see resolve for the resolver behavior and the
     *      error shapes. Node.js has no equivalent directly under net, its lookup lives in the dns module.
     *      @param name specifies the host name
     *      @return returns the queried ip string
     *
     */
    function ip(name: string): Promise<string>;

    /**
     * @description Quickly queries the host IPv4 address, equivalent to resolve(name)
     *
     *      A convenience wrapper of resolve(name, AF_INET); see resolve for the resolver behavior and the
     *      error shapes. Node.js has no equivalent directly under net, its lookup lives in the dns module.
     *      @param name specifies the host name
     *      @return returns the queried ip string
     *
     */
    function ipSync(name: string): string;

    /**
     * @description Quickly queries the host IPv4 address, equivalent to resolve(name)
     *
     *      A convenience wrapper of resolve(name, AF_INET); see resolve for the resolver behavior and the
     *      error shapes. Node.js has no equivalent directly under net, its lookup lives in the dns module.
     *      @param name specifies the host name
     *      @return returns the queried ip string
     *
     */
    function ipAsync(name: string): Promise<string>;

    /**
     * @description Quickly queries the host IPv6 address, equivalent to resolve(name, net.AF_INET6)
     *
     *      A convenience wrapper of resolve(name, AF_INET6). It fails when the name has no IPv6 address,
     *      which is common on hosts without IPv6 connectivity, so prefer ip() unless the IPv6 address is
     *      required.
     *      @param name specifies the host name
     *      @return returns the queried ipv6 string
     *
     */
    function ipv6(name: string): Promise<string>;

    /**
     * @description Quickly queries the host IPv6 address, equivalent to resolve(name, net.AF_INET6)
     *
     *      A convenience wrapper of resolve(name, AF_INET6). It fails when the name has no IPv6 address,
     *      which is common on hosts without IPv6 connectivity, so prefer ip() unless the IPv6 address is
     *      required.
     *      @param name specifies the host name
     *      @return returns the queried ipv6 string
     *
     */
    function ipv6Sync(name: string): string;

    /**
     * @description Quickly queries the host IPv6 address, equivalent to resolve(name, net.AF_INET6)
     *
     *      A convenience wrapper of resolve(name, AF_INET6). It fails when the name has no IPv6 address,
     *      which is common on hosts without IPv6 connectivity, so prefer ip() unless the IPv6 address is
     *      required.
     *      @param name specifies the host name
     *      @return returns the queried ipv6 string
     *
     */
    function ipv6Async(name: string): Promise<string>;

    /**
     * @description The Socket class entry point, creates TCP and unix socket objects, see Socket
     *
     *      The same class as the global net.Socket and the Node.js net.Socket; net.connect(...) is the
     *      connected-instance factory and the accepted sockets of a TcpServer are Sockets too.
     *
     */
    const Socket: typeof Class_Socket;

    /**
     * @description Creates a Socket object and establishes a connection
     *
     *      This is the blocking form: the fiber waits until the connection is established and the method
     *      returns the connected Socket, or throws. options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "port": 80,          // the remote port, required
     *          "host": "localhost", // the remote address or host name
     *          "timeout": 0         // the connect timeout in milliseconds, 0 disables the fibjs timer
     *      })
     *      ```
     *      With timeout 0 the operating system connect timeout applies, which can be minutes on an
     *      unroutable address, so pass an explicit value when the peer may be unreachable. Node.js option
     *      keys such as path, family, localAddress, localPort, lookup, signal, keepAlive or noDelay are
     *      not read; to connect to a unix socket use the path form of Socket.connect or a unix:/ URL
     *      through the module entry. A refused or unreachable peer throws an Error with syscall 'connect'
     *      and a code such as ECONNREFUSED; an expired fibjs timer throws error number 20021.
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    function connect(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description Creates a Socket object and establishes a connection
     *
     *      This is the blocking form: the fiber waits until the connection is established and the method
     *      returns the connected Socket, or throws. options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "port": 80,          // the remote port, required
     *          "host": "localhost", // the remote address or host name
     *          "timeout": 0         // the connect timeout in milliseconds, 0 disables the fibjs timer
     *      })
     *      ```
     *      With timeout 0 the operating system connect timeout applies, which can be minutes on an
     *      unroutable address, so pass an explicit value when the peer may be unreachable. Node.js option
     *      keys such as path, family, localAddress, localPort, lookup, signal, keepAlive or noDelay are
     *      not read; to connect to a unix socket use the path form of Socket.connect or a unix:/ URL
     *      through the module entry. A refused or unreachable peer throws an Error with syscall 'connect'
     *      and a code such as ECONNREFUSED; an expired fibjs timer throws error number 20021.
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    function connectSync(options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description Creates a Socket object and establishes a connection
     *
     *      This is the blocking form: the fiber waits until the connection is established and the method
     *      returns the connected Socket, or throws. options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "port": 80,          // the remote port, required
     *          "host": "localhost", // the remote address or host name
     *          "timeout": 0         // the connect timeout in milliseconds, 0 disables the fibjs timer
     *      })
     *      ```
     *      With timeout 0 the operating system connect timeout applies, which can be minutes on an
     *      unroutable address, so pass an explicit value when the peer may be unreachable. Node.js option
     *      keys such as path, family, localAddress, localPort, lookup, signal, keepAlive or noDelay are
     *      not read; to connect to a unix socket use the path form of Socket.connect or a unix:/ URL
     *      through the module entry. A refused or unreachable peer throws an Error with syscall 'connect'
     *      and a code such as ECONNREFUSED; an expired fibjs timer throws error number 20021.
     *
     *      @param options specifies the connection options object
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking arity-2 form. `options` may be the options object above, the remote port (the
     *      host defaults to localhost) or the unix socket path. The method returns the Socket immediately
     *      and the connection continues in the background; connectListener is registered as a once
     *      'connect' listener whose `this` is the socket. A failure is delivered to the 'error' event as
     *      an event object carrying errno/code, syscall, hostname and args, not as an Error, and no
     *      exception is thrown; this differs from the blocking form, which throws. See the Socket class
     *      for the events of a connected socket.
     *
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking arity-2 form. `options` may be the options object above, the remote port (the
     *      host defaults to localhost) or the unix socket path. The method returns the Socket immediately
     *      and the connection continues in the background; connectListener is registered as a once
     *      'connect' listener whose `this` is the socket. A failure is delivered to the 'error' event as
     *      an event object carrying errno/code, syscall, hostname and args, not as an Error, and no
     *      exception is thrown; this differs from the blocking form, which throws. See the Socket class
     *      for the events of a connected socket.
     *
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking arity-2 form. `options` may be the options object above, the remote port (the
     *      host defaults to localhost) or the unix socket path. The method returns the Socket immediately
     *      and the connection continues in the background; connectListener is registered as a once
     *      'connect' listener whose `this` is the socket. A failure is delivered to the 'error' event as
     *      an event object carrying errno/code, syscall, hostname and args, not as an Error, and no
     *      exception is thrown; this differs from the blocking form, which throws. See the Socket class
     *      for the events of a connected socket.
     *
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a Socket object and establishes a TCP connection
     *
     *      The blocking port/host/timeout form. host may be an IPv4 or IPv6 literal or a name resolved as
     *      part of the connection, and timeout is the connect timeout in milliseconds (0 leaves the
     *      operating system default). The returned Socket is already connected and ready for send/recv;
     *      see the options overload for the error behavior and the Node.js differences of the resolution
     *      step. Equivalent to `new net.Socket().connect(port, host, timeout)`.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host?: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description Creates a Socket object and establishes a TCP connection
     *
     *      The blocking port/host/timeout form. host may be an IPv4 or IPv6 literal or a name resolved as
     *      part of the connection, and timeout is the connect timeout in milliseconds (0 leaves the
     *      operating system default). The returned Socket is already connected and ready for send/recv;
     *      see the options overload for the error behavior and the Node.js differences of the resolution
     *      step. Equivalent to `new net.Socket().connect(port, host, timeout)`.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host?: string, timeout?: number): Class_Stream;

    /**
     * @description Creates a Socket object and establishes a TCP connection
     *
     *      The blocking port/host/timeout form. host may be an IPv4 or IPv6 literal or a name resolved as
     *      part of the connection, and timeout is the connect timeout in milliseconds (0 leaves the
     *      operating system default). The returned Socket is already connected and ready for send/recv;
     *      see the options overload for the error behavior and the Node.js differences of the resolution
     *      step. Equivalent to `new net.Socket().connect(port, host, timeout)`.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host?: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description Creates a Socket or SslSocket object and establishes a connection
     *
     *      The URL form accepts these schemes:
     *      - `tcp://host:port` connects a plain Socket;
     *      - `ssl://host:port` is handled by the tls module and returns a TLSSocket;
     *      - `unix:/path` (or `unix:path`) connects a unix socket and `pipe://./name` a Windows named pipe.
     *      An unknown scheme and a tcp:// URL without a port throw a TypeError. A bare path is not a URL
     *      here: net.connect('/tmp/app.sock') throws, use 'unix:/tmp/app.sock' or
     *      new net.Socket().connect('/tmp/app.sock') instead.
     *
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port, or unix:/usr/local/proc1 or pipe://./pipe/proc1; when connecting to a pipe, replace `\` with `/`
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket or SslSocket object
     *
     */
    function connect(url: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description Creates a Socket or SslSocket object and establishes a connection
     *
     *      The URL form accepts these schemes:
     *      - `tcp://host:port` connects a plain Socket;
     *      - `ssl://host:port` is handled by the tls module and returns a TLSSocket;
     *      - `unix:/path` (or `unix:path`) connects a unix socket and `pipe://./name` a Windows named pipe.
     *      An unknown scheme and a tcp:// URL without a port throw a TypeError. A bare path is not a URL
     *      here: net.connect('/tmp/app.sock') throws, use 'unix:/tmp/app.sock' or
     *      new net.Socket().connect('/tmp/app.sock') instead.
     *
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port, or unix:/usr/local/proc1 or pipe://./pipe/proc1; when connecting to a pipe, replace `\` with `/`
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket or SslSocket object
     *
     */
    function connectSync(url: string, timeout?: number): Class_Stream;

    /**
     * @description Creates a Socket or SslSocket object and establishes a connection
     *
     *      The URL form accepts these schemes:
     *      - `tcp://host:port` connects a plain Socket;
     *      - `ssl://host:port` is handled by the tls module and returns a TLSSocket;
     *      - `unix:/path` (or `unix:path`) connects a unix socket and `pipe://./name` a Windows named pipe.
     *      An unknown scheme and a tcp:// URL without a port throw a TypeError. A bare path is not a URL
     *      here: net.connect('/tmp/app.sock') throws, use 'unix:/tmp/app.sock' or
     *      new net.Socket().connect('/tmp/app.sock') instead.
     *
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port, or unix:/usr/local/proc1 or pipe://./pipe/proc1; when connecting to a pipe, replace `\` with `/`
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Socket or SslSocket object
     *
     */
    function connectAsync(url: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking port/host form: equivalent to connect(port, host, 0, connectListener), so no
     *      fibjs connect timer is armed and the result arrives through 'connect'/'error'.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking port/host form: equivalent to connect(port, host, 0, connectListener), so no
     *      fibjs connect timer is armed and the result arrives through 'connect'/'error'.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking port/host form: equivalent to connect(port, host, 0, connectListener), so no
     *      fibjs connect timer is armed and the result arrives through 'connect'/'error'.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking port/host/timeout form; timeout bounds the connection attempt and an expired
     *      attempt is delivered to 'error' as the event object rather than thrown.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking port/host/timeout form; timeout bounds the connection attempt and an expired
     *      attempt is delivered to 'error' as the event object rather than thrown.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking port/host/timeout form; timeout bounds the connection attempt and an expired
     *      attempt is delivered to 'error' as the event object rather than thrown.
     *
     *      @param port specifies the remote port
     *      @param host specifies the remote address or host name, default is localhost
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking unix socket / Windows pipe form: path is taken literally (no scheme), timeout
     *      bounds the attempt and the result arrives through 'connect'/'error'.
     *
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(path: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking unix socket / Windows pipe form: path is taken literally (no scheme), timeout
     *      bounds the attempt and the result arrives through 'connect'/'error'.
     *
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(path: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Establishes a connection and triggers the connect event after the connection is established
     *
     *      The non-blocking unix socket / Windows pipe form: path is taken literally (no scheme), timeout
     *      bounds the attempt and the result arrives through 'connect'/'error'.
     *
     *      @param path specifies the unix socket or Windows pipe path
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(path: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description The Smtp class entry point, creates SMTP client objects, see Smtp
     *
     *      SMTP is an application protocol on top of a TCP/TLS connection; net.openSmtp(url, timeout) is
     *      the connected-instance factory. Node.js has no built-in SMTP client.
     *
     */
    const Smtp: typeof Class_Smtp;

    /**
     * @description Creates a Smtp object and establishes a connection, see Smtp
     *
     *      Blocking: the fiber waits for the SMTP greeting and the method returns an Smtp ready for the
     *      protocol commands (see Smtp). Connection failures are the same as connect, and an smtp:// url
     *      is not accepted, pass tcp:// or ssl:// explicitly.
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Smtp object
     *
     */
    function openSmtp(url: string, timeout?: number): Promise<Class_SmtpPromise>;

    /**
     * @description Creates a Smtp object and establishes a connection, see Smtp
     *
     *      Blocking: the fiber waits for the SMTP greeting and the method returns an Smtp ready for the
     *      protocol commands (see Smtp). Connection failures are the same as connect, and an smtp:// url
     *      is not accepted, pass tcp:// or ssl:// explicitly.
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Smtp object
     *
     */
    function openSmtpSync(url: string, timeout?: number): Class_Smtp;

    /**
     * @description Creates a Smtp object and establishes a connection, see Smtp
     *
     *      Blocking: the fiber waits for the SMTP greeting and the method returns an Smtp ready for the
     *      protocol commands (see Smtp). Connection failures are the same as connect, and an smtp:// url
     *      is not accepted, pass tcp:// or ssl:// explicitly.
     *      @param url specifies the connection protocol, which can be: tcp://host:port or ssl://host:port
     *      @param timeout specifies the timeout in milliseconds, default is 0
     *      @return returns the connected Smtp object
     *
     */
    function openSmtpAsync(url: string, timeout?: number): Promise<Class_SmtpPromise>;

    /**
     * @description The TcpServer class entry point, creates a TCP server, see TcpServer
     *
     *      The fibjs counterpart of the Node.js net.Server; net.createServer(...) is the factory form and
     *      a TcpServer may also be created with new net.TcpServer(...).
     *
     */
    const TcpServer: typeof Class_TcpServer;

    /**
     * @description Creates a TCP server
     *
     *      The deferred form: only the handler is stored, no address is bound. Call listen(port[, addr[,
     *      backlog]]) or start() after a bind before clients can connect. See the options overload for the
     *      accepted listener forms and the handler model.
     *      @param listener the connection handler
     *      @return returns a TcpServer object not bound to a port; listen() must be called to start it
     *
     */
    function createServer(listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string): Class_TcpServer;

    /**
     * @description Creates a TCP server
     *
     *      options supports two bind properties:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "address": "", // the listening address, defaults to all addresses
     *          "port": 0       // the listening port; without it nothing is bound and listen() is required
     *      })
     *      ```
     *      With both properties the server binds address:port immediately and start() begins accepting;
     *      with address only it binds that address with an OS-assigned port (or a unix path when the
     *      address is not an IP literal); with neither it only stores the handler and listen() must be
     *      called. These are bind options, not the socket options of Node.js net.createServer, and Node.js
     *      always requires listen().
     *
     *      listener may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(socket) => any`, called with each accepted connection;
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these
     *        same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *      - a path/address string: a directory or an `http(s)://` address, converted through the Handler
     *        constructor.
     *
     *      @param options the server options object, which can contain the following properties:
     *       - address: specifies the listening address, default is all addresses
     *       - port: specifies the listening port, optional. When not provided, listen() must be called to start
     *      @param listener the connection handler
     *      @return returns the TcpServer object
     *
     */
    function createServer(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_Socket | Class_SocketPromise)=>any) | FIBJS.GeneralObject | string): Class_TcpServer;

    /**
     * @description The alias of the UrlObject class, see UrlObject
     *
     *      The URL parser used by the URL forms of connect. Node.js has no equivalent under net; its URL
     *      helpers live in the url module.
     *
     */
    const Url: typeof Class_UrlObject;

    /**
     * @description Queries the asynchronous network engine of the current system
     *
     *      The name is platform specific: 'IOCP' on win32, 'KQueue' on darwin/freebsd and 'EPoll' on
     *      linux/android. It reports the platform engine; sockets created while use_uv_socket is true use
     *      libuv instead. Node.js always uses libuv and has no equivalent accessor.
     *      @return returns the network engine name
     *
     */
    function backend(): string;

    /**
     * @description Detects whether the input is an IP address
     *
     *      IPv4 must be dot-decimal without leading zeroes ('127.000.000.001' returns 0). IPv6 accepts
     *      compressed and IPv4-mapped forms ('::ffff:127.0.0.1' returns 6). A value that is not a string
     *      is converted with toString first, so an object whose toString yields an address is classified
     *      exactly like the Node.js behavior. No lookup is performed, a host name returns 0.
     *      @param ip the value to detect; a non-string value is not an IP address
     *      @return returns 0 for an invalid IP address, 4 for IPv4 and 6 for IPv6
     *
     */
    function isIP(ip?: any): number;

    /**
     * @description Detects whether the input is an IPv4 address
     *
     *      Equivalent to `isIP(ip) == 4`; the accepted syntax is dot-decimal without leading zeroes. Any
     *      failure to convert the value (for example null or a number) returns false instead of throwing.
     *      @param ip the value to detect; a non-string value is not an IP address
     *      @return returns true if it is IPv4, otherwise returns false
     *
     */
    function isIPv4(ip?: any): boolean;

    /**
     * @description Detects whether the input is an IPv6 address
     *
     *      Equivalent to `isIP(ip) == 6`; it accepts compressed forms and IPv4-mapped addresses. Any
     *      failure to convert the value returns false instead of throwing.
     *      @param ip the value to detect; a non-string value is not an IP address
     *      @return returns true if it is IPv6, otherwise returns false
     *
     */
    function isIPv6(ip?: any): boolean;

    /**
     * @description Queries whether net.connect enables automatic address family selection by default, compatible with Node.js >= 18.13
     *
     *      Compatibility accessor: fibjs stores the value so Node.js consumers that read it at module load
     *      (for example playwright-core) work unmodified, but connect never performs family
     *      autodetection. The companion timeout is getDefaultAutoSelectFamilyAttemptTimeout.
     *      @return returns the current default value, default is true
     *
     */
    function getDefaultAutoSelectFamily(): boolean;

    /**
     * @description Sets whether net.connect enables automatic address family selection by default, compatible with Node.js >= 18.13
     *
     *      Stores the value only; it does not change the behavior of connect, which always uses the family
     *      implied by the host name or the address family of the Socket. The setter returns nothing.
     *      @param enabled specifies the default value, which must be a boolean
     *
     */
    function setDefaultAutoSelectFamily(enabled: boolean): void;

    /**
     * @description Queries the default automatic address family selection timeout, compatible with Node.js >= 18.13
     *
     *      Compatibility accessor for Node.js consumers; see setDefaultAutoSelectFamily. It is only stored
     *      and is never used by connect.
     *      @return returns the current default timeout in milliseconds, default is 250
     *
     */
    function getDefaultAutoSelectFamilyAttemptTimeout(): number;

    /**
     * @description Sets the default automatic address family selection timeout, compatible with Node.js >= 18.13
     *
     *      Values below 10 are rejected with an out-of-range error whose message is
     *      "setDefaultAutoSelectFamilyAttemptTimeout: milliseconds must be >= 10, got N.", matching the
     *      Node.js validation. Like the family switch, the value is only stored and never used by connect.
     *      @param milliseconds specifies the default timeout in milliseconds, which must be an integer greater than or equal to 10
     *
     */
    function setDefaultAutoSelectFamilyAttemptTimeout(milliseconds: number): void;

}


declare module "net" {
    const promises: typeof import("net/promises");
}
