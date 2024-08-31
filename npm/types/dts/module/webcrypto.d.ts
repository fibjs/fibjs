/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/CryptoKey.d.ts" />
/// <reference path="../module/subtle.d.ts" />
/**
 * @description WebCrypto API 模块
 * 
 * WebCrypto API 模块提供了一组用于加密和解密的函数。可以通过 global 对象的 webcrypto 属性或者 require("crypto").webcrypto 来获取。
 * 
 */
declare module 'webcrypto' {
    /**
     * @description 生成随机数
     * 
     *     @param data 一个 TypedArray 对象，用于存放生成的随机数。
     *     @return 返回 data 对象。
     *     
     */
    function getRandomValues(data: TypedArray): TypedArray;

    /**
     * @description 生成一个 UUID
     * 
     *     @return 返回生成的 UUID 字符串。
     *     
     */
    function randomUUID(): string;

    /**
     * @description CryptoKey 类来表示对称或非对称密钥，每种密钥公开不同的功能 
     */
    const CryptoKey: typeof Class_CryptoKey;

    /**
     * @description 提供对 SubtleCrypto API 的访问 
     */
    const subtle: typeof import ('subtle');

}

