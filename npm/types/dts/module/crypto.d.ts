/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/crypto_constants.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
/// <reference path="../interface/Digest.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Cipher.d.ts" />
/// <reference path="../interface/Sign.d.ts" />
/// <reference path="../interface/Verify.d.ts" />
/// <reference path="../interface/X509CertificateRequest.d.ts" />
/// <reference path="../module/webcrypto.d.ts" />
/**
 * @description `crypto` 模块是 `fibjs` 内置的加密算法模块。它提供了对称加密、非对称加密、摘要算法、密码学随机数生成器等功能。在使用之前，需要通过 `require('crypto')` 加载该模块 
 */
declare module 'crypto' {
    /**
     * ! crypto 模块的常量对象，参见 crypto_constants 
     */
    const constants: typeof import ('crypto_constants');

    /**
     * @description KeyObject 对象，参见 KeyObject 
     */
    const KeyObject: typeof Class_KeyObject;

    /**
     * @description X509Certificate 构造函数，参见 X509Certificate 
     */
    const X509Certificate: typeof Class_X509Certificate;

    /**
     * @description 获取 crypto 模块支持的的 hash(摘要) 算法
     *      @return 返回支持的 hash 算法数组
     *      
     */
    function getHashes(): any[];

    /**
     * @description 根据给定的算法名称创建一个信息摘要对象
     *      @param algo 指定信息摘要对象的算法
     *      @return 返回信息摘要对象
     *     
     */
    function createHash(algo: string): Class_Digest;

    /**
     * @description 根据给定的算法名称创建一个 hmac 信息摘要对象
     *      @param algo 指定信息摘要对象的算法
     *      @param key 二进制签名密钥
     *      @return 返回信息摘要对象
     *     
     */
    function createHmac(algo: string, key: Class_Buffer): Class_Digest;

    /**
     * @description 获取 crypto 模块支持的的对称加密算法
     *      @return 返回支持的对称加密算法数组
     *      
     */
    function getCiphers(): any[];

    /**
     * @description 创建一个对称加密的加密对象
     *      @param algorithm 指定加密算法
     *      @param key 指定加密解密密码
     *      @param options 指定加密选项
     *      @return 返回对称加密的加密对象
     *      
     */
    function createCipher(algorithm: string, key: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description 创建一个对称加密的加密对象
     *      @param algorithm 指定加密算法
     *      @param key 指定加密解密密码
     *      @param iv 指定初始向量
     *      @param options 指定加密选项
     *      @return 返回对称加密的加密对象
     *      
     */
    function createCipheriv(algorithm: string, key: Class_Buffer, iv: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description 创建一个对称加密的加密对象
     *      @param algorithm 指定加密算法
     *      @param key 指定加密解密密码
     *      @param iv 指定初始向量
     *      @param options 指定加密选项
     *      @return 返回对称加密的加密对象
     *      
     */
    function createCipheriv(algorithm: string, key: Class_KeyObject, iv: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description 创建一个对称加密的解密对象
     *      @param algorithm 指定加密算法
     *      @param key 指定加密解密密码
     *      @param options 指定加密选项
     *      @return 返回对称加密的解密对象
     *      
     */
    function createDecipher(algorithm: string, key: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description 创建一个对称加密的解密对象
     *      @param algorithm 指定加密算法
     *      @param key 指定加密解密密码
     *      @param iv 指定初始向量
     *      @param options 指定加密选项
     *      @return 返回对称加密的解密对象
     *      
     */
    function createDecipheriv(algorithm: string, key: Class_Buffer, iv: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description 创建一个对称加密的解密对象
     *      @param algorithm 指定加密算法
     *      @param key 指定加密解密密码
     *      @param iv 指定初始向量
     *      @param options 指定加密选项
     *      @return 返回对称加密的解密对象
     *      
     */
    function createDecipheriv(algorithm: string, key: Class_KeyObject, iv: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description 获取 crypto 模块支持的的 ecc 曲线
     *      @return 返回支持的 ecc 曲线
     *      
     */
    function getCurves(): any[];

    /**
     * @description 创建一个新的密钥对象，其中包含非对称加密的私钥
     *      @param key 指定 pem 格式的私钥
     *      @return 返回私钥的密钥对象
     *      
     */
    function createPrivateKey(key: Class_Buffer): Class_KeyObject;

    /**
     * @description 创建一个新的密钥对象，其中包含非对称加密的私钥
     * 
     *     参数 key 用于指定创建私钥的配置属性，支持的属性包括: 
     *     - key: PEM 字符串，DER 二进制 或者 JWK 格式对象
     *     - format: 必须是 'pem', 'der', 'jwk' 或 'raw'。默认值: 'pem'。Bls12381G1/Bls12381G2 仅支持 'raw'
     *     - type: 必须是 'pkcs1', 'pkcs8' 或 'sec1'。仅当 format 为 'der' 时才需要此选项，否则忽略
     *     - namedCurve: 当 format 为 'raw' 时用于指定 key 的曲线名称，可以是 EC 曲线名，或者 SM2/Ed25519/Ed448/X25519/X448/Bls12381G1/Bls12381G2
     *     - passphrase: 用于解密的密码字符串
     *     - encoding: 当 key 是字符串时使用的字符串编码
     * 
     *      @param key 创建私钥的配置属性
     *      @return 返回私钥的密钥对象
     *      
     */
    function createPrivateKey(key: FIBJS.GeneralObject): Class_KeyObject;

    /**
     * @description 创建一个新的密钥对象，其中包含非对称加密的公钥
     *      @param key 指定 pem 格式的公钥
     *      @return 返回公钥的密钥对象
     *      
     */
    function createPublicKey(key: Class_Buffer): Class_KeyObject;

    /**
     * @description 基于给定的私钥创建一个新的密钥对象，其中包含给定私钥对应的公钥
     *      @param key 指定一个非对称加密的私钥
     *      @return 返回公钥的密钥对象
     *      
     */
    function createPublicKey(key: Class_KeyObject): Class_KeyObject;

    /**
     * @description 创建一个新的密钥对象，其中包含非对称加密的公钥
     * 
     *     参数 key 用于指定创建公钥的配置属性，支持的属性包括: 
     *     - key: PEM 字符串，DER 二进制 或者 JWK 格式对象
     *     - format: 必须是 'pem', 'der', 'jwk' 或 'raw'。默认值: 'pem'
     *     - type: 必须是 'pkcs1', 或 'sec1'。仅当 format 为 'der' 时才需要此选项，否则忽略
     *     - namedCurve: 当 format 为 'raw' 时用于指定 key 的曲线名称，可以是 EC 曲线名，或者 SM2/Ed25519/Ed448/X25519/X448/Bls12381G1/Bls12381G2
     *     - encoding: 当 key 是字符串时使用的字符串编码
     * 
     *      @param key 创建公钥的配置属性
     *      @return 返回公钥的密钥对象
     *      
     */
    function createPublicKey(key: FIBJS.GeneralObject): Class_KeyObject;

    /**
     * @description 基于 algorithm 指定的算法创建一个新的签名对象
     *      @param algorithm 指定签名算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param options 指定签名选项，未使用
     *      @return 返回签名对象
     *      
     */
    function createSign(algorithm: string, options?: FIBJS.GeneralObject): Class_Sign;

    /**
     * @description 基于 algorithm 指定的算法创建一个新的验签对象
     *      @param algorithm 指定验签算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param options 指定验签选项，未使用
     *      @return 返回验签对象
     *      
     */
    function createVerify(algorithm: string, options?: FIBJS.GeneralObject): Class_Verify;

    /**
     * @description 创建一个新的密钥对象，其中包含对称加密或 Hmac 的密钥
     *      @param key 指定加密解密密码
     *      @param encoding 指定密码的编码，缺省为 "buffer"
     *      @return 返回对称加密的解密对象
     *      
     */
    function createSecretKey(key: Class_Buffer, encoding?: string): Class_KeyObject;

    /**
     * @description 创建一个新的密钥对象，其中包含对称加密或 Hmac 的密钥
     *      @param key 指定加密解密密码
     *      @param encoding 指定密码的编码，缺省为 "buffer"
     *      @return 返回对称加密的解密对象
     *      
     */
    function createSecretKey(key: string, encoding: string): Class_KeyObject;

    /**
     * @description 创建一个新的证书请求对象
     *      @param csr 指定 PEM 格式的证书请求的数据
     *      @return 返回证书请求对象
     *     
     */
    function createCertificateRequest(csr: Class_Buffer): Class_X509CertificateRequest;

    /**
     * @description 创建一个新的证书请求对象
     * 
     *      options 内的参数会用于调用 crypto.createPrivateKey 创建私钥对象，此外还支持指定 subject 和 hashAlgorithm。示例如下：
     * 
     *      ```JavaScript
     *         var pk = crypto.createPrivateKey(rsa4096_pem);
     *         var req = crypto.createCertificateRequest({
     *             key: pk,
     *             hashAlgorithm: 'sha256', // 缺省为 'sha256'，如果 key 是 SM2 类型，缺省为 'sm3'
     *             subject: {
     *                 C: "CN",
     *                 O: "baoz.cn",
     *                 CN: "baoz.me"
     *             }
     *         });
     *      ```
     * 
     *      @param options 指定创建证书请求的选项
     *      @return 返回证书请求对象
     *     
     */
    function createCertificateRequest(options: FIBJS.GeneralObject): Class_X509CertificateRequest;

    /**
     * @description 基于 privateKey 和 publicKey 计算 Diffie-Hellman 密钥
     * 
     *      options 支持以下属性:
     *       - privateKey: 用于计算的私钥
     *       - publicKey: 用于计算的公钥
     * 
     *      @param options 指定 Diffie-Hellman 密钥计算的选项
     *      @return 返回 Diffie-Hellman 密钥
     *     
     */
    function diffieHellman(options: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description 用于创建数据的一次性哈希摘要的实用程序。当散列少量可用的数据（<= 5MB）时，它比基于对象的 crypto.createHash() 更快。如果数据很大或者是流式传输，仍然建议使用 crypto.createHash()
     *      @param algorithm 指定签名算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param data 指定要签名的数据
     *      @param outputEncoding 指定输出编码，缺省为 "hex"
     *      @return 返回散列后的数据
     *     
     */
    function hash(algorithm: string, data: Class_Buffer, outputEncoding?: string): any;

    /**
     * @description 生成指定尺寸的随机数，使用 havege 生成器
     *      @param size 指定生成的随机数尺寸
     *      @return 返回生成的随机数
     *      
     */
    function randomBytes(size?: number): Class_Buffer;

    function randomBytes(size?: number, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用随机数填充指定的 Buffer，使用 havege 生成器
     *      @param buffer 指定生成的 Buffer
     *      @param offset 指定起始偏移，缺省为 0
     *      @param size 指定生成的随机数尺寸，缺省为 buffer.length - offset
     *      @return 返回生成的随机数
     *      
     */
    function randomFill(buffer: Class_Buffer, offset?: number, size?: number): Class_Buffer;

    function randomFill(buffer: Class_Buffer, offset?: number, size?: number, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 生成给定 type 的新非对称密钥对。目前支持 RSA、RSA-PSS、DSA、EC、Ed25519、Ed448、X25519、X448、SM2、Bls12381G1、Bls12381G2
     * 
     *     options 支持以下属性:
     *     - modulusLength: 密钥大小（以位为单位）（RSA、DSA）。
     *     - publicExponent: 公共指数 (RSA)。默认值: 0x10001。
     *     - hashAlgorithm: 消息摘要的名称 (RSA-PSS)。
     *     - mgf1HashAlgorithm: MGF1 (RSA-PSS) 使用的消息摘要的名称。
     *     - saltLength: 最小盐长度（以字节为单位）（RSA-PSS）。
     *     - divisorLength: q 的大小（以位 (DSA) 为单位）。
     *     - namedCurve: 要使用的曲线名称 (EC)。
     *     - prime: 主要参数 (DH)。
     *     - primeLength: 质数长度（以位 (DH) 为单位）。
     *     - generator: 自定义生成器 (DH)。默认值: 2。
     *     - groupName: <字符串> Diffie-Hellman 组名称 (DH)。请参阅 crypto.getDiffieHellman。
     *     - paramEncoding: 必须是 'named' 或 'explicit'(EC)。默认值: 'named'。
     *     - publicKeyEncoding: 请参阅 keyObject.export。
     *     - privateKeyEncoding: 请参阅 keyObject.export。
     * 
     *     @param type 指定要生成的密钥类型，必须是 'rsa'、'rsa-pss'、'dsa'、'ec'、'ed25519'、'x25519'、'x448'、'sm2'、'Bls12381G1'、'Bls12381G2'
     *     @param options 指定生成密钥的选项
     *     @return 返回包含生成密钥对的对象
     *     
     */
    function generateKeyPair(type: string, options?: FIBJS.GeneralObject): [publicKey: any, privateKey: any];

    function generateKeyPair(type: string, options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: [publicKey: any, privateKey: any])=>any): void;

    /**
     * @description 依据 rfc5869 根据明文 password 生成要求的二进制钥匙
     *      @param algoName 指定要使用的 hash 算法，详见 hash 模块
     *      @param password 指定使用的密码
     *      @param salt 指定 khdf 使用的 salt
     *      @param info 指定 khdf 使用的 info
     *      @param size 指定钥匙尺寸
     *      @return 返回生成的二进制钥匙
     *      
     */
    function hkdf(algoName: string, password: Class_Buffer, salt: Class_Buffer, info: Class_Buffer, size: number): Class_Buffer;

    function hkdf(algoName: string, password: Class_Buffer, salt: Class_Buffer, info: Class_Buffer, size: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 依据 rfc2898 根据明文 password 生成要求的二进制钥匙
     *      @param password 指定使用的密码
     *      @param salt 指定 hmac 使用的 salt
     *      @param iterations 指定迭代次数
     *      @param size 指定钥匙尺寸
     *      @param algoName 指定要使用的 hash 算法，详见 hash 模块
     *      @return 返回生成的二进制钥匙
     *      
     */
    function pbkdf2(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algoName: string): Class_Buffer;

    function pbkdf2(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algoName: string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 privateKey 解密 buffer。buffer 之前已使用相应的公钥进行加密
     *      @param privateKey 指定私钥
     *      @param buffer 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    function privateDecrypt(privateKey: Class_Buffer, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 privateKey 解密 buffer。buffer 之前已使用相应的公钥进行加密
     *      @param privateKey 指定私钥
     *      @param buffer 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    function privateDecrypt(privateKey: Class_KeyObject, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 key 指定的私钥和配置解密 buffer。buffer 之前已使用相应的公钥进行加密
     *      @param key 指定私钥和配置
     *      @param buffer 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    function privateDecrypt(key: FIBJS.GeneralObject, buffer: any): Class_Buffer;

    /**
     * @description 使用 privateKey 加密 buffer。返回的数据可以使用相应的公钥进行解密
     *      @param privateKey 指定私钥
     *      @param buffer 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    function privateEncrypt(privateKey: Class_Buffer, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 privateKey 加密 buffer。返回的数据可以使用相应的公钥进行解密
     *      @param privateKey 指定私钥
     *      @param buffer 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    function privateEncrypt(privateKey: Class_KeyObject, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 key 指定的私钥和配置加密 buffer。返回的数据可以使用相应的公钥进行解密
     *      @param key 指定私钥和配置
     *      @param buffer 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    function privateEncrypt(key: FIBJS.GeneralObject, buffer: any): Class_Buffer;

    /**
     * @description 使用 publicKey 解密 buffer。buffer 之前已使用相应的私钥加密
     *      @param publicKey 指定公钥
     *      @param buffer 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    function publicDecrypt(publicKey: Class_Buffer, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 publicKey 解密 buffer。buffer 之前已使用相应的私钥加密
     *      @param publicKey 指定公钥
     *      @param buffer 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    function publicDecrypt(publicKey: Class_KeyObject, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 key 指定的公钥和配置解密 buffer。buffer 之前已使用相应的私钥加密
     *      @param key 指定公钥和配置
     *      @param buffer 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    function publicDecrypt(key: FIBJS.GeneralObject, buffer: any): Class_Buffer;

    /**
     * @description 使用 publicKey 加密 buffer。返回的数据可以使用相应的私钥进行解密
     *      @param publicKey 指定私钥
     *      @param buffer 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    function publicEncrypt(publicKey: Class_Buffer, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 publicKey 加密 buffer。返回的数据可以使用相应的私钥进行解密
     *      @param publicKey 指定私钥
     *      @param buffer 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    function publicEncrypt(publicKey: Class_KeyObject, buffer: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 key 指定的私钥和配置加密 buffer。返回的数据可以使用相应的私钥进行解密
     *      @param key 指定私钥和配置
     *      @param buffer 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    function publicEncrypt(key: FIBJS.GeneralObject, buffer: any): Class_Buffer;

    /**
     * @description 使用给定的私钥和算法计算并返回 data 的签名。如果 algorithm 是 null 或 undefined，则算法取决于密钥类型（尤其是 Ed25519 和 Ed448）
     *      @param algorithm 指定签名算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param data 指定要签名的数据
     *      @param privateKey 指定私钥
     *      @return 返回签名后的数据
     *     
     */
    function sign(algorithm: any, data: Class_Buffer, privateKey: Class_Buffer): Class_Buffer;

    function sign(algorithm: any, data: Class_Buffer, privateKey: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用给定的私钥和算法计算并返回 data 的签名。如果 algorithm 是 null 或 undefined，则算法取决于密钥类型（尤其是 Ed25519 和 Ed448）
     *      @param algorithm 指定签名算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param data 指定要签名的数据
     *      @param privateKey 指定私钥
     *      @return 返回签名后的数据
     *     
     */
    function sign(algorithm: any, data: Class_Buffer, privateKey: Class_KeyObject): Class_Buffer;

    function sign(algorithm: any, data: Class_Buffer, privateKey: Class_KeyObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用给定的私钥和算法计算并返回 data 的签名。如果 algorithm 是 null 或 undefined，则算法取决于密钥类型（尤其是 Ed25519 和 Ed448）
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
     *      @param algorithm 指定签名算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param data 指定要签名的数据
     *      @param key 指定私钥和签名参数
     *      @return 返回签名后的数据
     *     
     */
    function sign(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject): Class_Buffer;

    function sign(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用给定的密钥和算法验证 data 的给定签名。如果 algorithm 是 null 或 undefined，则算法取决于密钥类型（尤其是 Ed25519 和 Ed448）
     *      @param algorithm 指定签名算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param data 指定要验证的数据
     *      @param publicKey 指定公钥
     *      @param signature 指定签名数据
     *      @return 返回验证结果
     *     
     */
    function verify(algorithm: any, data: Class_Buffer, publicKey: Class_Buffer, signature: Class_Buffer): boolean;

    function verify(algorithm: any, data: Class_Buffer, publicKey: Class_Buffer, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 使用给定的密钥和算法验证 data 的给定签名。如果 algorithm 是 null 或 undefined，则算法取决于密钥类型（尤其是 Ed25519 和 Ed448）
     *      @param algorithm 指定签名算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param data 指定要验证的数据
     *      @param publicKey 指定公钥
     *      @param signature 指定签名数据
     *      @return 返回验证结果
     *     
     */
    function verify(algorithm: any, data: Class_Buffer, publicKey: Class_KeyObject, signature: Class_Buffer): boolean;

    function verify(algorithm: any, data: Class_Buffer, publicKey: Class_KeyObject, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 使用给定的密钥和算法验证 data 的给定签名。如果 algorithm 是 null 或 undefined，则算法取决于密钥类型（尤其是 Ed25519 和 Ed448）
     * 
     *      key 内的参数会用于调用 crypto.createPublicKey 创建私钥对象，此外还支持以下签名参数：
     *      - dsaEncoding 对于 DSA 和 ECDSA，此选项指定生成的签名的格式。它可以是以下之一: 
     *       - 'der'（默认）: DER 编码的 ASN.1 签名结构编码 (r, s) 
     *       - 'ieee-p1363' : IEEE-P1363 中提议的签名格式 r || s
     *      - padding RSA 的可选填充值，以下之一: 
     *       - RSA_PKCS1_PADDING（默认）
     *       - RSA_PKCS1_PSS_PADDING，RSA_PKCS1_PSS_PADDING 将使用 MGF1，其哈希函数与用于对 RFC 4055 第 3.1 节中指定的消息进行签名的哈希函数相同
     *      - saltLength 当填充为 RSA_PKCS1_PSS_PADDING 时的盐长度。特殊值 RSA_PSS_SALTLEN_DIGEST 将盐长度设置为摘要大小，RSA_PSS_SALTLEN_MAX_SIGN（默认）将其设置为最大允许值
     * 
     *      @param algorithm 指定签名算法，使用 crypto.getHashes 获取可用摘要算法的名称
     *      @param data 指定要验证的数据
     *      @param key 指定私钥和签名参数
     *      @param signature 指定签名数据
     *      @return 返回验证结果
     *     
     */
    function verify(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject, signature: Class_Buffer): boolean;

    function verify(algorithm: any, data: Class_Buffer, key: FIBJS.GeneralObject, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 使用 Bls12381G2 进行 BBS 签名的函数
     *      @param messages 指定要签名的一组消息
     *      @param privateKey 指定私钥，必须是 Bls12381G2 的私钥
     *      @return 返回签名后的数据
     *     
     */
    function bbsSign(messages: any[], privateKey: Class_Buffer): Class_Buffer;

    function bbsSign(messages: any[], privateKey: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 Bls12381G2 进行 BBS 签名的函数
     *      @param messages 指定要签名的一组消息
     *      @param privateKey 指定私钥，必须是 Bls12381G2 的私钥
     *      @return 返回签名后的数据
     *     
     */
    function bbsSign(messages: any[], privateKey: Class_KeyObject): Class_Buffer;

    function bbsSign(messages: any[], privateKey: Class_KeyObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 Bls12381G2 进行 BBS 签名的函数
     * 
     *      key 内的参数会用于调用 crypto.createPrivateKey 创建私钥对象，此外还支持以下签名参数：
     *       - suite: 必须是 'Bls12381Sha256', 'Bls12381Shake256'。默认值: 'Bls12381Sha256'
     *       - header: 用于签名的附加数据
     * 
     *      @param messages 指定要签名的一组消息
     *      @param key 指定私钥和选项
     *      @return 返回签名后的数据
     *     
     */
    function bbsSign(messages: any[], key: FIBJS.GeneralObject): Class_Buffer;

    function bbsSign(messages: any[], key: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 Bls12381G2 进行 BBS 验证的函数
     *      @param messages 指定要验证的一组消息
     *      @param publicKey 指定公钥，必须是 Bls12381G2 的公钥
     *      @param signature 指定签名数据
     *      @return 返回验证结果
     *     
     */
    function bbsVerify(messages: any[], publicKey: Class_Buffer, signature: Class_Buffer): boolean;

    function bbsVerify(messages: any[], publicKey: Class_Buffer, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 使用 Bls12381G2 进行 BBS 验证的函数
     *      @param messages 指定要验证的一组消息
     *      @param publicKey 指定公钥，必须是 Bls12381G2 的公钥
     *      @param signature 指定签名数据
     *      @return 返回验证结果
     *     
     */
    function bbsVerify(messages: any[], publicKey: Class_KeyObject, signature: Class_Buffer): boolean;

    function bbsVerify(messages: any[], publicKey: Class_KeyObject, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 使用 Bls12381G2 进行 BBS 验证的函数
     * 
     *      key 内的参数会用于调用 crypto.createPublicKey 创建公钥对象，此外还支持以下签名参数：
     *       - suite: 必须是 'Bls12381Sha256', 'Bls12381Shake256'。默认值: 'Bls12381Sha256'
     *       - header: 用于签名的附加数据
     * 
     *      @param messages 指定要验证的一组消息
     *      @param key 指定公钥和选项
     *      @param signature 指定签名数据
     *      @return 返回验证结果
     *     
     */
    function bbsVerify(messages: any[], key: FIBJS.GeneralObject, signature: Class_Buffer): boolean;

    function bbsVerify(messages: any[], key: FIBJS.GeneralObject, signature: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 使用 Bls12381G2 生成 BBS 选择证明的函数
     *      @param signature 指定 BBS 签名
     *      @param messages 指定要签名的一组消息
     *      @param index 指定要选择的证明的索引
     *      @param privateKey 指定私钥，必须是 Bls12381G2 的公钥
     *      @return 返回证明数据
     *     
     */
    function proofGen(signature: Class_Buffer, messages: any[], index: any[], publicKey: Class_Buffer): Class_Buffer;

    function proofGen(signature: Class_Buffer, messages: any[], index: any[], publicKey: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 Bls12381G2 生成 BBS 选择证明的函数
     *      @param signature 指定 BBS 签名
     *      @param messages 指定要签名的一组消息
     *      @param index 指定要选择的证明的索引
     *      @param privateKey 指定私钥，必须是 Bls12381G2 的公钥
     *      @return 返回证明数据
     *     
     */
    function proofGen(signature: Class_Buffer, messages: any[], index: any[], publicKey: Class_KeyObject): Class_Buffer;

    function proofGen(signature: Class_Buffer, messages: any[], index: any[], publicKey: Class_KeyObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 Bls12381G2 生成 BBS 选择证明的函数
     * 
     *      key 内的参数会用于调用 crypto.createPublicKey 创建公钥对象，此外还支持以下签名参数：
     *       - suite: 必须是 'Bls12381Sha256', 'Bls12381Shake256'。默认值: 'Bls12381Sha256'
     *       - header: 用于签名的附加数据
     *       - proof_header: 用于证明的附加数据
     * 
     *      @param signature 指定 BBS 签名
     *      @param messages 指定要签名的一组消息
     *      @param index 指定要选择的证明的索引
     *      @param key 指定公钥和选项
     *      @return 返回证明数据
     *     
     */
    function proofGen(signature: Class_Buffer, messages: any[], index: any[], key: FIBJS.GeneralObject): Class_Buffer;

    function proofGen(signature: Class_Buffer, messages: any[], index: any[], key: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 Bls12381G2 验证 BBS 选择证明的函数
     *      @param messages 指定要验证的一组消息
     *      @param index 指定要选择的证明的索引
     *      @param publicKey 指定公钥，必须是 Bls12381G2 的公钥
     *      @param proof 指定证明数据
     *      @return 返回验证结果
     *     
     */
    function proofVerify(messages: any[], index: any[], publicKey: Class_Buffer, proof: Class_Buffer): boolean;

    function proofVerify(messages: any[], index: any[], publicKey: Class_Buffer, proof: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 使用 Bls12381G2 验证 BBS 选择证明的函数
     *      @param messages 指定要验证的一组消息
     *      @param index 指定要选择的证明的索引
     *      @param publicKey 指定公钥，必须是 Bls12381G2 的公钥
     *      @param proof 指定证明数据
     *      @return 返回验证结果
     *     
     */
    function proofVerify(messages: any[], index: any[], publicKey: Class_KeyObject, proof: Class_Buffer): boolean;

    function proofVerify(messages: any[], index: any[], publicKey: Class_KeyObject, proof: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 使用 Bls12381G2 验证 BBS 选择证明的函数
     * 
     *      key 内的参数会用于调用 crypto.createPublicKey 创建公钥对象，此外还支持以下签名参数：
     *       - suite: 必须是 'Bls12381Sha256', 'Bls12381Shake256'。默认值: 'Bls12381Sha256'
     *       - header: 用于签名的附加数据
     *       - proof_header: 用于证明的附加数据
     * 
     *      @param messages 指定要验证的一组消息
     *      @param index 指定要选择的证明的索引
     *      @param key 指定公钥和选项
     *      @param proof 指定证明数据
     *      @return 返回验证结果
     *     
     */
    function proofVerify(messages: any[], index: any[], key: FIBJS.GeneralObject, proof: Class_Buffer): boolean;

    function proofVerify(messages: any[], index: any[], key: FIBJS.GeneralObject, proof: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description WebCrypto API 模块 
     */
    const webcrypto: typeof import ('webcrypto');

}

