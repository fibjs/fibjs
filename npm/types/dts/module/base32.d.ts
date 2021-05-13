/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description base32 编码与解码模块
 *  引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  var base32 = encoding.base32;
 *  ```
 *  或者
 *  ```JavaScript
 *  var base32 = require('base32');
 *  ```
 *  
 */
declare module 'base32' {
    /**
     * @description 以 base32 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: Class_Buffer): string;

    /**
     * @description 以 base32 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string): Class_Buffer;

}

