/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
/**
 * @description tls/ssl network socket object
 *
 * TLSSocket belongs to the tls module; creation:
 *  ```JavaScript
 *  var s = new tls.TLSSocket();
 *  ```
 *
 */
declare class Class_TLSSocket extends Class_Stream {
    /**
     * @description creates a new TLSSocket object using the current default SecureContext
     */
    constructor();

    /**
     * @description creates a new TLSSocket object from context
     *      @param context specifies the secure context used to create TLSSocket
     *
     */
    constructor(context: Class_SecureContext);

    /**
     * @description creates a new TLSSocket object from options
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param isServer whether it is in server mode
     *
     */
    constructor(options: FIBJS.GeneralObject, isServer?: boolean);

    /**
     * @description establishes a tls/ssl connection on the given connection, client mode
     *      @param socket the given underlying connection
     *      @param server_name the server name, used to verify the server certificate
     *
     */
    connect(socket: Class_Stream, server_name?: string): void;

    connect(socket: Class_Stream, server_name?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description establishes a tls/ssl connection on the given connection, client mode
     *      @param socket the given underlying connection
     *      @param server_name the server name, used to verify the server certificate
     *
     */
    connectSync(socket: Class_Stream, server_name?: string): void;

    /**
     * @description establishes a tls/ssl connection on the given connection, client mode
     *      @param socket the given underlying connection
     *      @param server_name the server name, used to verify the server certificate
     *
     */
    connectAsync(socket: Class_Stream, server_name?: string): Promise<void>;

    /**
     * @description establishes a tls/ssl connection on the given connection, client mode, and triggers the connect event after the connection is established
     *      @param socket the given underlying connection
     *      @param connectListener specifies the once connect event listener
     *
     */
    connect(socket: Class_Stream, connectListener: (...args: any[])=>any): void;

    /**
     * @description establishes a tls/ssl connection on the given connection, client mode, and triggers the connect event after the connection is established
     *      @param socket the given underlying connection
     *      @param server_name the server name, used to verify the server certificate
     *      @param connectListener specifies the once connect event listener
     *
     */
    connect(socket: Class_Stream, server_name: string, connectListener: (...args: any[])=>any): void;

    /**
     * @description establishes a tls/ssl connection on the given connection, server mode
     *      @param socket the given underlying connection
     *
     */
    accept(socket: Class_Stream): void;

    accept(socket: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description establishes a tls/ssl connection on the given connection, server mode
     *      @param socket the given underlying connection
     *
     */
    acceptSync(socket: Class_Stream): void;

    /**
     * @description establishes a tls/ssl connection on the given connection, server mode
     *      @param socket the given underlying connection
     *
     */
    acceptAsync(socket: Class_Stream): Promise<void>;

    /**
     * @description queries the underlying stream object when the tls/ssl connection was established
     */
    readonly stream: Class_Stream;

    /**
     * @description the tls/ssl protocol version negotiated by the current connection
     *      @return returns the tls/ssl protocol version
     *
     */
    getProtocol(): string;

    /**
     * @description the local certificate negotiated by the current connection
     *      @return returns the local certificate
     *
     */
    getX509Certificate(): Class_X509Certificate;

    /**
     * @description the peer certificate negotiated by the current connection
     *      @return returns the peer certificate
     *
     */
    getPeerX509Certificate(): Class_X509Certificate;

    /**
     * @description queries the SecureContext used by the current TLSSocket
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description queries the peer address of the current connection
     */
    readonly remoteAddress: string;

    /**
     * @description queries the peer port of the current connection
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
     * @description queries the ALPN protocol negotiated by the current connection, returns undefined if not negotiated
     */
    readonly alpnProtocol: string;

}

