/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The KeyObject class represents symmetric or asymmetric keys, each exposing different features
 *
 * The crypto.createSecretKey , crypto.createPublicKey and crypto.createPrivateKey methods are used to create KeyObject instances. KeyObject objects cannot be created directly with the new keyword.
 *
 */
declare class Class_KeyObject extends Class_object {
    /**
     * @description Information about an asymmetric key
     *     The returned result contains the following:
     *     ```JavaScript
     *     {
     *         modulusLength: 2048, // Key size in bits (RSA, DSA).
     *         publicExponent: 65537n, // Public exponent (RSA).
     *         hashAlgorithm:  'sha1', // Name of the message digest (RSA-PSS).
     *         mgf1HashAlgorithm:  'sha1', // Name of the message digest used by MGF1 (RSA-PSS).
     *         saltLength: 20, // Minimum salt length in bytes (RSA-PSS).
     *         divisorLength: , // Size of q in bits (DSA).
     *         namedCurve: '' // Curve name (EC).
     *     }
     *     ```
     *
     */
    readonly asymmetricKeyDetails: FIBJS.GeneralObject;

    /**
     * @description The type of the key
     *
     *     For asymmetric keys, this property indicates the type of the key. Supported key types are:
     *     - 'rsa' (OID 1.2.840.113549.1.1.1)
     *     - 'rsa-pss' (OID 1.2.840.113549.1.1.10)
     *     - 'dsa' (OID 1.2.840.10040.4.1)
     *     - 'ec' (OID 1.2.840.10045.2.1)
     *     - 'x25519' (OID 1.3.101.110)
     *     - 'x448' (OID 1.3.101.111)
     *     - 'ed25519' (OID 1.3.101.112)
     *     - 'ed448' (OID 1.3.101.113)
     *     - 'dh' (OID 1.2.840.113549.1.3.1)
     *
     *     For unrecognized KeyObject types and symmetric keys, this property is undefined .
     *
     */
    readonly asymmetricKeyType: string;

    /**
     * @description For secret keys, this property indicates the key size in bytes. For asymmetric keys, this property is undefined
     */
    readonly symmetricKeySize: number;

    /**
     * @description The type of the key; for secret (symmetric) keys this property is 'secret', for public (asymmetric) keys it is 'public' or 'private'
     */
    readonly type: string;

    /**
     * @description Exports the key's information according to the given options
     *
     *     For symmetric keys, the following encoding options can be used:
     *     - format: must be 'buffer' (default) or 'jwk'
     *
     *     For public keys, the following encoding options can be used:
     *     - format: must be 'pem', 'der' or 'jwk', 'raw' (only EC/SM2/Ed25519/Ed448/X25519/X448)
     *     - type: when format is 'pem' or 'der', type must be one of 'pkcs1' (only RSA) or 'spki'; when format is 'raw', type must be one of 'uncompressed', 'compressed' or 'hybrid'
     *
     *     For private keys, the following encoding options can be used:
     *     - format: must be 'pem', 'der' or 'jwk', 'raw' (only EC/SM2/Ed25519/Ed448/X25519/X448)
     *     - type: must be one of 'pkcs1' (only RSA), 'pkcs8' or 'sec1' (only EC)
     *     - cipher: if specified, PKCS#5 v2.0 password-based encryption is used, encrypting the private key with the given cipher and passphrase
     *     - passphrase: <string> | the password used for encryption, see cipher
     *
     *     When the JWK encoding format is selected, all other encoding options are ignored.
     *
     *     Combinations of the cipher and format options can be used to encrypt PKCS#1, SEC1 and PKCS#8 type keys. PKCS#8 type can be used with any format to encrypt any key algorithm (RSA, EC or DH) by specifying a cipher. When PEM format is used, only PKCS#1 and SEC1 can be encrypted by specifying a cipher. For maximum compatibility, use PKCS#8 for encrypted private keys. Because PKCS#8 defines its own encryption mechanism, PEM-level encryption is not supported when encrypting PKCS#8 keys. For information about PKCS#8 encryption, see RFC 5208; for information about PKCS#1 and SEC1 encryption, see RFC 1421.
     *
     *     @param options the options for exporting the key
     *     @return returns the key's information
     *
     */
    export(options?: FIBJS.GeneralObject): any;

    /**
     * @description Compares whether two KeyObject objects are equal
     *     @param otherKey the KeyObject to compare
     *     @return returns true if the two KeyObject objects are equal, false otherwise
     *
     */
    equals(otherKey: Class_KeyObject): boolean;

}

