/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description HttpsServer 是用于创建 https 服务器的对象，HttpsServer 对象可以使用 HttpServer 所有的接口函数和属性。HttpsServer 对象可以通过在创建时传入之前使用 openssl 生成的证书对象 (X509Cert 类型) 和密钥对象 (PKey 类型)，从而为客户端提供 tls/ssl 加密保护的服务
 * 
 *  https 服务器对象是将 TLSServer 和 HttpHandler 组合封装的对象，方便快速搭建服务器，逻辑上相当于：
 *  ```JavaScript
 *  var svr = new tls.Server({
 *         crt,
 *         key
 *     }, addr, port, new http.Handler(function(req){
 *     ...
 *  }));
 *  ```
 * 
 * 下面是一个使用 HttpsServer 的示例代码：
 * ```JavaScript
 * const http = require("http");
 * 
 * // create https server
 * const server = new http.HttpsServer({
 *         cert,
 *         key
 *     }, 8443, function(req) {
 *     resp.response.write(`Hello, Fibjs!`);
 * });
 * server.start();
 * ```
 * 
 * 在上面的例子中，我们加载了一个名为 "server.crt" 和 "server.key" 的证书和私钥文件，然后使用 HttpsServer 对象创建了一个服务，并开启了监听 8443 端口的服务，当客户端通过"https://localhost:8443/" 访问服务时，就可以受到 ssl 加密保护。
 * 
 * 需要注意的是，如果是需要让外部访问的话，需要确保证书是公信机构颁发的，否则客户端无法验证，降低了性能和安全，并可能触发安全警告。
 *  
 */
declare class Class_HttpsServer extends Class_HttpServer {
    /**
     * @description HttpsServer 构造函数，在所有本机地址侦听
     *     @param context SecureContext 安全上下文
     *     @param port 指定 http 服务器侦听端口
     *     @param hdlr http 内置消息处理器，处理函数，链式处理数组，路由对象
     *     
     */
    constructor(context: Class_SecureContext, port: number, hdlr: Class_Handler);

    /**
     * @description HttpsServer 构造函数
     *      @param context SecureContext 安全上下文
     *      @param addr 指定 http 服务器侦听地址，为 "" 则在本机所有地址侦听
     *      @param port 指定 http 服务器侦听端口
     *      @param hdlr http 内置消息处理器，处理函数，链式处理数组，路由对象
     *     
     */
    constructor(context: Class_SecureContext, addr: string, port: number, hdlr: Class_Handler);

    /**
     * @description HttpsServer 构造函数，在所有本机地址侦听
     * 
     *      options 除用于创建 SecureContext 的属性之外，还需提供以下属性：
     *      - address: 指定监听的地址，可选，默认在所有地址监听
     *      - port: 指定监听的端口，必须提供
     * 
     *      @param options 使用 tls.createSecureContext 创建安全上下文需要的选项
     *      @param hdlr http 内置消息处理器，处理函数，链式处理数组，路由对象
     *     
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler);

    /**
     * @description 查询当前 HttpsServer 使用的 SecureContext 
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description 设置当前 HttpsServer 使用的 SecureContext
     *     @param context 指定新的 SecureContext
     *     
     */
    setSecureContext(context: Class_SecureContext): void;

    /**
     * @description 设置当前 HttpsServer 使用的 SecureContext
     *     @param options 使用 tls.createSecureContext 创建安全上下文需要的选项
     *     
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}

