/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * @description Condition variable object
 *
 *  A condition variable is a mechanism for synchronization using global variables shared between fibers; it mainly involves two actions:
 *  1) one thread waits for a condition to become true and suspends itself;
 *  2) another thread makes the condition true and notifies the waiting fibers to continue execution.
 *
 *  To prevent races, each condition variable needs to work with a Lock (the Lock can be created explicitly and passed in, or fibjs can create it for you)
 *
 *  By using a condition variable, one condition variable can control the switching of a batch of fibers;
 *
 *  The following is an example of scheduling two fibers:
 *  ```JavaScript
 *  var coroutine = require("coroutine");
 *  var cond = new coroutine.Condition();
 *  var ready = false;
 *  var state = "ready";
 *
 *  function funcwait() {
 *     cond.acquire();
 *     while (!ready)
 *         cond.wait();
 *     state = "go"
 *     cond.release();
 *  }
 *
 *  coroutine.start(funcwait);
 *
 *  cond.acquire();
 *  console.log(state)
 *  ready = true;
 *  cond.notify();
 *  coroutine.sleep();
 *  console.log(state);
 *  ```
 *  will output:
 *  ```sh
 *  ready
 *  go
 *  ```
 *
 */
declare class Class_Condition extends Class_Lock {
    /**
     * @description Condition variable constructor (the lock needed by the condition variable is constructed internally by fibjs)
     */
    constructor();

    /**
     * @description Condition variable constructor
     *      @param lock use a self-constructed lock
     *
     */
    constructor(lock: Class_Lock);

    /**
     * @description Waits for a condition variable
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    wait(timeout?: number): boolean;

    wait(timeout?: number, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Waits for a condition variable
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    waitSync(timeout?: number): boolean;

    /**
     * @description Waits for a condition variable
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    waitAsync(timeout?: number): Promise<boolean>;

    /**
     * @description Notifies one blocked fiber (the last one added to the fiber pool) to continue execution
     */
    notify(): void;

    /**
     * @description Notifies all blocked fibers to continue execution
     */
    notifyAll(): void;

}

