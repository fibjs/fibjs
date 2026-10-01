/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/// <reference path="../interface/MessagePort.d.ts" />
/// <reference path="../interface/MessageChannel.d.ts" />
/**
 * @description worker basic module, providing inter-thread communication capabilities
 *
 *  Main capabilities of the module:
 *
 *  - **Thread object**: `Worker`, an independent thread worker object;
 *  - **Message communication**: `MessagePort` and `MessageChannel` message channels, and synchronous message receiving through `receiveMessageOnPort`;
 *  - **Thread information**: `isMainThread`, `threadId`, `parentPort`, `workerData`;
 *  - **Compatibility APIs**: `markAsUncloneable`, `markAsUntransferable`, `isMarkedAsUntransferable` (provided for compatibility with the existing ecosystem).
 *
 *  Import method:
 *  ```JavaScript
 *  var worker_threads = require('worker_threads');
 *  ```
 *
 *  In the main thread, `parentPort` and `workerData` are null; inside a Worker thread, the thread communicates with the main thread through `parentPort`.
 *
 *  Thread model and Addon compatibility:
 *
 *  By default fibjs pins all JS of an isolate to execute on one dedicated OS thread (`--no-js-thread-affinity` disables this),
 *  consistent with the Node.js contract that "one napi_env corresponds to one OS thread", so that N-API extensions which cache class constructors or runtime handles in thread-local storage
 *  (TLS) can work correctly.
 *
 *  - Default: the JS of an isolate always executes on the same OS thread;
 *  - `--no-js-thread-affinity`: JS fibers go back to being scheduled on the shared thread pool, giving higher parallelism in CPU-intensive scenarios, but extensions that depend on TLS may fail randomly;
 *  - Known limitation: an extension using `napi_tsfn_blocking` on a full threadsafe function inside a callback from JS blocks this single JS thread,
 *    and draining the queue also needs it, leading to self-deadlock (the single-threaded event loop of Node.js has the same limitation); use nonblocking instead.
 *
 */
declare module 'worker_threads' {
    /**
     * @description Independent thread worker object, see Worker
     */
    const Worker: typeof Class_Worker;

    /**
     * @description One end of a message channel, see MessagePort
     */
    const MessagePort: typeof Class_MessagePort;

    /**
     * @description A pair of connected MessagePort objects, see MessageChannel
     */
    const MessageChannel: typeof Class_MessageChannel;

    /**
     * @description Queries whether the current Worker is the main thread
     */
    const isMainThread: boolean;

    /**
     * @description Queries the logical worker identifier of the current execution context
     */
    const threadId: number;

    /**
     * @description Queries the parent thread of the current Worker
     */
    const parentPort: Class_MessagePort;

    /**
     * @description Queries the clone of the data passed to this thread by the parent thread through the Worker constructor
     */
    const workerData: any;

    /**
     * @description Synchronously receives the next queued message on a MessagePort
     *
     *      Returns undefined when there is no queued message on the port; otherwise returns an object containing a `message` field.
     *      @param port the MessagePort object to receive messages from
     *      @return returns the received message object, or undefined when the port is empty
     *
     */
    function receiveMessageOnPort(port: Class_MessagePort): any;

    /**
     * @description Marks an object as uncloneable. If the object is used as the message of a port.postMessage() call,
     *         an error is thrown. For primitive values, this operation is a no-op.
     *
     *         Notes: fibjs uses V8's ValueSerializer for postMessage serialization,
     *         and does not check the transfer mode private symbol. This marking has no effect on the serialization
     *         behavior of fibjs, but the API is provided for compatibility with packages that rely on it (such as undici)
     *         calling it in Web API constructors.
     *      @param object the object to mark
     *
     */
    function markAsUncloneable(object: any): void;

    /**
     * @description Marks an object as untransferable. If the object appears in the transfer
     *         list of a port.postMessage() call, it is ignored.
     *
     *         Notes: this is a no-op in fibjs, provided for compatibility with existing API calls.
     *      @param object the object to mark
     *
     */
    function markAsUntransferable(object: any): void;

    /**
     * @description Checks whether an object is marked as untransferable.
     *
     *         Notes: always returns false in fibjs, provided for compatibility with existing API calls.
     *      @param object the object to check
     *      @return returns whether the object is marked as untransferable; always false in fibjs
     *
     */
    function isMarkedAsUntransferable(object: any): boolean;

}

