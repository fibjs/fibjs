/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/HttpCookie.d.ts" />
/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/HttpClient.d.ts" />
/// <reference path="../interface/HttpsServer.d.ts" />
/// <reference path="../interface/HttpHandler.d.ts" />
/// <reference path="../interface/HttpRepeater.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/X509Cert.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description 超文本传输协议模块，用以支持 http 协议处理，模块别名：https 
 */
declare module 'http' {
    /**
     * @description 创建一个 http 请求对象，参见 HttpRequest 
     */
    const Request: typeof Class_HttpRequest;

    /**
     * @description 创建一个 http 响应对象，参见 HttpResponse 
     */
    const Response: typeof Class_HttpResponse;

    /**
     * @description 创建一个 http cookie 对象，参见 HttpCookie 
     */
    const Cookie: typeof Class_HttpCookie;

    /**
     * @description 创建一个 http 服务器，参见 HttpServer 
     */
    const Server: typeof Class_HttpServer;

    /**
     * @description 创建一个 http 客户端，参见 HttpClient 
     */
    const Client: typeof Class_HttpClient;

    /**
     * @description 创建一个 https 服务器，参见 HttpsServer 
     */
    const HttpsServer: typeof Class_HttpsServer;

    /**
     * @description 创建一个 http 协议处理器对象，参见 HttpHandler 
     */
    const Handler: typeof Class_HttpHandler;

    /**
     * @description 创建一个 http 请求转发处理器对象，参见 HttpRepeater 
     */
    const Repeater: typeof Class_HttpRepeater;

    /**
     * @description 返回标准的 HTTP 响应状态码的集合，以及各自的简短描述。 
     */
    const STATUS_CODES: any[];

    /**
     * @description 返回http客户端的 HttpCookie 对象列表 
     */
    const cookies: any[];

    /**
     * @description 查询和设置超时时间 
     */
    var timeout: number;

    /**
     * @description cookie 功能开关，默认开启 
     */
    var enableCookie: boolean;

    /**
     * @description 自动 redirect 功能开关，默认开启 
     */
    var autoRedirect: boolean;

    /**
     * @description 自动解压缩功能开关，默认开启 
     */
    var enableEncoding: boolean;

    /**
     * @description 查询和设置 body 最大尺寸，以 MB 为单位，缺省为 -1，不限制尺寸 
     */
    var maxBodySize: number;

    /**
     * @description 查询和设置 http 请求中的浏览器标识 
     */
    var userAgent: string;

    /**
     * @description 查询和设置 keep-alive 最大缓存连接数，缺省 128 
     */
    var poolSize: number;

    /**
     * @description 查询和设置 keep-alive 缓存连接超时时间，缺省 10000 ms 
     */
    var poolTimeout: number;

    /**
     * @description 查询和设置代理服务器 
     */
    var proxyAgent: string;

    /**
     * @description 创建一个 http 静态文件处理器，用以用静态文件响应 http 消息
     * 
     *      fileHandler 支持 gzip 预压缩，当请求接受 gzip 编码，且相同路径下 filename.ext.gz 文件存在时，将直接返回此文件，
     *      从而避免重复压缩带来服务器负载。
     *      @param root 文件根路径
     *      @param mimes 扩展 mime 设置
     *      @param autoIndex 是否支持浏览目录文件，缺省为 false，不支持
     *      @return 返回一个静态文件处理器用于处理 http 消息
     *      
     */
    function fileHandler(root: string, mimes?: FIBJS.GeneralObject, autoIndex?: boolean): Class_Handler;

    /**
     * @description 设定缺省客户端证书
     *     @param crt 证书，用于发送给服务器验证客户端
     *     @param key 私钥，用于与客户端会话
     *    
     */
    function setClientCert(crt: Class_X509Cert, key: Class_PKey): void;

    /**
     * @description 发送 http 请求到指定的流对象，并返回结果
     *      @param conn 指定处理请求的流对象
     *      @param req 要发送的 HttpRequest 对象
     *      @return 返回服务器响应
     *      
     */
    function request(conn: Class_Stream, req: Class_HttpRequest): Class_HttpResponse;

    function request(conn: Class_Stream, req: Class_HttpRequest, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 发送 http 请求到指定的流对象，并返回结果
     *      @param conn 指定处理请求的流对象
     *      @param req 要发送的 HttpRequest 对象
     *      @param response_body 指定 response.body 的流
     *      @return 返回服务器响应
     *      
     */
    function request(conn: Class_Stream, req: Class_HttpRequest, response_body: Class_SeekableStream): Class_HttpResponse;

    function request(conn: Class_Stream, req: Class_HttpRequest, response_body: Class_SeekableStream, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

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
     *          "response_body": SeekableStream //指定接受 resposne 数据的流
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param method 指定 http 请求方法：GET, POST 等
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    function request(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    function request(method: string, url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

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
    function get(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    function get(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

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
    function post(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    function post(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

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
    function del(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    function del(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

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
    function put(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    function put(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

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
    function patch(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    function patch(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

}

