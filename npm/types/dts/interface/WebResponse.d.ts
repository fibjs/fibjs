/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Headers.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Web Fetch API 标准的 Response 对象
 * 
 *  WebResponse 是 Web Fetch API 标准的 Response 接口实现，用于表示一个 HTTP 响应。
 *  它封装了 HttpResponse，提供与浏览器兼容的标准接口。
 * 
 *  使用示例：
 *  ```JavaScript
 *  // 通过 fetch 获取
 *  const resp = await fetch('http://example.com');
 *  console.log(resp.status);        // 200
 *  console.log(resp.statusText);    // "OK"
 *  const text = await resp.text();
 * 
 *  // 直接创建
 *  const resp2 = new Response('Hello', { status: 200, statusText: 'OK' });
 *  ```
 * 
 */
declare class Class_WebResponse extends Class_object {
    /**
     * @description WebResponse 构造函数
     *      @param body 响应体内容，可以是字符串、Buffer、Blob 或 null
     *      @param options 选项对象，支持 status、statusText、headers 属性
     *     
     */
    constructor(body: any, options?: FIBJS.GeneralObject);

    /**
     * @description WebResponse 构造函数
     *      @param options 选项对象，支持 status、statusText、headers 属性
     *     
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * @description 查询响应的 HTTP 状态码 
     */
    readonly status: number;

    /**
     * @description 查询响应的 HTTP 状态消息 
     */
    readonly statusText: string;

    /**
     * @description 查询响应是否成功（状态码 200-299） 
     */
    readonly ok: boolean;

    /**
     * @description 查询响应的类型 
     */
    readonly type: string;

    /**
     * @description 查询响应的 URL 
     */
    readonly url: string;

    /**
     * @description 查询响应是否经过了重定向 
     */
    readonly redirected: boolean;

    /**
     * @description 查询响应的 body 是否已经被消费 
     */
    readonly bodyUsed: boolean;

    /**
     * @description 查询响应的 Headers 对象 
     */
    readonly headers: Class_Headers;

    /**
     * @description 查询响应的 body 流对象 
     */
    readonly body: Class_Stream;

    /**
     * @description 以文本形式读取响应体
     *      @return 返回文本内容
     *     
     */
    text(): Promise<string>;

    /**
     * @description 以文本形式读取响应体
     *      @return 返回文本内容
     *     
     */
    textSync(): string;

    /**
     * @description 以文本形式读取响应体
     *      @return 返回文本内容
     *     
     */
    textAsync(): Promise<string>;

    /**
     * @description 以 JSON 形式读取响应体
     *      @return 返回解析后的 JSON 值
     *     
     */
    json(): Promise<any>;

    /**
     * @description 以 JSON 形式读取响应体
     *      @return 返回解析后的 JSON 值
     *     
     */
    jsonSync(): any;

    /**
     * @description 以 JSON 形式读取响应体
     *      @return 返回解析后的 JSON 值
     *     
     */
    jsonAsync(): Promise<any>;

    /**
     * @description 以 ArrayBuffer 形式读取响应体
     *      @return 返回 ArrayBuffer 对象
     *     
     */
    arrayBuffer(): Promise<ArrayBuffer>;

    /**
     * @description 以 ArrayBuffer 形式读取响应体
     *      @return 返回 ArrayBuffer 对象
     *     
     */
    arrayBufferSync(): ArrayBuffer;

    /**
     * @description 以 ArrayBuffer 形式读取响应体
     *      @return 返回 ArrayBuffer 对象
     *     
     */
    arrayBufferAsync(): Promise<ArrayBuffer>;

    /**
     * @description 以 Blob 形式读取响应体
     *      @return 返回 Blob 对象
     *     
     */
    blob(): Promise<Class_Blob>;

    /**
     * @description 以 Blob 形式读取响应体
     *      @return 返回 Blob 对象
     *     
     */
    blobSync(): Class_Blob;

    /**
     * @description 以 Blob 形式读取响应体
     *      @return 返回 Blob 对象
     *     
     */
    blobAsync(): Promise<Class_Blob>;

    /**
     * @description 以 Uint8Array 形式读取响应体
     *      @return 返回 Uint8Array 对象
     *     
     */
    bytes(): Promise<Class_Buffer>;

    /**
     * @description 以 Uint8Array 形式读取响应体
     *      @return 返回 Uint8Array 对象
     *     
     */
    bytesSync(): Class_Buffer;

    /**
     * @description 以 Uint8Array 形式读取响应体
     *      @return 返回 Uint8Array 对象
     *     
     */
    bytesAsync(): Promise<Class_Buffer>;

    /**
     * @description 克隆响应对象
     *      @return 返回新的 WebResponse 对象
     *     
     */
    clone(): Class_WebResponse;

    /**
     * @description 创建一个 JSON 响应
     *      @param data 要序列化为 JSON 的数据
     *      @param options 选项对象，支持 status、statusText、headers 属性
     *      @return 返回新的 WebResponse 对象
     *     
     */
    static json(data: any, options?: FIBJS.GeneralObject): Class_WebResponse;

    /**
     * @description 创建一个重定向响应
     *      @param url 重定向的目标 URL
     *      @param status 重定向状态码，默认 302
     *      @return 返回新的 WebResponse 对象
     *     
     */
    static redirect(url: string, status?: number): Class_WebResponse;

    /**
     * @description 创建一个错误响应
     *      @return 返回新的 WebResponse 对象
     *     
     */
    static error(): Class_WebResponse;

}

