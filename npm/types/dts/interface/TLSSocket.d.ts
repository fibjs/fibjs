/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
/**
 * @description SSL/TLS 网络套接口对象
 * 
 * TLSSocket 属于 tls 模块，创建方法
 *  ```JavaScript
 *  var s = new tls.TLSSocket();
 *  ```
 * 
 */
declare class Class_TLSSocket extends Class_Stream {
    /**
     * @description 创建一个新的 TLSSocket 对象，使用当前缺省 SecureContext 
     */
    constructor();

    /**
     * @description 根据 context 创建一个新的 TLSSocket 对象
     *      @param context 指定创建 TLSSocket 使用的安全上下文
     *     
     */
    constructor(context: Class_SecureContext);

    /**
     * @description 根据 options 创建一个新的 TLSSocket 对象
     *      @param options 使用 tls.createSecureContext 创建安全上下文需要的选项
     *      @param isServer 是否是服务端模式
     *     
     */
    constructor(options: FIBJS.GeneralObject, isServer?: boolean);

    /**
     * @description 在给定的连接上连接 SSL/TLS 连接，客户端模式
     *      @param socket 给定的底层连接
     *      @param server_name 服务端名称，用于验证服务端证书
     *     
     */
    connect(socket: Class_Stream, server_name?: string): void;

    connect(socket: Class_Stream, server_name?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 在给定的连接上连接 SSL/TLS 连接，服务端模式
     *      @param socket 给定的底层连接
     *     
     */
    accept(socket: Class_Stream): void;

    accept(socket: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 当前连接协商的 SSL/TLS 协议版本
     *      @return 返回 SSL/TLS 协议版本
     *     
     */
    getProtocol(): string;

    /**
     * @description 当前连接协商的本地证书
     *      @return 返回本地证书
     *     
     */
    getX509Certificate(): Class_X509Certificate;

    /**
     * @description 当前连接协商的对等证书
     *      @return 返回对等证书
     *     
     */
    getPeerX509Certificate(): Class_X509Certificate;

    /**
     * @description 查询当前 TLSSocket 使用的 SecureContext 
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description 查询当前连接的对方地址 
     */
    readonly remoteAddress: string;

    /**
     * @description 查询当前连接的对方端口 
     */
    readonly remotePort: number;

    /**
     * @description 查询当前连接的本地地址 
     */
    readonly localAddress: string;

    /**
     * @description 查询当前连接的本地端口 
     */
    readonly localPort: number;

}

