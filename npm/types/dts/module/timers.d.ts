/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description timers 模块提供定时任务调度能力，包括延时执行、周期执行、空闲执行与带超时限制的函数调用，可用于延迟任务、周期轮询、避免阻塞与超时保护等场景
 * 
 *  定时函数提供以下能力：
 * 
 *  - `setTimeout`：在指定延时后执行一次回调函数；
 *  - `setInterval`：按固定间隔周期执行回调函数；
 *  - `setHrInterval`：高精度周期执行，回调可打断正在运行的脚本；
 *  - `setImmediate`：在当前同步代码执行完毕后执行回调函数；
 *  - `call`：在指定时间内调用函数，超时未返回时中断执行并抛出异常。
 * 
 *  模块内所有定时函数均为全局函数，全局 `setTimeout`、`setInterval`、`setImmediate` 与模块内同名函数行为一致，可直接调用，无需引入模块。
 * 
 *  定时函数均返回 Timer 对象，通过该对象控制定时器生命周期：
 * 
 *  - 定时器执行完毕或被清除后，其 `stopped` 属性为 `true`；
 *  - 回调函数中的 `this` 指向当前定时器对象，可在回调内直接清除自身；
 *  - 定时器默认会阻止 fibjs 进程退出；调用 `Timer.unref()` 后，进程可在定时器等待期间正常退出；
 *  - 清除函数接受任意值，非定时器对象时静默忽略，重复清除同一定时器不会产生错误。
 * 
 *  延时与周期函数的 timeout 参数以毫秒为单位，小于 1 或大于 2^31-1（约 24.8 天）的值按 1ms 处理。
 * 
 *  示例：
 * 
 *  ```JavaScript
 *  var timers = require('timers');
 * 
 *  // 延时执行一次，timeout 缺省为 1ms
 *  timers.setTimeout(() => {
 *      console.log('timeout');
 *  }, 1000);
 * 
 *  // 周期执行，在回调内清除自身
 *  var intervalId = timers.setInterval(function () {
 *      console.log('tick');
 *      timers.clearInterval(this);
 *  }, 500);
 * 
 *  // 空闲执行：当前同步代码执行完毕后立即执行，不会阻塞
 *  timers.setImmediate(() => console.log('immediate'));
 * 
 *  // 带超时的函数调用，返回函数执行结果
 *  var r = timers.call((a, b) => a + b, 100, 3, 4);
 *  console.log(r); // 7
 *  ```
 * 
 *  上述示例中创建的定时器会保持进程运行，定时器全部结束或清除后进程才会退出。
 *  
 */
declare module 'timers' {
    /**
     * @description 延时执行回调函数，定时器执行一次后自动停止
     * 
     *      timeout 缺省为 1ms，小于 1 或大于 2^31-1 的值按 1ms 处理。args 中的参数将原样传递给回调函数。
     *      @param callback 回调函数
     *      @param timeout 延时时间，以毫秒为单位，缺省为 1
     *      @param args 传递给回调函数的额外参数，可选
     *      @return 返回定时器对象
     *      
     */
    function setTimeout(callback: (...args: any[])=>any, timeout?: number, ...args: any[]): Class_Timer;

    /**
     * @description 清除指定的定时器
     * 
     *      参数可以是任意值，非定时器对象时静默忽略；重复清除同一定时器不会产生错误。
     *      @param t 指定要清除的定时器
     *     
     */
    function clearTimeout(t: any): void;

    /**
     * @description 按固定间隔周期执行回调函数
     * 
     *      timeout 取值规则与 setTimeout 相同。定时器将持续运行，直到调用 clearInterval 清除，或在回调内清除自身。
     *      @param callback 回调函数
     *      @param timeout 间隔时间，以毫秒为单位
     *      @param args 传递给回调函数的额外参数，可选
     *      @return 返回定时器对象
     *     
     */
    function setInterval(callback: (...args: any[])=>any, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description 清除指定的定时器
     * 
     *      参数可以是任意值，非定时器对象时静默忽略；重复清除同一定时器不会产生错误。
     *      @param t 指定要清除的定时器
     *     
     */
    function clearInterval(t: any): void;

    /**
     * @description 按固定间隔周期执行回调函数的高精度定时器
     * 
     *      与 setInterval 不同，高精度定时器的回调不依赖事件循环调度，可在任意时刻打断正在运行的脚本执行，时间精度更高。
     * 
     *      由于回调可能随时插入执行，回调内不应修改可能影响其它模块的数据，也不应调用任何异步 API，否则将产生不可预知的结果。例如：
     * 
     *      ```JavaScript
     *      var timers = require('timers');
     * 
     *      var n = 0;
     *      var t = timers.setHrInterval(() => n++, 100);
     * 
     *      // 忙等 50ms，期间回调仍会按间隔触发
     *      var end = Date.now() + 50;
     *      while (Date.now() < end);
     * 
     *      timers.clearHrInterval(t);
     *      console.log(n);
     *      ```
     * 
     *      需要注意的是，即时编译器可能对循环条件进行优化，导致回调中变量的修改无法被循环感知，例如 `while (n < 10)` 不会因回调修改了 n 而退出。
     *      @param callback 回调函数
     *      @param timeout 间隔时间，以毫秒为单位
     *      @param args 传递给回调函数的额外参数，可选
     *      @return 返回定时器对象
     *     
     */
    function setHrInterval(callback: (...args: any[])=>any, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description 清除指定的定时器
     * 
     *      参数可以是任意值，非定时器对象时静默忽略；重复清除同一定时器不会产生错误。
     *      @param t 指定要清除的定时器
     *     
     */
    function clearHrInterval(t: any): void;

    /**
     * @description 在当前同步代码执行完毕后执行回调函数
     * 
     *      args 中的参数将原样传递给回调函数。
     *      @param callback 回调函数
     *      @param args 传递给回调函数的额外参数，可选
     *      @return 返回定时器对象
     *     
     */
    function setImmediate(callback: (...args: any[])=>any, ...args: any[]): Class_Timer;

    /**
     * @description 清除指定的定时器
     * 
     *      参数可以是任意值，非定时器对象时静默忽略；重复清除同一定时器不会产生错误。
     *      @param t 指定要清除的定时器
     *     
     */
    function clearImmediate(t: any): void;

    /**
     * @description 在指定时间内调用函数，函数超时未返回时中断执行并抛出异常
     * 
     *      timeout 以毫秒为单位，小于 1 或大于 2^31-1 的值按 1ms 处理。函数正常返回时，返回其执行结果；args 中的参数将原样传递给函数。
     * 
     *      ```JavaScript
     *      var timers = require('timers');
     * 
     *      // 正常返回函数结果
     *      var r = timers.call((a, b) => a + b, 100, 3, 4);
     * 
     *      // 超时抛出异常
     *      try {
     *          timers.call(() => {
     *              while (true);
     *          }, 30);
     *      } catch (e) {
     *          console.error(e);
     *      }
     *      ```
     *      @param func 要调用的函数
     *      @param timeout 超时时间，以毫秒为单位
     *      @param args 传递给函数的额外参数，可选
     *      @return 返回函数的执行结果
     *     
     */
    function call(func: (...args: any[])=>any, timeout: number, ...args: any[]): any;

}

