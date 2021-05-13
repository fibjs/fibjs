/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description timers 模块 
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

