/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description HttpClient 是针对 HTTP 客户端功能设计的类库，提供了基本的 HTTP/HTTPS 请求、代理访问、cookie 管理等功能
 * 
 * 使用 HttpClient 可以轻松地访问和操作 web 页面，这里举一个简单的例子——在一个 web 页面上打印出其源代码：
 * 
 * ```JavaScript
 * const http = require('http');
 * 
 * const res = http.get('http://www.example.com/');
 * 
 * console.log(res.body.readAll().toString());
 * ```
 * 
 * 在该例子中，通过 require 引入 http 模块，然后使用 http.get 发起一个 get 请求，其中 url 参数指定了请求的网址。因为 http.get 方法返回的是一个 HttpResponse 对象，所以可以通过其 body 属性来访问请求返回的主体内容并通过 toString 方法将其转化为字符串。
 * 
 * 当请求的 url 是 https 类型而不是 http 类型时，代码只需要将 http 改为 https 即可：
 * 
 * ```JavaScript
 * const http = require('http');
 * 
 * const res = http.get('https://www.example.com/');
 * 
 * console.log(res.body.readAll().toString());
 * ```
 * 
 * 除此之外，还有通过 HttpClient 直接发起 POST 请求、设置 User-Agent 的例子：
 * 
 * ```JavaScript
 * const http = require('http');
 * 
 * const httpClient = new http.Client();
 * httpClient.userAgent = 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36';
 * const res = httpClient.post('http://www.example.com/post', {
 *     json: { name: 'fibjs', version: '0.31.0' }
 * });
 * console.log(res.body.readAll().toString());
 * ```
 * 
 * 在该例子中，首先创建了一个 HttpClient 对象 httpClient，并设置其 userAgent 为浏览器的 User-Agent。然后通过它的 post 方法来发起一个 post 请求，其中参数 name 和 version 来指定请求的主体内容。最后将返回值的主体内容输出。
 *  
 */
declare class Class_HttpClient extends Class_object {
    /**
     * @description HttpClient 构造函数，创建一个新的HttpClient对象 
     */
    constructor();

    /**
     * @description HttpClient 构造函数，创建一个新的HttpClient对象
     *      @param context 指定创建 HttpClient 使用的安全上下文
     *      
     */
    constructor(context: Class_SecureContext);

    /**
     * @description HttpClient 构造函数，创建一个新的HttpClient对象
     * 
     *      options 除用于创建 SecureContext 的属性之外，还需提供以下属性：
     *      - keepAlive: 指定是否保持连接
     *      - timeout: 指定超时时间
     *      - enableCookie: 指定是否启用 cookie 功能
     *      - autoRedirect: 指定是否启用自动重定向功能
     *      - enableEncoding: 指定是否启用自动解压缩功能
     *      - maxHeadersCount: 指定最大请求头个数
     *      - maxHeaderSize: 指定最大请求头长度
     *      - maxBodySize: 指定 body 最大尺寸
     *      - userAgent: 指定浏览器标识
     *      - poolSize: 指定 keep-alive 最大缓存连接数
     *      - poolTimeout: 指定 keep-alive 缓存连接超时时间
     *      - http_proxy: 指定 http 代理地址
     *      - https_Proxy: 指定 https 代理地址
     * 
     *      @param options 使用 tls.createSecureContext 创建安全上下文需要的选项
     *      
     */
    constructor(options: FIBJS.GeneralObject);

    /**
     * @description 返回http客户端的 HttpCookie 对象列表 
     */
    readonly cookies: any[];

    /**
     * @description 查询和设定是否保持连接 
     */
    keepAlive: boolean;

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
     * @description 查询和设置最大请求头个数，缺省为 128 
     */
    maxHeadersCount: number;

    /**
     * @description 查询和设置最大请求头长度，缺省为 8192 
     */
    maxHeaderSize: number;

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
     * @description 查询和设置 http 请求代理，支持 http/https/socks5 代理 
     */
    http_proxy: string;

    /**
     * @description 查询和设置 https 请求代理，支持 http/https/socks5 代理，不设置，或者设置为空，则复用 http_proxy 
     */
    https_proxy: string;

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
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "response_body": SeekableStream // specify the response.body stream
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
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
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
    request(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    request(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 请求 opts 指定的 url，并返回结果
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    request(opts: FIBJS.GeneralObject): Class_HttpResponse;

    request(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 用 GET 方法请求指定的 url，并返回结果，等同于 request("GET", ...)
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
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
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
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
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
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
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
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
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
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

    /**
     * @description 用 HEAD 方法请求指定的 url，并返回结果，等同于 request("PATCH", ...)
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
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
    head(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    head(url: string, opts?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

}

