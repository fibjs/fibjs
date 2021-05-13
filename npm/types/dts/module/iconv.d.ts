/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description iconv 编码与解码模块
 * 
 *  引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  var iconv = encoding.iconv;
 *  ```
 *  或者
 *  ```JavaScript
 *  var iconv = require('iconv');
 *  ```
 *  iconv 会使用系统已安装的编码进行处理，iconv 模块同时还支持以下内置编码集：
 *  ```JavaScript
 *  "utf8", "utf-8",
 * 
 *  "ucs2", "ucs-2", "utf16", "utf-16",
 *  "ucs2le", "ucs-2le", "utf16le", "utf-16le",
 *  "ucs2be", "ucs-2be", "utf16be", "utf-16be",
 *  
 *  "ucs4", "ucs-4", "utf32", "utf-32",
 *  "ucs4le", "ucs-4le", "utf32le", "utf-32le",
 *  "ucs4be", "ucs-4be", "utf32be", "utf-32be"
 *  ```
 *  
 */
declare module 'iconv' {
    /**
     * @description 用 iconv 将文本转换为二进制数据
     * 	 @param charset 指定字符集
     * 	 @param data 要转换的文本
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function encode(charset: string, data: string): Class_Buffer;

    /**
     * @description 用 iconv 将 Buffer 内容转换为文本
     * 	 @param charset 指定字符集
     * 	 @param data 要转换的二进制数据
     * 	 @return 返回编码的字符串
     * 	 
     */
    function decode(charset: string, data: Class_Buffer): string;

    /**
     * @description 检测字符集是否被支持
     * 	 @param charset 指定字符集
     * 	 @return 返回是否支持该字符集
     * 	 
     */
    function isEncoding(charset: string): boolean;

}

