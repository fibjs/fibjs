/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * @description Fiber semaphore object
 *
 *  The semaphore object manages an internal counter; the counter is decremented by calls to acquire or wait, and incremented by calls to release or post.
 *  The counter never decreases below zero, because acquire and wait sleep the current fiber when they find the value is 0, until another fiber increments the counter through release or post.
 *
 *  Semaphores are commonly used to limit concurrent use of resources and in producer/consumer pattern applications.
 *
 *  Taking database requests as an example, limiting concurrent use of resources looks like this:
 *  ```JavaScript
 *  var maxconnections = 5;
 *  var l = new coroutine.Semaphore(maxconnections);
 *
 *  ......
 *
 *  l.acquire();
 *  var conn = connectdb()
 *  .....
 *  conn.close();
 *  l.release();
 *  ```
 *
 *  The producer/consumer pattern usually uses a semaphore together with a queue. The producer adds data to the queue and posts a signal, while the consumer waits for the signal first and then queries the queue for data after acquiring the signal.
 *
 *
 */
declare class Class_Semaphore extends Class_Lock {
    /**
     * @description Semaphore constructor
     *      @param value the initial value of the counter
     *
     */
    constructor(value?: number);

    /**
     * @description Waits for a semaphore
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    wait(timeout?: number): boolean;

    wait(timeout?: number, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Waits for a semaphore
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    waitSync(timeout?: number): boolean;

    /**
     * @description Waits for a semaphore
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    waitAsync(timeout?: number): Promise<boolean>;

    /**
     * @description Releases a semaphore, equivalent to release()
     */
    post(): void;

    /**
     * @description Tries to acquire a signal; if it cannot be acquired, returns immediately with false, equivalent to acquire(false)
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    trywait(): boolean;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * The promise variant of the Semaphore class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_SemaphorePromise extends Class_LockPromise {
    /**
     * @description Semaphore constructor
     *      @param value the initial value of the counter
     *
     */
    constructor(value?: number);

    /**
     * @description Waits for a semaphore
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    wait(timeout?: number): Promise<boolean>;

    /**
     * @description Waits for a semaphore
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    waitSync(timeout?: number): boolean;

    /**
     * @description Waits for a semaphore
     *      @param timeout the timeout in milliseconds, default is -1, which means never time out.
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    waitAsync(timeout?: number): Promise<boolean>;

    /**
     * @description Releases a semaphore, equivalent to release()
     */
    post(): void;

    /**
     * @description Tries to acquire a signal; if it cannot be acquired, returns immediately with false, equivalent to acquire(false)
     *      @return returns true if acquired successfully, or false on timeout
     *
     */
    trywait(): boolean;

}


declare namespace Class_Semaphore {
    const promises: FIBJS.GeneralObject;
}
