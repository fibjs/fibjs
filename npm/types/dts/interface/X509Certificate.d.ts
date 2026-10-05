/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description Encapsulates an X509 certificate and provides information reading
 *
 * ```JavaScript
 * const crypto = require('crypto');
 * const cert = new crypto.X509Certificate(Buffer.from('...'));
 * ```
 *
 */
declare class Class_X509Certificate extends Class_object {
    /**
     * @description Creates an X509Certificate object from a certificate
     *
     *     If cert contains multiple certificates, the returned object will contain the first certificate, and the next() method will return the next certificate
     *
     *      cert may be the PEM/DER data of the certificate, or the PEM text as a string.
     *      @param cert the certificate data
     *     @return returns an X509Certificate object
     *
     */
    constructor(cert: Class_Buffer | string);

    /**
     * @description Creates an X509Certificate object from a group of certificates
     *
     *     If certs contains multiple certificates, the returned object will contain the first certificate, and the next() method will return the next certificate; a chain may mix strings and buffers
     *
     *     @param certs the array of certificates in PEM format
     *     @return returns an X509Certificate object
     *
     */
    constructor(certs: (Class_Buffer | string)[]);

    /**
     * @description The subject of the certificate
     */
    readonly subject: string;

    /**
     * @description The serial number of the certificate
     */
    readonly serialNumber: string;

    /**
     * @description The certified public key of the certificate
     */
    readonly publicKey: Class_KeyObject;

    /**
     * @description The subject alternative names of the certificate
     */
    readonly subjectAltName: string;

    /**
     * @description The information access extension of the certificate; returns a newline-separated list of access descriptions. Each line begins with the access method and the type of the access location, followed by a colon and the value associated with the access location
     */
    readonly infoAccess: string;

    /**
     * @description The issuer of the certificate
     */
    readonly issuer: string;

    /**
     * @description Whether the certificate is a CA certificate
     */
    readonly ca: boolean;

    /**
     * @description The path length constraint of the certificate
     */
    readonly pathlen: number;

    /**
     * @description The key usage of the certificate
     */
    readonly keyUsage: string[];

    /**
     * @description The Netscape type of the certificate
     */
    readonly type: string[];

    /**
     * @description The start time of the certificate validity period
     */
    readonly validFrom: string;

    /**
     * @description The end time of the certificate validity period
     */
    readonly validTo: string;

    /**
     * @description The raw binary data of the certificate
     */
    readonly raw: Class_Buffer;

    /**
     * @description The PEM encoding of the certificate
     */
    readonly pem: string;

    /**
     * @description The SHA-1 fingerprint of the certificate
     */
    readonly fingerprint: string;

    /**
     * @description The SHA-256 fingerprint of the certificate
     */
    readonly fingerprint256: string;

    /**
     * @description The SHA-512 fingerprint of the certificate
     */
    readonly fingerprint512: string;

    /**
     * @description The next certificate in the certificate chain
     *      @return returns the next certificate
     *
     */
    next(): Class_X509Certificate;

    /**
     * @description Checks whether the certificate matches the given email address
     *
     *     If the options.subject option is undefined or set to 'default', the certificate subject is considered only when the subject alternative name extension is absent or contains no email addresses.
     *
     *     If the options.subject option is set to 'always' and the subject alternative name extension is absent or contains no matching email address, the certificate subject is considered.
     *
     *     If the options.subject option is set to 'never', the certificate subject is never considered, even if the certificate contains no subject alternative names.
     *
     *     @param email the email address
     *     @param options the options
     *     @return returns email if the certificate matches, or undefined if it does not
     *
     */
    checkEmail(email: string, options?: FIBJS.GeneralObject): string;

    /**
     * @description Checks whether the certificate matches the given host name
     *
     *     If the certificate matches the given host name, the matching subject name is returned. The returned name may be an exact match (foo.example.com) or may contain wildcards (*.example.com ). Since host name comparison is case-insensitive, the case of the returned subject name may differ from the given name.
     *
     *     options supports the following properties:
     *      - subject: 'default', 'always' or 'never'. Default: 'default'.
     *      - wildcards: default true.
     *      - partialWildcards: default true.
     *      - multiLabelWildcards: default false.
     *      - singleLabelSubdomains: default false.
     *
     *     If the options.subject option is undefined or set to 'default', the certificate subject is considered only when the subject alternative name extension is absent or contains no DNS names.
     *
     *     If the options.subject option is set to 'always' and the subject alternative name extension is absent or contains no matching DNS names, the certificate subject is considered.
     *
     *     If the options.subject option is set to 'never', the certificate subject is never considered, even if the certificate contains no subject alternative names.
     *
     *      @param name the host name
     *      @param options the options
     *      @return returns the subject name matching name, or undefined if no subject name matches name
     *
     */
    checkHost(name: string, options?: FIBJS.GeneralObject): string;

    /**
     * @description Checks whether the certificate matches the given IP address (IPv4 or IPv6)
     *     @param ip the IP address
     *     @return returns ip if the certificate matches, or undefined if it does not
     *
     */
    checkIP(ip: string): string;

    /**
     * @description Checks whether this certificate was issued by the given issuer
     *
     *     issuer may be an X509Certificate object, a PEM string, or a DER/PEM Buffer.
     *     @param issuer the issuer certificate
     *     @return returns true if the certificate was issued by issuer, false otherwise
     *
     */
    checkIssued(issuer: Class_X509Certificate | Class_Buffer | string): boolean;

    /**
     * @description Checks whether the certificate's public key matches the signature of the given private key
     *     @param privateKey the private key
     *     @return returns true if they match, false otherwise
     *
     */
    checkPrivateKey(privateKey: Class_KeyObject): boolean;

    /**
     * @description Verifies that this certificate was signed by the given public key. No other validation checks are performed on the certificate
     *     @param publicKey the public key
     *     @return returns true if verification succeeds, false otherwise
     *
     */
    verify(publicKey: Class_KeyObject): boolean;

}

