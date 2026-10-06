/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description Worker creates a JavaScript child thread and controls it; use it for CPU-bound work that would block the fiber scheduler of the main isolate
 *
 *  A Worker is a separate OS thread with its own V8 isolate: it shares no JavaScript objects with the
 *  creator and never blocks it, which makes it the heavyweight but truly parallel option next to the
 *  lightweight fibers of the coroutine module. Communication is message-based: the parent posts to the
 *  Worker object, the worker posts to `parentPort`, and both sides listen for message events.
 *
 *  Obtained from:
 *  - `new Worker(path, opts)` — starts a thread from a script path, a `file://` URL or inline source
 *    (`eval: true`); the class is also installed as the global `Worker`.
 *
 *  Concepts:
 *
 *  - **Script source**: without `eval`, `path` must be absolute, start with `./`/`../` (resolved
 *    against the current working directory) or be a `file://` URL; a bare relative path throws
 *    `TypeError` with code `ERR_WORKER_PATH`. With `eval: true`, `path` is the program text.
 *  - **Lifecycle and events**: `online` (isolate started), zero or more `message`, then `error` for an
 *    uncaught exception, and `exit` once with the exit code. `terminate()` stops the thread and
 *    resolves with that code.
 *  - **Messages**: `postMessage` structured-clones the value, so the receiver gets an independent
 *    copy. On the worker side `parentPort` delivers and accepts messages; the parent reads them with
 *    `worker.on('message')` (or the `onmessage` property) and receives the raw value, not a
 *    MessageEvent.
 *  - **Transferables**: the second argument of `postMessage` is a transfer list; fibjs detaches
 *    ArrayBuffer entries and ignores all other entries.
 *  - **Keep-alive**: a Worker refs the process by default; `unref()` lets the parent exit without
 *    waiting and `ref()` restores the default. Messages queue until the first `message` listener is
 *    attached.
 *  - **Exit codes**: 0 for a script that finished, N for `process.exit(N)` inside the worker, 1 for an
 *    uncaught exception or for terminate().
 *
 *  Example 1 — compute on a temporary worker script and read the result:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const { Worker } = require('worker_threads');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-worker-'));
 *  const script = path.join(dir, 'sum.js');
 *  fs.writeFile(script,
 *      'const { parentPort, workerData } = require("worker_threads");\n' +
 *      'parentPort.postMessage(workerData.reduce((a, b) => a + b, 0));\n');
 *
 *  const worker = new Worker(script, { workerData: [1, 2, 3, 4] });
 *  worker.on('message', (total) => console.log(total)); // 10
 *  worker.on('exit', (code) => {
 *      console.log('exit code', code); // exit code 0
 *      fs.rmSync(dir, { recursive: true, force: true });
 *  });
 *  ```
 *
 *  Example 2 — terminate a running worker and await its exit code:
 *  ```JavaScript
 *  const { Worker } = require('worker_threads');
 *
 *  const worker = new Worker('setInterval(() => {}, 1000);', { eval: true });
 *  worker.on('online', async () => {
 *      console.log('exit code', await worker.terminate()); // exit code 1
 *  });
 *  ```
 *
 *  Example 3 — observe an uncaught worker exception:
 *  ```JavaScript
 *  const { Worker } = require('worker_threads');
 *
 *  const worker = new Worker('throw new Error("worker failed");', { eval: true });
 *  worker.on('error', (err) => console.log('error:', err.message)); // error: worker failed
 *  worker.on('exit', (code) => console.log('exit code', code)); // exit code 1
 *  ```
 *
 *  Notes:
 *
 *  - Node.js members that fibjs does not provide: `worker.exitCode`, `worker.stdin`, `worker.stdout`
 *    and `worker.stderr`.
 *  - fibjs implements the constructor options `eval` and `workerData`, plus the fibjs-only
 *    `file_system` and `safe_buffer`; the remaining Node.js options (`argv`, `env`, `execArgv`,
 *    `resourceLimits`, `trackUnmanagedFds`, `signal`) are not read.
 *  - The `on<event>` properties (`ononline`, `onmessage`, `onerror`, `onexit`) are a fibjs
 *    convenience provided by EventEmitter; they carry the same payloads as `on('<event>')`.
 *
 */
declare class Class_Worker extends Class_EventEmitter {
    /**
     * @description Creates a worker and starts its thread
     *
     *    The `path` argument selects the source:
     *
     *    - a script path: absolute, starting with `./` or `../` (resolved against the current working
     *      directory), or a `file://` URL; a bare relative path such as `'worker.js'` throws `TypeError`
     *      with code `ERR_WORKER_PATH`, as in Node.js;
     *    - JavaScript source when `opts.eval` is true; the source runs with the filename
     *      `[worker eval].js`.
     *
     *    `opts` accepts the following options:
     *    ```JavaScript
     *    // fragment: constructor options
     *    ({
     *        "eval": false,        // true: path holds JavaScript source instead of a file name
     *        "workerData": null,   // value cloned into the worker; read as worker_threads.workerData
     *        "file_system": true,  // fibjs extension: false makes real-file access throw [20009]
     *        "safe_buffer": false  // fibjs extension: true restricts Buffer codecs to native ones
     *    })
     *    ```
     *
     *    `workerData` is cloned synchronously in the constructor; a value that cannot be cloned (for
     *    example a MessagePort or a SharedArrayBuffer) makes the constructor throw. Messages posted
     *    immediately after construction are queued until the worker starts.
     *      @param path the Worker entry script; accepts an absolute path, a relative path starting with ./ or ../, or the source code directly when opts.eval = true
     *      @param opts construction options, supports eval, workerData, file_system and safe_buffer
     *
     */
    constructor(path: string, opts?: FIBJS.GeneralObject);

    /**
     * @description Queries the logical worker id of the target worker
     *
     *      Assigned when the Worker is created; the same number is available inside the thread as
     *      `require('worker_threads').threadId`. The main thread has id 0 and worker ids are positive and
     *      increasing, but they are logical fibjs isolate ids, not OS thread ids, and are not stable
     *      across runs.
     *
     */
    readonly threadId: number;

    /**
     * @description Sends a message to the peer thread
     *
     *      The value is structured-cloned, so the worker receives an independent copy; delivery is
     *      asynchronous and ordered, and messages posted before the worker started are queued until the
     *      first `message` listener is attached. Inside the worker the message arrives as the raw value
     *      on `parentPort`; posting to a Worker whose thread has already exited is a silent no-op. Use
     *      the transfer overload to move an ArrayBuffer instead of copying it.
     *
     *      Example — post to an eval worker and read the reply:
     *      ```JavaScript
     *      const { Worker } = require('worker_threads');
     *
     *      const worker = new Worker(
     *          'const { parentPort } = require("worker_threads");\n' +
     *          'parentPort.on("message", (value) => parentPort.postMessage(value * 2));',
     *          { eval: true });
     *      worker.on('message', (doubled) => {
     *          console.log(doubled); // 8
     *          worker.terminate();
     *      });
     *      worker.postMessage(4);
     *      ```
     *      @param data the message content to send
     *
     */
    postMessage(data: any): void;

    /**
     * @description Sends a message to the peer thread and transfers the specified objects
     *
     *      Every `ArrayBuffer` in `transfer` is detached on the sender before the message is queued (its
     *      `byteLength` becomes 0) and arrives usable in the worker; the message and the buffers are
     *      delivered together. fibjs only detaches ArrayBuffer entries: any other value, including a
     *      MessagePort that Node.js can transfer, is silently ignored and stays usable on the sender.
     *
     *      Example — transfer an ArrayBuffer to the worker:
     *      ```JavaScript
     *      const { Worker } = require('worker_threads');
     *
     *      const worker = new Worker(
     *          'const { parentPort } = require("worker_threads");\n' +
     *          'parentPort.on("message", (buffer) => ' +
     *          'parentPort.postMessage(new Uint8Array(buffer)[0]));',
     *          { eval: true });
     *      const buffer = new ArrayBuffer(1);
     *      new Uint8Array(buffer)[0] = 7;
     *      worker.on('message', (value) => {
     *          console.log(value, buffer.byteLength); // 7 0
     *          worker.terminate();
     *      });
     *      worker.postMessage(buffer, [buffer]);
     *      ```
     *      @param data the message content to send
     *      @param transfer the array of objects to transfer (ArrayBuffer, etc.); after transfer, the original objects can no longer be used by the sender
     *
     */
    postMessage(data: any, transfer: any[]): void;

    /**
     * @description Terminates the worker and resolves with its exit code
     *
     *    The termination starts synchronously and the returned promise resolves when the worker emits
     *    `exit`, with the same code. A worker that has already exited resolves immediately with its
     *    recorded code, and calling terminate() twice is safe. A worker stopped by terminate() reports
     *    exit code 1, including a worker that was terminated before it went online (Node.js reports 0
     *    for that case); no `error` event is emitted for the interruption and the worker's pending
     *    `beforeExit`/`exit` handlers do not run.
     *
     *    Example — stop a worker that would otherwise run forever:
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker('setInterval(() => {}, 1000);', { eval: true });
     *    worker.on('online', async () => {
     *        console.log(await worker.terminate()); // 1
     *    });
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminate(): Promise<number>;

    /**
     * @description Terminates the worker and resolves with its exit code
     *
     *    The termination starts synchronously and the returned promise resolves when the worker emits
     *    `exit`, with the same code. A worker that has already exited resolves immediately with its
     *    recorded code, and calling terminate() twice is safe. A worker stopped by terminate() reports
     *    exit code 1, including a worker that was terminated before it went online (Node.js reports 0
     *    for that case); no `error` event is emitted for the interruption and the worker's pending
     *    `beforeExit`/`exit` handlers do not run.
     *
     *    Example — stop a worker that would otherwise run forever:
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker('setInterval(() => {}, 1000);', { eval: true });
     *    worker.on('online', async () => {
     *        console.log(await worker.terminate()); // 1
     *    });
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminateSync(): number;

    /**
     * @description Terminates the worker and resolves with its exit code
     *
     *    The termination starts synchronously and the returned promise resolves when the worker emits
     *    `exit`, with the same code. A worker that has already exited resolves immediately with its
     *    recorded code, and calling terminate() twice is safe. A worker stopped by terminate() reports
     *    exit code 1, including a worker that was terminated before it went online (Node.js reports 0
     *    for that case); no `error` event is emitted for the interruption and the worker's pending
     *    `beforeExit`/`exit` handlers do not run.
     *
     *    Example — stop a worker that would otherwise run forever:
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker('setInterval(() => {}, 1000);', { eval: true });
     *    worker.on('online', async () => {
     *        console.log(await worker.terminate()); // 1
     *    });
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminateAsync(): Promise<number>;

    /**
     * @description Keeps the fibjs process from exiting
     *
     *    A Worker refs the process from creation, so the parent waits for the worker; the method is
     *    idempotent and returns undefined. Call `unref()` to drop that reference again.
     *
     */
    ref(): void;

    /**
     * @description Allows the fibjs process to exit
     *
     *    After the call the worker no longer keeps the process alive: pending `beforeExit` handlers can
     *    run and the process may exit while the worker is still running. The worker keeps running as long
     *    as the process lives for other reasons. Idempotent, returns undefined; `ref()` restores the
     *    reference.
     *
     */
    unref(): void;

    /**
     * @description Queries and binds the worker ready event, equivalent to on("online", func);
     *
     *    Emitted once the worker thread has started and before it runs its script; the listener gets no
     *    argument. It is not emitted when the worker is terminated before starting, so do not use it as
     *    the only completion signal. The `ononline` property is the same binding.
     *
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
     *
     *    Emitted once the worker thread has started and before it runs its script; the listener gets no
     *    argument. It is not emitted when the worker is terminated before starting, so do not use it as
     *    the only completion signal. The `ononline` property is the same binding.
     *
     */
    ononline: (()=>void) | null;

    /**
     * @description Queries and binds the postMessage message event, equivalent to on("message", func);
     *
     *    The listener receives the deserialized value itself (a structured clone), not a MessageEvent;
     *    `worker.onmessage` behaves identically. Messages posted by the worker before the listener is
     *    attached are queued and delivered once the listener is added, because the first `message`
     *    listener starts the implicit port automatically.
     *      @param data the message sent by the worker thread
     *
     */
    on(event: "message", listener: (data: any)=>void): this;

    once(event: "message", listener: (data: any)=>void): this;

    off(event: "message", listener: (data: any)=>void): this;

    addListener(event: "message", listener: (data: any)=>void): this;

    removeListener(event: "message", listener: (data: any)=>void): this;

    addEventListener(event: "message", listener: (data: any)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: (data: any)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: (data: any)=>void): this;

    prependOnceListener(event: "message", listener: (data: any)=>void): this;

    /**
     * @description Queries and binds the postMessage message event, equivalent to on("message", func);
     *
     *    The listener receives the deserialized value itself (a structured clone), not a MessageEvent;
     *    `worker.onmessage` behaves identically. Messages posted by the worker before the listener is
     *    attached are queued and delivered once the listener is added, because the first `message`
     *    listener starts the implicit port automatically.
     *      @param data the message sent by the worker thread
     *
     */
    onmessage: ((data: any)=>void) | null;

    /**
     * @description Queries and binds the error message event, equivalent to on("error", func);
     *
     *    Emitted when the worker script throws an uncaught exception; the listener receives an Error
     *    carrying the original message and the worker-side stack. The worker then exits with code 1 and
     *    emits `exit`, so attach this listener together with `exit`. terminate() does not emit `error`.
     *      @param err the uncaught error of the worker thread
     *
     */
    on(event: "error", listener: (err: any)=>void): this;

    once(event: "error", listener: (err: any)=>void): this;

    off(event: "error", listener: (err: any)=>void): this;

    addListener(event: "error", listener: (err: any)=>void): this;

    removeListener(event: "error", listener: (err: any)=>void): this;

    addEventListener(event: "error", listener: (err: any)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (err: any)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: (err: any)=>void): this;

    prependOnceListener(event: "error", listener: (err: any)=>void): this;

    /**
     * @description Queries and binds the error message event, equivalent to on("error", func);
     *
     *    Emitted when the worker script throws an uncaught exception; the listener receives an Error
     *    carrying the original message and the worker-side stack. The worker then exits with code 1 and
     *    emits `exit`, so attach this listener together with `exit`. terminate() does not emit `error`.
     *      @param err the uncaught error of the worker thread
     *
     */
    onerror: ((err: any)=>void) | null;

    /**
     * @description Queries and binds the worker exit event, equivalent to on("exit", func);
     *
     *    Emitted exactly once when the thread ends, after `error` if there was one. The argument is the
     *    numeric exit code: 0 for a script that finished, N for `process.exit(N)` inside the worker and 1
     *    for an uncaught exception or terminate(). The terminate() promise resolves right after this
     *    event.
     *      @param code the exit code of the worker thread
     *
     */
    on(event: "exit", listener: (code: number)=>void): this;

    once(event: "exit", listener: (code: number)=>void): this;

    off(event: "exit", listener: (code: number)=>void): this;

    addListener(event: "exit", listener: (code: number)=>void): this;

    removeListener(event: "exit", listener: (code: number)=>void): this;

    addEventListener(event: "exit", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "exit", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "exit", listener: (code: number)=>void): this;

    prependOnceListener(event: "exit", listener: (code: number)=>void): this;

    /**
     * @description Queries and binds the worker exit event, equivalent to on("exit", func);
     *
     *    Emitted exactly once when the thread ends, after `error` if there was one. The argument is the
     *    numeric exit code: 0 for a script that finished, N for `process.exit(N)` inside the worker and 1
     *    for an uncaught exception or terminate(). The terminate() promise resolves right after this
     *    event.
     *      @param code the exit code of the worker thread
     *
     */
    onexit: ((code: number)=>void) | null;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * The promise variant of the Worker class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_WorkerPromise extends Class_EventEmitter {
    /**
     * @description Creates a worker and starts its thread
     *
     *    The `path` argument selects the source:
     *
     *    - a script path: absolute, starting with `./` or `../` (resolved against the current working
     *      directory), or a `file://` URL; a bare relative path such as `'worker.js'` throws `TypeError`
     *      with code `ERR_WORKER_PATH`, as in Node.js;
     *    - JavaScript source when `opts.eval` is true; the source runs with the filename
     *      `[worker eval].js`.
     *
     *    `opts` accepts the following options:
     *    ```JavaScript
     *    // fragment: constructor options
     *    ({
     *        "eval": false,        // true: path holds JavaScript source instead of a file name
     *        "workerData": null,   // value cloned into the worker; read as worker_threads.workerData
     *        "file_system": true,  // fibjs extension: false makes real-file access throw [20009]
     *        "safe_buffer": false  // fibjs extension: true restricts Buffer codecs to native ones
     *    })
     *    ```
     *
     *    `workerData` is cloned synchronously in the constructor; a value that cannot be cloned (for
     *    example a MessagePort or a SharedArrayBuffer) makes the constructor throw. Messages posted
     *    immediately after construction are queued until the worker starts.
     *      @param path the Worker entry script; accepts an absolute path, a relative path starting with ./ or ../, or the source code directly when opts.eval = true
     *      @param opts construction options, supports eval, workerData, file_system and safe_buffer
     *
     */
    constructor(path: string, opts?: FIBJS.GeneralObject);

    /**
     * @description Queries the logical worker id of the target worker
     *
     *      Assigned when the Worker is created; the same number is available inside the thread as
     *      `require('worker_threads').threadId`. The main thread has id 0 and worker ids are positive and
     *      increasing, but they are logical fibjs isolate ids, not OS thread ids, and are not stable
     *      across runs.
     *
     */
    readonly threadId: number;

    /**
     * @description Sends a message to the peer thread
     *
     *      The value is structured-cloned, so the worker receives an independent copy; delivery is
     *      asynchronous and ordered, and messages posted before the worker started are queued until the
     *      first `message` listener is attached. Inside the worker the message arrives as the raw value
     *      on `parentPort`; posting to a Worker whose thread has already exited is a silent no-op. Use
     *      the transfer overload to move an ArrayBuffer instead of copying it.
     *
     *      Example — post to an eval worker and read the reply:
     *      ```JavaScript
     *      const { Worker } = require('worker_threads');
     *
     *      const worker = new Worker(
     *          'const { parentPort } = require("worker_threads");\n' +
     *          'parentPort.on("message", (value) => parentPort.postMessage(value * 2));',
     *          { eval: true });
     *      worker.on('message', (doubled) => {
     *          console.log(doubled); // 8
     *          worker.terminate();
     *      });
     *      worker.postMessage(4);
     *      ```
     *      @param data the message content to send
     *
     */
    postMessage(data: any): void;

    /**
     * @description Sends a message to the peer thread and transfers the specified objects
     *
     *      Every `ArrayBuffer` in `transfer` is detached on the sender before the message is queued (its
     *      `byteLength` becomes 0) and arrives usable in the worker; the message and the buffers are
     *      delivered together. fibjs only detaches ArrayBuffer entries: any other value, including a
     *      MessagePort that Node.js can transfer, is silently ignored and stays usable on the sender.
     *
     *      Example — transfer an ArrayBuffer to the worker:
     *      ```JavaScript
     *      const { Worker } = require('worker_threads');
     *
     *      const worker = new Worker(
     *          'const { parentPort } = require("worker_threads");\n' +
     *          'parentPort.on("message", (buffer) => ' +
     *          'parentPort.postMessage(new Uint8Array(buffer)[0]));',
     *          { eval: true });
     *      const buffer = new ArrayBuffer(1);
     *      new Uint8Array(buffer)[0] = 7;
     *      worker.on('message', (value) => {
     *          console.log(value, buffer.byteLength); // 7 0
     *          worker.terminate();
     *      });
     *      worker.postMessage(buffer, [buffer]);
     *      ```
     *      @param data the message content to send
     *      @param transfer the array of objects to transfer (ArrayBuffer, etc.); after transfer, the original objects can no longer be used by the sender
     *
     */
    postMessage(data: any, transfer: any[]): void;

    /**
     * @description Terminates the worker and resolves with its exit code
     *
     *    The termination starts synchronously and the returned promise resolves when the worker emits
     *    `exit`, with the same code. A worker that has already exited resolves immediately with its
     *    recorded code, and calling terminate() twice is safe. A worker stopped by terminate() reports
     *    exit code 1, including a worker that was terminated before it went online (Node.js reports 0
     *    for that case); no `error` event is emitted for the interruption and the worker's pending
     *    `beforeExit`/`exit` handlers do not run.
     *
     *    Example — stop a worker that would otherwise run forever:
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker('setInterval(() => {}, 1000);', { eval: true });
     *    worker.on('online', async () => {
     *        console.log(await worker.terminate()); // 1
     *    });
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminate(): Promise<number>;

    /**
     * @description Terminates the worker and resolves with its exit code
     *
     *    The termination starts synchronously and the returned promise resolves when the worker emits
     *    `exit`, with the same code. A worker that has already exited resolves immediately with its
     *    recorded code, and calling terminate() twice is safe. A worker stopped by terminate() reports
     *    exit code 1, including a worker that was terminated before it went online (Node.js reports 0
     *    for that case); no `error` event is emitted for the interruption and the worker's pending
     *    `beforeExit`/`exit` handlers do not run.
     *
     *    Example — stop a worker that would otherwise run forever:
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker('setInterval(() => {}, 1000);', { eval: true });
     *    worker.on('online', async () => {
     *        console.log(await worker.terminate()); // 1
     *    });
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminateSync(): number;

    /**
     * @description Terminates the worker and resolves with its exit code
     *
     *    The termination starts synchronously and the returned promise resolves when the worker emits
     *    `exit`, with the same code. A worker that has already exited resolves immediately with its
     *    recorded code, and calling terminate() twice is safe. A worker stopped by terminate() reports
     *    exit code 1, including a worker that was terminated before it went online (Node.js reports 0
     *    for that case); no `error` event is emitted for the interruption and the worker's pending
     *    `beforeExit`/`exit` handlers do not run.
     *
     *    Example — stop a worker that would otherwise run forever:
     *    ```JavaScript
     *    const { Worker } = require('worker_threads');
     *
     *    const worker = new Worker('setInterval(() => {}, 1000);', { eval: true });
     *    worker.on('online', async () => {
     *        console.log(await worker.terminate()); // 1
     *    });
     *    ```
     *      @return returns the exit code of the worker
     *
     */
    terminateAsync(): Promise<number>;

    /**
     * @description Keeps the fibjs process from exiting
     *
     *    A Worker refs the process from creation, so the parent waits for the worker; the method is
     *    idempotent and returns undefined. Call `unref()` to drop that reference again.
     *
     */
    ref(): void;

    /**
     * @description Allows the fibjs process to exit
     *
     *    After the call the worker no longer keeps the process alive: pending `beforeExit` handlers can
     *    run and the process may exit while the worker is still running. The worker keeps running as long
     *    as the process lives for other reasons. Idempotent, returns undefined; `ref()` restores the
     *    reference.
     *
     */
    unref(): void;

    /**
     * @description Queries and binds the worker ready event, equivalent to on("online", func);
     *
     *    Emitted once the worker thread has started and before it runs its script; the listener gets no
     *    argument. It is not emitted when the worker is terminated before starting, so do not use it as
     *    the only completion signal. The `ononline` property is the same binding.
     *
     */
    ononline: (()=>void) | null;

    /**
     * @description Queries and binds the postMessage message event, equivalent to on("message", func);
     *
     *    The listener receives the deserialized value itself (a structured clone), not a MessageEvent;
     *    `worker.onmessage` behaves identically. Messages posted by the worker before the listener is
     *    attached are queued and delivered once the listener is added, because the first `message`
     *    listener starts the implicit port automatically.
     *      @param data the message sent by the worker thread
     *
     */
    onmessage: ((data: any)=>void) | null;

    /**
     * @description Queries and binds the error message event, equivalent to on("error", func);
     *
     *    Emitted when the worker script throws an uncaught exception; the listener receives an Error
     *    carrying the original message and the worker-side stack. The worker then exits with code 1 and
     *    emits `exit`, so attach this listener together with `exit`. terminate() does not emit `error`.
     *      @param err the uncaught error of the worker thread
     *
     */
    onerror: ((err: any)=>void) | null;

    /**
     * @description Queries and binds the worker exit event, equivalent to on("exit", func);
     *
     *    Emitted exactly once when the thread ends, after `error` if there was one. The argument is the
     *    numeric exit code: 0 for a script that finished, N for `process.exit(N)` inside the worker and 1
     *    for an uncaught exception or terminate(). The terminate() promise resolves right after this
     *    event.
     *      @param code the exit code of the worker thread
     *
     */
    onexit: ((code: number)=>void) | null;

}


declare namespace Class_Worker {
    const promises: FIBJS.GeneralObject;
}
