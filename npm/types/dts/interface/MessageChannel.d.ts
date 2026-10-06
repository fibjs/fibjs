/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/MessagePort.d.ts" />
/**
 * @description MessageChannel creates a connected pair of MessagePort objects
 *
 *  `new MessageChannel()` (the global class or `require('worker_threads').MessageChannel`) returns a
 *  channel whose two ports are already linked: a value posted to `port1` is delivered to `port2` and
 *  vice versa. Use it to decouple two parts of one program, or to build a message path step by step.
 *
 *  Concepts:
 *
 *  - **Pair semantics**: the constructor creates both endpoints at once; the ports are independent
 *    objects, and closing one severs the link without closing the other.
 *  - **Message passing**: the sender structured-clones the value and the receiver gets a MessageEvent
 *    carrying it as `data`; delivery is asynchronous and ordered. An optional transfer list moves
 *    ArrayBuffers instead of copying them.
 *  - **Lifetime**: a started port keeps the process alive while it can receive; close both ports (or
 *    `unref()` them) so the program can exit.
 *
 *  Example 1 — a round trip between the two ports:
 *  ```JavaScript
 *  const { MessageChannel } = require('worker_threads');
 *
 *  const channel = new MessageChannel();
 *  channel.port2.onmessage = (ev) => {
 *      console.log(ev.data); // ping
 *      channel.port1.close();
 *      channel.port2.close();
 *  };
 *  channel.port1.postMessage('ping');
 *  ```
 *
 *  Example 2 — bidirectional exchange:
 *  ```JavaScript
 *  const { MessageChannel } = require('worker_threads');
 *
 *  const channel = new MessageChannel();
 *  let count = 0;
 *  const closeWhenDone = () => {
 *      if (++count < 2) return;
 *      channel.port1.close();
 *      channel.port2.close();
 *  };
 *  channel.port1.on('message', (ev) => {
 *      console.log('port1 got', ev.data); // port1 got from2
 *      closeWhenDone();
 *  });
 *  channel.port2.on('message', (ev) => {
 *      console.log('port2 got', ev.data); // port2 got from1
 *      closeWhenDone();
 *  });
 *  channel.port1.postMessage('from1');
 *  channel.port2.postMessage('from2');
 *  ```
 *
 *  Example 3 — messages arrive in the order they were posted:
 *  ```JavaScript
 *  const { MessageChannel } = require('worker_threads');
 *
 *  const channel = new MessageChannel();
 *  const received = [];
 *  channel.port2.onmessage = (ev) => {
 *      received.push(ev.data);
 *      if (received.length < 3) return;
 *      console.log(JSON.stringify(received)); // [1,2,3]
 *      channel.port1.close();
 *      channel.port2.close();
 *  };
 *  channel.port1.postMessage(1);
 *  channel.port1.postMessage(2);
 *  channel.port1.postMessage(3);
 *  ```
 *
 */
declare class Class_MessageChannel extends Class_object {
    /**
     * @description MessageChannel constructor. Creates a new channel with two connected ports.
     *
     *      The constructor takes no arguments and links the two ports immediately; there is no way to
     *      create an unconnected MessagePort. MDN and Node.js expose the same constructor.
     *
     */
    constructor();

    /**
     * @description The first port of the channel
     *
     *      The two ports are interchangeable except for the property name: a value posted to `port1` is
     *      received by `port2` and vice versa. The property is read-only, but the port object itself
     *      supports postMessage/start/close/ref/unref like any MessagePort.
     *
     */
    readonly port1: Class_MessagePort;

    /**
     * @description The second port of the channel
     *
     *      The peer of `port1`; either port can send first, and both must be closed when the channel is
     *      no longer needed so the process can exit.
     *
     */
    readonly port2: Class_MessagePort;

}

