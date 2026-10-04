/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description TextDecoder decoding object
 *
 *  Creation method
 *  ```JavaScript
 *  var textDecoder = new util.TextDecoder('utf8');
 *  ```
 *
 */
declare class Class_TextDecoder extends Class_object {
    /**
     * @description TextDecoder object constructor, constructed with parameters
     * 	 @param codec the decoding charset
     *  	 @param opts decoding options
     *
     */
    constructor(codec?: string, opts?: FIBJS.GeneralObject);

    /**
     * @description Converts binary data to text; a string data is encoded as utf8
     *      @param data the binary to convert, a string is encoded as utf8
     *      @param opts decoding options
     *      @return returns the decoded text
     *
     */
    decode(data: Class_Buffer | string, opts?: FIBJS.GeneralObject): string;

    /**
     * @description Converts binary data to text
     * 	 @return returns the decoded text
     *
     */
    decode(): string;

    /**
     * @description Queries the current encoding charset
     */
    readonly encoding: string;

    /**
     * @description Queries whether decoding errors throw an exception
     */
    readonly fatal: boolean;

    /**
     * @description Queries whether the BOM is ignored
     */
    readonly ignoreBOM: boolean;

}

