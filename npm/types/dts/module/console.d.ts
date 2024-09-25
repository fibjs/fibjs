/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/assert.d.ts" />
/**
 * @description 控制台访问对象
 * 
 *  console 模块是一个核心模块，它提供了类似于浏览器中 console 对象的功能，可以将信息输出到控制台，方便调试和输出信息。
 * 
 *  console 模块中最常用的方法是 log()，该方法可以将任何 JavaScript 值打印到控制台，并自动添加换行符。除了 log() 方法外，还有 info()、warn()、error() 方法，分别用于输出信息、警告和错误，它们的功能和 log() 方法基本相同，只是在控制台中显示的样式不同。
 * 
 *  console 模块还提供了 dir() 方法，用于将一个对象的属性和方法以可读性更强的形式输出到控制台，方便调试复杂的对象。另外，还有 time() 和 timeEnd() 方法，用于在控制台中计时代码执行的时间，并输出时间差。
 * 
 *  除了以上常用的方法，console 模块还提供了一些其他的方法，如 assert()、notic()、trace() 等，可以在不同的情况下方便地进行调试和信息输出。
 * 
 *  console 模块是一个非常实用的模块，可以在开发过程中提高调试效率，方便快捷地输出各种信息。
 *  
 */
declare module 'console' {
    /**
     * @description loglevel 级别常量 
     */
    export const FATAL: 0;

    /**
     * @description loglevel 级别常量 
     */
    export const ALERT: 1;

    /**
     * @description loglevel 级别常量 
     */
    export const CRIT: 2;

    /**
     * @description loglevel 级别常量 
     */
    export const ERROR: 3;

    /**
     * @description loglevel 级别常量 
     */
    export const WARN: 4;

    /**
     * @description loglevel 级别常量 
     */
    export const NOTICE: 5;

    /**
     * @description loglevel 级别常量 
     */
    export const INFO: 6;

    /**
     * @description loglevel 级别常量 
     */
    export const DEBUG: 7;

    /**
     * @description loglevel 仅用于输出，信息输出后不换行，file 和 syslog 不保存此级别信息 
     */
    export const PRINT: 9;

    /**
     * @description loglevel 级别常量 
     */
    export const NOTSET: 10;

    /**
     * @description 输出级别，用以过滤输出信息，缺省为 NOTSET，全部输出。信息过滤之后才会输出给 add 设定的各个设备。
     *      
     */
    var loglevel: number;

    /**
     * @description 查询终端每行字符数 
     */
    const width: number;

    /**
     * @description 查询终端行数 
     */
    const height: number;

    /**
     * @description 添加 console 输出系统，支持的设备为 console, syslog, event，最多可以添加 10 个输出
     * 
     *      通过配置 console，可以将程序输出和系统错误发往不同设备，用于运行环境信息收集。
     * 
     *      type 为配置，为设备名称字符串：
     * 
     *      ```JavaScript
     *      console.add("console");
     *      ```
     * 
     *      syslog 仅在 posix 平台有效：
     *      ```JavaScript
     *      console.add("syslog");
     *      ```
     * 
     *      event 仅在 windows 平台有效：
     *      ```JavaScript
     *      console.add("event");
     *      ```
     * 
     *      @param type 输出设备
     *      
     */
    function add(type: string): void;

    /**
     * @description 添加 console 输出系统，支持的设备为 console, syslog, event 和 file，最多可以添加 10 个输出
     * 
     *      通过配置 console，可以将程序输出和系统错误发往不同设备，用于运行环境信息收集。
     * 
     *      cfg 可以为一个设备配置对象：
     *      ```JavaScript
     *      console.add({
     *         type: "console",
     *         levels: [console.INFO, console.ERROR]  // optional, default is all levels
     *      });
     *      ```
     * 
     *      syslog 仅在 posix 平台有效：
     *      ```JavaScript
     *      console.add({
     *         type: "syslog",
     *         levels: [console.INFO, console.ERROR]
     *      });
     *      ```
     * 
     *      event 仅在 windows 平台有效：
     *      ```JavaScript
     *      console.add({
     *         type: "event",
     *         levels: [console.INFO, console.ERROR]
     *      });
     *      ```
     * 
     *      file 日志：
     *      ```JavaScript
     *      console.add({
     *         type: "file",
     *         levels: [console.INFO, console.ERROR],
     *         path: "path/to/file_%s.log", // specifies the log output file, can use %s to specify the insertion date location, or add at the end if not specified
     *         split: "30m", // Optional values are "day", "hour", "minute", "####k", "####m", "####g", default is "1m"
     *         count: 10 // option, selectable from 2 to 128, default is 128
     *      });
     *      ```
     * 
     *      @param cfg 输出配置
     *      
     */
    function add(cfg: FIBJS.GeneralObject): void;

    /**
     * @description 批量添加 console 输出系统，支持的设备为 console, syslog, event 和 file，最多可以添加 10 个输出
     * 
     *      通过配置 console，可以将程序输出和系统错误发往不同设备，用于运行环境信息收集。
     * 
     *      ```JavaScript
     *      console.add(["console", {
     *         type: "syslog",
     *         levels: [console.INFO, console.ERROR]
     *      }]);
     *      ```
     * 
     *      @param cfg 输出配置数组
     *      
     */
    function add(cfg: any[]): void;

    /**
     * @description 初始化到缺省设置，只在 console 输出信息 
     */
    function reset(): void;

    /**
     * @description 记录普通日志信息，与 info 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function log(fmt: string, ...args: any[]): void;

    /**
     * @description 记录普通日志信息，与 info 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param args 可选参数列表
     *      
     */
    function log(...args: any[]): void;

    /**
     * @description 记录调试日志信息
     * 
     *      记录调试日志信息。通常用于输出调试信息。不重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function debug(fmt: string, ...args: any[]): void;

    /**
     * @description 记录调试日志信息
     * 
     *      记录调试日志信息。通常用于输出调试信息。不重要。
     *      @param args 可选参数列表
     *      
     */
    function debug(...args: any[]): void;

    /**
     * @description 记录普通日志信息，与 log 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function info(fmt: string, ...args: any[]): void;

    /**
     * @description 记录普通日志信息，与 log 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param args 可选参数列表
     *      
     */
    function info(...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出提示性调试信息。一般重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function notice(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出提示性调试信息。一般重要。
     *      @param args 可选参数列表
     *      
     */
    function notice(...args: any[]): void;

    /**
     * @description 记录警告日志信息，与 warning 等同
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function warn(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警告日志信息，与 warning 等同
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param args 可选参数列表
     *      
     */
    function warn(...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function warning(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param args 可选参数列表
     *      
     */
    function warning(...args: any[]): void;

    /**
     * @description 记录错误日志信息
     * 
     *      记录用于错误日志信息。通常用于输出错误信息。非常重要。系统的出错信息也会以此等级记录。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function error(fmt: string, ...args: any[]): void;

    /**
     * @description 记录错误日志信息
     * 
     *      记录用于错误日志信息。通常用于输出错误信息。非常重要。系统的出错信息也会以此等级记录。
     *      @param args 可选参数列表
     *      
     */
    function error(...args: any[]): void;

    /**
     * @description 记录关键错误日志信息，与 critical 等同
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function crit(fmt: string, ...args: any[]): void;

    /**
     * @description 记录关键错误日志信息，与 critical 等同
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param args 可选参数列表
     *      
     */
    function crit(...args: any[]): void;

    /**
     * @description 记录关键错误日志信息
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function critical(fmt: string, ...args: any[]): void;

    /**
     * @description 记录关键错误日志信息
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param args 可选参数列表
     *      
     */
    function critical(...args: any[]): void;

    /**
     * @description 记录警报错误日志信息
     * 
     *      记录用于警报错误日志信息。通常用于输出警报错误信息。非常重要。为最高级别信息。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function alert(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警报错误日志信息
     * 
     *      记录用于警报错误日志信息。通常用于输出警报错误信息。非常重要。为最高级别信息。
     *      @param args 可选参数列表
     *      
     */
    function alert(...args: any[]): void;

    /**
     * @description 输出当前调用堆栈
     * 
     *      通过日志输出当前调用堆栈。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function trace(fmt: string, ...args: any[]): void;

    /**
     * @description 输出当前调用堆栈
     * 
     *      通过日志输出当前调用堆栈。
     *      @param args 可选参数列表
     *      
     */
    function trace(...args: any[]): void;

    /**
     * @description 用 JSON 格式输出对象
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "colors": false, // specify if output should be colorized, defaults to false
     *          "depth": 2, // specify the max depth of the output, defaults to 2
     *          "table": false, // specify if output should be a table, defaults to false
     *          "encode_string": true, // specify if string should be encoded, defaults to true
     *          "maxArrayLength": 100, // specify max number of array elements to show, set to 0 or negative to show no elements, defaults to 100
     *          "maxStringLength": 10000, // specify max string length to output, set to 0 or negative to show no strings, defaults to 10000
     *          "fields": [], // specify the fields to be displayed, defaults to all
     *      }
     *      ```
     *      @param obj 指定需要处理的对象
     *      @param options 指定格式控制选项
     *      
     */
    function dir(obj: any, options?: FIBJS.GeneralObject): void;

    /**
     * @description 用 JSON 格式输出对象
     *      @param obj 给定要显示的对象
     *      
     */
    function table(obj: any): void;

    /**
     * @description 用 JSON 格式输出对象
     *      @param obj 给定要显示的对象
     *      @param fields 给定要显示的字段
     *      
     */
    function table(obj: any, fields: any[]): void;

    /**
     * @description 向控制台输出格式化文本，输出内容不会记入日志系统，输出文本后不会自动换行，可连续输出
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function print(fmt: string, ...args: any[]): void;

    /**
     * @description 向控制台输出格式化文本，输出内容不会记入日志系统，输出文本后不会自动换行，可连续输出
     *      @param args 可选参数列表
     *      
     */
    function print(...args: any[]): void;

    /**
     * @description 移动控制台光标到指定位置
     *      @param row 指定新光标的行坐标
     *      @param column 指定新光标的列坐标
     *      
     */
    function moveTo(row: number, column: number): void;

    /**
     * @description 隐藏控制台光标 
     */
    function hideCursor(): void;

    /**
     * @description 显示控制台光标 
     */
    function showCursor(): void;

    /**
     * @description 清除控制台 
     */
    function clear(): void;

    /**
     * @description 从控制台读取用户输入
     *      @param msg 提示信息
     *      @return 返回用户输入的信息
     *      
     */
    function readLine(msg?: string): string;

    function readLine(msg?: string, callback?: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 从控制台读取用户输入的密码
     *      @param msg 提示信息
     *      @return 返回用户输入的密码
     *      
     */
    function getpass(msg?: string): string;

    function getpass(msg?: string, callback?: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 启动一个计时器
     * 
     *      @param label 标题，缺省为空字符串。
     *      
     */
    function time(label?: string): void;

    /**
     * @description 输出指定计时器当前计时值
     * 
     *      @param label 标题，缺省为空字符串。
     *      
     */
    function timeElapse(label?: string): void;

    /**
     * @description 结束指定计时器，并输出最后计时值
     * 
     *      @param label 标题，缺省为空字符串。
     *      
     */
    function timeEnd(label?: string): void;

    /**
     * @description 断言测试，如果测试值为假，则报错 
     */
    const assert: typeof import ('assert');

}

