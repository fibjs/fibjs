/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../module/console.d.ts" />
/// <reference path="../module/process.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description 全局对象，所有脚本均可以访问的基础对象
 */
declare module 'global' {
    /**
     * @description 二进制数据缓存对象，用于 io 读写的数据处理，参见 Buffer 对象。
     */
    const Buffer: typeof Class_Buffer;

    /**
     * @description 控制台访问对象 
     */
    const console: typeof import ('console');

    /**
     * @description 进程对象 
     */
    const process: typeof import ('process');

    /**
     * @description Worker 宿主对象，仅在 Worker 入口脚本有效 
     */
    const Master: Class_Worker;

    /**
     * @description 全局对象 
     */
    const global: FIBJS.GeneralObject;

    /**
     * @description 运行一个脚本
     *      @param fname 指定要运行的脚本路径
     *      @param argv 指定要运行的参数，此参数可在脚本内使用 argv 获取
     *      
     */
    function run(fname: string, argv?: any[]): void;

    /**
     * @description 加载一个模块并返回模块对象，更多信息参阅 @ref module
     * 
     *      require 可用于加载基础模块，文件模块。
     * 
     *      基础模块是沙箱创建时初始化的模块，引用时只需传递相应的 id，比如 require("net")。
     * 
     *      文件模块是用户自定义模块，引用时需传递以 ./ 或 ../ 开头的相对路径。文件模块支持 .js, .jsc 和 .json 文件。
     * 
     *      文件模块也支持 package.json 格式，当模块为目录结构时，require 会先查询 package.json 中的 main，未发现则尝试加载路径下的 index.js, index.jsc 或 index.json。
     * 
     *      若引用路径不是 ./ 或 ../ 开头，并且非基础模块，require 从当前模块所在路径下的 node_modules 查找，并上级目录递归。
     * 
     *      基础流程如下:
     * 
     *      ```dot
     *         digraph{
     *             node [fontname = "Helvetica,sans-Serif", fontsize = 10];
     *             edge [fontname = "Helvetica,sans-Serif", fontsize = 10];
     * 
     *             start [label="start"];
     *             resolve [label="path.resolve" shape="rect"];
     *             search [label="recursive lookup\nnode_modules\nfrom the current path" shape="rect"];
     *             load [label="load" shape="rect"];
     *             end [label="end" shape="doublecircle"];
     * 
     *             is_native [label="is internal module?" shape="diamond"];
     *             is_mod [label="is module?" shape="diamond"];
     *             is_abs [label="is absolute?" shape="diamond"];
     *             has_file [label="module exists?" shape="diamond"];
     *             has_ext [label="module.js exists?" shape="diamond"];
     *             has_package [label="/package.json\nexists?" shape="diamond"];
     *             has_main [label="main exists?" shape="diamond"];
     *             has_index [label="index.js exists?" shape="diamond"];
     * 
     *             start -> is_native;
     *             is_native -> end [label="Yes"];
     *             is_native -> is_mod [label="No"];
     *             is_mod -> search [label="Yes"];
     *             search -> has_file;
     *             is_mod -> is_abs [label="No"];
     *             is_abs -> has_file [label="Yes"];
     *             is_abs -> resolve [label="No"];
     *             resolve -> has_file;
     *             has_file -> load [label="Yes"];
     *             has_file -> has_ext [label="No"];
     *             has_ext -> load [label="Yes"];
     *             has_ext -> has_package [label="No"];
     *             has_package -> has_main [label="Yes"];
     *             has_package -> has_index [label="No"];
     *             has_main -> load [label="Yes"];
     *             has_main -> has_index [label="No"];
     *             has_index -> load [label="Yes"];
     *             has_index -> end [label="No"];
     *             load -> end;
     *         }
     *      ```
     * 
     *      @param id 指定要加载的模块名称
     *      @return 返回加载模块的引出对象
     *      
     */
    function require(id: string): any;

    /**
     * @description 获取当前脚本的运行参数，启动 js 获取进程启动参数，run 执行的脚本获取传递的参数 
     */
    const argv: any[];

    /**
     * @description 当前脚本文件名 
     */
    const __filename: string;

    /**
     * @description 当前脚本所在目录 
     */
    const __dirname: string;

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
     * @description 强制要求进行垃圾回收
     */
    function GC(): void;

}

