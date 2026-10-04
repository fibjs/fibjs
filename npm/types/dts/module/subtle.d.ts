/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/CryptoKey.d.ts" />
/**
 * @description Provides access to the SubtleCrypto API
 *
 * The SubtleCrypto API module provides a set of functions for encryption and decryption. It can be obtained through the global.webcrypto.subtle property or require("crypto").webcrypto.subtle.
 *
 */
declare module 'subtle' {
    /**
     * @description Computes the hash value of the given data
     *      @param algorithm the hash algorithm to use, as an object ({ name }) or a string
     *      @param data the data to compute the hash value of; a string is encoded as utf8
     *      @return returns the computed hash value
     *
     */
    function digest(algorithm: FIBJS.GeneralObject | string, data: Class_Buffer | string): Promise<ArrayBuffer>;

    /**
     * @description Computes the hash value of the given data
     *      @param algorithm the hash algorithm to use, as an object ({ name }) or a string
     *      @param data the data to compute the hash value of; a string is encoded as utf8
     *      @return returns the computed hash value
     *
     */
    function digestSync(algorithm: FIBJS.GeneralObject | string, data: Class_Buffer | string): ArrayBuffer;

    /**
     * @description Computes the hash value of the given data
     *      @param algorithm the hash algorithm to use, as an object ({ name }) or a string
     *      @param data the data to compute the hash value of; a string is encoded as utf8
     *      @return returns the computed hash value
     *
     */
    function digestAsync(algorithm: FIBJS.GeneralObject | string, data: Class_Buffer | string): Promise<ArrayBuffer>;

    /**
     * @description Exports the key's information; returns an error if the key is not exportable
     *
     *     @param format the export format, which can be 'raw', 'pkcs8', 'spki' or 'jwk'.
     *     @param key the key to export
     *     @return returns the exported key information
     *
     */
    function exportKey(format: string, key: Class_CryptoKey): Promise<any>;

    /**
     * @description Exports the key's information; returns an error if the key is not exportable
     *
     *     @param format the export format, which can be 'raw', 'pkcs8', 'spki' or 'jwk'.
     *     @param key the key to export
     *     @return returns the exported key information
     *
     */
    function exportKeySync(format: string, key: Class_CryptoKey): any;

    /**
     * @description Exports the key's information; returns an error if the key is not exportable
     *
     *     @param format the export format, which can be 'raw', 'pkcs8', 'spki' or 'jwk'.
     *     @param key the key to export
     *     @return returns the exported key information
     *
     */
    function exportKeyAsync(format: string, key: Class_CryptoKey): Promise<any>;

    /**
     * @description Generates a new key
     *      @param algorithm the algorithm used to generate the key, as an object or a string
     *      @param extractable specifies whether the key can be exported
     *      @param usages the usages of the key
     *      @return returns the generated key
     *
     */
    function generateKey(algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Promise<any>;

    /**
     * @description Generates a new key
     *      @param algorithm the algorithm used to generate the key, as an object or a string
     *      @param extractable specifies whether the key can be exported
     *      @param usages the usages of the key
     *      @return returns the generated key
     *
     */
    function generateKeySync(algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): any;

    /**
     * @description Generates a new key
     *      @param algorithm the algorithm used to generate the key, as an object or a string
     *      @param extractable specifies whether the key can be exported
     *      @param usages the usages of the key
     *      @return returns the generated key
     *
     */
    function generateKeyAsync(algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Promise<any>;

    /**
     * @description Imports a key
     *      @param format the import format, which can be 'raw', 'pkcs8', 'spki' or 'jwk'.
     *      @param keyData the object containing the key data
     *      @param algorithm the algorithm of the key, as an object or a string
     *      @param extractable specifies whether the key can be exported
     *      @param usages the usages of the key
     *      @return returns the imported key
     *
     */
    function importKey(format: string, keyData: any, algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Promise<Class_CryptoKey>;

    /**
     * @description Imports a key
     *      @param format the import format, which can be 'raw', 'pkcs8', 'spki' or 'jwk'.
     *      @param keyData the object containing the key data
     *      @param algorithm the algorithm of the key, as an object or a string
     *      @param extractable specifies whether the key can be exported
     *      @param usages the usages of the key
     *      @return returns the imported key
     *
     */
    function importKeySync(format: string, keyData: any, algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Class_CryptoKey;

    /**
     * @description Imports a key
     *      @param format the import format, which can be 'raw', 'pkcs8', 'spki' or 'jwk'.
     *      @param keyData the object containing the key data
     *      @param algorithm the algorithm of the key, as an object or a string
     *      @param extractable specifies whether the key can be exported
     *      @param usages the usages of the key
     *      @return returns the imported key
     *
     */
    function importKeyAsync(format: string, keyData: any, algorithm: FIBJS.GeneralObject | string, extractable: boolean, usages: any[]): Promise<Class_CryptoKey>;

    /**
     * @description Signs data using the key
     *      @param algorithm the signing algorithm to use, as an object or a string
     *      @param key the key used for signing
     *      @param data the data to sign; a string is encoded as utf8
     *      @return returns the signed data
     *
     */
    function sign(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, data: Class_Buffer | string): Promise<ArrayBuffer>;

    /**
     * @description Signs data using the key
     *      @param algorithm the signing algorithm to use, as an object or a string
     *      @param key the key used for signing
     *      @param data the data to sign; a string is encoded as utf8
     *      @return returns the signed data
     *
     */
    function signSync(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, data: Class_Buffer | string): ArrayBuffer;

    /**
     * @description Signs data using the key
     *      @param algorithm the signing algorithm to use, as an object or a string
     *      @param key the key used for signing
     *      @param data the data to sign; a string is encoded as utf8
     *      @return returns the signed data
     *
     */
    function signAsync(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, data: Class_Buffer | string): Promise<ArrayBuffer>;

    /**
     * @description Verifies data using the key
     *      @param algorithm the signing algorithm to use, as an object or a string
     *      @param key the key used for verification
     *      @param signature the signature data to use; a string is encoded as utf8
     *      @param data the data to verify; a string is encoded as utf8
     *      @return returns the verification result
     *
     */
    function verify(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, signature: Class_Buffer | string, data: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Verifies data using the key
     *      @param algorithm the signing algorithm to use, as an object or a string
     *      @param key the key used for verification
     *      @param signature the signature data to use; a string is encoded as utf8
     *      @param data the data to verify; a string is encoded as utf8
     *      @return returns the verification result
     *
     */
    function verifySync(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, signature: Class_Buffer | string, data: Class_Buffer | string): boolean;

    /**
     * @description Verifies data using the key
     *      @param algorithm the signing algorithm to use, as an object or a string
     *      @param key the key used for verification
     *      @param signature the signature data to use; a string is encoded as utf8
     *      @param data the data to verify; a string is encoded as utf8
     *      @return returns the verification result
     *
     */
    function verifyAsync(algorithm: FIBJS.GeneralObject | string, key: Class_CryptoKey, signature: Class_Buffer | string, data: Class_Buffer | string): Promise<boolean>;

    /**
     * @description Derives bits from a base key
     *      @param algorithm the derivation algorithm to use, as an object or a string
     *      @param baseKey the base key used for derivation
     *      @param length the number of bits to derive
     *      @return returns the derived bits
     *
     */
    function deriveBits(algorithm: FIBJS.GeneralObject | string, baseKey: Class_CryptoKey, length?: number): Promise<ArrayBuffer>;

    /**
     * @description Derives bits from a base key
     *      @param algorithm the derivation algorithm to use, as an object or a string
     *      @param baseKey the base key used for derivation
     *      @param length the number of bits to derive
     *      @return returns the derived bits
     *
     */
    function deriveBitsSync(algorithm: FIBJS.GeneralObject | string, baseKey: Class_CryptoKey, length?: number): ArrayBuffer;

    /**
     * @description Derives bits from a base key
     *      @param algorithm the derivation algorithm to use, as an object or a string
     *      @param baseKey the base key used for derivation
     *      @param length the number of bits to derive
     *      @return returns the derived bits
     *
     */
    function deriveBitsAsync(algorithm: FIBJS.GeneralObject | string, baseKey: Class_CryptoKey, length?: number): Promise<ArrayBuffer>;

}

