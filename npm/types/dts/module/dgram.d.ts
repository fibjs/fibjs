/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DgramSocket.d.ts" />
/**
 * @description dgram 模块提供了 UDP 数据包 socket 的实现
 * 
 *  基础模块，引用方式：
 *  ```JavaScript
 *  var dgram = require('dgram');
 *  ```
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
     *          "type": "udp4" | "udp6",   // 必填
     *          "reuseAddr": true | false, // 若设置为 true，socket.bind() 则会重用地址，即时另一个进程已经在其上面绑定了一个套接字。 默认是 false
     *          "ipv6Only": true | false, // 若设置为 true，socket.bind() 将只接收 IPv6 的数据包。 默认是 false
     *          "recvBufferSize": 1024,     // 设置 SO_RCVBUF 套接字值
     *          "sendBufferSize": 1024      //设置 SO_RCVBUF 套接字值
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
     *          "type": "udp4" | "udp6",   // 必填
     *          "reuseAddr": true | false, // 若设置为 true，socket.bind() 则会重用地址，即时另一个进程已经在其上面绑定了一个套接字。 默认是 false
     *          "ipv6Only": true | false, // 若设置为 true，socket.bind() 将只接收 IPv6 的数据包。 默认是 false
     *          "recvBufferSize": ###,     // 设置 SO_RCVBUF 套接字值
     *          "sendBufferSize": ###      //设置 SO_RCVBUF 套接字值
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

