/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Digest.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/**
 * @description 信息摘要计算模块，可用于计算信息摘要和摘要签名 
 */
declare module 'hash' {
    /**
     * @description MD5 信息摘要算法标识常量 
     */
    export const MD5: 1;

    /**
     * @description SHA1 信息摘要算法标识常量 
     */
    export const SHA1: 2;

    /**
     * @description SHA224 信息摘要算法标识常量 
     */
    export const SHA224: 3;

    /**
     * @description SHA256 信息摘要算法标识常量 
     */
    export const SHA256: 4;

    /**
     * @description SHA384 信息摘要算法标识常量 
     */
    export const SHA384: 5;

    /**
     * @description SHA512 信息摘要算法标识常量 
     */
    export const SHA512: 6;

    /**
     * @description RIPEMD160 信息摘要算法标识常量 
     */
    export const RIPEMD160: 7;

    /**
     * @description SM3 信息摘要算法标识常量 
     */
    export const SM3: 8;

    /**
     * @description KECCAK256 信息摘要算法标识常量 
     */
    export const KECCAK256: 9;

    /**
     * @description KECCAK384 信息摘要算法标识常量 
     */
    export const KECCAK384: 10;

    /**
     * @description KECCAK512 信息摘要算法标识常量 
     */
    export const KECCAK512: 11;

    /**
     * @description BLAKE2S 信息摘要算法标识常量 
     */
    export const BLAKE2S: 12;

    /**
     * @description BLAKE2B 信息摘要算法标识常量 
     */
    export const BLAKE2B: 13;

    /**
     * @description BLAKE2SP 信息摘要算法标识常量 
     */
    export const BLAKE2SP: 14;

    /**
     * @description BLAKE2BP 信息摘要算法标识常量 
     */
    export const BLAKE2BP: 15;

    /**
     * @description 根据指定的算法标识创建一个信息摘要运算对象
     *      @param algo 指定摘要运算算法
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function digest(algo: number, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 MD5 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function md5(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA1 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function sha1(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA224 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function sha224(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA256 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function sha256(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA384 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function sha384(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA512 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function sha512(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 RIPEMD160 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function ripemd160(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SM3 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function sm3(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SM3 信息摘要运算对象并进行预处理
     *      @param pubKey 签名公钥
     *      @param id 签名 ID
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function sm3(pubKey: Class_PKey, id: string, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 KECCAK256 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function keccak256(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 KECCAK384 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function keccak384(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 KECCAK512 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function keccak512(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 BLAKE2S 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function blake2s(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 BLAKE2B 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function blake2b(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 BLAKE2SP 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function blake2sp(data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 BLAKE2BP 信息摘要运算对象
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function blake2bp(data?: Class_Buffer): Class_Digest;

    /**
     * @description 根据指定的算法标识创建一个信息摘要签名运算对象
     *      @param algo 指定摘要运算算法
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac(algo: number, key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 MD5 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_md5(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA1 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_sha1(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA224 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_sha224(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA256 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_sha256(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA384 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_sha384(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SHA512 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_sha512(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 RIPEMD160 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_ripemd160(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SM3 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_sm3(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 SM3 信息摘要签名运算对象并进行预处理
     *      @param pubKey 签名公钥
     *      @param id 签名 ID
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_sm3(pubKey: Class_PKey, id: string, key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 KECCAK256 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_keccak256(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 KECCAK384 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_keccak384(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 KECCAK512 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_keccak512(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 BLAKE2S 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_blake2s(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 BLAKE2B 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_blake2b(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 BLAKE2SP 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_blake2sp(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

    /**
     * @description 创建一个 BLAKE2BP 信息摘要签名运算对象
     *      @param key 二进制签名密钥
     *      @param data 创建同时更新的二进制数据，缺省为 null，不更新数据
     *      @return 返回构造的信息摘要对象
     *      
     */
    function hmac_blake2bp(key: Class_Buffer, data?: Class_Buffer): Class_Digest;

}

