/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description Child process object
 *
 *  ```JavaScript
 *  var child_process = require("child_process");
 *  var child = child_process.spawn("ls");
 *  ```
 *
 */
declare class Class_ChildProcess extends Class_EventEmitter {
    /**
     * @description Kills the process this object refers to and delivers a signal
     *       @param signal the signal to deliver
     *
     */
    kill(signal: number): void;

    /**
     * @description Kills the process this object refers to and delivers a signal
     *       @param signal the signal to deliver
     *
     */
    kill(signal?: string): void;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *       @return the exit code of the process
     *
     */
    join(): number;

    join(callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *       @return the exit code of the process
     *
     */
    joinSync(): number;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *       @return the exit code of the process
     *
     */
    joinAsync(): Promise<number>;

    /**
     * @description Queries whether the pipe to the child process is properly connected
     */
    readonly connected: boolean;

    /**
     * @description Closes the ipc pipe to the child process
     */
    disconnect(): void;

    /**
     * @description Sends a message to the current child process
     *      @param msg the message to send
     *
     */
    send(msg: any): void;

    /**
     * @description Resizes the terminal of the current child process
     *      @param cols the number of terminal columns
     *      @param rows the number of terminal rows
     *
     */
    resize(cols: number, rows: number): void;

    /**
     * @description Queries the number of terminal columns
     */
    readonly cols: number;

    /**
     * @description Queries the number of terminal rows
     */
    readonly rows: number;

    /**
     * @description Queries the memory used and the time spent by the current process
     *
     *      The memory report is generated similar to the following result:
     *      ```JavaScript
     *      {
     *        "user": 132379,
     *        "system": 50507,
     *        "rss": 8622080
     *      }
     *      ```
     *      Where:
     *      - user returns the time spent by the process in user code, in microseconds (millionths of a second)
     *      - system returns the time spent by the process in system code, in microseconds (millionths of a second)
     *      - rss returns the amount of physical memory currently used by the process
     *      @return returns the report containing the time information
     *
     */
    usage(): FIBJS.GeneralObject;

    /**
     * @description Reads the id of the process this object refers to
     *
     */
    readonly pid: number;

    /**
     * @description Queries whether the process this object refers to has already exited
     */
    readonly killed: boolean;

    /**
     * @description Queries and sets the exit code of the current process
     */
    readonly exitCode: number;

    /**
     * @description Reads the standard input object of the process this object refers to
     *
     */
    readonly stdin: Class_Stream;

    /**
     * @description Reads the standard output object of the process this object refers to
     *
     */
    readonly stdout: Class_Stream;

    /**
     * @description Reads the standard error object of the process this object refers to
     *
     */
    readonly stderr: Class_Stream;

    /**
     * @description Reads the list of standard IO objects of the process this object refers to
     *
     *      The array contains the standard IO streams of the child process, corresponding to the stdio option passed to spawn. Pipe entries are
     *      Stream objects, and other entries are null.
     *
     */
    readonly stdio: any[];

    /**
     * @description Queries and binds the process exit event, equivalent to on("exit", func);
     */
    on(event: "exit", listener: ()=>void): this;

    /**
     * @description Queries and binds the child process message event, equivalent to on("message", func);
     */
    on(event: "message", listener: ()=>void): this;

    /**
     * @description Queries and binds the child process spawn event, equivalent to on("spawn", func);
     */
    on(event: "spawn", listener: ()=>void): this;

    /**
     * @description Queries and binds the child process disconnect event, equivalent to on("disconnect", func);
     */
    on(event: "disconnect", listener: ()=>void): this;

    /**
     * @description Keeps the fibjs process alive; prevents the fibjs process from exiting while the object is bound
     *      @return returns the current object
     *
     */
    ref(): Class_ChildProcess;

    /**
     * @description Allows the fibjs process to exit; permits the fibjs process to exit while the object is bound
     *      @return returns the current object
     *
     */
    unref(): Class_ChildProcess;

}

