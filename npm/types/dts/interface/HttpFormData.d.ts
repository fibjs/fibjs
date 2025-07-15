/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/**
 * @description HttpFormData 是用于管理 HTTP 表单数据（multipart/form-data）的容器类，继承自 HttpCollection。
 * 
 * HttpFormData 提供了标准的 Web FormData API，支持多种方式初始化和操作表单字段，适用于 HTTP 文件上传、表单数据构建等场景。
 * 
 * 主要特性：
 * 1. 支持通过空构造、对象、已有 HttpFormData 实例进行初始化。
 * 2. 支持 append、set 等方法添加和修改字段，支持文件（Blob）和文件名参数。
 * 3. 兼容 Web 标准 FormData 行为，允许同名字段多值、文件上传等。
 * 
 * 常见用法示例：
 * ```JavaScript
 * // 创建空表单数据
 * const form = new HttpFormData();
 * 
 * // 使用对象初始化
 * const form = new HttpFormData({ foo: 'bar', file: blob });
 * 
 * // 追加字段
 * form.append('name', 'value');
 * form.append('file', blob, 'filename.txt');
 * 
 * // 覆盖字段
 * form.set('name', 'newValue');
 * form.set('file', blob2, 'file2.txt');
 * ```
 *  
 */
declare class Class_HttpFormData extends Class_HttpCollection {
    /**
     * @description HttpFormData 构造函数，创建一个新的空 HTTP 表单数据容器
     *         创建一个空的 HttpFormData 实例，用于后续动态添加表单字段。
     *     
     */
    constructor();

    /**
     * @description HttpFormData 构造函数，使用给定的对象初始化 HTTP 表单数据容器
     *         
     *      通过传入一个对象，批量初始化表单字段。对象的键为字段名，值为字段值（可为字符串、Blob 或数组）。
     * 
     *      @param init 初始化用的字段对象，键为字段名，值为字段值（字符串、Blob 或数组）
     *     
     */
    constructor(init: FIBJS.GeneralObject);

    /**
     * @description HttpFormData 构造函数，使用给定的 HTTP 表单数据容器初始化 HTTP 表单数据容器
     *         
     *      通过传入另一个 HttpFormData 实例，复制其所有字段。
     *     
     *      @param init 初始化用的 HTTP 表单数据容器
     *     
     */
    constructor(init: Class_HttpFormData);

    /**
     * @description 添加一个键值数据，添加数据并不修改已存在的键值的数据
     *         
     *      向表单中追加一个字段。如果同名字段已存在，则不会覆盖，允许同名多值。
     * 
     *      @param name 指定要添加的字段名
     *      @param value 指定要添加的 Blob
     *     
     */
    append(name: string, value: Class_Blob): void;

    /**
     * @description 添加一个键值数据，添加数据并不修改已存在的键值的数据
     *         
     *      向表单中追加一个字段。如果同名字段已存在，则不会覆盖，允许同名多值。
     * 
     *      @param name 指定要添加的字段名
     *      @param value 指定要添加的 Blob
     *      @param filename 指定要添加的文件名
     *     
     */
    append(name: string, value: Class_Blob, filename: string): void;

    /**
     * @description 设定一个键值数据，设定数据将修改键值所对应的第一个数值，并清除相同键值的其余数据
     *         
     *      设置表单字段。如果同名字段已存在，则只保留第一个并覆盖，移除其余同名字段。
     * 
     *      @param name 指定要设定的字段名
     *      @param value 指定要设定的 Blob
     *     
     */
    set(name: string, value: Class_Blob): void;

    /**
     * @description 设定一个键值数据，设定数据将修改键值所对应的第一个数值，并清除相同键值的其余数据
     *         
     *      设置表单字段。如果同名字段已存在，则只保留第一个并覆盖，移除其余同名字段。
     * 
     *      @param name 指定要设定的字段名
     *      @param value 指定要设定的 Blob
     *      @param filename 指定要设定的文件名
     *     
     */
    set(name: string, value: Class_Blob, filename: string): void;

}

