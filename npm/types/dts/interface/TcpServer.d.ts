/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Socket.d.ts" />
/**
 * @description tcp 服务器对象，可方便创建一个标准多纤程 tcp 服务器
 * 
 *  使用 TcpServer 对象可以迅速创建一个多纤程并发处理的 tcp 服务器。
 *  ```JavaScript
 *  function func(conn)
 *  {
 *      var data;
 * 
 *      while(data = conn.read())
 *          conn.write(data);
 * 
 *      conn.close();
 *  }
 * 
 *  new net.TcpServer(8080, func).start();
 *  ```
 *  
 */
declare class Class_TcpServer extends Class_object {
    /**
     * @description TcpServer 构造函数，在所有本机地址侦听
     *     @param port 指定 tcp 服务器侦听端口
     *     @param listener 指定 tcp 接收到的内置消息处理器，处理函数，链式处理数组，路由对象，详见 mq.Handler
     *    
     */
    constructor(port: number, listener: Class_Handler);

    /**
     * @description TcpServer 构造函数
     *     @param addr 指定 tcp 服务器侦听地址，为 "" 则在本机所有地址侦听
     *     @param port 指定 tcp 服务器侦听端口
     *     @param listener 指定 tcp 接收到的连接的内置消息处理器，处理函数，链式处理数组，路由对象，详见 mq.Handler
     *    
     */
    constructor(addr: string, port: number, listener: Class_Handler);

    /**
     * @description TcpServer 构造函数
     *     @param addr 指定 unix socket 或者 Windows pipe 服务器侦听地址
     *     @param listener 指定 tcp 接收到的连接的内置消息处理器，处理函数，链式处理数组，路由对象，详见 mq.Handler
     *    
     */
    constructor(addr: string, listener: Class_Handler);

    /**
     * @description 启动当前服务器 
     */
    start(): void;

    /**
     * @description 关闭 socket中止正在运行的服务器 
     */
    stop(): void;

    stop(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 服务器当前侦听的 Socket 对象  
     */
    readonly socket: Class_Socket;

    /**
     * @description 服务器当前事件处理接口对象 
     */
    handler: Class_Handler;

}

