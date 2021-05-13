/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 对称加密算法对象
 * 
 *  Cipher 对象属于 crypto 模块，创建：
 *  ```JavaScript
 *  var c = new crypto.Cipher(crypto.AES, crypto.ECB, ...);
 *  ```
 *  
 */
declare class Class_Cipher extends Class_object {
    /**
     * @description Cipher 构造函数，仅用于 ARC4 初始化
     *      @param provider 指定加密算法
     *      @param key 指定加密解密密码
     *     
     */
    constructor(provider: number, key: Class_Buffer);

    /**
     * @description Cipher 构造函数
     *      @param provider 指定加密算法
     *      @param mode 指定分组密码工作模式
     *      @param key 指定加密解密密码
     *     
     */
    constructor(provider: number, mode: number, key: Class_Buffer);

    /**
     * @description Cipher 构造函数
     *      @param provider 指定加密算法
     *      @param mode 指定分组密码工作模式
     *      @param key 指定加密解密密码
     *      @param iv 指定初始向量
     *     
     */
    constructor(provider: number, mode: number, key: Class_Buffer, iv: Class_Buffer);

    /**
     * @description 返回当前算法名称 
     */
    readonly name: string;

    /**
     * @description 返回当前算法密码长度，以位为单位 
     */
    readonly keySize: number;

    /**
     * @description 返回当前算法初始向量长度，以字节为单位 
     */
    readonly ivSize: number;

    /**
     * @description 返回当前算法数据块长度，以字节为单位 
     */
    readonly blockSize: number;

    /**
     * @description 使用填充模式
     *      @param mode 指定填充模式，缺省为 PADDING_PKCS7
     *      
     */
    paddingMode(mode: number): void;

    /**
     * @description 使用当前算法密码加密数据
     *      @param data 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    encrypt(data: Class_Buffer): Class_Buffer;

    encrypt(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用当前算法密码解密数据
     *      @param data 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    decrypt(data: Class_Buffer): Class_Buffer;

    decrypt(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

}

