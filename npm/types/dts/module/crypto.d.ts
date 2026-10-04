/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/crypto_constants.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
/// <reference path="../interface/ECDH.d.ts" />
/// <reference path="../interface/Digest.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Cipher.d.ts" />
/// <reference path="../interface/Sign.d.ts" />
/// <reference path="../interface/Verify.d.ts" />
/// <reference path="../interface/X509CertificateRequest.d.ts" />
/// <reference path="../module/webcrypto.d.ts" />
/// <reference path="../module/subtle.d.ts" />
/**
 * @description The `crypto` module is fibjs' built-in cryptographic algorithm module. It provides symmetric encryption, asymmetric encryption, digest algorithms, cryptographic random number generators and other features. It must be loaded via `require('crypto')` before use
 *
 *  Main capabilities:
 *
 *  - **Digest**: `createHash`, `createHmac`, `hash` compute message digests and HMAC;
 *  - **Symmetric encryption**: `createCipher`/`createCipheriv`, `createDecipher`/`createDecipheriv`;
 *  - **Asymmetric encryption**: `createSign`/`sign`, `createVerify`/`verify`, `privateEncrypt`/`publicDecrypt` and other public/private key encryption and decryption;
 *  - **Key management**: `createPrivateKey`, `createPublicKey`, `createSecretKey`, `generateKeyPair` generate and import keys;
 *  - **Key exchange**: `createECDH`, `diffieHellman`, `hkdf`, `pbkdf2`, `scrypt` derive and exchange keys;
 *  - **Random numbers**: `randomBytes`, `randomFill`, `getRandomValues`, `randomUUID`;
 *  - **Certificates**: `X509Certificate`, `createCertificateRequest`;
 *  - **Others**: `getHashes`/`getCiphers`/`getCurves` query the supported lists, `timingSafeEqual` constant-time comparison, BBS signatures (`bbsSign`/`bbsVerify`/`proofGen`/`proofVerify`).
 *
 */
declare module 'crypto' {
    /**
     * ! The crypto module's constants object, see crypto_constants
     */
    const constants: typeof import ('crypto_constants');

    /**
     * @description The KeyObject object, see KeyObject
     */
    const KeyObject: typeof Class_KeyObject;

    /**
     * @description The X509Certificate constructor, see X509Certificate
     */
    const X509Certificate: typeof Class_X509Certificate;

    /**
     * @description Gets the hash (digest) algorithms supported by the crypto module
     *      @return returns the array of supported hash algorithms
     *
     */
    function getHashes(): any[];

    /**
     * @description Creates an ECDH object for the given ECC curve name
     *      @param curve the ECC curve name to use
     *      @return returns the ECDH object
     *
     */
    function createECDH(curve: string): Class_ECDH;

    /**
     * @description Creates a message digest object for the given algorithm name
     *      @param algo the algorithm of the message digest object to use
     *      @return returns the message digest object
     *
     */
    function createHash(algo: string): Class_Digest;

    /**
     * @description Creates an hmac message digest object for the given algorithm name
     *      @param algo the algorithm of the message digest object to use
     *      @param key the binary signing key
     *      @return returns the message digest object
     *
     */
    function createHmac(algo: string, key: Class_Buffer): Class_Digest;

    /**
     * @description Creates an hmac message digest object for the given algorithm name
     *      @param algo the algorithm of the message digest object to use
     *      @param key the signing key, a KeyObject
     *      @return returns the message digest object
     *
     */
    function createHmac(algo: string, key: Class_KeyObject): Class_Digest;

    /**
     * @description Gets the symmetric encryption algorithms supported by the crypto module
     *      @return returns the array of supported symmetric encryption algorithms
     *
     */
    function getCiphers(): any[];

    /**
     * @description Gets algorithm information by cipher algorithm name
     *      @param name the name of the algorithm to query
     *      @param options optional parameters; keyLength and ivLength may be specified for further filtering
     *      @return returns an object containing algorithm information, or undefined if the algorithm does not exist or the options do not match. The returned object contains the following properties: name, nid, blockSize, ivLength, keyLength, mode
     *
     */
    function getCipherInfo(name: string, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Gets algorithm information by cipher algorithm NID
     *      @param nid the NID of the algorithm to query
     *      @param options optional parameters; keyLength and ivLength may be specified for further filtering
     *      @return returns an object containing algorithm information, or undefined if the algorithm does not exist or the options do not match. The returned object contains the following properties: name, nid, blockSize, ivLength, keyLength, mode
     *
     */
    function getCipherInfo(nid: number, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Creates a symmetric encryption cipher object
     *      @param algorithm the encryption algorithm to use
     *      @param key the encryption/decryption key to use
     *      @param options the encryption options to use
     *      @return returns the symmetric encryption cipher object
     *
     */
    function createCipher(algorithm: string, key: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a symmetric encryption cipher object
     *      @param algorithm the encryption algorithm to use
     *      @param key the encryption/decryption key to use
     *      @param iv the initialization vector to use
     *      @param options the encryption options to use
     *      @return returns the symmetric encryption cipher object
     *
     */
    function createCipheriv(algorithm: string, key: Class_Buffer, iv: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a symmetric encryption cipher object
     *      @param algorithm the encryption algorithm to use
     *      @param key the encryption/decryption key to use
     *      @param iv the initialization vector to use
     *      @param options the encryption options to use
     *      @return returns the symmetric encryption cipher object
     *
     */
    function createCipheriv(algorithm: string, key: Class_KeyObject, iv: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a symmetric decryption decipher object
     *      @param algorithm the encryption algorithm to use
     *      @param key the encryption/decryption key to use
     *      @param options the encryption options to use
     *      @return returns the symmetric decryption decipher object
     *
     */
    function createDecipher(algorithm: string, key: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a symmetric decryption decipher object
     *      @param algorithm the encryption algorithm to use
     *      @param key the encryption/decryption key to use
     *      @param iv the initialization vector to use
     *      @param options the encryption options to use
     *      @return returns the symmetric decryption decipher object
     *
     */
    function createDecipheriv(algorithm: string, key: Class_Buffer, iv: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a symmetric decryption decipher object
     *      @param algorithm the encryption algorithm to use
     *      @param key the encryption/decryption key to use
     *      @param iv the initialization vector to use
     *      @param options the encryption options to use
     *      @return returns the symmetric decryption decipher object
     *
     */
    function createDecipheriv(algorithm: string, key: Class_KeyObject, iv: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Gets the ecc curves supported by the crypto module
     *      @return returns the supported ecc curves
     *
     */
    function getCurves(): any[];

    /**
     * @description Creates a new key object containing an asymmetric private key
     *      @param key the private key in pem format to use
     *      @return returns the key object of the private key
     *
     */
    function createPrivateKey(key: Class_Buffer): Class_KeyObject;

    /**
     * @description Creates a new key object containing an asymmetric private key
     *
     *     The key parameter specifies the configuration properties for creating the private key. Supported properties include:
     *     - key: a PEM string, DER binary or JWK format object
     *     - format: must be 'pem', 'der', 'jwk' or 'raw'. Default: 'pem'. Bls12381G1/Bls12381G2 only support 'raw'
     *     - type: must be 'pkcs1', 'pkcs8' or 'sec1'. This option is required only when format is 'der', otherwise it is ignored
     *     - namedCurve: used when format is 'raw' to specify the curve name of key; it can be an EC curve name, or SM2/Ed25519/Ed448/X25519/X448/Bls12381G1/Bls12381G2
     *     - passphrase: the password string used for decryption
     *     - encoding: the string encoding used when key is a string
     *
     *      @param key the configuration properties for creating the private key
     *      @return returns the key object of the private key
     *
     */
    function createPrivateKey(key: FIBJS.GeneralObject): Class_KeyObject;

    /**
     * @description Creates a new key object containing an asymmetric public key
     *      @param key the public key in pem format to use
     *      @return returns the key object of the public key
     *
     */
    function createPublicKey(key: Class_Buffer): Class_KeyObject;

    /**
     * @description Creates a new key object containing the public key corresponding to the given private key
     *      @param key the asymmetric private key to use
     *      @return returns the key object of the public key
     *
     */
    function createPublicKey(key: Class_KeyObject): Class_KeyObject;

    /**
     * @description Creates a new key object containing an asymmetric public key
     *
     *     The key parameter specifies the configuration properties for creating the public key. Supported properties include:
     *     - key: a PEM string, DER binary or JWK format object
     *     - format: must be 'pem', 'der', 'jwk' or 'raw'. Default: 'pem'
     *     - type: must be 'pkcs1' or 'sec1'. This option is required only when format is 'der', otherwise it is ignored
     *     - namedCurve: used when format is 'raw' to specify the curve name of key; it can be an EC curve name, or SM2/Ed25519/Ed448/X25519/X448/Bls12381G1/Bls12381G2
     *     - encoding: the string encoding used when key is a string
     *
     *      @param key the configuration properties for creating the public key
     *      @return returns the key object of the public key
     *
     */
    function createPublicKey(key: FIBJS.GeneralObject): Class_KeyObject;

    /**
     * @description Creates a new signing object based on the algorithm specified by algorithm
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param options the signing options to use, unused
     *      @return returns the signing object
     *
     */
    function createSign(algorithm: string, options?: FIBJS.GeneralObject): Class_Sign;

    /**
     * @description Creates a new verification object based on the algorithm specified by algorithm
     *      @param algorithm the verification algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param options the verification options to use, unused
     *      @return returns the verification object
     *
     */
    function createVerify(algorithm: string, options?: FIBJS.GeneralObject): Class_Verify;

    /**
     * @description Creates a new key object containing a symmetric encryption or Hmac key
     *      @param key the encryption/decryption key to use
     *      @param encoding the encoding of the key, default "buffer"
     *      @return returns the symmetric decryption decipher object
     *
     */
    function createSecretKey(key: Class_Buffer, encoding?: string): Class_KeyObject;

    /**
     * @description Creates a new key object containing a symmetric encryption or Hmac key
     *      @param key the encryption/decryption key to use
     *      @param encoding the encoding of the key, default "buffer"
     *      @return returns the symmetric decryption decipher object
     *
     */
    function createSecretKey(key: string, encoding: string): Class_KeyObject;

    /**
     * @description Creates a new certificate request object
     *      @param csr the data of the certificate request in PEM format to use
     *      @return returns the certificate request object
     *
     */
    function createCertificateRequest(csr: Class_Buffer): Class_X509CertificateRequest;

    /**
     * @description Creates a new certificate request object
     *
     *      The parameters in options are used to call crypto.createPrivateKey to create the private key object; subject and hashAlgorithm can also be specified. Example:
     *
     *      ```JavaScript
     *         var pk = crypto.createPrivateKey(rsa4096_pem);
     *         var req = crypto.createCertificateRequest({
     *             key: pk,
     *             hashAlgorithm: 'sha256', // Default is 'sha256', if key is SM2 type, default is 'sm3'
     *             subject: {
     *                 C: "CN",
     *                 O: "baoz.cn",
     *                 CN: "baoz.me"
     *             }
     *         });
     *      ```
     *
     *      @param options the options for creating the certificate request
     *      @return returns the certificate request object
     *
     */
    function createCertificateRequest(options: FIBJS.GeneralObject): Class_X509CertificateRequest;

    /**
     * @description Computes a Diffie-Hellman key from privateKey and publicKey
     *
     *      options supports the following properties:
     *       - privateKey: the private key used for the computation
     *       - publicKey: the public key used for the computation
     *
     *      @param options the options for the Diffie-Hellman key computation
     *      @return returns the Diffie-Hellman key
     *
     */
    function diffieHellman(options: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description A utility for creating a one-shot hash digest of data. When hashing a small amount of available data (<= 5MB), it is faster than the object-based crypto.createHash(). If the data is large or streamed, crypto.createHash() is still recommended
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param outputEncoding the output encoding, default "hex"
     *      @return returns the hashed data
     *
     */
    function hash(algorithm: string, data: Class_Buffer, outputEncoding?: string): any;

    /**
     * @description Generates a random number of the specified size using the havege generator
     *      @param size the size of the random number to generate
     *      @return returns the generated random number
     *
     */
    function randomBytes(size?: number): Class_Buffer;

    /**
     * @description Fills the specified Buffer with random numbers using the havege generator
     *      @param buffer the Buffer to fill
     *      @param offset the starting offset, default 0
     *      @param size the size of the random numbers to generate, default buffer.length - offset
     *      @return returns the generated random number
     *
     */
    function randomFill(buffer: Class_Buffer, offset?: number, size?: number): Class_Buffer;

    function randomFill(buffer: Class_Buffer, offset?: number, size?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Fills the specified Buffer with random numbers using the havege generator
     *      @param buffer the Buffer to fill
     *      @param offset the starting offset, default 0
     *      @param size the size of the random numbers to generate, default buffer.length - offset
     *      @return returns the generated random number
     *
     */
    function randomFillSync(buffer: Class_Buffer, offset?: number, size?: number): Class_Buffer;

    /**
     * @description Fills the specified Buffer with random numbers using the havege generator
     *      @param buffer the Buffer to fill
     *      @param offset the starting offset, default 0
     *      @param size the size of the random numbers to generate, default buffer.length - offset
     *      @return returns the generated random number
     *
     */
    function randomFillAsync(buffer: Class_Buffer, offset?: number, size?: number): Promise<Class_Buffer>;

    /**
     * @description Fills the specified TypedArray with strong random numbers
     *      @param data the TypedArray to fill
     *      @return returns the filled TypedArray
     *
     */
    function getRandomValues(data: TypedArray): TypedArray;

    /**
     * @description Generates a random RFC 4122 version 4 UUID
     *      @param options optional parameters; disableEntropyCache may be specified to disable the entropy cache (the option is ignored and kept only for compatibility)
     *      @return returns a UUID v4 string
     *
     */
    function randomUUID(options?: FIBJS.GeneralObject): string;

    /**
     * @description Generates a new asymmetric key pair of the given type. Currently supports RSA, RSA-PSS, DSA, EC, Ed25519, Ed448, X25519, X448, SM2, Bls12381G1, Bls12381G2
     *
     *     options supports the following properties:
     *     - modulusLength: key size in bits (RSA, DSA).
     *     - publicExponent: public exponent (RSA). Default: 0x10001.
     *     - hashAlgorithm: name of the message digest (RSA-PSS).
     *     - mgf1HashAlgorithm: name of the message digest used by MGF1 (RSA-PSS).
     *     - saltLength: minimum salt length in bytes (RSA-PSS).
     *     - divisorLength: size of q in bits (DSA).
     *     - namedCurve: name of the curve to use (EC).
     *     - prime: the prime parameter (DH).
     *     - primeLength: prime length in bits (DH).
     *     - generator: custom generator (DH). Default: 2.
     *     - groupName: <string> Diffie-Hellman group name (DH). See crypto.getDiffieHellman.
     *     - paramEncoding: must be 'named' or 'explicit' (EC). Default: 'named'.
     *     - publicKeyEncoding: see keyObject.export.
     *     - privateKeyEncoding: see keyObject.export.
     *
     *     @param type the key type to generate; must be 'rsa', 'rsa-pss', 'dsa', 'ec', 'ed25519', 'x25519', 'x448', 'sm2', 'Bls12381G1', 'Bls12381G2'
     *     @param options the options for generating the key
     *     @return returns an object containing the generated key pair
     *
     */
    function generateKeyPair(type: string, options?: FIBJS.GeneralObject): {
        publicKey: any;
        privateKey: any;
    };

    function generateKeyPair(type: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: {
        publicKey: any;
        privateKey: any;
    })=>any): void;

    /**
     * @description Generates a new asymmetric key pair of the given type. Currently supports RSA, RSA-PSS, DSA, EC, Ed25519, Ed448, X25519, X448, SM2, Bls12381G1, Bls12381G2
     *
     *     options supports the following properties:
     *     - modulusLength: key size in bits (RSA, DSA).
     *     - publicExponent: public exponent (RSA). Default: 0x10001.
     *     - hashAlgorithm: name of the message digest (RSA-PSS).
     *     - mgf1HashAlgorithm: name of the message digest used by MGF1 (RSA-PSS).
     *     - saltLength: minimum salt length in bytes (RSA-PSS).
     *     - divisorLength: size of q in bits (DSA).
     *     - namedCurve: name of the curve to use (EC).
     *     - prime: the prime parameter (DH).
     *     - primeLength: prime length in bits (DH).
     *     - generator: custom generator (DH). Default: 2.
     *     - groupName: <string> Diffie-Hellman group name (DH). See crypto.getDiffieHellman.
     *     - paramEncoding: must be 'named' or 'explicit' (EC). Default: 'named'.
     *     - publicKeyEncoding: see keyObject.export.
     *     - privateKeyEncoding: see keyObject.export.
     *
     *     @param type the key type to generate; must be 'rsa', 'rsa-pss', 'dsa', 'ec', 'ed25519', 'x25519', 'x448', 'sm2', 'Bls12381G1', 'Bls12381G2'
     *     @param options the options for generating the key
     *     @return returns an object containing the generated key pair
     *
     */
    function generateKeyPairSync(type: string, options?: FIBJS.GeneralObject): {
        publicKey: any;
        privateKey: any;
    };

    /**
     * @description Generates a new asymmetric key pair of the given type. Currently supports RSA, RSA-PSS, DSA, EC, Ed25519, Ed448, X25519, X448, SM2, Bls12381G1, Bls12381G2
     *
     *     options supports the following properties:
     *     - modulusLength: key size in bits (RSA, DSA).
     *     - publicExponent: public exponent (RSA). Default: 0x10001.
     *     - hashAlgorithm: name of the message digest (RSA-PSS).
     *     - mgf1HashAlgorithm: name of the message digest used by MGF1 (RSA-PSS).
     *     - saltLength: minimum salt length in bytes (RSA-PSS).
     *     - divisorLength: size of q in bits (DSA).
     *     - namedCurve: name of the curve to use (EC).
     *     - prime: the prime parameter (DH).
     *     - primeLength: prime length in bits (DH).
     *     - generator: custom generator (DH). Default: 2.
     *     - groupName: <string> Diffie-Hellman group name (DH). See crypto.getDiffieHellman.
     *     - paramEncoding: must be 'named' or 'explicit' (EC). Default: 'named'.
     *     - publicKeyEncoding: see keyObject.export.
     *     - privateKeyEncoding: see keyObject.export.
     *
     *     @param type the key type to generate; must be 'rsa', 'rsa-pss', 'dsa', 'ec', 'ed25519', 'x25519', 'x448', 'sm2', 'Bls12381G1', 'Bls12381G2'
     *     @param options the options for generating the key
     *     @return returns an object containing the generated key pair
     *
     */
    function generateKeyPairAsync(type: string, options?: FIBJS.GeneralObject): Promise<{
        publicKey: any;
        privateKey: any;
    }>;

    /**
     * @description Derives the required binary key from the plaintext password according to rfc5869
     *      @param algoName the hash algorithm to use, see the hash module
     *      @param password the password to use
     *      @param salt the salt used by khdf
     *      @param info the info used by khdf
     *      @param size the key size to use
     *      @return returns the generated binary key
     *
     */
    function hkdf(algoName: string, password: Class_Buffer, salt: Class_Buffer, info: Class_Buffer, size: number): Class_Buffer;

    function hkdf(algoName: string, password: Class_Buffer, salt: Class_Buffer, info: Class_Buffer, size: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Derives the required binary key from the plaintext password according to rfc5869
     *      @param algoName the hash algorithm to use, see the hash module
     *      @param password the password to use
     *      @param salt the salt used by khdf
     *      @param info the info used by khdf
     *      @param size the key size to use
     *      @return returns the generated binary key
     *
     */
    function hkdfSync(algoName: string, password: Class_Buffer, salt: Class_Buffer, info: Class_Buffer, size: number): Class_Buffer;

    /**
     * @description Derives the required binary key from the plaintext password according to rfc5869
     *      @param algoName the hash algorithm to use, see the hash module
     *      @param password the password to use
     *      @param salt the salt used by khdf
     *      @param info the info used by khdf
     *      @param size the key size to use
     *      @return returns the generated binary key
     *
     */
    function hkdfAsync(algoName: string, password: Class_Buffer, salt: Class_Buffer, info: Class_Buffer, size: number): Promise<Class_Buffer>;

    /**
     * @description Derives the required binary key from the plaintext password using the pbkdf2 algorithm
     *      @param password the password to use
     *      @param salt the salt used by hmac
     *      @param iterations the number of iterations to use
     *      @param size the key size to use
     *      @param algoName the hash algorithm to use, see the hash module
     *      @return returns the generated binary key
     *
     */
    function pbkdf2(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algoName: string): Class_Buffer;

    function pbkdf2(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algoName: string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Derives the required binary key from the plaintext password using the pbkdf2 algorithm
     *      @param password the password to use
     *      @param salt the salt used by hmac
     *      @param iterations the number of iterations to use
     *      @param size the key size to use
     *      @param algoName the hash algorithm to use, see the hash module
     *      @return returns the generated binary key
     *
     */
    function pbkdf2Sync(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algoName: string): Class_Buffer;

    /**
     * @description Derives the required binary key from the plaintext password using the pbkdf2 algorithm
     *      @param password the password to use
     *      @param salt the salt used by hmac
     *      @param iterations the number of iterations to use
     *      @param size the key size to use
     *      @param algoName the hash algorithm to use, see the hash module
     *      @return returns the generated binary key
     *
     */
    function pbkdf2Async(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algoName: string): Promise<Class_Buffer>;

    /**
     * @description Generates a key using the scrypt algorithm
     *      @param password the password to use
     *      @param salt the salt to use
     *      @param keylen the length of the key to generate
     *      @param options optional parameters; supports N, r, p, maxmem
     *      @return returns the generated binary key
     *
     */
    function scrypt(password: Class_Buffer, salt: Class_Buffer, keylen: number, options?: FIBJS.GeneralObject): Class_Buffer;

    function scrypt(password: Class_Buffer, salt: Class_Buffer, keylen: number, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Generates a key using the scrypt algorithm
     *      @param password the password to use
     *      @param salt the salt to use
     *      @param keylen the length of the key to generate
     *      @param options optional parameters; supports N, r, p, maxmem
     *      @return returns the generated binary key
     *
     */
    function scryptSync(password: Class_Buffer, salt: Class_Buffer, keylen: number, options?: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Generates a key using the scrypt algorithm
     *      @param password the password to use
     *      @param salt the salt to use
     *      @param keylen the length of the key to generate
     *      @param options optional parameters; supports N, r, p, maxmem
     *      @return returns the generated binary key
     *
     */
    function scryptAsync(password: Class_Buffer, salt: Class_Buffer, keylen: number, options?: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decrypts buffer with privateKey. buffer was previously encrypted with the corresponding public key
     *      @param privateKey the private key to use
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function privateDecrypt(privateKey: Class_Buffer, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description Decrypts buffer with privateKey. buffer was previously encrypted with the corresponding public key
     *      @param privateKey the private key to use
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function privateDecrypt(privateKey: Class_KeyObject, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description Decrypts buffer with the private key and configuration specified by key. buffer was previously encrypted with the corresponding public key
     *      @param key the private key and configuration to use
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function privateDecrypt(key: FIBJS.GeneralObject, buffer: any): Class_Buffer;

    /**
     * @description Encrypts buffer with privateKey. The returned data can be decrypted with the corresponding public key
     *      @param privateKey the private key to use
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function privateEncrypt(privateKey: Class_Buffer, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description Encrypts buffer with privateKey. The returned data can be decrypted with the corresponding public key
     *      @param privateKey the private key to use
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function privateEncrypt(privateKey: Class_KeyObject, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description Encrypts buffer with the private key and configuration specified by key. The returned data can be decrypted with the corresponding public key
     *      @param key the private key and configuration to use
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function privateEncrypt(key: FIBJS.GeneralObject, buffer: any): Class_Buffer;

    /**
     * @description Decrypts buffer with publicKey. buffer was previously encrypted with the corresponding private key
     *      @param publicKey the public key to use
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function publicDecrypt(publicKey: Class_Buffer, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description Decrypts buffer with publicKey. buffer was previously encrypted with the corresponding private key
     *      @param publicKey the public key to use
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function publicDecrypt(publicKey: Class_KeyObject, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description Decrypts buffer with the public key and configuration specified by key. buffer was previously encrypted with the corresponding private key
     *      @param key the public key and configuration to use
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function publicDecrypt(key: FIBJS.GeneralObject, buffer: any): Class_Buffer;

    /**
     * @description Encrypts buffer with publicKey. The returned data can be decrypted with the corresponding private key
     *      @param publicKey the private key to use
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function publicEncrypt(publicKey: Class_Buffer, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description Encrypts buffer with publicKey. The returned data can be decrypted with the corresponding private key
     *      @param publicKey the private key to use
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function publicEncrypt(publicKey: Class_KeyObject, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description Encrypts buffer with the private key and configuration specified by key. The returned data can be decrypted with the corresponding private key
     *      @param key the private key and configuration to use
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function publicEncrypt(key: FIBJS.GeneralObject, buffer: any): Class_Buffer;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param privateKey the private key to use
     *      @return returns the signed data
     *
     */
    function sign(algorithm: any, data: Class_Buffer, privateKey: Class_Buffer): Class_Buffer;

    function sign(algorithm: any, data: Class_Buffer, privateKey: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param privateKey the private key to use
     *      @return returns the signed data
     *
     */
    function signSync(algorithm: any, data: Class_Buffer, privateKey: Class_Buffer): Class_Buffer;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param privateKey the private key to use
     *      @return returns the signed data
     *
     */
    function signAsync(algorithm: any, data: Class_Buffer, privateKey: Class_Buffer): Promise<Class_Buffer>;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param privateKey the private key to use
     *      @return returns the signed data
     *
     */
    function sign(algorithm: any, data: Class_Buffer, privateKey: Class_KeyObject): Class_Buffer;

    function sign(algorithm: any, data: Class_Buffer, privateKey: Class_KeyObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param privateKey the private key to use
     *      @return returns the signed data
     *
     */
    function signSync(algorithm: any, data: Class_Buffer, privateKey: Class_KeyObject): Class_Buffer;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param privateKey the private key to use
     *      @return returns the signed data
     *
     */
    function signAsync(algorithm: any, data: Class_Buffer, privateKey: Class_KeyObject): Promise<Class_Buffer>;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The parameters in key are used to call crypto.createPrivateKey to create the private key object; the following signing parameters are also supported:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param key the private key and signing parameters to use
     *      @return returns the signed data
     *
     */
    function sign(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject): Class_Buffer;

    function sign(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The parameters in key are used to call crypto.createPrivateKey to create the private key object; the following signing parameters are also supported:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param key the private key and signing parameters to use
     *      @return returns the signed data
     *
     */
    function signSync(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The parameters in key are used to call crypto.createPrivateKey to create the private key object; the following signing parameters are also supported:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param key the private key and signing parameters to use
     *      @return returns the signed data
     *
     */
    function signAsync(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param publicKey the public key to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verify(algorithm: any, data: Class_Buffer, publicKey: Class_Buffer, signature: Class_Buffer): boolean;

    function verify(algorithm: any, data: Class_Buffer, publicKey: Class_Buffer, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param publicKey the public key to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verifySync(algorithm: any, data: Class_Buffer, publicKey: Class_Buffer, signature: Class_Buffer): boolean;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param publicKey the public key to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verifyAsync(algorithm: any, data: Class_Buffer, publicKey: Class_Buffer, signature: Class_Buffer): Promise<boolean>;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param publicKey the public key to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verify(algorithm: any, data: Class_Buffer, publicKey: Class_KeyObject, signature: Class_Buffer): boolean;

    function verify(algorithm: any, data: Class_Buffer, publicKey: Class_KeyObject, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param publicKey the public key to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verifySync(algorithm: any, data: Class_Buffer, publicKey: Class_KeyObject, signature: Class_Buffer): boolean;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param publicKey the public key to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verifyAsync(algorithm: any, data: Class_Buffer, publicKey: Class_KeyObject, signature: Class_Buffer): Promise<boolean>;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the private key object; the following signing parameters are also supported:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param key the private key and signing parameters to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verify(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject, signature: Class_Buffer): boolean;

    function verify(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the private key object; the following signing parameters are also supported:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param key the private key and signing parameters to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verifySync(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject, signature: Class_Buffer): boolean;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the private key object; the following signing parameters are also supported:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param key the private key and signing parameters to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function verifyAsync(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject, signature: Class_Buffer): Promise<boolean>;

    /**
     * @description Compares whether the two given pieces of data are equal, using constant-time comparison to prevent timing side-channel attacks
     *      @param a the data to compare
     *      @param b the data to compare
     *      @return returns the comparison result
     *
     */
    function timingSafeEqual(a: Class_Buffer, b: Class_Buffer): boolean;

    /**
     * @description Function for BBS signing with Bls12381G2
     *      @param messages the group of messages to sign
     *      @param privateKey the private key to use; must be a Bls12381G2 private key
     *      @return returns the signed data
     *
     */
    function bbsSign(messages: Class_Buffer[], privateKey: Class_Buffer): Class_Buffer;

    function bbsSign(messages: Class_Buffer[], privateKey: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Function for BBS signing with Bls12381G2
     *      @param messages the group of messages to sign
     *      @param privateKey the private key to use; must be a Bls12381G2 private key
     *      @return returns the signed data
     *
     */
    function bbsSignSync(messages: Class_Buffer[], privateKey: Class_Buffer): Class_Buffer;

    /**
     * @description Function for BBS signing with Bls12381G2
     *      @param messages the group of messages to sign
     *      @param privateKey the private key to use; must be a Bls12381G2 private key
     *      @return returns the signed data
     *
     */
    function bbsSignAsync(messages: Class_Buffer[], privateKey: Class_Buffer): Promise<Class_Buffer>;

    /**
     * @description Function for BBS signing with Bls12381G2
     *      @param messages the group of messages to sign
     *      @param privateKey the private key to use; must be a Bls12381G2 private key
     *      @return returns the signed data
     *
     */
    function bbsSign(messages: Class_Buffer[], privateKey: Class_KeyObject): Class_Buffer;

    function bbsSign(messages: Class_Buffer[], privateKey: Class_KeyObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Function for BBS signing with Bls12381G2
     *      @param messages the group of messages to sign
     *      @param privateKey the private key to use; must be a Bls12381G2 private key
     *      @return returns the signed data
     *
     */
    function bbsSignSync(messages: Class_Buffer[], privateKey: Class_KeyObject): Class_Buffer;

    /**
     * @description Function for BBS signing with Bls12381G2
     *      @param messages the group of messages to sign
     *      @param privateKey the private key to use; must be a Bls12381G2 private key
     *      @return returns the signed data
     *
     */
    function bbsSignAsync(messages: Class_Buffer[], privateKey: Class_KeyObject): Promise<Class_Buffer>;

    /**
     * @description Function for BBS signing with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPrivateKey to create the private key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      @param messages the group of messages to sign
     *      @param key the private key and options to use
     *      @return returns the signed data
     *
     */
    function bbsSign(messages: Class_Buffer[], key: FIBJS.GeneralObject): Class_Buffer;

    function bbsSign(messages: Class_Buffer[], key: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Function for BBS signing with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPrivateKey to create the private key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      @param messages the group of messages to sign
     *      @param key the private key and options to use
     *      @return returns the signed data
     *
     */
    function bbsSignSync(messages: Class_Buffer[], key: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Function for BBS signing with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPrivateKey to create the private key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      @param messages the group of messages to sign
     *      @param key the private key and options to use
     *      @return returns the signed data
     *
     */
    function bbsSignAsync(messages: Class_Buffer[], key: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Function for BBS verification with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerify(messages: Class_Buffer[], publicKey: Class_Buffer, signature: Class_Buffer): boolean;

    function bbsVerify(messages: Class_Buffer[], publicKey: Class_Buffer, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Function for BBS verification with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerifySync(messages: Class_Buffer[], publicKey: Class_Buffer, signature: Class_Buffer): boolean;

    /**
     * @description Function for BBS verification with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerifyAsync(messages: Class_Buffer[], publicKey: Class_Buffer, signature: Class_Buffer): Promise<boolean>;

    /**
     * @description Function for BBS verification with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerify(messages: Class_Buffer[], publicKey: Class_KeyObject, signature: Class_Buffer): boolean;

    function bbsVerify(messages: Class_Buffer[], publicKey: Class_KeyObject, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Function for BBS verification with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerifySync(messages: Class_Buffer[], publicKey: Class_KeyObject, signature: Class_Buffer): boolean;

    /**
     * @description Function for BBS verification with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerifyAsync(messages: Class_Buffer[], publicKey: Class_KeyObject, signature: Class_Buffer): Promise<boolean>;

    /**
     * @description Function for BBS verification with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      @param messages the group of messages to verify
     *      @param key the public key and options to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerify(messages: Class_Buffer[], key: FIBJS.GeneralObject, signature: Class_Buffer): boolean;

    function bbsVerify(messages: Class_Buffer[], key: FIBJS.GeneralObject, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Function for BBS verification with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      @param messages the group of messages to verify
     *      @param key the public key and options to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerifySync(messages: Class_Buffer[], key: FIBJS.GeneralObject, signature: Class_Buffer): boolean;

    /**
     * @description Function for BBS verification with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      @param messages the group of messages to verify
     *      @param key the public key and options to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerifyAsync(messages: Class_Buffer[], key: FIBJS.GeneralObject, signature: Class_Buffer): Promise<boolean>;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @return returns the proof data
     *
     */
    function proofGen(signature: Class_Buffer, messages: Class_Buffer[], index: number[], publicKey: Class_Buffer): Class_Buffer;

    function proofGen(signature: Class_Buffer, messages: Class_Buffer[], index: number[], publicKey: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @return returns the proof data
     *
     */
    function proofGenSync(signature: Class_Buffer, messages: Class_Buffer[], index: number[], publicKey: Class_Buffer): Class_Buffer;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @return returns the proof data
     *
     */
    function proofGenAsync(signature: Class_Buffer, messages: Class_Buffer[], index: number[], publicKey: Class_Buffer): Promise<Class_Buffer>;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @return returns the proof data
     *
     */
    function proofGen(signature: Class_Buffer, messages: Class_Buffer[], index: number[], publicKey: Class_KeyObject): Class_Buffer;

    function proofGen(signature: Class_Buffer, messages: Class_Buffer[], index: number[], publicKey: Class_KeyObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @return returns the proof data
     *
     */
    function proofGenSync(signature: Class_Buffer, messages: Class_Buffer[], index: number[], publicKey: Class_KeyObject): Class_Buffer;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @return returns the proof data
     *
     */
    function proofGenAsync(signature: Class_Buffer, messages: Class_Buffer[], index: number[], publicKey: Class_KeyObject): Promise<Class_Buffer>;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *       - proof_header: additional data used for the proof
     *
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param key the public key and options to use
     *      @return returns the proof data
     *
     */
    function proofGen(signature: Class_Buffer, messages: Class_Buffer[], index: number[], key: FIBJS.GeneralObject): Class_Buffer;

    function proofGen(signature: Class_Buffer, messages: Class_Buffer[], index: number[], key: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *       - proof_header: additional data used for the proof
     *
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param key the public key and options to use
     *      @return returns the proof data
     *
     */
    function proofGenSync(signature: Class_Buffer, messages: Class_Buffer[], index: number[], key: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *       - proof_header: additional data used for the proof
     *
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param key the public key and options to use
     *      @return returns the proof data
     *
     */
    function proofGenAsync(signature: Class_Buffer, messages: Class_Buffer[], index: number[], key: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerify(messages: Class_Buffer[], index: number[], publicKey: Class_Buffer, proof: Class_Buffer): boolean;

    function proofVerify(messages: Class_Buffer[], index: number[], publicKey: Class_Buffer, proof: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerifySync(messages: Class_Buffer[], index: number[], publicKey: Class_Buffer, proof: Class_Buffer): boolean;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerifyAsync(messages: Class_Buffer[], index: number[], publicKey: Class_Buffer, proof: Class_Buffer): Promise<boolean>;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerify(messages: Class_Buffer[], index: number[], publicKey: Class_KeyObject, proof: Class_Buffer): boolean;

    function proofVerify(messages: Class_Buffer[], index: number[], publicKey: Class_KeyObject, proof: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerifySync(messages: Class_Buffer[], index: number[], publicKey: Class_KeyObject, proof: Class_Buffer): boolean;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key to use; must be a Bls12381G2 public key
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerifyAsync(messages: Class_Buffer[], index: number[], publicKey: Class_KeyObject, proof: Class_Buffer): Promise<boolean>;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *       - proof_header: additional data used for the proof
     *
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param key the public key and options to use
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerify(messages: Class_Buffer[], index: number[], key: FIBJS.GeneralObject, proof: Class_Buffer): boolean;

    function proofVerify(messages: Class_Buffer[], index: number[], key: FIBJS.GeneralObject, proof: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *       - proof_header: additional data used for the proof
     *
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param key the public key and options to use
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerifySync(messages: Class_Buffer[], index: number[], key: FIBJS.GeneralObject, proof: Class_Buffer): boolean;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *
     *      The parameters in key are used to call crypto.createPublicKey to create the public key object; the following signing parameters are also supported:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *       - proof_header: additional data used for the proof
     *
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param key the public key and options to use
     *      @param proof the proof data to use
     *      @return returns the verification result
     *
     */
    function proofVerifyAsync(messages: Class_Buffer[], index: number[], key: FIBJS.GeneralObject, proof: Class_Buffer): Promise<boolean>;

    /**
     * @description WebCrypto API module
     */
    const webcrypto: typeof import ('webcrypto');

    /**
     * @description Provides access to the SubtleCrypto API
     */
    const subtle: typeof import ('subtle');

}

