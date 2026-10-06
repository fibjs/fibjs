/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/CryptoKey.d.ts" />
/**
 * @description Promise-based Web Crypto operations: digests, keys, signatures and ECDH agreement
 *
 *  The subtle module is the operation surface of the Web Crypto API (the MDN SubtleCrypto
 *  interface). Every method returns a Promise; fibjs also generates a blocking `...Sync` alias
 *  and a `...Async` alias, so `await subtle.digest(...)`, `subtle.digestSync(...)` and
 *  `subtle.digestAsync(...)` all perform the same operation.
 *
 *  Obtained from:
 *  - `require('crypto').subtle` — the standalone entry point;
 *  - `require('crypto').webcrypto.subtle` and the global `crypto.subtle` — the same object;
 *  - `new subtle()` is not allowed: the object is a singleton.
 *
 *  Concepts:
 *
 *  - **Algorithm identifiers**: every method takes an algorithm that is either a string or an
 *    object with a `name`; the extra members differ per operation: `namedCurve` (ECDSA,
 *    ECDH), `hash` (HMAC keys; ECDSA signing), `length` in bits (HMAC generation) and `public`
 *    (the peer key of ECDH agreement). Names match case-insensitively.
 *  - **Supported algorithms**: digest accepts every OpenSSL digest name; key generation,
 *    import and export cover ECDSA, Ed25519, ECDH (P-256/P-384/P-521) and HMAC
 *    (SHA-1/SHA-256/SHA-384/SHA-512). RSA, AES, PBKDF2, HKDF, encrypt/decrypt, deriveKey and
 *    wrapKey are not implemented: use the crypto module for those.
 *  - **Key usages**: a key can only perform operations listed in its usages, otherwise the
 *    returned promise rejects. ECDSA and Ed25519 pairs distribute 'sign' to the private key
 *    and 'verify' to the public key; ECDH public keys carry no usages.
 *  - **extractable**: only extractable keys can be exported. Public keys are always
 *    extractable; a private or secret key is extractable only when the caller asks for it.
 *  - **Data and encodings**: data and key material accept a Buffer, a typed array, an
 *    ArrayBuffer or a string (read as utf8). Signatures use the Web Crypto encodings: ECDSA is
 *    the fixed-size P1363 r||s pair (64 bytes for P-256) and Ed25519 is the raw 64-byte
 *    signature, unlike the DER encoding that `crypto.sign` uses by default (`dsaEncoding:
 *    'ieee-p1363'` makes the crypto module interoperate).
 *  - **Errors**: unsupported algorithms, names, curves, hashes or usages reject with an Error
 *    whose `number` is 20024; wrong argument types throw a TypeError (20005).
 *
 *  Example 1 — SHA-256 digest of a fixed message:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  (async () => {
 *      const digest = await crypto.subtle.digest('SHA-256', 'abc');
 *      console.log(Buffer.from(digest).toString('hex'));
 *      // ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
 *  })();
 *  ```
 *
 *  Example 2 — sign and verify with an ECDSA P-256 key pair:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  (async () => {
 *      const pair = await crypto.subtle.generateKey(
 *          { name: 'ECDSA', namedCurve: 'P-256' }, true, ['sign', 'verify']);
 *      const signature = await crypto.subtle.sign(
 *          { name: 'ECDSA', hash: 'SHA-256' }, pair.privateKey, 'message');
 *
 *      console.log(signature.byteLength); // 64
 *      console.log(await crypto.subtle.verify(
 *          { name: 'ECDSA', hash: 'SHA-256' }, pair.publicKey, signature, 'message')); // true
 *      console.log(await crypto.subtle.verify(
 *          { name: 'ECDSA', hash: 'SHA-256' }, pair.publicKey, signature, 'tampered')); // false
 *  })();
 *  ```
 *
 *  Example 3 — both sides of an ECDH exchange derive the same secret:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  (async () => {
 *      const alice = await crypto.subtle.generateKey(
 *          { name: 'ECDH', namedCurve: 'P-256' }, false, ['deriveBits']);
 *      const bob = await crypto.subtle.generateKey(
 *          { name: 'ECDH', namedCurve: 'P-256' }, false, ['deriveBits']);
 *
 *      const fromAlice = await crypto.subtle.deriveBits(
 *          { name: 'ECDH', public: bob.publicKey }, alice.privateKey, 256);
 *      const fromBob = await crypto.subtle.deriveBits(
 *          { name: 'ECDH', public: alice.publicKey }, bob.privateKey, 256);
 *
 *      console.log(fromAlice.byteLength); // 32
 *      console.log(Buffer.from(fromAlice).equals(Buffer.from(fromBob))); // true
 *  })();
 *  ```
 *
 */
declare module 'subtle' {
    /**
     * @description Computes the digest of data and resolves to an ArrayBuffer
     *
     *      The algorithm is a digest name or an object with a `name` member; any digest OpenSSL
     *      knows is accepted (SHA-1, SHA-256, SHA-384, SHA-512, SHA3-256, MD5, SM3, ...) and names
     *      match case-insensitively. The data may be a Buffer, a typed array, an ArrayBuffer or a
     *      string (encoded as utf8). The result size follows the algorithm: 20 bytes for SHA-1,
     *      32 for SHA-256, 48 for SHA-384 and 64 for SHA-512. An unknown algorithm rejects with
     *      Error 20024 and an unsupported data type throws a TypeError.
     *
     *      Example - an object algorithm and a typed array input:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const digest = await crypto.subtle.digest(
     *              { name: 'SHA-256' }, new Uint8Array([1, 2, 3]));
     *          console.log(digest.byteLength); // 32
     *      })();
     *      ```
     *
     *      @param algorithm the digest name or an object with a name member
     *      @param data the data to digest
     *      @return the digest as an ArrayBuffer
     *
     */
    function digest(algorithm: FIBJS.GeneralObject | string, data: Class_Buffer | string): Promise<ArrayBuffer>;

    /**
     * @description Computes the digest of data and resolves to an ArrayBuffer
     *
     *      The algorithm is a digest name or an object with a `name` member; any digest OpenSSL
     *      knows is accepted (SHA-1, SHA-256, SHA-384, SHA-512, SHA3-256, MD5, SM3, ...) and names
     *      match case-insensitively. The data may be a Buffer, a typed array, an ArrayBuffer or a
     *      string (encoded as utf8). The result size follows the algorithm: 20 bytes for SHA-1,
     *      32 for SHA-256, 48 for SHA-384 and 64 for SHA-512. An unknown algorithm rejects with
     *      Error 20024 and an unsupported data type throws a TypeError.
     *
     *      Example - an object algorithm and a typed array input:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const digest = await crypto.subtle.digest(
     *              { name: 'SHA-256' }, new Uint8Array([1, 2, 3]));
     *          console.log(digest.byteLength); // 32
     *      })();
     *      ```
     *
     *      @param algorithm the digest name or an object with a name member
     *      @param data the data to digest
     *      @return the digest as an ArrayBuffer
     *
     */
    function digestSync(algorithm: FIBJS.GeneralObject | string, data: Class_Buffer | string): ArrayBuffer;

    /**
     * @description Computes the digest of data and resolves to an ArrayBuffer
     *
     *      The algorithm is a digest name or an object with a `name` member; any digest OpenSSL
     *      knows is accepted (SHA-1, SHA-256, SHA-384, SHA-512, SHA3-256, MD5, SM3, ...) and names
     *      match case-insensitively. The data may be a Buffer, a typed array, an ArrayBuffer or a
     *      string (encoded as utf8). The result size follows the algorithm: 20 bytes for SHA-1,
     *      32 for SHA-256, 48 for SHA-384 and 64 for SHA-512. An unknown algorithm rejects with
     *      Error 20024 and an unsupported data type throws a TypeError.
     *
     *      Example - an object algorithm and a typed array input:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const digest = await crypto.subtle.digest(
     *              { name: 'SHA-256' }, new Uint8Array([1, 2, 3]));
     *          console.log(digest.byteLength); // 32
     *      })();
     *      ```
     *
     *      @param algorithm the digest name or an object with a name member
     *      @param data the data to digest
     *      @return the digest as an ArrayBuffer
     *
     */
    function digestAsync(algorithm: FIBJS.GeneralObject | string, data: Class_Buffer | string): Promise<ArrayBuffer>;

    /**
     * @description Exports a key as raw, pkcs8, spki or jwk
     *
     *      The formats work as follows:
     *      - `raw` - the key material itself: the uncompressed EC point (65 bytes for P-256), the
     *        32-byte Ed25519 public key or the secret bytes of an HMAC key; public EC/Ed25519 keys
     *        and secret keys only;
     *      - `pkcs8` - DER-encoded private key (EC and Ed25519);
     *      - `spki` - DER-encoded public key (EC and Ed25519);
     *      - `jwk` - a plain object: EC keys expose `kty`, `crv`, `x`, `y` and, when private, `d`;
     *        Ed25519 keys use `kty: 'OKP'`; HMAC keys use `kty: 'oct'`, the base64url key `k` and
     *        an `alg` field such as `HS256`.
     *
     *      The key must be extractable, otherwise the promise rejects with `Key is not extractable`
     *      (Error 20024); public keys are always extractable.
     *
     *      Example - export an EC public key as JWK and re-import it:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'ECDSA', namedCurve: 'P-256' }, true, ['sign', 'verify']);
     *
     *          const jwk = await crypto.subtle.exportKey('jwk', pair.publicKey);
     *          console.log(jwk.kty, jwk.crv, typeof jwk.x); // EC P-256 string
     *
     *          const imported = await crypto.subtle.importKey('jwk', jwk,
     *              { name: 'ECDSA', namedCurve: 'P-256' }, true, ['verify']);
     *          console.log(imported.type); // public
     *      })();
     *      ```
     *
     *      @param format the export format: 'raw', 'pkcs8', 'spki' or 'jwk'
     *      @param key the key to export; it must be extractable
     *      @return the exported material as an ArrayBuffer, or a plain object for jwk
     *
     */
    function exportKey(format: string, key: Class_CryptoKey): Promise<any>;

    /**
     * @description Exports a key as raw, pkcs8, spki or jwk
     *
     *      The formats work as follows:
     *      - `raw` - the key material itself: the uncompressed EC point (65 bytes for P-256), the
     *        32-byte Ed25519 public key or the secret bytes of an HMAC key; public EC/Ed25519 keys
     *        and secret keys only;
     *      - `pkcs8` - DER-encoded private key (EC and Ed25519);
     *      - `spki` - DER-encoded public key (EC and Ed25519);
     *      - `jwk` - a plain object: EC keys expose `kty`, `crv`, `x`, `y` and, when private, `d`;
     *        Ed25519 keys use `kty: 'OKP'`; HMAC keys use `kty: 'oct'`, the base64url key `k` and
     *        an `alg` field such as `HS256`.
     *
     *      The key must be extractable, otherwise the promise rejects with `Key is not extractable`
     *      (Error 20024); public keys are always extractable.
     *
     *      Example - export an EC public key as JWK and re-import it:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'ECDSA', namedCurve: 'P-256' }, true, ['sign', 'verify']);
     *
     *          const jwk = await crypto.subtle.exportKey('jwk', pair.publicKey);
     *          console.log(jwk.kty, jwk.crv, typeof jwk.x); // EC P-256 string
     *
     *          const imported = await crypto.subtle.importKey('jwk', jwk,
     *              { name: 'ECDSA', namedCurve: 'P-256' }, true, ['verify']);
     *          console.log(imported.type); // public
     *      })();
     *      ```
     *
     *      @param format the export format: 'raw', 'pkcs8', 'spki' or 'jwk'
     *      @param key the key to export; it must be extractable
     *      @return the exported material as an ArrayBuffer, or a plain object for jwk
     *
     */
    function exportKeySync(format: string, key: Class_CryptoKey): any;

    /**
     * @description Exports a key as raw, pkcs8, spki or jwk
     *
     *      The formats work as follows:
     *      - `raw` - the key material itself: the uncompressed EC point (65 bytes for P-256), the
     *        32-byte Ed25519 public key or the secret bytes of an HMAC key; public EC/Ed25519 keys
     *        and secret keys only;
     *      - `pkcs8` - DER-encoded private key (EC and Ed25519);
     *      - `spki` - DER-encoded public key (EC and Ed25519);
     *      - `jwk` - a plain object: EC keys expose `kty`, `crv`, `x`, `y` and, when private, `d`;
     *        Ed25519 keys use `kty: 'OKP'`; HMAC keys use `kty: 'oct'`, the base64url key `k` and
     *        an `alg` field such as `HS256`.
     *
     *      The key must be extractable, otherwise the promise rejects with `Key is not extractable`
     *      (Error 20024); public keys are always extractable.
     *
     *      Example - export an EC public key as JWK and re-import it:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'ECDSA', namedCurve: 'P-256' }, true, ['sign', 'verify']);
     *
     *          const jwk = await crypto.subtle.exportKey('jwk', pair.publicKey);
     *          console.log(jwk.kty, jwk.crv, typeof jwk.x); // EC P-256 string
     *
     *          const imported = await crypto.subtle.importKey('jwk', jwk,
     *              { name: 'ECDSA', namedCurve: 'P-256' }, true, ['verify']);
     *          console.log(imported.type); // public
     *      })();
     *      ```
     *
     *      @param format the export format: 'raw', 'pkcs8', 'spki' or 'jwk'
     *      @param key the key to export; it must be extractable
     *      @return the exported material as an ArrayBuffer, or a plain object for jwk
     *
     */
    function exportKeyAsync(format: string, key: Class_CryptoKey): Promise<any>;

    /**
     * @description Generates a new key or key pair
     *
     *      The algorithm selects the key type and its parameters:
     *      - `ECDSA` with namedCurve - an EC signing key pair;
     *      - `Ed25519` - an Ed25519 signing key pair (no extra parameter);
     *      - `ECDH` with namedCurve - an EC key-agreement key pair;
     *      - `HMAC` with hash, and optionally length in bits - one secret key.
     *
     *      A CryptoKey is returned for HMAC and a plain object with `publicKey` and `privateKey`
     *      properties for ECDSA, Ed25519 and ECDH. The private key is extractable only when the
     *      extractable argument is true; public keys are always extractable. usages is the
     *      allow-list the key will accept: ECDSA and Ed25519 need 'sign' (the pair then gets
     *      'verify' on the public key); ECDH needs 'deriveKey' or 'deriveBits'; HMAC accepts
     *      'sign' and/or 'verify'. An HMAC key generated without length uses the hash block size
     *      (512 bits for SHA-256), reported in `key.algorithm.length`. Unsupported names, curves,
     *      hashes or usages reject with Error 20024.
     *
     *      Example 1 - generate an Ed25519 pair and use it:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'Ed25519' }, true, ['sign', 'verify']);
     *          console.log(pair.privateKey.type, pair.publicKey.type); // private public
     *          console.log(pair.privateKey.algorithm.name, pair.publicKey.usages.length);
     *          // Ed25519 1
     *
     *          const signature = await crypto.subtle.sign('Ed25519', pair.privateKey, 'message');
     *          console.log(signature.byteLength); // 64
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, signature, 'message')); // true
     *      })();
     *      ```
     *
     *      Example 2 - an HMAC key with an explicit length:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const key = await crypto.subtle.generateKey(
     *              { name: 'HMAC', hash: 'SHA-256', length: 256 }, true, ['sign']);
     *          console.log(key.type, key.algorithm.length); // secret 256
     *          console.log((await crypto.subtle.exportKey('raw', key)).byteLength); // 32
     *      })();
     *      ```
     *
     *      @param algorithm the algorithm name or an object with the parameters of the key
     *      @param extractable whether the generated private or secret key can be exported later
     *      @param usages the operations the key may perform; see the module Concepts
     *      @return a CryptoKey, or a `{ publicKey, privateKey }` object for a key pair
     *
     */
    function generateKey(algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Promise<any>;

    /**
     * @description Generates a new key or key pair
     *
     *      The algorithm selects the key type and its parameters:
     *      - `ECDSA` with namedCurve - an EC signing key pair;
     *      - `Ed25519` - an Ed25519 signing key pair (no extra parameter);
     *      - `ECDH` with namedCurve - an EC key-agreement key pair;
     *      - `HMAC` with hash, and optionally length in bits - one secret key.
     *
     *      A CryptoKey is returned for HMAC and a plain object with `publicKey` and `privateKey`
     *      properties for ECDSA, Ed25519 and ECDH. The private key is extractable only when the
     *      extractable argument is true; public keys are always extractable. usages is the
     *      allow-list the key will accept: ECDSA and Ed25519 need 'sign' (the pair then gets
     *      'verify' on the public key); ECDH needs 'deriveKey' or 'deriveBits'; HMAC accepts
     *      'sign' and/or 'verify'. An HMAC key generated without length uses the hash block size
     *      (512 bits for SHA-256), reported in `key.algorithm.length`. Unsupported names, curves,
     *      hashes or usages reject with Error 20024.
     *
     *      Example 1 - generate an Ed25519 pair and use it:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'Ed25519' }, true, ['sign', 'verify']);
     *          console.log(pair.privateKey.type, pair.publicKey.type); // private public
     *          console.log(pair.privateKey.algorithm.name, pair.publicKey.usages.length);
     *          // Ed25519 1
     *
     *          const signature = await crypto.subtle.sign('Ed25519', pair.privateKey, 'message');
     *          console.log(signature.byteLength); // 64
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, signature, 'message')); // true
     *      })();
     *      ```
     *
     *      Example 2 - an HMAC key with an explicit length:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const key = await crypto.subtle.generateKey(
     *              { name: 'HMAC', hash: 'SHA-256', length: 256 }, true, ['sign']);
     *          console.log(key.type, key.algorithm.length); // secret 256
     *          console.log((await crypto.subtle.exportKey('raw', key)).byteLength); // 32
     *      })();
     *      ```
     *
     *      @param algorithm the algorithm name or an object with the parameters of the key
     *      @param extractable whether the generated private or secret key can be exported later
     *      @param usages the operations the key may perform; see the module Concepts
     *      @return a CryptoKey, or a `{ publicKey, privateKey }` object for a key pair
     *
     */
    function generateKeySync(algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): any;

    /**
     * @description Generates a new key or key pair
     *
     *      The algorithm selects the key type and its parameters:
     *      - `ECDSA` with namedCurve - an EC signing key pair;
     *      - `Ed25519` - an Ed25519 signing key pair (no extra parameter);
     *      - `ECDH` with namedCurve - an EC key-agreement key pair;
     *      - `HMAC` with hash, and optionally length in bits - one secret key.
     *
     *      A CryptoKey is returned for HMAC and a plain object with `publicKey` and `privateKey`
     *      properties for ECDSA, Ed25519 and ECDH. The private key is extractable only when the
     *      extractable argument is true; public keys are always extractable. usages is the
     *      allow-list the key will accept: ECDSA and Ed25519 need 'sign' (the pair then gets
     *      'verify' on the public key); ECDH needs 'deriveKey' or 'deriveBits'; HMAC accepts
     *      'sign' and/or 'verify'. An HMAC key generated without length uses the hash block size
     *      (512 bits for SHA-256), reported in `key.algorithm.length`. Unsupported names, curves,
     *      hashes or usages reject with Error 20024.
     *
     *      Example 1 - generate an Ed25519 pair and use it:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'Ed25519' }, true, ['sign', 'verify']);
     *          console.log(pair.privateKey.type, pair.publicKey.type); // private public
     *          console.log(pair.privateKey.algorithm.name, pair.publicKey.usages.length);
     *          // Ed25519 1
     *
     *          const signature = await crypto.subtle.sign('Ed25519', pair.privateKey, 'message');
     *          console.log(signature.byteLength); // 64
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, signature, 'message')); // true
     *      })();
     *      ```
     *
     *      Example 2 - an HMAC key with an explicit length:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const key = await crypto.subtle.generateKey(
     *              { name: 'HMAC', hash: 'SHA-256', length: 256 }, true, ['sign']);
     *          console.log(key.type, key.algorithm.length); // secret 256
     *          console.log((await crypto.subtle.exportKey('raw', key)).byteLength); // 32
     *      })();
     *      ```
     *
     *      @param algorithm the algorithm name or an object with the parameters of the key
     *      @param extractable whether the generated private or secret key can be exported later
     *      @param usages the operations the key may perform; see the module Concepts
     *      @return a CryptoKey, or a `{ publicKey, privateKey }` object for a key pair
     *
     */
    function generateKeyAsync(algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Promise<any>;

    /**
     * @description Imports existing key material as a CryptoKey
     *
     *      The keyData depends on the format:
     *      - `raw` - the key bytes as a Buffer, typed array or ArrayBuffer: the uncompressed EC
     *        point, the 32-byte Ed25519 public key or the HMAC secret; the algorithm's namedCurve
     *        is required for EC keys and must match the data;
     *      - `pkcs8` - a DER private key; `spki` - a DER public key;
     *      - `jwk` - the object exported by exportKey (or a compatible one from Node.js); `kty` is
     *        required.
     *
     *      The algorithm must match the material: an EC key on a different curve is rejected.
     *      usages must be consistent with the key type: a private key needs 'sign' and must not
     *      have 'verify'; a public key needs 'verify' and must not have 'sign'; ECDH private keys
     *      need 'deriveKey' or 'deriveBits' while ECDH public keys must have no usages; HMAC keys
     *      need 'sign' or 'verify'. Violations and malformed material reject with Error 20024.
     *
     *      Example - import a fixed HMAC key and match the RFC 4231 test vector:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const key = await crypto.subtle.importKey('raw', Buffer.from('0b'.repeat(20), 'hex'),
     *              { name: 'HMAC', hash: 'SHA-256' }, true, ['sign']);
     *
     *          const signature = await crypto.subtle.sign('HMAC', key, 'Hi There');
     *          console.log(Buffer.from(signature).toString('hex'));
     *          // b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7
     *      })();
     *      ```
     *
     *      @param format the import format: 'raw', 'pkcs8', 'spki' or 'jwk'
     *      @param keyData the key material (Buffer, typed array, ArrayBuffer, or an object for jwk)
     *      @param algorithm the algorithm name or an object describing the imported key
     *      @param extractable whether the imported key can later be exported
     *      @param usages the operations the key may perform
     *      @return the imported CryptoKey
     *
     */
    function importKey(format: string, keyData: any, algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Promise<Class_CryptoKey>;

    /**
     * @description Imports existing key material as a CryptoKey
     *
     *      The keyData depends on the format:
     *      - `raw` - the key bytes as a Buffer, typed array or ArrayBuffer: the uncompressed EC
     *        point, the 32-byte Ed25519 public key or the HMAC secret; the algorithm's namedCurve
     *        is required for EC keys and must match the data;
     *      - `pkcs8` - a DER private key; `spki` - a DER public key;
     *      - `jwk` - the object exported by exportKey (or a compatible one from Node.js); `kty` is
     *        required.
     *
     *      The algorithm must match the material: an EC key on a different curve is rejected.
     *      usages must be consistent with the key type: a private key needs 'sign' and must not
     *      have 'verify'; a public key needs 'verify' and must not have 'sign'; ECDH private keys
     *      need 'deriveKey' or 'deriveBits' while ECDH public keys must have no usages; HMAC keys
     *      need 'sign' or 'verify'. Violations and malformed material reject with Error 20024.
     *
     *      Example - import a fixed HMAC key and match the RFC 4231 test vector:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const key = await crypto.subtle.importKey('raw', Buffer.from('0b'.repeat(20), 'hex'),
     *              { name: 'HMAC', hash: 'SHA-256' }, true, ['sign']);
     *
     *          const signature = await crypto.subtle.sign('HMAC', key, 'Hi There');
     *          console.log(Buffer.from(signature).toString('hex'));
     *          // b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7
     *      })();
     *      ```
     *
     *      @param format the import format: 'raw', 'pkcs8', 'spki' or 'jwk'
     *      @param keyData the key material (Buffer, typed array, ArrayBuffer, or an object for jwk)
     *      @param algorithm the algorithm name or an object describing the imported key
     *      @param extractable whether the imported key can later be exported
     *      @param usages the operations the key may perform
     *      @return the imported CryptoKey
     *
     */
    function importKeySync(format: string, keyData: any, algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Class_CryptoKey;

    /**
     * @description Imports existing key material as a CryptoKey
     *
     *      The keyData depends on the format:
     *      - `raw` - the key bytes as a Buffer, typed array or ArrayBuffer: the uncompressed EC
     *        point, the 32-byte Ed25519 public key or the HMAC secret; the algorithm's namedCurve
     *        is required for EC keys and must match the data;
     *      - `pkcs8` - a DER private key; `spki` - a DER public key;
     *      - `jwk` - the object exported by exportKey (or a compatible one from Node.js); `kty` is
     *        required.
     *
     *      The algorithm must match the material: an EC key on a different curve is rejected.
     *      usages must be consistent with the key type: a private key needs 'sign' and must not
     *      have 'verify'; a public key needs 'verify' and must not have 'sign'; ECDH private keys
     *      need 'deriveKey' or 'deriveBits' while ECDH public keys must have no usages; HMAC keys
     *      need 'sign' or 'verify'. Violations and malformed material reject with Error 20024.
     *
     *      Example - import a fixed HMAC key and match the RFC 4231 test vector:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const key = await crypto.subtle.importKey('raw', Buffer.from('0b'.repeat(20), 'hex'),
     *              { name: 'HMAC', hash: 'SHA-256' }, true, ['sign']);
     *
     *          const signature = await crypto.subtle.sign('HMAC', key, 'Hi There');
     *          console.log(Buffer.from(signature).toString('hex'));
     *          // b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7
     *      })();
     *      ```
     *
     *      @param format the import format: 'raw', 'pkcs8', 'spki' or 'jwk'
     *      @param keyData the key material (Buffer, typed array, ArrayBuffer, or an object for jwk)
     *      @param algorithm the algorithm name or an object describing the imported key
     *      @param extractable whether the imported key can later be exported
     *      @param usages the operations the key may perform
     *      @return the imported CryptoKey
     *
     */
    function importKeyAsync(format: string, keyData: any, algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Promise<Class_CryptoKey>;

    /**
     * @description Signs data with a private or secret key and resolves to the signature
     *
     *      The key must include 'sign' in its usages. Parameters per algorithm:
     *      - `ECDSA`: `hash` is required when the algorithm is an object (a string such as
     *        'SHA-256' or an object with a `name`) and may be any digest OpenSSL knows; the result
     *        is the raw P1363 r||s pair, 64 bytes for P-256;
     *      - `Ed25519`: no parameters; the signature is 64 bytes;
     *      - `HMAC`: the hash comes from the key, so an algorithm object may omit it and a `hash`
     *        member is ignored.
     *
     *      fibjs also accepts the algorithm as a plain string. In that form the parameters come
     *      from the key: HMAC and Ed25519 use the key's hash, and ECDSA signs the data without a
     *      pre-hash (a fibjs extension; pass `{ name: 'ECDSA', hash: '...' }` for the standard
     *      behavior, otherwise the object form throws a TypeError when hash is missing).
     *
     *      Example - sign with ECDSA and repeat through the blocking alias:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'ECDSA', namedCurve: 'P-256' }, false, ['sign', 'verify']);
     *          const data = 'a message';
     *
     *          const signature = await crypto.subtle.sign(
     *              { name: 'ECDSA', hash: 'SHA-256' }, pair.privateKey, data);
     *          const same = crypto.subtle.signSync(
     *              { name: 'ECDSA', hash: 'SHA-256' }, pair.privateKey, data);
     *          console.log(signature.byteLength, same.byteLength); // 64 64
     *      })();
     *      ```
     *
     *      @param algorithm the signing algorithm and its parameters
     *      @param key the signing key; it must have the 'sign' usage
     *      @param data the data to sign (Buffer, typed array, ArrayBuffer or utf8 string)
     *      @return the signature as an ArrayBuffer
     *
     */
    function sign(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, data: Class_Buffer | string): Promise<ArrayBuffer>;

    /**
     * @description Signs data with a private or secret key and resolves to the signature
     *
     *      The key must include 'sign' in its usages. Parameters per algorithm:
     *      - `ECDSA`: `hash` is required when the algorithm is an object (a string such as
     *        'SHA-256' or an object with a `name`) and may be any digest OpenSSL knows; the result
     *        is the raw P1363 r||s pair, 64 bytes for P-256;
     *      - `Ed25519`: no parameters; the signature is 64 bytes;
     *      - `HMAC`: the hash comes from the key, so an algorithm object may omit it and a `hash`
     *        member is ignored.
     *
     *      fibjs also accepts the algorithm as a plain string. In that form the parameters come
     *      from the key: HMAC and Ed25519 use the key's hash, and ECDSA signs the data without a
     *      pre-hash (a fibjs extension; pass `{ name: 'ECDSA', hash: '...' }` for the standard
     *      behavior, otherwise the object form throws a TypeError when hash is missing).
     *
     *      Example - sign with ECDSA and repeat through the blocking alias:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'ECDSA', namedCurve: 'P-256' }, false, ['sign', 'verify']);
     *          const data = 'a message';
     *
     *          const signature = await crypto.subtle.sign(
     *              { name: 'ECDSA', hash: 'SHA-256' }, pair.privateKey, data);
     *          const same = crypto.subtle.signSync(
     *              { name: 'ECDSA', hash: 'SHA-256' }, pair.privateKey, data);
     *          console.log(signature.byteLength, same.byteLength); // 64 64
     *      })();
     *      ```
     *
     *      @param algorithm the signing algorithm and its parameters
     *      @param key the signing key; it must have the 'sign' usage
     *      @param data the data to sign (Buffer, typed array, ArrayBuffer or utf8 string)
     *      @return the signature as an ArrayBuffer
     *
     */
    function signSync(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, data: Class_Buffer | string): ArrayBuffer;

    /**
     * @description Signs data with a private or secret key and resolves to the signature
     *
     *      The key must include 'sign' in its usages. Parameters per algorithm:
     *      - `ECDSA`: `hash` is required when the algorithm is an object (a string such as
     *        'SHA-256' or an object with a `name`) and may be any digest OpenSSL knows; the result
     *        is the raw P1363 r||s pair, 64 bytes for P-256;
     *      - `Ed25519`: no parameters; the signature is 64 bytes;
     *      - `HMAC`: the hash comes from the key, so an algorithm object may omit it and a `hash`
     *        member is ignored.
     *
     *      fibjs also accepts the algorithm as a plain string. In that form the parameters come
     *      from the key: HMAC and Ed25519 use the key's hash, and ECDSA signs the data without a
     *      pre-hash (a fibjs extension; pass `{ name: 'ECDSA', hash: '...' }` for the standard
     *      behavior, otherwise the object form throws a TypeError when hash is missing).
     *
     *      Example - sign with ECDSA and repeat through the blocking alias:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'ECDSA', namedCurve: 'P-256' }, false, ['sign', 'verify']);
     *          const data = 'a message';
     *
     *          const signature = await crypto.subtle.sign(
     *              { name: 'ECDSA', hash: 'SHA-256' }, pair.privateKey, data);
     *          const same = crypto.subtle.signSync(
     *              { name: 'ECDSA', hash: 'SHA-256' }, pair.privateKey, data);
     *          console.log(signature.byteLength, same.byteLength); // 64 64
     *      })();
     *      ```
     *
     *      @param algorithm the signing algorithm and its parameters
     *      @param key the signing key; it must have the 'sign' usage
     *      @param data the data to sign (Buffer, typed array, ArrayBuffer or utf8 string)
     *      @return the signature as an ArrayBuffer
     *
     */
    function signAsync(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, data: Class_Buffer | string): Promise<ArrayBuffer>;

    /**
     * @description Verifies a signature and resolves to true or false
     *
     *      The algorithm, key and data arguments mirror sign: the key must include 'verify' in its
     *      usages and the signature must use the same encoding (P1363 for ECDSA, the raw 64 bytes
     *      for Ed25519, the key's hash for HMAC). A signature with the wrong length or corrupted
     *      bytes resolves to false instead of throwing, so callers normally do not need to catch;
     *      a key without the 'verify' usage, an unsupported algorithm or a key from a different
     *      algorithm family rejects with Error 20024.
     *
     *      Example - a valid signature is true and a tampered one is false:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'Ed25519' }, false, ['sign', 'verify']);
     *          const signature = await crypto.subtle.sign('Ed25519', pair.privateKey, 'message');
     *
     *          const tampered = Buffer.from(signature);
     *          tampered[0] ^= 0xff;
     *
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, signature, 'message')); // true
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, tampered, 'message')); // false
     *      })();
     *      ```
     *
     *      @param algorithm the signing algorithm and its parameters
     *      @param key the verification key; it must have the 'verify' usage
     *      @param signature the signature bytes (Buffer, typed array, ArrayBuffer or utf8 string)
     *      @param data the data whose signature is verified
     *      @return whether the signature is valid for the data and the key
     *
     */
    function verify(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, signature: Class_Buffer | string, data: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Verifies a signature and resolves to true or false
     *
     *      The algorithm, key and data arguments mirror sign: the key must include 'verify' in its
     *      usages and the signature must use the same encoding (P1363 for ECDSA, the raw 64 bytes
     *      for Ed25519, the key's hash for HMAC). A signature with the wrong length or corrupted
     *      bytes resolves to false instead of throwing, so callers normally do not need to catch;
     *      a key without the 'verify' usage, an unsupported algorithm or a key from a different
     *      algorithm family rejects with Error 20024.
     *
     *      Example - a valid signature is true and a tampered one is false:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'Ed25519' }, false, ['sign', 'verify']);
     *          const signature = await crypto.subtle.sign('Ed25519', pair.privateKey, 'message');
     *
     *          const tampered = Buffer.from(signature);
     *          tampered[0] ^= 0xff;
     *
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, signature, 'message')); // true
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, tampered, 'message')); // false
     *      })();
     *      ```
     *
     *      @param algorithm the signing algorithm and its parameters
     *      @param key the verification key; it must have the 'verify' usage
     *      @param signature the signature bytes (Buffer, typed array, ArrayBuffer or utf8 string)
     *      @param data the data whose signature is verified
     *      @return whether the signature is valid for the data and the key
     *
     */
    function verifySync(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, signature: Class_Buffer | string, data: Class_Buffer | string): boolean;

    /**
     * @description Verifies a signature and resolves to true or false
     *
     *      The algorithm, key and data arguments mirror sign: the key must include 'verify' in its
     *      usages and the signature must use the same encoding (P1363 for ECDSA, the raw 64 bytes
     *      for Ed25519, the key's hash for HMAC). A signature with the wrong length or corrupted
     *      bytes resolves to false instead of throwing, so callers normally do not need to catch;
     *      a key without the 'verify' usage, an unsupported algorithm or a key from a different
     *      algorithm family rejects with Error 20024.
     *
     *      Example - a valid signature is true and a tampered one is false:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const pair = await crypto.subtle.generateKey(
     *              { name: 'Ed25519' }, false, ['sign', 'verify']);
     *          const signature = await crypto.subtle.sign('Ed25519', pair.privateKey, 'message');
     *
     *          const tampered = Buffer.from(signature);
     *          tampered[0] ^= 0xff;
     *
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, signature, 'message')); // true
     *          console.log(await crypto.subtle.verify(
     *              'Ed25519', pair.publicKey, tampered, 'message')); // false
     *      })();
     *      ```
     *
     *      @param algorithm the signing algorithm and its parameters
     *      @param key the verification key; it must have the 'verify' usage
     *      @param signature the signature bytes (Buffer, typed array, ArrayBuffer or utf8 string)
     *      @param data the data whose signature is verified
     *      @return whether the signature is valid for the data and the key
     *
     */
    function verifyAsync(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, signature: Class_Buffer | string, data: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Derives a shared secret with ECDH key agreement
     *
     *      ECDH is the only supported algorithm: `algorithm.name` is 'ECDH' and
     *      `algorithm.public` carries the peer public key; baseKey must be the matching private
     *      key with 'deriveKey' or 'deriveBits' in its usages. Both sides of the exchange produce
     *      the same bytes: deriveBits with bob.publicKey on alice.privateKey equals deriveBits
     *      with alice.publicKey on bob.privateKey.
     *
     *      length is the number of bits to return. When it is omitted, null or negative the whole
     *      shared secret is returned (32 bytes for P-256, 66 bytes for P-521). A positive length
     *      truncates the result to length / 8 bytes and masks the trailing bits when length is
     *      not a multiple of 8, as the standard requires; a length larger than the secret, a
     *      missing public key or a peer key on another curve rejects with Error 20024.
     *
     *      Example - the full secret and a truncated half share the same prefix:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const alice = await crypto.subtle.generateKey(
     *              { name: 'ECDH', namedCurve: 'P-256' }, false, ['deriveBits']);
     *          const bob = await crypto.subtle.generateKey(
     *              { name: 'ECDH', namedCurve: 'P-256' }, false, ['deriveBits']);
     *
     *          const full = await crypto.subtle.deriveBits(
     *              { name: 'ECDH', public: bob.publicKey }, alice.privateKey);
     *          const half = await crypto.subtle.deriveBits(
     *              { name: 'ECDH', public: bob.publicKey }, alice.privateKey, 128);
     *
     *          console.log(full.byteLength, half.byteLength); // 32 16
     *          console.log(Buffer.from(full).slice(0, 16).equals(Buffer.from(half))); // true
     *      })();
     *      ```
     *
     *      @param algorithm an object with name 'ECDH' and the peer public key in public
     *      @param baseKey the private ECDH key with deriveKey or deriveBits usage
     *      @param length the number of bits to derive; the whole secret when omitted or -1
     *      @return the derived bits as an ArrayBuffer
     *
     */
    function deriveBits(algorithm: FIBJS.GeneralObject | string, baseKey: Class_CryptoKey, length?: number): Promise<ArrayBuffer>;

    /**
     * @description Derives a shared secret with ECDH key agreement
     *
     *      ECDH is the only supported algorithm: `algorithm.name` is 'ECDH' and
     *      `algorithm.public` carries the peer public key; baseKey must be the matching private
     *      key with 'deriveKey' or 'deriveBits' in its usages. Both sides of the exchange produce
     *      the same bytes: deriveBits with bob.publicKey on alice.privateKey equals deriveBits
     *      with alice.publicKey on bob.privateKey.
     *
     *      length is the number of bits to return. When it is omitted, null or negative the whole
     *      shared secret is returned (32 bytes for P-256, 66 bytes for P-521). A positive length
     *      truncates the result to length / 8 bytes and masks the trailing bits when length is
     *      not a multiple of 8, as the standard requires; a length larger than the secret, a
     *      missing public key or a peer key on another curve rejects with Error 20024.
     *
     *      Example - the full secret and a truncated half share the same prefix:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const alice = await crypto.subtle.generateKey(
     *              { name: 'ECDH', namedCurve: 'P-256' }, false, ['deriveBits']);
     *          const bob = await crypto.subtle.generateKey(
     *              { name: 'ECDH', namedCurve: 'P-256' }, false, ['deriveBits']);
     *
     *          const full = await crypto.subtle.deriveBits(
     *              { name: 'ECDH', public: bob.publicKey }, alice.privateKey);
     *          const half = await crypto.subtle.deriveBits(
     *              { name: 'ECDH', public: bob.publicKey }, alice.privateKey, 128);
     *
     *          console.log(full.byteLength, half.byteLength); // 32 16
     *          console.log(Buffer.from(full).slice(0, 16).equals(Buffer.from(half))); // true
     *      })();
     *      ```
     *
     *      @param algorithm an object with name 'ECDH' and the peer public key in public
     *      @param baseKey the private ECDH key with deriveKey or deriveBits usage
     *      @param length the number of bits to derive; the whole secret when omitted or -1
     *      @return the derived bits as an ArrayBuffer
     *
     */
    function deriveBitsSync(algorithm: FIBJS.GeneralObject | string, baseKey: Class_CryptoKey, length?: number): ArrayBuffer;

    /**
     * @description Derives a shared secret with ECDH key agreement
     *
     *      ECDH is the only supported algorithm: `algorithm.name` is 'ECDH' and
     *      `algorithm.public` carries the peer public key; baseKey must be the matching private
     *      key with 'deriveKey' or 'deriveBits' in its usages. Both sides of the exchange produce
     *      the same bytes: deriveBits with bob.publicKey on alice.privateKey equals deriveBits
     *      with alice.publicKey on bob.privateKey.
     *
     *      length is the number of bits to return. When it is omitted, null or negative the whole
     *      shared secret is returned (32 bytes for P-256, 66 bytes for P-521). A positive length
     *      truncates the result to length / 8 bytes and masks the trailing bits when length is
     *      not a multiple of 8, as the standard requires; a length larger than the secret, a
     *      missing public key or a peer key on another curve rejects with Error 20024.
     *
     *      Example - the full secret and a truncated half share the same prefix:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      (async () => {
     *          const alice = await crypto.subtle.generateKey(
     *              { name: 'ECDH', namedCurve: 'P-256' }, false, ['deriveBits']);
     *          const bob = await crypto.subtle.generateKey(
     *              { name: 'ECDH', namedCurve: 'P-256' }, false, ['deriveBits']);
     *
     *          const full = await crypto.subtle.deriveBits(
     *              { name: 'ECDH', public: bob.publicKey }, alice.privateKey);
     *          const half = await crypto.subtle.deriveBits(
     *              { name: 'ECDH', public: bob.publicKey }, alice.privateKey, 128);
     *
     *          console.log(full.byteLength, half.byteLength); // 32 16
     *          console.log(Buffer.from(full).slice(0, 16).equals(Buffer.from(half))); // true
     *      })();
     *      ```
     *
     *      @param algorithm an object with name 'ECDH' and the peer public key in public
     *      @param baseKey the private ECDH key with deriveKey or deriveBits usage
     *      @param length the number of bits to derive; the whole secret when omitted or -1
     *      @return the derived bits as an ArrayBuffer
     *
     */
    function deriveBitsAsync(algorithm: FIBJS.GeneralObject | string, baseKey: Class_CryptoKey, length?: number): Promise<ArrayBuffer>;

}

