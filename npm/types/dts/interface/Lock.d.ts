/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Lock is a built-in object that can be used to control concurrent access from fibers; one fiber can acquire the lock to prevent other fibers from acquiring it at the same time. A Lock can be created with the coroutine.Lock() function
 *
 * A common case is that in a multi-threaded scenario, when multiple threads want to modify the same data, data inconsistency occurs. For example, if two threads both want to modify the same value in the same data, improper control may lead to inconsistent results. In this case, using a Lock object achieves mutually exclusive access to the same data.
 *
 * The following is a simple example that uses Lock to make two fibers execute alternately, where the value of the shared variable v is not 300.
 *
 * ```JavaScript
 * var coroutine = require("coroutine")
 *
 * var l = new coroutine.Lock()
 * var v = 100
 * function f() {
 *     l.acquire()
 *     v = 200
 *     coroutine.sleep(1)
 *     v = 300
 *     l.release()
 * }
 * coroutine.start(f)
 *
 * coroutine.sleep(1)
 *
 * l.acquire()
 * assert.notEqual(300, v)
 * assert.equal(200, v)
 * l.release()
 * ```
 *
 * First a Lock object is created, then fiber f is entered, which acquires the lock, modifies variable v, and then releases the lock. In the main thread, fiber f is waited for first... After fiber f releases the Lock, the main thread starts to acquire the Lock, ensuring that the value of variable v has been changed to 300.
 *
 */
declare class Class_Lock extends Class_object {
    /**
     * @description Constructor
     */
    constructor();

    /**
     * @description Acquires ownership of the lock
     *
     *      The acquire method acquires ownership of the lock; when the lock is available, this method immediately returns true.
     *
     *      When the lock is unavailable and blocking is true, the current fiber sleeps; after another fiber releases the lock, this method returns true.
     *
     *      When the lock is unavailable and blocking is false, the method returns false.
     *      @param blocking whether to wait; waits when true, default is true
     *      @return returns whether the lock was successfully acquired; true means acquired successfully
     *
     */
    acquire(blocking?: boolean): boolean;

    acquire(blocking?: boolean, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Acquires ownership of the lock
     *
     *      The acquire method acquires ownership of the lock; when the lock is available, this method immediately returns true.
     *
     *      When the lock is unavailable and blocking is true, the current fiber sleeps; after another fiber releases the lock, this method returns true.
     *
     *      When the lock is unavailable and blocking is false, the method returns false.
     *      @param blocking whether to wait; waits when true, default is true
     *      @return returns whether the lock was successfully acquired; true means acquired successfully
     *
     */
    acquireSync(blocking?: boolean): boolean;

    /**
     * @description Acquires ownership of the lock
     *
     *      The acquire method acquires ownership of the lock; when the lock is available, this method immediately returns true.
     *
     *      When the lock is unavailable and blocking is true, the current fiber sleeps; after another fiber releases the lock, this method returns true.
     *
     *      When the lock is unavailable and blocking is false, the method returns false.
     *      @param blocking whether to wait; waits when true, default is true
     *      @return returns whether the lock was successfully acquired; true means acquired successfully
     *
     */
    acquireAsync(blocking?: boolean): Promise<boolean>;

    /**
     * @description Releases ownership of the lock
     *
     *      This method releases ownership of the lock; if the current fiber does not own the lock, this method throws an error.
     *
     */
    release(): void;

    /**
     * @description Queries the number of currently waiting tasks
     *      @return returns the number of tasks
     *
     */
    count(): number;

}

