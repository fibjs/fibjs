/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description multibase 是在 Base1x 编码的基础上，引入了多种编码前缀的编码方式
 * 
 * multibase 可以将相同的数据使用不同的编码方式编码，并添加一个前缀表示编码方式。multibase 支持的编码方式共有 15 种，分别是 base1、base2、base8、base10、base16、base32、base32hex、base32z、base36、base40、base56、base58flickr、base58btc、base64、base64url。其中，base16、base32 和 base64 是比较常用的。
 * 
 * multibase 可以用于在不改变二进制数据的情况下，改变其展示形式。例如，将一个随机生成的二进制数据编码为 base32 形式的字符串：
 * ```JavaScript
 * const {
 *     encode
 * } = require('multibase');
 * 
 * const crypto = require('crypto');
 * const data = crypto.randomBytes(10); // generate 10 bytes random data
 * const encodedStr = encode(data, 'base32'); // encode data to base32 string
 * console.log(encodedStr); // ==> "bpgwnvztqmlbo5fy"
 * ```
 * 将上述字符串解码为原始的二进制数据：
 * ```JavaScript
 * const {
 *     decode
 * } = require('multibase');
 * 
 * const data = decode('bpgwnvztqmlbo5fy', 'base32'); // decode base32 string to data
 * console.log(data); // ==> <Buffer a7 55 3d 33 ca 97 ac 0d aa 40>
 * ```
 * 可以看到，通过 multibase，我们将原始的二进制数据编码为了 base32 形式的字符串，并且可以将这个字符串解码为原始的二进制数据。
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

