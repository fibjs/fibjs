/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/CryptoKey.d.ts" />
/**
 * @description 提供对 SubtleCrypto API 的访问
 * 
 * SubtleCrypto API 模块提供了一组用于加密和解密的函数。可以通过 global.webcrypto.subtle 属性或者 require("crypto").webcrypto.subtle 来获取。
 * 
 */
declare module 'subtle' {
    /**
     * @description 计算给定数据的哈希值
     * 
     *     @param algorithm 指定哈希算法
     *     @param data 指定要计算哈希值的数据
     *     @return 返回计算得到的哈希值
     *     
     */
    function digest(algorithm: string, data: Class_Buffer): Class_Buffer;

    function digest(algorithm: string, data: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 计算给定数据的哈希值
     * 
     *     algorithm 的 name 属性指定哈希算法，例如：
     *     ```
     *     {
     *         name: "SHA-256"
     *     }
     *     ```
     * 
     *     @param algorithm 指定哈希算法
     *     @param data 指定要计算哈希值的数据
     *     @return 返回计算得到的哈希值
     *     
     */
    function digest(algorithm: FIBJS.GeneralObject, data: Class_Buffer): Class_Buffer;

    function digest(algorithm: FIBJS.GeneralObject, data: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 导出 key 的信息，如果 key 不可导出，则返回一个错误
     * 
     *     @param format 导出的格式，可以是 'raw'，'pkcs8'，'spki' 或 'jwk'.
     *     @param key 要导出的 key
     *     @return 返回导出的 key 信息
     *     
     */
    function exportKey(format: string, key: Class_CryptoKey): any;

    function exportKey(format: string, key: Class_CryptoKey, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 生成一个新的 key
     * 
     *     @param algorithm 指定生成 key 的算法
     *     @param extractable 指定 key 是否可以导出到外部
     *     @param usages 指定 key 的用途
     *     @return 返回生成的 key
     *     
     */
    function generateKey(algorithm: FIBJS.GeneralObject, extractable: boolean, usages: any[]): any;

    function generateKey(algorithm: FIBJS.GeneralObject, extractable: boolean, usages: any[], callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 导入 key
     * 
     *     @param format 导入的格式，可以是 'raw'，'pkcs8'，'spki' 或 'jwk'.
     *     @param keyData 包含 key 数据的对象
     *     @param algorithm 指定 key 的算法
     *     @param extractable 指定 key 是否可以导出到外部
     *     @param usages 指定 key 的用途
     *     @return 返回导入的 key
     *     
     */
    function importKey(format: string, keyData: any, algorithm: FIBJS.GeneralObject, extractable: boolean, usages: any[]): Class_CryptoKey;

    function importKey(format: string, keyData: any, algorithm: FIBJS.GeneralObject, extractable: boolean, usages: any[], callback: (err: Error | undefined | null, retVal: Class_CryptoKey)=>any): void;

}

