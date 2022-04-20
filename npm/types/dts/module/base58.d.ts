/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description base58 编码与解码模块
 *  引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  var base58 = encoding.base58;
 *  ```
 *  或者
 *  ```JavaScript
 *  var base58 = require('base58');
 *  ```
 *  
 */
declare module 'base58' {
    /**
     * @description 以 base58 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: Class_Buffer): string;

    /**
     * @description 以 base58check 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @param chk_ver 指定校验版本
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: Class_Buffer, chk_ver: number): string;

    /**
     * @description 以 base58 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string): Class_Buffer;

    /**
     * @description 以 base58check 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @param chk_ver 指定校验版本
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string, chk_ver: number): Class_Buffer;

}

