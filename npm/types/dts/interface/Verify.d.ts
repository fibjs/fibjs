/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description A utility for verifying signatures
 *
 *   The crypto.createVerify() method is used to create a Verify instance. Verify objects cannot be created directly with the new keyword.
 *
 *   Example:
 *   ```JavaScript
 *     const {
 *         generateKeyPairSync,
 *         createSign,
 *         createVerify,
 *     } = require('crypto');
 *
 *     const { privateKey, publicKey } = generateKeyPairSync('rsa', {
 *         modulusLength: 2048,
 *     });
 *
 *     const sign = createSign('SHA256');
 *     sign.update('some data to sign');
 *     const signature = sign.sign(privateKey);
 *
 *     const verify = createVerify('SHA256');
 *     verify.update('some data to sign');
 *     console.log(verify.verify(publicKey, signature));
 *   ```
 *
 */
declare class Class_Verify extends Class_object {
    /**
     * @description Updates the Verify content with the given data
     *      data may be a Buffer or a string; a string is decoded with codec.
     *      @param data the data to update with
     *      @param codec the encoding of a string data, default "utf8"
     *      @return returns the Verify object itself
     *
     */
    update(data: Class_Buffer | string, codec?: string): Class_Verify;

    /**
     * @description Verifies the signature of all the data passed in
     *
     *      The parameters in key are used to call crypto.createPrivateKey to create the private key object; the following signing parameters are also supported:
     *      - dsaEncoding for DSA and ECDSA, this option specifies the format of the generated signature. It can be one of the following:
     *       - 'der' (default): DER-encoded ASN.1 signature structure encoding (r, s)
     *       - 'ieee-p1363' : the signature format r || s proposed in IEEE-P1363
     *      - padding optional RSA padding value, one of the following:
     *       - RSA_PKCS1_PADDING (default)
     *       - RSA_PKCS1_PSS_PADDING; RSA_PKCS1_PSS_PADDING will use MGF1 with the same hash function as the one used to sign the message specified in RFC 4055 section 3.1
     *      - saltLength the salt length when padding is RSA_PKCS1_PSS_PADDING. The special value RSA_PSS_SALTLEN_DIGEST sets the salt length to the digest size, and RSA_PSS_SALTLEN_MAX_SIGN (default) sets it to the maximum allowed value
     *      privateKey may be a Buffer or a string in the PEM/DER form, a KeyObject, or an object with the key parameters.
     *      @param privateKey the public key used for verification
     *      signature may be a Buffer or a string; a string is decoded with encoding.
     *      @param signature the signature to verify
     *      @param encoding the encoding of a string signature, default "buffer"
     *      @return returns true if the signature is valid, false otherwise
     *
     */
    verify(privateKey: Class_Buffer | Class_KeyObject | FIBJS.GeneralObject | string, signature: Class_Buffer | string, encoding?: string): boolean;

}

