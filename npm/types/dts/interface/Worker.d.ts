/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description 独立线程工作对象
 *  
 */
declare class Class_Worker extends Class_EventEmitter {
    /**
     * @description Worker 对象构造函数
     *      @param path 指定 Worker 入口脚本，只接受绝对路径
     *      @param opts 构造选项，暂未支持
     *      
     */
    constructor(path: string, opts?: FIBJS.GeneralObject);

    /**
     * @description 向 Master 或 Worker 发送消息，
     *      @param data 指定发送的消息内容
     *      
     */
    postMessage(data: any): void;

    /**
     * @description 查询和绑定接受 load 消息事件，相当于 on("load", func); 
     */
    onload: (...args: any[])=>any;

    /**
     * @description 查询和绑定接受 postMessage 消息事件，相当于 on("message", func); 
     */
    onmessage: (...args: any[])=>any;

    /**
     * @description 查询和绑定接受 error 消息事件，相当于 on("error", func); 
     */
    onerror: (...args: any[])=>any;

}

