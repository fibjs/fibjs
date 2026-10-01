/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description The timers module provides timer scheduling capabilities, including delayed execution, periodic execution, idle execution and function calls with a timeout limit, and can be used for delayed tasks, periodic polling, avoiding blocking and timeout protection and other scenarios
 *
 *  The timer functions provide the following capabilities:
 *
 *  - `setTimeout`: executes a callback function once after the specified delay;
 *  - `setInterval`: executes a callback function periodically at a fixed interval;
 *  - `setHrInterval`: high-precision periodic execution; the callback can interrupt the running script;
 *  - `setImmediate`: executes a callback function after the current synchronous code finishes executing;
 *  - `call`: calls a function within the specified time; if it does not return in time, the execution is interrupted and an exception is thrown.
 *
 *  All timer functions in the module are global functions; the global `setTimeout`, `setInterval` and `setImmediate` behave the same as the functions with the same names in this module and can be called directly without requiring the module.
 *
 *  All timer functions return a Timer object, through which the timer lifecycle is controlled:
 *
 *  - after the timer finishes executing or is cleared, its `stopped` property is `true`;
 *  - the `this` in the callback function points to the current timer object, so the callback can clear itself directly;
 *  - by default a timer prevents the fibjs process from exiting; after calling `Timer.unref()`, the process can exit normally while the timer is waiting;
 *  - the clear functions accept any value; non-timer objects are silently ignored, and clearing the same timer repeatedly does not raise an error.
 *
 *  The timeout parameter of delay and periodic functions is in milliseconds; values less than 1 or greater than 2^31-1 (about 24.8 days) are treated as 1ms.
 *
 *  Example:
 *
 *  ```JavaScript
 *  var timers = require('timers');
 *
 *  // Execute once after a delay; timeout defaults to 1ms
 *  timers.setTimeout(() => {
 *      console.log('timeout');
 *  }, 1000);
 *
 *  // Periodic execution; clear itself inside the callback
 *  var intervalId = timers.setInterval(function () {
 *      console.log('tick');
 *      timers.clearInterval(this);
 *  }, 500);
 *
 *  // Idle execution: runs immediately after the current synchronous code finishes, without blocking
 *  timers.setImmediate(() => console.log('immediate'));
 *
 *  // Function call with a timeout; returns the function execution result
 *  var r = timers.call((a, b) => a + b, 100, 3, 4);
 *  console.log(r); // 7
 *  ```
 *
 *  The timers created in the above examples keep the process running; the process exits only after all timers have finished or been cleared.
 *
 */
declare module 'timers' {
    /**
     * @description Executes the callback function after a delay; the timer stops automatically after one execution
     *
     *      timeout defaults to 1ms; values less than 1 or greater than 2^31-1 are treated as 1ms. The arguments in args are passed to the callback function unchanged.
     *      @param callback callback function
     *      @param timeout delay time in milliseconds, default 1
     *      @param args additional arguments passed to the callback function, optional
     *      @return returns a timer object
     *
     */
    function setTimeout(callback: (...args: any[])=>any, timeout?: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the specified timer
     *
     *      The parameter can be any value; non-timer objects are silently ignored; clearing the same timer repeatedly does not raise an error.
     *      @param t the timer to clear
     *
     */
    function clearTimeout(t: any): void;

    /**
     * @description Executes the callback function periodically at a fixed interval
     *
     *      The timeout value rules are the same as setTimeout. The timer keeps running until clearInterval is called, or it clears itself inside the callback.
     *      @param callback callback function
     *      @param timeout interval time in milliseconds
     *      @param args additional arguments passed to the callback function, optional
     *      @return returns a timer object
     *
     */
    function setInterval(callback: (...args: any[])=>any, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the specified timer
     *
     *      The parameter can be any value; non-timer objects are silently ignored; clearing the same timer repeatedly does not raise an error.
     *      @param t the timer to clear
     *
     */
    function clearInterval(t: any): void;

    /**
     * @description High-precision timer that executes the callback function periodically at a fixed interval
     *
     *      Unlike setInterval, the callback of a high-precision timer does not depend on event loop scheduling; it can interrupt the running script at any time and has higher time precision.
     *
     *      Since the callback may be inserted at any time, it should not modify data that may affect other modules, nor call any asynchronous API, otherwise unpredictable results may occur. For example:
     *
     *      ```JavaScript
     *      var timers = require('timers');
     *
     *      var n = 0;
     *      var t = timers.setHrInterval(() => n++, 100);
     *
     *      // Busy-wait for 50ms; the callback still fires at the interval during this time
     *      var end = Date.now() + 50;
     *      while (Date.now() < end);
     *
     *      timers.clearHrInterval(t);
     *      console.log(n);
     *      ```
     *
     *      Note that the just-in-time compiler may optimize the loop condition, so modifications to variables in the callback may not be observed by the loop; for example, `while (n < 10)` will not exit because the callback modified n.
     *      @param callback callback function
     *      @param timeout interval time in milliseconds
     *      @param args additional arguments passed to the callback function, optional
     *      @return returns a timer object
     *
     */
    function setHrInterval(callback: (...args: any[])=>any, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the specified timer
     *
     *      The parameter can be any value; non-timer objects are silently ignored; clearing the same timer repeatedly does not raise an error.
     *      @param t the timer to clear
     *
     */
    function clearHrInterval(t: any): void;

    /**
     * @description Executes the callback function after the current synchronous code finishes executing
     *
     *      The arguments in args are passed to the callback function unchanged.
     *      @param callback callback function
     *      @param args additional arguments passed to the callback function, optional
     *      @return returns a timer object
     *
     */
    function setImmediate(callback: (...args: any[])=>any, ...args: any[]): Class_Timer;

    /**
     * @description Clears the specified timer
     *
     *      The parameter can be any value; non-timer objects are silently ignored; clearing the same timer repeatedly does not raise an error.
     *      @param t the timer to clear
     *
     */
    function clearImmediate(t: any): void;

    /**
     * @description Calls a function within the specified time; if the function does not return in time, the execution is interrupted and an exception is thrown
     *
     *      timeout is in milliseconds; values less than 1 or greater than 2^31-1 are treated as 1ms. If the function returns normally, its execution result is returned; the arguments in args are passed to the function unchanged.
     *
     *      ```JavaScript
     *      var timers = require('timers');
     *
     *      // The function returns its result normally
     *      var r = timers.call((a, b) => a + b, 100, 3, 4);
     *
     *      // Timeout throws an exception
     *      try {
     *          timers.call(() => {
     *              while (true);
     *          }, 30);
     *      } catch (e) {
     *          console.error(e);
     *      }
     *      ```
     *      @param func the function to call
     *      @param timeout timeout in milliseconds
     *      @param args additional arguments passed to the function, optional
     *      @return returns the execution result of the function
     *
     */
    function call(func: (...args: any[])=>any, timeout: number, ...args: any[]): any;

}

