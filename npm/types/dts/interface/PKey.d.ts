/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 不对称加密算法对象
 * 
 *  PKey 对象属于 crypto 模块，创建：
 *  ```JavaScript
 *  var k = new crypto.PKey();
 *  ```
 *  
 */
declare class Class_PKey extends Class_object {
    /**
     * @description PKey 构造函数 
     */
    constructor();

    /**
     * @description 返回当前算法名称 
     */
    readonly name: string;

    /**
     * @description 返回当前算法密码长度，以位为单位 
     */
    readonly keySize: number;

    /**
     * @description 返回当前密钥的公钥
     *      @return 当前密钥的公钥
     *     
     */
    readonly publicKey: Class_PKey;

    /**
     * @description 生成一个 RSA 私钥
     *      @param size 指定 RSA 密钥长度，bit 为单位
     *     
     */
    genRsaKey(): void;

    /**
     * @description 生成一个 EC 私钥
     *      @param curve 指定预置椭圆曲线，可选值为："secp521r1", "brainpoolP512r1", "secp384r1", "brainpoolP384r1", "secp256r1", "secp256k1", "brainpoolP256r1", "secp224r1", "secp224k1", "secp192r1", "secp192k1"
     *     
     */
    genEcKey(): void;

    /**
     * @description 生成一个 SM2 私钥
     *     
     */
    genSm2Key(): void;

    /**
     * @description 查询当前密钥是否为私钥
     *      @return 为 True 表示为私钥
     *     
     */
    isPrivate(): boolean;

    /**
     * @description 复制当前密钥
     *      @return 当前密钥的复制对象
     *     
     */
    clone(): Class_PKey;

    /**
     * @description 加载一个 DER 格式的密钥
     *      @param DerKey DER 格式的密钥
     *      @param password 解密密码
     *     
     */
    importKey(): void;

    /**
     * @description 加载一个 PEM/DER 格式的密钥文件
     *      @param filename 密钥文件名
     *      @param password 解密密码
     *     
     */
    importFile(): void;

    /**
     * @description 返回当前 key 的 PEM 格式编码
     *      @return 当前 key 的 PEM 格式编码
     *     
     */
    exportPem(): string;

    /**
     * @description 返回当前 key 的 DER 格式编码
     *      @return 当前 key 的 DER 格式编码
     *     
     */
    exportDer(): Class_Buffer;

    /**
     * @description 返回当前 key 的 DER 格式编码
     *      @return 当前 key 的 DER 格式编码
     *     
     */
    exportJson(): object;

    /**
     * @description 使用当前算法密码公钥加密数据
     *      @param data 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    encrypt(): Class_Buffer;

    /**
     * @description 使用当前算法密码私钥解密数据
     *      @param data 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    decrypt(): Class_Buffer;

    /**
     * @description 使用当前算法密码私钥签名数据
     *      @param data 指定要签名的数据
     *      @param alg 指定要签名的算法, 默认0. 支持算法: 0=NONE,1=MD2,2=MD4,3=MD5,4=SHA1,5=SHA224,6=SHA256,7=SHA384,8=SHA512,9=RIPEMD160
     *      @return 返回签名后的数据
     *      
     */
    sign(): Class_Buffer;

    /**
     * @description 使用当前算法密码公钥验证数据
     *      @param data 指定要验证的数据
     *      @param sign 指定要验证的签名
     *      @param alg 指定要签名的算法, 默认0. 支持算法: 0=NONE,1=MD2,2=MD4,3=MD5,4=SHA1,5=SHA224,6=SHA256,7=SHA384,8=SHA512,9=RIPEMD160
     *      @return 返回验证后的结果
     *      
     */
    verify(): boolean;

}

