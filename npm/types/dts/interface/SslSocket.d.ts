/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/X509Cert.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/**
 * @description ssl 网络套接口对象
 * 
 *  SslSocket 属于 ssl 模块，创建方法
 *  ```JavaScript
 *  var s = new ssl.Socket();
 *  ```
 *  
 */
declare class Class_SslSocket extends Class_Stream {
    /**
     * @description SslSocket 构造函数，创建一个新的 SslSocket 对象
     * 
     *     certs 格式为：
     *     ```JavaScript
     *     [
     *         {
     *             name: "fibjs.org",
     *             crt: [X509Cert object],
     *             key: [PKey object]
     *         },
     *         {
     *             name: "*.fibjs.org",
     *             crt: [X509Cert object],
     *             key: [PKey object]
     *         }
     *     ]
     *     ```
     *     @param certs 服务器证书列表
     *    
     */
    constructor(certs?: any[]);

    /**
     * @description SslSocket 构造函数，创建一个新的 SslSocket 对象
     *     @param crt X509Cert 证书，用于客户端验证服务器
     *     @param key PKey 私钥，用于与客户端会话
     *    
     */
    constructor(crt: Class_X509Cert, key: Class_PKey);

    /**
     * @description 设定证书验证模式，缺省为 VERIFY_REQUIRED 
     */
    verification: number;

    /**
     * @description 证书链，客户端模式 connect 时自动引用 ssl.ca，服务器模式 accept 生成 SslSocket 自动引用当前 SslSocket 的 ca
     */
    readonly ca: Class_X509Cert;

    /**
     * @description 连接对方的证书 
     */
    readonly peerCert: Class_X509Cert;

    /**
     * @description 连接的主机名 
     */
    readonly hostname: string;

    /**
     * @description 查询消息 ssl 建立时的下层流对象 
     */
    readonly stream: Class_Stream;

    /**
     * @description 在给定的连接上连接 ssl 连接，客户端模式
     *     @param s 给定的底层连接
     *     @param server_name 指定服务器名称，可缺省
     *     @return 连接成功返回 0，证书可选验证时，验证不成功则返回非 0，详细错误见 ssl 模块
     *   
     */
    connect(s: Class_Stream, server_name?: string): number;

    connect(s: Class_Stream, server_name?: string, callback?: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 在给定的连接上接收一个 ssl 连接，并生成一个新的 SslSocket
     *     @param s 给定的底层连接
     *     @return 返回新建立的 SslSocket 对象
     *    
     */
    accept(s: Class_Stream): Class_SslSocket;

    accept(s: Class_Stream, callback: (err: Error | undefined | null, retVal: Class_SslSocket)=>any): void;

}

