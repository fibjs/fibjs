/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
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
declare class Class_HttpClient extends Class_EventEmitter {
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
     *      - enableH2: 指定是否启用 HTTP/2 自动升级
     *      - maxHeadersCount: 指定最大请求头个数
     *      - maxHeaderSize: 指定最大请求头长度
     *      - maxBodySize: 指定 body 最大尺寸
     *      - userAgent: 指定浏览器标识
     *      - poolTimeout: 指定 keep-alive 缓存连接超时时间
     *      - proxyEnv: 指定代理配置环境变量，包含 HTTP_PROXY、HTTPS_PROXY、NO_PROXY 及其小写形式
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
     * @description HTTP/2 自动升级开关，默认关闭 
     */
    enableH2: boolean;

    /**
     * @description 查询和设置最大请求头个数，缺省为 128 
     */
    maxHeadersCount: number;

    /**
     * @description 查询和设置最大请求头长度，缺省为 8192 
     */
    maxHeaderSize: number;

    /**
     * @description 查询和设置 chunk 最大尺寸，以 MB 为单位，缺省为 2 
     */
    maxChunkSize: number;

    /**
     * @description 查询和设置 body 最大尺寸，以 MB 为单位，缺省为 -1，不限制尺寸 
     */
    maxBodySize: number;

    /**
     * @description 查询和设置 http 请求中的浏览器标识 
     */
    userAgent: string;

    /**
     * @description 查询和设置 keep-alive 缓存连接超时时间，缺省 10000 ms 
     */
    poolTimeout: number;

    /**
     * @description 查询和设置代理配置环境变量，支持 HTTP_PROXY、HTTPS_PROXY、NO_PROXY 及其小写形式 
     */
    proxyEnv: FIBJS.GeneralObject;

    /**
     * @description 查询和设置每个主机的最大连接数，缺省为无限制 
     */
    maxSockets: number;

    /**
     * @description 查询和设置所有主机的最大连接总数，缺省为无限制 
     */
    maxTotalSockets: number;

    /**
     * @description 查询和设置每个主机的最大空闲连接数，缺省为 256 
     */
    maxFreeSockets: number;

    /**
     * @description 查询和设置 getName() 中使用的默认端口，缺省为 80 
     */
    defaultPort: number;

    /**
     * @description 查询和设置 getName() 中使用的默认协议，缺省为 "http:" 
     */
    protocol: string;

    /**
     * @description 返回以 host:port 为键的空闲连接映射 
     */
    readonly freeSockets: FIBJS.GeneralObject;

    /**
     * @description 返回以 host:port 为键的使用中的连接映射 
     */
    readonly sockets: FIBJS.GeneralObject;

    /**
     * @description 返回所有主机的使用中连接总数 
     */
    readonly totalSocketCount: number;

    /**
     * @description 返回给定请求选项的唯一键，用于连接池
     *      @param options 请求选项
     *      @return 返回连接池键字符串
     *      
     */
    getName(options?: FIBJS.GeneralObject): string;

    /**
     * @description 销毁当前正在使用的所有连接 
     */
    destroy(): void;

    /**
     * @description 发送 http 请求到指定的流对象，并返回结果
     *      @param conn 指定处理请求的流对象
     *      @param req 要发送的 HttpRequest 对象
     *      @return 返回服务器响应
     *      
     */
    request(conn: Class_Stream, req: Class_HttpRequest): Class_HttpRequest;

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
     *          "path": "", // pathname 的别名，用于 request 选项。
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param method 指定 http 请求方法：GET, POST 等
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回服务器响应
     *      
     */
    requestSync(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

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
    requestSync(opts: FIBJS.GeneralObject): Class_HttpResponse;

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
    requestSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
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
     *          "path": "", // pathname 的别名，用于 request 选项。
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param method 指定 http 请求方法：GET, POST 等
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    request(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 请求 opts 指定的 url，并返回 HttpRequest 对象
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
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    request(opts: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 请求指定的 url，并返回 HttpRequest 对象
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
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    request(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param method 指定 http 请求方法：GET, POST 等
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    request(method: string, url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 请求 opts 指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    request(opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    request(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    request(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param method 指定 http 请求方法：GET, POST 等
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    request(method: string, url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    getSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 用 GET 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    get(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 用 GET 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    get(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 用 GET 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    get(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    postSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 用 POST 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    post(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 用 POST 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    post(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 用 POST 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    post(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    delSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 用 DELETE 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    del(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 用 DELETE 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    del(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 用 DELETE 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    del(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    putSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 用 PUT 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    put(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 用 PUT 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    put(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 用 PUT 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    put(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    patchSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 用 PATCH 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    patch(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 用 PATCH 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    patch(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 用 PATCH 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    patch(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    headSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 用 HEAD 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      opts 包含请求的附加选项，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "query": {},
     *          "headers": {},
     *          "signal": AbortSignal // 用于取消请求的 AbortSignal 对象
     *      }
     *      ```
     *      缺省为 {}，不包含任何附加信息
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpRequest 对象（可监听 'response' 事件接收响应）
     *      
     */
    head(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description 用 HEAD 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param opts 指定附加信息
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    head(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 用 HEAD 方法请求指定的 url，注册回调接收响应，返回 HttpRequest 对象
     *      @param url 指定 url，必须是包含主机的完整 url
     *      @param callback 响应回调函数，接收 HttpResponse 作为参数
     *      @return 返回 HttpRequest 对象
     *      
     */
    head(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description 使用 Web Fetch 标准发送请求，返回 HttpResponse 对象
     *      @param url 指定 url，必须是包含 host 的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpResponse 对象
     *      
     */
    fetch(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    fetch(url: string, opts?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 使用 Web Fetch 标准发送请求，返回 HttpResponse 对象
     *      @param url 指定 url，必须是包含 host 的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpResponse 对象
     *      
     */
    fetchSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 使用 Web Fetch 标准发送请求，返回 HttpResponse 对象
     *      @param url 指定 url，必须是包含 host 的完整 url
     *      @param opts 指定附加信息
     *      @return 返回 HttpResponse 对象
     *      
     */
    fetchAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponse>;

    /**
     * @description 使用 Web Fetch 标准发送请求，以 HttpRequest 对象作为请求源，返回 HttpResponse 对象
     *      opts 可覆盖 request 中的请求字段，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "method": "GET", // 覆盖 request 中的请求方法
     *          "headers": {}, // 与 request.headers 合并，opts 中的同名头覆盖 request 中的
     *          "body": SeekableStream | Buffer | String | {}, // 覆盖 request.body
     *          "keepAlive": unknown, // 覆盖连接保持设置
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "redirect": "follow", // 重定向模式："follow"（默认）| "error" | "manual"
     *          "signal": AbortSignal, // 用于取消请求的 AbortSignal 对象
     *          "streaming": false // 是否以流模式返回响应体
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不覆盖任何 request 中的信息
     *      @param request 请求源对象，提供 url、method、headers、body 等基础信息
     *      @param opts 指定附加信息，可覆盖 request 中的对应字段
     *      @return 返回服务器响应，包含 status、headers、body、ok、redirected、url、type 等属性
     *      
     */
    fetch(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    fetch(request: Class_HttpRequest, opts?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description 使用 Web Fetch 标准发送请求，以 HttpRequest 对象作为请求源，返回 HttpResponse 对象
     *      opts 可覆盖 request 中的请求字段，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "method": "GET", // 覆盖 request 中的请求方法
     *          "headers": {}, // 与 request.headers 合并，opts 中的同名头覆盖 request 中的
     *          "body": SeekableStream | Buffer | String | {}, // 覆盖 request.body
     *          "keepAlive": unknown, // 覆盖连接保持设置
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "redirect": "follow", // 重定向模式："follow"（默认）| "error" | "manual"
     *          "signal": AbortSignal, // 用于取消请求的 AbortSignal 对象
     *          "streaming": false // 是否以流模式返回响应体
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不覆盖任何 request 中的信息
     *      @param request 请求源对象，提供 url、method、headers、body 等基础信息
     *      @param opts 指定附加信息，可覆盖 request 中的对应字段
     *      @return 返回服务器响应，包含 status、headers、body、ok、redirected、url、type 等属性
     *      
     */
    fetchSync(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description 使用 Web Fetch 标准发送请求，以 HttpRequest 对象作为请求源，返回 HttpResponse 对象
     *      opts 可覆盖 request 中的请求字段，支持的内容如下：
     *      ```JavaScript
     *      {
     *          "method": "GET", // 覆盖 request 中的请求方法
     *          "headers": {}, // 与 request.headers 合并，opts 中的同名头覆盖 request 中的
     *          "body": SeekableStream | Buffer | String | {}, // 覆盖 request.body
     *          "keepAlive": unknown, // 覆盖连接保持设置
     *          "timeout": 0, // 请求超时时间（毫秒），缺省使用客户端默认设置
     *          "redirect": "follow", // 重定向模式："follow"（默认）| "error" | "manual"
     *          "signal": AbortSignal, // 用于取消请求的 AbortSignal 对象
     *          "streaming": false // 是否以流模式返回响应体
     *      }
     *      ```
     *      其中 body，json，pack 不得同时出现。缺省为 {}，不覆盖任何 request 中的信息
     *      @param request 请求源对象，提供 url、method、headers、body 等基础信息
     *      @param opts 指定附加信息，可覆盖 request 中的对应字段
     *      @return 返回服务器响应，包含 status、headers、body、ok、redirected、url、type 等属性
     *      
     */
    fetchAsync(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponse>;

}

