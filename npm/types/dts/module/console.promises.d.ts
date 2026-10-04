/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/assert.d.ts" />
/// <reference path="../interface/ConsoleObject.d.ts" />
/**
 * The promise variant of the console module: async members return a Promise as their primary form.
 */
declare module 'console/promises' {
    /**
     * @description loglevel constant, fatal error, the most severe level
     */
    export const FATAL: 0;

    /**
     * @description loglevel constant, alert level
     */
    export const ALERT: 1;

    /**
     * @description loglevel constant, critical error level
     */
    export const CRIT: 2;

    /**
     * @description loglevel constant, error level
     */
    export const ERROR: 3;

    /**
     * @description loglevel constant, warning level
     */
    export const WARN: 4;

    /**
     * @description loglevel constant, notice level
     */
    export const NOTICE: 5;

    /**
     * @description loglevel constant, info level
     */
    export const INFO: 6;

    /**
     * @description loglevel constant, debug level
     */
    export const DEBUG: 7;

    /**
     * @description loglevel for output only; no newline after the message is output; file and syslog do not record information at this level
     */
    export const PRINT: 9;

    /**
     * @description loglevel constant, output everything, the default level
     */
    export const NOTSET: 10;

    /**
     * @description Output level used to filter output information; the default is NOTSET, which outputs everything. Information is filtered before being output to the devices configured by add.
     *
     */
    var loglevel: number;

    /**
     * @description Queries the number of characters per line of the terminal
     */
    const width: number;

    /**
     * @description Queries the number of lines of the terminal
     */
    const height: number;

    /**
     * @description Adds a console output system; supported devices are console, syslog, event; up to 10 outputs can be added
     *
     *      By configuring console, program output and system errors can be sent to different devices for runtime environment information collection.
     *
     *      type is the configuration, a device name string:
     *
     *      ```JavaScript
     *      console.add("console");
     *      ```
     *
     *      syslog is only valid on posix platforms:
     *      ```JavaScript
     *      console.add("syslog");
     *      ```
     *
     *      event is only valid on windows platforms:
     *      ```JavaScript
     *      console.add("event");
     *      ```
     *
     *      @param type output device
     *
     */
    function add(type: string): void;

    /**
     * @description Adds a console output system; supported devices are console, syslog, event, nslog and file; up to 10 outputs can be added
     *
     *      By configuring console, program output and system errors can be sent to different devices for runtime environment information collection.
     *
     *      cfg can be a device configuration object:
     *      ```JavaScript
     *      console.add({
     *         type: "console",
     *         levels: [console.INFO, console.ERROR]  // optional, default is all levels
     *      });
     *      ```
     *
     *      syslog is only valid on posix platforms:
     *      ```JavaScript
     *      console.add({
     *         type: "syslog",
     *         levels: [console.INFO, console.ERROR]
     *      });
     *      ```
     *
     *      event is only valid on windows platforms:
     *      ```JavaScript
     *      console.add({
     *         type: "event",
     *         levels: [console.INFO, console.ERROR]
     *      });
     *      ```
     *
     *      nslog is only valid on Darwin platforms:
     *      ```JavaScript
     *      console.add({
     *          type: "nslog",
     *          levels: [console.INFO, console.ERROR]
     *      });
     *      ```
     *
     *      file log:
     *      ```JavaScript
     *      console.add({
     *         type: "file",
     *         levels: [console.INFO, console.ERROR],
     *         path: "path/to/file_%s.log", // Specify the log output file, you can use %s to specify the date insertion position, if not specified, it will be added to the end
     *         split: "30m", // Optional values are "day", "hour", "minute", "####k", "####m", "####g", default is "1m"
     *         count: 10 // option, selectable from 2 to 128, default is 128
     *      });
     *      ```
     *      @param cfg output configuration
     *
     */
    function add(cfg: FIBJS.GeneralObject | any[]): void;

    /**
     * @description Adds a console output system; supported devices are console, syslog, event; up to 10 outputs can be added
     *
     *      By configuring console, program output and system errors can be sent to different devices for runtime environment information collection.
     *
     *      type is the configuration, a device name string:
     *
     *      ```JavaScript
     *      console.use("console");
     *      ```
     *
     *      syslog is only valid on posix platforms:
     *      ```JavaScript
     *      console.use("syslog");
     *      ```
     *
     *      event is only valid on windows platforms:
     *      ```JavaScript
     *      console.use("event");
     *      ```
     *
     *      @param type output device
     *
     */
    function use(type: string): void;

    /**
     * @description Adds a console output system; supported devices are console, syslog, event, nslog and file; up to 10 outputs can be added
     *
     *      By configuring console, program output and system errors can be sent to different devices for runtime environment information collection.
     *
     *      cfg can be a device configuration object:
     *      ```JavaScript
     *      console.use({
     *         type: "console",
     *         levels: [console.INFO, console.ERROR]  // optional, default is all levels
     *      });
     *      ```
     *
     *      syslog is only valid on posix platforms:
     *      ```JavaScript
     *      console.use({
     *         type: "syslog",
     *         levels: [console.INFO, console.ERROR]
     *      });
     *      ```
     *
     *      event is only valid on windows platforms:
     *      ```JavaScript
     *      console.use({
     *         type: "event",
     *         levels: [console.INFO, console.ERROR]
     *      });
     *      ```
     *
     *      nslog is only valid on Darwin platforms:
     *      ```JavaScript
     *      console.use({
     *          type: "nslog",
     *          levels: [console.INFO, console.ERROR]
     *      });
     *      ```
     *
     *      file log:
     *      ```JavaScript
     *      console.use({
     *         type: "file",
     *         levels: [console.INFO, console.ERROR],
     *         path: "path/to/file_%s.log", // Specify the log output file, you can use %s to specify the date insertion position, if not specified, it will be added to the end
     *         split: "30m", // Optional values are "day", "hour", "minute", "####k", "####m", "####g", default is "1m"
     *         count: 10 // option, selectable from 2 to 128, default is 128
     *      });
     *      ```
     *      @param cfg output configuration
     *
     */
    function use(cfg: FIBJS.GeneralObject | any[]): void;

    /**
     * @description Resets to the default settings, outputting information only to console
     */
    function reset(): void;

    /**
     * @description Records general log information, same as info
     *
     *      Records log information at the general level. Usually used to output non-error prompt information.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function log(...args: any[]): void;

    /**
     * @description Records debug log information
     *
     *      Records debug log information. Usually used to output debug information. Not important.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function debug(...args: any[]): void;

    /**
     * @description Records general log information, same as log
     *
     *      Records log information at the general level. Usually used to output non-error prompt information.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function info(...args: any[]): void;

    /**
     * @description Records notice log information
     *
     *      Records notice log information. Usually used to output prompt debug information. Moderately important.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function notice(...args: any[]): void;

    /**
     * @description Records warning log information, same as warning
     *
     *      Records warning log information. Usually used to output warning debug information. Important.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function warn(...args: any[]): void;

    /**
     * @description Records warning log information
     *
     *      Records warning log information. Usually used to output warning debug information. Important.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function warning(...args: any[]): void;

    /**
     * @description Records error log information
     *
     *      Records error log information. Usually used to output error information. Very important. System error messages are also recorded at this level.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function error(...args: any[]): void;

    /**
     * @description Records critical error log information, same as critical
     *
     *      Records critical error log information. Usually used to output critical error information. Very important.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function crit(...args: any[]): void;

    /**
     * @description Records critical error log information
     *
     *      Records critical error log information. Usually used to output critical error information. Very important.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function critical(...args: any[]): void;

    /**
     * @description Records alert error log information
     *
     *      Records alert error log information. Usually used to output alert error information. Very important. It is the highest-level information.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function alert(...args: any[]): void;

    /**
     * @description Outputs the current call stack
     *
     *      Outputs the current call stack through logging.
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function trace(...args: any[]): void;

    /**
     * @description Outputs an object in JSON format
     *
     *      The following parameters are supported:
     *      ```JavaScript
     *      {
     *          "colors": false, // Specify whether to color the output, default is false
     *          "depth": 2, // Specify the maximum depth of output, default is 2
     *          "table": false, // Specify whether to output in table format, default is false
     *          "encode_string": true, // Specify whether to encode strings, default is true
     *          "maxArrayLength": 100, // Specify the maximum number of array elements to display, set to 0 or negative to not display elements, default is 100
     *          "maxStringLength": 10000, // Specify the maximum length of output strings, set to 0 or negative to not display strings, default is 10000
     *          "fields": [], // Specify the fields to display, default is all
     *      }
     *      ```
     *      @param obj specifies the object to process
     *      @param options specifies the format control options
     *
     */
    function dir(obj: any, options?: FIBJS.GeneralObject): void;

    /**
     * @description Outputs an object in JSON format
     *      @param obj the object to display
     *
     */
    function table(obj: any): void;

    /**
     * @description Outputs an object in JSON format
     *      @param obj the object to display
     *      @param fields the fields to display
     *
     */
    function table(obj: any, fields: any[]): void;

    /**
     * @description Outputs formatted text to the console; the output is not recorded in the logging system and no newline is appended, so it can be output continuously
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function print(...args: any[]): void;

    /**
     * @description Moves the console cursor to the specified position
     *      @param row specifies the row coordinate of the new cursor
     *      @param column specifies the column coordinate of the new cursor
     *
     */
    function moveTo(row: number, column: number): void;

    /**
     * @description Hides the console cursor
     */
    function hideCursor(): void;

    /**
     * @description Shows the console cursor
     */
    function showCursor(): void;

    /**
     * @description Clears the console
     */
    function clear(): void;

    /**
     * @description Reads user input from the console
     *      @param msg prompt message
     *      @return returns the information entered by the user
     *
     */
    function readLine(msg?: string): Promise<string>;

    /**
     * @description Reads user input from the console
     *      @param msg prompt message
     *      @return returns the information entered by the user
     *
     */
    function readLineSync(msg?: string): string;

    /**
     * @description Reads user input from the console
     *      @param msg prompt message
     *      @return returns the information entered by the user
     *
     */
    function readLineAsync(msg?: string): Promise<string>;

    /**
     * @description Reads a password entered by the user from the console
     *      @param msg prompt message
     *      @return returns the password entered by the user
     *
     */
    function getpass(msg?: string): Promise<string>;

    /**
     * @description Reads a password entered by the user from the console
     *      @param msg prompt message
     *      @return returns the password entered by the user
     *
     */
    function getpassSync(msg?: string): string;

    /**
     * @description Reads a password entered by the user from the console
     *      @param msg prompt message
     *      @return returns the password entered by the user
     *
     */
    function getpassAsync(msg?: string): Promise<string>;

    /**
     * @description Starts a timer
     *
     *      @param label title, defaults to an empty string.
     *
     */
    function time(label?: string): void;

    /**
     * @description Outputs the current timing value of the specified timer
     *
     *      @param label title, defaults to an empty string.
     *
     */
    function timeElapse(label?: string): void;

    /**
     * @description Ends the specified timer and outputs the final timing value
     *
     *      @param label title, defaults to an empty string.
     *
     */
    function timeEnd(label?: string): void;

    /**
     * @description Assertion test; reports an error if the test value is falsy
     */
    const assert: typeof import ('assert');

    /**
     * @description Console constructor, used to create a new Console instance that outputs to the specified streams
     */
    const Console: typeof Class_ConsoleObject;

}


declare module "console" {
    const promises: typeof import("console/promises");
}
