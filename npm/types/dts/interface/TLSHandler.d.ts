/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/**
 * @description tls/ssl 协议转换处理器
 * 
 *  用以将数据流转换为 tls/ssl 流协议。TLSHandler 是对 TLSSocket 的封装，用于构建服务器，逻辑上相当于：
 *  ```JavaScript
 *  
 *  function(s){
 *     var s1 = new tls.TLSSocket(ctx);
 *     s1.accept(s);
 *     hdlr.invoke(s1);
 *     s1.close();
 *  }
 *  ```
 *  
 */
declare class Class_TLSHandler extends Class_Handler {
    /**
     * @description 创建一个新的 TLSHandler 对象
     *     @param context 指定创建 TLSHandler 使用的安全上下文
     *     @param handler 事件处理接口对象
     *     
     */
    constructor(context: Class_SecureContext, handler: Class_Handler);

    /**
     * @description 创建一个新的 TLSHandler 对象
     *     @param options 使用 tls.createSecureContext 创建安全上下文需要的选项
     *     @param handler 事件处理接口对象
     *     
     */
    constructor(options: FIBJS.GeneralObject, handler: Class_Handler);

    /**
     * @description 查询当前 TLSHandler 使用的 SecureContext 
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description ssl 协议转换处理器当前事件处理接口对象 
     */
    handler: Class_Handler;

}

