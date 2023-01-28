/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 椭圆曲线加密算法对象 
 */
declare class Class_ECCKey extends Class_PKey {
    /**
     * @description 通过 DER 格式的密钥构造 ECCKey
     *      @param DerKey DER 格式的密钥
     *      @param password 解密密码
     *     
     */
    constructor(DerKey: Class_Buffer, password?: string);

    /**
     * @description 通过 PEM 格式的密钥构造 ECCKey
     *      @param pemKey PEM 格式的密钥
     *      @param password 解密密码
     *     
     */
    constructor(pemKey: string, password?: string);

    /**
     * @description 通过 JSON 格式的密钥构造 ECCKey
     * 
     *      jsonKey 的格式支持以下两种，EC 私钥：
     *      ```JavaScript
     *      {
     *         "kty": "EC",
     *         "crv": "P-521",
     *         "x": "ATfNNFuuvlGxrTGoXgyfSAGgRNNDnO3rN3k74urKJdVS14RYhdnSwm91Bm-F1l-T1XKlAY2yRnzG9w1Ukvo8c0wL",
     *         "y": "ASBHqrruB6kdkEUB3vlW3-UIkk4HtKdUeTwN-7m3j2rgZvYR1ffRAapDvWqKGiBjomqWafxokBkbDI0c95f6f4XU",
     *         "d": "AfkIbUHXfW41njdpoKuqqKludcoLJS8D_oMEwkj-GVaXFNKccIoF5iKGu2c69kNDjo83R_7wyGlfRczsklkik1ST"
     *      }
     *      ```
     *      EC 公钥：
     *      ```JavaScript
     *      {
     *         "kty": "EC",
     *         "crv": "P-521",
     *         "x": "ATfNNFuuvlGxrTGoXgyfSAGgRNNDnO3rN3k74urKJdVS14RYhdnSwm91Bm-F1l-T1XKlAY2yRnzG9w1Ukvo8c0wL",
     *         "y": "ASBHqrruB6kdkEUB3vlW3-UIkk4HtKdUeTwN-7m3j2rgZvYR1ffRAapDvWqKGiBjomqWafxokBkbDI0c95f6f4XU"
     *      }
     *      ```
     *      @param jsonKey JSON 格式的密钥
     *     
     */
    constructor(jsonKey: FIBJS.GeneralObject);

    /**
     * @description 返回当前算法的椭圆曲线名称 
     */
    readonly curve: string;

    /**
     * @description 从可恢复签名中恢复公钥，仅支持 secp256k1
     *      @param data 签名的原始数据
     *      @param sig 可恢复签名
     *      @return 返回包含公钥的对象
     *     
     */
    static recover(data: Class_Buffer, sig: Class_Buffer): Class_ECCKey;

    static recover(data: Class_Buffer, sig: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_ECCKey)=>any): void;

    /**
     * @description 合并一组签名为一个单一签名，仅支持 BLS12381_G1 和 BLS12381_G2
     *      @param sigs 待合并的一组签名
     *      @return 返回合并的单一签名
     *     
     */
    static aggregateSignatures(sigs: any[]): Class_Buffer;

    /**
     * @description 使用当前算法计算椭圆曲线 Diffie-Hellman (ECDH) 共享密钥
     *      @param publicKey 指定对方的公钥
     *      @return 返回计算出的共享密钥
     *      
     */
    computeSecret(publicKey: Class_ECCKey): Class_Buffer;

    computeSecret(publicKey: Class_ECCKey, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

}

