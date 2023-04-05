/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description ECKey 类是在 crypto 模块中实现的，可以使用该类来生成 ECDSA 密钥对、签名和验证消息等
 * 
 * 以下是一个例子：
 * ```javascript
 * const crypto = require("crypto");
 * 
 * // generate a secp256k1 key
 * const key = crypto.generateKey("secp256k1");
 * console.log("Private key:", key.pem());
 * console.log("Public key:", key.publicKey.pem());
 * 
 * const message = "Hello, fibjs!";
 * // sign a message
 * const sig = key.sign(message);
 * console.log("Signature:", sig.hex());
 * 
 * // very the signature
 * const verify = key.verify(message, sig);
 * console.log("Verify result:", verify);
 * ```
 * 
 * 在该例子中，我们首先生成了一个 secp256k1 的 ECKey 对象，然后打印了生成的公钥和私钥。接着，我们签名了消息 "Hello, fibjs!" 并打印了签名结果，最后使用同样的密钥对象验证了签名，并打印了验证结果。 
 */
declare class Class_ECKey extends Class_PKey {
    /**
     * @description 通过 DER 格式的密钥构造 ECKey
     *      @param DerKey DER 格式的密钥
     *      @param password 解密密码
     *     
     */
    constructor(DerKey: Class_Buffer, password?: string);

    /**
     * @description 通过 PEM 格式的密钥构造 ECKey
     *      @param pemKey PEM 格式的密钥
     *      @param password 解密密码
     *     
     */
    constructor(pemKey: string, password?: string);

    /**
     * @description 通过 JSON 格式的密钥构造 ECKey
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
    static recover(data: Class_Buffer, sig: Class_Buffer): Class_ECKey;

    static recover(data: Class_Buffer, sig: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_ECKey)=>any): void;

    /**
     * @description 使用当前算法计算椭圆曲线 Diffie-Hellman (ECDH) 共享密钥
     *      @param publicKey 指定对方的公钥
     *      @return 返回计算出的共享密钥
     *      
     */
    computeSecret(publicKey: Class_ECKey): Class_Buffer;

    computeSecret(publicKey: Class_ECKey, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

}

