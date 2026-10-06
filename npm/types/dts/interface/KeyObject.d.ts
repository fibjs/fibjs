/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Opaque handle to symmetric or asymmetric key material
 *
 *  A KeyObject bundles a parsed key with its type, so the rest of the program never has
 *  to touch raw key bytes: pass it to createHmac, createCipheriv, sign/verify,
 *  diffieHellman, X509Certificate checks and the other crypto members. Secret keys
 *  hold one byte string; asymmetric keys hold a public key, a private key or a pair
 *  and expose metadata about them.
 *
 *  Concepts:
 *  - **Opaque keys**: the material stays inside the object; export() reproduces it in
 *    an interchange format when it must leave the process. Import key material once
 *    and reuse the same KeyObject instead of re-parsing PEM files.
 *  - **Types**: `type` is 'secret', 'public' or 'private'. An asymmetric key also
 *    reports `asymmetricKeyType` ('rsa', 'rsa-pss', 'dsa', 'dh', 'ec', 'sm2',
 *    'ed25519', 'ed448', 'x25519', 'x448', or the fibjs Bls12381G1/G2 extensions).
 *    A public key can always be derived from a private key with
 *    crypto.createPublicKey, never the other way round.
 *  - **Formats**: PEM is text (string), DER is binary (Buffer), JWK is an object with
 *    base64url fields, and the raw form is the bare point, scalar or seed (Buffer) for
 *    EC, SM2 and the one-shot curves. Private keys can be encrypted by combining a
 *    cipher and a passphrase; crypto.createPrivateKey reads them back with the same
 *    passphrase.
 *  - **One-shot key types**: Ed25519/Ed448/X25519/X448 work only with the one-shot
 *    crypto.sign, crypto.verify and crypto.diffieHellman APIs, not with the streaming
 *    Sign/Verify classes. SM2 and the Bls12381 types are fibjs extensions.
 *
 *  Obtained from:
 *  - `crypto.createSecretKey(key[, encoding])` — wrap raw bytes as a symmetric key;
 *  - `crypto.createPrivateKey(key)` — import a private key from PEM/DER/JWK data or an
 *    options object (a KeyObject is not accepted);
 *  - `crypto.createPublicKey(key)` — import a public key or certificate, or derive the
 *    public part of a private key or private KeyObject;
 *  - `crypto.generateKeyPairSync(type, options)` and `crypto.generateKeyPair(...)` —
 *    produce a pair as KeyObjects unless a key encoding is requested;
 *  - `crypto.KeyObject` is the exposed class object; instances cannot be created with
 *    `new`.
 *
 *  Example 1 — a secret key and its export forms:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const key = crypto.createSecretKey('0123456789abcdef');
 *
 *  console.log(key.type, key.symmetricKeySize); // secret 16
 *  console.log(key.asymmetricKeyType); // undefined
 *  console.log(key.export().toString()); // 0123456789abcdef
 *  console.log(JSON.stringify(key.export({ format: 'jwk' })));
 *  // {"kty":"oct","k":"MDEyMzQ1Njc4OWFiY2RlZg"}
 *  ```
 *
 *  Example 2 — RSA metadata and deriving the public key:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const { privateKey, publicKey } =
 *      crypto.generateKeyPairSync('rsa', { modulusLength: 1024 });
 *
 *  console.log(publicKey.type, publicKey.asymmetricKeyType); // public rsa
 *  console.log(privateKey.asymmetricKeyDetails.modulusLength); // 1024
 *  console.log(privateKey.asymmetricKeyDetails.publicExponent); // 65537n
 *
 *  // The public key is always derivable from the private key, never the reverse.
 *  console.log(crypto.createPublicKey(privateKey).equals(publicKey)); // true
 *  ```
 *
 *  Example 3 — EC raw/JWK round trip and an encrypted private key:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const { privateKey, publicKey } =
 *      crypto.generateKeyPairSync('ec', { namedCurve: 'prime256v1' });
 *
 *  // The raw form is the bare EC point (65 bytes uncompressed) for public keys.
 *  console.log(publicKey.export({ format: 'raw' }).length); // 65
 *
 *  // A JWK can be imported back into an equal key object.
 *  const jwk = publicKey.export({ format: 'jwk' });
 *  console.log(crypto.createPublicKey({ key: jwk, format: 'jwk' }).equals(publicKey)); // true
 *
 *  // Private keys can be encrypted with a cipher and a passphrase.
 *  const pem = privateKey.export({
 *      format: 'pem',
 *      type: 'pkcs8',
 *      cipher: 'aes-256-cbc',
 *      passphrase: 'secret'
 *  });
 *  console.log(crypto.createPrivateKey({ key: pem, passphrase: 'secret' }).equals(privateKey));
 *  // true
 *  ```
 *
 */
declare class Class_KeyObject extends Class_object {
    /**
     * @description Metadata of an asymmetric key, or undefined for types without any
     *
     *      The property exists only on asymmetric keys; for a secret key, and for key types
     *      such as Ed25519/Ed448/X25519/X448, it is undefined, while a DH key reports an
     *      empty object. The fields depend on the key type:
     *      - RSA: `modulusLength` (bits) and `publicExponent` (a BigInt, 65537n by default);
     *      - RSA-PSS: additionally `hashAlgorithm`, `mgf1HashAlgorithm` and `saltLength`;
     *      - DSA: `modulusLength` and `divisorLength` (the size of q in bits);
     *      - EC and SM2: `namedCurve`, the OpenSSL curve name ('prime256v1', ...).
     *      Node.js reports the same fields and an empty object for Ed25519 where fibjs
     *      returns undefined; none of the values can be used to recover the key.
     *
     *      The returned result contains the following:
     *      ```JavaScript
     *      // fragment: the shape of the returned object; fields depend on the key type
     *      ({
     *          modulusLength: 2048,       // key size in bits (RSA, DSA)
     *          publicExponent: 65537n,    // public exponent as a BigInt (RSA)
     *          hashAlgorithm: 'sha1',     // digest name (RSA-PSS)
     *          mgf1HashAlgorithm: 'sha1', // MGF1 digest name (RSA-PSS)
     *          saltLength: 20,            // minimum salt length in bytes (RSA-PSS)
     *          divisorLength: 224,        // size of q in bits (DSA)
     *          namedCurve: 'prime256v1'   // curve name (EC, SM2)
     *      })
     *      ```
     *
     */
    readonly asymmetricKeyDetails: FIBJS.GeneralObject;

    /**
     * @description The type of an asymmetric key, or undefined for secret and unknown keys
     *
     *      For asymmetric keys, this property indicates the type of the key. Supported key
     *      types are:
     *      - 'rsa' (OID 1.2.840.113549.1.1.1)
     *      - 'rsa-pss' (OID 1.2.840.113549.1.1.10)
     *      - 'dsa' (OID 1.2.840.10040.4.1)
     *      - 'dh' (OID 1.2.840.113549.1.3.1)
     *      - 'ec' (OID 1.2.840.10045.2.1)
     *      - 'sm2' (fibjs extension)
     *      - 'x25519' (OID 1.3.101.110)
     *      - 'x448' (OID 1.3.101.111)
     *      - 'ed25519' (OID 1.3.101.112)
     *      - 'ed448' (OID 1.3.101.113)
     *      - 'Bls12381G1' / 'Bls12381G2' (fibjs BLS extensions)
     *
     *      For unrecognized KeyObject types and symmetric keys, this property is undefined.
     *
     */
    readonly asymmetricKeyType: string;

    /**
     * @description The size of a secret key in bytes, or undefined for asymmetric keys
     *
     *      Only secret keys have a byte-string length: a key built with
     *      `crypto.createSecretKey('0123456789abcdef')` reports 16. Asymmetric keys have no
     *      defined length and report undefined, as in Node.js. Read-only.
     *
     *      @return returns the key size in bytes
     *
     */
    readonly symmetricKeySize: number;

    /**
     * @description The kind of key: 'secret', 'public' or 'private'
     *
     *      Secret keys are symmetric byte strings used by ciphers and HMAC; public and
     *      private keys are the two halves of an asymmetric pair, where the public half can
     *      be derived from the private half. Node.js uses the same three values. Read-only.
     *
     *      @return returns the key type
     *
     */
    readonly type: string;

    /**
     * @description Exports the key's information according to the given options
     *
     *      The options object selects the interchange format; unsupported combinations
     *      throw an Error.
     *
     *      For symmetric keys, the following encoding options can be used:
     *      - format: must be 'buffer' (default, the raw bytes) or 'jwk' (an object such as
     *        `{ kty: 'oct', k: '<base64url>' }`); 'pem' and 'der' are rejected
     *
     *      For public keys, the following encoding options can be used:
     *      - format: must be 'pem' (default, a string), 'der' (a Buffer), 'jwk' or 'raw'
     *        (only EC/SM2/Ed25519/Ed448/X25519/X448)
     *      - type: when format is 'pem' or 'der', one of 'spki' (default) or 'pkcs1' (only
     *        RSA); when format is 'raw', one of 'uncompressed' (default), 'compressed' or
     *        'hybrid'
     *
     *      For private keys, the following encoding options can be used:
     *      - format: must be 'pem' (default, a string), 'der' (a Buffer), 'jwk' or 'raw'
     *        (only EC/SM2/Ed25519/Ed448/X25519/X448)
     *      - type: one of 'pkcs8' (default), 'pkcs1' (only RSA) or 'sec1' (only EC/SM2)
     *      - cipher: if specified, PKCS#5 v2.0 password-based encryption is used,
     *        encrypting the private key with the given cipher and passphrase
     *      - passphrase: <string> | <Buffer> the password used for encryption, required
     *        when cipher is set (a cipher is also required when a passphrase is set)
     *
     *      When the JWK encoding format is selected, all other encoding options are ignored.
     *      Combinations of the cipher and format options can encrypt PKCS#1, SEC1 and
     *      PKCS#8 keys. PKCS#8 can encrypt any key algorithm (RSA, EC, DH) in both PEM and
     *      DER form; PKCS#1 and SEC1 support encryption only when PEM is used. For maximum
     *      compatibility use PKCS#8 for encrypted private keys, because PKCS#8 defines its
     *      own encryption mechanism (RFC 5208; RFC 1421 covers PKCS#1 and SEC1). PEM
     *      returns a string, DER and raw return a Buffer, JWK returns an object. fibjs
     *      applies the classic Node defaults (spki/pkcs8) and also accepts `export()` with
     *      no arguments; Node.js v25 requires an explicit type for asymmetric PEM/DER
     *      export.
     *
     *      Example: three representations of the same RSA public key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { publicKey } = crypto.generateKeyPairSync('rsa', { modulusLength: 1024 });
     *
     *      const pem = publicKey.export({ format: 'pem', type: 'spki' });
     *      console.log(pem.startsWith('-----BEGIN PUBLIC KEY-----')); // true
     *
     *      const der = publicKey.export({ format: 'der', type: 'spki' });
     *      console.log(Buffer.isBuffer(der)); // true
     *
     *      const jwk = publicKey.export({ format: 'jwk' });
     *      console.log(jwk.kty, Object.keys(jwk).join(',')); // RSA e,n
     *      ```
     *
     *      @param options the options for exporting the key
     *      @return returns the key's information
     *
     */
    export(options?: FIBJS.GeneralObject): any;

    /**
     * @description Compares whether two KeyObject objects are equal
     *
     *      Two secret keys are equal when their bytes match; two asymmetric keys are equal
     *      when their type and key material match, so a public key equals the public key
     *      derived from the matching private key. The comparison is not constant time.
     *      Calling it with a non-KeyObject throws a TypeError, as in Node.js.
     *
     *      Example: a public key equals the one derived from its private key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { privateKey, publicKey } =
     *          crypto.generateKeyPairSync('ec', { namedCurve: 'prime256v1' });
     *
     *      console.log(publicKey.equals(crypto.createPublicKey(privateKey))); // true
     *      console.log(publicKey.equals(privateKey)); // false
     *      console.log(crypto.createSecretKey('a').equals(crypto.createSecretKey('a'))); // true
     *      ```
     *
     *      @param otherKey the KeyObject to compare
     *      @return returns true if the two KeyObject objects are equal, false otherwise
     *
     */
    equals(otherKey: Class_KeyObject): boolean;

}

