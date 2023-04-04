/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description HttpServer 是内置对象之一，它是用于创建 HTTP 服务器的对象。一个 HttpServer 对象包含两个必传的参数：端口和事件处理接口对象。在事件处理接口对象中，具体的实现方式可以是简单的回调函数，也可以使用复杂的路由、链式处理数组等方式
 * 
 *  http 服务器对象是将 TcpServer 和 HttpHandler 组合封装的对象，方便快速搭建服务器，逻辑上相当于：
 *  ```JavaScript
 *  var svr = new net.TcpServer(addr, port, new http.Handler(function(req){
 *     ...
 *  }));
 *  ```
 * 
 * 下面是一个最简单的 HttpServer 的应用示例，它仅仅是对所有请求，返回字符串 hello world。
 * ```JavaScript
 * const http = require('http');
 * var svr = new http.Server(8080, (req) => {
 *     req.response.write('hello, world');
 * });
 * svr.start();
 * ```
 * 
 * 从代码中可以看到，首先，我们导入了内置的 http 模块。然后我们创建了一个新的 HttpServer 对象，并且传入了两个必要参数：一个是端口号，另一个是具体的事件处理接口对象。在这个例子中，我们使用了一个简单的回调函数作为事件处理接口，用来响应来自 HTTP 请求的数据。其中 ``req.response.write('hello, world')`` 用于将我们的字符串 hello world 响应到客户端。
 * 
 * 在完成了 HttpServer 对象的创建之后，使用 ``svr.start()`` 来启动服务器，这样我们就能通过这个服务器接收来自 Internet 的 HTTP 请求了。
 *  
 */
declare class Class_HttpServer extends Class_TcpServer {
    /**
     * @description HttpServer 构造函数，在所有本机地址侦听
     *     @param port 指定 http 服务器侦听端口
     *     @param hdlr http 内置消息处理器，处理函数，链式处理数组，路由对象，详见 mq.Handler
     *    
     */
    constructor(port: number, hdlr: Class_Handler);

    /**
     * @description HttpServer 构造函数
     *     @param addr 指定 http 服务器侦听地址，为 "" 则在本机所有地址侦听
     *     @param port 指定 http 服务器侦听端口
     *     @param hdlr http 内置消息处理器，处理函数，链式处理数组，路由对象，详见 mq.Handler
     *    
     */
    constructor(addr: string, port: number, hdlr: Class_Handler);

    /**
     * @description HttpServer 构造函数
     *     @param addr 指定 http 服务器侦听地址，为 "" 则在本机所有地址侦听
     *     @param hdlr http 内置消息处理器，处理函数，链式处理数组，路由对象，详见 mq.Handler
     *    
     */
    constructor(addr: string, hdlr: Class_Handler);

    /**
     * @description 允许跨域请求
     *      @param allowHeaders 指定接受的 http 头字段
     *      
     */
    enableCrossOrigin(allowHeaders?: string): void;

    /**
     * @description 查询和设置最大请求头个数，缺省为 128 
     */
    maxHeadersCount: number;

    /**
     * @description 查询和设置 body 最大尺寸，以 MB 为单位，缺省为 64 
     */
    maxBodySize: number;

    /**
     * @description 自动解压缩功能开关，默认关闭 
     */
    enableEncoding: boolean;

    /**
     * @description 查询和设置服务器名称，缺省为：fibjs/0.x.0 
     */
    serverName: string;

}

