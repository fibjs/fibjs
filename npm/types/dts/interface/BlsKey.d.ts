/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ECKey.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 椭圆曲线加密算法对象 
 * 
 * BlsKey 对象是 fibjs 中用于表示 BLS 密钥的对象。它可以用于构建、导入、导出和操作 BLS 密钥。BLS 密钥是一种公钥加密方式，通常用于身份验证和数据签名，具有在保证安全性的同时提供较高执行速度的特点。
 * 
 * BlsKey 对象提供了多种构造方法，可以从不同的密钥格式中加载密钥。它也提供了一些类似于 toString()、clone()、name、publicKey() 等公共属性和方法，以及一些静态方法，例如 from() 等。可以用这些方法操作 BlsKey 对象。
 * 
 * 此外，BlsKey 对象具有 isPrivate()、toJSON(key = "") 等成员属性，可以用于查询密钥是否为私钥以及导出对象的 JSON 格式表示等操作。
 * 
 * BlsKey 对象提供了一组灵活而又强大的 API，可以方便地管理 BLS 密钥，实现身份验证和数据签名等安全需求。
 * 
 * 下面我们以一个简单的示例来演示如何使用 BlsKey 对象进行签名和验签：
 * ```JavaScript
 * var crypto = require('crypto');
 * 
 * // create a private key
 * var privateKey = new crypto.BlsKey({
 *   'kty': 'EC',
 *   'crv': 'BLS12-381-G1',
 *   'x': 'TPk62yDxSISkoSBRPYkpO%tJmm0tZd4tJeLuCKVFv4UmBPfOQ2aDWrCifANam2wj',
 *   'd': 'zE-pf24p-l0IT_lMcrX0gStTcsx_k1f7DnJmrN8V7ZU',
 * });
 * 
 * // sign a message
 * var message = '这是一条需要签名的消息';
 * var signature = privateKey.sign(message);
 * 
 * // verify the signature
 * var publicKey = privateKey.publicKey;
 * var verify = publicKey.verify(message, signature);
 * 
 * console.log('verification result:', verify);
 * ```
 * 
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
     *  @description BBS 签名的加密套件，缺省为 Bls12381Sha256，可修改为 Bls12381Shake256，仅支持 BLS12-381-G2 曲线
     *     
     */
    bbs_suite: string;

    /**
     *  @description 使用当前私钥签名一组消息，仅支持 BLS12-381-G2 曲线
     *      opts 支持以下参数:
     *      ```JavaScript
     *      {
     *         header: Buffer|string // specified header for signature, default is empty
     *      }
     *      ```
     * 
     *      @param messages 指定待签署的消息数组
     *      @param opts 指定签名选项
     *      @return 返回签署的消息
     *     
     */
    bbsSign(messages: any[], opts?: FIBJS.GeneralObject): Class_Buffer;

    bbsSign(messages: any[], opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     *  @description 使用当前公钥验证签名，仅支持 BLS12-381-G2 曲线
     *      opts 支持以下参数:
     *      ```JavaScript
     *      {
     *         header: Buffer|string // specified header for signature, default is empty
     *      }
     *      ```
     * 
     *      @param messages 指定完整的消息数组
     *      @param sig 指定要验证的签名
     *      @param opts 指定验证选项
     *      @return 返回验证后的结果
     *     
     */
    bbsVerify(messages: any[], sig: Class_Buffer, opts?: FIBJS.GeneralObject): boolean;

    bbsVerify(messages: any[], sig: Class_Buffer, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     *  @description 使用当前公钥和签名生成一个证明，仅支持 BLS12-381-G2 曲线
     *      opts 支持以下参数:
     *      ```JavaScript
     *      {
     *         header: Buffer|string, // specified header for signature, default is empty
     *         proof_header: Buffer|string // specified header for proof, default is empty
     *      }
     *      ```
     * 
     *      @param sig 指定生成证明时使用的签名
     *      @param messages 指定完整的消息数组
     *      @param idx 指定生成证明时使用的消息索引，索引以 1 为基数
     *      @param opts 指定证明选项
     *      @return 返回生成的证明
     *     
     */
    proofGen(sig: Class_Buffer, messages: any[], idx: any[], opts?: FIBJS.GeneralObject): Class_Buffer;

    proofGen(sig: Class_Buffer, messages: any[], idx: any[], opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     *  @description 使用当前公钥验证证明，仅支持 BLS12-381-G2 曲线
     *      opts 支持以下参数:
     *      ```JavaScript
     *      {
     *         header: Buffer|string, // specified header for signature, default is empty
     *         proof_header: Buffer|string // specified header for proof, default is empty
     *      }
     *      ```
     * 
     *      @param messages 指定根据索引过滤的消息数组
     *      @param idx 指定证明使用的消息索引，索引以 1 为基数
     *      @param proof 指定生成的证明
     *      @param opts 指定证明选项
     *      @return 返回验证后的结果
     *     
     */
    proofVerify(messages: any[], idx: any[], proof: Class_Buffer, opts?: FIBJS.GeneralObject): boolean;

    proofVerify(messages: any[], idx: any[], proof: Class_Buffer, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 合并一组签名为一个单一签名
     *      @param sigs 待合并的一组签名
     *      @return 返回合并的单一签名
     *     
     */
    static aggregateSignature(sigs: any[]): Class_Buffer;

    /**
     * @description 合并一组公钥为一个单一公钥
     *      @param keys 待合并的一组公钥
     *      @return 返回合并的单一公钥
     *     
     */
    static aggregatePublicKey(keys: any[]): Class_BlsKey;

}

