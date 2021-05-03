/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/X509Cert.d.ts" />
/**
 * @description https 服务器对象
 * 
 *  https 服务器对象是将 SslServer 和 HttpHandler 组合封装的对象，方便快速搭建服务器，逻辑上相当于：
 *  ```JavaScript
 *  var svr = new net.SslServer(crt, key, addr, port, new http.Handler(function(req){
 *     ...
 *  }));
 *  ```
 * 
 *  创建方法：
 *  ```JavaScript
 *  var http = require("http");
 *  var svr = new http.HttpsServer(crt, key, 443, function(req){
 *      ...
 *  });
 *  ```
 *  
 */
declare class Class_HttpsServer extends Class_HttpServer {
    /**
     * @description HttpsServer 构造函数，在所有本机地址侦听
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
     *     @param port 指定 http 服务器侦听端口
     *     @param hdlr http 内置消息处理器，处理函数，链式处理数组，路由对象，详见
     *    
     */
    constructor(certs: any[], port: number, hdlr: Class_Handler);

    /**
     * @description 设定证书验证模式，缺省为 VERIFY_NONE 
     */
    verification: number;

    /**
     * @description 客户端证书验证 ca 
     */
    readonly ca: Class_X509Cert;

}

