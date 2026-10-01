/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/// <reference path="../interface/Semaphore.d.ts" />
/// <reference path="../interface/Condition.d.ts" />
/// <reference path="../interface/Event.d.ts" />
/// <reference path="../interface/Fiber.d.ts" />
/**
 * @description Concurrency control module, providing fiber creation, scheduling, concurrent execution and synchronization primitives
 *
 *  The `coroutine` module is based on a cooperative multitasking model: fibers voluntarily yield the CPU when needed (for example, by calling `sleep` or waiting for I/O), rather than being preemptively scheduled by the system. The module provides the following capabilities:
 *
 *  - **Fiber management**: `start` starts a fiber, `current` gets the current fiber, `fibers` queries the running fibers;
 *  - **Concurrent execution**: `parallel` runs a set of functions in parallel or processes a set of data, with an optional concurrency limit;
 *  - **Scheduling control**: `sleep` pauses the current fiber and yields the CPU so that other fibers can run;
 *  - **Synchronization primitives**: `Lock` lock, `Semaphore` semaphore, `Condition` condition variable, `Event` event object.
 *
 *  Usage:
 *
 *  ```JavaScript
 *  const coroutine = require('coroutine');
 *  ```
 *
 *  The following is a simple example demonstrating how to use the `coroutine` module:
 *
 *  ```JavaScript
 *  const coroutine = require('coroutine');
 *
 *  function foo() {
 *   console.log('start foo');
 *   coroutine.sleep(1000); // enter sleep mode
 *   console.log('end foo');
 *  }
 *
 *  function bar() {
 *   console.log('start bar');
 *   coroutine.sleep(2000);
 *   console.log('end bar');
 *  }
 *
 *  coroutine.start(foo);
 *  coroutine.start(bar);
 *  ```
 *
 *  In the code above, we define two functions `foo` and `bar`, then use `coroutine.start` to start two fibers. In each fiber, we use `coroutine.sleep` to yield the CPU so that other fibers can run.
 *
 */
declare module 'coroutine' {
    /**
     * @description Lock object, see Lock
     */
    const Lock: typeof Class_Lock;

    /**
     * @description Semaphore object, see Semaphore
     */
    const Semaphore: typeof Class_Semaphore;

    /**
     * @description Condition variable object, see Condition
     */
    const Condition: typeof Class_Condition;

    /**
     * @description Event object, see Event
     */
    const Event: typeof Class_Event;

    /**
     * @description Starts a fiber and returns the fiber object
     *
     *      The parameters in args are passed to the function inside the fiber. The new fiber runs concurrently with the current fiber.
     *      @param func specifies the function executed by the fiber
     *      @param args variable argument sequence, passed to the function inside the fiber
     *      @return returns the fiber object
     *
     */
    function start(func: (...args: any[])=>any, ...args: any[]): Class_Fiber;

    /**
     * @description Runs a set of functions in parallel and waits for the results
     *
     *      Returns after all functions have finished; the returned array corresponds to the order of funcs. fibers specifies the number of concurrent fibers; the default is -1, which uses as many fibers as there are funcs.
     *      @param funcs array of functions to run in parallel
     *      @param fibers limits the number of concurrent fibers; the default is -1, which uses as many fibers as there are funcs
     *      @return returns an array of function results
     *
     */
    function parallel(funcs: any[], fibers?: number): any[];

    /**
     * @description Runs a function in parallel over a set of data and waits for the results
     *
     *      Each element in datas is passed as a parameter to func; after all complete, an array of results is returned. fibers specifies the number of concurrent fibers; the default is -1, which uses as many fibers as there are datas.
     *      @param datas array of data to process in parallel
     *      @param func the function to run in parallel
     *      @param fibers limits the number of concurrent fibers; the default is -1, which uses as many fibers as there are datas
     *      @return returns an array of function results
     *
     */
    function parallel(datas: any[], func: (...args: any[])=>any, fibers?: number): any[];

    /**
     * @description Runs a function in parallel multiple times and waits for the results
     *
     *      The function is executed num times, and an array of num results is returned. fibers specifies the number of concurrent fibers; the default is -1, which uses as many fibers as there are tasks.
     *      @param func the function to run in parallel
     *      @param num number of repeated tasks
     *      @param fibers limits the number of concurrent fibers; the default is -1, which uses as many fibers as there are functions
     *      @return returns an array of function results
     *
     */
    function parallel(func: (...args: any[])=>any, num: number, fibers?: number): any[];

    /**
     * @description Runs a set of functions in parallel and waits for the results
     *
     *      Each parameter is treated as a function to execute; after all have finished, an array of results is returned.
     *      @param funcs a set of functions to run in parallel
     *      @return returns an array of function results
     *
     */
    function parallel(...funcs: any[]): any[];

    /**
     * @description Returns the current fiber
     *      @return the current fiber object
     *
     */
    function current(): Class_Fiber;

    /**
     * @description Pauses the current fiber for the specified time
     *
     *      During the pause, the CPU is yielded so that other fibers can run. ms defaults to 0, which means resume as soon as the CPU is free.
     *      @param ms specifies the pause time in milliseconds; the default is 0, which means resume as soon as the CPU is free
     *
     */
    function sleep(ms?: number): void;

    function sleep(ms?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Pauses the current fiber for the specified time
     *
     *      During the pause, the CPU is yielded so that other fibers can run. ms defaults to 0, which means resume as soon as the CPU is free.
     *      @param ms specifies the pause time in milliseconds; the default is 0, which means resume as soon as the CPU is free
     *
     */
    function sleepSync(ms?: number): void;

    /**
     * @description Pauses the current fiber for the specified time
     *
     *      During the pause, the CPU is yielded so that other fibers can run. ms defaults to 0, which means resume as soon as the CPU is free.
     *      @param ms specifies the pause time in milliseconds; the default is 0, which means resume as soon as the CPU is free
     *
     */
    function sleepAsync(ms?: number): Promise<void>;

    /**
     * @description Returns the array of all currently running fibers
     */
    const fibers: any[];

    /**
     * @description Queries and sets the number of spare fibers; you can moderately increase the number of spare fibers when server jitter is large. The default is 256
     */
    var spareFibers: number;

    /**
     * @description Queries the current vm id
     */
    const vmid: number;

}

