/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Socket.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Smtp.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/**
 * @description net 模块提供网络基础能力，包括建立 TCP 连接、域名解析、IP 地址检测、创建 TCP 服务器等，是 http、tls、smtp 等网络模块的基础
 * 
 *  模块的主要能力：
 * 
 *  - **连接**：`connect` 以多种形式建立 TCP 连接，支持 `tcp://`、`ssl://`、`unix:`、`pipe://` 协议；
 *  - **解析**：`resolve`、`ip`、`ipv6` 查询主机名的地址；
 *  - **服务器**：`createServer` 创建 TCP 服务器；
 *  - **检测**：`isIP`、`isIPv4`、`isIPv6` 检测 IP 地址格式；
 *  - **对象别名**：`Socket`、`Smtp`、`TcpServer`、`Url`。
 * 
 *  引用方式：
 * 
 *  ```JavaScript
 *  var net = require('net');
 *  ```
 * 
 *  建立 TCP 连接示例：
 * 
 *  ```JavaScript
 *  var net = require('net');
 * 
 *  // 指定端口与主机
 *  var sock = net.connect(80, 'example.com');
 *  sock.send('GET / HTTP/1.0\r\n\r\n');
 *  console.log(sock.recv());
 *  sock.close();
 * 
 *  // 使用 URL 形式，支持 tcp:// 与 ssl:// 协议
 *  var ssl = net.connect('ssl://example.com:443');
 *  ```
 *  
 */
declare module 'net' {
    /**
     * @description 地址集常量，指定 unix socket 
     */
    export const AF_UNIX: 1;

    /**
     * @description 地址集常量，指定 Windows pipe 
     */
    export const AF_PIPE: 1;

    /**
     * @description 地址集常量，指定 ipv4
     */
    export const AF_INET: 2;

    /**
     * @description 地址集常量，指定 ipv6
     */
    export const AF_INET6: 10;

    /**
     * @description 查询和设置 socket 后端是否使用 uv，缺省为 false 
     */
    var use_uv_socket: boolean;

    /**
     * @description 查询当前运行环境网络信息
     *      @return 返回网卡信息
     *     
     */
    function info(): FIBJS.GeneralObject;

    /**
     * @description 查询给定的主机名的地址
     * 
     *      family 指定返回的地址族，取值为 AF_INET 或 AF_INET6，其他取值抛出异常。
     *      @param name 指定主机名
     *      @param family 指定查询返回类型，缺省为 AF_INET
     *      @return 返回查询的 ip 字符串
     *      
     */
    function resolve(name: string, family: number): string;

    function resolve(name: string, family: number, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 查询给定的主机名的地址
     * 
     *      family 指定返回的地址族，取值为 AF_INET 或 AF_INET6，其他取值抛出异常。
     *      @param name 指定主机名
     *      @param family 指定查询返回类型，缺省为 AF_INET
     *      @return 返回查询的 ip 字符串
     *      
     */
    function resolveSync(name: string, family: number): string;

    /**
     * @description 查询给定的主机名的地址
     * 
     *      family 指定返回的地址族，取值为 AF_INET 或 AF_INET6，其他取值抛出异常。
     *      @param name 指定主机名
     *      @param family 指定查询返回类型，缺省为 AF_INET
     *      @return 返回查询的 ip 字符串
     *      
     */
    function resolveAsync(name: string, family: number): Promise<string>;

    /**
     * @description 快速查询的主机地址，等效于 resolve(name)
     *      @param name 指定主机名
     *      @return 返回查询的 ip 字符串
     *      
     */
    function ip(name: string): string;

    function ip(name: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 快速查询的主机地址，等效于 resolve(name)
     *      @param name 指定主机名
     *      @return 返回查询的 ip 字符串
     *      
     */
    function ipSync(name: string): string;

    /**
     * @description 快速查询的主机地址，等效于 resolve(name)
     *      @param name 指定主机名
     *      @return 返回查询的 ip 字符串
     *      
     */
    function ipAsync(name: string): Promise<string>;

    /**
     * @description 快速查询的主机 ipv6 地址，等效于 resolve(name, net.AF_INET6)
     *      @param name 指定主机名
     *      @return 返回查询的 ipv6 字符串
     *      
     */
    function ipv6(name: string): string;

    function ipv6(name: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 快速查询的主机 ipv6 地址，等效于 resolve(name, net.AF_INET6)
     *      @param name 指定主机名
     *      @return 返回查询的 ipv6 字符串
     *      
     */
    function ipv6Sync(name: string): string;

    /**
     * @description 快速查询的主机 ipv6 地址，等效于 resolve(name, net.AF_INET6)
     *      @param name 指定主机名
     *      @return 返回查询的 ipv6 字符串
     *      
     */
    function ipv6Async(name: string): Promise<string>;

    /**
     * @description 创建一个 Socket 对象，参见 Socket 
     */
    const Socket: typeof Class_Socket;

    /**
     * @description 创建一个 Socket 对象并建立连接
     * 
     *      options 参数可以包含以下属性：
     *       - port: 指定对方端口
     *       - host: 指定对方地址或主机名
     *       - timeout: 指定超时时间，单位是毫秒，默认为 0
     * 
     *      @param options 指定连接选项对象
     *      @return 返回连接成功的 Socket 对象
     *      
     */
    function connect(options: FIBJS.GeneralObject): Class_Stream;

    function connect(options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description 创建一个 Socket 对象并建立连接
     * 
     *      options 参数可以包含以下属性：
     *       - port: 指定对方端口
     *       - host: 指定对方地址或主机名
     *       - timeout: 指定超时时间，单位是毫秒，默认为 0
     * 
     *      @param options 指定连接选项对象
     *      @return 返回连接成功的 Socket 对象
     *      
     */
    function connectSync(options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description 创建一个 Socket 对象并建立连接
     * 
     *      options 参数可以包含以下属性：
     *       - port: 指定对方端口
     *       - host: 指定对方地址或主机名
     *       - timeout: 指定超时时间，单位是毫秒，默认为 0
     * 
     *      @param options 指定连接选项对象
     *      @return 返回连接成功的 Socket 对象
     *      
     */
    function connectAsync(options: FIBJS.GeneralObject): Promise<Class_Stream>;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param options 指定连接选项对象，可以包含以下属性：
     *       - port: 指定对方端口
     *       - host: 指定对方地址或主机名
     *       - timeout: 指定超时时间，单位是毫秒，默认为 0
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    function connect(options: FIBJS.GeneralObject, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 创建一个 Socket 或 SslSocket 对象并建立连接
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port，也可以是：unix:/usr/local/proc1 或者 pipe://./pipe/proc1，连接 pipe 时需要用 `/` 替换 `\`
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Socket 或者 SslSocket 对象
     *      
     */
    function connect(url: string, timeout?: number): Class_Stream;

    function connect(url: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description 创建一个 Socket 或 SslSocket 对象并建立连接
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port，也可以是：unix:/usr/local/proc1 或者 pipe://./pipe/proc1，连接 pipe 时需要用 `/` 替换 `\`
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Socket 或者 SslSocket 对象
     *      
     */
    function connectSync(url: string, timeout?: number): Class_Stream;

    /**
     * @description 创建一个 Socket 或 SslSocket 对象并建立连接
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port，也可以是：unix:/usr/local/proc1 或者 pipe://./pipe/proc1，连接 pipe 时需要用 `/` 替换 `\`
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Socket 或者 SslSocket 对象
     *      
     */
    function connectAsync(url: string, timeout?: number): Promise<Class_Stream>;

    /**
     * @description 创建一个 Socket 对象并建立连接
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Socket 对象
     *      
     */
    function connect(port: number, host?: string, timeout?: number): Class_Stream;

    function connect(port: number, host?: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description 创建一个 Socket 对象并建立连接
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Socket 对象
     *      
     */
    function connectSync(port: number, host?: string, timeout?: number): Class_Stream;

    /**
     * @description 创建一个 Socket 对象并建立连接
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Socket 对象
     *      
     */
    function connectAsync(port: number, host?: string, timeout?: number): Promise<Class_Stream>;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param port 指定对方端口
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    function connect(port: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    function connect(port: number, host: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    function connect(port: number, host: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param path 指定 unix socket 或 Windows pipe 路径
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    function connect(path: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param path 指定 unix socket 或 Windows pipe 路径
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    function connect(path: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 创建一个 Smtp 对象，参见 Smtp 
     */
    const Smtp: typeof Class_Smtp;

    /**
     * @description 创建一个 Smtp 对象并建立连接，参见 Smtp
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Smtp 对象
     *      
     */
    function openSmtp(url: string, timeout?: number): Class_Smtp;

    function openSmtp(url: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Smtp)=>any): void;

    /**
     * @description 创建一个 Smtp 对象并建立连接，参见 Smtp
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Smtp 对象
     *      
     */
    function openSmtpSync(url: string, timeout?: number): Class_Smtp;

    /**
     * @description 创建一个 Smtp 对象并建立连接，参见 Smtp
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接成功的 Smtp 对象
     *      
     */
    function openSmtpAsync(url: string, timeout?: number): Promise<Class_Smtp>;

    /**
     * @description 创建一个 TcpServer 对象，参见 TcpServer 
     */
    const TcpServer: typeof Class_TcpServer;

    /**
     * @description 创建一个 TCP 服务器
     *      @param options 服务器选项对象，可以包含以下属性：
     *       - address: 指定监听地址，默认为所有地址
     *       - port: 指定监听端口，可选。不提供时需调用 listen() 启动
     *      @param listener 连接处理函数
     *      @return 返回 TcpServer 对象
     *      
     */
    function createServer(options: FIBJS.GeneralObject, listener: Class_Handler): Class_TcpServer;

    /**
     * @description 创建一个 TCP 服务器
     *      @param listener 连接处理函数
     *      @return 返回未绑定端口的 TcpServer 对象，需调用 listen() 启动
     *      
     */
    function createServer(listener: Class_Handler): Class_TcpServer;

    /**
     * @description 创建一个 UrlObject 对象，参见 UrlObject 
     */
    const Url: typeof Class_UrlObject;

    /**
     * @description 查询当前系统异步网络引擎
     *      @return 返回网络引擎名称
     *     
     */
    function backend(): string;

    /**
     * @description 检测输入是否是 IP 地址
     *      @param ip 指定要检测的字符串
     *      @return 非合法的 IP 地址，返回 0, 如果是 IPv4 则返回 4，如果是 IPv6 则返回 6
     *     
     */
    function isIP(ip?: string): number;

    /**
     * @description 检测输入是否是 IPv4 地址
     *      @param ip 指定要检测的字符串
     *      @return 如果是 IPv4 则返回 true.否则返回 false
     *     
     */
    function isIPv4(ip?: string): boolean;

    /**
     * @description 检测输入是否是 IPv6 地址
     *      @param ip 指定要检测的字符串
     *      @return 如果是 IPv6 则返回 true.否则返回 false
     *     
     */
    function isIPv6(ip?: string): boolean;

    /**
     * @description 查询 net.connect 默认是否启用自动地址族选择，兼容 Node.js >= 18.13
     *      @return 返回当前默认值，缺省为 true
     *     
     */
    function getDefaultAutoSelectFamily(): boolean;

    /**
     * @description 设置 net.connect 默认是否启用自动地址族选择，兼容 Node.js >= 18.13
     *      @param enabled 指定默认值，必须为布尔值
     *     
     */
    function setDefaultAutoSelectFamily(enabled: boolean): void;

    /**
     * @description 查询默认的自动地址族选择超时时间，兼容 Node.js >= 18.13
     *      @return 返回当前默认超时时间，单位毫秒，缺省为 250
     *     
     */
    function getDefaultAutoSelectFamilyAttemptTimeout(): number;

    /**
     * @description 设置默认的自动地址族选择超时时间，兼容 Node.js >= 18.13
     *      @param milliseconds 指定默认超时时间，单位毫秒，必须为大于等于 10 的整数
     *     
     */
    function setDefaultAutoSelectFamilyAttemptTimeout(milliseconds: number): void;

}

