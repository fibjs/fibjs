/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
declare class Class_Blob extends Class_object {
    /**
     * @description Blob 对象构造函数
     *      @param blobParts 初始化数据数组，可以包含字符串、ArrayBuffer、TypedArray、Blob 等
     *      @param options 选项对象，包含 type 和 endings 属性
     *      
     */
    constructor(blobParts?: any[], options?: FIBJS.GeneralObject);

    /**
     * @description Blob 对象类型 
     */
    readonly type: string;

    /**
     * @description Blob 对象的大小 
     */
    readonly size: number;

    /**
     * @description 返回指定范围的 Blob 切片
     *      @param start 起始位置（可选）
     *      @param end 结束位置（可选）
     *      @param contentType 内容类型（可选）
     *      @return 返回新的 Blob 对象
     *      
     */
    slice(start?: number, end?: number, contentType?: string): Class_Blob;

    /**
     * @description 以文本形式读取 Blob 内容
     *      @return 返回包含文本内容的 Promise
     *      
     */
    text(): Promise<string>;

    /**
     * @description 以 ArrayBuffer 形式读取 Blob 内容
     *      @return 返回包含二进制数据的 Promise
     *      
     */
    arrayBuffer(): Promise<ArrayBuffer>;

}

