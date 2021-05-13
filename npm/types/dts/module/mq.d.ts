/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/// <reference path="../interface/HttpHandler.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Chain.d.ts" />
/// <reference path="../interface/Routing.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 消息队列模块 
 */
declare module 'mq' {
    /**
     * @description 创建一个消息对象，参见 Message 
     */
    const Message: typeof Class_Message;

    /**
     * @description 创建一个 http 协议处理器对象，参见 HttpHandler 
     */
    const HttpHandler: typeof Class_HttpHandler;

    /**
     * @description 创建一个消息处理器对象，传递值内置处理器则直接返回
     * 
     *      hdlr 接受内置消息处理器，处理函数，链式处理数组，路由对象：
     *      - Function javascript 函数，将使用此函数进行处理
     *      - Handler 内置处理器，将使用此处理器进行处理
     *      - 链式处理数组，等同于返回 new mq.Chain(hdlr)，参见 Chain
     *      - 路由对象，等同于返回 new mq.Routing(hdlr)，参见 Routing
     * 
     *      消息处理函数语法如下：
     *      ```JavaScript
     *      function func(v){
     *      }
     *      ```
     *      参数 v 为正在处理的消息，返回结果允许有四种:
     *      - Function javascript 函数，将使用此函数进行下一阶段处理
     *      - Handler 内置处理器，将使用此处理器进行下一阶段处理
     *      - 链式处理数组，等同于 new mq.Chain(v)，参见 Chain
     *      - 路由对象，等同于 new mq.Routing(v)，参见 Routing
     * 
     *      无返回或者其他的返回结果将结束消息处理。
     *      @param hdlr 内置消息处理器，处理函数，链式处理数组，路由对象
     *      @return 返回封装了处理函数的处理器
     *      
     */
    const Handler: typeof Class_Handler;

    /**
     * @description 创建一个消息处理器链处理对象，参见 Chain 
     */
    const Chain: typeof Class_Chain;

    /**
     * @description 创建一个消息处理器路由对象，参见 Routing
     */
    const Routing: typeof Class_Routing;

    /**
     * @description 创建一个空处理器对象，次处理对象不做任何处理直接返回
     *      @return 返回空处理函数
     *      
     */
    function nullHandler(): Class_Handler;

    /**
     * @description 使用给定的处理器处理一个消息或对象
     * 
     *     不同于处理器的 invoke 方法，此方法将循环调用每个处理器的返回处理器，直到处理器返回 null 为止。
     *      @param hdlr 指定使用的处理器
     *      @param v 指定要处理的消息或对象
     *      
     */
    function invoke(hdlr: Class_Handler, v: Class_object): void;

    function invoke(hdlr: Class_Handler, v: Class_object, callback: (err: Error | undefined | null)=>any): void;

}

