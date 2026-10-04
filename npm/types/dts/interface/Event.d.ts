/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * @description The Event module provides an event object for coordinated shared data operations. It allows multiple fibers (coroutines) to perform synchronous operations, implementing cooperative multitasking. The event object has three methods: wait, pulse and clear. The wait method blocks the current fiber until the event is triggered, the pulse method wakes up all fibers waiting for the event, and the clear method resets the event flag to false. By using the coroutine.Event module, developers can control the execution order and data sharing between fibers and implement complex business logic.
 *
 * For example, suppose two fibers need to share data, but the order in which they execute is indeterminate; you can use an event object to control the execution order of the fibers and ensure that the event of one fiber is triggered before another fiber executes.
 * ```JavaScript
 * const coroutine = require('coroutine');
 *
 * var evt = new coroutine.Event();
 *
 * coroutine.start(function() {
 *    console.log('[1] wait for event');
 *    evt.wait();
 *    console.log('[1] receive event');
 * });
 *
 * coroutine.start(function() {
 *    loop:for (var i = 0; i < 10; i++) {
 *       console.log('[2] do some work');
 *       if (i === 5) {
 *          evt.pulse();
 *       }
 *       coroutine.sleep(1000);
 *    }
 * });
 * ```
 * In the example above, we create an event object evt, use the wait method in fiber 1 to wait for the event to be triggered, and trigger the event through the pulse method in fiber 2. When i equals 5, fiber 2 triggers the event, and fiber 1 is woken up through the event listener and continues executing. In this process, no locks or other synchronization tools are used between the two fibers, but they guarantee data synchronization at the fiber level.
 *
 */
declare class Class_Event extends Class_Lock {
    /**
     * @description Event object constructor
     *      @param value whether to wait; waits when true, default is false
     *
     */
    constructor(value?: boolean);

    /**
     * @description Determines whether the event object is true
     *      @return returns true if the event is true
     *
     */
    isSet(): boolean;

    /**
     * @description Activates the event (sets the event state to true) and calls pulse()
     */
    set(): void;

    /**
     * @description Activates all fibers waiting for this event
     */
    pulse(): void;

    /**
     * @description Resets the event (sets the event state to false)
     */
    clear(): void;

    /**
     * @description Waits for an event
     */
    wait(): void;

    wait(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Waits for an event
     */
    waitSync(): void;

    /**
     * @description Waits for an event
     */
    waitAsync(): Promise<void>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * The promise variant of the Event class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_EventPromise extends Class_LockPromise {
    /**
     * @description Event object constructor
     *      @param value whether to wait; waits when true, default is false
     *
     */
    constructor(value?: boolean);

    /**
     * @description Determines whether the event object is true
     *      @return returns true if the event is true
     *
     */
    isSet(): boolean;

    /**
     * @description Activates the event (sets the event state to true) and calls pulse()
     */
    set(): void;

    /**
     * @description Activates all fibers waiting for this event
     */
    pulse(): void;

    /**
     * @description Resets the event (sets the event state to false)
     */
    clear(): void;

    /**
     * @description Waits for an event
     */
    wait(): Promise<void>;

    /**
     * @description Waits for an event
     */
    waitSync(): void;

    /**
     * @description Waits for an event
     */
    waitAsync(): Promise<void>;

}


declare namespace Class_Event {
    const promises: FIBJS.GeneralObject;
}
