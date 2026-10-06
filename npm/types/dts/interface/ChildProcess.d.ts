/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description A handle to a child process created by spawn, fork or the callback form of exec, execFile and run
 *
 *  The object is an EventEmitter that exposes the process id, its stdio streams and its exit
 *  state. It is created by the child_process module; the synchronous forms of exec, execFile and
 *  run return a buffered result instead, so use the callback form when the handle is needed.
 *
 *  Concepts:
 *
 *  - **Lifecycle**: `pid` is available immediately; `exitCode` is null while the process runs and
 *    `killed` records whether kill was called (or a timeout or abort signal fired). After the exit
 *    `exitCode` holds 0-255 for a normal exit, or the negative signal number (-15 for SIGTERM, -9
 *    for SIGKILL) when the process died from a signal. Node.js reports null in that case and
 *    exposes the signal separately through `signalCode`.
 *  - **Waiting**: `join()` blocks the current fiber until the process exits and returns the same
 *    value as `exitCode`; wait before reading state or letting the parent finish. `ref` and `unref`
 *    control whether a running child keeps the fibjs process alive.
 *  - **stdio**: descriptors configured as 'pipe' are exposed as Stream objects through `stdin`,
 *    `stdout`, `stderr` and the `stdio` array; 'ignore' and 'inherit' descriptors are null. Read
 *    the output streams to drain them, otherwise the child can block on a full pipe buffer.
 *  - **Terminal (pty)**: with `stdio: 'pty'` stdin and stdout are a pseudo terminal; `cols`, `rows`
 *    and `resize` are available in this mode and throw Error 20024 for other children.
 *  - **IPC**: a fork child, or a spawn with an 'ipc' stdio entry, gets a message channel;
 *    `connected`, `send` and `disconnect` operate on it and incoming messages are delivered to the
 *    'message' event. Only one 'ipc' entry is allowed per child (ERR_IPC_ONE_PIPE).
 *  - **Events**: the implementation emits `exit` and the Node-style `close` event (emitted after
 *    the stdio streams close, not declared above) with `(code, signal)`; code is null on a signal
 *    death and signal is null on a normal exit. `spawn` fires after a successful spawn and
 *    `disconnect` fires when the IPC channel closes.
 *  - **Node.js comparison**: fibjs adds `join()` and `usage()`; Node has `signalCode`, `kill`
 *    returns a boolean, `send` accepts a callback and a signal death is not encoded as a negative
 *    exit code.
 *
 *  Obtained from:
 *  - `child_process.spawn(...)` and `child_process.fork(...)` — always return a ChildProcess;
 *  - `child_process.exec(...)`, `execFile(...)` and `run(...)` — return one only in the callback
 *    form; their synchronous form returns the buffered output or the exit code.
 *
 *  Example 1 — read the stdout and stderr pipes of a spawned child:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *
 *  const script = 'console.log("out"); console.error("err");';
 *  const child = child_process.spawn(process.execPath, ['-e', script]);
 *  console.log(child.stdout.readAll().toString().trim()); // out
 *  console.log(child.stderr.readAll().toString().trim()); // err
 *  console.log(child.join()); // 0
 *  ```
 *
 *  Example 2 — kill a child and observe the exit event:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *  const coroutine = require('coroutine');
 *
 *  const child = child_process.spawn(process.execPath, ['-e', 'setTimeout(() => {}, 30000)']);
 *  child.on('exit', (code, signal) => {
 *      console.log('exit', code, signal); // exit null SIGTERM
 *  });
 *  child.kill(); // SIGTERM by default
 *  console.log('join', child.join()); // join -15
 *  coroutine.sleep(1);
 *  ```
 *
 *  Example 3 — fork a module and exchange IPC messages:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-ipc-'));
 *  const childCode = 'process.on("message", (m) => { process.send(m * 2);' +
 *      ' setTimeout(() => process.exit(0), 50); });';
 *  fs.writeFile(path.join(dir, 'echo.js'), childCode);
 *
 *  const child = child_process.fork(path.join(dir, 'echo.js'), { silent: true });
 *  child.on('message', (m) => console.log('message', m)); // message 42
 *  child.send(21);
 *  console.log('join', child.join()); // join 0
 *  console.log('connected', child.connected); // connected false
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 */
declare class Class_ChildProcess extends Class_EventEmitter {
    /**
     * @description Sends a signal to the process this object refers to
     *
     *       signal may be a number, or a name such as "SIGTERM"; the default is SIGTERM. The call only
     *       delivers the signal and returns immediately, so use join to wait for the exit. After the
     *       call `killed` is true; when the process dies from the signal `exitCode` becomes the negative
     *       signal number and the exit event reports (null, signal name). Killing a process that has
     *       already exited throws Error 3 (no such process). Numeric signals are POSIX only; on Windows
     *       use the signal names supported by the platform.
     *
     *      @param signal the signal to deliver
     *
     */
    kill(signal?: string | number): void;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *
     *       Blocks the current fiber until the process exits; the return value equals `exitCode` at
     *       that moment: 0-255 for a normal exit or the negative signal number for a signal death.
     *       Calling join after the exit returns the stored code. This is a fibjs extension; in Node.js
     *       wait for the 'exit' event or use the promise form of exec instead.
     *
     *       Example:
     *       ```JavaScript
     *       const child_process = require('child_process');
     *
     *       const child = child_process.spawn(process.execPath, ['-e', 'process.exit(7)']);
     *       console.log(child.join()); // 7
     *       console.log(child.exitCode); // 7
     *       ```
     *
     *       @return the exit code of the process
     *
     */
    join(): number;

    join(callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *
     *       Blocks the current fiber until the process exits; the return value equals `exitCode` at
     *       that moment: 0-255 for a normal exit or the negative signal number for a signal death.
     *       Calling join after the exit returns the stored code. This is a fibjs extension; in Node.js
     *       wait for the 'exit' event or use the promise form of exec instead.
     *
     *       Example:
     *       ```JavaScript
     *       const child_process = require('child_process');
     *
     *       const child = child_process.spawn(process.execPath, ['-e', 'process.exit(7)']);
     *       console.log(child.join()); // 7
     *       console.log(child.exitCode); // 7
     *       ```
     *
     *       @return the exit code of the process
     *
     */
    joinSync(): number;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *
     *       Blocks the current fiber until the process exits; the return value equals `exitCode` at
     *       that moment: 0-255 for a normal exit or the negative signal number for a signal death.
     *       Calling join after the exit returns the stored code. This is a fibjs extension; in Node.js
     *       wait for the 'exit' event or use the promise form of exec instead.
     *
     *       Example:
     *       ```JavaScript
     *       const child_process = require('child_process');
     *
     *       const child = child_process.spawn(process.execPath, ['-e', 'process.exit(7)']);
     *       console.log(child.join()); // 7
     *       console.log(child.exitCode); // 7
     *       ```
     *
     *       @return the exit code of the process
     *
     */
    joinAsync(): Promise<number>;

    /**
     * @description Queries whether the pipe to the child process is properly connected
     *
     *       True while the message channel is usable: for a fork child, or a spawn with an 'ipc' stdio
     *       entry, until the channel closes or the process exits. It is false for processes without an
     *       IPC channel. See send, disconnect and the message event.
     *
     */
    readonly connected: boolean;

    /**
     * @description Closes the ipc pipe to the child process
     *
     *       After the call `connected` is false and send fails; the disconnect event is emitted when
     *       the channel closes. Calling it when no channel is connected throws Error 20024 ("IPC
     *       channel is already disconnected").
     *
     */
    disconnect(): void;

    /**
     * @description Sends a message to the current child process
     *
     *       The value is serialized as JSON-compatible data and delivered to the child's message
     *       handler (process.on('message') in the child). There is no callback form; it throws Error
     *       20009 when the channel is not available (no 'ipc' stdio entry and no fork IPC), and the
     *       child must still be running.
     *
     *      @param msg the message to send
     *
     */
    send(msg: any): void;

    /**
     * @description Resizes the terminal of the current child process
     *
     *       Only available when the child runs in 'pty' mode; otherwise it throws Error 20024
     *       ("resize() only available in PTY mode"). Both dimensions must be positive, otherwise
     *       Error 20004; the default size is 80 columns by 24 rows.
     *
     *      @param cols the number of terminal columns
     *      @param rows the number of terminal rows
     *
     */
    resize(cols: number, rows: number): void;

    /**
     * @description Number of terminal columns of the child process
     *
     *       Only available in 'pty' mode; reading it for a non-pty child throws Error 20024 ("cols
     *       property only available in PTY mode"). The default is 80 and resize updates it.
     *
     */
    readonly cols: number;

    /**
     * @description Number of terminal rows of the child process
     *
     *       Only available in 'pty' mode; reading it for a non-pty child throws Error 20024 ("rows
     *       property only available in PTY mode"). The default is 24 and resize updates it.
     *
     */
    readonly rows: number;

    /**
     * @description Queries the memory used and the time spent by the current process
     *
     *      The report is taken from the child process, not from the fibjs process, and can be called
     *      while the child runs. The fields are `user` and `system` in microseconds (millionths of a
     *      second) and `rss` in bytes of physical memory. This is a fibjs extension.
     *
     *      The report looks similar to:
     *      ```JavaScript
     *      // fragment: report shape
     *      ({
     *        "user": 132379,
     *        "system": 50507,
     *        "rss": 8622080
     *      })
     *      ```
     *      @return returns the report containing the time information
     *
     */
    usage(): FIBJS.GeneralObject;

    /**
     * @description Reads the id of the process this object refers to
     *
     *       The operating system process id, available immediately after a successful spawn. A
     *       spawnSync result reports pid 0 when the process could not be spawned; a ChildProcess
     *       instance always has a real pid because spawn failures throw before it is returned.
     *
     */
    readonly pid: number;

    /**
     * @description Queries whether the process this object refers to has already been killed
     *
     *       True once kill has been called, or a timeout or AbortSignal has terminated the process;
     *       false for a process that ended on its own. Node.js sets killed only when a signal was
     *       successfully delivered, so the flag has slightly different semantics there.
     *
     */
    readonly killed: boolean;

    /**
     * @description Queries and sets the exit code of the current process
     *
     *       null while the process is still running (see join). After the exit it holds 0-255 for a
     *       normal exit or the negative signal number for a signal death (-15 for SIGTERM, -9 for
     *       SIGKILL); Node.js reports null in the signal case and uses signalCode instead.
     *
     */
    readonly exitCode: number;

    /**
     * @description Reads the standard input object of the process this object refers to
     *
     *       A writable Stream when the descriptor is configured as 'pipe'; null for 'ignore' and
     *       'inherit'. Write input and call close() to signal EOF; remember to close a piped stdin when
     *       the child waits for input. In 'pty' mode stdin is the terminal.
     *
     */
    readonly stdin: Class_Stream;

    /**
     * @description Reads the standard output object of the process this object refers to
     *
     *       A readable Stream when the descriptor is configured as 'pipe'; null for 'ignore' and
     *       'inherit'. Read it (for example with readAll) to drain the pipe; in 'pty' mode stdout is
     *       the terminal.
     *
     *       Example:
     *       ```JavaScript
     *       const child_process = require('child_process');
     *
     *       const child = child_process.spawn(process.execPath, ['-e', 'console.log("data")']);
     *       console.log(child.stdout.readAll().toString().trim()); // data
     *       console.log(child.join()); // 0
     *       ```
     *
     */
    readonly stdout: Class_Stream;

    /**
     * @description Reads the standard error object of the process this object refers to
     *
     *       A readable Stream when the descriptor is configured as 'pipe'; null for 'ignore' and
     *       'inherit'. Read it to collect error output; a child can block when an undrained pipe
     *       buffer fills up.
     *
     */
    readonly stderr: Class_Stream;

    /**
     * @description Reads the list of standard IO objects of the process this object refers to
     *
     *       The array is indexed by file descriptor and corresponds to the stdio option passed to
     *       spawn: pipe entries are Stream objects, all other entries (ignore, inherit, ipc) are null.
     *       Extra pipe descriptors (3 and above) are reached through this array; indexes 0, 1 and 2
     *       mirror stdin, stdout and stderr.
     *
     */
    readonly stdio: any[];

    /**
     * @description Queries and binds the process exit event, equivalent to on("exit", func)
     *
     *      The handler receives (code, signal): code is the exit status 0-255 for a normal exit and
     *      null when the process was killed by a signal; signal is the signal name for a signal death
     *      and null on a normal exit. The implementation also emits the Node-style 'close' event with
     *      the same arguments after the stdio streams have closed.
     *
     *      @param code the exit code, null when the process was killed by a signal
     *      @param signal the signal name, null when the process exited normally
     *
     */
    on(event: "exit", listener: (code: any, signal: any)=>void): this;

    once(event: "exit", listener: (code: any, signal: any)=>void): this;

    off(event: "exit", listener: (code: any, signal: any)=>void): this;

    addListener(event: "exit", listener: (code: any, signal: any)=>void): this;

    removeListener(event: "exit", listener: (code: any, signal: any)=>void): this;

    addEventListener(event: "exit", listener: (code: any, signal: any)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "exit", listener: (code: any, signal: any)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "exit", listener: (code: any, signal: any)=>void): this;

    prependOnceListener(event: "exit", listener: (code: any, signal: any)=>void): this;

    /**
     * @description Queries and binds the process exit event, equivalent to on("exit", func)
     *
     *      The handler receives (code, signal): code is the exit status 0-255 for a normal exit and
     *      null when the process was killed by a signal; signal is the signal name for a signal death
     *      and null on a normal exit. The implementation also emits the Node-style 'close' event with
     *      the same arguments after the stdio streams have closed.
     *
     *      @param code the exit code, null when the process was killed by a signal
     *      @param signal the signal name, null when the process exited normally
     *
     */
    onexit: ((code: any, signal: any)=>void) | null;

    /**
     * @description Queries and binds the child process message event, equivalent to on("message", func)
     *
     *      Emitted when a message sent from the child (process.send) is received; the value is the
     *      decoded message. Only meaningful for children that have an IPC channel, which fork creates
     *      by default.
     *
     *      @param msg the decoded message sent by the child process
     *
     */
    on(event: "message", listener: (msg: any)=>void): this;

    once(event: "message", listener: (msg: any)=>void): this;

    off(event: "message", listener: (msg: any)=>void): this;

    addListener(event: "message", listener: (msg: any)=>void): this;

    removeListener(event: "message", listener: (msg: any)=>void): this;

    addEventListener(event: "message", listener: (msg: any)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: (msg: any)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: (msg: any)=>void): this;

    prependOnceListener(event: "message", listener: (msg: any)=>void): this;

    /**
     * @description Queries and binds the child process message event, equivalent to on("message", func)
     *
     *      Emitted when a message sent from the child (process.send) is received; the value is the
     *      decoded message. Only meaningful for children that have an IPC channel, which fork creates
     *      by default.
     *
     *      @param msg the decoded message sent by the child process
     *
     */
    onmessage: ((msg: any)=>void) | null;

    /**
     * @description Queries and binds the child process spawn event, equivalent to on("spawn", func)
     *
     *      Emitted after the process has been spawned successfully. It is not emitted when the spawn
     *      fails: in that case spawn throws before a ChildProcess exists and spawnSync reports the
     *      failure in its error field instead.
     *
     */
    on(event: "spawn", listener: ()=>void): this;

    once(event: "spawn", listener: ()=>void): this;

    off(event: "spawn", listener: ()=>void): this;

    addListener(event: "spawn", listener: ()=>void): this;

    removeListener(event: "spawn", listener: ()=>void): this;

    addEventListener(event: "spawn", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "spawn", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "spawn", listener: ()=>void): this;

    prependOnceListener(event: "spawn", listener: ()=>void): this;

    /**
     * @description Queries and binds the child process spawn event, equivalent to on("spawn", func)
     *
     *      Emitted after the process has been spawned successfully. It is not emitted when the spawn
     *      fails: in that case spawn throws before a ChildProcess exists and spawnSync reports the
     *      failure in its error field instead.
     *
     */
    onspawn: (()=>void) | null;

    /**
     * @description Queries and binds the child process disconnect event, equivalent to on("disconnect", func)
     *
     *      Emitted when the IPC channel closes, either after disconnect is called or when the channel
     *      is torn down (for example because the child exited). Only meaningful for children with an
     *      IPC channel.
     *
     */
    on(event: "disconnect", listener: ()=>void): this;

    once(event: "disconnect", listener: ()=>void): this;

    off(event: "disconnect", listener: ()=>void): this;

    addListener(event: "disconnect", listener: ()=>void): this;

    removeListener(event: "disconnect", listener: ()=>void): this;

    addEventListener(event: "disconnect", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "disconnect", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "disconnect", listener: ()=>void): this;

    prependOnceListener(event: "disconnect", listener: ()=>void): this;

    /**
     * @description Queries and binds the child process disconnect event, equivalent to on("disconnect", func)
     *
     *      Emitted when the IPC channel closes, either after disconnect is called or when the channel
     *      is torn down (for example because the child exited). Only meaningful for children with an
     *      IPC channel.
     *
     */
    ondisconnect: (()=>void) | null;

    /**
     * @description Keeps the fibjs process alive while this child is running
     *
     *       A referenced child prevents the fibjs process from exiting while its event loop is
     *       otherwise empty; children are referenced by default. Returns the object so calls can be
     *       chained. See unref for the opposite behavior.
     *
     *      @return returns the current object
     *
     */
    ref(): Class_ChildProcess;

    /**
     * @description Allows the fibjs process to exit while this child is still running
     *
     *       After unref the child no longer holds the fibjs process open, so the parent can exit (and
     *       the child keeps running). Returns the object so calls can be chained. The parent should
     *       still observe or kill the child when its result matters.
     *
     *      @return returns the current object
     *
     */
    unref(): Class_ChildProcess;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * The promise variant of the ChildProcess class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_ChildProcessPromise extends Class_EventEmitter {
    /**
     * @description Sends a signal to the process this object refers to
     *
     *       signal may be a number, or a name such as "SIGTERM"; the default is SIGTERM. The call only
     *       delivers the signal and returns immediately, so use join to wait for the exit. After the
     *       call `killed` is true; when the process dies from the signal `exitCode` becomes the negative
     *       signal number and the exit event reports (null, signal name). Killing a process that has
     *       already exited throws Error 3 (no such process). Numeric signals are POSIX only; on Windows
     *       use the signal names supported by the platform.
     *
     *      @param signal the signal to deliver
     *
     */
    kill(signal?: string | number): void;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *
     *       Blocks the current fiber until the process exits; the return value equals `exitCode` at
     *       that moment: 0-255 for a normal exit or the negative signal number for a signal death.
     *       Calling join after the exit returns the stored code. This is a fibjs extension; in Node.js
     *       wait for the 'exit' event or use the promise form of exec instead.
     *
     *       Example:
     *       ```JavaScript
     *       const child_process = require('child_process');
     *
     *       const child = child_process.spawn(process.execPath, ['-e', 'process.exit(7)']);
     *       console.log(child.join()); // 7
     *       console.log(child.exitCode); // 7
     *       ```
     *
     *       @return the exit code of the process
     *
     */
    join(): Promise<number>;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *
     *       Blocks the current fiber until the process exits; the return value equals `exitCode` at
     *       that moment: 0-255 for a normal exit or the negative signal number for a signal death.
     *       Calling join after the exit returns the stored code. This is a fibjs extension; in Node.js
     *       wait for the 'exit' event or use the promise form of exec instead.
     *
     *       Example:
     *       ```JavaScript
     *       const child_process = require('child_process');
     *
     *       const child = child_process.spawn(process.execPath, ['-e', 'process.exit(7)']);
     *       console.log(child.join()); // 7
     *       console.log(child.exitCode); // 7
     *       ```
     *
     *       @return the exit code of the process
     *
     */
    joinSync(): number;

    /**
     * @description Waits for the process this object refers to to exit and returns the exit code
     *
     *       Blocks the current fiber until the process exits; the return value equals `exitCode` at
     *       that moment: 0-255 for a normal exit or the negative signal number for a signal death.
     *       Calling join after the exit returns the stored code. This is a fibjs extension; in Node.js
     *       wait for the 'exit' event or use the promise form of exec instead.
     *
     *       Example:
     *       ```JavaScript
     *       const child_process = require('child_process');
     *
     *       const child = child_process.spawn(process.execPath, ['-e', 'process.exit(7)']);
     *       console.log(child.join()); // 7
     *       console.log(child.exitCode); // 7
     *       ```
     *
     *       @return the exit code of the process
     *
     */
    joinAsync(): Promise<number>;

    /**
     * @description Queries whether the pipe to the child process is properly connected
     *
     *       True while the message channel is usable: for a fork child, or a spawn with an 'ipc' stdio
     *       entry, until the channel closes or the process exits. It is false for processes without an
     *       IPC channel. See send, disconnect and the message event.
     *
     */
    readonly connected: boolean;

    /**
     * @description Closes the ipc pipe to the child process
     *
     *       After the call `connected` is false and send fails; the disconnect event is emitted when
     *       the channel closes. Calling it when no channel is connected throws Error 20024 ("IPC
     *       channel is already disconnected").
     *
     */
    disconnect(): void;

    /**
     * @description Sends a message to the current child process
     *
     *       The value is serialized as JSON-compatible data and delivered to the child's message
     *       handler (process.on('message') in the child). There is no callback form; it throws Error
     *       20009 when the channel is not available (no 'ipc' stdio entry and no fork IPC), and the
     *       child must still be running.
     *
     *      @param msg the message to send
     *
     */
    send(msg: any): void;

    /**
     * @description Resizes the terminal of the current child process
     *
     *       Only available when the child runs in 'pty' mode; otherwise it throws Error 20024
     *       ("resize() only available in PTY mode"). Both dimensions must be positive, otherwise
     *       Error 20004; the default size is 80 columns by 24 rows.
     *
     *      @param cols the number of terminal columns
     *      @param rows the number of terminal rows
     *
     */
    resize(cols: number, rows: number): void;

    /**
     * @description Number of terminal columns of the child process
     *
     *       Only available in 'pty' mode; reading it for a non-pty child throws Error 20024 ("cols
     *       property only available in PTY mode"). The default is 80 and resize updates it.
     *
     */
    readonly cols: number;

    /**
     * @description Number of terminal rows of the child process
     *
     *       Only available in 'pty' mode; reading it for a non-pty child throws Error 20024 ("rows
     *       property only available in PTY mode"). The default is 24 and resize updates it.
     *
     */
    readonly rows: number;

    /**
     * @description Queries the memory used and the time spent by the current process
     *
     *      The report is taken from the child process, not from the fibjs process, and can be called
     *      while the child runs. The fields are `user` and `system` in microseconds (millionths of a
     *      second) and `rss` in bytes of physical memory. This is a fibjs extension.
     *
     *      The report looks similar to:
     *      ```JavaScript
     *      // fragment: report shape
     *      ({
     *        "user": 132379,
     *        "system": 50507,
     *        "rss": 8622080
     *      })
     *      ```
     *      @return returns the report containing the time information
     *
     */
    usage(): FIBJS.GeneralObject;

    /**
     * @description Reads the id of the process this object refers to
     *
     *       The operating system process id, available immediately after a successful spawn. A
     *       spawnSync result reports pid 0 when the process could not be spawned; a ChildProcess
     *       instance always has a real pid because spawn failures throw before it is returned.
     *
     */
    readonly pid: number;

    /**
     * @description Queries whether the process this object refers to has already been killed
     *
     *       True once kill has been called, or a timeout or AbortSignal has terminated the process;
     *       false for a process that ended on its own. Node.js sets killed only when a signal was
     *       successfully delivered, so the flag has slightly different semantics there.
     *
     */
    readonly killed: boolean;

    /**
     * @description Queries and sets the exit code of the current process
     *
     *       null while the process is still running (see join). After the exit it holds 0-255 for a
     *       normal exit or the negative signal number for a signal death (-15 for SIGTERM, -9 for
     *       SIGKILL); Node.js reports null in the signal case and uses signalCode instead.
     *
     */
    readonly exitCode: number;

    /**
     * @description Reads the standard input object of the process this object refers to
     *
     *       A writable Stream when the descriptor is configured as 'pipe'; null for 'ignore' and
     *       'inherit'. Write input and call close() to signal EOF; remember to close a piped stdin when
     *       the child waits for input. In 'pty' mode stdin is the terminal.
     *
     */
    readonly stdin: Class_StreamPromise;

    /**
     * @description Reads the standard output object of the process this object refers to
     *
     *       A readable Stream when the descriptor is configured as 'pipe'; null for 'ignore' and
     *       'inherit'. Read it (for example with readAll) to drain the pipe; in 'pty' mode stdout is
     *       the terminal.
     *
     *       Example:
     *       ```JavaScript
     *       const child_process = require('child_process');
     *
     *       const child = child_process.spawn(process.execPath, ['-e', 'console.log("data")']);
     *       console.log(child.stdout.readAll().toString().trim()); // data
     *       console.log(child.join()); // 0
     *       ```
     *
     */
    readonly stdout: Class_StreamPromise;

    /**
     * @description Reads the standard error object of the process this object refers to
     *
     *       A readable Stream when the descriptor is configured as 'pipe'; null for 'ignore' and
     *       'inherit'. Read it to collect error output; a child can block when an undrained pipe
     *       buffer fills up.
     *
     */
    readonly stderr: Class_StreamPromise;

    /**
     * @description Reads the list of standard IO objects of the process this object refers to
     *
     *       The array is indexed by file descriptor and corresponds to the stdio option passed to
     *       spawn: pipe entries are Stream objects, all other entries (ignore, inherit, ipc) are null.
     *       Extra pipe descriptors (3 and above) are reached through this array; indexes 0, 1 and 2
     *       mirror stdin, stdout and stderr.
     *
     */
    readonly stdio: any[];

    /**
     * @description Queries and binds the process exit event, equivalent to on("exit", func)
     *
     *      The handler receives (code, signal): code is the exit status 0-255 for a normal exit and
     *      null when the process was killed by a signal; signal is the signal name for a signal death
     *      and null on a normal exit. The implementation also emits the Node-style 'close' event with
     *      the same arguments after the stdio streams have closed.
     *
     *      @param code the exit code, null when the process was killed by a signal
     *      @param signal the signal name, null when the process exited normally
     *
     */
    onexit: ((code: any, signal: any)=>void) | null;

    /**
     * @description Queries and binds the child process message event, equivalent to on("message", func)
     *
     *      Emitted when a message sent from the child (process.send) is received; the value is the
     *      decoded message. Only meaningful for children that have an IPC channel, which fork creates
     *      by default.
     *
     *      @param msg the decoded message sent by the child process
     *
     */
    onmessage: ((msg: any)=>void) | null;

    /**
     * @description Queries and binds the child process spawn event, equivalent to on("spawn", func)
     *
     *      Emitted after the process has been spawned successfully. It is not emitted when the spawn
     *      fails: in that case spawn throws before a ChildProcess exists and spawnSync reports the
     *      failure in its error field instead.
     *
     */
    onspawn: (()=>void) | null;

    /**
     * @description Queries and binds the child process disconnect event, equivalent to on("disconnect", func)
     *
     *      Emitted when the IPC channel closes, either after disconnect is called or when the channel
     *      is torn down (for example because the child exited). Only meaningful for children with an
     *      IPC channel.
     *
     */
    ondisconnect: (()=>void) | null;

    /**
     * @description Keeps the fibjs process alive while this child is running
     *
     *       A referenced child prevents the fibjs process from exiting while its event loop is
     *       otherwise empty; children are referenced by default. Returns the object so calls can be
     *       chained. See unref for the opposite behavior.
     *
     *      @return returns the current object
     *
     */
    ref(): Class_ChildProcess;

    /**
     * @description Allows the fibjs process to exit while this child is still running
     *
     *       After unref the child no longer holds the fibjs process open, so the parent can exit (and
     *       the child keeps running). Returns the object so calls can be chained. The parent should
     *       still observe or kill the child when its result matters.
     *
     *      @return returns the current object
     *
     */
    unref(): Class_ChildProcess;

}


declare namespace Class_ChildProcess {
    const promises: FIBJS.GeneralObject;
}
