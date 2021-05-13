/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description base64 编码与解码模块
 *  引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  var base64 = encoding.base64;
 *  ```
 *  或者
 *  ```JavaScript
 *  var base64 = require('base64');
 *  ```
 *  
 */
declare module 'base64' {
    /**
     * @description 以 base64 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @param url 指定是否使用 url 安全字符编码
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: Class_Buffer, url?: boolean): string;

    /**
     * @description 以 base64 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string): Class_Buffer;

}

