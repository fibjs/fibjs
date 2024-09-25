/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 日志对象，用于记录日志信息
 * 
 *  可以通过 util.debuglog 模块创建：
 *  ```JavaScript
 *  var debug = util.debuglog('foo');
 *  ```
 *  
 */
declare class Class_Logger extends Class_object {
    /**
     * @description 记录普通日志信息，与 info 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    Function(fmt: string, ...args: any[]): void;

    /**
     * @description 记录普通日志信息，与 info 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param args 可选参数列表
     *      
     */
    Function(...args: any[]): void;

    /**
     * @description 查询当前日志对象的 section 名称 
     */
    readonly section: string;

    /**
     * @description 查询当前日志对象是否启用 
     */
    readonly enabled: boolean;

    /**
     * @description 记录普通日志信息，与 info 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    log(fmt: string, ...args: any[]): void;

    /**
     * @description 记录普通日志信息，与 info 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param args 可选参数列表
     *      
     */
    log(...args: any[]): void;

    /**
     * @description 记录调试日志信息
     * 
     *      记录调试日志信息。通常用于输出调试信息。不重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    debug(fmt: string, ...args: any[]): void;

    /**
     * @description 记录调试日志信息
     * 
     *      记录调试日志信息。通常用于输出调试信息。不重要。
     *      @param args 可选参数列表
     *      
     */
    debug(...args: any[]): void;

    /**
     * @description 记录普通日志信息，与 log 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    info(fmt: string, ...args: any[]): void;

    /**
     * @description 记录普通日志信息，与 log 等同
     * 
     *      记录一般等级的日志信息。通常用于输出非错误性提示信息。
     *      @param args 可选参数列表
     *      
     */
    info(...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出提示性调试信息。一般重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    notice(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出提示性调试信息。一般重要。
     *      @param args 可选参数列表
     *      
     */
    notice(...args: any[]): void;

    /**
     * @description 记录警告日志信息，与 warning 等同
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    warn(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警告日志信息，与 warning 等同
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param args 可选参数列表
     *      
     */
    warn(...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    warning(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警告日志信息
     * 
     *      记录警告日志信息。通常用于输出警告性调试信息。重要。
     *      @param args 可选参数列表
     *      
     */
    warning(...args: any[]): void;

    /**
     * @description 记录错误日志信息
     * 
     *      记录用于错误日志信息。通常用于输出错误信息。非常重要。系统的出错信息也会以此等级记录。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    error(fmt: string, ...args: any[]): void;

    /**
     * @description 记录错误日志信息
     * 
     *      记录用于错误日志信息。通常用于输出错误信息。非常重要。系统的出错信息也会以此等级记录。
     *      @param args 可选参数列表
     *      
     */
    error(...args: any[]): void;

    /**
     * @description 记录关键错误日志信息，与 critical 等同
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    crit(fmt: string, ...args: any[]): void;

    /**
     * @description 记录关键错误日志信息，与 critical 等同
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param args 可选参数列表
     *      
     */
    crit(...args: any[]): void;

    /**
     * @description 记录关键错误日志信息
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    critical(fmt: string, ...args: any[]): void;

    /**
     * @description 记录关键错误日志信息
     * 
     *      记录用于关键错误日志信息。通常用于输出关键错误信息。非常重要。
     *      @param args 可选参数列表
     *      
     */
    critical(...args: any[]): void;

    /**
     * @description 记录警报错误日志信息
     * 
     *      记录用于警报错误日志信息。通常用于输出警报错误信息。非常重要。为最高级别信息。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    alert(fmt: string, ...args: any[]): void;

    /**
     * @description 记录警报错误日志信息
     * 
     *      记录用于警报错误日志信息。通常用于输出警报错误信息。非常重要。为最高级别信息。
     *      @param args 可选参数列表
     *      
     */
    alert(...args: any[]): void;

    /**
     * @description 输出当前调用堆栈
     * 
     *      通过日志输出当前调用堆栈。
     *      @param fmt 格式化字符串
     *      @param args 可选参数列表
     *      
     */
    trace(fmt: string, ...args: any[]): void;

    /**
     * @description 输出当前调用堆栈
     * 
     *      通过日志输出当前调用堆栈。
     *      @param args 可选参数列表
     *      
     */
    trace(...args: any[]): void;

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
    dir(obj: any, options?: FIBJS.GeneralObject): void;

}

