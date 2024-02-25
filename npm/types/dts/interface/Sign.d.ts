/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description 用于生成签名的实用程序
 * 
 *   crypto.createSign 方法用于创建 Sign 实例。参数是要使用的哈希函数的字符串名称。Sign 对象不能直接使用 new 关键字创建。
 * 
 *   示例:
 *   ```JavaScript
 *     const {
 *         generateKeyPairSync,
 *         createSign,
 *         createVerify,
 *     } = require('crypto');
 * 
 *     const { privateKey, publicKey } = generateKeyPairSync('rsa', {
 *         modulusLength: 2048,
 *     });
 * 
 *     const sign = createSign('SHA256');
 *     sign.update('some data to sign');
 *     const signature = sign.sign(privateKey);
 * 
 *     const verify = createVerify('SHA256');
 *     verify.update('some data to sign');
 *     console.log(verify.verify(publicKey, signature));
 *   ```
 *  
 */
declare class Class_Sign extends Class_object {
    /**
     * @description 使用给定的 data 更新 Sign 内容
     *      @param data 要更新的数据
     *      @return 返回 Sign 对象本身
     *     
     */
    update(data: Class_Buffer): Class_Sign;

    /**
     * @description 使用给定的 data 更新 Sign 内容
     *      @param data 要更新的数据
     *      @param codec data 的编码方式
     *      @return 返回 Sign 对象本身
     *     
     */
    update(data: string, codec?: string): Class_Sign;

    /**
     * @description 计算传递的所有数据的签名
     *      @param privateKey 用于签名的私钥
     *      @param encoding 返回值的编码方式
     *      @return 返回签名的值
     *     
     */
    sign(privateKey: Class_Buffer, encoding?: string): any;

    /**
     * @description 计算传递的所有数据的签名
     *      @param privateKey 用于签名的私钥
     *      @param encoding 返回值的编码方式
     *      @return 返回签名的值
     *     
     */
    sign(privateKey: Class_KeyObject, encoding?: string): any;

    /**
     * @description 计算传递的所有数据的签名
     * 
     *      key 内的参数会用于调用 crypto.createPrivateKey 创建私钥对象，此外还支持以下签名参数：
     *      - dsaEncoding 对于 DSA 和 ECDSA，此选项指定生成的签名的格式。它可以是以下之一: 
     *       - 'der'（默认）: DER 编码的 ASN.1 签名结构编码 (r, s) 
     *       - 'ieee-p1363' : IEEE-P1363 中提议的签名格式 r || s
     *      - padding RSA 的可选填充值，以下之一: 
     *       - RSA_PKCS1_PADDING（默认）
     *       - RSA_PKCS1_PSS_PADDING，RSA_PKCS1_PSS_PADDING 将使用 MGF1，其哈希函数与用于对 RFC 4055 第 3.1 节中指定的消息进行签名的哈希函数相同
     *      - saltLength 当填充为 RSA_PKCS1_PSS_PADDING 时的盐长度。特殊值 RSA_PSS_SALTLEN_DIGEST 将盐长度设置为摘要大小，RSA_PSS_SALTLEN_MAX_SIGN（默认）将其设置为最大允许值
     * 
     *      @param key 用于签名的私钥
     *      @param encoding 返回值的编码方式
     *      @return 返回签名的值
     *     
     */
    sign(key: FIBJS.GeneralObject, encoding?: string): any;

}

