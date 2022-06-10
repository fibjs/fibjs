/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description TextEncoder 编码对象
 * 
 *  创建方法
 *  ```JavaScript
 *  var textEncoder = new util.TextEncoder('utf8');
 *  ```
 *  
 */
declare class Class_TextEncoder extends Class_object {
    /**
     * @description TextEncoder 对象构造函数，使用参数构造
     * 	 @param codec 指定编码字符集
     *  	 @param opts 指定编码选项
     *      
     */
    constructor(codec?: string, opts?: FIBJS.GeneralObject);

    /**
     * @description 将文本转换为二进制数据
     * 	 @param data 要转换的文本
     *  	 @param opts 指定编码选项
     * 	 @return 返回编码的二进制数据
     * 	 
     */
    encode(data?: string, opts?: FIBJS.GeneralObject): Class_Buffer;

    /**
     * @description 查询当前的编码字符集 
     */
    readonly encoding: string;

}

