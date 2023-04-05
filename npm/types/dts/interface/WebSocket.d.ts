/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description WebSocket 是一种基于 TCP 协议的全双工通信协议，在浏览器和服务器之间建立起一个不断开的连接，可以实现实时双向数据传输，并且可以支持任意格式的数据传输。在 fibjs 中，WebSocket 支持模块提供了相应的 API 接口，可以实现 WebSocket 服务器端和客户端的开发
 * 
 * WebSocket 支持模块只是 WebSocket 协议的一个实现，需要在 HTTP 协议之上才能工作。在服务器端，可以通过 upgrade 函数将 HTTP 请求转换为 WebSocket 连接，而在客户端，则需要通过 WebSocket 协议的 URL 来指定需要连接的服务器地址。
 * 
 * 启动WebSocket服务器示例：
 * ```JavaScript
 * var ws = require('ws');
 * var http = require('http');
 * 
 * var svr = new http.Server(80, {
 *     '/ws': ws.upgrade(conn => {
 *         conn.onmessage = e => {
 *             conn.send('fibjs:' + e.data);
 *         };
 *     })
 * });
 * svr.start();
 * ```
 * 在客户端中与上述服务器建立连接的示例：
 * ```JavaScript
 * var ws = require("ws");
 * 
 * var conn = new ws.Socket("ws://127.0.0.1/ws");
 * // emit open event
 * conn.onopen = () => {
 *     console.log("websocket connected");
 *     conn.send("hi");
 * };
 * // emit close event
 * conn.onmessage = evt => {
 *     console.log("websocket receive: " + evt.data);
 * };
 * ```
 *  
 */
declare class Class_WebSocket extends Class_EventEmitter {
    /**
     * @description WebSocket 构造函数
     *      @param url 指定连接的服务器
     *      @param protocol 指定握手协议，缺省为 ""
     *      @param origin 指定握手时模拟的源，缺省为 ""
     *     
     */
    constructor(url: string, protocol?: string, origin?: string);

    /**
     * @description WebSocket 构造函数
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "protocol": "", // specify the sub-protocol, default is ""
     *          "origin": "", // specify the origin, default is ""
     *          "perMessageDeflate": false, // specify whether to enable permessage-deflate, default is false
     *          "maxPayload": 67108864, // specify the max payload size, default is 64MB
     *          "httpClient": hc, // specify the http client, default is null, use the global http client
     *          "headers": // specify the http headers, default is {}
     *      }
     *      ```
     *      @param url 指定连接的服务器
     *      @param opts 连接选项，缺省是 {}
     *     
     */
    constructor(url: string, opts: FIBJS.GeneralObject);

    /**
     * @description 查询当前对象连接的服务器 
     */
    readonly url: string;

    /**
     * @description 查询当前对象连接时的协议 
     */
    readonly protocol: string;

    /**
     * @description 查询当前对象连接的源 
     */
    readonly origin: string;

    /**
     * @description 查询当前对象的连接状态，参见 ws 
     */
    readonly readyState: number;

    /**
     * @description 关闭当前连接，此操作会向对方发送 CLOSE 数据包，并等待对方响应
     *      @param code 指定关闭的代码，允许值为 3000-4999 或者 1000，缺省为 1000
     *      @param reason 指定关闭的原因，缺省为 ""
     *     
     */
    close(code?: number, reason?: string): void;

    /**
     * @description 向对方发送一段文本
     *      @param data 指定发送的文本
     *     
     */
    send(data: string): void;

    /**
     * @description 向对方发送一段二进制数据
     *      @param data 指定发送的二进制数据
     *     
     */
    send(data: Class_Buffer): void;

    /**
     * @description 查询和绑定连接成功事件，相当于 on("open", func); 
     */
    onopen: (...args: any[])=>any;

    /**
     * @description 查询和绑定接受到对方消息的事件，相当于 on("message", func); 
     */
    onmessage: (...args: any[])=>any;

    /**
     * @description 查询和绑定连接关闭的事件，相当于 on("close", func); 
     */
    onclose: (...args: any[])=>any;

    /**
     * @description 查询和绑定错误发生的事件，相当于 on("error", func); 
     */
    onerror: (...args: any[])=>any;

    /**
     * @description 维持 fibjs 进程不退出，在对象绑定期间阻止 fibjs 进程退出
     *      @return 返回当前对象
     *     
     */
    ref(): Class_WebSocket;

    /**
     * @description 允许 fibjs 进程退出，在对象绑定期间允许 fibjs 进程退出
     *      @return 返回当前对象
     *     
     */
    unref(): Class_WebSocket;

}

