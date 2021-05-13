/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/X509Cert.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description http客户端对象
 *  
 *  http客户端对象模拟浏览器环境缓存cookie，并在访问url的时候携带对应的cookie，不同的http客户端对象是相互隔离的，提供http的request、get、post等方法。
 *  用法如下：
 * 
 *  ```JavaScript
 *  var http = require('http');
 *  var httpClient = new http.Client();
 *  httpClient.request('GET', 'http://fibjs.org');
 *  ```
 *  
 */
declare class Class_HttpClient extends Class_object {
    /**
     * @description HttpClient 构造函数，创建一个新的HttpClient对象 
     */
    constructor();

    /**
     * @description 返回http客户端的 HttpCookie 对象列表 
     */
    readonly cookies: any[];

    /**
     * @description 查询和设置超时时间 单位毫秒
     */
    timeout: number;

    /**
     * @description cookie 功能开关，默认开启 
     */
    enableCookie: boolean;

    /**
     * @description 自动 redirect 功能开关，默认开启 
     */
    autoRedirect: boolean;

    /**
     * @description 自动解压缩功能开关，默认开启 
     */
    enableEncoding: boolean;

    /**
     * @description 查询和设置 body 最大尺寸，以 MB 为单位，缺省为 -1，不限制尺寸 
     */
    maxBodySize: number;

    /**
     * @description 查询和设置 http 请求中的浏览器标识 
     */
    userAgent: string;

    /**
     * @description 查询和设置 keep-alive 最大缓存连接数，缺省 128 
     */
    poolSize: number;

    /**
     * @description 查询和设置 keep-alive 缓存连接超时时间，缺省 10000 ms 
     */
    poolTimeout: number;

    /**
     * @description 查询和设置代理服务器 
     */
    proxyAgent: string;

    /**
     * @description 查询和设置连接 https 时的证书验证模式, 参考 ssl 模块的 VERIFY_* 常量, 默认值为 ssl.verification 
     */
    sslVerification: number;

    /**
     * @description 设定缺省客户端证书
     *     @param crt 证书，用于发送给服务器验证客户端
     *     @param key 私钥，用于与客户端会话
     *    
     */
    setClientCert(crt: Class_X509Cert, key: Class_PKey): void;

    /**
     * @description 发送 http 请求到指定的流对象，并返回结果
     *      @param conn 指定处理请求的流对象
     *      @param req 要发送的 HttpRequest 对象
     *      @return 返回服务器响应
     *      
     */
    request(conn: Class_Stream, req: Class_HttpRequest): Class_HttpResponse;

    request(conn: Class_Stream, req: Class_HttpRequest, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 发送 http 请求到指定的流对象，并返回结果
     *      @param conn 指定处理请求的流对象
     *      @param req 要发送的 HttpRequest 对象
     *      @param response_body 指定 response.body 的流
     *      @return 返回服务器响应
     *      
     */
    request(conn: Class_Stream, req: Class_HttpRequest, response_body: Class_SeekableStream): Class_HttpResponse;

    request(conn: Class_Stream, req: Class_HttpRequest, response_body: Class_SeekableStream, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 请求指定的 url，并返回结果
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "response_body": SeekableStream // 指定接受 resposne 数据的流
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param method 指定 http 请求方法：GET, POST 等
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    request(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    request(method: string, url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 用 GET 方法请求指定的 url，并返回结果，等同于 request("GET", ...)
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    get(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    get(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 用 POST 方法请求指定的 url，并返回结果，等同于 request("POST", ...)
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    post(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    post(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 用 DELETE 方法请求指定的 url，并返回结果，等同于 request("DELETE", ...)
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    del(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    del(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 用 PUT 方法请求指定的 url，并返回结果，等同于 request("PUT", ...)
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    put(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    put(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 用 PATCH 方法请求指定的 url，并返回结果，等同于 request("PATCH", ...)
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    patch(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    patch(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

}

