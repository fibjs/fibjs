/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DgramSocket.d.ts" />
/**
 * @description dgram 基础模块之一，主要用于实现 UDP 数据包 socket 的封装。
 * 
 * 使用步骤：
 * 
 * 1. 首先，通过下面的语句引入 dgram 模块。
 * 
 * ```
 * var dgram = require('dgram');
 * ```
 * 
 * 2. 创建 UDP 数据包 socket 实例。
 * 
 * ```
 * var sock = dgram.createSocket('udp4');
 * ```
 * 
 * 3. 为 UDP 数据包 socket 注册数据接收事件消息回调函数。
 * 
 * ```
 * sock.on('message', function (msg, rinfo) {
 *   // process received message
 * });
 * ```
 * 
 * 4. 发送 UDP 数据包消息到指定目标地址。
 * 
 * ```
 * var msg = ...; // message to send
 * var port = ...; // destination port
 * var host = ...; // destination host
 * var bytes = sock.send(msg, 0, msg.length, port, host);
 * console.log('UDP message sent to ' + host + ':' + port);
 * ```
 *  
 */
declare module 'dgram' {
    /**
     * @description dgram.Socket 对象是一个封装了数据包函数功能的 EventEmitter。参见 DgramSocket
     *      dgram.Socket 实例是由 dgram.createSocket() 创建的。创建 dgram.Socket 实例不需要使用 new 关键字。
     *     
     */
    const Socket: typeof Class_DgramSocket;

    /**
     * @description 创建一个 dgram.Socket 对象
     * 
     *      opts 允许的选项是:
     *      ```JavaScript
     *      {
     *          "type": "udp4" | "udp6",   // socket type
     *          "reuseAddr": true | false, // reuse address, default is false
     *          "ipv6Only": true | false, // only accept IPv6 packets, default is false
     *          "recvBufferSize": 1024,     // specify the size of the receive buffer
     *          "sendBufferSize": 1024      // specify the size of the send buffer
     *      }
     *      ```
     *      @param opts
     *      @return 返回创建的 Socket 对象
     *     
     */
    function createSocket(opts: FIBJS.GeneralObject): Class_DgramSocket;

    /**
     * @description 创建一个 dgram.Socket 对象
     * 
     *      opts 允许的选项是:
     *      ```JavaScript
     *      {
     *          "type": "udp4" | "udp6",   // socket type
     *          "reuseAddr": true | false, // reuse address, default is false
     *          "ipv6Only": true | false, // only accept IPv6 packets, default is false
     *          "recvBufferSize": 1024,     // specify the size of the receive buffer
     *          "sendBufferSize": 1024      // specify the size of the send buffer
     *      }
     *      ```
     *      @param opts
     *      @param callback 为 'message' 事件添加一个监听器。
     *      @return 返回创建的 Socket 对象
     *     
     */
    function createSocket(opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_DgramSocket;

    /**
     * @description 创建一个 dgram.Socket 对象
     *      @param type 套接字族，'udp4' 或 'udp6'。
     *      @return 返回创建的 Socket 对象
     *     
     */
    function createSocket(type: string): Class_DgramSocket;

    /**
     * @description 创建一个 dgram.Socket 对象
     *      @param type 套接字族，'udp4' 或 'udp6'。
     *      @param callback 为 'message' 事件添加一个监听器。
     *      @return 返回创建的 Socket 对象
     *     
     */
    function createSocket(type: string, callback: (...args: any[])=>any): Class_DgramSocket;

}

