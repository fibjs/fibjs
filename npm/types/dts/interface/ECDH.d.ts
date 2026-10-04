/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The ECDH object
 *
 *
 */
declare class Class_ECDH extends Class_object {
    /**
     * @description Converts a public key to the specified format
     *      @param key the public key to convert, or a string in inputEncoding
     *      @param curve the predefined elliptic curve to use
     *      @param inputEncoding the encoding of key: 'buffer', 'hex', 'base64', 'base58'; default 'hex'
     *      @param outputEncoding the encoding of the result: 'buffer', 'hex', 'base64', 'base58'; default 'hex'
     *      @param format the format of the public key: 'compressed', 'uncompressed', 'hybrid'; default 'uncompressed'
     *      @return returns the converted public key
     *
     */
    static convertKey(key: Class_Buffer | string, curve: string, inputEncoding?: string, outputEncoding?: string, format?: string): any;

    /**
     * @description Computes the shared secret from another public key
     *      @param otherPublicKey the other party's public key, or a string in inputEncoding
     *      @param inputEncoding the encoding of otherPublicKey: 'buffer', 'hex', 'base64', 'base58'; default 'hex'
     *      @param outputEncoding the encoding of the result: 'buffer', 'hex', 'base64', 'base58'; default 'buffer'
     *      @return returns the computed shared secret
     *
     */
    computeSecret(otherPublicKey: Class_Buffer | string, inputEncoding?: string, outputEncoding?: string): any;

    /**
     * @description Generates a key pair
     *         @param outputEncoding the encoding of the result: 'buffer', 'hex', 'base64', 'base58'; default 'buffer'
     *         @param format the format of the public key: 'compressed', 'uncompressed', 'hybrid'; default 'uncompressed'
     *         @return returns the generated public key
     *
     */
    generateKeys(outputEncoding?: string, format?: string): any;

    /**
     * @description Gets the name of the elliptic curve
     *         @return returns the name of the elliptic curve
     *
     */
    readonly curveName: string;

    /**
     * @description Gets the private key
     *         @param encoding the encoding of the private key: 'buffer', 'hex', 'base64', 'base58'; default 'buffer'
     *         @return returns the private key
     *
     */
    getPrivateKey(encoding?: string): any;

    /**
     * @description Gets the public key
     *         @param encoding the encoding of the public key: 'buffer', 'hex', 'base64', 'base58'; default 'buffer'
     *         @param format the format of the public key: 'compressed', 'uncompressed', 'hybrid'; default 'uncompressed'
     *         @return returns the public key
     *
     */
    getPublicKey(encoding?: string, format?: string): any;

    /**
     * @description Sets the private key
     *      @param privateKey the private key data, or a string in encoding
     *      @param encoding the encoding of privateKey: 'buffer', 'hex', 'base64', 'base58'; default 'hex'
     *
     */
    setPrivateKey(privateKey: Class_Buffer | string, encoding?: string): void;

    /**
     * @description Sets the public key
     *      @param publicKey the public key data, or a string in encoding
     *      @param encoding the encoding of publicKey: 'buffer', 'hex', 'base64', 'base58'; default 'hex'
     *
     */
    setPublicKey(publicKey: Class_Buffer | string, encoding?: string): void;

}

