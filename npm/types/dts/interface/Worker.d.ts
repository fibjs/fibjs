/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description The Worker object is used to create child threads, allowing child threads to be created and handled in a program. A Worker object can be understood as a JavaScript process running in a thread different from the main thread. A Worker does not share memory with the main thread and does not block the main thread; it is a mainstream way of asynchronous programming
 *
 * The constructor of the Worker object is as follows:
 *
 * ```JavaScript
 * new Worker(String path, Object opts = {})
 * ```
 *
 * Here, the path parameter specifies the JavaScript file path of the new thread. For example, you can write a work.js file with the following content:
 *
 * ```JavaScript
 * const { Worker } = require('worker_threads');
 * console.log('Hi from worker');
 * ```
 *
 * In the main program, run work.js with the following code:
 *
 * ```JavaScript
 * const { Worker } = require('worker_threads');
 * const worker = new Worker('path/to/work.js');
 * ```
 *
 * After running it, you can see the output "Hi from worker" in the console of the main program.
 *
 * In the following example, suppose we have a long-running computation and want to put it into another thread for processing, while avoiding being blocked by this computation in the main thread. The code is as follows:
 *
 *
 * Main thread:
 * ```JavaScript
 * const { Worker } = require('worker_threads');
 *
 * // create a worker thread
 * const fib = new Worker(__dirname + '/fib-worker.js');
 * // Receive result from worker thread
 * fib.on('message', (result) => {
 *   console.log('result: ', result);
 * });
 * fib.on('error', (err) => {
 *   console.error(err);
 * });
 * fib.postMessage(40);
 * console.log('main thread still working');
 * ```
 *
 * In this example, we create a worker thread through the Worker object constructor to handle the computation of a Fibonacci sequence; the main thread passes data to the worker thread through the postMessage() method and obtains the processing result through the message event. At the same time, the main thread displays the 'still working' message, proving that this computation task has been 'delegated' to the worker thread and that it can continue to handle other things.
 *
 * The worker thread code looks like this:
 *
 * ```JavaScript
 * // fib-worker.js
 * const { parentPort } = require('worker_threads');
 *
 * parentPort.on('message', (n) => {
 *   const result = fib(n);
 *   // After calculation, result is sent back to main thread.
 *   parentPort.postMessage(result);
 * });
 * function fib(n) {
 *   if (n <= 1) return n;
 *   return fib(n - 1) + fib(n - 2);
 * }
 * ```
 *
 * In the worker thread, we listen for messages sent by the main thread through parentPort.on('message'), compute the specified Fibonacci sequence, and send the result back to the main thread through parentPort.postMessage().
 *
 * This is the most basic Worker example. When developing with Worker objects, the main thread and the worker thread are completely asynchronous; each Worker object is a separate thread, and a Worker object instantiated in the main thread does not cause any blocking.
 *
 */
declare class Class_Worker extends Class_EventEmitter {
    /**
     * @description Worker object constructor
     *    @param path the Worker entry script; accepts an absolute path, a relative path starting with ./ or ../, or the source code directly when opts.eval = true
     *    @param opts construction options, supports eval and workerData
     *
     */
    constructor(path: string, opts?: FIBJS.GeneralObject);

    /**
     * @description Queries the logical worker id of the target worker
     */
    readonly threadId: number;

    /**
     * @description Sends a message to the peer thread
     *      @param data the message content to send
     *
     */
    postMessage(data: any): void;

    /**
     * @description Sends a message to the peer thread and transfers the specified objects
     *      @param data the message content to send
     *      @param transfer the array of objects to transfer (ArrayBuffer, etc.); after transfer, the original objects can no longer be used by the sender
     *
     */
    postMessage(data: any, transfer: any[]): void;

    /**
     * @description Terminates the worker
     *
     *    Consistent with Node.js, returns a Promise that resolves with the exit code when the worker exits (the `exit` event).
     *    After being called, the JavaScript in the worker stops executing as soon as possible.
     *
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker(__dirname + '/fib-worker.js');
     *    const exitCode = await worker.terminate();
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminate(): Promise<number>;

    /**
     * @description Terminates the worker
     *
     *    Consistent with Node.js, returns a Promise that resolves with the exit code when the worker exits (the `exit` event).
     *    After being called, the JavaScript in the worker stops executing as soon as possible.
     *
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker(__dirname + '/fib-worker.js');
     *    const exitCode = await worker.terminate();
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminateSync(): number;

    /**
     * @description Terminates the worker
     *
     *    Consistent with Node.js, returns a Promise that resolves with the exit code when the worker exits (the `exit` event).
     *    After being called, the JavaScript in the worker stops executing as soon as possible.
     *
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker(__dirname + '/fib-worker.js');
     *    const exitCode = await worker.terminate();
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminateAsync(): Promise<number>;

    /**
     * @description Keeps the fibjs process from exiting
     *
     */
    ref(): void;

    /**
     * @description Allows the fibjs process to exit
     *
     */
    unref(): void;

    /**
     * @description Queries and binds the worker ready event, equivalent to on("online", func);
     */
    on(event: "online", listener: ()=>void): this;

    once(event: "online", listener: ()=>void): this;

    off(event: "online", listener: ()=>void): this;

    addListener(event: "online", listener: ()=>void): this;

    removeListener(event: "online", listener: ()=>void): this;

    addEventListener(event: "online", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "online", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "online", listener: ()=>void): this;

    prependOnceListener(event: "online", listener: ()=>void): this;

    /**
     * @description Queries and binds the worker ready event, equivalent to on("online", func);
     */
    ononline: (()=>void) | null;

    /**
     * @description Queries and binds the postMessage message event, equivalent to on("message", func);
     */
    on(event: "message", listener: ()=>void): this;

    once(event: "message", listener: ()=>void): this;

    off(event: "message", listener: ()=>void): this;

    addListener(event: "message", listener: ()=>void): this;

    removeListener(event: "message", listener: ()=>void): this;

    addEventListener(event: "message", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: ()=>void): this;

    prependOnceListener(event: "message", listener: ()=>void): this;

    /**
     * @description Queries and binds the postMessage message event, equivalent to on("message", func);
     */
    onmessage: (()=>void) | null;

    /**
     * @description Queries and binds the error message event, equivalent to on("error", func);
     */
    on(event: "error", listener: ()=>void): this;

    once(event: "error", listener: ()=>void): this;

    off(event: "error", listener: ()=>void): this;

    addListener(event: "error", listener: ()=>void): this;

    removeListener(event: "error", listener: ()=>void): this;

    addEventListener(event: "error", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: ()=>void): this;

    prependOnceListener(event: "error", listener: ()=>void): this;

    /**
     * @description Queries and binds the error message event, equivalent to on("error", func);
     */
    onerror: (()=>void) | null;

    /**
     * @description Queries and binds the worker exit event, equivalent to on("exit", func);
     */
    on(event: "exit", listener: ()=>void): this;

    once(event: "exit", listener: ()=>void): this;

    off(event: "exit", listener: ()=>void): this;

    addListener(event: "exit", listener: ()=>void): this;

    removeListener(event: "exit", listener: ()=>void): this;

    addEventListener(event: "exit", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "exit", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "exit", listener: ()=>void): this;

    prependOnceListener(event: "exit", listener: ()=>void): this;

    /**
     * @description Queries and binds the worker exit event, equivalent to on("exit", func);
     */
    onexit: (()=>void) | null;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * The promise variant of the Worker class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_WorkerPromise extends Class_EventEmitter {
    /**
     * @description Worker object constructor
     *    @param path the Worker entry script; accepts an absolute path, a relative path starting with ./ or ../, or the source code directly when opts.eval = true
     *    @param opts construction options, supports eval and workerData
     *
     */
    constructor(path: string, opts?: FIBJS.GeneralObject);

    /**
     * @description Queries the logical worker id of the target worker
     */
    readonly threadId: number;

    /**
     * @description Sends a message to the peer thread
     *      @param data the message content to send
     *
     */
    postMessage(data: any): void;

    /**
     * @description Sends a message to the peer thread and transfers the specified objects
     *      @param data the message content to send
     *      @param transfer the array of objects to transfer (ArrayBuffer, etc.); after transfer, the original objects can no longer be used by the sender
     *
     */
    postMessage(data: any, transfer: any[]): void;

    /**
     * @description Terminates the worker
     *
     *    Consistent with Node.js, returns a Promise that resolves with the exit code when the worker exits (the `exit` event).
     *    After being called, the JavaScript in the worker stops executing as soon as possible.
     *
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker(__dirname + '/fib-worker.js');
     *    const exitCode = await worker.terminate();
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminate(): Promise<number>;

    /**
     * @description Terminates the worker
     *
     *    Consistent with Node.js, returns a Promise that resolves with the exit code when the worker exits (the `exit` event).
     *    After being called, the JavaScript in the worker stops executing as soon as possible.
     *
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker(__dirname + '/fib-worker.js');
     *    const exitCode = await worker.terminate();
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminateSync(): number;

    /**
     * @description Terminates the worker
     *
     *    Consistent with Node.js, returns a Promise that resolves with the exit code when the worker exits (the `exit` event).
     *    After being called, the JavaScript in the worker stops executing as soon as possible.
     *
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker(__dirname + '/fib-worker.js');
     *    const exitCode = await worker.terminate();
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminateAsync(): Promise<number>;

    /**
     * @description Keeps the fibjs process from exiting
     *
     */
    ref(): void;

    /**
     * @description Allows the fibjs process to exit
     *
     */
    unref(): void;

    /**
     * @description Queries and binds the worker ready event, equivalent to on("online", func);
     */
    ononline: (()=>void) | null;

    /**
     * @description Queries and binds the postMessage message event, equivalent to on("message", func);
     */
    onmessage: (()=>void) | null;

    /**
     * @description Queries and binds the error message event, equivalent to on("error", func);
     */
    onerror: (()=>void) | null;

    /**
     * @description Queries and binds the worker exit event, equivalent to on("exit", func);
     */
    onexit: (()=>void) | null;

}


declare namespace Class_Worker {
    const promises: FIBJS.GeneralObject;
}
