/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description HttpRequest 是用来处理 HTTP 请求的类， 它允许你创建 HTTP 请求并与服务器交互。你可以使用它来向 Web 服务器发送 GET、POST 以及其它类型的 HTTP 请求
 * 
 * 假设我们有一个 key 为 name 的查询参数，我们来根据这个参数返回不同的处理结果：如果参数为空，返回 "Hello world!"；如果参数为 "fibjs"，返回 "Hello fibjs!"；否则返回 "Hello some body!". 
 * 
 * 代码实现如下： 
 * ```JavaScript
 * const http = require('http');
 * 
 * var svr = new http.Server(8080, (req) => {
 *   var name = req.query.get('name');
 *   var msg = name ? `Hello ${name}!` : 'Hello world!';
 *   
 *   req.response.write(msg);
 * });
 * 
 * svr.start();
 * ```
 * 
 * 这里我们使用了 `req.query` 这个 Collection 类型，它代表 HTTP 请求 URL 中的查询参数。 
 * 
 * 我们向服务程序通过浏览器访问 http://127.0.0.1:8080/?name=fibjs 得到的服务端响应内容是 `Hello fibjs!`。
 *  
 */
declare class Class_HttpRequest extends Class_HttpMessage {
    /**
     * @description HttpRequest 构造函数，创建一个新的 HttpRequest 对象 
     */
    constructor();

    /**
     * @description 获取响应消息对象
     */
    readonly response: Class_HttpResponse;

    /**
     * @description 查询和设置请求方法 
     */
    method: string;

    /**
     * @description 查询和设置请求地址 
     */
    address: string;

    /**
     * @description 查询和设置请求查询字符串 
     */
    queryString: string;

    /**
     * @description 获取包含消息 cookies 的容器
     */
    readonly cookies: Class_HttpCollection;

    /**
     * @description 获取包含消息 form 的容器
     */
    readonly form: Class_HttpCollection;

    /**
     * @description 获取包含消息 query 的容器
     */
    readonly query: Class_HttpCollection;

}

