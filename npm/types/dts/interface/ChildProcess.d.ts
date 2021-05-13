/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description 子进程对象
 * 
 *  ```JavaScript
 *  var child_process = require("child_process");
 *  var child = child_process.spawn("ls");
 *  ```
 *  
 */
declare class Class_ChildProcess extends Class_EventEmitter {
    /**
     * @description 杀掉当前对象指向的进程，并传递信号
     *       @param signal 传递的信号
     *      
     */
    kill(signal: number): void;

    /**
     * @description 等待当前对象指向的进程结束，并返回进程结束代码
     *       @return 进程的结束代码
     *      
     */
    join(): void;

    join(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 读取当前对象指向的进程的 id
     *      
     */
    readonly pid: number;

    /**
     * @description 查询和设置当前进程的退出码 
     */
    readonly exitCode: number;

    /**
     * @description 读取当前对象指向的进程的标准输入对象
     *      
     */
    readonly stdin: Class_Stream;

    /**
     * @description 读取当前对象指向的进程的标准输出对象
     *      
     */
    readonly stdout: Class_Stream;

    /**
     * @description 读取当前对象指向的进程的标准错误对象
     *      
     */
    readonly stderr: Class_Stream;

    /**
     * @description 查询和绑定进程退出事件，相当于 on("exit", func); 
     */
    onexit: (...args: any[])=>any;

}

