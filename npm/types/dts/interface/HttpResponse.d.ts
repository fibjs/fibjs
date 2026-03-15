/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpCookie.d.ts" />
/**
 * @description HttpResponse 是一个 HTTP 响应对象，使用 HttpRequest.response 对象完成 Http 服务端数据响应，或 http.request 请求返回服务器的响应数据
 * 
 * 以下的例子展示如何在 http.Server 中使用，示例代码如下：
 * ```
 * const http = require('http');
 * 
 * const server = new http.Server(8080, (request) => {
 *   // retreive the response object
 *   const response = request.response;
 *   // set the status code
 *   response.statusCode = 200;
 *   // set the content type to text/plain
 *   response.setHeader('Content-Type', 'text/plain');
 *   // write the response body
 *   response.write('ok');
 * });
 * 
 * server.start();
 * ```
 * 
 */
declare class Class_HttpResponse extends Class_HttpMessage {
    /**
     * @description HttpResponse 构造函数，创建一个新的 HttpResponse 对象 
     */
    constructor();

    /**
     * @description HttpResponse 构造函数，创建一个新的 HttpResponse 对象（Web API 兼容）
     *      
     *      支持 Web 标准 Response 构造方式，例如：
     *      ```JavaScript
     *      const response = new http.Response("Hello World", {
     *          status: 200,
     *          statusText: "OK",
     *          headers: { "Content-Type": "text/plain" }
     *      });
     *      ```
     *      @param body 响应体内容，可以是字符串、Buffer 或 null
     *      @param options 选项对象，支持 status、statusText、headers 属性
     *     
     */
    constructor(body: any, options?: FIBJS.GeneralObject);

    /**
     * @description 查询和设置响应消息的返回状态 
     */
    statusCode: number;

    /**
     * @description 查询和设置响应消息的返回消息 
     */
    statusMessage: string;

    /**
     * @description 查询和设置响应消息的返回消息，等同于 statusMessage（Web API 兼容） 
     */
    statusText: string;

    /**
     * @description 查询和设置响应消息的返回状态，等同于 statusCode 
     */
    status: number;

    /**
     * @description 查询当前响应是否正常 
     */
    readonly ok: boolean;

    /**
     * @description 设置响应消息的返回状态，返回消息，并添加响应头
     *      @param statusCode 指定响应消息的返回状态
     *      @param statusMessage 指定响应消息的返回消息
     *      @param headers 指定响应消息添加的响应头
     *      
     */
    writeHead(statusCode: number, statusMessage: string, headers?: FIBJS.GeneralObject): void;

    /**
     * @description 设置响应消息的返回状态，返回消息，并添加响应头
     *      @param statusCode 指定响应消息的返回状态
     *      @param headers 指定响应消息添加的响应头
     *      
     */
    writeHead(statusCode: number, headers?: FIBJS.GeneralObject): void;

    /**
     * @description 返回当前消息的 HttpCookie 对象列表 
     */
    readonly cookies: any[];

    /**
     * @description 向 cookies 添加一个 HttpCookie 对象
     *      @param cookie 指定要添加的 HttpCookie 对象
     *      
     */
    addCookie(cookie: Class_HttpCookie): void;

    /**
     * @description 发送重定向到客户端
     *      @param url 重定向的地址
     *      
     */
    redirect(url: string): void;

    /**
     * @description 发送重定向到客户端
     *      @param statusCode 指定响应消息的返回状态，接受的状态为：301, 302, 307
     *      @param url 重定向的地址
     *      
     */
    redirect(statusCode: number, url: string): void;

    /**
     * @description Fetch API 响应的最终 URL（经过重定向后的地址）
     */
    readonly url: string;

    /**
     * @description 是否经过重定向 
     */
    readonly redirected: boolean;

    /**
     * @description 响应类型（"basic"、"cors"、"error" 等），覆盖 Message.type 
     */
    readonly type: string;

    /**
     * @description 创建一个 JSON 响应（静态工厂）
     *      @param data 要序列化为 JSON 的数据
     *      @param options 选项对象，支持 status、statusText、headers
     *      @return 返回新的 HttpResponse 对象
     *      
     */
    static json(data: any, options?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 创建一个重定向响应（静态工厂）
     *      @param url 重定向目标 URL
     *      @param status 重定向状态码，默认 302
     *      @return 返回新的 HttpResponse 对象
     *      
     */
    static redirect(url: string, status?: number): Class_HttpResponse;

    /**
     * @description 创建一个错误响应（静态工厂）
     *      @return 返回 type="error" 的新的 HttpResponse 对象
     *      
     */
    static error(): Class_HttpResponse;

}

