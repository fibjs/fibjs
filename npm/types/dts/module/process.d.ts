/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description 进程处理模块，用以管理当前进程的资源
 * 
 *  引用方法：
 *  ```JavaScript
 *  var process = require('process');
 *  ```
 * 
 *  ## 进程事件
 *  process 模块对象是 EventEmitter 的实例，可以通过注册事件监听器响应进程级别的事件。
 * 
 *  ### beforeExit 事件
 *  **当 fibjs 的任务已经为空，并且没有额外的工作被添加进来，事件 `beforeExit` 会被触发**
 *  ```JavaScript
 *  process.on('beforeExit', exitCode => {});
 *  ```
 *  正常情况下，如果没有额外的工作被添加到任务队列，fibjs 进程会结束。但是如果 `beforeExit` 事件绑定的监听器的回调函数中，启动了一个新的任务，比如开启一个 fiber，那么 fibjs 进程会继续运行。
 * 
 *  process.exitCode 作为唯一的参数值传递给 `beforeExit` 事件监听器的回调函数。如果进程由于显式的原因而将要终止，例如直接调用 process.exit 或抛出未捕获的异常，`beforeExit`事件不会被触发。
 * 
 *  ### exit 事件
 *  **当 fibjs 退出时，事件 `exit` 会被触发，一旦所有与 `exit` 事件绑定的监听器执行完成，进程会终止**
 *  ```JavaScript
 *  process.on('exit', exitCode => {});
 *  ```
 *  `exit` 事件监听器的回调函数，只有一个入参，这个参数的值可以是 process.exitCode 的属性值，或者是调用 process.exit 方法时传入的 `exitCode` 值。
 * 
 *  ### Signal 事件
 *  **当 fibjs 进程接收到一个信号时，会触发信号事件，目前支持的信号有 SIGINT 和 SIGTERM。每个事件名称，以信号名称的大写表示 (比如事件'SIGINT' 对应信号 SIGINT)。**
 *  
 *  信号事件不同于其它进程事件，信号事件是抢占的，当信号发生时，无论当前在 io 操作，还是 JavaScript 运算，都会尽快触发相应事件。比如你可以用下面的代码，中断当前应用，并输出运行状态：
 *  ```JavaScript
 *  var coroutine = require('coroutine');
 * 
 *  process.on('SIGINT', () => {
 *     coroutine.fibers.forEach(f => console.error("Fiber %d:\n%s", f.id, f.stack));
 *     process.exit();
 *  });
 *  ```
 *  信号名称及其意义如下：
 *  * SIGINT：在终端运行时，可以被所有平台支持，通常可以通过 CTRL+C 触发。
 *  * SIGTERM：当进程被 kill 时触发此信号。Windows 下不支持。
 *  
 */
declare module 'process' {
    /**
     * @description 返回当前进程的命令行参数 
     */
    const argv: any[];

    /**
     * @description 返回当前进程的特殊命令行参数，这些参数被 fibjs 用于设置运行环境 
     */
    const execArgv: any[];

    /**
     * @description 返回 fibjs 版本字符串 
     */
    const version: string;

    /**
     * @description 返回 fibjs 及组件的版本信息 
     */
    const versions: FIBJS.GeneralObject;

    /**
     * @description 查询当前运行执行文件完整路径 
     */
    const execPath: string;

    /**
     * @description 查询当前进程的环境变量 
     */
    const env: FIBJS.GeneralObject;

    /**
     * @description 查询当前 cpu 环境，可能的结果为 'amd64', 'arm', 'arm64', 'ia32' 
     */
    const arch: string;

    /**
     * @description 查询当前平台名称，可能的结果为 'darwin', 'freebsd', 'linux', 或 'win32' 
     */
    const platform: string;

    /**
     * @description 读取当前对象指向的进程的 id 
     */
    const pid: number;

    /**
     * @description 读取当前对象指向的父进程的 id 
     */
    const ppid: number;

    /**
     * @description 查询当前进程标准输入对象, 在 tty 中为 TTYInputStream, 否则为 Stream 
     */
    const stdin: Class_Stream;

    /**
     * @description 查询当前进程标准输出对象, 在 tty 中为 TTYOutputStream, 否则为 Stream 
     */
    const stdout: Class_Stream;

    /**
     * @description 查询当前进程标准错误输出对象, 在 tty 中为 TTYOutputStream, 否则为 Stream 
     */
    const stderr: Class_Stream;

    /**
     * @description 查询和设置当前进程的退出码 
     */
    var exitCode: number;

    /**
     * @description 改变当前的 umask，Windows 不支持此方法
     *      @param mask 指定新的掩码
     *      @return 返回之前的 mask
     *     
     */
    function umask(mask: number): number;

    /**
     * @description 改变当前的 umask，Windows 不支持此方法
     *      @param mask 指定新的掩码， 字符串类型八进制(e.g: "0664")
     *      @return 返回之前的 mask
     *     
     */
    function umask(mask: string): number;

    /**
     * @description 返回当前的 umask，Windows 不支持此方法
     *      @return 返回当前的 mask 值
     *     
     */
    function umask(): number;

    /**
     * @description 返回系统高精度时间，此时间与当前时间无关，仅用于高精度计时
     *      @param diff 用于比较的初始时间
     *      @return 返回计时时间，格式为 [seconds, nanoseconds]
     *     
     */
    function hrtime(diff?: any[]): any[];

    /**
     * @description 退出当前进程，并返回 exitCode 作为进程结果     
     */
    function exit(): void;

    /**
     * @description 退出当前进程，并返回结果
     *      @param code 返回进程结果
     *      
     */
    function exit(code: number): void;

    /**
     * @description 返回操作系统当前工作路径
     *      @return 返回当前系统路径
     *      
     */
    function cwd(): string;

    /**
     * @description 修改操作系统当前工作路径
     *      @param directory 指定设定的新路径
     *      
     */
    function chdir(directory: string): void;

    /**
     * @description 查询运行环境运行时间，以秒为单位
     *      @return 返回表示时间的数值
     *     
     */
    function uptime(): number;

    /**
     * @description 查询当前进程内存使用报告
     * 
     *      内存报告生成类似以下结果：
     *      ```JavaScript
     *      {
     *        "rss": 8622080,
     *        "heapTotal": 4083456,
     *        "heapUsed": 1621800
     *      }
     *      ```
     *      其中：
     *      - rss 返回进程当前占用物理内存大小
     *      - heapTotal 返回 v8 引擎堆内存大小
     *      - heapUsed 返回 v8 引擎正在使用堆内存大小
     *      @return 返回包含内存报告
     *      
     */
    function memoryUsage(): FIBJS.GeneralObject;

    /**
     * @description 启动一个纤程
     *      @param func 制定纤程执行的函数
     *      @param args 可变参数序列，此序列会在纤程内传递给函数
     *      
     */
    function nextTick(func: (...args: any[])=>any, ...args: any[]): void;

    /**
     * @description 获取指定名称的内部模块
     *      @param name 指定要查询的内部模块名称
     *      @return 返回指定的内部模块
     *      
     */
    function binding(name: string): any;

}

