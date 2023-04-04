/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/**
 * @description 信息摘要对象 
 * 
 * 可以这样使用 Digest 对象:
 * 
 * ```
 * const crypto = require('crypto');
 * // 创建 SHA-512 摘要运算对象
 * const digest = crypto.createHash('sha512');
 * // 更新数据
 * digest.update('hello');
 * digest.update('world');
 * // 获取二进制结果
 * const result = digest.digest();
 * console.log(result);
 * 
 * // 生成指定编码的结果
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
     * @description 计算并返回摘要
     *      @param codec 指定编码格式，允许值为："buffer", "hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回指定编码的摘要表示
     *      
     */
    digest(codec?: string): any;

    /**
     * @description 签名并返回摘要
     *      opts 支持以下参数:
     *      ```JavaScript
     *      {
     *         alg: 0， 指定签名的 hash 算法，仅在 RSA 时有效，缺省为 0. 支持算法: 0=NONE,1=MD5,2=SHA1,3=SHA224,4=SHA256,5=SHA384,6=SHA512,7=RIPEMD160
     *         to: pk, 指定验证方公钥，仅在 ecsdsa 或 sm2 时有效
     *      }
     *      ```
     * 
     *      @param key 签名的私钥
     *      @param opts 指定签名选项
     *      @return 返回签名后的数据
     *      
     */
    sign(key: Class_PKey, opts?: FIBJS.GeneralObject): Class_Buffer;

    sign(key: Class_PKey, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 验证签名是否一致
     *      opts 支持以下参数:
     *      ```JavaScript
     *      {
     *         alg: 0， 指定签名的 hash 算法，仅在 RSA 时有效，缺省为 0. 支持算法: 0=NONE,1=MD5,2=SHA1,3=SHA224,4=SHA256,5=SHA384,6=SHA512,7=RIPEMD160
     *         to: pk, 指定验证方公钥，仅在 ecsdsa 或 sm2 时有效
     *      }
     *      ```
     * 
     *      @param key 验证签名的公钥
     *      @param sign 指定要验证的签名
     *      @param opts 指定验证选项
     *      @return 返回验证后的结果
     *      
     */
    verify(key: Class_PKey, sign: Class_Buffer, opts?: FIBJS.GeneralObject): boolean;

    verify(key: Class_PKey, sign: Class_Buffer, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 查询当前信息摘要算法的摘要字节数 
     */
    readonly size: number;

}

