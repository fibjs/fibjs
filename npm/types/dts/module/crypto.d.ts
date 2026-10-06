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
 * @description The `crypto` module is the built-in cryptography module of fibjs. It provides
 *  message digests and MACs, symmetric and asymmetric ciphers, key derivation and agreement,
 *  digital signatures, X.509 certificates and cryptographically secure random numbers. Load
 *  it with `require('crypto')` before use
 *
 *  Capabilities:
 *
 *  - **Digests and MACs**: `createHash` and the one-shot `hash` compute message digests;
 *    `createHmac` computes keyed digests (HMAC);
 *  - **Key derivation and agreement**: `pbkdf2`, `scrypt` and `hkdf` derive keys from
 *    secrets; `createECDH` and `diffieHellman` compute shared secrets;
 *  - **Symmetric ciphers**: `createCipheriv` and `createDecipheriv` run a cipher with an
 *    explicit key and IV; the legacy `createCipher` and `createDecipher` derive both from
 *    a password;
 *  - **AEAD**: GCM, CCM, OCB and ChaCha20-Poly1305 authenticate the ciphertext with a tag
 *    carried by `getAuthTag`/`setAuthTag` and `setAAD`;
 *  - **Asymmetric keys**: `createPrivateKey`, `createPublicKey` and `createSecretKey`
 *    import or wrap key material into `KeyObject` instances; `generateKeyPair` creates a
 *    new pair;
 *  - **Asymmetric ciphers**: `publicEncrypt`/`privateDecrypt` and
 *    `privateEncrypt`/`publicDecrypt` for RSA key transport;
 *  - **Signatures**: `sign` and `verify` in one shot; `createSign` and `createVerify`
 *    through a streaming pipeline;
 *  - **Secure randomness**: `randomBytes`, `randomFill`, `getRandomValues` and
 *    `randomUUID`;
 *  - **Certificates and BBS**: `X509Certificate`, `createCertificateRequest`, and the BBS
 *    methods `bbsSign`, `bbsVerify`, `proofGen` and `proofVerify`;
 *  - **Discovery**: `getHashes`, `getCiphers`, `getCurves`, `getCipherInfo` and
 *    `timingSafeEqual`.
 *
 *  Concepts:
 *  - **Digest vs MAC**: a digest (hash) maps data to a fixed-length fingerprint. It is
 *    not keyed, so anyone can recompute it: it proves integrity but not origin. An HMAC
 *    mixes a secret key into the digest and proves both integrity and authenticity. Use
 *    a MAC (or a signature) whenever an attacker could otherwise recompute the value.
 *  - **Password-based KDFs**: `pbkdf2` stretches a password using a salt and a public
 *    iteration count; `scrypt` additionally takes N (CPU/memory cost, a power of 2), r
 *    (block size) and p (parallelism) and is memory-hard. Both are intentionally slow,
 *    so raise their cost as hardware improves, and store the salt and the parameters
 *    with the derived key. `hkdf` is not for passwords: it expands an already
 *    high-entropy secret into several purpose-bound keys.
 *  - **Symmetric ciphers**: one secret key transforms the data. Block ciphers (AES, SM4)
 *    need a mode (CBC, CTR, ...) and a unique, unpredictable IV per encryption. Reusing
 *    an IV with the same key destroys the security guarantees, especially in CTR and
 *    GCM. fibjs uses OpenSSL names such as 'aes-256-cbc' and 'aes-256-gcm'; `getCiphers`
 *    lists them and `getCipherInfo` reports key and IV lengths.
 *  - **Padding**: block modes require the plaintext to be a multiple of the block size.
 *    fibjs pads it by default (PKCS#7) and removes the padding on decryption. Call
 *    `setAutoPadding(false)` when the data is already aligned or the protocol defines
 *    its own padding, and then feed exactly what was encrypted.
 *  - **AEAD and the auth tag**: authenticated modes (GCM, CCM, OCB, ChaCha20-Poly1305)
 *    produce a tag over the ciphertext and the additional authenticated data (AAD).
 *    After encryption call `getAuthTag()`; before decrypting call `setAuthTag(tag)`
 *    and, on both sides, `setAAD(aad)` when AAD is used. The tag is 16 bytes unless
 *    `authTagLength` selects another valid length, and `final()` throws when the tag
 *    does not match - treat that as tampering.
 *  - **Key material**: keys are imported from PEM/DER strings or Buffers, or built from
 *    an options object (`key`, `format`, `type`, `passphrase`), and are exposed as
 *    `KeyObject` instances. `KeyObject.export` returns PEM, DER or JWK forms; a public
 *    key can always be derived from a private key, never the other way round.
 *  - **RSA vs EC vs one-shot keys**: RSA keys are sized in bits (use 2048 or more for
 *    anything valuable) and support both encryption and PKCS#1/PSS signatures. EC keys
 *    name a curve (prime256v1, secp384r1, secp256k1) and are used for signatures and key
 *    agreement. Ed25519/Ed448 and X25519/X448 work in one shot: pass a null algorithm
 *    and keep the whole message in memory. The streaming `createSign`/`createVerify`
 *    pipeline cannot be used with them.
 *  - **Signatures vs MACs**: an HMAC is fast and symmetric, so both parties hold the
 *    same key. A digital signature uses a private key to sign and the matching public
 *    key to verify, and also proves who signed. Prefer signatures when the verifier
 *    must not be able to forge.
 *  - **Secure randomness**: `randomBytes`, `randomFill`, `getRandomValues` and
 *    `randomUUID` draw from the operating system CSPRNG. Never use `Math.random()` for
 *    keys, IVs, salts, tokens or anything security-related: losing the uniqueness of an
 *    IV or a salt can break a cipher or a KDF.
 *  - **Call forms**: every member declared `async` follows the fibjs convention: the
 *    bare call blocks the calling fiber and returns the result, a trailing callback
 *    receives `(err, result)`, and the generated aliases add an explicit blocking
 *    `...Sync` form and a `...Async` form that returns a Promise. The `crypto.promises`
 *    namespace maps every async member to its promise-returning form (sync members are
 *    unchanged), so `crypto.promises.pbkdf2(...)` is `crypto.pbkdf2Async(...)`. See the
 *    coroutine module for the fiber model.
 *
 *  Import:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *  ```
 *
 *  Example 1 - digest and HMAC round trip:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  // A digest is a fixed-length fingerprint; the same bytes always give the same value.
 *  console.log(crypto.hash('sha256', 'abc'));
 *  // ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
 *
 *  // An HMAC mixes in a secret key, so only key holders can recompute it.
 *  const tag = crypto.createHmac('sha256', 'secret key')
 *      .update('message to authenticate')
 *      .digest('hex');
 *  console.log(tag);
 *  // 5628bb29bb02a78cee9910cb32419e9674d6d954d8112b76018623df1a528662
 *  ```
 *
 *  Example 2 - derive keys from a password (PBKDF2 and scrypt):
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  // RFC 6070 test vector: one iteration keeps the example deterministic.
 *  console.log(crypto.pbkdf2Sync('password', 'salt', 1, 20, 'sha1').toString('hex'));
 *  // 0c60c80f961f0e71f3a9b524af6012062fe037a6
 *
 *  // scrypt is memory-hard; in production use the current recommended parameters.
 *  const salt = crypto.randomBytes(16);
 *  const key = crypto.scryptSync('correct horse battery staple', salt, 32);
 *  console.log(key.length); // 32
 *
 *  // The same password, salt and parameters always derive the same key.
 *  console.log(key.equals(crypto.scryptSync('correct horse battery staple', salt, 32))); // true
 *  ```
 *
 *  Example 3 - AES-256-GCM encrypt/decrypt with AAD and auth tag:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const key = crypto.randomBytes(32); // keep the key secret
 *  const iv = crypto.randomBytes(12);  // never reuse an IV with the same key
 *
 *  const cipher = crypto.createCipheriv('aes-256-gcm', key, iv);
 *  cipher.setAAD(Buffer.from('v1'));
 *  const ciphertext = Buffer.concat([
 *      cipher.update('secret message', 'utf8'),
 *      cipher.final()
 *  ]);
 *  const tag = cipher.getAuthTag();
 *
 *  const decipher = crypto.createDecipheriv('aes-256-gcm', key, iv);
 *  decipher.setAAD(Buffer.from('v1'));
 *  decipher.setAuthTag(tag);
 *  console.log(Buffer.concat([decipher.update(ciphertext), decipher.final()]).toString());
 *  // secret message
 *  ```
 *
 *  Notes: there is no global `webcrypto` or `subtle`; reach the Web Crypto API through
 *  `crypto.webcrypto` and `crypto.subtle`. Node.js APIs without a fibjs counterpart
 *  include the `getDiffieHellman`/`createDiffieHellman` family, `checkPrime` and
 *  `generatePrime`, and the callback form of `randomBytes`; `getRandomValues` also
 *  accepts floating-point TypedArrays instead of rejecting them. In return, fibjs adds
 *  the blocking and Sync/Async call forms, the `crypto.promises` namespace, SM2 and
 *  Bls12381 key types, BBS signatures and `createCertificateRequest`.
 *
 */
declare module 'crypto' {
    /**
     * @description The module's constants object, see crypto_constants
     *
     *      It carries the RSA padding values (RSA_PKCS1_PADDING, RSA_PKCS1_OAEP_PADDING,
     *      RSA_PKCS1_PSS_PADDING, RSA_PSS_SALTLEN_*), the TLS version constants and the
     *      OpenSSL error strings used by the other members
     */
    const constants: typeof import ('crypto_constants');

    /**
     * @description The KeyObject class, see KeyObject
     *
     *      `createPrivateKey`, `createPublicKey`, `createSecretKey` and `generateKeyPair`
     *      produce KeyObject instances. They are accepted wherever a key is expected, such
     *      as createHmac, createCipheriv, sign, verify and diffieHellman, so raw key bytes
     *      need not stay exposed in the program
     */
    const KeyObject: typeof Class_KeyObject;

    /**
     * @description The X509Certificate class, see X509Certificate
     *
     *      Instances wrap a parsed certificate and are obtained with
     *      `new crypto.X509Certificate(...)`, either from PEM/DER data or from a list of
     *      certificates that form a chain
     */
    const X509Certificate: typeof Class_X509Certificate;

    /**
     * @description Lists the digest algorithm names accepted by createHash, createHmac, hash,
     *      pbkdf2, hkdf, sign and verify
     *
     *      The list comes from the linked OpenSSL build, so it depends on the build and the
     *      provider configuration. Names are matched case-insensitively and a '-' may be used
     *      in place of '_'. A few aliases are accepted in addition to the OpenSSL names:
     *      'dss1' maps to sha1, 'sha3_256'/'sha3_384'/'sha3_512' map to the corresponding
     *      'sha3-*' names, and 'blake2s'/'blake2b' map to 'blake2s256'/'blake2b512'.
     *      Extendable-output functions such as shake128 are accepted by createHash and hash
     *      (the digest has the default length) but rejected by createHmac.
     *      @return returns the array of supported hash algorithm names
     *
     */
    function getHashes(): string[];

    /**
     * @description Creates an ECDH key-agreement object for the given ECC curve name
     *
     *      The returned ECDH owns one side of the exchange: call `generateKeys` to create a
     *      fresh key pair, send the public key to the peer, then call `computeSecret` with
     *      the peer's public key to derive the shared secret. Both sides must use the same
     *      curve; see getCurves for the accepted names (for example 'prime256v1',
     *      'secp384r1' or 'secp256k1'). For one-shot agreement between keys generated with
     *      generateKeyPair, use diffieHellman instead.
     *
     *      Example: two parties derive the same secret over prime256v1:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const alice = crypto.createECDH('prime256v1');
     *      const bob = crypto.createECDH('prime256v1');
     *      const alicePublic = alice.generateKeys();
     *      const bobPublic = bob.generateKeys();
     *
     *      const aliceSecret = alice.computeSecret(bobPublic);
     *      const bobSecret = bob.computeSecret(alicePublic);
     *      console.log(aliceSecret.equals(bobSecret), aliceSecret.length); // true 32
     *      ```
     *
     *      Throws when the curve name is empty or unknown.
     *      @param curve the ECC curve name to use
     *      @return returns the ECDH object
     *
     */
    function createECDH(curve: string): Class_ECDH;

    /**
     * @description Creates a streaming digest object for the given algorithm name
     *
     *      Feed the data with `update` and finish with `digest`; `digest` may be called only
     *      once and no `update` is allowed afterwards. The algorithm names come from
     *      getHashes. Unknown names throw. See hash for the one-shot form and createHmac for
     *      keyed digests.
     *
     *      Example: several updates produce the same digest as one:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const digest = crypto.createHash('sha256').update('abc').update('def').digest();
     *      console.log(digest.toString('base64'));
     *      // vvV+x/U6bUC+tkCngKY5yDvCmsipgW8fxsXG3Nk8RyE=
     *
     *      console.log(crypto.createHash('sha256').size); // 32 (bytes)
     *      ```
     *
     *      @param algo the digest algorithm name to use
     *      @return returns the message digest object
     *
     */
    function createHash(algo: string): Class_Digest;

    /**
     * @description Creates a streaming keyed-digest (HMAC) object
     *
     *      An HMAC binds the message to a secret key, so only holders of the key can
     *      recompute it; it is the usual primitive for authenticating messages between two
     *      parties that share a secret. The key may be a Buffer, a string decoded as utf8 or
     *      a secret KeyObject; passing a non-secret KeyObject throws, as do XOF algorithms.
     *      Compare the resulting tag with timingSafeEqual, not with ==.
     *
     *      Example: a secret KeyObject and the same key as a Buffer give the same tag:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const key = crypto.createSecretKey('secret key');
     *      const tag = crypto.createHmac('sha256', key)
     *          .update('message to authenticate')
     *          .digest('hex');
     *      console.log(tag);
     *      // 5628bb29bb02a78cee9910cb32419e9674d6d954d8112b76018623df1a528662
     *
     *      console.log(crypto.createHmac('sha256', Buffer.from('secret key'))
     *          .update('message to authenticate').digest('hex') === tag); // true
     *      ```
     *
     *      @param algo the digest algorithm name to use
     *      @param key the secret key to use
     *      @return returns the message digest object
     *
     */
    function createHmac(algo: string, key: Class_Buffer | Class_KeyObject | string): Class_Digest;

    /**
     * @description Lists the symmetric cipher names accepted by the createCipher family and
     *      described by getCipherInfo
     *
     *      The list comes from the linked OpenSSL build; names are OpenSSL spellings such as
     *      'aes-256-cbc', 'aes-256-gcm', 'chacha20-poly1305' and 'sm4-cbc'. Authenticated
     *      modes are GCM, CCM, OCB and ChaCha20-Poly1305.
     *      @return returns the array of supported cipher names
     *
     */
    function getCiphers(): string[];

    /**
     * @description Looks up a cipher by name or numeric NID and reports its parameters
     *
     *      `nameOrNid` is a cipher name or the OpenSSL NID returned in the `nid` field. When
     *      `options.keyLength` or `options.ivLength` is given and does not match, the call
     *      returns undefined instead of throwing. CCM accepts IV lengths from 7 to 13 bytes
     *      and OCB from 1 to 15; other modes report a fixed IV length.
     *
     *      The returned object has the following properties:
     *      - name: the canonical OpenSSL name, lowercased (for example 'id-aes256-gcm');
     *      - nid: the OpenSSL numeric identifier, accepted by this function;
     *      - blockSize: the block size in bytes (1 for stream ciphers);
     *      - ivLength: the IV size in bytes;
     *      - keyLength: the key size in bytes;
     *      - mode: 'cbc', 'ctr', 'gcm', 'ccm', 'ocb', 'xts', 'wrap', 'siv', 'ecb', 'cfb',
     *        'ofb' or 'stream'.
     *      @param nameOrNid the cipher name or NID to query
     *      @param options the optional keyLength and ivLength filters
     *      @return returns the cipher information, or undefined when it does not match
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
     * @description Creates an encryption object, deriving the key and IV from a password
     *
     *      Legacy form: the key and IV are derived from `key` with OpenSSL's EVP_BytesToKey
     *      (MD5, one iteration). That derivation is weak and not compatible with proper
     *      password KDFs, so prefer createCipheriv with a key from pbkdf2/scrypt and a
     *      random IV. The string form of `key` is decoded as utf8; `options` accepts the
     *      same entries as createCipheriv.
     *      @param algorithm the cipher algorithm name
     *      @param key the password to derive the key and IV from
     *      @param options the cipher options
     *      @return returns the encryption object
     *
     */
    function createCipher(algorithm: string, key: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates an encryption object with an explicit key and IV
     *
     *      Use a distinct, unpredictable IV for every encryption under the same key; a
     *      common pattern is to generate it with randomBytes and store it (or prefix it)
     *      together with the ciphertext. For block ciphers the IV must be exactly the
     *      `ivLength` reported by getCipherInfo; authenticated modes also accept
     *      `options.authTagLength` (GCM: 4, 8 or 12-16 bytes, default 16). The key may be a
     *      secret KeyObject, a Buffer or a string decoded as utf8.
     *
     *      Example: AES-256-CBC round trip:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const key = crypto.randomBytes(32);
     *      const iv = crypto.randomBytes(16); // unique per message
     *
     *      const cipher = crypto.createCipheriv('aes-256-cbc', key, iv);
     *      const ciphertext = Buffer.concat([
     *          cipher.update('hello '), cipher.update('world'), cipher.final()
     *      ]);
     *
     *      const decipher = crypto.createDecipheriv('aes-256-cbc', key, iv);
     *      console.log(Buffer.concat([decipher.update(ciphertext), decipher.final()]).toString());
     *      // hello world
     *      ```
     *
     *      See the module example for the authenticated GCM form.
     *      @param algorithm the cipher algorithm name
     *      @param key the encryption key
     *      @param iv the initialization vector
     *      @param options the cipher options
     *      @return returns the encryption object
     *
     */
    function createCipheriv(algorithm: string, key: Class_Buffer | Class_KeyObject | string, iv: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a decryption object, deriving the key and IV from a password
     *
     *      Legacy counterpart of createCipher: the same password and derivation must be used
     *      to decrypt. Prefer createDecipheriv with an explicit key and IV.
     *      @param algorithm the cipher algorithm name
     *      @param key the password to derive the key and IV from
     *      @param options the cipher options
     *      @return returns the decryption object
     *
     */
    function createDecipher(algorithm: string, key: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Creates a decryption object with an explicit key and IV
     *
     *      The algorithm, key and IV must match those used to encrypt; for AEAD modes also
     *      call `setAAD` with the same AAD and `setAuthTag` with the tag from `getAuthTag`
     *      before updating, or `final` fails. In authenticated modes nothing is verified
     *      until `final` succeeds; a tag mismatch (tampering) makes it throw.
     *      @param algorithm the cipher algorithm name
     *      @param key the decryption key
     *      @param iv the initialization vector
     *      @param options the cipher options
     *      @return returns the decryption object
     *
     */
    function createDecipheriv(algorithm: string, key: Class_Buffer | Class_KeyObject | string, iv: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Lists the ECC curve names supported by createECDH and accepted by
     *      `generateKeyPair('ec', ...)` and `generateKeyPair('sm2')`
     *
     *      Typical names are 'prime256v1' (NIST P-256), 'secp384r1', 'secp521r1' and
     *      'secp256k1'. The list depends on the linked OpenSSL build.
     *      @return returns the array of supported curve names
     *
     */
    function getCurves(): string[];

    /**
     * @description Creates a KeyObject holding an asymmetric private key
     *
     *      The key may be a PEM string or Buffer, an existing private KeyObject, or an
     *      options object carrying the key material and its format:
     *      - key: the key material itself (PEM/DER string or Buffer, or a JWK object);
     *      - format: 'pem' (default) or 'der';
     *      - type: the key type when the input is not self-describing, for example
     *        'pkcs1', 'pkcs8' or 'sec1';
     *      - passphrase: the passphrase of an encrypted key;
     *      - namedCurve: the curve for a raw EC key.
     *
     *      The matching public key can be derived with createPublicKey and the key exported
     *      again with `KeyObject.export`. fibjs also accepts `toX25519` to convert an
     *      Ed25519/Ed448 key to its X25519 form (an extension over Node.js).
     *      @param key the private key material or its options
     *      @return returns the private key object
     *
     */
    function createPrivateKey(key: Class_Buffer | FIBJS.GeneralObject | string): Class_KeyObject;

    /**
     * @description Creates a KeyObject holding an asymmetric public key
     *
     *      Accepts a public key in PEM/DER form (string or Buffer), another KeyObject, or
     *      an options object with the same entries as createPrivateKey (plus `toX25519`,
     *      an extension over Node.js). A private KeyObject is accepted as well: the
     *      matching public key is derived from it.
     *      @param key the public or private key material, or its options
     *      @return returns the public key object
     *
     */
    function createPublicKey(key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_KeyObject;

    /**
     * @description Creates a streaming signing object for the given digest algorithm
     *
     *      Update it with the data, then call `sign` once with the private key; the signing
     *      parameters (such as `dsaEncoding` and the RSA padding) go into the key argument
     *      of `sign`. This `options` parameter is reserved and currently unused. The
     *      streaming form is not available for one-shot algorithms (Ed25519 and Ed448) -
     *      use sign for those.
     *
     *      Example: a two-part update signed with an EC key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { privateKey, publicKey } = crypto.generateKeyPairSync('ec', {
     *          namedCurve: 'prime256v1'
     *      });
     *
     *      const signer = crypto.createSign('sha256');
     *      signer.update('signed ');
     *      signer.update('in one message');
     *      const signature = signer.sign(privateKey);
     *
     *      const verifier = crypto.createVerify('sha256');
     *      verifier.update('signed in one message');
     *      console.log(verifier.verify(publicKey, signature)); // true
     *      ```
     *
     *      @param algorithm the digest algorithm name
     *      @param options the signing options
     *      @return returns the signing object
     *
     */
    function createSign(algorithm: string, options?: FIBJS.GeneralObject): Class_Sign;

    /**
     * @description Creates a streaming verification object for the given digest algorithm
     *
     *      The counterpart of createSign: update it with the data and call `verify` with the
     *      public key and the signature. `verify` returns false for a wrong signature or key
     *      and throws only for malformed input. The options of the verification (such as
     *      `dsaEncoding`, which must match the signer) go into the key argument of `verify`;
     *      this `options` parameter is reserved and currently unused.
     *      @param algorithm the digest algorithm name
     *      @param options the verifying options, reserved and unused
     *      @return returns the verification object
     *
     */
    function createVerify(algorithm: string, options?: FIBJS.GeneralObject): Class_Verify;

    /**
     * @description Creates a KeyObject holding a symmetric (secret) key
     *
     *      Wraps raw key bytes so they can be passed to createHmac, createCipheriv and the
     *      other members that accept a KeyObject without keeping the bytes in a Buffer. The
     *      string form is decoded with `encoding` (default 'utf8'); the Buffer form is used
     *      as-is and the encoding is ignored. `type` is 'secret' and `symmetricKeySize`
     *      reports the length.
     *
     *      Example: wrap a raw key and confirm the round trip:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const key = crypto.createSecretKey('0123456789abcdef');
     *      console.log(key.type, key.symmetricKeySize); // secret 16
     *      console.log(key.export().toString());        // 0123456789abcdef
     *      ```
     *
     *      @param key the key material
     *      @param encoding the encoding of a string key
     *      @return returns the secret key object
     *
     */
    function createSecretKey(key: Class_Buffer | string, encoding?: string): Class_KeyObject;

    /**
     * @description Creates an X.509 certificate request (CSR) from key material or parses an
     *      existing one
     *
     *      When `csr` is an options object, its entries are passed to createPrivateKey
     *      (`key`, `passphrase`, ...) and the following entries build the request:
     *      - subject: the subject as key/value pairs, for example
     *        `{ C: 'CN', O: 'example', CN: 'host.example' }`;
     *      - hashAlgorithm: the signature digest, default 'sha256', or 'sm3' for SM2 keys.
     *
     *      When `csr` is a PEM/DER string or Buffer, the request is parsed instead of
     *      created. The result exposes `subject`, `publicKey`, `subjectAltName`,
     *      `infoAccess`, `raw` and `pem`, and `checkPrivateKey` proves that a private key
     *      matches the request. This member has no Node.js counterpart.
     *
     *      Example: build a CSR and confirm its public key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { privateKey } = crypto.generateKeyPairSync('ec', { namedCurve: 'prime256v1' });
     *      const req = crypto.createCertificateRequest({
     *          key: privateKey,
     *          subject: { C: 'CN', O: 'fibjs', CN: 'example.com' }
     *      });
     *
     *      console.log(req.checkPrivateKey(privateKey)); // true
     *      console.log(req.publicKey.asymmetricKeyType);  // ec
     *      console.log(String(req.pem).split('\n')[0]);
     *      // -----BEGIN CERTIFICATE REQUEST-----
     *      ```
     *
     *      @param csr the PEM/DER request or the options used to create one
     *      @return returns the certificate request object
     *
     */
    function createCertificateRequest(csr: Class_Buffer | FIBJS.GeneralObject | string): Class_X509CertificateRequest;

    /**
     * @description Computes the shared secret between a private key and a peer's public key
     *
     *      The one-shot companion of createECDH: generate the pairs with generateKeyPair,
     *      exchange the public keys, then call this with both KeyObjects. `privateKey` must
     *      be a private KeyObject and `publicKey` a public one, and both must have the same
     *      key type (for example X25519, X448 or EC); otherwise it throws. The result is
     *      the raw shared secret as a Buffer.
     *
     *      options supports the following options:
     *      - privateKey: the local private KeyObject;
     *      - publicKey: the peer's public KeyObject.
     *      @param options the key pair to compute the secret from
     *      @return returns the shared secret
     *
     */
    function diffieHellman(options: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Computes a one-shot message digest of the data
     *
     *      Convenient and faster than createHash for small inputs (roughly up to 5 MB);
     *      createHash is the better choice for large or streamed data because it does not
     *      need the whole input in memory. The data may be a Buffer or a string decoded as
     *      utf8. The output encoding may be any Buffer encoding, including 'buffer' to get
     *      the raw bytes.
     *
     *      Example:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      console.log(crypto.hash('sha256', 'abc'));
     *      // ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
     *
     *      console.log(crypto.hash('sha256', 'abc', 'base64'));
     *      // ungWv48Bz+pBQUDeXa4iI7ADYaOWF3qctBD/YfIAFa0=
     *
     *      console.log(Buffer.isBuffer(crypto.hash('sha256', 'abc', 'buffer'))); // true
     *      ```
     *
     *      @param algorithm the digest algorithm name
     *      @param data the data to hash
     *      @param outputEncoding the encoding of the result
     *      @return returns the digest
     *
     */
    function hash(algorithm: string, data: Class_Buffer | string, outputEncoding?: string): any;

    /**
     * @description Generates `size` cryptographically secure random bytes
     *
     *      The bytes come from the CSPRNG of the operating system (OpenSSL RAND_bytes), so
     *      they are suitable for keys, IVs, salts and tokens; never use Math.random() for
     *      those. The default size is 16 and the value must be at least 1; an out-of-range
     *      size throws RangeError. Unlike Node.js there is no callback form - call
     *      randomFillAsync (or randomFill with a callback) when a promise is preferred.
     *
     *      Example:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      console.log(crypto.randomBytes().length);   // 16 (default)
     *      console.log(crypto.randomBytes(32).length); // 32
     *
     *      // Two independent draws are practically never equal.
     *      console.log(crypto.randomBytes(8).equals(crypto.randomBytes(8))); // false
     *      ```
     *
     *      @param size the number of bytes to generate
     *      @return returns the random bytes
     *
     */
    function randomBytes(size?: number): Class_Buffer;

    /**
     * @description Fills a Buffer with cryptographically secure random bytes
     *
     *      Writes `size` bytes starting at `offset`; by default the whole range from
     *      `offset` to the end of the buffer is filled. A negative `size` also means "to the
     *      end". The buffer is modified in place; note that fibjs returns a Buffer object
     *      distinct from the argument (Node.js returns the same object). A string argument
     *      is decoded as utf8 and a new filled Buffer is returned. An offset outside the
     *      buffer or `offset + size` beyond its length throws RangeError. This member
     *      follows the async call forms (bare, callback, `randomFillSync`,
     *      `randomFillAsync`).
     *      @param buffer the buffer to fill
     *      @param offset the starting offset
     *      @param size the number of bytes to write, -1 for the rest of the buffer
     *      @return returns the filled buffer
     *
     */
    function randomFill(buffer: Class_Buffer | string, offset?: number, size?: number): Class_Buffer;

    function randomFill(buffer: Class_Buffer | string, offset?: number, size?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Fills a Buffer with cryptographically secure random bytes
     *
     *      Writes `size` bytes starting at `offset`; by default the whole range from
     *      `offset` to the end of the buffer is filled. A negative `size` also means "to the
     *      end". The buffer is modified in place; note that fibjs returns a Buffer object
     *      distinct from the argument (Node.js returns the same object). A string argument
     *      is decoded as utf8 and a new filled Buffer is returned. An offset outside the
     *      buffer or `offset + size` beyond its length throws RangeError. This member
     *      follows the async call forms (bare, callback, `randomFillSync`,
     *      `randomFillAsync`).
     *      @param buffer the buffer to fill
     *      @param offset the starting offset
     *      @param size the number of bytes to write, -1 for the rest of the buffer
     *      @return returns the filled buffer
     *
     */
    function randomFillSync(buffer: Class_Buffer | string, offset?: number, size?: number): Class_Buffer;

    /**
     * @description Fills a Buffer with cryptographically secure random bytes
     *
     *      Writes `size` bytes starting at `offset`; by default the whole range from
     *      `offset` to the end of the buffer is filled. A negative `size` also means "to the
     *      end". The buffer is modified in place; note that fibjs returns a Buffer object
     *      distinct from the argument (Node.js returns the same object). A string argument
     *      is decoded as utf8 and a new filled Buffer is returned. An offset outside the
     *      buffer or `offset + size` beyond its length throws RangeError. This member
     *      follows the async call forms (bare, callback, `randomFillSync`,
     *      `randomFillAsync`).
     *      @param buffer the buffer to fill
     *      @param offset the starting offset
     *      @param size the number of bytes to write, -1 for the rest of the buffer
     *      @return returns the filled buffer
     *
     */
    function randomFillAsync(buffer: Class_Buffer | string, offset?: number, size?: number): Promise<Class_Buffer>;

    /**
     * @description Fills a TypedArray with cryptographically secure random bytes
     *
     *      The whole array is filled byte-wise and the same array is returned (the array is
     *      modified in place). At most 65536 bytes may be requested in one call; a larger
     *      array throws RangeError. This is the Web Crypto entry point, also available as
     *      `crypto.webcrypto.getRandomValues`. Unlike Node.js, fibjs also accepts
     *      floating-point TypedArrays (they still receive random bytes) and rejects a
     *      DataView with a type error.
     *      @param data the TypedArray to fill
     *      @return returns the filled TypedArray
     *
     */
    function getRandomValues(data: TypedArray): TypedArray;

    /**
     * @description Generates a random RFC 4122 version 4 UUID
     *
     *      The value is built from 16 random bytes with the version and variant bits set,
     *      so it matches 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx' with y in [89ab]. The same
     *      function is available as `crypto.webcrypto.randomUUID`. `options` is accepted
     *      for Node.js compatibility but ignored, including `disableEntropyCache`.
     *      @param options reserved; not used
     *      @return returns the UUID string
     *
     */
    function randomUUID(options?: FIBJS.GeneralObject): string;

    /**
     * @description Generates a new asymmetric key pair of the given type
     *
     *      The supported types are 'rsa', 'rsa-pss', 'dsa', 'ec', 'ed25519', 'ed448',
     *      'x25519', 'x448', 'sm2', 'Bls12381G1' and 'Bls12381G2' (the last three are fibjs
     *      extensions). By default the result carries `publicKey` and `privateKey`
     *      KeyObjects; when `publicKeyEncoding` and/or `privateKeyEncoding` is given, the
     *      requested exported form is returned instead.
     *
     *      options supports the following entries:
     *      - modulusLength: key size in bits (RSA, DSA);
     *      - publicExponent: RSA public exponent, default 0x10001;
     *      - hashAlgorithm, mgf1HashAlgorithm, saltLength: RSA-PSS parameters;
     *      - divisorLength: size of q in bits (DSA);
     *      - namedCurve: the curve name (EC), see getCurves;
     *      - paramEncoding: 'named' (default) or 'explicit' (EC);
     *      - prime, primeLength, generator, groupName: Diffie-Hellman parameters;
     *      - publicKeyEncoding, privateKeyEncoding: export options in the form accepted by
     *        `KeyObject.export`, for example `{ type: 'spki', format: 'pem' }`.
     *
     *      Example: an EC pair and its exported public key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { publicKey, privateKey } = crypto.generateKeyPairSync('ec', {
     *          namedCurve: 'prime256v1'
     *      });
     *      console.log(privateKey.asymmetricKeyType); // ec
     *
     *      const spki = publicKey.export({ type: 'spki', format: 'pem' });
     *      console.log(String(spki).split('\n')[0]);
     *      // -----BEGIN PUBLIC KEY-----
     *      ```
     *
     *      @param type the key type to generate
     *      @param options the key generation and encoding options
     *      @return returns the generated key pair
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
     * @description Generates a new asymmetric key pair of the given type
     *
     *      The supported types are 'rsa', 'rsa-pss', 'dsa', 'ec', 'ed25519', 'ed448',
     *      'x25519', 'x448', 'sm2', 'Bls12381G1' and 'Bls12381G2' (the last three are fibjs
     *      extensions). By default the result carries `publicKey` and `privateKey`
     *      KeyObjects; when `publicKeyEncoding` and/or `privateKeyEncoding` is given, the
     *      requested exported form is returned instead.
     *
     *      options supports the following entries:
     *      - modulusLength: key size in bits (RSA, DSA);
     *      - publicExponent: RSA public exponent, default 0x10001;
     *      - hashAlgorithm, mgf1HashAlgorithm, saltLength: RSA-PSS parameters;
     *      - divisorLength: size of q in bits (DSA);
     *      - namedCurve: the curve name (EC), see getCurves;
     *      - paramEncoding: 'named' (default) or 'explicit' (EC);
     *      - prime, primeLength, generator, groupName: Diffie-Hellman parameters;
     *      - publicKeyEncoding, privateKeyEncoding: export options in the form accepted by
     *        `KeyObject.export`, for example `{ type: 'spki', format: 'pem' }`.
     *
     *      Example: an EC pair and its exported public key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { publicKey, privateKey } = crypto.generateKeyPairSync('ec', {
     *          namedCurve: 'prime256v1'
     *      });
     *      console.log(privateKey.asymmetricKeyType); // ec
     *
     *      const spki = publicKey.export({ type: 'spki', format: 'pem' });
     *      console.log(String(spki).split('\n')[0]);
     *      // -----BEGIN PUBLIC KEY-----
     *      ```
     *
     *      @param type the key type to generate
     *      @param options the key generation and encoding options
     *      @return returns the generated key pair
     *
     */
    function generateKeyPairSync(type: string, options?: FIBJS.GeneralObject): {
        publicKey: any;
        privateKey: any;
    };

    /**
     * @description Generates a new asymmetric key pair of the given type
     *
     *      The supported types are 'rsa', 'rsa-pss', 'dsa', 'ec', 'ed25519', 'ed448',
     *      'x25519', 'x448', 'sm2', 'Bls12381G1' and 'Bls12381G2' (the last three are fibjs
     *      extensions). By default the result carries `publicKey` and `privateKey`
     *      KeyObjects; when `publicKeyEncoding` and/or `privateKeyEncoding` is given, the
     *      requested exported form is returned instead.
     *
     *      options supports the following entries:
     *      - modulusLength: key size in bits (RSA, DSA);
     *      - publicExponent: RSA public exponent, default 0x10001;
     *      - hashAlgorithm, mgf1HashAlgorithm, saltLength: RSA-PSS parameters;
     *      - divisorLength: size of q in bits (DSA);
     *      - namedCurve: the curve name (EC), see getCurves;
     *      - paramEncoding: 'named' (default) or 'explicit' (EC);
     *      - prime, primeLength, generator, groupName: Diffie-Hellman parameters;
     *      - publicKeyEncoding, privateKeyEncoding: export options in the form accepted by
     *        `KeyObject.export`, for example `{ type: 'spki', format: 'pem' }`.
     *
     *      Example: an EC pair and its exported public key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { publicKey, privateKey } = crypto.generateKeyPairSync('ec', {
     *          namedCurve: 'prime256v1'
     *      });
     *      console.log(privateKey.asymmetricKeyType); // ec
     *
     *      const spki = publicKey.export({ type: 'spki', format: 'pem' });
     *      console.log(String(spki).split('\n')[0]);
     *      // -----BEGIN PUBLIC KEY-----
     *      ```
     *
     *      @param type the key type to generate
     *      @param options the key generation and encoding options
     *      @return returns the generated key pair
     *
     */
    function generateKeyPairAsync(type: string, options?: FIBJS.GeneralObject): Promise<{
        publicKey: any;
        privateKey: any;
    }>;

    /**
     * @description Derives key material from a secret with HKDF (RFC 5869)
     *
     *      HKDF is not a password hash: `password` must already be a high-entropy secret,
     *      for example the output of an ECDH exchange. It extracts a pseudorandom key with
     *      the salt and then expands it to `size` bytes bound to `info`; use a different
     *      `info` for every purpose and never reuse one output for two purposes. `salt` and
     *      `info` may be empty, but a non-empty random salt is recommended. The size must
     *      be at least 1. Follows the async call forms; `hkdfSync` blocks.
     *      @param algoName the digest algorithm name
     *      @param password the input keying material
     *      @param salt the extract salt
     *      @param info the context and application specific information
     *      @param size the number of bytes to derive
     *      @return returns the derived key
     *
     */
    function hkdf(algoName: string, password: Class_Buffer | string, salt: Class_Buffer | string, info: Class_Buffer | string, size: number): Class_Buffer;

    function hkdf(algoName: string, password: Class_Buffer | string, salt: Class_Buffer | string, info: Class_Buffer | string, size: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Derives key material from a secret with HKDF (RFC 5869)
     *
     *      HKDF is not a password hash: `password` must already be a high-entropy secret,
     *      for example the output of an ECDH exchange. It extracts a pseudorandom key with
     *      the salt and then expands it to `size` bytes bound to `info`; use a different
     *      `info` for every purpose and never reuse one output for two purposes. `salt` and
     *      `info` may be empty, but a non-empty random salt is recommended. The size must
     *      be at least 1. Follows the async call forms; `hkdfSync` blocks.
     *      @param algoName the digest algorithm name
     *      @param password the input keying material
     *      @param salt the extract salt
     *      @param info the context and application specific information
     *      @param size the number of bytes to derive
     *      @return returns the derived key
     *
     */
    function hkdfSync(algoName: string, password: Class_Buffer | string, salt: Class_Buffer | string, info: Class_Buffer | string, size: number): Class_Buffer;

    /**
     * @description Derives key material from a secret with HKDF (RFC 5869)
     *
     *      HKDF is not a password hash: `password` must already be a high-entropy secret,
     *      for example the output of an ECDH exchange. It extracts a pseudorandom key with
     *      the salt and then expands it to `size` bytes bound to `info`; use a different
     *      `info` for every purpose and never reuse one output for two purposes. `salt` and
     *      `info` may be empty, but a non-empty random salt is recommended. The size must
     *      be at least 1. Follows the async call forms; `hkdfSync` blocks.
     *      @param algoName the digest algorithm name
     *      @param password the input keying material
     *      @param salt the extract salt
     *      @param info the context and application specific information
     *      @param size the number of bytes to derive
     *      @return returns the derived key
     *
     */
    function hkdfAsync(algoName: string, password: Class_Buffer | string, salt: Class_Buffer | string, info: Class_Buffer | string, size: number): Promise<Class_Buffer>;

    /**
     * @description Derives a key from a password with PBKDF2
     *
     *      PBKDF2 applies the digest many times over the password and the salt; the
     *      iteration count is the cost. The salt must be random, unique per password and at
     *      least 16 bytes, and it must be stored with the derived key, as must the iteration
     *      count and the digest name - without them the key cannot be reproduced. There is
     *      no built-in cost bound: choose an iteration count that keeps one derivation
     *      around 100 ms on the target hardware. Both the iteration count and the size must
     *      be at least 1. Follows the async call forms; `pbkdf2Sync` blocks.
     *
     *      Example: the RFC 6070 vector and a salted derivation:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      console.log(crypto.pbkdf2Sync('password', 'salt', 1, 20, 'sha1').toString('hex'));
     *      // 0c60c80f961f0e71f3a9b524af6012062fe037a6
     *
     *      const salt = crypto.randomBytes(16);
     *      const key = crypto.pbkdf2Sync('secret password', salt, 210000, 32, 'sha256');
     *      console.log(key.length); // 32
     *      ```
     *
     *      @param password the password to derive from
     *      @param salt the random salt
     *      @param iterations the iteration count
     *      @param size the number of bytes to derive
     *      @param algoName the digest algorithm name
     *      @return returns the derived key
     *
     */
    function pbkdf2(password: Class_Buffer | string, salt: Class_Buffer | string, iterations: number, size: number, algoName: string): Class_Buffer;

    function pbkdf2(password: Class_Buffer | string, salt: Class_Buffer | string, iterations: number, size: number, algoName: string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Derives a key from a password with PBKDF2
     *
     *      PBKDF2 applies the digest many times over the password and the salt; the
     *      iteration count is the cost. The salt must be random, unique per password and at
     *      least 16 bytes, and it must be stored with the derived key, as must the iteration
     *      count and the digest name - without them the key cannot be reproduced. There is
     *      no built-in cost bound: choose an iteration count that keeps one derivation
     *      around 100 ms on the target hardware. Both the iteration count and the size must
     *      be at least 1. Follows the async call forms; `pbkdf2Sync` blocks.
     *
     *      Example: the RFC 6070 vector and a salted derivation:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      console.log(crypto.pbkdf2Sync('password', 'salt', 1, 20, 'sha1').toString('hex'));
     *      // 0c60c80f961f0e71f3a9b524af6012062fe037a6
     *
     *      const salt = crypto.randomBytes(16);
     *      const key = crypto.pbkdf2Sync('secret password', salt, 210000, 32, 'sha256');
     *      console.log(key.length); // 32
     *      ```
     *
     *      @param password the password to derive from
     *      @param salt the random salt
     *      @param iterations the iteration count
     *      @param size the number of bytes to derive
     *      @param algoName the digest algorithm name
     *      @return returns the derived key
     *
     */
    function pbkdf2Sync(password: Class_Buffer | string, salt: Class_Buffer | string, iterations: number, size: number, algoName: string): Class_Buffer;

    /**
     * @description Derives a key from a password with PBKDF2
     *
     *      PBKDF2 applies the digest many times over the password and the salt; the
     *      iteration count is the cost. The salt must be random, unique per password and at
     *      least 16 bytes, and it must be stored with the derived key, as must the iteration
     *      count and the digest name - without them the key cannot be reproduced. There is
     *      no built-in cost bound: choose an iteration count that keeps one derivation
     *      around 100 ms on the target hardware. Both the iteration count and the size must
     *      be at least 1. Follows the async call forms; `pbkdf2Sync` blocks.
     *
     *      Example: the RFC 6070 vector and a salted derivation:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      console.log(crypto.pbkdf2Sync('password', 'salt', 1, 20, 'sha1').toString('hex'));
     *      // 0c60c80f961f0e71f3a9b524af6012062fe037a6
     *
     *      const salt = crypto.randomBytes(16);
     *      const key = crypto.pbkdf2Sync('secret password', salt, 210000, 32, 'sha256');
     *      console.log(key.length); // 32
     *      ```
     *
     *      @param password the password to derive from
     *      @param salt the random salt
     *      @param iterations the iteration count
     *      @param size the number of bytes to derive
     *      @param algoName the digest algorithm name
     *      @return returns the derived key
     *
     */
    function pbkdf2Async(password: Class_Buffer | string, salt: Class_Buffer | string, iterations: number, size: number, algoName: string): Promise<Class_Buffer>;

    /**
     * @description Derives a key from a password with scrypt
     *
     *      scrypt is memory-hard: N (CPU/memory cost), r (block size) and p (parallelism)
     *      control the work, and the defaults N=16384, r=8, p=1 need about 16 MB of memory
     *      per call. N must be a power of 2 greater than 1, r and p must be positive, and
     *      maxmem caps the memory in bytes. As with pbkdf2, store the salt and the
     *      parameters with the derived key. The size must be at least 1. Follows the async
     *      call forms; `scryptSync` blocks.
     *
     *      options supports the following options:
     *      - N: CPU/memory cost, a power of 2, default 16384;
     *      - r: block size, default 8;
     *      - p: parallelization, default 1;
     *      - maxmem: memory cap in bytes, default 32 * 1024 * 1024.
     *
     *      Example: the same parameters derive the same key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const salt = crypto.randomBytes(16);
     *      const key = crypto.scryptSync('correct horse battery staple', salt, 32, {
     *          N: 16384, r: 8, p: 1
     *      });
     *      console.log(key.length); // 32
     *
     *      // The defaults are N=16384, r=8, p=1, so this derives the same key.
     *      console.log(key.equals(crypto.scryptSync('correct horse battery staple', salt, 32))); // true
     *      ```
     *
     *      @param password the password to derive from
     *      @param salt the random salt
     *      @param keylen the number of bytes to derive
     *      @param options the N, r, p and maxmem parameters
     *      @return returns the derived key
     *
     */
    function scrypt(password: Class_Buffer | string, salt: Class_Buffer | string, keylen: number, options?: FIBJS.GeneralObject): Class_Buffer;

    function scrypt(password: Class_Buffer | string, salt: Class_Buffer | string, keylen: number, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Derives a key from a password with scrypt
     *
     *      scrypt is memory-hard: N (CPU/memory cost), r (block size) and p (parallelism)
     *      control the work, and the defaults N=16384, r=8, p=1 need about 16 MB of memory
     *      per call. N must be a power of 2 greater than 1, r and p must be positive, and
     *      maxmem caps the memory in bytes. As with pbkdf2, store the salt and the
     *      parameters with the derived key. The size must be at least 1. Follows the async
     *      call forms; `scryptSync` blocks.
     *
     *      options supports the following options:
     *      - N: CPU/memory cost, a power of 2, default 16384;
     *      - r: block size, default 8;
     *      - p: parallelization, default 1;
     *      - maxmem: memory cap in bytes, default 32 * 1024 * 1024.
     *
     *      Example: the same parameters derive the same key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const salt = crypto.randomBytes(16);
     *      const key = crypto.scryptSync('correct horse battery staple', salt, 32, {
     *          N: 16384, r: 8, p: 1
     *      });
     *      console.log(key.length); // 32
     *
     *      // The defaults are N=16384, r=8, p=1, so this derives the same key.
     *      console.log(key.equals(crypto.scryptSync('correct horse battery staple', salt, 32))); // true
     *      ```
     *
     *      @param password the password to derive from
     *      @param salt the random salt
     *      @param keylen the number of bytes to derive
     *      @param options the N, r, p and maxmem parameters
     *      @return returns the derived key
     *
     */
    function scryptSync(password: Class_Buffer | string, salt: Class_Buffer | string, keylen: number, options?: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Derives a key from a password with scrypt
     *
     *      scrypt is memory-hard: N (CPU/memory cost), r (block size) and p (parallelism)
     *      control the work, and the defaults N=16384, r=8, p=1 need about 16 MB of memory
     *      per call. N must be a power of 2 greater than 1, r and p must be positive, and
     *      maxmem caps the memory in bytes. As with pbkdf2, store the salt and the
     *      parameters with the derived key. The size must be at least 1. Follows the async
     *      call forms; `scryptSync` blocks.
     *
     *      options supports the following options:
     *      - N: CPU/memory cost, a power of 2, default 16384;
     *      - r: block size, default 8;
     *      - p: parallelization, default 1;
     *      - maxmem: memory cap in bytes, default 32 * 1024 * 1024.
     *
     *      Example: the same parameters derive the same key:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const salt = crypto.randomBytes(16);
     *      const key = crypto.scryptSync('correct horse battery staple', salt, 32, {
     *          N: 16384, r: 8, p: 1
     *      });
     *      console.log(key.length); // 32
     *
     *      // The defaults are N=16384, r=8, p=1, so this derives the same key.
     *      console.log(key.equals(crypto.scryptSync('correct horse battery staple', salt, 32))); // true
     *      ```
     *
     *      @param password the password to derive from
     *      @param salt the random salt
     *      @param keylen the number of bytes to derive
     *      @param options the N, r, p and maxmem parameters
     *      @return returns the derived key
     *
     */
    function scryptAsync(password: Class_Buffer | string, salt: Class_Buffer | string, keylen: number, options?: FIBJS.GeneralObject): Promise<Class_Buffer>;

    /**
     * @description Decrypts data that was encrypted with the matching public key
     *
     *      The RSA counterpart of publicEncrypt; the default padding is OAEP
     *      (RSA_PKCS1_OAEP_PADDING). `privateKey` may be a private KeyObject, a PEM/DER
     *      string or Buffer, or an options object that is passed to createPrivateKey
     *      (`key`, `passphrase`, ...) together with the RSA options `padding`, `oaepHash`
     *      (default 'sha1') and `oaepLabel`. A string `buffer` is accepted only together
     *      with the options object and is decoded with its `encoding` (default utf8).
     *      Throws when the key is wrong or the padding does not match.
     *      @param privateKey the private key and options
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function privateDecrypt(privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, buffer: Class_Buffer | string): Class_Buffer;

    /**
     * @description Encrypts data with the private key (RSA signature-style operation)
     *
     *      Despite the name this does not provide confidentiality: anyone with the public
     *      key can recover the plaintext. It computes the RSA private-key operation with
     *      PKCS#1 v1.5 padding by default and is useful for interoperating with protocols
     *      that expect it; use sign for normal signatures. `privateKey` accepts the same
     *      forms as privateDecrypt and the options add `padding`; a string buffer is
     *      decoded with the object's `encoding`.
     *      @param privateKey the private key and options
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function privateEncrypt(privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, buffer: Class_Buffer | string): Class_Buffer;

    /**
     * @description Decrypts data that was encrypted with the matching private key
     *
     *      The counterpart of privateEncrypt, with PKCS#1 v1.5 padding by default. It only
     *      shows that the data was produced by the holder of the private key - it is not a
     *      public-key encryption API, because anyone can recompute it with the public key.
     *      `publicKey` accepts the same forms as publicEncrypt and the options add
     *      `padding`; a string buffer is decoded with the object's `encoding`.
     *      @param publicKey the public key and options
     *      @param buffer the data to decrypt
     *      @return returns the decrypted data
     *
     */
    function publicDecrypt(publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, buffer: Class_Buffer | string): Class_Buffer;

    /**
     * @description Encrypts data with the public key so that only the private key can decrypt it
     *
     *      The default padding is OAEP (RSA_PKCS1_OAEP_PADDING), the safe choice for new
     *      protocols; PKCS#1 v1.5 (`padding: crypto.constants.RSA_PKCS1_PADDING`) is
     *      available for compatibility. RSA can encrypt only a small payload (a 2048-bit
     *      key with OAEP-SHA1 accepts at most 214 bytes): encrypt a symmetric key or a
     *      digest and carry the bulk data with a cipher. `publicKey` may be a public
     *      KeyObject, a PEM/DER string or Buffer, or an options object passed to
     *      createPublicKey plus `padding`, `oaepHash` and `oaepLabel`; a string buffer is
     *      decoded with its `encoding`.
     *
     *      Example: RSA-OAEP-SHA256 round trip:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { publicKey, privateKey } = crypto.generateKeyPairSync('rsa', {
     *          modulusLength: 2048
     *      });
     *      const ciphertext = crypto.publicEncrypt({
     *          key: publicKey,
     *          padding: crypto.constants.RSA_PKCS1_OAEP_PADDING,
     *          oaepHash: 'sha256'
     *      }, Buffer.from('secret message'));
     *
     *      const plaintext = crypto.privateDecrypt({
     *          key: privateKey,
     *          padding: crypto.constants.RSA_PKCS1_OAEP_PADDING,
     *          oaepHash: 'sha256'
     *      }, ciphertext);
     *      console.log(plaintext.toString()); // secret message
     *      ```
     *
     *      @param publicKey the public key and options
     *      @param buffer the data to encrypt
     *      @return returns the encrypted data
     *
     */
    function publicEncrypt(publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, buffer: Class_Buffer | string): Class_Buffer;

    /**
     * @description Computes the signature of data in one shot
     *
     *      The private key selects the algorithm family: RSA (PKCS#1 v1.5 or PSS), ECDSA,
     *      DSA, Ed25519/Ed448, SM2 and Bls12381G1/G2 are supported. For Ed25519 and Ed448
     *      pass `null` (or undefined) as the algorithm - they digest internally and reject
     *      a separate one. With an RSA-PSS key, or when `padding` is set to
     *      RSA_PKCS1_PSS_PADDING, the signature is PSS. `dsaEncoding` selects 'der'
     *      (default) or 'ieee-p1363' for DSA/ECDSA, and the verifier must use the same.
     *      The one-shot form is the only option for Ed25519/Ed448; use createSign when the
     *      data arrives in chunks. Follows the async call forms.
     *
     *      Example: an EC signature, verified and tampered with:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { publicKey, privateKey } = crypto.generateKeyPairSync('ec', {
     *          namedCurve: 'prime256v1'
     *      });
     *      const signature = crypto.sign('sha256', 'message', {
     *          key: privateKey,
     *          dsaEncoding: 'ieee-p1363'
     *      });
     *
     *      console.log(crypto.verify('sha256', 'message', {
     *          key: publicKey,
     *          dsaEncoding: 'ieee-p1363'
     *      }, signature)); // true
     *      console.log(crypto.verify('sha256', 'tampered', publicKey, signature)); // false
     *      ```
     *
     *      @param algorithm the digest algorithm name, or null for Ed25519/Ed448
     *      @param data the data to sign
     *      @param key the private key and signing options
     *      @return returns the signature
     *
     */
    function sign(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    function sign(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Computes the signature of data in one shot
     *
     *      The private key selects the algorithm family: RSA (PKCS#1 v1.5 or PSS), ECDSA,
     *      DSA, Ed25519/Ed448, SM2 and Bls12381G1/G2 are supported. For Ed25519 and Ed448
     *      pass `null` (or undefined) as the algorithm - they digest internally and reject
     *      a separate one. With an RSA-PSS key, or when `padding` is set to
     *      RSA_PKCS1_PSS_PADDING, the signature is PSS. `dsaEncoding` selects 'der'
     *      (default) or 'ieee-p1363' for DSA/ECDSA, and the verifier must use the same.
     *      The one-shot form is the only option for Ed25519/Ed448; use createSign when the
     *      data arrives in chunks. Follows the async call forms.
     *
     *      Example: an EC signature, verified and tampered with:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { publicKey, privateKey } = crypto.generateKeyPairSync('ec', {
     *          namedCurve: 'prime256v1'
     *      });
     *      const signature = crypto.sign('sha256', 'message', {
     *          key: privateKey,
     *          dsaEncoding: 'ieee-p1363'
     *      });
     *
     *      console.log(crypto.verify('sha256', 'message', {
     *          key: publicKey,
     *          dsaEncoding: 'ieee-p1363'
     *      }, signature)); // true
     *      console.log(crypto.verify('sha256', 'tampered', publicKey, signature)); // false
     *      ```
     *
     *      @param algorithm the digest algorithm name, or null for Ed25519/Ed448
     *      @param data the data to sign
     *      @param key the private key and signing options
     *      @return returns the signature
     *
     */
    function signSync(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    /**
     * @description Computes the signature of data in one shot
     *
     *      The private key selects the algorithm family: RSA (PKCS#1 v1.5 or PSS), ECDSA,
     *      DSA, Ed25519/Ed448, SM2 and Bls12381G1/G2 are supported. For Ed25519 and Ed448
     *      pass `null` (or undefined) as the algorithm - they digest internally and reject
     *      a separate one. With an RSA-PSS key, or when `padding` is set to
     *      RSA_PKCS1_PSS_PADDING, the signature is PSS. `dsaEncoding` selects 'der'
     *      (default) or 'ieee-p1363' for DSA/ECDSA, and the verifier must use the same.
     *      The one-shot form is the only option for Ed25519/Ed448; use createSign when the
     *      data arrives in chunks. Follows the async call forms.
     *
     *      Example: an EC signature, verified and tampered with:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { publicKey, privateKey } = crypto.generateKeyPairSync('ec', {
     *          namedCurve: 'prime256v1'
     *      });
     *      const signature = crypto.sign('sha256', 'message', {
     *          key: privateKey,
     *          dsaEncoding: 'ieee-p1363'
     *      });
     *
     *      console.log(crypto.verify('sha256', 'message', {
     *          key: publicKey,
     *          dsaEncoding: 'ieee-p1363'
     *      }, signature)); // true
     *      console.log(crypto.verify('sha256', 'tampered', publicKey, signature)); // false
     *      ```
     *
     *      @param algorithm the digest algorithm name, or null for Ed25519/Ed448
     *      @param data the data to sign
     *      @param key the private key and signing options
     *      @return returns the signature
     *
     */
    function signAsync(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Verifies the signature of data in one shot
     *
     *      Returns true when the signature matches and false when it does not; malformed
     *      input or an unusable key throws instead. The algorithm and the key options must
     *      match those used to sign, including `dsaEncoding` for DSA/ECDSA and the RSA
     *      padding. For Ed25519 and Ed448 pass null as the algorithm. `data` and
     *      `signature` may be Buffers or strings decoded as utf8. Follows the async call
     *      forms.
     *      @param algorithm the digest algorithm name, or null for Ed25519/Ed448
     *      @param data the data that was signed
     *      @param key the public key and verifying options
     *      @param signature the signature to verify
     *      @return returns true when the signature is valid
     *
     */
    function verify(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): boolean;

    function verify(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Verifies the signature of data in one shot
     *
     *      Returns true when the signature matches and false when it does not; malformed
     *      input or an unusable key throws instead. The algorithm and the key options must
     *      match those used to sign, including `dsaEncoding` for DSA/ECDSA and the RSA
     *      padding. For Ed25519 and Ed448 pass null as the algorithm. `data` and
     *      `signature` may be Buffers or strings decoded as utf8. Follows the async call
     *      forms.
     *      @param algorithm the digest algorithm name, or null for Ed25519/Ed448
     *      @param data the data that was signed
     *      @param key the public key and verifying options
     *      @param signature the signature to verify
     *      @return returns true when the signature is valid
     *
     */
    function verifySync(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): boolean;

    /**
     * @description Verifies the signature of data in one shot
     *
     *      Returns true when the signature matches and false when it does not; malformed
     *      input or an unusable key throws instead. The algorithm and the key options must
     *      match those used to sign, including `dsaEncoding` for DSA/ECDSA and the RSA
     *      padding. For Ed25519 and Ed448 pass null as the algorithm. `data` and
     *      `signature` may be Buffers or strings decoded as utf8. Follows the async call
     *      forms.
     *      @param algorithm the digest algorithm name, or null for Ed25519/Ed448
     *      @param data the data that was signed
     *      @param key the public key and verifying options
     *      @param signature the signature to verify
     *      @return returns true when the signature is valid
     *
     */
    function verifyAsync(algorithm: any, data: Class_Buffer | string, key: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Compares two byte sequences in constant time
     *
     *      Use it to compare MACs, password hashes and other secret-derived values: an
     *      ordinary == comparison returns as soon as a difference is found and leaks how
     *      many leading bytes matched. Both inputs are taken as bytes - strings are encoded
     *      as utf8 - and must have the same length, otherwise an error is thrown (a length
     *      difference is not secret when the expected length is public). The comparison
     *      does not reveal which byte differed.
     *
     *      Example: compare a received HMAC tag against the expected one:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const key = crypto.randomBytes(32);
     *      const expected = crypto.createHmac('sha256', key).update('payload').digest();
     *      const received = crypto.createHmac('sha256', key).update('payload').digest();
     *
     *      console.log(crypto.timingSafeEqual(expected, received)); // true
     *      console.log(crypto.timingSafeEqual(expected, Buffer.alloc(expected.length))); // false
     *      ```
     *
     *      @param a the first value to compare
     *      @param b the second value to compare
     *      @return returns true when the bytes are equal
     *
     */
    function timingSafeEqual(a: Class_Buffer | string, b: Class_Buffer | string): boolean;

    /**
     * @description Signs a group of messages with a Bls12381G2 key using BBS
     *
     *      BBS signatures support selective disclosure: one signature over a set of
     *      messages can later be turned into a proof that reveals only chosen messages
     *      (see proofGen). `messages` is an array of Buffers or strings (utf8). The private
     *      key must be a Bls12381G2 private key; the options object carries `suite`, either
     *      'Bls12381Sha256' (default) or 'Bls12381Shake256', and an optional `header` bound
     *      into the signature. The signer and the verifier must use the same suite and
     *      header. Follows the async call forms; this member has no Node.js counterpart.
     *      @param messages the messages to sign
     *      @param privateKey the private key and options
     *      @return returns the signature
     *
     */
    function bbsSign(messages: (Class_Buffer | string)[], privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    function bbsSign(messages: (Class_Buffer | string)[], privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Signs a group of messages with a Bls12381G2 key using BBS
     *
     *      BBS signatures support selective disclosure: one signature over a set of
     *      messages can later be turned into a proof that reveals only chosen messages
     *      (see proofGen). `messages` is an array of Buffers or strings (utf8). The private
     *      key must be a Bls12381G2 private key; the options object carries `suite`, either
     *      'Bls12381Sha256' (default) or 'Bls12381Shake256', and an optional `header` bound
     *      into the signature. The signer and the verifier must use the same suite and
     *      header. Follows the async call forms; this member has no Node.js counterpart.
     *      @param messages the messages to sign
     *      @param privateKey the private key and options
     *      @return returns the signature
     *
     */
    function bbsSignSync(messages: (Class_Buffer | string)[], privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    /**
     * @description Signs a group of messages with a Bls12381G2 key using BBS
     *
     *      BBS signatures support selective disclosure: one signature over a set of
     *      messages can later be turned into a proof that reveals only chosen messages
     *      (see proofGen). `messages` is an array of Buffers or strings (utf8). The private
     *      key must be a Bls12381G2 private key; the options object carries `suite`, either
     *      'Bls12381Sha256' (default) or 'Bls12381Shake256', and an optional `header` bound
     *      into the signature. The signer and the verifier must use the same suite and
     *      header. Follows the async call forms; this member has no Node.js counterpart.
     *      @param messages the messages to sign
     *      @param privateKey the private key and options
     *      @return returns the signature
     *
     */
    function bbsSignAsync(messages: (Class_Buffer | string)[], privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Verifies a BBS signature over a group of messages
     *
     *      All messages covered by the signature must be supplied, in the original order,
     *      together with the same `suite` and `header` options used to sign. Returns true
     *      for a valid signature and false otherwise. Follows the async call forms.
     *      @param messages the messages to verify
     *      @param publicKey the public key and options
     *      @param signature the signature to verify
     *      @return returns true when the signature is valid
     *
     */
    function bbsVerify(messages: (Class_Buffer | string)[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): boolean;

    function bbsVerify(messages: (Class_Buffer | string)[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Verifies a BBS signature over a group of messages
     *
     *      All messages covered by the signature must be supplied, in the original order,
     *      together with the same `suite` and `header` options used to sign. Returns true
     *      for a valid signature and false otherwise. Follows the async call forms.
     *      @param messages the messages to verify
     *      @param publicKey the public key and options
     *      @param signature the signature to verify
     *      @return returns true when the signature is valid
     *
     */
    function bbsVerifySync(messages: (Class_Buffer | string)[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): boolean;

    /**
     * @description Verifies a BBS signature over a group of messages
     *
     *      All messages covered by the signature must be supplied, in the original order,
     *      together with the same `suite` and `header` options used to sign. Returns true
     *      for a valid signature and false otherwise. Follows the async call forms.
     *      @param messages the messages to verify
     *      @param publicKey the public key and options
     *      @param signature the signature to verify
     *      @return returns true when the signature is valid
     *
     */
    function bbsVerifyAsync(messages: (Class_Buffer | string)[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Derives a selective-disclosure proof from a BBS signature
     *
     *      Creates a proof that reveals only the messages at the positions named by `index`
     *      (0-based positions in `messages`) while proving that the full set was signed.
     *      The signature must be the one returned by bbsSign; the public key options carry
     *      `suite` and `header`, which must match the signature. Follows the async call
     *      forms.
     *      @param signature the BBS signature to derive from
     *      @param messages the full signed message set
     *      @param index the positions of the messages to reveal
     *      @param publicKey the public key and options
     *      @return returns the proof
     *
     */
    function proofGen(signature: Class_Buffer | string, messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    function proofGen(signature: Class_Buffer | string, messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Derives a selective-disclosure proof from a BBS signature
     *
     *      Creates a proof that reveals only the messages at the positions named by `index`
     *      (0-based positions in `messages`) while proving that the full set was signed.
     *      The signature must be the one returned by bbsSign; the public key options carry
     *      `suite` and `header`, which must match the signature. Follows the async call
     *      forms.
     *      @param signature the BBS signature to derive from
     *      @param messages the full signed message set
     *      @param index the positions of the messages to reveal
     *      @param publicKey the public key and options
     *      @return returns the proof
     *
     */
    function proofGenSync(signature: Class_Buffer | string, messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Class_Buffer;

    /**
     * @description Derives a selective-disclosure proof from a BBS signature
     *
     *      Creates a proof that reveals only the messages at the positions named by `index`
     *      (0-based positions in `messages`) while proving that the full set was signed.
     *      The signature must be the one returned by bbsSign; the public key options carry
     *      `suite` and `header`, which must match the signature. Follows the async call
     *      forms.
     *      @param signature the BBS signature to derive from
     *      @param messages the full signed message set
     *      @param index the positions of the messages to reveal
     *      @param publicKey the public key and options
     *      @return returns the proof
     *
     */
    function proofGenAsync(signature: Class_Buffer | string, messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string): Promise<Class_Buffer>;

    /**
     * @description Verifies a selective-disclosure BBS proof
     *
     *      `messages` must contain only the revealed messages, in the same order as the
     *      positions listed in `index` (so `index` and `messages` have the same length).
     *      Returns true when the proof is valid for those messages and the public key.
     *      Follows the async call forms.
     *      @param messages the revealed messages, in the order of index
     *      @param index the positions the revealed messages came from
     *      @param publicKey the public key and options
     *      @param proof the proof to verify
     *      @return returns true when the proof is valid
     *
     */
    function proofVerify(messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, proof: Class_Buffer | string): boolean;

    function proofVerify(messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, proof: Class_Buffer | string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Verifies a selective-disclosure BBS proof
     *
     *      `messages` must contain only the revealed messages, in the same order as the
     *      positions listed in `index` (so `index` and `messages` have the same length).
     *      Returns true when the proof is valid for those messages and the public key.
     *      Follows the async call forms.
     *      @param messages the revealed messages, in the order of index
     *      @param index the positions the revealed messages came from
     *      @param publicKey the public key and options
     *      @param proof the proof to verify
     *      @return returns true when the proof is valid
     *
     */
    function proofVerifySync(messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, proof: Class_Buffer | string): boolean;

    /**
     * @description Verifies a selective-disclosure BBS proof
     *
     *      `messages` must contain only the revealed messages, in the same order as the
     *      positions listed in `index` (so `index` and `messages` have the same length).
     *      Returns true when the proof is valid for those messages and the public key.
     *      Follows the async call forms.
     *      @param messages the revealed messages, in the order of index
     *      @param index the positions the revealed messages came from
     *      @param publicKey the public key and options
     *      @param proof the proof to verify
     *      @return returns true when the proof is valid
     *
     */
    function proofVerifyAsync(messages: (Class_Buffer | string)[], index: number[], publicKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, proof: Class_Buffer | string): Promise<boolean>;

    /**
     * @description The Web Crypto API entry point, see webcrypto
     *
     *      Provides `getRandomValues`, `randomUUID`, `CryptoKey` and `subtle`. Reach it as
     *      `crypto.webcrypto` - there is no global `webcrypto` object.
     */
    const webcrypto: typeof import ('webcrypto');

    /**
     * @description The SubtleCrypto API, see subtle
     *
     *      The same object as `crypto.webcrypto.subtle`; its methods return promises and
     *      cover digest, generateKey, importKey/exportKey, sign/verify, encrypt/decrypt and
     *      deriveBits/deriveKey.
     */
    const subtle: typeof import ('subtle');

}

