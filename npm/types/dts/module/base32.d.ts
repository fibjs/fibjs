/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description base32 编码与解码模块
 *  
 * `base32`模块是一个用于 base32 编码和解码的模块。Base32 是一种用于将二进制数据编码为 ASCII 字符串的算法，用于将二进制数据在邮件、DNS 等网络协议中传输。
 * 
 * 该模块提供了两个方法：`encode` 和 `decode`。其中 `encode` 方法用于将二进制数据编码为 Base32 字符串，`decode` 方法用于将 Base32 字符串解码为二进制数据。下面是使用示例：
 * 
 *  ```JavaScript
 *     const base32 = require('base32');
 *     const data = new Uint8Array([0x4e, 0x4f, 0x44, 0x45]); // 'NODE'
 *     const encoded = base32.encode(data); // 'KRUGKIDROV======'
 *     const decoded = base32.decode(encoded); // [0x4e, 0x4f, 0x44, 0x45]
 *     console.log(encoded, decoded); // KRUGKIDROV====== [78, 79, 68, 69]
 *  ```
 *  可以看到，`encode` 方法将二进制数据编码为了 `KRUGKIDROV======`，而 `decode` 方法将其解码为了 `[0x4e, 0x4f, 0x44, 0x45]`。
 *  
 *  需要注意的是，Base32 编码的结果字符串长度是原二进制数据长度的约 8/5 倍，因此不适合用于对大量数据的编码。如果需要对大量数据进行编码，建议使用 Base64 编码。
 *   
 */
declare module 'base32' {
    /**
     * @description 以 base32 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: Class_Buffer): string;

    /**
     * @description 以 base32 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string): Class_Buffer;

}

