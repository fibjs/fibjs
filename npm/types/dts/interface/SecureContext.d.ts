/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description tls 安全上下文对象，用于多个 tls 连接共享基础配置
 * 
 *   SecureContext 对象是一个安全上下文对象，用于多个 tls 连接共享基础配置。SecureContext 对象可以通过 tls.createSecureContext 方法创建。
 *   ```JavaScript
 *     const tls = require('tls');
 *     const fs = require('fs');
 * 
 *     const options = {
 *       key: fs.readFileSync('server-key.pem'),
 *       cert: fs.readFileSync('server-cert.pem')
 *     };
 * 
 *     const context = tls.createSecureContext(options);
 *   ```
 *  
 */
declare class Class_SecureContext extends Class_object {
    /**
     * @description 查询安全上下文的受信任的 CA 证书 
     */
    readonly ca: Class_X509Certificate;

    /**
     * @description 查询安全上下文连接的私钥 
     */
    readonly key: Class_KeyObject;

    /**
     * @description 查询安全上下文连接的证书 
     */
    readonly cert: Class_X509Certificate;

    /**
     * @description 查询安全上下文允许的最大 TLS 版本 
     */
    readonly maxVersion: string;

    /**
     * @description 查询安全上下文允许的最小 TLS 版本 
     */
    readonly minVersion: string;

    /**
     * @description 查询安全上下文使用的 TLS 协议版本
     */
    readonly secureProtocol: string;

    /**
     * @description 查询安全上下文是否需要客户端证书 
     */
    readonly requestCert: boolean;

    /**
     * @description 查询安全上下文是否拒绝任何未通过 CA 列表验证证书的连接 
     */
    readonly rejectUnverified: boolean;

    /**
     * @description 查询安全上下文是否拒绝任何未提供 CA 列表授权证书的连接 
     */
    readonly rejectUnauthorized: boolean;

    /**
     * @description 查询安全上下文回话超时 
     */
    readonly sessionTimeout: number;

    /**
     * @description 设置 SNI 上下文
     *     @param servername 指定的服务器名
     *     @param context 指定的安全上下文
     *     
     */
    setSNIContext(servername: string, context: Class_SecureContext): void;

    /**
     * @description 设置 SNI 上下文
     *     @param servername 指定的服务器名
     *     @param options 使用 tls.createSecureContext 创建安全上下文需要的选项
     *     
     */
    setSNIContext(servername: string, options: FIBJS.GeneralObject): void;

    /**
     * @description 查询 SNI 上下文
     *     @param servername 指定的服务器名
     *     @param auto_resolve 是否自动创建上下文
     *     @return 返回指定的安全上下文
     *     
     */
    getSNIContext(servername: string, auto_resolve?: boolean): Class_SecureContext;

    getSNIContext(servername: string, auto_resolve?: boolean, callback?: (err: Error | undefined | null, retVal: Class_SecureContext)=>any): void;

    /**
     * @description 删除 SNI 上下文
     *     @param servername 指定的服务器名
     *     
     */
    removeSNIContext(servername: string): void;

    /**
     * @description 清除所有 SNI 上下文 
     */
    clearSNIContexts(): void;

}

