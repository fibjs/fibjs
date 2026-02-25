/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 日志对象，用于记录日志信息
 * 
 * 
 * `Logger` 对象用于记录各种等级的日志信息。它是一个强大的工具，可以帮助开发者在开发和调试过程中记录和追踪应用程序的行为。通过记录日志信息，开发者可以更容易地发现和解决问题，提高代码的可靠性和可维护性。
 * 
 * 在软件开发过程中，日志记录是一个非常重要的环节。通过记录日志，开发者可以了解应用程序的运行状态、捕获异常和错误、分析性能瓶颈等。日志信息通常分为不同的等级，例如调试信息、普通信息、警告信息、错误信息和关键错误信息等。不同等级的日志信息可以帮助开发者更好地分类和管理日志数据。
 * 
 * `Logger` 对象提供了多种方法来记录不同等级的日志信息。可以通过 `util.debuglog` 模块创建 
 * 
 * `Logger` 对象。例如：
 * 
 * ```JavaScript
 * var logger = util.debuglog('example');
 * ```
 * 
 * `Logger` 对象提供了以下主要功能：
 * 
 * - **记录普通日志信息**：用于输出非错误性提示信息。
 * - **记录调试日志信息**：用于输出调试信息，帮助开发者在开发过程中追踪代码执行情况。
 * - **记录警告日志信息**：用于输出提示性调试信息，通常表示可能需要注意的问题。
 * - **记录错误日志信息**：用于输出错误信息，表示程序运行过程中出现了问题。
 * - **记录关键错误日志信息**：用于输出关键错误信息，表示程序运行过程中出现了严重问题。
 * - **记录警报错误日志信息**：用于输出最高级别的错误信息，表示程序运行过程中出现了非常严重的问题。
 * - **输出当前调用堆栈**：通过日志输出当前调用堆栈，帮助开发者了解代码的执行路径。
 * - **用 JSON 格式输出对象**：以 JSON 格式输出对象，支持多种格式控制选项。
 * 
 * 以下是一些使用 `Logger` 对象的示例：
 * 
 * ```JavaScript
 * // Create Logger object
 * var logger = util.debuglog('example');
 * 
 * // Log general log information
 * logger('This is a log message');
 * logger.log('This is a log message with format: %s', 'example');
 * 
 * // Log debug log information
 * logger.debug('This is a debug message');
 * 
 * // Log warning log information
 * logger.warn('This is a warning message');
 * logger.warning('This is a warning message');
 * 
 * // Log error log information
 * logger.error('This is an error message');
 * 
 * // Log critical error log information
 * logger.crit('This is a critical message');
 * logger.critical('This is a critical message');
 * 
 * // Log alert error log information
 * logger.alert('This is an alert message');
 * 
 * // Output current call stack
 * logger.trace('This is a trace message');
 * 
 * // Output object in JSON format
 * logger.dir({ key: 'value' }, { colors: true, depth: 1 });
 * ```
 * 
 * 通过这些方法，您可以方便地记录和管理应用程序中的日志信息。日志记录不仅可以帮助开发者在开发和调试过程中发现和解决问题，还可以在应用程序的生产环境中提供重要的运行时信息，帮助运维人员监控和维护系统的稳定性和性能。
 *  
 */
declare class Class_ConsoleObject extends Class_object {
    /**
     * @description ConsoleObject 构造函数，创建新的 ConsoleObject 对象 
     */
    constructor();

    /**
     * @description ConsoleObject 构造函数，创建新的 ConsoleObject 对象
     *      @param out 指定输出的可写流，默认为 process.stdout
     *      @param err 指定错误输出的可写流，默认为 stdout
     *      
     */
    constructor(out: any, err?: any);

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

    /**
     * @description 用 JSON 格式输出对象
     *      @param obj 给定要显示的对象
     *      
     */
    table(obj: any): void;

    /**
     * @description 用 JSON 格式输出对象
     *      @param obj 给定要显示的对象
     *      @param fields 给定要显示的字段
     *      
     */
    table(obj: any, fields: any[]): void;

    /**
     * @description 启动一个计时器
     * 
     *      @param label 标题，缺省为空字符串。
     *      
     */
    time(label?: string): void;

    /**
     * @description 输出指定计时器当前计时值
     * 
     *      @param label 标题，缺省为空字符串。
     *      
     */
    timeElapse(label?: string): void;

    /**
     * @description 结束指定计时器，并输出最后计时值
     * 
     *      @param label 标题，缺省为空字符串。
     *      
     */
    timeEnd(label?: string): void;

}

