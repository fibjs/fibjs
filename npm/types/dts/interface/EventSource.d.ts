/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * @description 事件源接口，用于服务器推送事件
 * 
 *   引用方式：
 *   ```JavaScript
 *     const http = require('http');
 *     
 *     const es = new http.EventSource('http://localhost:8080');
 *   ```
 * 
 */
declare class Class_EventSource extends Class_EventEmitter {
    /**
     * @description 构造函数
     * 
     *      options 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     * 
     *       @param url 服务器地址
     *       @param options 选项
     *     
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description 关闭连接 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 事件源状态，取值为 CONNECTING、OPEN、CLOSED 
     */
    readonly readyState: number;

    /**
     * @description 服务器地址 
     */
    readonly url: string;

    /**
     * @description 是否携带凭证 
     */
    readonly withCredentials: boolean;

    /**
     * @description http 响应对象 
     */
    readonly response: Class_HttpResponse;

    /**
     * @description 打开事件回调 
     */
    onopen: (...args: any[])=>any;

    /**
     * @description 错误事件回调 
     */
    onerror: (...args: any[])=>any;

    /**
     * @description 消息事件回调 
     */
    onmessage: (...args: any[])=>any;

    /**
     * @description 关闭事件回调 
     */
    onclose: (...args: any[])=>any;




}

