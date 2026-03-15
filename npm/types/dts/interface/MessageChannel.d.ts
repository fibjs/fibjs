/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/MessagePort.d.ts" />
/**
 * @description MessageChannel provides a pair of connected MessagePort objects
 * 
 *  The MessageChannel constructor creates a new channel with two ports (port1 and port2).
 *  Messages sent on one port are delivered to the other port, enabling bidirectional
 *  communication.
 * 
 *  ```JavaScript
 *  const mc = new MessageChannel();
 *  mc.port1.postMessage('hello');
 *  mc.port2.onmessage = (ev) => {
 *      console.log(ev.data); // 'hello'
 *  };
 *  ```
 *  
 */
declare class Class_MessageChannel extends Class_object {
    /**
     * @description MessageChannel constructor. Creates a new channel with two connected ports. 
     */
    constructor();

    /**
     * @description The first port of the channel 
     */
    readonly port1: Class_MessagePort;

    /**
     * @description The second port of the channel 
     */
    readonly port2: Class_MessagePort;

}

