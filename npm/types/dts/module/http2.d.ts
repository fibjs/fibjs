/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Http2Server.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Http2Session.d.ts" />
/// <reference path="../interface/Http2Stream.d.ts" />
/**
 * @description http2 模块提供 HTTP/2 协议支持
 * 
 * http2 模块允许创建 HTTP/2 服务器和客户端，完整支持流多路复用、头部压缩和流量控制。
 * 
 * ```JavaScript
 * const http2 = require('http2');
 * 
 * // 客户端示例
 * const session = http2.connect('https://example.com');
 * const stream = session.request({ ':path': '/' });
 * const response = stream.read();
 * session.close();
 * 
 * // 服务端示例
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
declare module 'http2' {
    /**
     * @description 创建 Http2Server 对象，参见 Http2Server 
     */
    const Server: typeof Class_Http2Server;

    /**
     * @description 创建 Http2 服务器
     *      @param options TLS 选项对象或 SecureContext 配置
     *      @param hdlr 请求处理函数
     *      @return 返回 Http2Server 对象，调用 listen() 然后 start() 开始服务
     *     
     */
    function createServer(options: FIBJS.GeneralObject, hdlr: Class_Handler): Class_Http2Server;

    /**
     * @description 创建 Http2 服务器
     *      @param context SecureContext 对象用于 TLS 配置
     *      @param hdlr 请求处理函数
     *      @return 返回 Http2Server 对象，调用 listen() 然后 start() 开始服务
     *     
     */
    function createServer(context: Class_SecureContext, hdlr: Class_Handler): Class_Http2Server;

    /**
     * @description 创建到指定目标的 HTTP/2 客户端会话
     * 
     *      authority 应为 URL 字符串，如 'https://example.com' 或 'https://example.com:8443'。
     * 
     *      options 可包含：
     *      - 所有 SecureContext 选项（key、cert、ca 等）
     * 
     *      @param authority 要连接的服务器 URL
     *      @param options 连接选项
     *      @return 返回 Http2Session 客户端会话
     *     
     */
    function connect(authority: string, options?: FIBJS.GeneralObject): Class_Http2Session;

    function connect(authority: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Http2Session)=>any): void;

    /**
     * @description 创建到指定目标的 HTTP/2 客户端会话
     * 
     *      authority 应为 URL 字符串，如 'https://example.com' 或 'https://example.com:8443'。
     * 
     *      options 可包含：
     *      - 所有 SecureContext 选项（key、cert、ca 等）
     * 
     *      @param authority 要连接的服务器 URL
     *      @param options 连接选项
     *      @return 返回 Http2Session 客户端会话
     *     
     */
    function connectSync(authority: string, options?: FIBJS.GeneralObject): Class_Http2Session;

    /**
     * @description 创建到指定目标的 HTTP/2 客户端会话
     * 
     *      authority 应为 URL 字符串，如 'https://example.com' 或 'https://example.com:8443'。
     * 
     *      options 可包含：
     *      - 所有 SecureContext 选项（key、cert、ca 等）
     * 
     *      @param authority 要连接的服务器 URL
     *      @param options 连接选项
     *      @return 返回 Http2Session 客户端会话
     *     
     */
    function connectAsync(authority: string, options?: FIBJS.GeneralObject): Promise<Class_Http2Session>;

    /**
     * @description 返回默认的 HTTP/2 设置对象
     *      @return 返回包含默认设置的对象
     *     
     */
    function getDefaultSettings(): FIBJS.GeneralObject;

    /**
     * @description Http2Stream 对象，参见 Http2Stream 
     */
    const Http2Stream: typeof Class_Http2Stream;

    /**
     * @description Http2Session 对象，参见 Http2Session 
     */
    const Http2Session: typeof Class_Http2Session;

}

