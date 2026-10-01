/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description Module defining commonly used constants of the crypto module
 *
 *  Reference method:
 *  ```JavaScript
 *  var constants = require('crypto').constants
 *  ```
 *
 */
declare module 'crypto_constants' {
    /**
     * @description PKCS#1 padding, the most commonly used RSA padding
     */
    export const RSA_PKCS1_PADDING: 1;

    /**
     * @description No padding, raw RSA encryption
     */
    export const RSA_NO_PADDING: 3;

    /**
     * @description PKCS#1 OAEP padding, providing more secure encryption
     */
    export const RSA_PKCS1_OAEP_PADDING: 4;

    /**
     * @description X9.31 padding
     */
    export const RSA_X931_PADDING: 5;

    /**
     * @description PKCS#1 PSS padding, used for digital signatures
     */
    export const RSA_PKCS1_PSS_PADDING: 6;

    /**
     * @description PSS padding uses the digest length as the salt length
     */
    export const RSA_PSS_SALTLEN_DIGEST: -1;

    /**
     * @description PSS padding uses the maximum possible length as the salt length
     */
    export const RSA_PSS_SALTLEN_MAX_SIGN: -2;

    /**
     * @description PSS padding determines the salt length automatically
     */
    export const RSA_PSS_SALTLEN_AUTO: -2;

}

