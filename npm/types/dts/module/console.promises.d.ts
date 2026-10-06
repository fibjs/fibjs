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
     * @description loglevel for raw output; no newline, not recorded by the file/syslog/event devices
     */
    export const PRINT: 9;

    /**
     * @description loglevel constant, output everything, the default level
     */
    export const NOTSET: 10;

    /**
     * @description Global severity threshold shared by all devices and Console objects
     *
     *      A record is written only when its level is less than or equal to `loglevel`;
     *      the initial value is NOTSET(10), which accepts everything. The filter is
     *      applied before the record reaches any device, in addition to the per-device
     *      `levels` whitelist configured through add/use, so a device cannot restore a
     *      record that was filtered here. Assigning a non-number throws Error 20005.
     *      See the console module for the severity model.
     *
     */
    var loglevel: number;

    /**
     * @description Width of the console terminal in character cells
     *
     *      Queried from the terminal attached to the process (ioctl TIOCGWINSZ on
     *      POSIX, the console screen buffer on Windows); accessing it throws when the
     *      output is not a terminal, for example Error 25 "inappropriate ioctl for
     *      device" when the process is piped. Useful to wrap or truncate output.
     *
     */
    const width: number;

    /**
     * @description Height of the console terminal in character rows
     *
     *      Queried together with `width` from the terminal attached to the process and
     *      throwing under the same conditions; use both to lay out a full-screen
     *      console interface.
     *
     */
    const height: number;

    /**
     * @description Registers an output device by name
     *
     *      Registers one of the supported devices and stops using the built-in console
     *      fallback; up to 10 devices can be registered and adding an 11th throws Error
     *      20024 ("console: Too many items."). `use` is the historical name of the same
     *      operation and `reset` removes every registered device.
     *
     *      The supported names are `"console"` on every platform, `"syslog"` on POSIX,
     *      `"event"` on Windows and `"nslog"` on Darwin; any other name throws Error
     *      20024 ("console: Unknown log type."). Device management is process-wide and
     *      is not available in worker threads (Error 20009).
     *
     *      Example:
     *      ```JavaScript
     *      console.add('console');
     *      console.log('written through the registered console device');
     *      console.reset();
     *      ```
     *
     *      @param type device name: "console", "syslog", "event" or "nslog"
     *
     */
    function add(type: string): void;

    /**
     * @description Registers output devices from a configuration object or an array of them
     *
     *      cfg is a device configuration object, or an array whose elements are
     *      registered in order. Each element is a device name string or an object:
     *
     *      - `type` (String): device name, required, one of the names accepted by
     *        `add(String)`; a missing type throws Error 20024 ("console: Missing log
     *        type.");
     *      - `levels` (Array): whitelist of severity levels recorded by this device.
     *        Only the listed numbers are written (PRINT is always accepted) and the
     *        default is all levels; an entry outside 0..NOTSET throws Error 20024
     *        ("console: too many logger.").
     *      - File device only:
     *        - `path` (String): target file, required; a missing path throws Error
     *          20024 ("console: Missing path."). A `%s` marker in the name is replaced
     *          by a `YYYYMMDDHHmmss` stamp, otherwise the stamp is appended to the
     *          file name;
     *        - `split`: `"day"`, `"hour"`, `"minute"` or a size threshold such as
     *          `"30m"`, `"10k"` or `"1g"`; giving `count` without `split` throws Error
     *          20024 ("console: Missing split mode.");
     *        - `count` (Integer): rotated files to keep, 2 to 128, 128 by default; a
     *          value outside the range throws Error 20024 ("console: Count must
     *          between 2 to 128.").
     *
     *      The example records only ERROR records to the console device:
     *      ```JavaScript
     *      console.add({ type: 'console', levels: [console.ERROR] });
     *      console.error('recorded');
     *      console.reset();
     *      ```
     *
     *      The file device writes timestamped lines asynchronously, so queued records
     *      may be lost if the program calls `reset` immediately; see the console module
     *      for the device model.
     *
     *      @param cfg device configuration object or array of them
     *
     */
    function add(cfg: FIBJS.GeneralObject | any[]): void;

    /**
     * @description Registers an output device by name; historical alias of add
     *
     *      Behaves exactly like `add(String type)`, including the supported device
     *      names, the 10-device limit and the Error 20024 failures. `use` and `add` are
     *      separate function objects with identical behavior, kept for compatibility
     *      with older fibjs code.
     *
     *      Example:
     *      ```JavaScript
     *      console.use('console');
     *      console.log('written through the registered console device');
     *      console.reset();
     *      ```
     *
     *      @param type device name: "console", "syslog", "event" or "nslog"
     *
     */
    function use(type: string): void;

    /**
     * @description Registers output devices from a configuration object or array; alias of add
     *
     *      Behaves exactly like `add(Object|Array cfg)`, including the file device
     *      configuration (`path`, `split`, `count`), the per-device `levels` whitelist
     *      and the Error 20024 validation failures; see `add` for the full option list.
     *
     *      Example:
     *      ```JavaScript
     *      console.use({ type: 'console', levels: [console.ERROR] });
     *      console.error('recorded');
     *      console.reset();
     *      ```
     *
     *      @param cfg device configuration object or array of them
     *
     */
    function use(cfg: FIBJS.GeneralObject | any[]): void;

    /**
     * @description Removes every registered device and restores the built-in console output
     *
     *      Stops the devices previously registered with add/use and deletes them;
     *      records still queued on an asynchronous device (file, syslog, event) may be
     *      dropped, so a caller that needs them must let the device write first. Device
     *      management is process-wide and is not available in worker threads (Error
     *      20009).
     *
     */
    function reset(): void;

    /**
     * @description Writes a record at INFO level
     *
     *      Records a general message and writes it to stdout, because INFO(6) is above
     *      the WARN error threshold. A leading string argument is a printf-like
     *      template: only `%s`, `%d`, `%j` and `%%` are substituted, other specifiers
     *      stay literal and their values are appended at the end, space separated;
     *      objects are rendered by the inspection formatter. The global `loglevel`
     *      filters the record. Level INFO(6), same as `info`.
     *
     *      Example:
     *      ```JavaScript
     *      console.log('%s has %d items', 'cart', 3); // cart has 3 items
     *      console.log('value:', 42);                 // value: 42
     *      ```
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function log(...args: any[]): void;

    /**
     * @description Writes a record at DEBUG level
     *
     *      The lowest standard level, useful when `loglevel` is raised to DEBUG to trace
     *      execution. The record goes to stdout and accepts the same printf-like
     *      template as `log`. Level DEBUG(7). Node.js treats console.debug as an alias
     *      of console.log, while fibjs keeps a separate level that can be filtered.
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function debug(...args: any[]): void;

    /**
     * @description Writes a record at INFO level, same as log
     *
     *      Records a general message and writes it to stdout. Identical to `log`; the
     *      name follows the Node.js console surface. Level INFO(6).
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function info(...args: any[]): void;

    /**
     * @description Writes a record at NOTICE level
     *
     *      Records a normal but significant message; less severe than WARN and more
     *      important than INFO. Written to stdout and filtered by `loglevel`. Level
     *      NOTICE(5); this is a fibjs extension, Node.js has no console.notice.
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function notice(...args: any[]): void;

    /**
     * @description Writes a record at WARN level
     *
     *      Records a warning and writes it to stderr, because WARN(4) is one of the
     *      error levels. Level WARN(4); Node.js console.warn also writes to stderr.
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function warn(...args: any[]): void;

    /**
     * @description Writes a record at WARN level, same as warn
     *
     *      Records a warning and writes it to stderr. Identical to `warn`, kept as a
     *      separate name for code that reads better with the long form; Node.js has no
     *      console.warning.
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function warning(...args: any[]): void;

    /**
     * @description Writes a record at ERROR level
     *
     *      Records an error and writes it to stderr. fibjs reports its own runtime
     *      errors through the same level, so system error messages may appear among the
     *      application ones. Level ERROR(3).
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function error(...args: any[]): void;

    /**
     * @description Writes a record at CRIT level, same as critical
     *
     *      Records a critical condition and writes it to stderr. Level CRIT(2), below
     *      ERROR in number and therefore more severe.
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function crit(...args: any[]): void;

    /**
     * @description Writes a record at CRIT level
     *
     *      Records a critical condition and writes it to stderr; identical to `crit`.
     *      Level CRIT(2); Node.js has no console.critical.
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function critical(...args: any[]): void;

    /**
     * @description Writes a record at ALERT level
     *
     *      Records the most severe condition at ALERT(1) and writes it to stderr; it is
     *      the highest severity level, meant for conditions that need immediate action.
     *      Node.js has no console.alert.
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function alert(...args: any[]): void;

    /**
     * @description Writes a call stack at WARN level
     *
     *      Formats the optional arguments like `log`, prefixes the text with `Trace: `
     *      and appends the current call stack, then writes the whole record to stderr at
     *      WARN(4) level through the logging system.
     *
     *      Example:
     *      ```JavaScript
     *      function inner() {
     *          console.trace('at inner');
     *      }
     *      inner(); // Trace: at inner, followed by the stack frames
     *      ```
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function trace(...args: any[]): void;

    /**
     * @description Renders a value with util.inspect and writes it at INFO level
     *
     *      The value is rendered by util.inspect and written as a single record to
     *      stdout; options are the inspect options: `colors` (default true, ANSI is
     *      emitted when the terminal supports color), `depth` (default 2, null means
     *      unlimited), `table` (render an array of records as a table), `fields`,
     *      `encode_string`, `maxArrayLength` (default 100) and `maxStringLength`
     *      (default 10000). Unknown options are ignored. Node.js console.dir uses its
     *      own renderer, while fibjs delegates to util.inspect, so keys and strings are
     *      quoted and the layout follows it.
     *
     *      Example:
     *      ```JavaScript
     *      console.dir([{ a: 1 }, { a: 2 }], { colors: false, table: true });
     *      ```
     *
     *      @param obj specifies the object to process
     *      @param options specifies the format control options
     *
     */
    function dir(obj: any, options?: FIBJS.GeneralObject): void;

    /**
     * @description Renders records as a text table at INFO level
     *
     *      An object is rendered as an `(index)`/`Values` table of its properties, an
     *      array of primitives as an `(index)`/`Values` table and an array of records
     *      with one column per record key; a key missing from a record leaves an empty
     *      cell. The table is written to stdout and filtered by `loglevel`; a
     *      ConsoleObject instance writes the same table to its own stdout object.
     *
     *      Example:
     *      ```JavaScript
     *      console.table([{ name: 'alpha', size: 12 }]);
     *      ```
     *
     *      @param obj the object to display
     *
     */
    function table(obj: any): void;

    /**
     * @description Renders records as a text table with selected columns
     *
     *      Same as `table(Value obj)`, but only the columns listed in `fields` are
     *      shown, in that order; the `(index)` column is always kept and a missing
     *      field leaves its cell empty.
     *
     *      @param obj the object to display
     *      @param fields the fields to display
     *
     */
    function table(obj: any, fields: any[]): void;

    /**
     * @description Writes raw text without a newline and without logging metadata
     *
     *      Values are formatted like `log`, but the result is written at PRINT(9): a
     *      newline is never appended and the file, syslog and event devices do not
     *      record it. A `loglevel` below 9 suppresses the output. Consecutive calls join
     *      on the same line; finish the line with `log()` or a literal newline.
     *
     *      Example:
     *      ```JavaScript
     *      console.print('building');
     *      console.print('...');
     *      console.log(); // end the line
     *      ```
     *
     *      @param args optional argument list
     *      When the first argument is a string it is used as the format template, see
     *      util.format; every other value is printed as-is.
     *
     */
    function print(...args: any[]): void;

    /**
     * @description Moves the terminal cursor to a 1-based position
     *
     *      Writes the ANSI sequence ESC[row;colH on POSIX and uses the Win32 console
     *      API on Windows. `row` and `column` must be at least 1, otherwise Error 20004
     *      ("Invalid argument.") is thrown. The sequence is written at PRINT level, so
     *      it is suppressed together with print when `loglevel` is below 9.
     *
     *      @param row the new cursor row, starting at 1
     *      @param column the new cursor column, starting at 1
     *
     */
    function moveTo(row: number, column: number): void;

    /**
     * @description Hides the terminal cursor
     *
     *      Writes the ANSI sequence ESC[?25l on POSIX and hides the console cursor on
     *      Windows; the sequence is written at PRINT level and is suppressed when
     *      `loglevel` is below 9. Use it before updating a status line in place and
     *      restore it with showCursor.
     *
     */
    function hideCursor(): void;

    /**
     * @description Shows the terminal cursor again
     *
     *      Writes the ANSI sequence ESC[?25h on POSIX and restores the console cursor
     *      on Windows; the counterpart of hideCursor, written at PRINT level.
     *
     */
    function showCursor(): void;

    /**
     * @description Clears the terminal
     *
     *      On POSIX this writes the ESC c reset sequence, which clears the screen and
     *      resets the terminal state; on Windows it fills the console screen buffer
     *      with spaces. It is written at PRINT level, so a `loglevel` below 9
     *      suppresses it as well.
     *
     */
    function clear(): void;

    /**
     * @description Reads a line from the standard input, printing an optional prompt
     *
     *      On a terminal the prompt is printed and the line is read with line editing
     *      and history enabled; when the standard input is redirected, the prompt is
     *      written to the standard input stream and at most 1023 bytes are read, with
     *      the trailing newline stripped. Pending device records are flushed before
     *      reading. The call is asynchronous: without a callback the current fiber is
     *      suspended until the line arrives; `readLineSync` is an alias of the same
     *      function, `readLineAsync` takes a callback and `console.promises.readLine`
     *      returns a promise. A read failure throws the underlying system error.
     *
     *      @param msg prompt message
     *      @return returns the line entered by the user, without the newline
     *
     */
    function readLine(msg?: string): Promise<string>;

    /**
     * @description Reads a line from the standard input, printing an optional prompt
     *
     *      On a terminal the prompt is printed and the line is read with line editing
     *      and history enabled; when the standard input is redirected, the prompt is
     *      written to the standard input stream and at most 1023 bytes are read, with
     *      the trailing newline stripped. Pending device records are flushed before
     *      reading. The call is asynchronous: without a callback the current fiber is
     *      suspended until the line arrives; `readLineSync` is an alias of the same
     *      function, `readLineAsync` takes a callback and `console.promises.readLine`
     *      returns a promise. A read failure throws the underlying system error.
     *
     *      @param msg prompt message
     *      @return returns the line entered by the user, without the newline
     *
     */
    function readLineSync(msg?: string): string;

    /**
     * @description Reads a line from the standard input, printing an optional prompt
     *
     *      On a terminal the prompt is printed and the line is read with line editing
     *      and history enabled; when the standard input is redirected, the prompt is
     *      written to the standard input stream and at most 1023 bytes are read, with
     *      the trailing newline stripped. Pending device records are flushed before
     *      reading. The call is asynchronous: without a callback the current fiber is
     *      suspended until the line arrives; `readLineSync` is an alias of the same
     *      function, `readLineAsync` takes a callback and `console.promises.readLine`
     *      returns a promise. A read failure throws the underlying system error.
     *
     *      @param msg prompt message
     *      @return returns the line entered by the user, without the newline
     *
     */
    function readLineAsync(msg?: string): Promise<string>;

    /**
     * @description Reads a password from the standard input without echoing it
     *
     *      Like readLine, but on a terminal the typed characters are not echoed and no
     *      history entry is added; when the input is redirected the two functions
     *      behave the same. Pending device records are flushed before reading, and the
     *      returned line has no trailing newline.
     *
     *      @param msg prompt message
     *      @return returns the password entered by the user
     *
     */
    function getpass(msg?: string): Promise<string>;

    /**
     * @description Reads a password from the standard input without echoing it
     *
     *      Like readLine, but on a terminal the typed characters are not echoed and no
     *      history entry is added; when the input is redirected the two functions
     *      behave the same. Pending device records are flushed before reading, and the
     *      returned line has no trailing newline.
     *
     *      @param msg prompt message
     *      @return returns the password entered by the user
     *
     */
    function getpassSync(msg?: string): string;

    /**
     * @description Reads a password from the standard input without echoing it
     *
     *      Like readLine, but on a terminal the typed characters are not echoed and no
     *      history entry is added; when the input is redirected the two functions
     *      behave the same. Pending device records are flushed before reading, and the
     *      returned line has no trailing newline.
     *
     *      @param msg prompt message
     *      @return returns the password entered by the user
     *
     */
    function getpassAsync(msg?: string): Promise<string>;

    /**
     * @description Starts or restarts a timer under a label
     *
     *      Stores the current time under `label` in a process-wide map shared by all
     *      isolates; starting an existing label silently restarts it and no warning is
     *      printed, unlike Node.js which warns about duplicates. The default label is
     *      `"time"` while Node.js uses `"default"`. `timeElapse` samples the timer and
     *      `timeEnd` stops it; both print `label: <elapsed>ms` at INFO level.
     *
     *      Example:
     *      ```JavaScript
     *      console.time('work');
     *      let sum = 0;
     *      for (let i = 0; i < 100000; i++) sum += i;
     *      console.timeEnd('work'); // work: <elapsed>ms
     *      ```
     *
     *      @param label the timer label, defaults to "time"
     *
     */
    function time(label?: string): void;

    /**
     * @description Prints the value of a timer without stopping it
     *
     *      Outputs `label: <elapsed>ms` at INFO level on stdout, with up to 10
     *      significant digits; the timer keeps running, so it can be sampled repeatedly
     *      before timeEnd. A label that was never started is treated as zero, so the
     *      printed value is huge instead of an error or warning (Node.js has no such
     *      member).
     *
     *      Example:
     *      ```JavaScript
     *      console.time('phase');
     *      console.timeElapse('phase'); // phase: <elapsed>ms
     *      console.timeEnd('phase');    // phase: <elapsed>ms
     *      ```
     *
     *      @param label the timer label, defaults to "time"
     *
     */
    function timeElapse(label?: string): void;

    /**
     * @description Stops a timer and prints its final value
     *
     *      Outputs `label: <elapsed>ms` at INFO level on stdout and removes the label,
     *      so `time` can start it again afterwards. Stopping a label that was never
     *      started measures from zero and prints a huge value; no warning is emitted,
     *      while Node.js warns about the missing label.
     *
     *      Example:
     *      ```JavaScript
     *      console.time('load');
     *      let data = 0;
     *      for (let i = 0; i < 1000; i++) data += i;
     *      console.timeEnd('load'); // load: <elapsed>ms
     *      ```
     *
     *      @param label the timer label, defaults to "time"
     *
     */
    function timeEnd(label?: string): void;

    /**
     * @description The assert module, which throws on a falsy value
     *
     *      `console.assert` is the assert module itself: `console.assert ===
     *      require('assert')` is true. Calling it with a falsy first argument throws an
     *      AssertionError whose message is the second argument, while a truthy value
     *      returns undefined. Node.js instead logs `Assertion failed: <message>` and
     *      never throws, so code ported from Node.js must not rely on that behavior.
     *      Use `assert.ok`, `assert.equal` and friends for richer checks.
     *
     *      Example:
     *      ```JavaScript
     *      try {
     *          console.assert(1 === 2, 'one is not two');
     *      } catch (e) {
     *          console.log('caught:', e.message); // caught: one is not two
     *      }
     *      ```
     *
     */
    const assert: typeof import ('assert');

    /**
     * @description The ConsoleObject constructor, exposed as console.Console
     *
     *      `console.Console` is the ConsoleObject class; `new console.Console(...)`
     *      creates a logger writing to explicit streams, while ConsoleObject is not
     *      available as a global name. See ConsoleObject for the construction forms and
     *      the stream behavior.
     *
     *      Example:
     *      ```JavaScript
     *      const io = require('io');
     *      const out = new io.MemoryStream();
     *      const c = new console.Console(out, out);
     *
     *      c.log('captured');
     *
     *      out.rewind();
     *      console.log(out.readAll().toString().trim()); // captured
     *      ```
     *
     */
    const Console: typeof Class_ConsoleObject;

}


declare module "console" {
    const promises: typeof import("console/promises");
}
