/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/base32.d.ts" />
/// <reference path="../module/base64.d.ts" />
/// <reference path="../module/base58.d.ts" />
/// <reference path="../module/hex.d.ts" />
/// <reference path="../module/multibase.d.ts" />
/// <reference path="../module/iconv.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../module/json.d.ts" />
/// <reference path="../module/msgpack.d.ts" />
/**
 * @description `encoding` 模块是 FibJS 内置的一个模块，用于实现各种数据编码格式与二进制之间的转换。这些数据编码格式包括了 `base64`、`base32`、`hex`、`json`、`msgpack`、`multibase`、`base58` 等等一些常用的数据编码格式。
 * 
 * 下面是 `encoding` 模块提供的各个子模块的简要介绍：
 * 
 * - `iconv`：用于处理各种编码之间的转换。
 * - `base64`：提供了一些对 Base64 编解码的支持，可以将字符串编码为 Base64 格式，也可以将 Base64 格式解码为字符串。
 * - `base32`：提供了对 Base32 编解码的支持，可以将给定的数据进行 Base32 编码并返回编码后的字符串。也可以将 Base32 编码的字符串解码为原始的数据。
 * - `hex`：提供了对十六进制编解码的支持，可以将给定的数据进行十六进制编码，并返回编码后的字符串。也可以将十六进制编码的字符串解码为原始的数据。
 * - `json`：提供了对 JSON 编解码的支持，可以将 JavaScript 对象序列化为 JSON 字符串，也可以将 JSON 字符串反序列化为 JavaScript 对象。
 * - `multibase`：提供了对 Multibase 编码的支持，Multibase 是在 Base1x 编码的基础上引入了多种编码前缀的编码方式。
 * - `msgpack`：提供了对 Msgpack 编解码的支持，Msgpack 是一种比 JSON 更轻量的数据交换格式，它可以将 JSON 对象序列化为二进制数据，以达到更快、更高效的数据交换效果。
 * - `base58`：提供了对 Base58 编解码的支持，Base58 是一种数字与字母的组合表示方法，它不包含数字 0、字母 O、字母 I、字母 L 等易混淆字符，具有不易出错的特点。
 * 
 * `encoding` 模块中的大部分子模块都包含了编码与解码两个函数，使用这些函数可以将特定格式的数据进行编码或解码。使用这些编解码模块时，需要根据模块的类型等特点来选择最适合的模块，才能保证编码和解码的正确性。
 *  
 *  encoding` 模块的引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  ```
 * 
 * 
 * 
 *  
 */
declare module 'encoding' {
    /**
     * @description base32 编码与解码模块 
     */
    const base32: typeof import ('base32');

    /**
     * @description base64 编码与解码模块 
     */
    const base64: typeof import ('base64');

    /**
     * @description base58 编码与解码模块 
     */
    const base58: typeof import ('base58');

    /**
     * @description hex 编码与解码模块 
     */
    const hex: typeof import ('hex');

    /**
     * @description multibase 编码与解码模块 
     */
    const multibase: typeof import ('multibase');

    /**
     * @description iconv 编码与解码模块 
     */
    const iconv: typeof import ('iconv');

    /**
     * @description 判断指定的编码是否支持
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回编码是否支持
     *     
     */
    function isEncoding(codec: string): boolean;

    /**
     * @description 将 Buffer 编码为字符串
     *      @param data 初始化字符串，字符串将以 utf-8 格式写入
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集，缺省为 "utf8"
     *      @return 返回编码的字符串
     *     
     */
    function encode(data: Class_Buffer, codec?: string): string;

    /**
     * @description 将字符串解码为 Buffer
     *      @param str 初始化字符串，字符串将以 utf-8 格式写入
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集，缺省为 "utf8"
     *      @return 返回解码的 Buffer
     *     
     */
    function decode(str: string, codec?: string): Class_Buffer;

    /**
     * @description json 编码与解码模块 
     */
    const json: typeof import ('json');

    /**
     * @description msgpack 编码与解码模块 
     */
    const msgpack: typeof import ('msgpack');

    /**
     * @description 将字符串编码为 javascript 转义字符串，用以在 javascript 代码中包含文本
     *      @param str 要编码的字符串
     *      @param json 是否生成json兼容字符串
     *      @return 返回编码的字符串
     *      
     */
    function jsstr(str: string, json?: boolean): string;

    /**
     * @description url 字符串安全编码
     *      @param url 要编码的 url
     *      @return 返回编码的字符串
     *      
     */
    function encodeURI(url: string): string;

    /**
     * @description url 部件字符串安全编码
     *      @param url 要编码的 url
     *      @return 返回编码的字符串
     *      
     */
    function encodeURIComponent(url: string): string;

    /**
     * @description url 安全字符串解码
     *      @param url 要解码的 url
     *      @return 返回解码的字符串
     *      
     */
    function decodeURI(url: string): string;

}

