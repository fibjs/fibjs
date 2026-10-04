/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description EventEmitter is an event-triggering object that can be used to build the observer pattern; all objects that support event triggering inherit from it
 *
 * When an event is triggered, all listeners associated with that event are invoked asynchronously. It also allows us to create highly customizable and flexible code.
 *
 * Commonly used functions include: addListener/on, once, removeListener/off, removeAllListeners and emit.
 *
 * The following is an example:
 *
 * ```javascript
 * var fs = require('fs');
 * var EventEmitter = require('events');
 * var event = new EventEmitter();
 *
 * event.on('read_file', function(filename) {
 *   fs.readFile(filename, 'utf8', function(err, data) {
 *     if (err) {
 *       event.emit('error', err);
 *       return;
 *     }
 *     event.emit('show_content', data);
 *   });
 * });
 *
 * event.on('error', function(err) {
 *   console.log(`Error ${err}`);
 * });
 *
 * event.on('show_content', function(content) {
 *   console.log(content);
 * });
 *
 * event.emit('read_file', 'test.txt');
 * ```
 *
 * In the example above, when run, the event emitter instance event first listens for the 'read_file' event, and then triggers the file reading operation when the event is triggered (`event.emit('read_file', 'test.txt')`). When the read succeeds, the 'show_content' event is triggered; the function listening for the 'show_content' event is then executed and displays the file content. If an error occurs while reading the file, the 'error' event is triggered, and the failure is handled.
 *
 * This pattern has great advantages in business scenarios dealing with asynchronous operations.
 *
 */
declare class Class_EventEmitter extends Class_object {
    /**
     * @description Constructor
     *      @param options options object, supports captureRejections, etc.
     *
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * @description The event emitter object
     */
    static EventEmitter: Class_EventEmitter;

    /**
     * @description Default global maximum number of listeners
     */
    static defaultMaxListeners: number;

    /**
     * @description Binds an event handler to the object
     *     @param ev the event name to bind
     *     @param func the event handler function
     *     @return returns the event object itself for chaining
     *
     */
    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    /**
     * @description Binds an event handler to the object
     *     @param map the event mapping; object property names are used as event names and property values as event handler functions
     *     @return returns the event object itself for chaining
     *
     */
    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Binds an event handler to the object
     *     @param ev the event name to bind
     *     @param func the event handler function
     *     @return returns the event object itself for chaining
     *
     */
    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    /**
     * @description Binds an event handler to the object
     *     @param map the event mapping; object property names are used as event names and property values as event handler functions
     *     @return returns the event object itself for chaining
     *
     */
    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Binds an event handler to the object
     *
     *     The options parameter is an object that can contain the following property:
     *     - once: if true, the event handler is triggered only once and is removed after being triggered
     *
     *     @param ev the event name to bind
     *     @param func the event handler function
     *     @param options the options of the event handler
     *     @return returns the event object itself for chaining
     *
     */
    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Binds an event handler to the start of the object's handler queue
     *     @param ev the event name to bind
     *     @param func the event handler function
     *     @return returns the event object itself for chaining
     *
     */
    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    /**
     * @description Binds an event handler to the start of the object's handler queue
     *     @param map the event mapping; object property names are used as event names and property values as event handler functions
     *     @return returns the event object itself for chaining
     *
     */
    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Binds a one-time event handler to the object; the one-time handler is triggered only once
     *     @param ev the event name to bind
     *     @param func the event handler function
     *     @return returns the event object itself for chaining
     *
     */
    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    /**
     * @description Binds a one-time event handler to the object; the one-time handler is triggered only once
     *     @param map the event mapping; object property names are used as event names and property values as event handler functions
     *     @return returns the event object itself for chaining
     *
     */
    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Binds an event handler to the start of the object's handler queue
     *     @param ev the event name to bind
     *     @param func the event handler function
     *     @return returns the event object itself for chaining
     *
     */
    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    /**
     * @description Binds an event handler to the start of the object's handler queue
     *     @param map the event mapping; object property names are used as event names and property values as event handler functions
     *     @return returns the event object itself for chaining
     *
     */
    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Removes the specified function from the object's handler queue
     *     @param ev the event name to unbind
     *     @param func the event handler function
     *     @return returns the event object itself for chaining
     *
     */
    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    /**
     * @description Removes all functions from the object's handler queue
     *     @param ev the event name to unbind
     *     @return returns the event object itself for chaining
     *
     */
    off(ev: any): FIBJS.GeneralObject;

    /**
     * @description Removes the specified function from the object's handler queue
     *     @param map the event mapping; object property names are used as event names and property values as event handler functions
     *     @return returns the event object itself for chaining
     *
     */
    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Removes the specified function from the object's handler queue
     *     @param ev the event name to unbind
     *     @param func the event handler function
     *     @return returns the event object itself for chaining
     *
     */
    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    /**
     * @description Removes all functions from the object's handler queue
     *     @param ev the event name to unbind
     *     @return returns the event object itself for chaining
     *
     */
    removeListener(ev: any): FIBJS.GeneralObject;

    /**
     * @description Removes the specified function from the object's handler queue
     *     @param map the event mapping; object property names are used as event names and property values as event handler functions
     *     @return returns the event object itself for chaining
     *
     */
    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Removes the specified function from the object's handler queue
     *     @param ev the event name to unbind
     *     @param func the event handler function
     *     @param options the options of the event handler
     *     @return returns the event object itself for chaining
     *
     */
    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Removes all listeners of all events from the object's handler queue; if an event is specified, removes all listeners of the specified event.
     *     @param ev the event name to remove
     *     @return returns the event object itself for chaining
     *
     */
    removeAllListeners(ev: any): FIBJS.GeneralObject;

    /**
     * @description Removes all listeners of all events from the object's handler queue; if an event is specified, removes all listeners of the specified event.
     *     @param evs the event names to remove
     *     @return returns the event object itself for chaining
     *
     */
    removeAllListeners(evs?: any[]): FIBJS.GeneralObject;

    /**
     *  The default listener limit, for compatibility only
     *     @param n the number of events
     *
     */
    setMaxListeners(n: number): void;

    /**
     *  Gets the default listener limit, for compatibility only
     *     @return returns the default limit
     *
     */
    getMaxListeners(): number;

    /**
     * @description Queries the listener array of the specified event of the object
     *     @param ev the event name to query
     *     @return returns the listener array of the specified event
     *
     */
    listeners(ev: any): any[];

    /**
     * @description Queries the listener array of the specified event of the object, including once wrapper functions
     *     @param ev the event name to query
     *     @return returns the listener array of the specified event
     *
     */
    rawListeners(ev: any): any[];

    /**
     * @description Queries the number of listeners of the specified event of the object
     *     @param ev the event name to query
     *     @return returns the number of listeners of the specified event
     *
     */
    listenerCount(ev: any): number;

    /**
     * @description Queries the number of listeners of the specified event of the object
     *     @param o the object to query
     *     @param ev the event name to query
     *     @return returns the number of listeners of the specified event
     *
     */
    listenerCount(o: any, ev: any): number;

    /**
     * @description Queries the names of the events with listeners
     *     @return returns the array of event names
     *
     */
    eventNames(): any[];

    /**
     * @description Actively triggers an event
     *     @param ev event name
     *     @param args event parameters, which are passed to the event handler
     *     @return returns the event trigger status; returns true if the event is responded to, otherwise false
     *
     */
    emit(ev: any, ...args: any[]): boolean;

    /**
     * @description Listens for the abort event of an AbortSignal and returns a disposable object
     *
     *     The returned object contains a `[Symbol.dispose]()` method; calling it removes the listener. If the signal has already been aborted, the listener is invoked immediately.
     *
     *     @param signal the AbortSignal object to listen to
     *     @param func the handler for the abort event
     *     @return returns a Disposable object containing a `[Symbol.dispose]` method
     *
     */
    static addAbortListener(signal: Class_EventEmitter, func: (ev: FIBJS.GeneralObject)=>void): FIBJS.GeneralObject;

    /**
     * @description Creates a Promise that resolves after the specified event is triggered once
     *
     *     Returns a Promise that resolves with the array of event parameters when the target event is triggered. If the 'error' event is triggered in the meantime (and what is listened to is not the 'error' event itself), the Promise is rejected.
     *
     *     The options parameter can contain:
     *     - signal: AbortSignal, used to cancel the wait
     *
     *     @param emitter the event emitter object to listen to
     *     @param ev the event name to listen for
     *     @param options optional parameter object
     *     @return returns a Promise that resolves with the array of event parameters
     *
     */
    static once(emitter: Class_EventEmitter, ev: any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Creates an async iterator that continuously listens for the specified event
     *
     *     Returns an AsyncIterator that yields the array of event parameters each time the event is triggered. If the 'error' event is triggered, the iterator throws an error.
     *
     *     The options parameter can contain:
     *     - signal: AbortSignal, used to cancel the iteration
     *     - close: string array, specifying the names of events that end the iteration
     *
     *     @param emitter the event emitter object to listen to
     *     @param ev the event name to listen for
     *     @param options optional parameter object
     *     @return returns an AsyncIterator object
     *
     */
    static on(emitter: Class_EventEmitter, ev: any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

}

