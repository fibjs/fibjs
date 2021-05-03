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
     * @description 处理一个消息或对象
     *      @param v 指定处理的消息或对象
     *      @return 返回下一步的处理器
     *      
     */
    invoke(): Class_Handler;

}

