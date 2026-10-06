/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description MessageEvent is the object a MessagePort delivers for a received message; the payload is available in its `data` property
 *
 *  Instances are produced by `MessageChannel` ports when they receive a message, and the constructor
 *  `new MessageEvent('message', { data })` creates one directly for tests or synthetic events. fibjs
 *  implements the subset of the MDN MessageEvent that MessagePort uses: `data` is exposed, while
 *  `type`, `origin`, `lastEventId`, `source` and `ports` are not.
 *
 *  Concepts:
 *
 *  - **Payload only**: `data` holds the value produced by the sender's structured clone when the
 *    event comes from a port, or the value passed to the constructor as-is (no clone).
 *  - **Not a DOM event**: fibjs has no DOM event dispatch; the object is a plain wrapper emitted by
 *    MessagePort listeners, and `instanceof MessageEvent` is the way to recognize it.
 *  - **Worker parent ports do not use it**: they deliver the raw payload instead (see MessagePort).
 *
 *  Example 1 — create an event and read its payload:
 *  ```JavaScript
 *  const event = new MessageEvent('message', { data: { id: 1 } });
 *  console.log(event.data.id); // 1
 *  ```
 *
 *  Example 2 — receive a MessageEvent from a channel:
 *  ```JavaScript
 *  const { MessageChannel } = require('worker_threads');
 *
 *  const { port1, port2 } = new MessageChannel();
 *  port2.onmessage = (ev) => {
 *      console.log(ev instanceof MessageEvent, ev.data); // true received
 *      port1.close();
 *      port2.close();
 *  };
 *  port1.postMessage('received');
 *  ```
 *
 *  Example 3 — listen with addEventListener and start:
 *  ```JavaScript
 *  const { MessageChannel } = require('worker_threads');
 *
 *  const { port1, port2 } = new MessageChannel();
 *  port2.addEventListener('message', (ev) => {
 *      console.log(ev.data); // 99
 *      port1.close();
 *      port2.close();
 *  });
 *  port2.start();
 *  port1.postMessage(99);
 *  ```
 *
 *  Notes:
 *
 *  - The constructor `type` argument is required; `new MessageEvent()` throws
 *    `TypeError [20002] Parameter not optional.` The type itself is not stored.
 *  - Node.js and MDN additionally expose `ports` for transferred MessagePorts, which fibjs does not
 *    support, so a received message never has attached ports.
 *
 */
declare class Class_MessageEvent extends Class_object {
    /**
     * @description MessageEvent constructor
     *
     *      `type` is mandatory, but fibjs stores only `eventInitDict.data` (default `undefined`) and
     *      exposes no `type` property, matching the global MessageEvent. The payload is kept by
     *      reference, not cloned; structured cloning only happens when a value travels through a
     *      MessagePort. Node.js and MDN expose the full event properties and a `ports` list of
     *      transferred MessagePorts, which fibjs does not support.
     *      @param type The type of the event
     *      @param eventInitDict Optional event initialization dictionary containing data property
     *
     */
    constructor(type: string, eventInitDict?: FIBJS.GeneralObject);

    /**
     * @description The data sent by the message emitter
     *
     *      The value is whatever the sender posted (a structured clone) or whatever was passed as
     *      `eventInitDict.data` when the event was constructed; it is `undefined` when no data was
     *      provided. The property is read-only.
     *
     *      Example — construct an event and read the payload:
     *      ```JavaScript
     *      const event = new MessageEvent('message', { data: { id: 1 } });
     *      console.log(event.data.id); // 1
     *      ```
     *
     */
    readonly data: any;

}

