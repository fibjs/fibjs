/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description MessagePort is one end of a message channel; a value posted to it is structured-cloned and delivered to the paired port
 *
 *  Ports come in connected pairs. Obtain one from `new MessageChannel()` (the global class or
 *  `require('worker_threads').MessageChannel`), or inside a worker from
 *  `require('worker_threads').parentPort`. A MessagePort cannot be constructed directly
 *  (`new MessagePort()` throws `TypeError`), and fibjs does not support port transfer: a port cannot
 *  be sent through workerData or carried in another port's transfer list.
 *
 *  Concepts:
 *
 *  - **Paired endpoints**: each port has at most one peer. `close()` severs the pair, and posting on
 *    a closed port, or to a peer that has closed, is a silent no-op; closing one port does not close
 *    the other.
 *  - **Message delivery**: delivery is asynchronous and ordered, and each listener runs in its own
 *    fiber. The sender structured-clones the value, so the two sides never share the object.
 *  - **Two delivery modes**: a port created by `new MessageChannel()` delivers a MessageEvent whose
 *    `data` holds the payload (`ev.data`); the `parentPort` of a Worker delivers the raw deserialized
 *    value because it is bound to the Worker's `message` event.
 *  - **Start and stop**: the port starts receiving when its first `message` listener is attached by
 *    any means (`on('message')`, `addEventListener('message')` or `onmessage`), and messages that
 *    arrived earlier are retained and delivered at that point. `start()` is therefore optional and
 *    idempotent in fibjs, where MDN requires an explicit start() after addEventListener.
 *  - **Event-loop keep-alive**: a started port that can still receive keeps the process alive until
 *    it is closed or `unref()` is called; an unstarted port never holds the process.
 *
 *  Example 1 — receive through the onmessage property:
 *  ```JavaScript
 *  const { MessageChannel } = require('worker_threads');
 *
 *  const { port1, port2 } = new MessageChannel();
 *  port2.onmessage = (ev) => {
 *      console.log(ev.data); // hello
 *      port1.close();
 *      port2.close();
 *  };
 *  port1.postMessage('hello');
 *  ```
 *
 *  Example 2 — listen with addEventListener and observe close:
 *  ```JavaScript
 *  const { MessageChannel } = require('worker_threads');
 *
 *  const { port1, port2 } = new MessageChannel();
 *  port1.addEventListener('close', () => console.log('port1 closed'));
 *  port2.addEventListener('message', (ev) => {
 *      console.log(ev.data); // 42
 *      port1.close();
 *      port2.close();
 *  });
 *  port2.start();
 *  port1.postMessage(42);
 *  ```
 *
 *  Example 3 — a bidirectional exchange with a transferred ArrayBuffer:
 *  ```JavaScript
 *  const { MessageChannel } = require('worker_threads');
 *
 *  const { port1, port2 } = new MessageChannel();
 *  port1.on('message', (ev) => {
 *      console.log('port1 got', ev.data); // port1 got ack
 *      port1.close();
 *      port2.close();
 *  });
 *  port2.onmessage = (ev) => {
 *      console.log('port2 got', ev.data.byteLength); // port2 got 8
 *      port2.postMessage('ack');
 *  };
 *  const buffer = new ArrayBuffer(8);
 *  port1.postMessage(buffer, [buffer]);
 *  console.log('sender buffer', buffer.byteLength); // sender buffer 0
 *  ```
 *
 *  Notes:
 *
 *  - Node.js and MDN allow a MessagePort to be transferred (as workerData or in the transfer list);
 *    fibjs ignores a port in a transfer list and throws `could not be cloned` for workerData, so ports
 *    cannot be exchanged. A Worker and its parent can only use the implicit `parentPort` channel.
 *  - `messageerror` exists for API compatibility but is never emitted, because a value produced by
 *    the V8 serializer can always be deserialized again.
 *
 */
declare class Class_MessagePort extends Class_EventEmitter {
    /**
     * @description Sends a message to the paired port
     *
     *      The value is cloned with the structured clone algorithm, so the receiver gets an independent
     *      copy; objects, arrays, Date, RegExp, Map, Set, Error, typed arrays and ArrayBuffer are
     *      supported, while functions, native handles and SharedArrayBuffer are not and make the call
     *      throw `Error: <value> could not be cloned.`. In the normal MessageChannel mode the receiver is
     *      called with a MessageEvent carrying the value as `data`. Messages are delivered in order;
     *      posting when this port or its peer is closed is a no-op.
     *      @param data The data to send. The data is cloned using structured clone algorithm.
     *
     */
    postMessage(data: any): void;

    /**
     * @description Sends a message and transfers the listed ArrayBuffers instead of copying them
     *
     *      Each `ArrayBuffer` in `transfer` is detached on the sender before the message is queued (its
     *      `byteLength` becomes 0) and the receiver gets the same memory. fibjs ignores non-ArrayBuffer
     *      entries, including MessagePort, which Node.js and MDN allow to be transferred; passing a port
     *      does not error and does not move it.
     *
     *      Example — detach an ArrayBuffer on send:
     *      ```JavaScript
     *      const { MessageChannel } = require('worker_threads');
     *
     *      const { port1, port2 } = new MessageChannel();
     *      port2.onmessage = (ev) => {
     *          console.log(ev.data.byteLength); // 8
     *          port1.close();
     *          port2.close();
     *      };
     *      const buffer = new ArrayBuffer(8);
     *      port1.postMessage(buffer, [buffer]);
     *      console.log(buffer.byteLength); // 0
     *      ```
     *      @param data The data to send
     *      @param transfer Array of transferable objects (e.g. ArrayBuffer) to transfer ownership
     *
     */
    postMessage(data: any, transfer: any[]): void;

    /**
     * @description Starts dispatching messages to the listeners of this port
     *
     *      Messages received before the call are retained and delivered after it; the call is idempotent
     *      and a no-op on a closed port. In fibjs the first `message` listener (including one added with
     *      `addEventListener`) starts the port automatically, so `start()` is optional; it exists for
     *      code written against MDN, where addEventListener requires an explicit start() to process the
     *      queued messages.
     *
     *      Example — start explicitly after addEventListener:
     *      ```JavaScript
     *      const { MessageChannel } = require('worker_threads');
     *
     *      const { port1, port2 } = new MessageChannel();
     *      port2.addEventListener('message', (ev) => {
     *          console.log(ev.data); // hello
     *          port1.close();
     *          port2.close();
     *      });
     *      port2.start();
     *      port1.postMessage('hello');
     *      ```
     *
     */
    start(): void;

    /**
     * @description Closes the port and fires the 'close' event
     *
     *      Closing severs the link with the peer: subsequent `postMessage` calls on either side are
     *      silently ignored, while the peer itself stays open. Messages that were already queued and have
     *      a delivery path are flushed before the port becomes unusable to new senders; otherwise the
     *      queue is discarded. The call is idempotent and the `close` event is emitted once per port.
     *
     *      Example — closing stops delivery and fires `close` once:
     *      ```JavaScript
     *      const { MessageChannel } = require('worker_threads');
     *
     *      const { port1, port2 } = new MessageChannel();
     *      port2.onmessage = (ev) => console.log('received', ev.data);
     *      port1.on('close', () => console.log('port1 closed'));
     *      port1.close();
     *      port1.postMessage('dropped'); // ignored: the port is closed
     *      port2.close();
     *      console.log('done');
     *      ```
     *
     */
    close(): void;

    /**
     * @description Mark the port as active to keep the event loop alive
     *
     *      A started port that can still receive messages holds a keep-alive reference; `ref()` restores
     *      that reference after an `unref()`. The call is idempotent and returns undefined. Ports that
     *      were never started do not hold the process (matching Node.js), and the main-thread side of a
     *      Worker's parent port leaves the keep-alive to the Worker object.
     *
     */
    ref(): void;

    /**
     * @description Mark the port as inactive so it doesn't keep the event loop alive
     *
     *      After the call the port no longer keeps the fibjs process alive: a pending `beforeExit` can
     *      run and the process may exit with queued messages undelivered. Receiving still works while the
     *      process stays alive for other reasons. Idempotent, returns undefined; `ref()` restores the
     *      reference.
     *
     */
    unref(): void;

    /**
     * @description Queries and binds the message reception event, equivalent to on("message", func); start() is called automatically once it is set.
     *
     *      The payload depends on the mode of the port: a MessageChannel port emits a MessageEvent whose
     *      `data` is the deserialized value, while a Worker's parent port emits the raw value. Attaching
     *      the first `message` listener by any means starts the port and flushes the messages that were
     *      queued before it.
     *      @param data the received message: the deserialized value in raw message mode, a MessageEvent otherwise
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
     * @description Queries and binds the message reception event, equivalent to on("message", func); start() is called automatically once it is set.
     *
     *      The payload depends on the mode of the port: a MessageChannel port emits a MessageEvent whose
     *      `data` is the deserialized value, while a Worker's parent port emits the raw value. Attaching
     *      the first `message` listener by any means starts the port and flushes the messages that were
     *      queued before it.
     *      @param data the received message: the deserialized value in raw message mode, a MessageEvent otherwise
     *
     */
    onmessage: ((data: any)=>void) | null;

    /**
     * @description Queries and binds the message deserialization error event, equivalent to on("messageerror", func);
     *
     *      Declared for API compatibility and never emitted in normal use: fibjs serializes with V8's
     *      serializer, whose output can always be deserialized again. Listening for it is harmless.
     *
     */
    on(event: "messageerror", listener: ()=>void): this;

    once(event: "messageerror", listener: ()=>void): this;

    off(event: "messageerror", listener: ()=>void): this;

    addListener(event: "messageerror", listener: ()=>void): this;

    removeListener(event: "messageerror", listener: ()=>void): this;

    addEventListener(event: "messageerror", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "messageerror", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "messageerror", listener: ()=>void): this;

    prependOnceListener(event: "messageerror", listener: ()=>void): this;

    /**
     * @description Queries and binds the message deserialization error event, equivalent to on("messageerror", func);
     *
     *      Declared for API compatibility and never emitted in normal use: fibjs serializes with V8's
     *      serializer, whose output can always be deserialized again. Listening for it is harmless.
     *
     */
    onmessageerror: (()=>void) | null;

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

