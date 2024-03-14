/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 对称加密算法对象 
 */
declare class Class_Cipher extends Class_object {
    /**
     * @description 设置认证标签
     *      @param buffer 指定认证标签数据
     *      @param encoding 指定认证标签数据编码方式
     *      @return 返回当前 Cipher 对象
     *      
     */
    setAuthTag(buffer: Class_Buffer, encoding?: string): Class_Cipher;

    /**
     * @description 设置认证标签
     *      @param buffer 指定认证标签数据
     *      @param encoding 指定认证标签数据编码方式
     *      @return 返回当前 Cipher 对象
     *      
     */
    setAuthTag(buffer: string, encoding?: string): Class_Cipher;

    /**
     * @description 查询认证标签 
     *       @return 返回认证标签数据
     *     
     */
    getAuthTag(): Class_Buffer;

    /**
     * @description 设置附加身份验证数据
     *      @param buffer 指定附加身份验证数据
     *      @param options 指定附加身份验证数据选项
     *      @return 返回当前 Cipher 对象
     *     
     */
    setAAD(buffer: Class_Buffer, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description 设置附加身份验证数据
     *      @param buffer 指定附加身份验证数据
     *      @param options 指定附加身份验证数据选项
     *      @return 返回当前 Cipher 对象
     *     
     */
    setAAD(buffer: string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description 设置自动填充
     *      @param autoPadding 指定是否自动填充
     *      @return 返回当前 Cipher 对象
     *     
     */
    setAutoPadding(autoPadding?: boolean): Class_Cipher;

    /**
     * @description 更新数据
     *       @param data 指定要更新的数据
     *       @param inputEncoding 指定输入数据编码方式
     *       @param outputEncoding 指定输出数据编码方式
     *       @return 返回更新后的数据
     *     
     */
    update(data: Class_Buffer, inputEncoding?: string, outputEncoding?: string): any;

    /**
     * @description 更新数据
     *       @param data 指定要更新的数据
     *       @param inputEncoding 指定输入数据编码方式
     *       @param outputEncoding 指定输出数据编码方式
     *       @return 返回更新后的数据
     *     
     */
    update(data: string, inputEncoding?: string, outputEncoding?: string): any;

    /**
     * @description 终止数据
     *       @param outputEncoding 指定输出数据编码方式
     *       @return 返回更新后的数据
     *     
     */
    final(outputEncoding?: string): any;

}

