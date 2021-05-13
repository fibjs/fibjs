/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description 控制台访问对象
 * 
 *  全局对象。可用于提示信息，警告和错误记录。通过启动配置文件，可将日志定位到不同的设备，以便于跟踪。日志支持格式化输出，例如：
 *  ```JavaScript
 *  console.log("%d + %d = %d", 100, 200, 100 + 200);
 *  ```
 *  可以使用的格式化参数如下：
 *  - %s - 字符串
 *  - %d - 数字，包括整数和数字
 *  - %j - 以 JSON 格式输出对象
 *  - %% - 输出字符 '%' 本身
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
     *         levels: [console.INFO, console.ERROR]  // 选项，省略则输出全部级别日志
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
     *         // 必选项，指定日志输出文件，可使用 s% 指定插入日期位置，不指定则添加在结尾
     *         path: "path/to/file_%s.log",
     *         // 选项，可选值为 "day", "hour", "minute", "###k", "###m", "###g"，缺省为 "1m"
     *         split: "30m",
     *         // 选项，可选范围为 2-128，缺省为 128
     *         count: 10
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
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function warn(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param args 可选参数列表
     *      
     */
    function warn(...args: any[]): void;

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
     * @description 记录关键错误日志信息
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    function crit(fmt: string, ...args: any[]): void;

    /**
     * @description 记录关键错误日志信息
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param args 可选参数列表
     *      
     */
    function crit(...args: any[]): void;

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
     * @description 用 JSON 格式输出对象
     *      @param obj 给定要显示的对象
     *      
     */
    function dir(obj: any): void;

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
     * @description 输出当前调用堆栈
     * 
     *      通过日志输出当前调用堆栈。
     *      @param label 标题，缺省为空字符串。
     *      
     */
    function trace(label?: string): void;

    /**
     * @description 断言测试，如果测试值为假，则报错
     *      @param value 测试的数值
     *      @param msg 报错信息
     *      
     */
    function assert(value: any, msg?: string): void;

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
     * @description 按下一个按键
     * 
     *      参数 key 可以使用字符串传入功能键：
     *      - 功能键：f1 - f12
     *      - 方向键：up, down,left, right, home, end, pageup, pagedown
     *      - 编辑键：backspace, delete, insert, enter, tab, escape, space
     *      - 控制键：control, alt, shift, command
     *      @param key 指定按键，单字符直接传入，功能键传入名称
     *      @param modifier 指定控制键，可以为：control, alt, shift, command
     *      
     */
    function keyDown(key: string, modifier?: string): void;

    /**
     * @description 按下一个按键
     * 
     *      参数 key 可以使用字符串传入功能键：
     *      - 功能键：f1 - f12
     *      - 方向键：up, down,left, right, home, end, pageup, pagedown
     *      - 编辑键：backspace, delete, insert, enter, tab, escape, space
     *      - 控制键：control, alt, shift, command
     *      @param key 指定按键，单字符直接传入，功能键传入名称
     *      @param modifier 指定控制键数组，可以为：control, alt, shift, command
     *      
     */
    function keyDown(key: string, modifier: any[]): void;

    /**
     * @description 松开一个按键
     * 
     *      参数 key 可以使用字符串传入功能键：
     *      - 功能键：f1 - f12
     *      - 方向键：up, down,left, right, home, end, pageup, pagedown
     *      - 编辑键：backspace, delete, insert, enter, tab, escape, space
     *      - 控制键：control, alt, shift, command
     *      @param key 指定按键，单字符直接传入，功能键传入名称
     *      @param modifier 指定控制键，可以为：control, alt, shift, command
     *      
     */
    function keyUp(key: string, modifier?: string): void;

    /**
     * @description 松开一个按键
     * 
     *      参数 key 可以使用字符串传入功能键：
     *      - 功能键：f1 - f12
     *      - 方向键：up, down,left, right, home, end, pageup, pagedown
     *      - 编辑键：backspace, delete, insert, enter, tab, escape, space
     *      - 控制键：control, alt, shift, command
     *      @param key 指定按键，单字符直接传入，功能键传入名称
     *      @param modifier 指定控制键数组，可以为：control, alt, shift, command
     *      
     */
    function keyUp(key: string, modifier: any[]): void;

    /**
     * @description 点击并松开一个按键
     * 
     *      参数 key 可以使用字符串传入功能键：
     *      - 功能键：f1 - f12
     *      - 方向键：up, down,left, right, home, end, pageup, pagedown
     *      - 编辑键：backspace, delete, insert, enter, tab, escape, space
     *      - 控制键：control, alt, shift, command
     *      @param key 指定按键，单字符直接传入，功能键传入名称
     *      @param modifier 指定控制键，可以为：control, alt, shift, command
     *      
     */
    function keyTap(key: string, modifier?: string): void;

    /**
     * @description 点击并松开一个按键
     * 
     *      参数 key 可以使用字符串传入功能键：
     *      - 功能键：f1 - f12
     *      - 方向键：up, down,left, right, home, end, pageup, pagedown
     *      - 编辑键：backspace, delete, insert, enter, tab, escape, space
     *      - 控制键：control, alt, shift, command
     *      @param key 指定按键，单字符直接传入，功能键传入名称
     *      @param modifier 指定控制键数组，可以为：control, alt, shift, command
     *      
     */
    function keyTap(key: string, modifier: any[]): void;

    /**
     * @description 输入一个字符串
     *      @param text 指定输入的字符串
     *      
     */
    function typeString(text: string): void;

    /**
     * @description 移动鼠标到指定的位置
     *      @param x 指定 x 坐标
     *      @param y 指定 y 坐标
     *      
     */
    function moveMouse(x: number, y: number): void;

    /**
     * @description 按下一个鼠标键
     *      @param button 指定鼠标键名称，允许值为: left, right, moddle
     *      
     */
    function mouseUp(button: string): void;

    /**
     * @description 放开一个鼠标键
     *      @param button 指定鼠标键名称，允许值为: left, right, moddle
     *      
     */
    function mouseDown(button: string): void;

    /**
     * @description 点击一个鼠标键
     *      @param button 指定鼠标键名称，允许值为: left, right, moddle
     *      @param dbclick 指定是否双击，缺省为 false
     *      
     */
    function clickMouse(button: string, dbclick?: boolean): void;

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

}

