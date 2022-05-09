/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description multibase 编码与解码模块
 *  引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  var multibase = encoding.multibase;
 *  ```
 *  或者
 *  ```JavaScript
 *  var multibase = require('multibase');
 *  ```
 *  
 */
declare module 'multibase' {
    /**
     * @description 以 multibase 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @param codec 指定编码方式
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: Class_Buffer, codec: string): string;

    /**
     * @description 以 multibase 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string): Class_Buffer;

}

