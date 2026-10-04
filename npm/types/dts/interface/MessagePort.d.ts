/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description MessagePort represents one end of a message channel
 *
 *  MessagePort allows sending and receiving messages between different parts of an application.
 *  It is created via the MessageChannel constructor, which creates a pair of connected ports.
 *
 *  Messages are delivered asynchronously. When using addEventListener, call start() to begin
 *  receiving messages. Setting onmessage implicitly calls start().
 *
 *  ```JavaScript
 *  const { port1, port2 } = new MessageChannel();
 *  port2.onmessage = (ev) => {
 *      console.log(ev.data);
 *  };
 *  port1.postMessage('hello');
 *  ```
 *
 */
declare class Class_MessagePort extends Class_EventEmitter {
    /**
     * @description Send a message to the connected port
     *      @param data The data to send. The data is cloned using structured clone algorithm.
     *
     */
    postMessage(data: any): void;

    /**
     * @description Send a message with transferable objects
     *      @param data The data to send
     *      @param transfer Array of transferable objects (e.g. ArrayBuffer) to transfer ownership
     *
     */
    postMessage(data: any, transfer: any[]): void;

    /**
     * @description Start receiving messages queued on this port
     *
     *      When using addEventListener to listen for messages, you must call start() to begin
     *      dispatching queued messages. This is called automatically when onmessage is set.
     *
     */
    start(): void;

    /**
     * @description Close the port. No more messages can be sent or received after closing.
     *      Fires the 'close' event.
     *
     */
    close(): void;

    /**
     * @description Mark the port as active to keep the event loop alive
     */
    ref(): void;

    /**
     * @description Mark the port as inactive so it doesn't keep the event loop alive
     */
    unref(): void;

    /**
     * @description Queries and binds the message reception event, equivalent to on("message", func); start() is called automatically once it is set.
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
     * @description Queries and binds the message reception event, equivalent to on("message", func); start() is called automatically once it is set.
     */
    onmessage: (()=>void) | null;

    /**
     * @description Queries and binds the message deserialization error event, equivalent to on("messageerror", func);
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
     */
    onmessageerror: (()=>void) | null;

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

