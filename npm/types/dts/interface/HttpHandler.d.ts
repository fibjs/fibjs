/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description http 协议转换处理器
 * 
 *   用以将数据流转换为 http 协议消息，创建方式：
 *   ```JavaScript
 *   var hdlr = new mq.HttpHandler(...);
 *   ```
 *   或者：
 *   ```JavaScript
 *   var hdlr = new http.Handler(...);
 *   ```
 *  
 */
declare class Class_HttpHandler extends Class_Handler {
    /**
     * @description 创建一个 http 协议处理器对象，将流对象的数据转变为 http 消息对象
     *     @param hdlr 内置消息处理器，处理函数，链式处理数组，路由对象，详见 mq.Handler
     *     
     */
    constructor(hdlr: Class_Handler);

    /**
     * @description 允许跨域请求
     *      @param allowHeaders 指定接受的 http 头字段
     *      
     */
    enableCrossOrigin(allowHeaders?: string): void;

    /**
     * @description 查询和设置最大请求头个数，缺省为 128 
     */
    maxHeadersCount: number;

    /**
     * @description 查询和设置 body 最大尺寸，以 MB 为单位，缺省为 64 
     */
    maxBodySize: number;

    /**
     * @description 查询和设置服务器名称，缺省为：fibjs/0.x.0 
     */
    serverName: string;

    /**
     * @description http 协议转换处理器当前事件处理接口对象 
     */
    handler: Class_Handler;

}

