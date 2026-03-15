/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description MessageEvent represents a message received by a target object
 * 
 *  MessageEvent is used to represent messages received through MessagePort's postMessage API.
 *  It contains a `data` property with the message payload.
 * 
 *  ```JavaScript
 *  const { port1, port2 } = new MessageChannel();
 *  port2.onmessage = (ev) => {
 *      console.log(ev.data); // message payload
 *  };
 *  port1.postMessage('hello');
 *  ```
 *  
 */
declare class Class_MessageEvent extends Class_object {
    /**
     * @description MessageEvent constructor
     *      @param type The type of the event
     *      @param eventInitDict Optional event initialization dictionary containing data property
     *      
     */
    constructor(type: string, eventInitDict?: FIBJS.GeneralObject);

    /**
     * @description The data sent by the message emitter 
     */
    readonly data: any;

}

