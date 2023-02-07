/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Cipher.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/// <reference path="../interface/ECKey.d.ts" />
/// <reference path="../interface/BlsKey.d.ts" />
/// <reference path="../interface/X509Cert.d.ts" />
/// <reference path="../interface/X509Crl.d.ts" />
/// <reference path="../interface/X509Req.d.ts" />
/// <reference path="../interface/Digest.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 加密算法模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  var crypto = require('crypto');
 *  ```
 *  
 */
declare module 'crypto' {
    /**
     * @description 指定对称加密算法 AES，支持 128, 192, 256 位 key，分组密码工作模式支持 ECB, CBC, CFB128, CTR, GCM，CCM，XTS 
     */
    export const AES: 1;

    /**
     * @description 指定对称加密算法 DES，支持 64 位 key，分组密码工作模式支持 ECB, CBC 
     */
    export const DES: 2;

    /**
     * @description 指定对称加密算法 DES-EDE3，支持 192 位 key，分组密码工作模式支持 ECB, CBC 
     */
    export const DES_EDE3: 3;

    /**
     * @description 指定对称加密算法 CAMELLIA，支持 128, 192, 256 位 key，分组密码工作模式支持 ECB, CBC, CFB128, CTR, GCM，CCM 
     */
    export const CAMELLIA: 4;

    /**
     * @description 指定对称加密算法 ARIA，支持 128, 192, 256 位 key，分组密码工作模式支持 ECB, CBC, CFB128, CTR, GCM，CCM 
     */
    export const ARIA: 5;

    /**
     * @description 指定对称加密算法 CHACHA20，支持 256 位 key，分组密码工作模式支持 POLY1305 
     */
    export const CHACHA20: 6;

    /**
     * @description 指定对称加密算法 SM4, 分组密码工作模式支持 ECB, CBC 
     */
    export const SM4: 7;

    /**
     * @description 指定分组密码工作模式支持 ECB 
     */
    export const ECB: 1;

    /**
     * @description 指定分组密码工作模式支持 CBC 
     */
    export const CBC: 2;

    /**
     * @description 指定分组密码工作模式支持 CFB64 
     */
    export const CFB64: 3;

    /**
     * @description 指定分组密码工作模式支持 CFB128 
     */
    export const CFB128: 4;

    /**
     * @description 指定分组密码工作模式支持 OFB 
     */
    export const OFB: 5;

    /**
     * @description 指定分组密码工作模式支持 CTR 
     */
    export const CTR: 6;

    /**
     * @description 指定分组密码工作模式支持 GCM 
     */
    export const GCM: 7;

    /**
     * @description 指定流密码模式 
     */
    export const STREAM: 8;

    /**
     * @description 指定分组密码工作模式支持 CCM 
     */
    export const CCM: 9;

    /**
     * @description 指定分组密码工作模式支持 XTS 
     */
    export const XTS: 10;

    /**
     * @description 指定分组密码工作模式支持 POLY1305 
     */
    export const POLY1305: 11;

    /**
     * @description 指定填充模式为 PKCS7 
     */
    export const PKCS7: 0;

    /**
     * @description 指定填充模式为 ONE_AND_ZEROS 
     */
    export const ONE_AND_ZEROS: 1;

    /**
     * @description 指定填充模式为 ZEROS_AND_LEN 
     */
    export const ZEROS_AND_LEN: 2;

    /**
     * @description 指定填充模式为 ZEROS 
     */
    export const ZEROS: 3;

    /**
     * @description 指定填充模式为 NOPADDING 
     */
    export const NOPADDING: 4;

    /**
     * @description Cipher 构造函数，参见 Cipher 
     */
    const Cipher: typeof Class_Cipher;

    /**
     * @description PKey 构造函数，参见 PKey 
     */
    const PKey: typeof Class_PKey;

    /**
     * @description ECKey 构造函数，参见 ECKey 
     */
    const ECKey: typeof Class_ECKey;

    /**
     * @description BlsKey 构造函数，参见 BlsKey 
     */
    const BlsKey: typeof Class_BlsKey;

    /**
     * @description X509Cert 构造函数，参见 X509Cert 
     */
    const X509Cert: typeof Class_X509Cert;

    /**
     * @description X509Crl 构造函数，参见 X509Crl 
     */
    const X509Crl: typeof Class_X509Crl;

    /**
     * @description X509Req 构造函数，参见 X509Req 
     */
    const X509Req: typeof Class_X509Req;

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
     * @description 加载一个 CRT/PEM/DER 格式的证书，可多次调用
     *      @param filename 证书文件名
     *      @return 返回包含证书的对象
     *     
     */
    function loadCert(filename: string): Class_X509Cert;

    /**
     * @description 加载一个 PEM/DER 格式的撤销证书，可多次调用
     *      @param filename 撤销证书文件名
     *      @return 返回包含撤销证书的对象
     *     
     */
    function loadCrl(filename: string): Class_X509Crl;

    /**
     * @description 加载一个 PEM/DER 格式的证书请求，可多次调用
     *      @param filename 证书请求文件名
     *      @return 返回包含请求证书的对象
     *     
     */
    function loadReq(filename: string): Class_X509Req;

    /**
     * @description 生成指定尺寸的随机数，使用 havege 生成器
     *      @param size 指定生成的随机数尺寸
     *      @return 返回生成的随机数
     *      
     */
    function randomBytes(size: number): Class_Buffer;

    function randomBytes(size: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 生成指定尺寸的低强度随机数，使用快速的算法
     *      @param size 指定生成的随机数尺寸
     *      @return 返回生成的随机数
     *      
     */
    function simpleRandomBytes(size: number): Class_Buffer;

    function simpleRandomBytes(size: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 生成指定尺寸的伪随机数，使用 entropy 生成器
     *      @param size 指定生成的随机数尺寸
     *      @return 返回生成的随机数
     *      
     */
    function pseudoRandomBytes(size: number): Class_Buffer;

    function pseudoRandomBytes(size: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

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
     * @description 生成给定数据的可视化字符图像
     *      @param data 指定要展示的数据
     *      @param title 指定字符图像的标题，多字节字符会导致宽度错误
     *      @param size 字符图像尺寸
     *      @return 返回生成的可视化字符串图像
     *      
     */
    function randomArt(data: Class_Buffer, title: string, size?: number): string;

    /**
     * @description 生成一个 RSA 私钥
     *      @param size 指定 RSA 密钥长度，bit 为单位
     *      @return 返回包含生成私钥的对象
     *     
     */
    function generateKey(size: number): Class_PKey;

    function generateKey(size: number, callback: (err: Error | undefined | null, retVal: Class_PKey)=>any): void;

    /**
     * @description 生成一个椭圆曲线私钥
     * 
     *      curve 可选的曲线包含 NIST 曲线和别名如下：
     * 
     *      | 曲线 | 别名 |
     *      | ----------- | ----------- |
     *      | NIST P-192 | 'NIST P-192', 'p192', 'P-192', 'prime192v1', 'secp192r1' |
     *      | NIST P-224 | 'NIST P-224', 'p224', 'P-224', 'prime224v1', 'secp224r1' |
     *      | NIST P-256 | 'NIST P-256', 'p256', 'P-256', 'prime256v1', 'secp256r1' |
     *      | NIST P-384 | 'NIST P-384', 'p384', 'P-384', 'prime384v1', 'secp384r1' |
     *      | NIST P-521 | 'NIST P-521', 'p521', 'P-521', 'prime521v1', 'secp521r1' |
     * 
     *      其它支持的曲线包括：
     *      "brainpoolP512r1", "brainpoolP384r1", "secp256k1", "P-256K", "brainpoolP256r1",
     *      "sm2p256r1", "SM2", "Ed25519", "BLS12381_G1", "BLS12381_G2"
     * 
     *      @param curve 指定预置椭圆曲线，缺省为 'secp256r1'
     *      @return 返回包含生成私钥的对象
     *     
     */
    function generateKey(curve?: string): Class_PKey;

    function generateKey(curve?: string, callback?: (err: Error | undefined | null, retVal: Class_PKey)=>any): void;

    /**
     * @description 依据 pbkdf1 根据明文 password 生成要求的二进制钥匙
     *      @param password 指定使用的密码
     *      @param salt 指定 hmac 使用的 salt
     *      @param iterations 指定迭代次数
     *      @param size 指定钥匙尺寸
     *      @param algo 指定要使用的 hash 算法，详见 hash 模块
     *      @return 返回生成的二进制钥匙
     *      
     */
    function pbkdf1(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algo: number): Class_Buffer;

    function pbkdf1(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algo: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 依据 pbkdf1 根据明文 password 生成要求的二进制钥匙
     *      @param password 指定使用的密码
     *      @param salt 指定 hmac 使用的 salt
     *      @param iterations 指定迭代次数
     *      @param size 指定钥匙尺寸
     *      @param algoName 指定要使用的 hash 算法，详见 hash 模块
     *      @return 返回生成的二进制钥匙
     *      
     */
    function pbkdf1(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algoName: string): Class_Buffer;

    function pbkdf1(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algoName: string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 依据 rfc2898 根据明文 password 生成要求的二进制钥匙
     *      @param password 指定使用的密码
     *      @param salt 指定 hmac 使用的 salt
     *      @param iterations 指定迭代次数
     *      @param size 指定钥匙尺寸
     *      @param algo 指定要使用的 hash 算法，详见 hash 模块
     *      @return 返回生成的二进制钥匙
     *      
     */
    function pbkdf2(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algo: number): Class_Buffer;

    function pbkdf2(password: Class_Buffer, salt: Class_Buffer, iterations: number, size: number, algo: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

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
     * @description 获取 crypto 模块支持的的 hash(摘要) 算法, 比如 'md5', 'sha224'
     *      @return 返回 fibjs 支持的 hash 算法数组
     *      
     */
    function getHashes(): any[];

}

