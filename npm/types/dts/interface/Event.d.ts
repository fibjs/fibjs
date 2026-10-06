/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * @description Event is the fiber-level event primitive of the coroutine module: a broadcast gate that
 *  suspends the calling fiber until the event is set, wakes every waiter at once and keeps its
 *  state readable
 *
 *  A fiber calls wait() to suspend until the event is set; another fiber calls set() to set the
 *  flag and wake all waiters, or pulse() to wake all waiters without changing the flag. clear()
 *  resets the flag. Since wait() returns immediately once the flag is set, an Event acts as a
 *  one-way gate rather than a counting semaphore: it synchronizes fibers, for example as a start
 *  or completion gate, instead of limiting concurrency.
 *
 *  Concepts:
 *
 *  - **Fiber scheduling**: wait() suspends only the current fiber, not the thread or the process;
 *    the runtime keeps scheduling other fibers, so create the parallel work with coroutine.start
 *    and see the coroutine module for the fiber model.
 *  - **Value semantics**: the event holds a boolean flag, false by default and true when the
 *    constructor receives a truthy value. set() sets the flag and wakes all waiters; pulse() wakes
 *    all waiters but leaves the flag unchanged, so a later wait() blocks again unless the event
 *    was set; clear() resets the flag.
 *  - **Broadcast, not a queue**: waiters are not queued or consumed; every fiber that waits on a set
 *    event returns immediately, and every fiber waiting when set()/pulse() is called is woken.
 *  - **Lock relationship**: Event derives from Lock, so it also provides acquire(blocking),
 *    release() and count(). release() is set(); acquire(false) is a non-blocking state check that
 *    returns the flag value; acquire() waits like wait(). Because release() leaves the event set,
 *    an Event does not provide mutual exclusion — use coroutine.Lock or coroutine.Semaphore for
 *    that.
 *  - **Node.js / MDN**: Node.js has no direct equivalent (the closest shapes are an inverted
 *    Semaphore or Atomics.wait); the global DOM-style `Event` class of fibjs is DOMEvent and is
 *    unrelated to this class.
 *
 *  Obtained from:
 *  - `new coroutine.Event(value = false)` — creates the event, already set when value is truthy.
 *
 *  Example 1 — a start gate that holds a worker fiber until the main fiber is ready:
 *  ```JavaScript
 *  const coroutine = require('coroutine');
 *
 *  const started = new coroutine.Event();
 *
 *  coroutine.start(() => {
 *      console.log('worker waiting'); // printed first
 *      started.wait();
 *      console.log('worker running'); // printed after main sets the event
 *  });
 *
 *  coroutine.sleep(10);
 *  console.log('main sets the event');
 *  started.set();
 *  coroutine.sleep(10);
 *  ```
 *
 *  Example 2 — pulse wakes every waiter without setting the flag:
 *  ```JavaScript
 *  const coroutine = require('coroutine');
 *
 *  const gate = new coroutine.Event();
 *  let done = 0;
 *
 *  for (let i = 0; i < 3; i++)
 *      coroutine.start(() => {
 *          gate.wait();
 *          done++;
 *      });
 *
 *  coroutine.sleep(10);
 *  console.log(gate.count()); // 3 waiting fibers, count() is inherited from Lock
 *  gate.pulse();
 *  coroutine.sleep(10);
 *  console.log(done); // 3
 *  console.log(gate.isSet()); // false, pulse does not set the flag
 *  ```
 *
 *  Example 3 — set/clear gate and wait on a set event:
 *  ```JavaScript
 *  const coroutine = require('coroutine');
 *
 *  const ready = new coroutine.Event(true);
 *  console.log(ready.isSet()); // true
 *  ready.wait(); // returns immediately, no fiber is suspended
 *  console.log('not blocked');
 *
 *  ready.clear();
 *  let resumed = false;
 *  coroutine.start(() => {
 *      ready.wait();
 *      resumed = true;
 *  });
 *
 *  coroutine.sleep(10);
 *  console.log(resumed); // false
 *  ready.set();
 *  coroutine.sleep(10);
 *  console.log(resumed); // true
 *  ```
 *
 */
declare class Class_Event extends Class_Lock {
    /**
     * @description Creates an event object
     *
     *      The value is converted to boolean: when it is truthy the event is created already set, so
     *      the first wait() returns immediately. The default is an unset event.
     *
     *      @param value initial state; the event is created set when true, default is false
     *
     */
    constructor(value?: boolean);

    /**
     * @description Returns the current state of the event
     *
     *      The state is the flag set by set() and cleared by clear(); pulse() does not change it. A
     *      newly created Event is unset unless the constructor received a truthy value.
     *
     *      @return returns true if the event is set
     *
     */
    isSet(): boolean;

    /**
     * @description Sets the event and wakes every waiting fiber
     *
     *      The state becomes true and all fibers blocked in wait() resume; later wait() calls return
     *      immediately until clear() is called. Calling set() on an already set event only wakes the
     *      fibers waiting at that moment. The inherited Lock#release() performs the same operation.
     *
     */
    set(): void;

    /**
     * @description Wakes every waiting fiber without changing the event state
     *
     *      All fibers blocked in wait() resume, but the flag stays as it was: a fiber that calls wait()
     *      again after a pulse() on an unset event blocks again. Calling pulse() with no waiter is a
     *      no-op.
     *
     *      Example — pulse() wakes all waiters but leaves the event unset:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const event = new coroutine.Event();
     *      let waiters = 0;
     *
     *      coroutine.start(() => { event.wait(); waiters++; });
     *      coroutine.start(() => { event.wait(); waiters++; });
     *      coroutine.sleep(10);
     *
     *      event.pulse();
     *      coroutine.sleep(10);
     *      console.log(waiters); // 2
     *      console.log(event.isSet()); // false
     *      ```
     *
     */
    pulse(): void;

    /**
     * @description Clears the event
     *
     *      The state becomes false, so later wait() calls block again until set() or pulse() is called.
     *      Fibers already waiting are not affected and keep waiting. clear() on an unset event is a
     *      no-op.
     *
     */
    clear(): void;

    /**
     * @description Waits for the event to be set or pulsed
     *
     *      If the event is already set the call returns immediately without suspending the fiber;
     *      otherwise the current fiber is suspended until set() or pulse() is called by another fiber.
     *      The state is not consumed: every waiting fiber resumes and a fiber that waits again is
     *      blocked or not according to the state at that moment. The generated waitAsync() provides the
     *      Promise form; from a context that cannot suspend the current fiber, the call fails.
     *
     *      Example — suspend a worker fiber until another fiber signals it:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const event = new coroutine.Event();
     *      let resumed = false;
     *
     *      coroutine.start(() => {
     *          event.wait();
     *          resumed = true;
     *      });
     *
     *      coroutine.sleep(10);
     *      console.log(resumed); // false
     *      event.set();
     *      coroutine.sleep(10);
     *      console.log(resumed); // true
     *      ```
     *
     */
    wait(): void;

    wait(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Waits for the event to be set or pulsed
     *
     *      If the event is already set the call returns immediately without suspending the fiber;
     *      otherwise the current fiber is suspended until set() or pulse() is called by another fiber.
     *      The state is not consumed: every waiting fiber resumes and a fiber that waits again is
     *      blocked or not according to the state at that moment. The generated waitAsync() provides the
     *      Promise form; from a context that cannot suspend the current fiber, the call fails.
     *
     *      Example — suspend a worker fiber until another fiber signals it:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const event = new coroutine.Event();
     *      let resumed = false;
     *
     *      coroutine.start(() => {
     *          event.wait();
     *          resumed = true;
     *      });
     *
     *      coroutine.sleep(10);
     *      console.log(resumed); // false
     *      event.set();
     *      coroutine.sleep(10);
     *      console.log(resumed); // true
     *      ```
     *
     */
    waitSync(): void;

    /**
     * @description Waits for the event to be set or pulsed
     *
     *      If the event is already set the call returns immediately without suspending the fiber;
     *      otherwise the current fiber is suspended until set() or pulse() is called by another fiber.
     *      The state is not consumed: every waiting fiber resumes and a fiber that waits again is
     *      blocked or not according to the state at that moment. The generated waitAsync() provides the
     *      Promise form; from a context that cannot suspend the current fiber, the call fails.
     *
     *      Example — suspend a worker fiber until another fiber signals it:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const event = new coroutine.Event();
     *      let resumed = false;
     *
     *      coroutine.start(() => {
     *          event.wait();
     *          resumed = true;
     *      });
     *
     *      coroutine.sleep(10);
     *      console.log(resumed); // false
     *      event.set();
     *      coroutine.sleep(10);
     *      console.log(resumed); // true
     *      ```
     *
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
     * @description Creates an event object
     *
     *      The value is converted to boolean: when it is truthy the event is created already set, so
     *      the first wait() returns immediately. The default is an unset event.
     *
     *      @param value initial state; the event is created set when true, default is false
     *
     */
    constructor(value?: boolean);

    /**
     * @description Returns the current state of the event
     *
     *      The state is the flag set by set() and cleared by clear(); pulse() does not change it. A
     *      newly created Event is unset unless the constructor received a truthy value.
     *
     *      @return returns true if the event is set
     *
     */
    isSet(): boolean;

    /**
     * @description Sets the event and wakes every waiting fiber
     *
     *      The state becomes true and all fibers blocked in wait() resume; later wait() calls return
     *      immediately until clear() is called. Calling set() on an already set event only wakes the
     *      fibers waiting at that moment. The inherited Lock#release() performs the same operation.
     *
     */
    set(): void;

    /**
     * @description Wakes every waiting fiber without changing the event state
     *
     *      All fibers blocked in wait() resume, but the flag stays as it was: a fiber that calls wait()
     *      again after a pulse() on an unset event blocks again. Calling pulse() with no waiter is a
     *      no-op.
     *
     *      Example — pulse() wakes all waiters but leaves the event unset:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const event = new coroutine.Event();
     *      let waiters = 0;
     *
     *      coroutine.start(() => { event.wait(); waiters++; });
     *      coroutine.start(() => { event.wait(); waiters++; });
     *      coroutine.sleep(10);
     *
     *      event.pulse();
     *      coroutine.sleep(10);
     *      console.log(waiters); // 2
     *      console.log(event.isSet()); // false
     *      ```
     *
     */
    pulse(): void;

    /**
     * @description Clears the event
     *
     *      The state becomes false, so later wait() calls block again until set() or pulse() is called.
     *      Fibers already waiting are not affected and keep waiting. clear() on an unset event is a
     *      no-op.
     *
     */
    clear(): void;

    /**
     * @description Waits for the event to be set or pulsed
     *
     *      If the event is already set the call returns immediately without suspending the fiber;
     *      otherwise the current fiber is suspended until set() or pulse() is called by another fiber.
     *      The state is not consumed: every waiting fiber resumes and a fiber that waits again is
     *      blocked or not according to the state at that moment. The generated waitAsync() provides the
     *      Promise form; from a context that cannot suspend the current fiber, the call fails.
     *
     *      Example — suspend a worker fiber until another fiber signals it:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const event = new coroutine.Event();
     *      let resumed = false;
     *
     *      coroutine.start(() => {
     *          event.wait();
     *          resumed = true;
     *      });
     *
     *      coroutine.sleep(10);
     *      console.log(resumed); // false
     *      event.set();
     *      coroutine.sleep(10);
     *      console.log(resumed); // true
     *      ```
     *
     */
    wait(): Promise<void>;

    /**
     * @description Waits for the event to be set or pulsed
     *
     *      If the event is already set the call returns immediately without suspending the fiber;
     *      otherwise the current fiber is suspended until set() or pulse() is called by another fiber.
     *      The state is not consumed: every waiting fiber resumes and a fiber that waits again is
     *      blocked or not according to the state at that moment. The generated waitAsync() provides the
     *      Promise form; from a context that cannot suspend the current fiber, the call fails.
     *
     *      Example — suspend a worker fiber until another fiber signals it:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const event = new coroutine.Event();
     *      let resumed = false;
     *
     *      coroutine.start(() => {
     *          event.wait();
     *          resumed = true;
     *      });
     *
     *      coroutine.sleep(10);
     *      console.log(resumed); // false
     *      event.set();
     *      coroutine.sleep(10);
     *      console.log(resumed); // true
     *      ```
     *
     */
    waitSync(): void;

    /**
     * @description Waits for the event to be set or pulsed
     *
     *      If the event is already set the call returns immediately without suspending the fiber;
     *      otherwise the current fiber is suspended until set() or pulse() is called by another fiber.
     *      The state is not consumed: every waiting fiber resumes and a fiber that waits again is
     *      blocked or not according to the state at that moment. The generated waitAsync() provides the
     *      Promise form; from a context that cannot suspend the current fiber, the call fails.
     *
     *      Example — suspend a worker fiber until another fiber signals it:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const event = new coroutine.Event();
     *      let resumed = false;
     *
     *      coroutine.start(() => {
     *          event.wait();
     *          resumed = true;
     *      });
     *
     *      coroutine.sleep(10);
     *      console.log(resumed); // false
     *      event.set();
     *      coroutine.sleep(10);
     *      console.log(resumed); // true
     *      ```
     *
     */
    waitAsync(): Promise<void>;

}


declare namespace Class_Event {
    const promises: FIBJS.GeneralObject;
}
