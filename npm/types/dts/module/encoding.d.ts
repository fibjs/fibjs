/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/base32.d.ts" />
/// <reference path="../module/base64.d.ts" />
/// <reference path="../module/hex.d.ts" />
/// <reference path="../module/iconv.d.ts" />
/// <reference path="../module/json.d.ts" />
/// <reference path="../module/msgpack.d.ts" />
/**
 * @description 编码与解码模块，用于处理不同的数据编码格式与二进制之间的转换
 *  引用方式：
 *  ```JavaScript
 *  var encoding = require('encoding');
 *  ```
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
     * @description hex 编码与解码模块 
     */
    const hex: typeof import ('hex');

    /**
     * @description iconv 编码与解码模块 
     */
    const iconv: typeof import ('iconv');

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

