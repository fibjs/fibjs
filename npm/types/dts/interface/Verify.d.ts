/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description Streaming signature verifier
 *
 *  A Verify checks a signature over data fed with update(); crypto.createVerify(algorithm)
 *  is the factory and crypto.verify(algorithm, data, key, signature) the one-shot
 *  equivalent. The signer side is Sign or crypto.sign.
 *
 *  Concepts:
 *  - **Algorithm selection**: the algorithm is a digest name from crypto.getHashes and
 *    must match the one used to sign. The key type selects the scheme (RSA PKCS#1 v1.5
 *    or PSS, ECDSA/DSA with DER or IEEE P1363 signatures), exactly as for Sign.
 *  - **Keys**: the key argument accepts a KeyObject, a PEM/DER string or Buffer, or an
 *    options object. Because a public key can be derived from a private key, a private
 *    key is also accepted and verifies with its public part; a secret key is rejected
 *    with "Verify: invalid key type, expected a public or private key". The parameter
 *    is named privateKey for historical reasons.
 *  - **Signatures**: a Buffer or a string decoded with encoding. The default encoding
 *    "buffer" is not a character set, so a string signature must pass an explicit
 *    encoding ('hex', 'base64', 'utf8'); Node.js decodes a string as utf8 when no
 *    encoding is given. Ed25519/Ed448 are one-shot algorithms and are rejected by this
 *    class; use crypto.verify(null, data, key, signature) for them.
 *  - **Result**: verify() returns true or false and finalizes the object; a malformed
 *    signature normally returns false, while an invalid key or an unsupported algorithm
 *    throws. Create a new Verify for each check, as Node.js requires.
 *
 *  Obtained from:
 *  - `crypto.createVerify(algorithm)` — the streaming factory; its optional options are
 *    not used by fibjs;
 *  - `crypto.verify(algorithm, data, key, signature[, callback])` — one-shot
 *    verification, required for Ed25519/Ed448.
 *
 *  Example 1 — RSA: valid, tampered data and a wrong signature:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const { privateKey, publicKey } = crypto.generateKeyPairSync('rsa', {
 *      modulusLength: 1024
 *  });
 *  const signature = crypto.createSign('SHA256').update('data').sign(privateKey);
 *
 *  console.log(crypto.createVerify('SHA256').update('data').verify(publicKey, signature));
 *  // true
 *  console.log(crypto.createVerify('SHA256').update('data!').verify(publicKey, signature));
 *  // false
 *  console.log(crypto.createVerify('SHA256').update('data')
 *      .verify(publicKey, Buffer.alloc(signature.length))); // false
 *  ```
 *
 *  Example 2 — verifying with a PEM public key:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const { privateKey, publicKey } = crypto.generateKeyPairSync('rsa', {
 *      modulusLength: 1024
 *  });
 *  const publicPem = publicKey.export({ format: 'pem', type: 'spki' });
 *  const signature = crypto.createSign('SHA256').update('data').sign(privateKey);
 *
 *  // The verifier never needs the private key, only the public PEM text.
 *  console.log(crypto.createVerify('SHA256').update('data').verify(publicPem, signature));
 *  // true
 *  ```
 *
 *  Example 3 — ECDSA IEEE P1363 and a private key as the verification key:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const { privateKey, publicKey } = crypto.generateKeyPairSync('ec', {
 *      namedCurve: 'prime256v1'
 *  });
 *  const signature = crypto.createSign('SHA256').update('data')
 *      .sign({ key: privateKey, dsaEncoding: 'ieee-p1363' });
 *
 *  const valid = crypto.createVerify('SHA256').update('data')
 *      .verify({ key: publicKey, dsaEncoding: 'ieee-p1363' }, signature);
 *  console.log(valid); // true
 *
 *  // A private key is also accepted: its public part is derived for the check.
 *  const same = crypto.createVerify('SHA256').update('data')
 *      .verify({ key: privateKey, dsaEncoding: 'ieee-p1363' }, signature);
 *  console.log(same); // true
 *  ```
 *
 */
declare class Class_Verify extends Class_object {
    /**
     * @description Updates the Verify content with the given data
     *
     *      data may be a Buffer or a string decoded with codec (default "utf8"); a Buffer
     *      ignores codec. Returns the Verify object, so calls can be chained, and it may be
     *      called any number of times before verify(). An unknown codec throws "encoding:
     *      Unknown charset". Calling update() after verify() is not supported; discard the
     *      object and create a new Verify instead.
     *
     *      @param data the data to update with
     *      @param codec the encoding of a string data, default "utf8"
     *      @return returns the Verify object itself
     *
     */
    update(data: Class_Buffer | string, codec?: string): Class_Verify;

    /**
     * @description Verifies the signature of all the data passed in
     *
     *      privateKey accepts a KeyObject, a PEM/DER string or Buffer, or an options object
     *      with the same entries as Sign.sign (key, format, type, passphrase, dsaEncoding,
     *      padding, saltLength); a private key is accepted because its public part can be
     *      derived. Signing and verification must use the same dsaEncoding and RSA padding.
     *
     *      signature may be a Buffer or a string decoded with encoding; passing a string
     *      without an explicit encoding throws "encoding: Unknown charset: 'buffer'", which
     *      differs from Node.js (utf8), so decode the string yourself or pass 'hex',
     *      'base64' or 'utf8'. The call finalizes the object and returns true when the
     *      signature is valid, false for a wrong signature; an invalid key or an
     *      unsupported algorithm throws.
     *
     *      Example: a base64 string signature verifies with the matching encoding:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { privateKey, publicKey } = crypto.generateKeyPairSync('rsa', {
     *          modulusLength: 1024
     *      });
     *      const signature = crypto.createSign('SHA256').update('data').sign(privateKey, 'base64');
     *
     *      // The string form needs the encoding explicitly.
     *      const valid = crypto.createVerify('SHA256').update('data')
     *          .verify(publicKey, signature, 'base64');
     *      console.log(valid); // true
     *
     *      // Decoding manually avoids the encoding rule.
     *      const same = crypto.createVerify('SHA256').update('data')
     *          .verify(publicKey, Buffer.from(signature, 'base64'));
     *      console.log(same); // true
     *      ```
     *
     *      @param privateKey the public key used for verification
     *      @param signature the signature to verify
     *      @param encoding the encoding of a string signature, default "buffer"
     *      @return returns true if the signature is valid, false otherwise
     *
     */
    verify(privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string, encoding?: string): boolean;

}

