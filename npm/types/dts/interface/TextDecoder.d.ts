/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description TextDecoder 解码对象
 * 
 *  创建方法
 *  ```JavaScript
 *  var textDecoder = new util.TextDecoder('utf8');
 *  ```
 *  
 */
declare class Class_TextDecoder extends Class_object {
    /**
     * @description TextDecoder 对象构造函数，使用参数构造
     * 	 @param codec 指定解码字符集
     *  	 @param opts 指定解码选项
     *      
     */
    constructor(codec?: string, opts?: FIBJS.GeneralObject);

    /**
     * @description 将二进制数据转换为文本
     * 	 @param data 要转换的二进制
     *  	 @param opts 指定解码选项
     * 	 @return 返回解码的文本
     * 	 
     */
    decode(data: Class_Buffer, opts?: FIBJS.GeneralObject): string;

    /**
     * @description 将二进制数据转换为文本
     * 	 @return 返回解码的文本
     * 	 
     */
    decode(): string;

    /**
     * @description 查询当前的编码字符集 
     */
    readonly encoding: string;

}

