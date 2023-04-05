/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/WebSocketMessage.d.ts" />
/// <reference path="../interface/WebSocket.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description websocket 支持模块
 * 
 *  WebSocket 协议是一种基于 TCP 协议的全双工通信协议，在浏览器和服务器之间建立起一个不断开的连接，可以实现实时双向数据传输，并且可以支持任意格式的数据传输。在 fibjs 中，WebSocket 支持模块提供了相应的 API 接口，可以实现 WebSocket 服务器端和客户端的开发。
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
 *  上面的代码中，首先通过 require 函数加载了 fibjs 内置的 WebSocket 支持模块，然后使用 http.Server 类创建了一个 HTTP 服务器实例，指定了监听的端口号和HTTP请求路径，接着在指定路径下使用 upgrade 函数，将请求升级为 WebSocket 连接，服务器会自动为每个连接创建一个 WebSocket 对象，提供了 onopen、onmessage、onerror、onclose 等事件，并且可以通过 send 函数向客户端发送数据。
 * 
 *  使用 WebSocket 客户端：
 *  ```JavaScript
 *  var ws = require('ws');
 *  
 *  var conn = new ws.Socket('ws://127.0.0.1/ws');
 *  conn.onmessage = e => console.log(e.data);
 *  ```
 *  可以通过创建 Socket 实例，并指定一个 WebSocket 的 URL，来完成客户端的连接。Socket 类提供了 onmessage、onerror、onclose 等回调函数和 send 函数，可以实现向服务器发送数据和接收服务器发送的数据。
 * 
 *  需要注意的是，WebSocket 支持模块仅仅是 WebSocket 协议的一个实现，需要在 HTTP 协议之上才能工作。在服务器端，可以通过 upgrade 函数来将 HTTP 请求转换为 WebSocket 连接，而在客户端，则需要通过 WebSocket 协议的 URL 来指定需要连接的服务器地址。
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
     *          "perMessageDeflate": false, // specify whether to use permessage-deflate, default is true
     *          "maxPayload": 67108864 // specify the maximum allowed message size, default is 64MB
     *      }
     *      ```
     *      @param opts 连接选项，缺省是 {}
     *      @param accept 连接成功处理函数，回调将传递两个参数，第一个参数为接收到的 WebSocket 对象，第二个参数为握手时的 HttpRequest 对象
     *      @return 返回协议处理器，可与 HttpServer, Chain, Routing 等对接
     *      
     */
    function upgrade(opts: FIBJS.GeneralObject, accept: (...args: any[])=>any): Class_Handler;

}

