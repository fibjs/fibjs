/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ECKey.d.ts" />
/**
 * @description 椭圆曲线加密算法对象 
 * 
 * 以下示例演示了 Ed25519Key 的用法：
 * 
 * ```JavaScript
 * const crypto = require('crypto');
 * 
 * // 生成 Ed25519 密钥对
 * const key = crypto.generateKey('ed25519');
 * 
 * // 打印公钥、私钥等信息
 * console.log('PrivateKey:', key.pem());
 * console.log('PublicKey:', key.publicKey.pem());
 * 
 * // 签名消息
 * const message = 'Hello, World!';
 * const sig = key.sign(message);
 * console.log('Sig:', sig.hex());
 * 
 * // 验证签名
 * const verifyResult = key.verify(message, sig);
 * console.log('Verify Result:', verifyResult);
 * ```
 * 
 */
declare class Class_Ed25519Key extends Class_ECKey {
    /**
     * @description 从当前对象转换 X25519 公私钥对，仅支持 Ed25519
     *      @return 返回对应的 X25519 公钥的对象
     *     
     */
    toX25519(): Class_ECKey;

    toX25519(callback: (err: Error | undefined | null, retVal: Class_ECKey)=>any): void;

}

