/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/**
 * @description WorkerMessage 是用于 Worker 线程通信的消息对象。通过 postMessage() 发送消息时，接收方的 message 事件会将本对象作为事件参数传入。data 属性包含传递的值。 
 */
declare class Class_WorkerMessage extends Class_Message {
    /**
     * @description 查询消息携带的数据，即传递给 postMessage() 的值。 
     */
    readonly data: any;

}

