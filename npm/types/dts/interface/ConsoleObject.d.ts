/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Log object, used to record log information
 *
 *
 * The `Logger` object is used to record log information at various levels. It is a powerful tool that helps developers record and track application behavior during development and debugging. By recording log information, developers can more easily discover and solve problems, improving the reliability and maintainability of the code.
 *
 * In the software development process, logging is a very important part. By logging, developers can understand the running state of the application, catch exceptions and errors, and analyze performance bottlenecks. Log information is usually divided into different levels, such as debug information, general information, warning information, error information and critical error information. Different levels of log information help developers better classify and manage log data.
 *
 * The `Logger` object provides multiple methods to record log information at different levels. A `Logger` object can be created through the `util.debuglog` module
 *
 * Example of creating a `Logger` object:
 *
 * ```JavaScript
 * var logger = util.debuglog('example');
 * ```
 *
 * The `Logger` object provides the following main features:
 *
 * - **Record general log information**: used to output non-error prompt information.
 * - **Record debug log information**: used to output debug information, helping developers track code execution during development.
 * - **Record warning log information**: used to output prompt debug information, usually indicating issues that may need attention.
 * - **Record error log information**: used to output error information, indicating that a problem occurred while the program was running.
 * - **Record critical error log information**: used to output critical error information, indicating that a serious problem occurred while the program was running.
 * - **Record alert error log information**: used to output the highest-level error information, indicating that a very serious problem occurred while the program was running.
 * - **Output the current call stack**: output the current call stack through logging, helping developers understand the code execution path.
 * - **Output objects in JSON format**: output objects in JSON format, supporting various format control options.
 *
 * The following are some examples of using the `Logger` object:
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
 * With these methods, you can conveniently record and manage log information in your application. Logging not only helps developers discover and solve problems during development and debugging, but also provides important runtime information in the production environment of the application, helping operators monitor and maintain the stability and performance of the system.
 *
 */
declare class Class_ConsoleObject extends Class_object {
    /**
     * @description ConsoleObject constructor, creates a new ConsoleObject object
     */
    constructor();

    /**
     * @description ConsoleObject constructor, creates a new ConsoleObject object
     *      @param out specifies the writable stream for output, the default is process.stdout
     *      @param err specifies the writable stream for error output, the default is stdout
     *
     */
    constructor(out: any, err?: any);

    /**
     * @description Records general log information, same as info
     *
     *      Records log information at the general level. Usually used to output non-error prompt information.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    Function(fmt: string, ...args: any[]): void;

    /**
     * @description Records general log information, same as info
     *
     *      Records log information at the general level. Usually used to output non-error prompt information.
     *      @param args optional argument list
     *
     */
    Function(...args: any[]): void;

    /**
     * @description Queries the section name of the current log object
     */
    readonly section: string;

    /**
     * @description Queries whether the current log object is enabled
     */
    readonly enabled: boolean;

    /**
     * @description Records general log information, same as info
     *
     *      Records log information at the general level. Usually used to output non-error prompt information.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    log(fmt: string, ...args: any[]): void;

    /**
     * @description Records general log information, same as info
     *
     *      Records log information at the general level. Usually used to output non-error prompt information.
     *      @param args optional argument list
     *
     */
    log(...args: any[]): void;

    /**
     * @description Records debug log information
     *
     *      Records debug log information. Usually used to output debug information. Not important.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    debug(fmt: string, ...args: any[]): void;

    /**
     * @description Records debug log information
     *
     *      Records debug log information. Usually used to output debug information. Not important.
     *      @param args optional argument list
     *
     */
    debug(...args: any[]): void;

    /**
     * @description Records general log information, same as log
     *
     *      Records log information at the general level. Usually used to output non-error prompt information.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    info(fmt: string, ...args: any[]): void;

    /**
     * @description Records general log information, same as log
     *
     *      Records log information at the general level. Usually used to output non-error prompt information.
     *      @param args optional argument list
     *
     */
    info(...args: any[]): void;

    /**
     * @description Records notice log information
     *
     *      Records notice log information. Usually used to output prompt debug information. Moderately important.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    notice(fmt: string, ...args: any[]): void;

    /**
     * @description Records notice log information
     *
     *      Records notice log information. Usually used to output prompt debug information. Moderately important.
     *      @param args optional argument list
     *
     */
    notice(...args: any[]): void;

    /**
     * @description Records warning log information, same as warning
     *
     *      Records warning log information. Usually used to output warning debug information. Important.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    warn(fmt: string, ...args: any[]): void;

    /**
     * @description Records warning log information, same as warning
     *
     *      Records warning log information. Usually used to output warning debug information. Important.
     *      @param args optional argument list
     *
     */
    warn(...args: any[]): void;

    /**
     * @description Records warning log information
     *
     *      Records warning log information. Usually used to output warning debug information. Important.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    warning(fmt: string, ...args: any[]): void;

    /**
     * @description Records warning log information
     *
     *      Records warning log information. Usually used to output warning debug information. Important.
     *      @param args optional argument list
     *
     */
    warning(...args: any[]): void;

    /**
     * @description Records error log information
     *
     *      Records error log information. Usually used to output error information. Very important. System error messages are also recorded at this level.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    error(fmt: string, ...args: any[]): void;

    /**
     * @description Records error log information
     *
     *      Records error log information. Usually used to output error information. Very important. System error messages are also recorded at this level.
     *      @param args optional argument list
     *
     */
    error(...args: any[]): void;

    /**
     * @description Records critical error log information, same as critical
     *
     *      Records critical error log information. Usually used to output critical error information. Very important.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    crit(fmt: string, ...args: any[]): void;

    /**
     * @description Records critical error log information, same as critical
     *
     *      Records critical error log information. Usually used to output critical error information. Very important.
     *      @param args optional argument list
     *
     */
    crit(...args: any[]): void;

    /**
     * @description Records critical error log information
     *
     *      Records critical error log information. Usually used to output critical error information. Very important.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    critical(fmt: string, ...args: any[]): void;

    /**
     * @description Records critical error log information
     *
     *      Records critical error log information. Usually used to output critical error information. Very important.
     *      @param args optional argument list
     *
     */
    critical(...args: any[]): void;

    /**
     * @description Records alert error log information
     *
     *      Records alert error log information. Usually used to output alert error information. Very important. It is the highest-level information.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    alert(fmt: string, ...args: any[]): void;

    /**
     * @description Records alert error log information
     *
     *      Records alert error log information. Usually used to output alert error information. Very important. It is the highest-level information.
     *      @param args optional argument list
     *
     */
    alert(...args: any[]): void;

    /**
     * @description Outputs the current call stack
     *
     *      Outputs the current call stack through logging.
     *      @param fmt format string
     *      @param args optional argument list
     *
     */
    trace(fmt: string, ...args: any[]): void;

    /**
     * @description Outputs the current call stack
     *
     *      Outputs the current call stack through logging.
     *      @param args optional argument list
     *
     */
    trace(...args: any[]): void;

    /**
     * @description Outputs an object in JSON format
     *
     *      The following parameters are supported:
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
     *      @param obj specifies the object to process
     *      @param options specifies the format control options
     *
     */
    dir(obj: any, options?: FIBJS.GeneralObject): void;

    /**
     * @description Outputs an object in JSON format
     *      @param obj the object to display
     *
     */
    table(obj: any): void;

    /**
     * @description Outputs an object in JSON format
     *      @param obj the object to display
     *      @param fields the fields to display
     *
     */
    table(obj: any, fields: any[]): void;

    /**
     * @description Starts a timer
     *
     *      @param label title, defaults to an empty string.
     *
     */
    time(label?: string): void;

    /**
     * @description Outputs the current timing value of the specified timer
     *
     *      @param label title, defaults to an empty string.
     *
     */
    timeElapse(label?: string): void;

    /**
     * @description Ends the specified timer and outputs the final timing value
     *
     *      @param label title, defaults to an empty string.
     *
     */
    timeEnd(label?: string): void;

}

