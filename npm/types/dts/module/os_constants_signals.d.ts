/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The signals table of os.constants: the signal numbers of the running platform
 *
 *  Reached through `require('os').constants.signals`. On POSIX the names and numbers match the
 *  host's `<signal.h>` (verified against the Linux x86-64 headers) and include the aliases
 *  SIGIOT = SIGABRT and SIGPOLL = SIGIO; SIGSTKFLT and SIGPWR exist only on Linux. Windows
 *  exposes the same names (matching Node.js) but can deliver only a few of them.
 *
 *  Concepts:
 *
 *  - **Numbers feed the process APIs**: process.kill and ChildProcess#kill accept a name or a
 *    number, and this table provides the numbers. A process that dies from a signal reports the
 *    negated number as its exit code (SIGTERM gives -15), the 'exit' event receives the signal
 *    name, and join() returns the same negative value.
 *  - **Delivery is limited**: in fibjs only SIGINT and SIGTERM reach a JavaScript listener
 *    registered with process.on; every other signal keeps the default action of the operating
 *    system. Sending SIGTERM or SIGKILL to a child is still the portable way to stop it.
 *  - **Not the legacy numbers**: the constants module ships a frozen BSD/macOS set (SIGUSR1 is
 *    30, SIGBUS is 10) whose values name different signals on Linux; always take the number
 *    from this table when it is passed to an API.
 *  - **Windows**: numeric signals other than 0 are POSIX-only; on Windows kill terminates the
 *    process unconditionally instead of delivering the named signal.
 *
 *  Import:
 *  ```JavaScript
 *  const signals = require('os').constants.signals;
 *  ```
 *
 *  Example 1 — stop a child process with SIGTERM:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *  const signals = require('os').constants.signals;
 *
 *  // Start a fibjs child that lives until it is killed.
 *  const child = child_process.spawn(process.execPath,
 *      ['-e', 'setTimeout(function () {}, 60000)'], { stdio: 'ignore' });
 *
 *  child.kill(signals.SIGTERM);
 *  const code = child.join();
 *
 *  // A process killed by a signal reports the negated signal number.
 *  console.log(code === -signals.SIGTERM, signals.SIGTERM); // true 15
 *  ```
 *
 *  Example 2 — the numbers and aliases of the common signals:
 *  ```JavaScript
 *  const signals = require('os').constants.signals;
 *
 *  console.log(signals.SIGINT, signals.SIGKILL, signals.SIGSTOP);  // 2 9 19
 *  console.log(signals.SIGTERM, signals.SIGSEGV, signals.SIGPIPE); // 15 11 13
 *  console.log(signals.SIGIO === signals.SIGPOLL);                 // true
 *  console.log(signals.SIGSTKFLT, signals.SIGCHLD, signals.SIGSYS); // 16 17 31
 *  ```
 *
 *  Example 3 — name the signal behind an exit code:
 *  ```JavaScript
 *  const signals = require('os').constants.signals;
 *
 *  // A child killed by SIGTERM exits with -15 (see ChildProcess#exitCode), so the
 *  // wanted name is the one whose value is the negated code.
 *  const code = -signals.SIGTERM;
 *  const names = Object.keys(signals).filter((name) => signals[name] === -code);
 *  console.log(names.join(', ')); // SIGTERM
 *  ```
 *
 */
declare module 'os_constants_signals' {
    /**
     * @description Hangup signal, sent when the terminal is closed
     */
    export const SIGHUP: 1;

    /**
     * @description Interrupt signal, usually triggered by CTRL+C
     */
    export const SIGINT: 2;

    /**
     * @description Quit signal, usually triggered by CTRL+\
     */
    export const SIGQUIT: 3;

    /**
     * @description Illegal instruction
     */
    export const SIGILL: 4;

    /**
     * @description Trap signal, used for debugging
     */
    export const SIGTRAP: 5;

    /**
     * @description Abort signal, sent when abort() is called
     */
    export const SIGABRT: 6;

    /**
     * @description Same as SIGABRT
     */
    export const SIGIOT: 6;

    /**
     * @description Bus error
     */
    export const SIGBUS: 7;

    /**
     * @description Floating-point exception
     */
    export const SIGFPE: 8;

    /**
     * @description Kill signal, cannot be caught or ignored
     */
    export const SIGKILL: 9;

    /**
     * @description User-defined signal 1
     */
    export const SIGUSR1: 10;

    /**
     * @description Segmentation fault, invalid memory access
     */
    export const SIGSEGV: 11;

    /**
     * @description User-defined signal 2
     */
    export const SIGUSR2: 12;

    /**
     * @description Broken pipe, write to a pipe with no readers
     */
    export const SIGPIPE: 13;

    /**
     * @description Timer expired signal
     */
    export const SIGALRM: 14;

    /**
     * @description Termination signal, usually sent by the kill command
     */
    export const SIGTERM: 15;

    /**
     * @description Coprocessor stack error
     */
    export const SIGSTKFLT: 16;

    /**
     * @description Child process stopped or terminated
     */
    export const SIGCHLD: 17;

    /**
     * @description Continue a stopped process
     */
    export const SIGCONT: 18;

    /**
     * @description Stop the process, cannot be caught or ignored
     */
    export const SIGSTOP: 19;

    /**
     * @description Terminal stop signal, usually triggered by CTRL+Z
     */
    export const SIGTSTP: 20;

    /**
     * @description Background process reading from the terminal
     */
    export const SIGTTIN: 21;

    /**
     * @description Background process writing to the terminal
     */
    export const SIGTTOU: 22;

    /**
     * @description Urgent data available on the socket
     */
    export const SIGURG: 23;

    /**
     * @description CPU time limit exceeded
     */
    export const SIGXCPU: 24;

    /**
     * @description File size limit exceeded
     */
    export const SIGXFSZ: 25;

    /**
     * @description Virtual timer expired
     */
    export const SIGVTALRM: 26;

    /**
     * @description Profiling timer expired
     */
    export const SIGPROF: 27;

    /**
     * @description Terminal window size changed
     */
    export const SIGWINCH: 28;

    /**
     * @description Asynchronous I/O is ready
     */
    export const SIGIO: 29;

    /**
     * @description Same as SIGIO
     */
    export const SIGPOLL: 29;

    /**
     * @description Power failure
     */
    export const SIGPWR: 30;

    /**
     * @description Invalid system call
     */
    export const SIGSYS: 31;

}

