/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Blob 对象用于表示不可变的原始数据块，兼容 Web 标准 Blob API。
 * 
 * Blob 可用于存储二进制数据、文本、图片等，常用于文件上传、数据处理等场景。Blob 支持多种数据类型的拼接、切片和读取，广泛应用于 Web、HTTP、文件系统等模块。
 * 
 * 主要特性：
 * 1. 支持通过数组和选项对象灵活构造，数据类型可为字符串、ArrayBuffer、TypedArray、Blob 等。
 * 2. 支持 type、size 等只读属性，便于获取数据类型和大小。
 * 3. 支持 slice 方法高效切片，支持类型转换。
 * 4. 支持异步读取为文本或二进制。
 * 
 * 常见用法示例：
 * ```JavaScript
 * // 创建空 Blob
 * const blob = new Blob();
 * 
 * // 创建包含字符串和二进制的 Blob
 * const blob = new Blob(["hello", new Uint8Array([1,2,3])], { type: "text/plain" });
 * 
 * // 切片
 * const part = blob.slice(0, 5);
 * 
 * // 读取文本内容
 * blob.text().then(txt => console.log(txt));
 * 
 * // 读取二进制内容
 * blob.arrayBuffer().then(buf => ...);
 * ```
 *  
 */
declare class Class_Blob extends Class_object {
    /**
     * @description Blob 对象构造函数
     *         
     *      创建一个新的 Blob 实例，可指定数据内容和类型。
     *      @param blobParts 初始化数据数组，可以包含字符串、ArrayBuffer、TypedArray、Blob 等
     *      @param options 选项对象，包含 type（MIME 类型）和 endings（换行符处理方式）属性
     *     
     */
    constructor(blobParts?: any[], options?: FIBJS.GeneralObject);

    /**
     * @description Blob 对象构造函数
     *         
     *      创建一个新的 Blob 实例，可指定数据内容和类型。
     *      @param blobData 初始化的二进制数据，可以是 Buffer 或其他二进制数据类型
     *      @param options 选项对象，包含 type（MIME 类型）和 endings（换行符处理方式）属性
     *     
     */
    constructor(blobData: Class_Buffer, options?: FIBJS.GeneralObject);

    /**
     * @description Blob 对象类型，返回 Blob 的 MIME 类型（如 "text/plain"、"image/png" 等），只读属性。
     *     
     */
    readonly type: string;

    /**
     * @description Blob 对象的大小，返回 Blob 数据的字节数，只读属性。
     *     
     */
    readonly size: number;

    /**
     * @description 返回指定范围的 Blob 切片
     *         
     *      创建一个新的 Blob，包含原始数据的指定区间内容。
     * 
     *      @param start 起始位置（字节，默认为 0）
     *      @param end 结束位置（字节，默认为 -1，表示到末尾）
     *      @param contentType 新 Blob 的 MIME 类型（可选）
     *      @return 返回新的 Blob 对象
     *     
     */
    slice(start?: number, end?: number, contentType?: string): Class_Blob;

    /**
     * @description 以文本形式读取 Blob 内容
     *         
     *      异步读取 Blob 数据为字符串，返回 Promise。
     *      @return 返回包含文本内容的 Promise
     *     
     */
    text(): Promise<string>;

    /**
     * @description 以 ArrayBuffer 形式读取 Blob 内容
     *         
     *      异步读取 Blob 数据为 ArrayBuffer，返回 Promise。
     *      @return 返回包含二进制数据的 Promise
     *     
     */
    arrayBuffer(): Promise<ArrayBuffer>;

}

