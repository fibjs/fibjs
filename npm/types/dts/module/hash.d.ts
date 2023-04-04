/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Digest.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/**
 * @description hash 模块主要用于实现消息摘要算法，是实现加密算法中，非常重要的一个子模块。同时，消息摘要算法也是许多加密算法的基础。
 * 
 * 在使用 `hash` 子模块之前，需要使用 `require('hash');` 加载该模块。
 * 
 * 在 `hash` 模块中，主要包含以下几个类 `Digest`、`Hmac`。其中，`Digest` 类用于计算消息摘要算法，`Hmac` 类用于计算带密钥的哈希消息。 
 * 
 * 以下是一个简单的使用 `Digest` 类计算 `MD5` 摘要的示例代码：
 * 
 * ```javascript
 * const hash = require('hash');
 * 
 * // 创建 `MD5` digest 对象
 * const digest = hash.digest(hash.MD5);
 * 
 * // 更新数据
 * const data = 'hello, world';
 * digest.update(data);
 * 
 * // 获取摘要结果
 * const result = digest.digest('hex');
 * console.log(result);
 * ```
 * 
 * 在以上示例代码中，我们首先使用 `hash.digest(hash.MD5)` 将一个 `Digest` 对象初始化为 `MD5` 模式。然后，我们使用 `update()` 方法向 Digest 对象添加需要进行摘要计算的数据。接着，我们使用 `digest()` 方法计算摘要结果，并返回通过参数指定的编码方式（这里使用的是 十六进制编码）。最后，输出计算出的摘要结果。
 * 
 * 除了 `MD5` 以外，`Digest` 类还支持其他摘要算法，比如 `SHA-1`、`SHA-256`、`SHA-512`等。 在使用时需要将 `md5` 参数改成对应的摘要算法即可。
 * 
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

