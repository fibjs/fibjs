/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/HttpClient.d.ts" />
/**
 * @description http 请求转发处理器
 *  
 *  http 用于将 http 请求转发到后端服务器
 *  用法如下：
 * 
 *  ```JavaScript
 *  var http = require('http');
 *  var hr = new http.Repeater('http://10.0.0.100:8080/test');
 *  ```
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

