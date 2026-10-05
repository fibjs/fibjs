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
     * @description tls/ssl network socket object, see TLSSocket
     */
    const TLSSocket: typeof Class_TLSSocket;

    /**
     * @description tls/ssl protocol conversion handler, see TLSHandler
     */
    const Handler: typeof Class_TLSHandler;

    /**
     * @description tls/ssl protocol conversion handler, see TLSServer
     */
    const Server: typeof Class_TLSServer;

    /**
     * @description creates a TLS server
     *
     *      options may be the SecureContext object used by the server, or the options for creating
     *      one (the same object tls.createSecureContext accepts).
     *
     *      listener may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a Chain and invoked in order;
     *      - a handler function `(socket) => any`, called with each accepted TLS connection (a TLSSocket; it extends Stream, not Socket);
     *      - a routing map object, whose keys are match patterns and whose values are handlers in these same forms (see mq.Routing); it matches messages, so a raw connection cannot be routed;
     *      - a path/address string: a directory or an `http(s)://` address, converted through the Handler constructor.
     *      @param options the secure context or the options used to create one
     *      @param listener the connection handler
     *      @return returns a TLSServer object with no port bound, which needs listen() to start
     *
     */
    function createServer(options: FIBJS.GeneralObject | Class_SecureContext | Class_SecureContextPromise, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string): Class_TLSServer;

    /**
     * @description creates a SecureContext object, used to maintain secure contexts in the tls module
     *
     *      The options for creating a secure context support the following options:
     *      - ca: overrides the trusted CA certificates. By default, the well-known CAs managed by Mozilla are trusted. When this option is used to explicitly specify CAs, Mozilla's CAs are completely replaced. The value can be a string or a Buffer, or an Array of strings or Buffers. Any string or Buffer can contain multiple PEM CAs concatenated together. The peer's certificate must be able to chain to a CA trusted by the server for the connection to be authenticated. When using certificates that do not chain to a well-known CA, the certificate's CA must be explicitly specified as a trusted CA, otherwise the connection will not be authenticated. If the certificate used by the peer does not match or chain to one of the default CAs, use the ca option to provide a CA certificate that the peer certificate can match or chain to. For self-signed certificates, the certificate is its own CA and must be provided. For PEM-encoded certificates, the supported types are TRUSTED CERTIFICATE, X509 CERTIFICATE and CERTIFICATE.
     *      - cert: certificate chains in PEM format. One certificate chain should be provided for each private key. Each certificate chain should contain the certificate in PEM format for the provided private key, followed by intermediate certificates in PEM format (if any), in order, and excluding the root CA (the root CA must be pre-generated). When multiple certificate chains are provided, their order does not have to be the same as the private keys in key. If intermediate certificates are not provided, the peer will not be able to verify the certificate and the handshake will fail.
     *      - key: the private key in PEM format. PEM allows encrypted private keys to be chosen. Encrypted keys will be decrypted using options.passphrase.
     *      - passphrase: the shared passphrase used for a single private key and/or a PFX.
     *      - requestCert: if true, the server will require a client certificate for authentication. Default: true.
     *      - rejectUnverified: if not false, the server will reject any connection whose certificate fails CA list verification. Default: true.
     *      - rejectUnauthorized: if not false, the server will reject any connection that does not provide a certificate authorized by the CA list. Default: true in client mode, false in server mode.
     *      - maxVersion: sets the maximum allowed TLS version. One of 'TLSv1.3', 'TLSv1.2', 'TLSv1.1' or 'TLSv1'. Cannot be specified together with the secureProtocol option.
     *      - minVersion: sets the minimum allowed TLS version. One of 'TLSv1.3', 'TLSv1.2', 'TLSv1.1' or 'TLSv1'. Cannot be specified together with the secureProtocol option.
     *      - secureProtocol: legacy mechanism to select the TLS protocol version to use; it does not support independent control of the minimum and maximum versions, nor restricting the protocol to TLSv1.3. Using minVersion and maxVersion is recommended instead.
     *      - sessionTimeout: the number of seconds after which a TLS session created by the server will no longer be resumable. Default: 300.
     *      - SNIResolver: used to resolve the server name in the SNI callback. The function signature is function(servername), where servername is the server name indication sent by the client. The return value is a SecureContext object, or null if it cannot be resolved.
     *      - SNICacheSize: the size of the SNI context cache. Default: 1024.
     *      - SNICacheTimeout: the timeout of the SNI context cache (in seconds). Default: 300. If set to 0 or a negative number, the cache will never expire.
     *      - SNICacheIdleTimeout: the timeout of the SNI idle context cache (in seconds). Default: 300. If set to 0 or a negative number, the idle cache will never expire.
     *
     *      @param options the options for creating the secure context
     *      @param isServer whether it is in server mode, default is false
     *      @return returns the created secure context
     *
     */
    function createSecureContext(options: FIBJS.GeneralObject, isServer?: boolean): Class_SecureContext;

    /**
     * @description creates a SecureContext object, used to maintain secure contexts in the tls module
     *      @param isServer whether it is in server mode, default is false
     *      @return returns the created secure context
     *
     */
    function createSecureContext(isServer?: boolean): Class_SecureContext;

    /**
     * @description queries the default SecureContext
     */
    const secureContext: Class_SecureContextPromise;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection and triggers the connect event after the connection is established
     *
     *      options may be given in any of these forms:
     *      - a connection options object carrying the port, host, timeout and the TLS options;
     *      - the url to connect to, such as 'ssl://host:port';
     *      - the remote port, with the host defaulting to localhost.
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection and triggers the connect event after the connection is established
     *
     *      options may be given in any of these forms:
     *      - a connection options object carrying the port, host, timeout and the TLS options;
     *      - the url to connect to, such as 'ssl://host:port';
     *      - the remote port, with the host defaulting to localhost.
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description creates a tls/ssl connection and triggers the connect event after the connection is established
     *
     *      options may be given in any of these forms:
     *      - a connection options object carrying the port, host, timeout and the TLS options;
     *      - the url to connect to, such as 'ssl://host:port';
     *      - the remote port, with the host defaulting to localhost.
     *      @param options the connection target
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(options: FIBJS.GeneralObject | string | number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host: string, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(url: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(url: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(url: string, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(url: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(url: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(url: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(port: number, host: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(port: number, host: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number, and triggers the connect event after the connection is established
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect
     *      @param options specifies the connection options
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectAsync(port: number, host: string, options: FIBJS.GeneralObject, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect, default is "localhost"
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(port: number, host?: string, options?: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect, default is "localhost"
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(port: number, host?: string, options?: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the hostname and port number
     *      @param port specifies the port number to connect
     *      @param host specifies the hostname to connect, default is "localhost"
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(port: number, host?: string, options?: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(url: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(url: string, timeout?: number): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(url: string, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout?: number): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout?: number): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connect(url: string, options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectSync(url: string, options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the url
     *      @param url specifies the URL to connect
     *      @param options specifies the connection options
     *      @return returns the tls/ssl connection object
     *
     */
    function connectAsync(url: string, options: FIBJS.GeneralObject): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connect(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Promise<Class_StreamPromise>;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
     *      @param url specifies the URL to connect
     *      @param secureContext specifies the secure context
     *      @param timeout specifies the connection timeout, default is 0
     *      @param connectListener specifies the once connect event listener
     *      @return returns the connected Socket object
     *
     */
    function connectSync(url: string, secureContext: Class_SecureContext | Class_SecureContextPromise, timeout: number, connectListener: (ev: FIBJS.GeneralObject)=>void): Class_Stream;

    /**
     * @description creates a tls/ssl connection based on the url, and triggers the connect event after the connection is established
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
