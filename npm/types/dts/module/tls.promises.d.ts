/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TLSSocket.d.ts" />
/// <reference path="../interface/TLSHandler.d.ts" />
/// <reference path="../interface/TLSServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * The promise variant of the tls module: async members return a Promise as their primary form.
 */
declare module 'tls/promises' {
    /**
     * @description The TLSSocket class entry point, see TLSSocket
     *
     *      The same class as the sockets returned by connect and handed to TLSServer handlers; use it
     *      directly to wrap an existing stream or to drive the handshake step by step.
     *
     */
    const TLSSocket: typeof Class_TLSSocket;

    /**
     * @description The TLSHandler class entry point, see TLSHandler
     *
     *      A handler that upgrades every accepted raw stream to TLS and then invokes the wrapped
     *      handler; pass it to net.createServer or new net.TcpServer to build a TLS protocol server.
     *
     */
    const Handler: typeof Class_TLSHandler;

    /**
     * @description The TLSServer class entry point, see TLSServer
     *
     *      The class of the servers created by createServer; it combines net.TcpServer with TLSHandler
     *      and can also be created with new tls.Server(context, listener). Node.js names its class
     *      tls.Server as well; neither environment exports tls.TLSServer.
     *
     */
    const Server: typeof Class_TLSServer;

    /**
     * @description Creates a TLS server from a secure context or the options used to create one
     *
     *      options may be a ready SecureContext or an options object accepted by createSecureContext.
     *      The returned server has no port bound: call listen() (or bind elsewhere and then start())
     *      before it accepts clients, and stop() when done. Unlike the TLSServer options constructor,
     *      an `address` or `port` key in options is not bound here, so tls.createServer({port: 8443},
     *      listener) still requires listen().
     *
     *      listener may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(socket) => any`, called with each accepted TLS connection (a TLSSocket; it extends Stream, not Socket);
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *      - a path/address string: a directory or an `http(s)://` address, converted through the Handler constructor.
     *      Node.js tls.createServer() accepts a missing listener and reports connections through the
     *      'secureConnection' event, while fibjs requires the listener and hands each TLSSocket to it
     *      directly; a missing listener throws a parameter-not-optional error.
     *      @param options the secure context or the options used to create one
     *      @param listener the connection handler
     *      @return returns a TLSServer object with no port bound, which needs listen() to start
     *
     */
    function createServer(options: FIBJS.GeneralObject | Class_SecureContext | Class_SecureContextPromise, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string): Class_TLSServer;

    /**
     * @description Creates a SecureContext that holds the certificates, protocol versions and verification flags shared by TLS connections
     *
     *      A context is validated while it is created: an unknown version name, a conflicting
     *      secureProtocol/minVersion combination and unparsable or mismatching certificate material
     *      fail here instead of at handshake time (invalid PEM material reports error 20024). Without
     *      options an empty context is built. With isServer false (the default) the context behaves as
     *      a client: it inherits the default Mozilla trust store and requires a verified server
     *      certificate, while a server context has no CA of its own and does not require a client
     *      certificate. When options carries a `secureContext` key, that ready context is returned and
     *      the other TLS keys are ignored; this is how the connect forms accept {secureContext: ctx}.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "ca": null,                 // trusted CAs: PEM string/Buffer/X509Certificate or an array
     *          "cert": null,               // PEM certificate chain: leaf first, then intermediates
     *          "key": null,                // PEM private key matching cert; needs passphrase if encrypted
     *          "passphrase": null,         // passphrase of an encrypted private key or PFX
     *          "requestCert": true,        // ask the peer for a certificate
     *          "rejectUnverified": true,   // fail the handshake if that certificate does not verify
     *          "rejectUnauthorized": undefined, // require a peer certificate; client true, server false
     *          "minVersion": null,         // 'TLSv1' | 'TLSv1.1' | 'TLSv1.2' | 'TLSv1.3'
     *          "maxVersion": null,         // 'TLSv1' | 'TLSv1.1' | 'TLSv1.2' | 'TLSv1.3'
     *          "secureProtocol": null,     // legacy method name, e.g. 'TLSv1_2_method'
     *          "sessionTimeout": 7200,     // server-side resumable session lifetime in seconds
     *          "alpnProtocols": [],        // protocol names offered through ALPN
     *          "SNIResolver": null,        // (servername) => SecureContext, server only
     *          "SNICacheSize": 1024,       // number of cached SNI contexts
     *          "SNICacheTimeout": 300,     // lifetime of a cached SNI context in seconds
     *          "SNICacheIdleTimeout": 300, // idle lifetime of a cached SNI context in seconds
     *          "secureContext": null       // a ready context; when set, the other TLS keys are ignored
     *      })
     *      ```
     *
     *      ca/cert accept a PEM string, a Buffer, an X509Certificate or an array of them; a PEM string
     *      may concatenate several certificates. key and cert must be provided together, and the key is
     *      checked to match the certificate. Supplying ca completely replaces the Mozilla store, so the
     *      peer certificate must chain to one of the given CAs. requestCert, rejectUnverified and
     *      rejectUnauthorized control the verification described in the module concepts. minVersion
     *      and maxVersion cannot be combined with a legacy secureProtocol value that already fixes a
     *      version. sessionTimeout only affects a server context. The SNI options configure the
     *      server-side cache used by getSNIContext and are ignored on a client context. Node.js
     *      accepts many more keys (ciphers, ecdhCurve, honorCipherOrder, ...), which fibjs ignores.
     *
     *      @param options the options for creating the secure context
     *      @param isServer whether it is in server mode, default is false
     *      @return returns the created secure context
     *
     */
    function createSecureContext(options: FIBJS.GeneralObject, isServer?: boolean): Class_SecureContext;

    /**
     * @description Creates an empty SecureContext, optionally with the defaults of a server
     *
     *      Shorthand for createSecureContext({}, isServer). With isServer false the context trusts the
     *      default Mozilla roots and requires a verified server certificate; with true it behaves as a
     *      server: rejectUnauthorized defaults to false and the SNI callback is installed, so the
     *      context can serve setSNIContext/getSNIContext lookups.
     *      @param isServer whether it is in server mode, default is false
     *      @return returns the created secure context
     *
     */
    function createSecureContext(isServer?: boolean): Class_SecureContext;

    /**
     * @description The process-wide default SecureContext
     *
     *      Used when connect, new TLSSocket() or createSecureContext() is called without an explicit
     *      context. It is a client context, so the Mozilla roots are trusted; the same object is
     *      exposed here for inspection or reuse. Node.js has no equivalent property, it exposes
     *      rootCertificates and the DEFAULT_* constants instead.
     *
     */
    const secureContext: Class_SecureContextPromise;

    /**
     * @description Creates a TLS connection from an options object and waits for the handshake
     *
     *      The object is read twice: as connection options it uses `host` (default 'localhost'), `port`
     *      (default 0) and `timeout` (connect timeout in milliseconds, default 0), while the remaining
     *      keys are the TLS options of createSecureContext (`ca`, `cert`, `key`, `rejectUnverified`,
     *      `secureContext`, ...). host is also the server name sent as SNI and verified against the
     *      server certificate. The call blocks the current fiber, returns the connected TLSSocket (the
     *      declared Stream type is its base interface) and throws an Error with a verification code on
     *      failure. Node.js tls.connect() instead returns immediately and reports readiness through
     *      the 'secureConnect' event; use the listener or promise form for the same non-blocking style.
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description Creates a TLS connection from an options object and waits for the handshake
     *
     *      The object is read twice: as connection options it uses `host` (default 'localhost'), `port`
     *      (default 0) and `timeout` (connect timeout in milliseconds, default 0), while the remaining
     *      keys are the TLS options of createSecureContext (`ca`, `cert`, `key`, `rejectUnverified`,
     *      `secureContext`, ...). host is also the server name sent as SNI and verified against the
     *      server certificate. The call blocks the current fiber, returns the connected TLSSocket (the
     *      declared Stream type is its base interface) and throws an Error with a verification code on
     *      failure. Node.js tls.connect() instead returns immediately and reports readiness through
     *      the 'secureConnect' event; use the listener or promise form for the same non-blocking style.
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description Creates a TLS connection from an options object and waits for the handshake
     *
     *      The object is read twice: as connection options it uses `host` (default 'localhost'), `port`
     *      (default 0) and `timeout` (connect timeout in milliseconds, default 0), while the remaining
     *      keys are the TLS options of createSecureContext (`ca`, `cert`, `key`, `rejectUnverified`,
     *      `secureContext`, ...). host is also the server name sent as SNI and verified against the
     *      server certificate. The call blocks the current fiber, returns the connected TLSSocket (the
     *      declared Stream type is its base interface) and throws an Error with a verification code on
     *      failure. Node.js tls.connect() instead returns immediately and reports readiness through
     *      the 'secureConnect' event; use the listener or promise form for the same non-blocking style.
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description Creates a TLS connection without blocking and reports the outcome through events
     *
     *      options selects one of the other entry forms: an options object (host/port/timeout plus the
     *      TLS options), an `ssl://` URL, or the remote port, in which case the host defaults to
     *      'localhost'. The call returns immediately; on success the 'connect' event fires with the
     *      TLSSocket, and on failure an 'error' event carries an Error with `code` and
     *      `args.servername` instead of throwing. Listen for the 'error' event, otherwise a failed
     *      handshake becomes an unhandled error.
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a TLS connection without blocking and reports the outcome through events
     *
     *      options selects one of the other entry forms: an options object (host/port/timeout plus the
     *      TLS options), an `ssl://` URL, or the remote port, in which case the host defaults to
     *      'localhost'. The call returns immediately; on success the 'connect' event fires with the
     *      TLSSocket, and on failure an 'error' event carries an Error with `code` and
     *      `args.servername` instead of throwing. Listen for the 'error' event, otherwise a failed
     *      handshake becomes an unhandled error.
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Creates a TLS connection without blocking and reports the outcome through events
     *
     *      options selects one of the other entry forms: an options object (host/port/timeout plus the
     *      TLS options), an `ssl://` URL, or the remote port, in which case the host defaults to
     *      'localhost'. The call returns immediately; on success the 'connect' event fires with the
     *      TLSSocket, and on failure an 'error' event carries an Error with `code` and
     *      `args.servername` instead of throwing. Listen for the 'error' event, otherwise a failed
     *      handshake becomes an unhandled error.
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking TLS connection to a port from an options object
     *
     *      The port is fixed, the host defaults to 'localhost' and options is the same object as in the
     *      blocking (port, host, options) form, so `host` inside it overrides the default. It returns
     *      immediately and reports the handshake through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking TLS connection to a port from an options object
     *
     *      The port is fixed, the host defaults to 'localhost' and options is the same object as in the
     *      blocking (port, host, options) form, so `host` inside it overrides the default. It returns
     *      immediately and reports the handshake through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Creates a non-blocking TLS connection to a port from an options object
     *
     *      The port is fixed, the host defaults to 'localhost' and options is the same object as in the
     *      blocking (port, host, options) form, so `host` inside it overrides the default. It returns
     *      immediately and reports the handshake through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking TLS connection to a host and port
     *
     *      The default context is used and the host is both the TCP target and the verified SNI name.
     *      The call returns immediately and reports the handshake through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking TLS connection to a host and port
     *
     *      The default context is used and the host is both the TCP target and the verified SNI name.
     *      The call returns immediately and reports the handshake through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Creates a non-blocking TLS connection to a host and port
     *
     *      The default context is used and the host is both the TCP target and the verified SNI name.
     *      The call returns immediately and reports the handshake through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL, with a connect listener
     *
     *      Returns immediately; the outcome is delivered through the 'connect'/'error' events. timeout
     *      bounds only the connect attempt (0 means no limit).
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(url: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL, with a connect listener
     *
     *      Returns immediately; the outcome is delivered through the 'connect'/'error' events. timeout
     *      bounds only the connect attempt (0 means no limit).
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(url: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL, with a connect listener
     *
     *      Returns immediately; the outcome is delivered through the 'connect'/'error' events. timeout
     *      bounds only the connect attempt (0 means no limit).
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(url: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL with an explicit context
     *
     *      Returns immediately; the given SecureContext is used for the handshake and the outcome is
     *      delivered through the 'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL with an explicit context
     *
     *      Returns immediately; the given SecureContext is used for the handshake and the outcome is
     *      delivered through the 'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL with an explicit context
     *
     *      Returns immediately; the given SecureContext is used for the handshake and the outcome is
     *      delivered through the 'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL and an options object
     *
     *      Returns immediately; the TLS keys of options build the context while its `host`, `port` and
     *      `timeout` keys are ignored (the URL provides the target), and the outcome is delivered
     *      through the 'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(url: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL and an options object
     *
     *      Returns immediately; the TLS keys of options build the context while its `host`, `port` and
     *      `timeout` keys are ignored (the URL provides the target), and the outcome is delivered
     *      through the 'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(url: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL and an options object
     *
     *      Returns immediately; the TLS keys of options build the context while its `host`, `port` and
     *      `timeout` keys are ignored (the URL provides the target), and the outcome is delivered
     *      through the 'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(url: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking TLS connection to a host and port with explicit options
     *
     *      The full listener form: options supplies the TLS keys and the connect timeout, host is both
     *      the TCP target and the verified SNI name. It returns immediately and reports the handshake
     *      through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking TLS connection to a host and port with explicit options
     *
     *      The full listener form: options supplies the TLS keys and the connect timeout, host is both
     *      the TCP target and the verified SNI name. It returns immediately and reports the handshake
     *      through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Creates a non-blocking TLS connection to a host and port with explicit options
     *
     *      The full listener form: options supplies the TLS keys and the connect timeout, host is both
     *      the TCP target and the verified SNI name. It returns immediately and reports the handshake
     *      through the 'connect'/'error' events.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a blocking TLS connection to a host and port
     *
     *      The synchronous workhorse: connect(port), connect(port, host) and connect(port, host,
     *      options) all land here. host defaults to 'localhost' and options to an empty object, so
     *      connect(port, {ca}) is not a valid form - pass the host explicitly or use an options object
     *      as the single argument. The host is both the TCP target and the verified SNI name.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect, default is "localhost"
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(port: number, host?: string, options?: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description Creates a blocking TLS connection to a host and port
     *
     *      The synchronous workhorse: connect(port), connect(port, host) and connect(port, host,
     *      options) all land here. host defaults to 'localhost' and options to an empty object, so
     *      connect(port, {ca}) is not a valid form - pass the host explicitly or use an options object
     *      as the single argument. The host is both the TCP target and the verified SNI name.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect, default is "localhost"
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(port: number, host?: string, options?: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description Creates a blocking TLS connection to a host and port
     *
     *      The synchronous workhorse: connect(port), connect(port, host) and connect(port, host,
     *      options) all land here. host defaults to 'localhost' and options to an empty object, so
     *      connect(port, {ca}) is not a valid form - pass the host explicitly or use an options object
     *      as the single argument. The host is both the TCP target and the verified SNI name.
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect, default is "localhost"
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(port: number, host?: string, options?: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL
     *
     *      The URL must carry the `ssl:` scheme and an explicit port, otherwise an invalid-argument
     *      error is thrown ("url must start with 'ssl:'" or "missing port in url"); the host part is
     *      the TCP target, the SNI name and the verified name. The default context supplies the trust
     *      store, so pass a SecureContext or an options object when the server certificate is not
     *      signed by a well-known CA. Node.js has no URL string form.
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(url: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL
     *
     *      The URL must carry the `ssl:` scheme and an explicit port, otherwise an invalid-argument
     *      error is thrown ("url must start with 'ssl:'" or "missing port in url"); the host part is
     *      the TCP target, the SNI name and the verified name. The default context supplies the trust
     *      store, so pass a SecureContext or an options object when the server certificate is not
     *      signed by a well-known CA. Node.js has no URL string form.
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(url: string, timeout?: number): Class_Stream;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL
     *
     *      The URL must carry the `ssl:` scheme and an explicit port, otherwise an invalid-argument
     *      error is thrown ("url must start with 'ssl:'" or "missing port in url"); the host part is
     *      the TCP target, the SNI name and the verified name. The default context supplies the trust
     *      store, so pass a SecureContext or an options object when the server certificate is not
     *      signed by a well-known CA. Node.js has no URL string form.
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(url: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL with an explicit context
     *
     *      Same as the plain URL form, but the given SecureContext supplies the trust store, the client
     *      certificate and the ALPN list instead of the default context.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL with an explicit context
     *
     *      Same as the plain URL form, but the given SecureContext supplies the trust store, the client
     *      certificate and the ALPN list instead of the default context.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout?: number): Class_Stream;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL with an explicit context
     *
     *      Same as the plain URL form, but the given SecureContext supplies the trust store, the client
     *      certificate and the ALPN list instead of the default context.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL and an options object
     *
     *      The TLS keys of options build the context as in the options-object form; its `host` and
     *      `port` keys are ignored because the URL provides both, while `timeout` still bounds the
     *      connect. A `secureContext` key inside options is honored.
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(url: string, options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL and an options object
     *
     *      The TLS keys of options build the context as in the options-object form; its `host` and
     *      `port` keys are ignored because the URL provides both, while `timeout` still bounds the
     *      connect. A `secureContext` key inside options is honored.
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(url: string, options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description Creates a blocking TLS connection from an `ssl://` URL and an options object
     *
     *      The TLS keys of options build the context as in the options-object form; its `host` and
     *      `port` keys are ignored because the URL provides both, while `timeout` still bounds the
     *      connect. A `secureContext` key inside options is honored.
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(url: string, options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL with a context and a timeout
     *
     *      The most explicit URL listener form: secureContext supplies the handshake configuration,
     *      timeout bounds the connect attempt and the outcome is delivered through the
     *      'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL with a context and a timeout
     *
     *      The most explicit URL listener form: secureContext supplies the handshake configuration,
     *      timeout bounds the connect attempt and the outcome is delivered through the
     *      'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description Creates a non-blocking connection from an `ssl://` URL with a context and a timeout
     *
     *      The most explicit URL listener form: secureContext supplies the handshake configuration,
     *      timeout bounds the connect attempt and the outcome is delivered through the
     *      'connect'/'error' events.
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

}


declare module "tls" {
    const promises: typeof import("tls/promises");
}
