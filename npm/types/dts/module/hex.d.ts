/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description hex 模块是内置模块，在编码和解码二进制数据和 ASCII 字符之间提供了十六进制的实现方式。`hex` 模块提供了编码和解码两种功能
 * 
 * 使用编码方法，可以将任意二进制数据编码为十六进制的字符串。例如：
 * 
 * ```JavaScript
 * const hex = require('hex')
 * 
 * const data = new Buffer([0x4e, 0x4f, 0x44, 0x45]) // [0x4e, 0x4f, 0x44, 0x45] => 'NODE'
 * const encodedData = hex.encode(data)
 * console.log(encodedData) // "4e4f4445"
 * ```
 * 
 * 使用解码方法，可以将十六进制的字符串解码为原来的二进制数据。例如：
 * 
 * ```JavaScript
 * const hex = require('hex')
 * 
 * const encodedData = '4e4f4445' // 'NODE'
 * const decodedData = hex.decode(encodedData)
 * console.log(decodedData) // [0x4e, 0x4f, 0x44, 0x45]
 * ```
 * 
 * 可以看到，`hex` 是非常简单的编码和解码模块，它适用于一些简单的二进制数据和字符串之间的转换需求。
 *  
 */
declare module 'hex' {
    /**
     * @description 以 hex 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: Class_Buffer): string;

    /**
     * @description 以 hex 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string): Class_Buffer;

}

