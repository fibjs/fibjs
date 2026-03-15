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
     * @description Event handler for message events. Setting this automatically calls start(). 
     */
    onmessage: any;

    /**
     * @description Event handler for message deserialization errors 
     */
    onmessageerror: any;

}

