/// <reference path="../interface/Handler.d.ts" />
/**
 * @description 消息处理器链处理对象
 * 
 *  消息处理器链处理对象用于链接一系列消息处理器，按照指定的顺序链式处理。创建方法：
 *  ```JavaScript
 *  var chain = new mq.Chain([
 *    func1, func2
 *  ]);
 *  ```
 *  
 */
declare class Class_Chain extends Class_Handler {
    /**
     * @description 构造一个消息处理器链处理对象
     *      @param hdlrs 处理器数组
     *      
     */
    constructor(hdlrs: any[]);

    /**
     * @description 添加处理器数组
     *      @param hdlrs 处理器数组
     *      
     */
    append(hdlrs: any[]): void;

}

