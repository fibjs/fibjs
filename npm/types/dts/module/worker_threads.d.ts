/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/// <reference path="../interface/MessagePort.d.ts" />
/// <reference path="../interface/MessageChannel.d.ts" />
/**
 * @description worker 基础模块，提供线程间通信能力
 *
 *  模块的主要能力：
 *
 *  - **线程对象**：`Worker` 独立线程工作对象；
 *  - **消息通信**：`MessagePort`、`MessageChannel` 消息通道，`receiveMessageOnPort` 同步接收消息；
 *  - **线程信息**：`isMainThread`、`threadId`、`parentPort`、`workerData`；
 *  - **兼容 API**：`markAsUncloneable`、`markAsUntransferable`、`isMarkedAsUntransferable`（提供既有生态兼容）。
 *
 *  引用方法：
 *  ```JavaScript
 *  var worker_threads = require('worker_threads');
 *  ```
 *
 *  主线程中 `parentPort` 与 `workerData` 为 null；Worker 线程内通过 `parentPort` 与主线程通信。
 *
 *  线程模型与 Addon 兼容性：
 *
 *  fibjs 缺省把一个 isolate 的全部 JS 固定在一条专属 OS 线程上执行（`--no-js-thread-affinity` 可关闭），
 *  以便与 Node.js 的「一个 napi_env 对应一条 OS 线程」契约一致，让把类构造器或运行时句柄缓存在线程本地存储
 *  （TLS）里的 N-API 扩展可以正常工作。
 *
 *  - 缺省：一个 isolate 的 JS 始终在同一条 OS 线程上执行；
 *  - `--no-js-thread-affinity`：JS fiber 回到共享线程池调度，CPU 密集场景并行度更高，但依赖 TLS 的扩展可能随机失败；
 *  - 已知限制：扩展在来自 JS 的回调里对已满的 threadsafe function 使用 `napi_tsfn_blocking` 会阻塞这条唯一的 JS 线程，
 *    而排空队列同样需要它，于是自锁（Node.js 的单线程事件循环有同样的限制），应改用 nonblocking。
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
     *
     *      端口上没有排队消息时返回 undefined；有消息时返回包含 `message` 字段的对象。
     *      @param port 指定接收消息的 MessagePort 对象
     *      @return 返回接收到的消息对象，端口为空时返回 undefined
     *
     */
    function receiveMessageOnPort(port: Class_MessagePort): any;

    /**
     * @description 将对象标记为不可克隆。如果对象被用作 port.postMessage() 调用的消息，
     *         会抛出错误。对于原始值，此操作为无操作。
     *
     *         注意：fibjs 使用 V8 的 ValueSerializer 进行 postMessage 序列化，
     *         不会检查 transfer mode 私有符号。该标记对 fibjs 的序列化
     *         行为没有影响，但提供此 API 是为了兼容依赖它的包（如 undici）在
     *         Web API 构造函数中调用。
     *      @param object 指定要标记的对象
     *
     */
    function markAsUncloneable(object: any): void;

    /**
     * @description 将对象标记为不可传输。如果对象出现在 port.postMessage() 调用的
     *         传输列表中，将被忽略。
     *
     *         注意：在 fibjs 中为无操作，为兼容既有 API 调用而提供。
     *      @param object 指定要标记的对象
     *
     */
    function markAsUntransferable(object: any): void;

    /**
     * @description 检查对象是否被标记为不可传输。
     *
     *         注意：在 fibjs 中始终返回 false，为兼容既有 API 调用而提供。
     *      @param object 指定要检查的对象
     *      @return 返回对象是否被标记为不可传输，fibjs 中始终为 false
     *
     */
    function isMarkedAsUntransferable(object: any): boolean;

}

