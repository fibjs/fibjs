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

    /**
     * @description 将对象标记为不可克隆。如果对象被用作 port.postMessage() 调用的消息，
     *         会抛出错误。对于原始值，此操作为无操作。
     * 
     *         注意：fibjs 使用 V8 的 ValueSerializer 进行 postMessage 序列化，
     *         不会检查 Node.js 的 transfer mode 私有符号。该标记对 fibjs 的序列化
     *         行为没有影响，但提供此 API 是为了兼容依赖它的包（如 undici）在
     *         Web API 构造函数中调用。 
     */
    function markAsUncloneable(object: any): void;

    /**
     * @description 将对象标记为不可传输。如果对象出现在 port.postMessage() 调用的
     *         传输列表中，将被忽略。
     * 
     *         注意：在 fibjs 中为无操作，提供用于 Node.js 兼容性。 
     */
    function markAsUntransferable(object: any): void;

    /**
     * @description 检查对象是否被标记为不可传输。
     * 
     *         注意：在 fibjs 中始终返回 false，提供用于 Node.js 兼容性。 
     */
    function isMarkedAsUntransferable(object: any): boolean;

}

