/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/HttpClient.d.ts" />
/**
 * @description HttpRepeater 是一个 HTTP 请求转发器，可以将 HTTP 请求转发到指定的后端服务器并获得响应。它常常用于需要在前端与多个服务端交互的复杂系统中，或者用于负载均衡
 * 
 * 使用 HttpRepeater 非常简单，只需要在创建实例时为其提供后端服务器的 URL 或负载均衡 URL 数组即可。
 * 
 * 如下是一个使用单个后端的示例：
 * ```JavaScript
 * var http = require('http');
 * var serverUrl = 'http://localhost:' + actualPort + '/example'
 * var repeater = new http.Repeater(serverUrl)
 * 
 * var server = new http.Server(8081, repeater);
 * server.start();
 * ```
 * 下面是一个使用 URL 数组实现负载均衡的示例：
 * ```JavaScript
 * var serverURLs = [
 *   'http://server1.example.com',
 *   'http://server2.example.com',
 *   'http://server3.example.com'
 * ]
 * var repeater = new http.Repeater(serverURLs)
 * 
 * var server = new http.Server(8081, repeater);
 * server.start();
 * ```
 *  
 */
declare class Class_HttpRepeater extends Class_Handler {
    /**
     * @description HttpRepeater 构造函数，创建一个新的 HttpRepeater 对象 
     *      @param url 指定一个后端服务器 url
     *     
     */
    constructor(url: string);

    /**
     * @description HttpRepeater 构造函数，创建一个新的 HttpRepeater 对象
     *      @param urls 指定一组后端服务器 url
     *     
     */
    constructor(urls: any[]);

    /**
     * @description 加载一组新的后端 url
     *      @param urls 指定一组后端服务器 url
     *     
     */
    load(urls: any[]): void;

    /**
     * @description 查询当前后端服务器 url 列表
     */
    readonly urls: any[];

    /**
     * @description 请求转发处理器内部使用的 HttpClient 对象
     */
    readonly client: Class_HttpClient;

}

