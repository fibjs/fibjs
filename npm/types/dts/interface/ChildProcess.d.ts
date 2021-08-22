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
     * @description 杀掉当前对象指向的进程，并传递信号
     *       @param signal 传递的信号
     *      
     */
    kill(signal: string): void;

    /**
     * @description 等待当前对象指向的进程结束，并返回进程结束代码
     *       @return 进程的结束代码
     *      
     */
    join(): void;

    join(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询当前进程占用的内存和花费的时间
     * 
     *      内存报告生成类似以下结果：
     *      ```JavaScript
     *      {
     *        "user": 132379,
     *        "system": 50507,
     *        "rss": 8622080
     *      }
     *      ```
     *      其中：
     *      - user 返回进程在用户代码中花费的时间，单位为微秒值（百万分之一秒）
     *      - system 返回进程在系统代码中花费的时间，单位为微秒值（百万分之一秒）
     *      - rss 返回进程当前占用物理内存大小
     *      @return 返回包含时间报告
     *      
     */
    usage(): FIBJS.GeneralObject;

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

