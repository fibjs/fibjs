/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description ECDH 对象 
 * 
 * 
 */
declare class Class_ECDH extends Class_object {
    /**
     * @description 将公钥转换为指定的格式
     *         @param key 待转换的公钥
     *         @param curve 指定预定义的椭圆曲线
     *         @param inputEncoding 指定 key 的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'hex'
     *         @param outputEncoding 指定返回结果的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'hex'
     *         @param format 指定公钥的格式，可以为：'compressed', 'uncompressed', 'hybrid'，默认为 'uncompressed'
     *         @return 返回转换后的公钥
     *     
     */
    static convertKey(key: any, curve: string, inputEncoding?: string, outputEncoding?: string, format?: string): any;

    /**
     * @description 根据其它公钥，计算共享密钥
     *         @param otherPublicKey 对方的公钥
     *         @param inputEncoding 指定 otherPublicKey 的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'hex'
     *         @param outputEncoding 指定返回结果的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'buffer'
     *         @return 返回计算得出的共享密钥
     *     
     */
    computeSecret(otherPublicKey: any, inputEncoding?: string, outputEncoding?: string): any;

    /**
     * @description 生成密钥对
     *         @param outputEncoding 指定返回结果的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'buffer'
     *         @param format 指定公钥的格式，可以为：'compressed', 'uncompressed', 'hybrid'，默认为 'uncompressed'
     *         @return 返回生成的公钥
     *     
     */
    generateKeys(outputEncoding?: string, format?: string): any;

    /**
     * @description 获取椭圆曲线的名称
     *         @return 返回椭圆曲线的名称
     *     
     */
    readonly curveName: string;

    /**
     * @description 获取私钥
     *         @param encoding 指定私钥的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'buffer'
     *         @return 返回私钥
     *     
     */
    getPrivateKey(encoding?: string): any;

    /**
     * @description 获取公钥
     *         @param encoding 指定公钥的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'buffer'
     *         @param format 指定公钥的格式，可以为：'compressed', 'uncompressed', 'hybrid'，默认为 'uncompressed'
     *         @return 返回公钥
     *     
     */
    getPublicKey(encoding?: string, format?: string): any;

    /**
     * @description 设置私钥
     *         @param privateKey 私钥数据
     *         @param encoding 指定 privateKey 的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'hex'
     *     
     */
    setPrivateKey(privateKey: any, encoding?: string): void;

    /**
     * @description 设置公钥
     *         @param publicKey 公钥数据
     *         @param encoding 指定 publicKey 的编码格式，可以为：'buffer', 'hex', 'base64', 'base58'，默认为 'hex'
     *     
     */
    setPublicKey(publicKey: any, encoding?: string): void;

}

