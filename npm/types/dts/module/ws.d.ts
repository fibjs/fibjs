/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/WebSocketMessage.d.ts" />
/// <reference path="../interface/WebSocket.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description websocket 支持模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  var ws = require('ws');
 *  ```
 *  创建一个服务器：
 *  ```JavaScript
 *  var ws = require('ws');
 *  var http = require('http');
 * 
 *  var svr = new http.Server(80, {
 *      '/ws': ws.upgrade((conn, req) => {
 *          conn.onmessage = e => console.log(e.data);
 *      })
 *  });
 *  svr.start();
 *  ```
 *  使用 WebSocket 客户端：
 *  ```JavaScript
 *  var ws = require('ws');
 *  
 *  var conn = new ws.Socket('ws://127.0.0.1/ws');
 *  conn.onmessage = e => console.log(e.data);
 *  ```
 *  
 */
declare module 'ws' {
    /**
     * @description 指定 websocket 消息类型 0，代表一个继续帧 
     */
    export const CONTINUE: 0;

    /**
     * @description 指定 websocket 消息类型 1，代表一个文本帧 
     */
    export const TEXT: 1;

    /**
     * @description 指定 websocket 消息类型 2，代表一个二进制帧 
     */
    export const BINARY: 2;

    /**
     * @description 指定 websocket 消息类型 8，连接关闭 
     */
    export const CLOSE: 8;

    /**
     * @description 指定 websocket 消息类型 9，代表一个 ping 帧 
     */
    export const PING: 9;

    /**
     * @description 指定 websocket 消息类型 10，代表一个 pong 帧 
     */
    export const PONG: 10;

    /**
     * @description 指定 WebSocket 状态，表示正在连接中
     */
    export const CONNECTING: 0;

    /**
     * @description 指定 WebSocket 状态，表示打开状态
     */
    export const OPEN: 1;

    /**
     * @description 指定 WebSocket 状态，表示已发送 CLOSE 消息，等待关闭中
     */
    export const CLOSING: 2;

    /**
     * @description 指定 WebSocket 状态，表示已经关闭
     */
    export const CLOSED: 3;

    /**
     * @description 创建一个 websocket 消息对象，参见 WebSocketMessage 
     */
    const Message: typeof Class_WebSocketMessage;

    /**
     * @description WebSocket 对象，参见 WebSocket 
     */
    const Socket: typeof Class_WebSocket;

    /**
     * @description 创建一个 websocket 协议处理器，从 http 接收 upgrade 请求并握手，生成 WebSocket 对象
     *      ```
     *      @param accept 连接成功处理函数，回调将传递两个参数，第一个参数为接收到的 WebSocket 对象，第二个参数为握手时的 HttpRequest 对象
     *      @return 返回协议处理器，可与 HttpServer, Chain, Routing 等对接
     *      
     */
    function upgrade(accept: (...args: any[])=>any): Class_Handler;

    /**
     * @description 创建一个 websocket 协议处理器，从 http 接收 upgrade 请求并握手，生成 WebSocket 对象
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "perMessageDeflate": true, // 指定是否支持压缩，缺省支持
     *          "maxPayload": 67108864 // 指定最大数据包尺寸，缺省为 67108864
     *      }
     *      ```
     *      @param opts 连接选项，缺省是 {}
     *      @param accept 连接成功处理函数，回调将传递两个参数，第一个参数为接收到的 WebSocket 对象，第二个参数为握手时的 HttpRequest 对象
     *      @return 返回协议处理器，可与 HttpServer, Chain, Routing 等对接
     *      
     */
    function upgrade(opts: FIBJS.GeneralObject, accept: (...args: any[])=>any): Class_Handler;

}

