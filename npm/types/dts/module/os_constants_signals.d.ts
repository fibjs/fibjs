/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description os_constants signals 子模块，包含 POSIX 信号常量
 * 
 *  引用方法：
 *  ```JavaScript
 *  var signals = require('os').constants.signals
 *  ```
 *  
 */
declare module 'os_constants_signals' {
    /**
     * @description 挂断信号，终端关闭时发送 
     */
    export const SIGHUP: 1;

    /**
     * @description 中断信号，通常由 CTRL+C 触发 
     */
    export const SIGINT: 2;

    /**
     * @description 退出信号，通常由 CTRL+\ 触发 
     */
    export const SIGQUIT: 3;

    /**
     * @description 非法指令 
     */
    export const SIGILL: 4;

    /**
     * @description 陷阱信号，用于调试 
     */
    export const SIGTRAP: 5;

    /**
     * @description 中止信号，调用 abort() 时发送 
     */
    export const SIGABRT: 6;

    /**
     * @description 与 SIGABRT 相同 
     */
    export const SIGIOT: 6;

    /**
     * @description 总线错误 
     */
    export const SIGBUS: 7;

    /**
     * @description 浮点异常 
     */
    export const SIGFPE: 8;

    /**
     * @description 强制终止信号，不可捕获或忽略 
     */
    export const SIGKILL: 9;

    /**
     * @description 用户自定义信号 1 
     */
    export const SIGUSR1: 10;

    /**
     * @description 段错误，访问无效内存 
     */
    export const SIGSEGV: 11;

    /**
     * @description 用户自定义信号 2 
     */
    export const SIGUSR2: 12;

    /**
     * @description 管道破裂，写入无读取端的管道 
     */
    export const SIGPIPE: 13;

    /**
     * @description 定时器到期信号 
     */
    export const SIGALRM: 14;

    /**
     * @description 终止信号，通常由 kill 命令发送 
     */
    export const SIGTERM: 15;

    /**
     * @description 协处理器栈错误 
     */
    export const SIGSTKFLT: 16;

    /**
     * @description 子进程停止或终止 
     */
    export const SIGCHLD: 17;

    /**
     * @description 继续运行被停止的进程 
     */
    export const SIGCONT: 18;

    /**
     * @description 停止进程，不可捕获或忽略 
     */
    export const SIGSTOP: 19;

    /**
     * @description 终端停止信号，通常由 CTRL+Z 触发 
     */
    export const SIGTSTP: 20;

    /**
     * @description 后台进程读取终端 
     */
    export const SIGTTIN: 21;

    /**
     * @description 后台进程写入终端 
     */
    export const SIGTTOU: 22;

    /**
     * @description 套接字上有紧急数据 
     */
    export const SIGURG: 23;

    /**
     * @description CPU 时间超限 
     */
    export const SIGXCPU: 24;

    /**
     * @description 文件大小超限 
     */
    export const SIGXFSZ: 25;

    /**
     * @description 虚拟定时器到期 
     */
    export const SIGVTALRM: 26;

    /**
     * @description 性能剖析定时器到期 
     */
    export const SIGPROF: 27;

    /**
     * @description 终端窗口大小变化 
     */
    export const SIGWINCH: 28;

    /**
     * @description 异步 I/O 就绪 
     */
    export const SIGIO: 29;

    /**
     * @description 与 SIGIO 相同 
     */
    export const SIGPOLL: 29;

    /**
     * @description 电源故障 
     */
    export const SIGPWR: 30;

    /**
     * @description 无效的系统调用 
     */
    export const SIGSYS: 31;

}

