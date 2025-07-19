/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description File 对象用于表示文件系统中的文件，兼容 Web 标准 File API。
 * 
 * File 继承自 Blob，除具备所有 Blob 的二进制数据能力外，还增加了文件名（name）和最后修改时间（lastModified）等属性，常用于文件上传、下载、Web API 交互等场景。
 * 
 * 主要特性：
 * 1. 继承自 Blob，支持所有二进制数据操作、切片、异步读取等。
 * 2. 只读属性 name：表示文件名，通常用于展示、上传或保存文件时使用。
 * 3. 只读属性 lastModified：表示文件的最后修改时间（自 1970-01-01 00:00:00 UTC 起的毫秒数）。
 * 4. 构造函数要求必须传递文件名参数，否则抛出 TypeError。
 * 5. 构造时可指定 type、endings、lastModified 等选项，行为与 Web File API 保持一致。
 * 
 * 常见用法示例：
 * ```JavaScript
 * // 创建 File 对象
 * const file = new File(["hello"], "greeting.txt", { type: "text/plain" });
 * 
 * // 读取文件名和类型
 * console.log(file.name); // "greeting.txt"
 * console.log(file.type); // "text/plain"
 * 
 * // 获取最后修改时间
 * console.log(file.lastModified);
 * 
 * // 切片
 * const part = file.slice(0, 2);
 * 
 * // 异步读取内容
 * file.text().then(txt => console.log(txt));
 * ```
 * 
 */
declare class Class_File extends Class_Blob {
    /**
     * !@description File 构造函数，创建一个新的 File 实例。File 继承自 Blob，支持所有 Blob 的数据类型。
     * 
     *     options 支持以下属性：
     *        - type: 指定 MIME 类型（如 "text/plain"），默认为空字符串。
     *        - lastModified: 指定最后修改时间（时间戳，单位为毫秒），默认为当前时间。
     *     
     *      @param blobParts 初始化数据数组，可以包含字符串、ArrayBuffer、TypedArray、Blob 等。
     *      @param name 文件名，必须为字符串，表示该文件的名称（如 "a.txt"），不能为空。
     *      @param options 可选参数对象
     *     
     */
    constructor(blobParts: any[], name: string, options?: FIBJS.GeneralObject);

    /**
     * !@description File 构造函数，创建一个新的 File 实例。File 继承自 Blob，支持所有 Blob 的数据类型。
     * 
     *     options 支持以下属性：
     *        - type: 指定 MIME 类型（如 "text/plain"），默认为空字符串。
     *        - lastModified: 指定最后修改时间（时间戳，单位为毫秒），默认为当前时间。
     *     
     *      @param blobData 初始化的二进制数据，可以是 Buffer 或其他二进制数据类型。
     *      @param name 文件名，必须为字符串，表示该文件的名称（如 "a.txt"），不能为空。
     *      @param options 可选参数对象
     *     
     */
    constructor(blobData: Class_Buffer, name: string, options?: FIBJS.GeneralObject);

    /**
     * !@description File 构造函数，创建一个新的 File 实例。File 继承自 Blob，支持所有 Blob 的数据类型。
     * 
     *     options 支持以下属性：
     *        - data: 初始化的二进制数据，可以是 Buffer 或其他二进制数据类型。
     *        - name: 文件名，必须为字符串，表示该文件的名称（如 "a.txt"），不能为空。
     *        - type: 指定 MIME 类型（如 "text/plain"），默认为空字符串。
     *        - lastModified: 指定最后修改时间（时间戳，单位为毫秒），默认为当前时间。
     *     
     *      @param options 可选参数对象
     *     
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * !@description 文件名，只读属性，返回文件的名称。
     *     
     *      该属性用于标识文件，通常用于显示、上传或保存文件时使用。
     *      
     */
    readonly name: string;

    /**
     * !@description 最后修改时间戳，只读属性，返回文件的最后修改时间（毫秒）。
     *     
     *      该属性表示文件的最后修改时间，单位为自 1970-01-01 00:00:00 UTC 起的毫秒数。
     *      
     */
    readonly lastModified: number;

}

