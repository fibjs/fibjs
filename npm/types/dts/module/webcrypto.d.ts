/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/CryptoKey.d.ts" />
/// <reference path="../module/subtle.d.ts" />
/**
 * @description WebCrypto API module
 *
 * The WebCrypto API module provides a set of functions for encryption and decryption. It can be obtained through the webcrypto property of the global object or require("crypto").webcrypto.
 *
 */
declare module 'webcrypto' {
    /**
     * @description Generates random numbers
     *
     *     @param data a TypedArray object used to hold the generated random numbers.
     *     @return returns the data object.
     *
     */
    function getRandomValues(data: TypedArray): TypedArray;

    /**
     * @description Generates a UUID
     *
     *     @return returns the generated UUID string.
     *
     */
    function randomUUID(): string;

    /**
     * @description The CryptoKey class represents symmetric or asymmetric keys, each exposing different features
     */
    const CryptoKey: typeof Class_CryptoKey;

    /**
     * @description Provides access to the SubtleCrypto API
     */
    const subtle: typeof import ('subtle');

}

