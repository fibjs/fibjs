/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ECKey.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 椭圆曲线加密算法对象 
 */
declare class Class_BlsKey extends Class_ECKey {
    /**
     * @description 通过 JSON 格式的密钥构造 BlsKey
     * 
     *      jsonKey 的格式支持以下两种，私钥：
     *      ```JavaScript
     *      {
     *         "kty": "EC",
     *         "crv": "BLS12381_G1",
     *         "x": "tCgCNuUYQotPEsrljWi-lIRIPpzhqsnJV1NPnE7je6glUb-FJm9IYkuv2hbHw22i",
     *         "d": "TXNvJBBG3h23H5hFJcnRZmYd_j1TqpwtJOllYGU3yyw"
     *      }
     *      ```
     *      Bls 公钥：
     *      ```JavaScript
     *      {
     *         "kty": "EC",
     *         "crv": "BLS12381_G1",
     *         "x": "tCgCNuUYQotPEsrljWi-lIRIPpzhqsnJV1NPnE7je6glUb-FJm9IYkuv2hbHw22i"
     *      }
     *      ```
     *      @param jsonKey JSON 格式的密钥
     *     
     */
    constructor(jsonKey: FIBJS.GeneralObject);

    /**
     * @description 合并一组签名为一个单一签名
     *      @param sigs 待合并的一组签名
     *      @return 返回合并的单一签名
     *     
     */
    static aggregateSignature(sigs: any[]): Class_Buffer;

    /**
     * @description 合并一组公钥为一个单一公钥
     *      @param sigs 待合并的一组公钥
     *      @return 返回合并的单一公钥
     *     
     */
    static aggregatePublicKey(sigs: any[]): Class_BlsKey;

}

