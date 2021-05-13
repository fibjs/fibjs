/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description msgpack 编码与解码模块
 *  引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  var msgpack = encoding.msgpack;
 *  ```
 *  或者
 *  ```JavaScript
 *  var msgpack = require('msgpack');
 *  ```
 *  
 */
declare module 'msgpack' {
    /**
     * @description 以 msgpack 格式编码变量
     * 	 @param data 要编码的变量
     * 	 @return 返回编码的二进制数据
     * 	 
     */
    function encode(data: any): Class_Buffer;

    /**
     * @description 以 msgpack 方式解码字符串为一个变量
     * 	 @param data 要解码的二进制数据
     * 	 @return 返回解码的变量
     * 	 
     */
    function decode(data: Class_Buffer): any;

}

