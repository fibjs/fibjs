/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SslSocket.d.ts" />
/// <reference path="../interface/SslHandler.d.ts" />
/// <reference path="../interface/SslServer.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/X509Cert.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/**
 * @description ssl/tls 模块，模块别名：tls 
 */
declare module 'ssl' {
    /**
     * @description 证书验证模式，不验证 
     */
    export const VERIFY_NONE: 0;

    /**
     * @description 证书验证模式，可选验证，允许验证不通过 
     */
    export const VERIFY_OPTIONAL: 1;

    /**
     * @description 证书验证模式，要求验证，验证不通过则中断 
     */
    export const VERIFY_REQUIRED: 2;

    /**
     * @description 证书验证结果，证书超时 
     */
    export const BADCERT_EXPIRED: 1;

    /**
     * @description 证书验证结果，证书被撤销 
     */
    export const BADCERT_REVOKED: 2;

    /**
     * @description 证书验证结果，证书名错误 
     */
    export const BADCERT_CN_MISMATCH: 4;

    /**
     * @description 证书验证结果，证书不可信 
     */
    export const BADCERT_NOT_TRUSTED: 8;

    /**
     * @description ssl 协议版本 ssl 3.0 
     */
    export const ssl3: 0;

    /**
     * @description ssl 协议版本 tls 1.0 
     */
    export const tls1: 1;

    /**
     * @description ssl 协议版本 tls 1.1 
     */
    export const tls1_1: 2;

    /**
     * @description ssl 协议版本 tls 1.2 
     */
    export const tls1_2: 3;

    /**
     * @description 创建一个 SslSocket 对象，参见 SslSocket 
     */
    const Socket: typeof Class_SslSocket;

    /**
     * @description 创建一个 SslHandler 对象，参见 SslHandler 
     */
    const Handler: typeof Class_SslHandler;

    /**
     * @description 创建一个 SslServer 对象，参见 SslServer 
     */
    const Server: typeof Class_SslServer;

    /**
     * @description 创建一个 SslSocket 对象并建立连接
     *     @param url 指定连接的协议，可以是：ssl://host:port
     *     @param timeout 指定超时时间，单位是毫秒，默认为0
     *     @return 返回连接成功的 SslSocket 对象
     *    
     */
    function connect(url: string, timeout?: number): Class_Stream;

    function connect(url: string, timeout?: number, callback?: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description 创建一个 SslSocket 对象并建立连接
     *     @param url 指定连接的协议，可以是：ssl://host:port
     *     @param crt 证书，用于发送给服务器验证客户端
     *     @param key 私钥，用于与客户端会话
     *     @param timeout 指定超时时间，单位是毫秒，默认为0
     *     @return 返回连接成功的 SslSocket 对象
     *    
     */
    function connect(url: string, crt: Class_X509Cert, key: Class_PKey, timeout?: number): Class_Stream;

    function connect(url: string, crt: Class_X509Cert, key: Class_PKey, timeout?: number, callback?: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description 设定缺省客户端证书
     *     @param crt 证书，用于发送给服务器验证客户端
     *     @param key 私钥，用于与客户端会话
     *    
     */
    function setClientCert(crt: Class_X509Cert, key: Class_PKey): void;

    /**
     * @description 从文件中加载缺省客户端证书
     *     @param crtFile 证书文件，用于客户端验证服务器
     *     @param keyFile 私钥文件，用于与客户端会话
     *     @param password 解密密码
     *    
     */
    function loadClientCertFile(crtFile: string, keyFile: string, password?: string): void;

    /**
     * @description 加载自带的缺省根证书，等同于 ssl.ca.loadRootCerts
     *     此证书内容源自：http://hg.mozilla.org/releases/mozilla-release/raw-file/default/security/nss/lib/ckfw/builtins/certdata.txt 
     */
    function loadRootCerts(): void;

    /**
     * @description 全局证书，用于 ssl 客户端模式验证服务器证书 
     */
    const ca: Class_X509Cert;

    /**
     * @description 设定证书验证模式，缺省为 VERIFY_REQUIRED 
     */
    var verification: number;

    /**
     * @description 设定最低版本支持，缺省 ssl3 
     */
    var min_version: number;

    /**
     * @description 设定最高版本支持，缺省 tls1_1 
     */
    var max_version: number;

}

