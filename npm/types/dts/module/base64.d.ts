/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description base64 编码与解码模块
 *  
 *  `base64` 是一种将二进制数据编码为 ASCII 字符串的方法，使其可在网络上进行传输。`base64` 模块提供了一些对 Base64 编解码的支持。
 * 
 *  使用 `base64` 模块，可以将字符串编码为 Base64 格式，也可以将 Base64 格式解码为字符串。例如，将字符串编码为 Base64 格式：
 *  ```JavaScript
 *  const { encode } = require('base64');
 *  const str = 'hello, world';
 *  const encodedStr = encode(str);
 *  console.log(encodedStr); // ==> "aGVsbG8sIHdvcmxk"
 *  ```
 *  将 Base64 格式的字符串解码为字符串：
 *  ```JavaScript
 *  const { decode } = require('base64');
 *  const encodedStr = 'aGVsbG8sIHdvcmxk';
 *  const str = decode(encodedStr);
 *  console.log(str); // ==> "hello, world"
 *  ```
 *  在处理包含敏感信息的数据时，使用 Base64 编码并不能提供安全保障。因为 Base64 编码可以被轻松地破解，所以应该使用其他更安全的方法来处理这些数据。
 *  
 */
declare module 'base64' {
    /**
     * @description 以 base64 方式编码数据
     * 	 @param data 要编码的数据
     * 	 @param url 指定是否使用 url 安全字符编码
     * 	 @return 返回编码的字符串
     * 	 
     */
    function encode(data: Class_Buffer, url?: boolean): string;

    /**
     * @description 以 base64 方式解码字符串为二进制数据
     * 	 @param data 要解码的字符串
     * 	 @return 返回解码的二进制数据
     * 	 
     */
    function decode(data: string): Class_Buffer;

}

