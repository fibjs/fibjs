/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 消息处理器接口 
 */
declare class Class_Handler extends Class_object {
    /**
     * @description 构造一个消息处理器链处理对象
     *      @param hdlrs 处理器数组
     *      
     */
    constructor(hdlrs: any[]);

    /**
     * @description 创建一个消息处理器路由对象
     *      @param map 初始化路由参数
     *      
     */
    constructor(map: FIBJS.GeneralObject);

    /**
     * @description 创建一个 JavaSvript 消息处理器
     *      @param hdlr JavaScript 处理器函数
     *      
     */
    constructor(hdlr: (...args: any[])=>any);

    /**
     * @description 构造一个 fileHandler 或者 HttpRepeater
     *      @param hdlr 处理器的地址参数
     *      
     */
    constructor(hdlr: string);

    /**
     * @description 处理一个消息或对象
     *      @param v 指定处理的消息或对象
     *      @return 返回下一步的处理器
     *      
     */
    invoke(v: Class_object): Class_Handler;

    invoke(v: Class_object, callback: (err: Error | undefined | null, retVal: Class_Handler)=>any): void;

}

