/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description Streaming signature generator
 *
 *  A Sign computes a digital signature over data fed with update() and produced by
 *  sign(privateKey), which finalizes the object. crypto.createSign(algorithm) is the
 *  factory; crypto.sign(algorithm, data, key) is the one-shot equivalent for data that
 *  fits in memory, and the verifier side is Verify or crypto.verify.
 *
 *  Concepts:
 *  - **Algorithm selection**: the algorithm is a digest name from crypto.getHashes
 *    ('sha256', 'SHA-256', 'sha1', ...), matched case-insensitively. The key type
 *    selects the scheme: RSA uses PKCS#1 v1.5 by default and PSS when the padding
 *    option says so; ECDSA/DSA sign the digest; the signature is DER-encoded unless
 *    dsaEncoding is 'ieee-p1363'.
 *  - **Key types**: RSA, DSA and EC keys sign through the streaming API.
 *    Ed25519/Ed448 are one-shot algorithms and are rejected here with "One-shot
 *    signature algorithms do not support sign"; use crypto.sign(null, data, key) with
 *    them. SM2 and Bls12381 are fibjs extensions.
 *  - **Key material**: sign() accepts a KeyObject, a PEM/DER string or Buffer, or an
 *    options object carrying the key plus the scheme options {key, format, type,
 *    passphrase, dsaEncoding, padding, saltLength}. Anything that is not a KeyObject
 *    is passed to crypto.createPrivateKey, exactly as in Node.js.
 *  - **One signature per object**: sign() is terminal, because the object already
 *    finalized its digest. Do not reuse it; create a new Sign for each signature. With
 *    RSA PKCS#1 the signature is deterministic, so the same key, data and algorithm
 *    always produce the same bytes.
 *
 *  Obtained from:
 *  - `crypto.createSign(algorithm)` — the streaming factory; its optional options are
 *    not used by fibjs;
 *  - `crypto.sign(algorithm, data, key[, callback])` — one-shot signing, required for
 *    Ed25519/Ed448.
 *
 *  Example 1 — RSA signature and verification:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  // 1024 bits keep the example fast; use 2048 or more for real keys.
 *  const { privateKey, publicKey } = crypto.generateKeyPairSync('rsa', {
 *      modulusLength: 1024
 *  });
 *
 *  const sign = crypto.createSign('SHA256');
 *  sign.update('some data to sign');
 *  const signature = sign.sign(privateKey);
 *
 *  const verify = crypto.createVerify('SHA256');
 *  verify.update('some data to sign');
 *  console.log(verify.verify(publicKey, signature)); // true
 *  ```
 *
 *  Example 2 — ECDSA with IEEE P1363 signatures:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const { privateKey, publicKey } = crypto.generateKeyPairSync('ec', {
 *      namedCurve: 'prime256v1'
 *  });
 *
 *  const signature = crypto.createSign('SHA256').update('data')
 *      .sign({ key: privateKey, dsaEncoding: 'ieee-p1363' });
 *  console.log(signature.length); // 64 for P-256 (32-byte r || 32-byte s)
 *
 *  const valid = crypto.createVerify('SHA256').update('data')
 *      .verify({ key: publicKey, dsaEncoding: 'ieee-p1363' }, signature);
 *  console.log(valid); // true
 *  ```
 *
 *  Example 3 — signing with a PEM key and a string encoding:
 *  ```JavaScript
 *  const crypto = require('crypto');
 *
 *  const { privateKey, publicKey } = crypto.generateKeyPairSync('rsa', {
 *      modulusLength: 1024
 *  });
 *  const privatePem = privateKey.export({ format: 'pem', type: 'pkcs8' });
 *  const publicPem = publicKey.export({ format: 'pem', type: 'spki' });
 *
 *  const signature = crypto.createSign('SHA256').update('data to sign')
 *      .sign(privatePem, 'hex');
 *
 *  const valid = crypto.createVerify('SHA256').update('data to sign')
 *      .verify(publicPem, signature, 'hex');
 *  console.log(valid); // true
 *  ```
 *
 */
declare class Class_Sign extends Class_object {
    /**
     * @description Updates the Sign content with the given data
     *
     *      data may be a Buffer or a string decoded with codec (default "utf8"); a Buffer
     *      ignores codec. Returns the Sign object, so calls can be chained, and it may be
     *      called any number of times before sign(). An unknown codec throws "encoding:
     *      Unknown charset". Calling update() after sign() is not supported; discard the
     *      object and create a new Sign instead, as Node.js requires.
     *
     *      @param data the data to update with
     *      @param codec the encoding of a string data, default "utf8"
     *      @return returns the Sign object itself
     *
     */
    update(data: Class_Buffer | string, codec?: string): Class_Sign;

    /**
     * @description Computes the signature of all the data passed in
     *
     *      privateKey may be a KeyObject, a PEM/DER string or Buffer (a string is decoded
     *      as utf8), or an options object that is passed to crypto.createPrivateKey; the
     *      following signing parameters are also supported in that object:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the
     *        generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same
     *         hash function as the one used to sign the message specified in RFC 4055
     *         section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special
     *        value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and
     *        RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *
     *      The call finalizes the object: a second sign() fails with an OpenSSL error
     *      (Node.js throws ERR_CRYPTO_INVALID_STATE). A secret key or a key without private
     *      material throws "Sign: invalid key type, expected a private key"; an
     *      Ed25519/Ed448 key throws "One-shot signature algorithms do not support sign".
     *      With encoding "buffer" (default) a Buffer is returned, otherwise a string; an
     *      unknown encoding throws "encoding: Unknown charset".
     *
     *      Example: RSA-PSS signing and verification with matching options:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *
     *      const { privateKey, publicKey } = crypto.generateKeyPairSync('rsa', {
     *          modulusLength: 1024
     *      });
     *
     *      const options = { key: privateKey, padding: crypto.constants.RSA_PKCS1_PSS_PADDING };
     *      const signature = crypto.createSign('SHA256').update('data').sign(options);
     *
     *      const valid = crypto.createVerify('SHA256').update('data')
     *          .verify({ key: publicKey, padding: crypto.constants.RSA_PKCS1_PSS_PADDING }, signature);
     *      console.log(valid); // true
     *      ```
     *
     *      @param privateKey the private key used for signing
     *      @param encoding the encoding of the return value
     *      @return returns the signature value
     *
     */
    sign(privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, encoding?: string): any;

}

