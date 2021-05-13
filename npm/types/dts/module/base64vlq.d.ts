/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description base64vlq 编码与解码模块
 *  引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  var base64vlq = encoding.base64vlq;
 *  ```
 *  或者
 *  ```JavaScript
 *  var base64vlq = require('base64vlq');
 *  ```
 *  
 */
declare module 'base64vlq' {
    /**
     * @description 以 base64vlq 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: number): string;

    /**
     * @description 以 base64vlq 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: any[]): string;

    /**
     * @description 以 base64vlq 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string): any[];

}

