/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description The timers module schedules callbacks: a one-time or repeated delay, an immediate callback and a function call with a timeout limit; useful for delayed tasks, periodic polling, letting other work run and protecting against blocking code
 *
 *  Main capabilities:
 *
 *  - **Delayed execution**: `setTimeout` runs a callback once after a delay, `clearTimeout` cancels
 *    it;
 *  - **Periodic execution**: `setInterval` runs a callback repeatedly, `clearInterval` cancels it;
 *  - **Immediate execution**: `setImmediate` runs a callback after the current task, `clearImmediate`
 *    cancels it;
 *  - **High-resolution periodic execution**: `setHrInterval`/`clearHrInterval`, a fibjs extension
 *    whose callback can interrupt running JavaScript;
 *  - **Timeout protection**: `call` runs a function and interrupts it when the given time is exceeded.
 *
 *  The scheduling functions are also globals, so `require('timers')` is only needed to use them under
 *  a module name, to reach the fibjs extensions `setHrInterval`, `clearHrInterval` and `call`, or to
 *  make the intent explicit. Promise variants live in the `timers/promises` submodule.
 *
 *  Concepts:
 *
 *  - **Timer objects and process lifetime**: every scheduling function returns a Timer object. A
 *    pending timer keeps the process alive, so a program whose only work is timers exits after they
 *    have fired; `Timer#unref` releases that hold (the callback then runs only if something else keeps
 *    the process alive) and `Timer#ref` acquires it again. A repeating timer is never released
 *    automatically: clear it or unref it, otherwise the process never exits.
 *  - **Callback context**: the callback runs on a fiber scheduled by the fibjs loop, and the extra
 *    arguments are passed through unchanged; `this` inside the callback is the Timer object, so a
 *    timer can clear itself. A callback that waits (I/O, `coroutine.sleep`) yields and lets other
 *    timers run, while a callback that blocks the CPU blocks all JavaScript until it returns.
 *  - **Delay clamping**: the delay is a number of milliseconds; values less than 1 or greater than
 *    2^31-1 are clamped to 1, and a fractional delay is truncated. `setTimeout` defaults a missing
 *    delay to 1ms, while `setInterval` requires it. `NaN` and non-numeric strings throw a TypeError
 *    (Node.js converts them to 1 and prints a warning).
 *  - **High-resolution interval**: `setHrInterval` uses a VM interrupt and can call back at any time,
 *    including while JavaScript is running; its callback must not modify state used by other code nor
 *    call asynchronous APIs. It is a fibjs extension with no Node.js equivalent.
 *  - **Scheduling order**: after the current task, `process.nextTick` callbacks run first, then V8
 *    micro-tasks, then `setImmediate`, then timers; an immediate therefore runs before a
 *    `setTimeout(callback, 0)` timer. See the global object for the fiber and event loop model.
 *  - **Node.js comparison**: Node.js returns `Timeout` and `Immediate` objects with `refresh`,
 *    `hasRef` and `Symbol.dispose`; fibjs returns one Timer class with `ref`, `unref`, `clear` and
 *    `stopped`. All four clear functions accept any Timer object, so they are interchangeable.
 *
 *  Import:
 *  ```JavaScript
 *  const timers = require('timers');
 *  // the same functions are globals: setTimeout, clearTimeout, setInterval, ...
 *  ```
 *
 *  Example 1 — one-time timer with callback arguments and cancellation:
 *  ```JavaScript
 *  const timers = require('timers');
 *
 *  const timer = timers.setTimeout(function (name) {
 *      // `this` is the Timer object and extra arguments are passed through
 *      console.log('fired', name, this.stopped); // fired late false
 *  }, 20, 'late');
 *
 *  // a cleared timer never runs and reports stopped === true
 *  const canceled = timers.setTimeout(() => console.log('never printed'), 20);
 *  timers.clearTimeout(canceled);
 *  console.log(canceled.stopped, timer.stopped); // true false
 *  ```
 *
 *  Example 2 — repeating timer that clears itself, next to an unreferenced timer:
 *  ```JavaScript
 *  const timers = require('timers');
 *
 *  let ticks = 0;
 *  const interval = timers.setInterval(function () {
 *      ticks++;
 *      console.log('tick', ticks);
 *      if (ticks === 3) {
 *          timers.clearInterval(this); // `this` is the interval itself
 *      }
 *  }, 5);
 *
 *  // unref lets the process exit without waiting for this long timer
 *  const idle = timers.setTimeout(() => console.log('printed only if needed'), 5000);
 *  idle.unref();
 *  ```
 *
 *  Example 3 — immediate ordering and a timeout-limited call:
 *  ```JavaScript
 *  const timers = require('timers');
 *
 *  console.log('start');
 *
 *  // an immediate runs after the current task, before the clamped 1ms timer
 *  timers.setImmediate(() => console.log('immediate'));
 *  setTimeout(() => console.log('timeout'), 0);
 *
 *  // call() returns the result of a function that returns in time
 *  console.log('sum', timers.call((a, b) => a + b, 50, 2, 3));
 *
 *  // a function that does not return in time is interrupted with an error
 *  try {
 *      timers.call(() => {
 *          while (true);
 *      }, 20);
 *  } catch (err) {
 *      console.log('interrupted', err.number); // interrupted 20021
 *  }
 *
 *  console.log('end');
 *  ```
 *
 *  Notes:
 *
 *  - `setHrInterval` and `call` are fibjs extensions; Node.js has no equivalent member, and Node.js
 *    `Timeout#refresh`/`hasRef` do not exist on the fibjs Timer.
 *  - `timers/promises` exists with `setTimeout` and `setImmediate` but has no `setInterval` iterator.
 *  - A callback that is not a function throws a TypeError with number 20005; Node.js throws
 *    `ERR_INVALID_ARG_TYPE`.
 *
 */
declare module 'timers' {
    /**
     * @description Executes the callback function after a delay; the timer stops automatically after one execution
     *
     *      The delay is specified in milliseconds and clamped like the module documents: less than 1 or
     *      greater than 2^31-1 becomes 1, a fractional value is truncated, and `NaN` or a non-numeric
     *      string throws a TypeError. The delay defaults to 1ms when it is omitted.
     *
     *      The arguments in args are passed to the callback unchanged, and `this` inside the callback is
     *      the returned Timer object. The timer keeps the process alive until it fires or is cleared; call
     *      `Timer#unref` on the returned object to let the process exit while the timer is pending.
     *
     *      Unlike Node.js there is no `refresh`/`hasRef` on the returned object, and a `NaN` delay is
     *      rejected instead of being treated as 1ms.
     *
     *      Example — the callback receives the extra argument and the timer as `this`:
     *      ```JavaScript
     *      const timers = require('timers');
     *
     *      const timer = timers.setTimeout(function (x) {
     *          console.log(x, this.stopped); // 3 false
     *      }, 5, 3);
     *      ```
     *
     *      @param callback callback function
     *      @param timeout delay time in milliseconds, default 1
     *      @param args additional arguments passed to the callback function, optional
     *      @return returns a timer object
     *
     */
    function setTimeout(callback: (...args: any[])=>void, timeout?: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the specified timer
     *
     *      The parameter can be any value: a Timer returned by `setTimeout`, `setInterval`, `setImmediate`
     *      or `setHrInterval` is cancelled, while a non-timer value is silently ignored. Clearing the
     *      same timer repeatedly does not raise an error, and a timer that already fired is simply left
     *      stopped.
     *
     *      The four clear functions are interchangeable because they all accept any Timer; Node.js instead
     *      routes `clearTimeout` to `Timeout` objects only.
     *
     *      Example — clearing an interval through clearTimeout and ignoring invalid values:
     *      ```JavaScript
     *      const timers = require('timers');
     *
     *      const interval = timers.setInterval(() => console.log('never printed'), 5);
     *      timers.clearTimeout(interval);
     *      console.log(interval.stopped); // true
     *
     *      // non-timer values are silently ignored
     *      timers.clearTimeout(undefined);
     *      timers.clearTimeout({});
     *      ```
     *
     *      @param t the timer to clear
     *
     */
    function clearTimeout(t: any): void;

    /**
     * @description Executes the callback function periodically at a fixed interval
     *
     *      The delay is required and is clamped like setTimeout; unlike Node.js there is no 1ms default,
     *      and omitting it throws a TypeError (parameter not optional). The next run is scheduled timeout
     *      milliseconds after the previous callback returns, so a slow callback reduces the frequency.
     *
     *      The timer keeps running until `clearInterval` or `Timer#clear` is called, or it clears itself
     *      inside the callback through `this`. The arguments in args are passed to every run, and a
     *      repeating timer keeps the process alive forever unless it is cleared or unreferenced.
     *
     *      @param callback callback function
     *      @param timeout interval time in milliseconds
     *      @param args additional arguments passed to the callback function, optional
     *      @return returns a timer object
     *
     */
    function setInterval(callback: (...args: any[])=>void, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the specified timer
     *
     *      Identical to `clearTimeout`: it accepts any Timer object, so an interval can also be cleared
     *      through `clearTimeout` and a one-time timer through `clearInterval`, and non-timer values are
     *      silently ignored. Clearing is idempotent and has no effect on a timer that already fired.
     *
     *      @param t the timer to clear
     *
     */
    function clearInterval(t: any): void;

    /**
     * @description High-precision timer that executes the callback function periodically at a fixed interval
     *
     *      Unlike setInterval, the callback of a high-precision timer does not depend on event loop
     *      scheduling; it can interrupt the running script at any time and has higher time precision. The
     *      delay is required and is clamped like setTimeout; the arguments in args are passed to every
     *      run.
     *
     *      Since the callback may be inserted at any time, it should not modify data that may affect other
     *      modules, nor call any asynchronous API, otherwise unpredictable results may occur. For example:
     *
     *      ```JavaScript
     *      var timers = require('timers');
     *
     *      var n = 0;
     *      var t = timers.setHrInterval(() => n++, 10);
     *
     *      // Busy-wait for 50ms; the callback fires several times while this loop is running
     *      var end = Date.now() + 50;
     *      while (Date.now() < end);
     *
     *      timers.clearHrInterval(t);
     *      console.log(n);
     *      ```
     *
     *      Note that the just-in-time compiler may optimize the loop condition, so modifications to
     *      variables in the callback may not be observed by the loop; for example, `while (n < 10)` will
     *      not exit because the callback modified n.
     *
     *      @param callback callback function
     *      @param timeout interval time in milliseconds
     *      @param args additional arguments passed to the callback function, optional
     *      @return returns a timer object
     *
     */
    function setHrInterval(callback: (...args: any[])=>void, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the specified timer
     *
     *      Identical to `clearTimeout`: it accepts any Timer object, so a high-resolution interval can be
     *      cleared through any of the clear functions and non-timer values are silently ignored. Since the
     *      callback runs from a VM interrupt, clearing from another fiber stops further interruptions.
     *
     *      @param t the timer to clear
     *
     */
    function clearHrInterval(t: any): void;

    /**
     * @description Executes the callback function after the current synchronous code finishes executing
     *
     *      An immediate runs after the current task and after V8 micro-tasks, but before timers that wait
     *      for a delay. There is no delay parameter and the timer fires exactly once, so the returned
     *      Timer is only useful to cancel it or to release its keep-alive hold with `Timer#unref`.
     *
     *      The arguments in args are passed to the callback unchanged and `this` is the returned Timer
     *      object. Node.js returns an `Immediate` object without a `stopped` property.
     *
     *      Example — immediates run after the current task and before 1ms timers:
     *      ```JavaScript
     *      const timers = require('timers');
     *
     *      console.log('start');
     *
     *      const immediate = timers.setImmediate((label) => console.log(label), 'immediate');
     *      timers.setTimeout(() => console.log('timeout'), 0);
     *
     *      // a canceled immediate never runs
     *      const canceled = timers.setImmediate(() => console.log('never printed'));
     *      timers.clearImmediate(canceled);
     *      ```
     *
     *      @param callback callback function
     *      @param args additional arguments passed to the callback function, optional
     *      @return returns a timer object
     *
     */
    function setImmediate(callback: (...args: any[])=>void, ...args: any[]): Class_Timer;

    /**
     * @description Clears the specified timer
     *
     *      Identical to `clearTimeout`: it accepts any Timer object, so an immediate can be cleared
     *      through any of the clear functions, and non-timer values are silently ignored. A canceled
     *      immediate never calls its callback and reports `stopped === true`.
     *
     *      @param t the timer to clear
     *
     */
    function clearImmediate(t: any): void;

    /**
     * @description Calls a function within the specified time; if the function does not return in time, the execution is interrupted and an exception is thrown
     *
     *      timeout is in milliseconds and clamped like setTimeout (less than 1 or greater than 2^31-1
     *      becomes 1). The arguments in args are passed to func unchanged and its return value is returned
     *      to the caller. When the time is exceeded, the running JavaScript is interrupted and an Error
     *      with number 20021 is thrown at the call site; the interrupted function cannot catch it, and
     *      execution continues normally after the call. An error thrown by func itself is propagated
     *      unchanged.
     *
     *      This is a fibjs extension: Node.js has no equivalent member, only the promise-based timeouts of
     *      `timers/promises`, which do not interrupt running code.
     *
     *      Example — normal return and interruption:
     *      ```JavaScript
     *      const timers = require('timers');
     *
     *      // the function returns its result normally
     *      const r = timers.call((a, b) => a + b, 100, 3, 4);
     *      console.log(r); // 7
     *
     *      // a function that does not return in time is interrupted
     *      try {
     *          timers.call(() => {
     *              while (true);
     *          }, 30);
     *      } catch (e) {
     *          console.error(e.number); // 20021
     *      }
     *      ```
     *
     *      @param func the function to call
     *      @param timeout timeout in milliseconds
     *      @param args additional arguments passed to the function, optional
     *      @return returns the execution result of the function
     *
     */
    function call(func: (...args: any[])=>any, timeout: number, ...args: any[]): any;

}

