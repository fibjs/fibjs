/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description timers 模块是一个内置模块，提供了一些与时间相关的函数和对象，包括 setTimeout()、setInterval()、clearInterval()、clearTimeout() 等等
 * 
 * setInterval() 函数每隔指定的时间后调用函数，返回定时器对象。
 * 
 * ```JavaScript
 * var timers = require('timers');
 * function callback() {
 *     console.log('Hello, World!');
 * }
 * var intervalId = timers.setInterval(callback, 1000);
 * ```
 * 
 * setTimeout() 函数在指定的时间后调用函数，返回定时器对象。
 * 
 * ```JavaScript
 * var timers = require('timers');
 * function callback() {
 *     console.log('Hello, World!');
 * }
 * var timeoutId = timers.setTimeout(callback, 1000);
 * ```
 * 
 * clearTimeout() 函数用于取消一个由 setTimeout() 方法设置的定时器。需要提供的是在 setTimeout() 中返回的定时器标识符值。
 * 
 * ```JavaScript
 * var timers = require('timers');
 * function callback() {
 *     console.log('Hello, World!');
 * }
 * var timeoutId = timers.setTimeout(callback, 1000);
 * timers.clearTimeout(timeoutId);
 * ```
 * 
 * clearInterval() 函数用于取消一个由 setInterval() 方法设置的定时器。需要提供的是在 setInterval() 中返回的定时器标识符值。
 * 
 * ```JavaScript
 * var timers = require('timers');
 * function callback() {
 *     console.log('Hello, World!');
 * }
 * var intervalId = timers.setInterval(callback, 1000);
 * timers.clearInterval(intervalId);
 * ```
 * 
 * setImmediate() 函数用于在下一个闲置时间立即调用函数，返回定时器对象。
 * 
 * ```JavaScript
 * var timers = require('timers');
 * function callback() {
 *     console.log('Hello, World!');
 * }
 * var immediateId = timers.setImmediate(callback);
 * ```
 * 
 * 以上都是 timers 模块提供的常用函数和对象示例，通过使用这些函数和对象，可以方便的在应用程序中实现延迟执行任务或调度任务执行。
 *  
 */
declare module 'timers' {
    /**
     * @description 在指定的时间后调用函数
     *     @param callback 指定回调函数
     *     @param timeout 指定延时的时间，以毫秒为单位。超过 2^31 的话,立即执行。
     *     @param args 额外的参数，传入到指定的 callback 内，可选。
     *     @return 返回定时器对象
     *     
     */
    function setTimeout(callback: (...args: any[])=>any, timeout?: number, ...args: any[]): Class_Timer;

    /**
     * @description 清除指定的定时器
     *      @param t 指定要清除的定时器
     *     
     */
    function clearTimeout(t: any): void;

    /**
     * @description 每间隔指定的时间后调用函数
     *      @param callback 指定回调函数
     *      @param timeout 指定间隔的时间，以毫秒为单位。超过 2^31 的话,立即执行。
     *      @param args 额外的参数，传入到指定的 callback 内，可选。
     *      @return 返回定时器对象
     *     
     */
    function setInterval(callback: (...args: any[])=>any, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description 清除指定的定时器
     *      @param t 指定要清除的定时器
     *     
     */
    function clearInterval(t: any): void;

    /**
     * @description 每间隔指定的时间后调用函数，这是个高精度定时器，会主动打断正在运行的 JavaScript 脚本执行定时器
     *      由于 setHrInterval 的定时器会中断正在运行的代码执行回调，因此不要在回调函数内修改可能影响其它模块的数据，或者在回调中调用任何标记为 async 的 api 函数，否则将会产生不可预知的结果。例如：
     *      ```JavaScript
     *         var timers = require('timers');
     * 
     *         var cnt = 0;
     *         timers.setHrInterval(() => {
     *             cnt++;
     *         }, 100);
     * 
     *         while (cnt < 10);
     * 
     *         console.error("===============================> done");
     *      ```
     *      这段代码中，第 8 行的循环并不会因为 cnt 的改变而结束，因为 JavaScript 在优化代码时会认定在这个循环过程中 cnt 不会被改变。
     *      @param callback 指定回调函数
     *      @param timeout 指定间隔的时间，以毫秒为单位。超过 2^31 的话,立即执行。
     *      @param args 额外的参数，传入到指定的 callback 内，可选。
     *      @return 返回定时器对象
     *     
     */
    function setHrInterval(callback: (...args: any[])=>any, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description 清除指定的定时器
     *      @param t 指定要清除的定时器
     *     
     */
    function clearHrInterval(t: any): void;

    /**
     * @description 下一个空闲时间立即执行回调函数
     *      @param callback 指定回调函数
     *      @param args 额外的参数，传入到指定的 callback 内，可选。
     *      @return 返回定时器对象
     *     
     */
    function setImmediate(callback: (...args: any[])=>any, ...args: any[]): Class_Timer;

    /**
     * @description 清除指定的定时器
     *      @param t 指定要清除的定时器
     *     
     */
    function clearImmediate(t: any): void;

    /**
     * @description 调用给定的函数，并在超时时间到期时中断函数运行
     *      @param func 指定要运行的函数
     *      @param timeout 指定超时时间
     *      @param args 额外的参数，传入到指定的 callback 内，可选。
     *      @return 返回 func 的运行结果
     *     
     */
    function call(func: (...args: any[])=>any, timeout: number, ...args: any[]): any;

}

