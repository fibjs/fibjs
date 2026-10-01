/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description tls secure context object, used to share basic configuration among multiple tls connections
 *
 *   The SecureContext object is a secure context object used to share basic configuration among multiple tls connections. A SecureContext object can be created with the tls.createSecureContext method.
 *   ```JavaScript
 *     const tls = require('tls');
 *     const fs = require('fs');
 *
 *     const options = {
 *       key: fs.readFileSync('server-key.pem'),
 *       cert: fs.readFileSync('server-cert.pem')
 *     };
 *
 *     const context = tls.createSecureContext(options);
 *   ```
 *
 */
declare class Class_SecureContext extends Class_object {
    /**
     * @description Queries the trusted CA certificate of the secure context
     */
    readonly ca: Class_X509Certificate;

    /**
     * @description Queries the private key of the secure context connection
     */
    readonly key: Class_KeyObject;

    /**
     * @description Queries the certificate of the secure context connection
     */
    readonly cert: Class_X509Certificate;

    /**
     * @description Queries the maximum TLS version allowed by the secure context
     */
    readonly maxVersion: string;

    /**
     * @description Queries the minimum TLS version allowed by the secure context
     */
    readonly minVersion: string;

    /**
     * @description Queries the TLS protocol version used by the secure context
     */
    readonly secureProtocol: string;

    /**
     * @description Queries whether the secure context requires a client certificate
     */
    readonly requestCert: boolean;

    /**
     * @description Queries whether the secure context rejects any connection whose certificate fails CA list verification
     */
    readonly rejectUnverified: boolean;

    /**
     * @description Queries whether the secure context rejects any connection that does not provide a certificate authorized by the CA list
     */
    readonly rejectUnauthorized: boolean;

    /**
     * @description Queries the secure context session timeout
     */
    readonly sessionTimeout: number;

    /**
     * @description Sets the SNI context
     *     @param servername the server name
     *     @param context the secure context
     *
     */
    setSNIContext(servername: string, context: Class_SecureContext): void;

    /**
     * @description Sets the SNI context
     *     @param servername the server name
     *     @param options options needed to create a secure context with tls.createSecureContext
     *
     */
    setSNIContext(servername: string, options: FIBJS.GeneralObject): void;

    /**
     * @description Queries the SNI context
     *     @param servername the server name
     *     @param auto_resolve whether to create the context automatically
     *     @return returns the specified secure context
     *
     */
    getSNIContext(servername: string, auto_resolve?: boolean): Class_SecureContext;

    getSNIContext(servername: string, auto_resolve?: boolean, callback: (err: Error | undefined | null, retVal: Class_SecureContext)=>any): void;

    /**
     * @description Queries the SNI context
     *     @param servername the server name
     *     @param auto_resolve whether to create the context automatically
     *     @return returns the specified secure context
     *
     */
    getSNIContextSync(servername: string, auto_resolve?: boolean): Class_SecureContext;

    /**
     * @description Queries the SNI context
     *     @param servername the server name
     *     @param auto_resolve whether to create the context automatically
     *     @return returns the specified secure context
     *
     */
    getSNIContextAsync(servername: string, auto_resolve?: boolean): Promise<Class_SecureContext>;

    /**
     * @description Removes the SNI context
     *     @param servername the server name
     *
     */
    removeSNIContext(servername: string): void;

    /**
     * @description Clears all SNI contexts
     */
    clearSNIContexts(): void;

}

