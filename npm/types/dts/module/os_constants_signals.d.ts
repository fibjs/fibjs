/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description os_constants signals submodule, containing POSIX signal constants
 *
 *  Usage:
 *  ```JavaScript
 *  var signals = require('os').constants.signals
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

