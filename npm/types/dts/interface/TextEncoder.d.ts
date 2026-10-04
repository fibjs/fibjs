/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description TextEncoder encoding object
 *
 *  Creation method
 *  ```JavaScript
 *  var textEncoder = new util.TextEncoder('utf8');
 *  ```
 *
 */
declare class Class_TextEncoder extends Class_object {
    /**
     * @description TextEncoder object constructor, constructed with parameters
     * 	 @param codec the encoding charset
     *  	 @param opts encoding options
     *
     */
    constructor(codec?: string, opts?: FIBJS.GeneralObject);

    /**
     * @description Converts text to binary data
     * 	 @param data the text to convert
     *  	 @param opts encoding options
     * 	 @return returns the encoded binary data
     *
     */
    encode(data?: string, opts?: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description Encodes text into the destination buffer
     * 	 @param source the text to encode
     * 	 @param destination the destination buffer to write into
     * 	 @return returns an object containing the read and written properties
     *
     */
    encodeInto(source: string, destination: Class_Buffer): {
        read: number;
        written: number;
    };

    /**
     * @description Queries the current encoding charset
     */
    readonly encoding: string;

}

