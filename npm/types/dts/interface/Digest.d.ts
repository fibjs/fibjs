/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 信息摘要对象 
 * 
 * 可以这样使用 Digest 对象:
 * 
 * ```
 * const crypto = require('crypto');
 * // create a SHA-512 digest object
 * const digest = crypto.createHash('sha512');
 * // update digest with data
 * digest.update('hello');
 * digest.update('world');
 * // get digest result
 * const result = digest.digest();
 * console.log(result);
 * 
 * // output result in hex and base64
 * console.log(result.toString('hex'));
 * console.log(result.toString('base64'));
 * ```
 * 上述代码中，使用 `crypto.createHash()` 方法创建了一个 SHA-512 摘要运算对象，可以通过 `update()` 方法逐步添加要进行摘要的数据，并通过 `digest()` 方法获取摘要结果。
 * 
 */
declare class Class_Digest extends Class_object {
    /**
     * @description 更新二进制摘要信息
     *      @param data 二进制数据块
     *      @return 返回信息摘要对象本身
     *      
     */
    update(data: Class_Buffer): Class_Digest;

    /**
     * @description 更新字符串摘要信息
     *      @param data 字符串数据
     *      @param codec 指定编码格式，允许值为："buffer", "hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回信息摘要对象本身
     *      
     */
    update(data: string, codec?: string): Class_Digest;

    /**
     * @description 计算并返回摘要
     *      @param codec 指定编码格式，允许值为："buffer", "hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回指定编码的摘要表示
     *      
     */
    digest(codec?: string): any;

    /**
     * @description 查询当前信息摘要算法的摘要字节数 
     */
    readonly size: number;

}

