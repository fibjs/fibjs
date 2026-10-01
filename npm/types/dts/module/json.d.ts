/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description json encoding and decoding module
 *  To require it:
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  var json = encoding.json;
 *  ```
 *  or
 *  ```JavaScript
 *  var json = require('json');
 *  ```
 *
 */
declare module 'json' {
    /**
     * @description Encodes a variable in json format
     * 	 @param data the variable to encode
     * 	 @return returns the encoded string
     *
     */
    function encode(data: any): string;

    /**
     * @description Decodes a string into a variable using json
     * 	 @param data the string to decode
     * 	 @return returns the decoded variable
     *
     */
    function decode(data: string): any;

}

