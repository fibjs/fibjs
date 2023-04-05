/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 对称加密算法对象
 * 
 *  Cipher 对象属于 crypto 模块，创建：
 *  ```JavaScript
 *  var c = new crypto.Cipher(crypto.AES, crypto.ECB, ...);
 *  ```
 * 
 *   下面以 AES 为例，演示如何使用 Cipher 对象进行加密和解密：
 * 
 *   首先，我们需要获取一个对称加密的 Cipher 对象，如 AES 加密算法：
 * ```JavaScript
 * const crypto = require('crypto');
 * const key = crypto.randomBytes(16); // generate a 16-byte random key
 * const cipher = new crypto.Cipher(crypto.AES, crypto.ECB, key);
 * ```
 * 以上代码中，我们通过 ``randomBytes`` 方法生成了 16 字节的随机数，作为 AES 密钥。然后，我们创建了一个 AES 算法的 Cipher 对象，使用 ECB 分组密码工作模式，并使用生成的随机数作为密钥。
 * 
 * 接下来，我们使用 Cipher 对象的 ``encrypt`` 方法对明文进行加密：
 * ```JavaScript
 * const plaintext = 'Hello, world!';
 * const encrypted = cipher.encrypt(plaintext);
 * console.log(encrypted); // output encrypted data
 * ```
 * 以上代码中，我们使用 ``encrypt`` 方法对字符串 ``Hello, world!`` 进行加密，并输出加密后的结果。
 * 
 * 针对以上代码，需要注意以下几点：
 * 
 * - 在进行加密之前，需要先创建一个对称加密的 Cipher 对象，并设置加密算法、密码和分组密码工作模式等参数。
 * - 在调用加密方法后，我们可以得到加密后的数据，这通常是一个 Buffer 对象。需要注意，输出的是二进制数据，需要使用相应的方式打印，例如使用 ``Hex`` 等方式进行转换。
 * 
 * 如果要解密该数据，我们同样需要一个 Cipher 对象，也是基于 AES 算法，且使用相同的 key 和分组密码工作模式：
 * ```JavaScript
 * const decipher = new crypto.Cipher(crypto.AES, crypto.ECB, key);
 * const decrypted = decipher.decrypt(encrypted);
 * console.log(decrypted.toString()); // output decrypted data
 * ```
 * 在这里，我们创建了一个新的 Cipher 对象，使用相同的 key 和密码工作模式，并且使用 ``decrypt`` 方法对加密数据进行解密。解密的结果是一个 Buffer 对象，需要转换为字符串才能被正确输出。    
 *  
 */
declare class Class_Cipher extends Class_object {
    /**
     * @description Cipher 构造函数，仅用于 ARC4 初始化
     *      @param provider 指定加密算法
     *      @param key 指定加密解密密码
     *     
     */
    constructor(provider: number, key: Class_Buffer);

    /**
     * @description Cipher 构造函数
     *      @param provider 指定加密算法
     *      @param mode 指定分组密码工作模式
     *      @param key 指定加密解密密码
     *     
     */
    constructor(provider: number, mode: number, key: Class_Buffer);

    /**
     * @description Cipher 构造函数
     *      @param provider 指定加密算法
     *      @param mode 指定分组密码工作模式
     *      @param key 指定加密解密密码
     *      @param iv 指定初始向量
     *     
     */
    constructor(provider: number, mode: number, key: Class_Buffer, iv: Class_Buffer);

    /**
     * @description 返回当前算法名称 
     */
    readonly name: string;

    /**
     * @description 返回当前算法密码长度，以位为单位 
     */
    readonly keySize: number;

    /**
     * @description 返回当前算法初始向量长度，以字节为单位 
     */
    readonly ivSize: number;

    /**
     * @description 返回当前算法数据块长度，以字节为单位 
     */
    readonly blockSize: number;

    /**
     * @description 使用填充模式
     *      @param mode 指定填充模式，缺省为 PADDING_PKCS7
     *      
     */
    paddingMode(mode: number): void;

    /**
     * @description 使用当前算法密码加密数据
     *      @param data 指定要加密的数据
     *      @return 返回加密后的数据
     *      
     */
    encrypt(data: Class_Buffer): Class_Buffer;

    encrypt(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用当前算法密码解密数据
     *      @param data 指定要解密的数据
     *      @return 返回解密后的数据
     *      
     */
    decrypt(data: Class_Buffer): Class_Buffer;

    decrypt(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

}

