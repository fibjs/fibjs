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
 * The promise variant of the crypto module: async members return a Promise as their primary form.
 */
declare module 'crypto/promises' {
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
    function getHashes(): string[];

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
     *
     *      key may be a Buffer, a KeyObject, or a string; a string is encoded as utf8.
     *      @param algo the algorithm of the message digest object to use, a string is encoded as utf8
     *      @param key the binary signing key
     *      @return returns the message digest object
     *
     */
    function createHmac(algo: string, key: Class_Buffer | Class_KeyObject | string): Class_Digest;

    /**
     * @description Gets the symmetric encryption algorithms supported by the crypto module
     *      @return returns the array of supported symmetric encryption algorithms
     *
     */
    function getCiphers(): string[];

    /**
     * @description Gets algorithm information by cipher algorithm name or NID
     *      nameOrNid is a string, looked up by name, or a number, looked up by NID.
     *      @param nameOrNid the name or the NID of the algorithm to query
     *      @param options optional parameters; keyLength and ivLength may be specified for further filtering
     *      @return returns an object containing algorithm information, or undefined if the algorithm does not exist or the options do not match. The returned object contains the following properties: name, nid, blockSize, ivLength, keyLength, mode
     *
     */
    function getCipherInfo(nameOrNid: string | number, options?: FIBJS.GeneralObject): {
        name: string;
        nid: number;
        blockSize: number;
        ivLength: number;
        keyLength: number;
        mode: string;
    };

    /**
     * @description Creates a symmetric encryption cipher object
     *
     *      key may be a Buffer or a string; a string is encoded as utf8.
     *      @param algorithm the encryption algorithm to use, a string is encoded as utf8
     *      @param key the encryption/decryption key to use
     *      @param options the encryption options to use
     *      @return returns the symmetric encryption cipher object
     *
     */
    function createCipher(algorithm: string, key: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a symmetric encryption cipher object
     *
     *      key may be a Buffer, a KeyObject, or a string; iv may be a Buffer or a string; a string is encoded as utf8.
     *      @param algorithm the encryption algorithm to use, a string is encoded as utf8
     *      @param key the encryption/decryption key to use
     *      @param iv the initialization vector to use
     *      @param options the encryption options to use
     *      @return returns the symmetric encryption cipher object
     *
     */
    function createCipheriv(algorithm: string, key: Class_Buffer | Class_KeyObject | string, iv: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a symmetric decryption decipher object
     *
     *      key may be a Buffer or a string; a string is encoded as utf8.
     *      @param algorithm the encryption algorithm to use, a string is encoded as utf8
     *      @param key the encryption/decryption key to use
     *      @param options the encryption options to use
     *      @return returns the symmetric decryption decipher object
     *
     */
    function createDecipher(algorithm: string, key: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a symmetric decryption decipher object
     *
     *      key may be a Buffer, a KeyObject, or a string; iv may be a Buffer or a string; a string is encoded as utf8.
     *      @param algorithm the encryption algorithm to use, a string is encoded as utf8
     *      @param key the encryption/decryption key to use
     *      @param iv the initialization vector to use
     *      @param options the encryption options to use
     *      @return returns the symmetric decryption decipher object
     *
     */
    function createDecipheriv(algorithm: string, key: Class_Buffer | Class_KeyObject | string, iv: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Gets the ecc curves supported by the crypto module
     *      @return returns the supported ecc curves
     *
     */
    function getCurves(): string[];

    /**
     * @description Creates a new key object containing an asymmetric private key from a PEM string
     *
     *      key may be a PEM/DER Buffer or string, or an options object carrying the key material and its format.
     *      @param key the private key to use
     *      @return returns the key object of the private key
     *
     */
    function createPrivateKey(key: Class_Buffer | FIBJS.GeneralObject | string): Class_KeyObject;

    /**
     * @description Creates a new key object containing an asymmetric public key from a PEM string
     *
     *      key may be a KeyObject, a PEM/DER Buffer or string, or an options object carrying the key material and its format.
     *      @param key the public key to use
     *      @return returns the key object of the public key
     *
     */
    function createPublicKey(key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_KeyObject;

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
     *      key may be a Buffer, or a string decoded with encoding.
     *      @param key the encryption/decryption key to use
     *      @param encoding the encoding of a string key, default "utf8"
     *      @return returns the symmetric decryption decipher object
     *
     */
    function createSecretKey(key: Class_Buffer | string, encoding?: string): Class_KeyObject;

    /**
     * @description Creates a new certificate request object
     *
     *      The request is either given as its PEM/DER data, or built from an options object whose parameters are used to call crypto.createPrivateKey to create the private key object; subject and hashAlgorithm can also be specified. Example:
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
     *      csr may be the PEM/DER data of the certificate request, or the options object used to create it.
     *      @param csr the certificate request data or the options to create it
     *      @return returns the certificate request object
     *
     */
    function createCertificateRequest(csr: Class_Buffer | FIBJS.GeneralObject | string): Class_X509CertificateRequest;

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
     *
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms, a string is encoded as utf8
     *      @param data the data to hash
     *      @param outputEncoding the output encoding, default "hex", a string is encoded as utf8
     *      @return returns the hashed data
     *
     */
    function hash(algorithm: string, data: Class_Buffer | string, outputEncoding?: string): any;

    /**
     * @description Generates a random number of the specified size using the havege generator
     *      @param size the size of the random number to generate
     *      @return returns the generated random number
     *
     */
    function randomBytes(size?: number): Class_Buffer;

    /**
     * @description Fills the specified Buffer with random numbers using the havege generator
     *
     *      buffer may be a Buffer or a string; a string is encoded as utf8.
     *      @param buffer the Buffer to fill
     *      @param offset the starting offset, default 0
     *      @param size the size of the random numbers to generate, default buffer.length - offset
     *      @return returns the generated random number
     *
     */
    function randomFill(buffer: Class_Buffer | string, offset?: number, size?: number): Promise<Class_Buffer>;

    /**
     * @description Fills the specified Buffer with random numbers using the havege generator
     *
     *      buffer may be a Buffer or a string; a string is encoded as utf8.
     *      @param buffer the Buffer to fill
     *      @param offset the starting offset, default 0
     *      @param size the size of the random numbers to generate, default buffer.length - offset
     *      @return returns the generated random number
     *
     */
    function randomFillSync(buffer: Class_Buffer | string, offset?: number, size?: number): Class_Buffer;

    /**
     * @description Fills the specified Buffer with random numbers using the havege generator
     *
     *      buffer may be a Buffer or a string; a string is encoded as utf8.
     *      @param buffer the Buffer to fill
     *      @param offset the starting offset, default 0
     *      @param size the size of the random numbers to generate, default buffer.length - offset
     *      @return returns the generated random number
     *
     */
    function randomFillAsync(buffer: Class_Buffer | string, offset?: number, size?: number): Promise<Class_Buffer>;

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
    function generateKeyPair(type: string, options?: FIBJS.GeneralObject): Promise<{
        publicKey: any;
        privateKey: any;
    }>;

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
     *
     *      password, salt and info may each be a Buffer or a string; a string is encoded as utf8.
     *      @param algoName the hash algorithm to use, see the hash module, a string is encoded as utf8
     *      @param password the password to use
     *      @param salt the salt used by hkdf
     *      @param info the info used by hkdf
     *      @param size the key size to use
     *      @return returns the generated binary key
     *
     */
    function hkdf(algoName: string, password: Class_Buffer | string, salt: Class_Buffer | string, info: Class_Buffer | string, size: number): Promise<Class_Buffer>;

    /**
     * @description Derives the required binary key from the plaintext password according to rfc5869
     *
     *      password, salt and info may each be a Buffer or a string; a string is encoded as utf8.
     *      @param algoName the hash algorithm to use, see the hash module, a string is encoded as utf8
     *      @param password the password to use
     *      @param salt the salt used by hkdf
     *      @param info the info used by hkdf
     *      @param size the key size to use
     *      @return returns the generated binary key
     *
     */
    function hkdfSync(algoName: string, password: Class_Buffer | string, salt: Class_Buffer | string, info: Class_Buffer | string, size: number): Class_Buffer;

    /**
     * @description Derives the required binary key from the plaintext password according to rfc5869
     *
     *      password, salt and info may each be a Buffer or a string; a string is encoded as utf8.
     *      @param algoName the hash algorithm to use, see the hash module, a string is encoded as utf8
     *      @param password the password to use
     *      @param salt the salt used by hkdf
     *      @param info the info used by hkdf
     *      @param size the key size to use
     *      @return returns the generated binary key
     *
     */
    function hkdfAsync(algoName: string, password: Class_Buffer | string, salt: Class_Buffer | string, info: Class_Buffer | string, size: number): Promise<Class_Buffer>;

    /**
     * @description Derives the required binary key from the plaintext password using the pbkdf2 algorithm
     *
     *      password and salt may each be a Buffer or a string; a string is encoded as utf8.
     *      @param password the password to use
     *      @param salt the salt used by hmac
     *      @param iterations the number of iterations to use
     *      @param size the key size to use
     *      @param algoName the hash algorithm to use, see the hash module, a string is encoded as utf8
     *      @return returns the generated binary key
     *
     */
    function pbkdf2(password: Class_Buffer | string, salt: Class_Buffer | string, iterations: number, size: number, algoName: string): Promise<Class_Buffer>;

    /**
     * @description Derives the required binary key from the plaintext password using the pbkdf2 algorithm
     *
     *      password and salt may each be a Buffer or a string; a string is encoded as utf8.
     *      @param password the password to use
     *      @param salt the salt used by hmac
     *      @param iterations the number of iterations to use
     *      @param size the key size to use
     *      @param algoName the hash algorithm to use, see the hash module, a string is encoded as utf8
     *      @return returns the generated binary key
     *
     */
    function pbkdf2Sync(password: Class_Buffer | string, salt: Class_Buffer | string, iterations: number, size: number, algoName: string): Class_Buffer;

    /**
     * @description Derives the required binary key from the plaintext password using the pbkdf2 algorithm
     *
     *      password and salt may each be a Buffer or a string; a string is encoded as utf8.
     *      @param password the password to use
     *      @param salt the salt used by hmac
     *      @param iterations the number of iterations to use
     *      @param size the key size to use
     *      @param algoName the hash algorithm to use, see the hash module, a string is encoded as utf8
     *      @return returns the generated binary key
     *
     */
    function pbkdf2Async(password: Class_Buffer | string, salt: Class_Buffer | string, iterations: number, size: number, algoName: string): Promise<Class_Buffer>;

    /**
     * @description Generates a key using the scrypt algorithm
     *
     *      password and salt may each be a Buffer or a string; a string is encoded as utf8.
     *      @param password the password to use
     *      @param salt the salt to use
     *      @param keylen the length of the key to generate
     *      @param options optional parameters; supports N, r, p, maxmem
     *      @return returns the generated binary key
     *
     */
    function scrypt(password: Class_Buffer | string, salt: Class_Buffer | string, keylen: number, options?: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Generates a key using the scrypt algorithm
     *
     *      password and salt may each be a Buffer or a string; a string is encoded as utf8.
     *      @param password the password to use
     *      @param salt the salt to use
     *      @param keylen the length of the key to generate
     *      @param options optional parameters; supports N, r, p, maxmem
     *      @return returns the generated binary key
     *
     */
    function scryptSync(password: Class_Buffer | string, salt: Class_Buffer | string, keylen: number, options?: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Generates a key using the scrypt algorithm
     *
     *      password and salt may each be a Buffer or a string; a string is encoded as utf8.
     *      @param password the password to use
     *      @param salt the salt to use
     *      @param keylen the length of the key to generate
     *      @param options optional parameters; supports N, r, p, maxmem
     *      @return returns the generated binary key
     *
     */
    function scryptAsync(password: Class_Buffer | string, salt: Class_Buffer | string, keylen: number, options?: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decrypts buffer with the private key and configuration specified by key. buffer was previously encrypted with the corresponding public key
     *
     *      The private key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPrivateKey; the object also carries the RSA options (padding, oaepHash, oaepLabel and encoding).
     *
     *      buffer may be a Buffer, or a string decoded with the options' encoding (default utf8); the options object is required for the string form.
     *      @param privateKey the private key and configuration to use
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function privateDecrypt(privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, buffer: Class_Buffer | string): Class_Buffer;

    /**
     * @description Encrypts buffer with the private key and configuration specified by key. The returned data can be decrypted with the corresponding public key
     *
     *      The private key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPrivateKey; the object also carries the RSA options (padding, oaepHash, oaepLabel and encoding).
     *
     *      buffer may be a Buffer, or a string decoded with the options' encoding (default utf8); the options object is required for the string form.
     *      @param privateKey the private key and configuration to use
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function privateEncrypt(privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, buffer: Class_Buffer | string): Class_Buffer;

    /**
     * @description Decrypts buffer with the public key and configuration specified by key. buffer was previously encrypted with the corresponding private key
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey; the object also carries the RSA options (padding, oaepHash, oaepLabel and encoding).
     *
     *      buffer may be a Buffer, or a string decoded with the options' encoding (default utf8); the options object is required for the string form.
     *      @param publicKey the public key and configuration to use
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function publicDecrypt(publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, buffer: Class_Buffer | string): Class_Buffer;

    /**
     * @description Encrypts buffer with the public key and configuration specified by key. The returned data can be decrypted with the corresponding private key
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey; the object also carries the RSA options (padding, oaepHash, oaepLabel and encoding).
     *
     *      buffer may be a Buffer, or a string decoded with the options' encoding (default utf8); the options object is required for the string form.
     *      @param publicKey the public key and configuration to use
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function publicEncrypt(publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, buffer: Class_Buffer | string): Class_Buffer;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The private key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPrivateKey; the object also carries the signing parameters:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      data may be a Buffer or a string, a string is encoded as utf8.
     *      key may be a KeyObject, a PEM/DER Buffer or string, or an options object carrying the key material and the signing parameters.
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param key the private key to sign with
     *      @return returns the signed data
     *
     */
    function sign(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The private key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPrivateKey; the object also carries the signing parameters:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      data may be a Buffer or a string, a string is encoded as utf8.
     *      key may be a KeyObject, a PEM/DER Buffer or string, or an options object carrying the key material and the signing parameters.
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param key the private key to sign with
     *      @return returns the signed data
     *
     */
    function signSync(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    /**
     * @description Computes and returns the signature of data using the given private key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The private key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPrivateKey; the object also carries the signing parameters:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      data may be a Buffer or a string, a string is encoded as utf8.
     *      key may be a KeyObject, a PEM/DER Buffer or string, or an options object carrying the key material and the signing parameters.
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to sign
     *      @param key the private key to sign with
     *      @return returns the signed data
     *
     */
    function signAsync(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey; the object also carries the verifying parameters (dsaEncoding, padding and saltLength, see sign).
     *
     *      data may be a Buffer or a string, a string is encoded as utf8.
     *      key may be a KeyObject, a PEM/DER Buffer or string, or an options object carrying the key material and the verifying parameters.
     *      signature may be a Buffer or a string, a string is encoded as utf8.
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param key the public key to verify with
     *      @param signature the signature to verify
     *      @return returns the verification result
     *
     */
    function verify(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey; the object also carries the verifying parameters (dsaEncoding, padding and saltLength, see sign).
     *
     *      data may be a Buffer or a string, a string is encoded as utf8.
     *      key may be a KeyObject, a PEM/DER Buffer or string, or an options object carrying the key material and the verifying parameters.
     *      signature may be a Buffer or a string, a string is encoded as utf8.
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param key the public key to verify with
     *      @param signature the signature to verify
     *      @return returns the verification result
     *
     */
    function verifySync(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): boolean;

    /**
     * @description Verifies the given signature of data using the given key and algorithm. If algorithm is null or undefined, the algorithm depends on the key type (especially Ed25519 and Ed448)
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey; the object also carries the verifying parameters (dsaEncoding, padding and saltLength, see sign).
     *
     *      data may be a Buffer or a string, a string is encoded as utf8.
     *      key may be a KeyObject, a PEM/DER Buffer or string, or an options object carrying the key material and the verifying parameters.
     *      signature may be a Buffer or a string, a string is encoded as utf8.
     *      @param algorithm the signing algorithm to use; use crypto.getHashes to get the names of the available digest algorithms
     *      @param data the data to verify
     *      @param key the public key to verify with
     *      @param signature the signature to verify
     *      @return returns the verification result
     *
     */
    function verifyAsync(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Compares whether the two given pieces of data are equal, using constant-time comparison to prevent timing side-channel attacks
     *
     *      Both pieces of data are used as their bytes: a string is encoded as utf8, and the two must have the same length.
     *      @param a the data to compare
     *      @param b the data to compare
     *      @return returns the comparison result
     *
     */
    function timingSafeEqual(a: Class_Buffer | string, b: Class_Buffer | string): boolean;

    /**
     * @description Function for BBS signing with Bls12381G2
     *
     *      The private key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPrivateKey to create the private key object; the object also carries the signing options:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      privateKey must be a Bls12381G2 private key.
     *      @param messages the group of messages to sign
     *      @param privateKey the private key and options to use
     *      @return returns the signed data
     *
     */
    function bbsSign(messages: (Class_Buffer | string)[], privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Function for BBS signing with Bls12381G2
     *
     *      The private key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPrivateKey to create the private key object; the object also carries the signing options:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      privateKey must be a Bls12381G2 private key.
     *      @param messages the group of messages to sign
     *      @param privateKey the private key and options to use
     *      @return returns the signed data
     *
     */
    function bbsSignSync(messages: (Class_Buffer | string)[], privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    /**
     * @description Function for BBS signing with Bls12381G2
     *
     *      The private key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPrivateKey to create the private key object; the object also carries the signing options:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      privateKey must be a Bls12381G2 private key.
     *      @param messages the group of messages to sign
     *      @param privateKey the private key and options to use
     *      @return returns the signed data
     *
     */
    function bbsSignAsync(messages: (Class_Buffer | string)[], privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Function for BBS verification with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the verifying options:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key. signature may be a Buffer or a string, a string is encoded as utf8.
     *      @param messages the group of messages to verify
     *      @param publicKey the public key and options to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerify(messages: (Class_Buffer | string)[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Function for BBS verification with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the verifying options:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key. signature may be a Buffer or a string, a string is encoded as utf8.
     *      @param messages the group of messages to verify
     *      @param publicKey the public key and options to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerifySync(messages: (Class_Buffer | string)[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): boolean;

    /**
     * @description Function for BBS verification with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the verifying options:
     *       - suite: must be 'Bls12381Sha256', 'Bls12381Shake256'. Default: 'Bls12381Sha256'
     *       - header: additional data used for signing
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key. signature may be a Buffer or a string, a string is encoded as utf8.
     *      @param messages the group of messages to verify
     *      @param publicKey the public key and options to use
     *      @param signature the signature data to use
     *      @return returns the verification result
     *
     */
    function bbsVerifyAsync(messages: (Class_Buffer | string)[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the suite and header options (see bbsSign).
     *
     *      signature may be a Buffer or a string, a string is encoded as utf8.
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key.
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key and options to use
     *      @return returns the proof data
     *
     */
    function proofGen(signature: Class_Buffer | string, messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the suite and header options (see bbsSign).
     *
     *      signature may be a Buffer or a string, a string is encoded as utf8.
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key.
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key and options to use
     *      @return returns the proof data
     *
     */
    function proofGenSync(signature: Class_Buffer | string, messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    /**
     * @description Function for generating a BBS selective proof with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the suite and header options (see bbsSign).
     *
     *      signature may be a Buffer or a string, a string is encoded as utf8.
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key.
     *      @param signature the BBS signature to use
     *      @param messages the group of messages to sign
     *      @param index the indices of the proof to select
     *      @param publicKey the public key and options to use
     *      @return returns the proof data
     *
     */
    function proofGenAsync(signature: Class_Buffer | string, messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the suite and header options (see bbsSign).
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key. proof may be a Buffer or a string, a string is encoded as utf8.
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key and options to use
     *      @param proof the proof data to verify
     *      @return returns the verification result
     *
     */
    function proofVerify(messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, proof: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the suite and header options (see bbsSign).
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key. proof may be a Buffer or a string, a string is encoded as utf8.
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key and options to use
     *      @param proof the proof data to verify
     *      @return returns the verification result
     *
     */
    function proofVerifySync(messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, proof: Class_Buffer | string): boolean;

    /**
     * @description Function for verifying a BBS selective proof with Bls12381G2
     *
     *      The public key is a Buffer holding the key data, a KeyObject, a string holding it in PEM form, or an object whose parameters are used to call crypto.createPublicKey to create the public key object; the object also carries the suite and header options (see bbsSign).
     *
     *      messages may be an array of Buffers or strings; a string message is encoded as utf8.
     *      publicKey must be a Bls12381G2 public key. proof may be a Buffer or a string, a string is encoded as utf8.
     *      @param messages the group of messages to verify
     *      @param index the indices of the proof to select
     *      @param publicKey the public key and options to use
     *      @param proof the proof data to verify
     *      @return returns the verification result
     *
     */
    function proofVerifyAsync(messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, proof: Class_Buffer | string): Promise<boolean>;

    /**
     * @description WebCrypto API module
     */
    const webcrypto: typeof import ('webcrypto');

    /**
     * @description Provides access to the SubtleCrypto API
     */
    const subtle: typeof import ('subtle');

}


declare module "crypto" {
    const promises: typeof import("crypto/promises");
}
