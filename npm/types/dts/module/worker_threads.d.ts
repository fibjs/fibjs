/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/// <reference path="../interface/MessagePort.d.ts" />
/// <reference path="../interface/MessageChannel.d.ts" />
/**
 * @description worker 基础模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var worker_threads = require('worker_threads');
 *  ```
 *  
 */
declare module 'worker_threads' {
    /**
     * @description 独立线程工作对象，参见 Worker 
     */
    const Worker: typeof Class_Worker;

    /**
     * @description 消息通道的一端，参见 MessagePort 
     */
    const MessagePort: typeof Class_MessagePort;

    /**
     * @description 一对相连的 MessagePort 对象，参见 MessageChannel 
     */
    const MessageChannel: typeof Class_MessageChannel;

    /**
     * @description 查询当前 Worker 是不是主线程 
     */
    const isMainThread: boolean;

    /**
     * @description 查询当前执行上下文的逻辑 worker 标识 
     */
    const threadId: number;

    /**
     * @description 查询当前 Worker 的父线程 
     */
    const parentPort: Class_MessagePort;

    /**
     * @description 查询父线程通过 Worker 构造函数传给该线程的的数据的克隆 
     */
    const workerData: any;

    /**
     * @description 同步接收 MessagePort 上排队的下一条消息 
     */
    function receiveMessageOnPort(port: Class_MessagePort): any;

}

