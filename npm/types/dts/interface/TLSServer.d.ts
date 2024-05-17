/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description tls 服务器对象，可方便创建一个标准多纤程 tls/ssl 服务器
 * 
 * TLSServer 对象是将 TcpServer 和 TLSHandler 组合封装的对象，方便快速搭建服务器，逻辑上相当于：
 *  ```JavaScript
 *  var svr = new net.TLSServer(addr, port, new tls.Handler(ctx, function(conn){
 *     ...
 *  }));
 *  ```
 * 
 *  创建方法：
 *  ```JavaScript
 *  var tls = require("tls");
 *  var svr = new tls.Server(ctx, function(conn){
 *      ...
 *  });
 *  ```
 *  
 */
declare class Class_TLSServer extends Class_TcpServer {
    /**
     * @description 创建一个新的 TLSServer 对象
     *      @param context 指定创建 TLSServer 使用的安全上下文
     *      @param port 指定监听的端口
     *      @param listener 事件处理接口对象
     *      
     */
    constructor(context: Class_SecureContext, port: number, listener: Class_Handler);

    /**
     * @description 创建一个新的 TLSServer 对象
     *      @param context 指定创建 TLSServer 使用的安全上下文
     *      @param addr 指定监听的地址
     *      @param port 指定监听的端口
     *      @param listener 事件处理接口对象
     *      
     */
    constructor(context: Class_SecureContext, addr: string, port: number, listener: Class_Handler);

    /**
     * @description 创建一个新的 TLSServer 对象
     * 
     *      options 除用于创建 SecureContext 的属性之外，还需提供以下属性：
     *      - address: 指定监听的地址，可选，默认在所有地址监听
     *      - port: 指定监听的端口，必须提供
     * 
     *      @param options 使用 tls.createSecureContext 创建安全上下文需要的选项
     *      @param listener 事件处理接口对象
     *      
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler);

    /**
     * @description 查询当前 TLSServer 使用的 SecureContext 
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description 设置当前 TLSServer 使用的 SecureContext
     *     @param context 指定新的 SecureContext
     *     
     */
    setSecureContext(context: Class_SecureContext): void;

    /**
     * @description 设置当前 TLSServer 使用的 SecureContext
     *     @param options 使用 tls.createSecureContext 创建安全上下文需要的选项
     *     
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}

