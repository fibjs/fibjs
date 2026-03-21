/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description Http2Server 是高并发 HTTP/2 服务器
 * 
 * Http2Server 通过 TLS (h2) 处理 HTTP/2 连接。当客户端连接时，服务器为每个连接创建 Http2Session，并为每个请求触发 'stream' 事件。
 * 
 * ```JavaScript
 * const http2 = require('http2');
 * 
 * const server = http2.createServer({
 *     key: ...,
 *     cert: ...
 * }, function(req) {
 *     req.response.write('Hello, HTTP/2!');
 * });
 * server.listen(8443);
 * server.start();
 * ```
 * 
 */
declare class Class_Http2Server extends Class_TcpServer {
    /**
     * @description Http2Server 构造函数
     *      @param context SecureContext 安全上下文
     *      @param hdlr http 内置消息处理器
     *     
     */
    constructor(context: Class_SecureContext, hdlr: Class_Handler);

    /**
     * @description Http2Server 构造函数
     *      @param context SecureContext 安全上下文
     *      @param port 监听端口
     *      @param hdlr http 内置消息处理器
     *     
     */
    constructor(context: Class_SecureContext, port: number, hdlr: Class_Handler);

    /**
     * @description Http2Server 构造函数
     *      @param context SecureContext 安全上下文
     *      @param addr 监听地址
     *      @param port 监听端口
     *      @param hdlr http 内置消息处理器
     *     
     */
    constructor(context: Class_SecureContext, addr: string, port: number, hdlr: Class_Handler);

    /**
     * @description Http2Server 构造函数，从选项创建 SecureContext
     *      @param options 创建 SecureContext 的选项，可包含 address 和 port
     *      @param hdlr http 内置消息处理器
     *     
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler);

    /**
     * @description 查询当前 Http2Server 使用的 SecureContext 
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description 设置当前 Http2Server 使用的 SecureContext
     *      @param context 指定新的 SecureContext
     *     
     */
    setSecureContext(context: Class_SecureContext): void;

    /**
     * @description 设置当前 Http2Server 使用的 SecureContext
     *      @param options 创建新 SecureContext 的选项
     *     
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}

