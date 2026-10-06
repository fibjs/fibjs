/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/// <reference path="../interface/MessagePort.d.ts" />
/// <reference path="../interface/MessageChannel.d.ts" />
/**
 * @description The worker_threads module runs JavaScript in real OS threads, one isolate per Worker, and exchanges structured-clone messages with them
 *
 *  The module complements the fiber-based concurrency of the coroutine module: a Worker is a separate
 *  JavaScript isolate on its own thread, so it can use multiple CPU cores and keep blocking native
 *  calls away from the main thread. Use Worker for CPU-bound or isolation-sensitive work; use
 *  Fiber/coroutine for lightweight, mostly IO-bound concurrency where objects are shared directly.
 *
 *  Main capabilities:
 *
 *  - **Thread class**: `Worker` creates and controls a child thread;
 *  - **Message channels**: `MessagePort` is one end of a channel, `MessageChannel` builds a
 *    connected pair, `parentPort` is the worker-side end of the implicit channel to its parent, and
 *    `receiveMessageOnPort` dequeues a queued message synchronously;
 *  - **Context information**: `isMainThread`, `threadId` and `workerData` describe the current
 *    execution context;
 *  - **Clone-control compatibility helpers**: `markAsUncloneable`, `markAsUntransferable` and
 *    `isMarkedAsUntransferable` are accepted but have no effect.
 *
 *  Concepts:
 *
 *  - **Worker isolate**: every Worker gets its own V8 isolate, global object and module cache on its
 *    own OS thread. Plain JavaScript objects are never shared, and a SharedArrayBuffer cannot be
 *    put into a message or into workerData, so all communication goes through messages.
 *  - **workerData handoff**: `new Worker(path, { workerData })` structured-clones the value once,
 *    before the thread starts; the worker reads the clone as `workerData`, and later mutations on
 *    either side do not propagate.
 *  - **Message passing and structured clone**: `postMessage` serializes the value with V8's
 *    serializer and delivers a copy to the peer. Objects, arrays, Date, RegExp, Map, Set, Error,
 *    typed arrays and ArrayBuffer survive the round trip; functions, classes, native handles and
 *    SharedArrayBuffer do not, and throw `Error: <value> could not be cloned.` at the sender.
 *  - **Transferables**: the optional transfer list detaches `ArrayBuffer` entries on the sender
 *    instead of copying them; entries of any other type are silently ignored.
 *  - **Lifecycle**: a worker emits `online` when its isolate has started, `message` for each
 *    delivered message, `error` for an uncaught exception and `exit` once with the exit code;
 *    `terminate()` returns a promise that resolves with that code. A live Worker keeps the process
 *    alive until `unref()` or `terminate()`.
 *  - **Main thread vs worker**: in the main thread `parentPort` and `workerData` are null and
 *    `threadId` is 0; inside a worker `parentPort` is the worker's end of the implicit channel and
 *    `threadId` is positive.
 *
 *  Import:
 *  ```JavaScript
 *  const worker_threads = require('worker_threads');
 *  ```
 *
 *  Example 1 — run a worker script and exchange a message:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const worker_threads = require('worker_threads');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-worker-'));
 *  const script = path.join(dir, 'echo.js');
 *  fs.writeFile(script,
 *      'const { parentPort } = require("worker_threads");\n' +
 *      'parentPort.on("message", (value) => parentPort.postMessage(value + "!"));\n');
 *
 *  const worker = new worker_threads.Worker(script);
 *  worker.on('message', (reply) => {
 *      console.log(reply); // hello!
 *      worker.terminate().then(() => fs.rmSync(dir, { recursive: true, force: true }));
 *  });
 *  worker.postMessage('hello');
 *  ```
 *
 *  Example 2 — hand a snapshot to an eval worker with workerData:
 *  ```JavaScript
 *  const worker_threads = require('worker_threads');
 *
 *  const settings = { factor: 3, values: [1, 2] };
 *  const worker = new worker_threads.Worker(
 *      'const { parentPort, workerData } = require("worker_threads");\n' +
 *      'parentPort.postMessage(workerData.values.map((v) => v * workerData.factor));',
 *      { eval: true, workerData: settings });
 *
 *  settings.values.push(99); // the worker received a clone, so this does not matter
 *  worker.on('message', (result) => {
 *      console.log(JSON.stringify(result)); // [3,6]
 *      worker.terminate();
 *  });
 *  ```
 *
 *  Example 3 — decouple two parts of one isolate with a MessageChannel:
 *  ```JavaScript
 *  const { MessageChannel, receiveMessageOnPort } = require('worker_threads');
 *
 *  const { port1, port2 } = new MessageChannel();
 *  port1.postMessage({ id: 7 });
 *  const received = receiveMessageOnPort(port2);
 *  console.log(received.message.id); // 7
 *  port1.close();
 *  port2.close();
 *  ```
 *
 *  Notes:
 *
 *  - The classes are also installed as globals (`Worker`, `MessagePort`, `MessageChannel` and
 *    `MessageEvent`).
 *  - Differences from Node.js: `worker.exitCode`, `worker.stdin`, `worker.stdout` and
 *    `worker.stderr` are not provided; a transfer list only detaches ArrayBuffers and a MessagePort
 *    cannot be transferred or used as workerData; the `markAs*` helpers are no-ops.
 *  - Thread affinity: by default all JavaScript of one isolate runs on one dedicated OS thread
 *    (`--no-js-thread-affinity` disables this), matching the N-API contract that a napi_env belongs
 *    to a single thread. An extension using a blocking threadsafe function can self-deadlock in a
 *    worker, as it would block the Node.js event loop.
 *
 */
declare module 'worker_threads' {
    /**
     * @description Independent thread worker object, see Worker
     *
     *      The class is shared with the global `Worker`: `new Worker(path, opts)` starts a real OS
     *      thread with its own isolate.
     *
     */
    const Worker: typeof Class_Worker;

    /**
     * @description One end of a message channel, see MessagePort
     *
     *      The class is shared with the global `MessagePort`; instances are obtained from
     *      `new MessageChannel()` or from `parentPort` inside a worker, never by construction.
     *
     */
    const MessagePort: typeof Class_MessagePort;

    /**
     * @description A pair of connected MessagePort objects, see MessageChannel
     *
     *      The class is shared with the global `MessageChannel`; `new MessageChannel()` returns the
     *      linked `port1`/`port2` pair.
     *
     */
    const MessageChannel: typeof Class_MessageChannel;

    /**
     * @description Queries whether the current Worker is the main thread
     *
     *      True in the main isolate and false in every Worker, in both cases for the isolate that
     *      evaluates the expression; Node.js exposes the same boolean.
     *
     */
    const isMainThread: boolean;

    /**
     * @description Queries the logical worker identifier of the current execution context
     *
     *      Zero in the main thread; inside a Worker, the positive id that is also available as
     *      `Worker#threadId`. It is a logical fibjs isolate id (not an OS thread id) and is not stable
     *      across runs; Node.js assigns small sequential ids from 1 in the same way.
     *
     */
    const threadId: number;

    /**
     * @description Queries the worker-side MessagePort connected to the parent thread
     *
     *      Null in the main thread. Inside a Worker it returns the worker's end of the implicit channel:
     *      values posted to it arrive at the parent's `Worker` object (`worker.on('message')`), and
     *      values posted by the parent arrive as raw values on its `message` event. The port starts
     *      automatically with the first `message` listener.
     *
     */
    const parentPort: Class_MessagePort;

    /**
     * @description Queries the clone of the data passed to this thread by the parent thread through the Worker constructor
     *
     *      Null in the main thread and in workers created without the `workerData` option. The value is
     *      the structured clone made by the Worker constructor, so changing it inside the worker does
     *      not affect the parent.
     *
     */
    const workerData: any;

    /**
     * @description Synchronously receives the next queued message on a MessagePort
     *
     *      Dequeues messages in FIFO order without starting the port and without invoking its `message`
     *      listeners; returns `undefined` when the queue is empty. The result is an object with a single
     *      `message` field holding the deserialized value. `port` must be a MessagePort: as in Node.js
     *      there is no `worker.port`, so use a channel created with `new MessageChannel()` or a
     *      `parentPort` inside a worker. A value that is not a MessagePort throws `TypeError`; fibjs
     *      reports its native coercion error (`[20005] The argument could not be coerced to the
     *      specified type.`) instead of Node.js's `ERR_INVALID_ARG_TYPE`.
     *
     *      Example — drain two messages without a listener:
     *      ```JavaScript
     *      const { MessageChannel, receiveMessageOnPort } = require('worker_threads');
     *
     *      const { port1, port2 } = new MessageChannel();
     *      port1.postMessage('first');
     *      port1.postMessage('second');
     *      console.log(receiveMessageOnPort(port2).message); // first
     *      console.log(receiveMessageOnPort(port2).message); // second
     *      console.log(receiveMessageOnPort(port2) === undefined); // true
     *      port1.close();
     *      port2.close();
     *      ```
     *      @param port the MessagePort object to receive messages from
     *      @return returns the received message object, or undefined when the port is empty
     *
     */
    function receiveMessageOnPort(port: Class_MessagePort): any;

    /**
     * @description Marks an object as uncloneable, a no-op in fibjs
     *
     *      Node.js makes `postMessage` throw when a marked object is used as a message; fibjs
     *      serializes with V8's ValueSerializer, which does not honor the mark, so the call has no
     *      effect and never throws. It is provided because packages such as undici call it in Web API
     *      constructors; primitives are accepted and ignored exactly like objects.
     *      @param object the object to mark
     *
     */
    function markAsUncloneable(object: any): void;

    /**
     * @description Marks an object as untransferable, a no-op in fibjs
     *
     *      fibjs only detaches ArrayBuffer entries of a transfer list, so the mark has nothing to
     *      affect. The call is accepted for compatibility with Node.js callers and returns undefined.
     *      @param object the object to mark
     *
     */
    function markAsUntransferable(object: any): void;

    /**
     * @description Checks whether an object is marked as untransferable, always false in fibjs
     *
     *      Because markAsUntransferable is a no-op, no object is ever marked; the function exists for
     *      compatibility with Node.js callers and returns false for every value, primitives included.
     *      @param object the object to check
     *      @return returns whether the object is marked as untransferable; always false in fibjs
     *
     */
    function isMarkedAsUntransferable(object: any): boolean;

}

