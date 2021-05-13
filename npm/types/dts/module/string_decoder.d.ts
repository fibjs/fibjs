/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/StringDecoder.d.ts" />
/**
 * @description 解码 Buffer 到字符串
 * 
 *  使用方法：
 *  ```JavaScript
 *   const { StringDecoder } = require('string_decoder');
 *   const decoder = new StringDecoder('utf8');
 * 
 *   const cent = Buffer.from([0xC2, 0xA2]);
 *   console.log(decoder.write(cent));
 * 
 *   const euro = Buffer.from([0xE2, 0x82, 0xAC]);
 *   console.log(decoder.write(euro));
 *  ```
 *  
 */
declare module 'string_decoder' {
    /**
     * @description 创建一个解码对象，参见 StringDecoder 
     */
    const StringDecoder: typeof Class_StringDecoder;

}

