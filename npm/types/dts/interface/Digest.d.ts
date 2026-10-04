/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Message digest object
 *
 * A Digest object can be used like this:
 *
 * ```
 * const crypto = require('crypto');
 * // create a SHA-512 digest object
 * const digest = crypto.createHash('sha512');
 * // update digest with data
 * digest.update('hello');
 * digest.update('world');
 * // get digest result
 * const result = digest.digest();
 * console.log(result);
 *
 * // output result in hex and base64
 * console.log(result.toString('hex'));
 * console.log(result.toString('base64'));
 * ```
 * In the code above, a SHA-512 digest object is created with the `crypto.createHash()` method; data to be digested can be added incrementally with the `update()` method, and the digest result is obtained with the `digest()` method.
 *
 */
declare class Class_Digest extends Class_object {
    /**
     * @description Updates the digest information with the given data
     *      @param data the data block, or a string decoded with codec
     *      @param codec the encoding format; allowed values are: "buffer", "hex", "base32", "base58", "base64", "utf8", or a character set supported by the iconv module
     *      @return returns the message digest object itself
     *
     */
    update(data: Class_Buffer | string, codec?: string): Class_Digest;

    /**
     * @description Computes and returns the digest
     *      @param codec the encoding format; allowed values are: "buffer", "hex", "base32", "base58", "base64", "utf8", or a character set supported by the iconv module
     *      @return returns the digest representation in the specified encoding
     *
     */
    digest(codec?: string): any;

    /**
     * @description Queries the digest size in bytes of the current message digest algorithm
     */
    readonly size: number;

}

