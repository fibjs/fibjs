/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ChildProcess.d.ts" />
/**
 * @description Child process management module
 *  Usage:
 *  ```JavaScript
 *  var child_process = require("child_process");
 *  var child = child_process.spawn("ls");
 *  ```
 *
 * When creating a child process, the options.stdio option is used to configure the pipes established between the parent process and the child process. By default, the child process's stdin, stdout and stderr are redirected to the corresponding stdin, stdout and stderr streams on the ChildProcess object. This is equivalent to setting options.stdio to ['pipe', 'pipe', 'pipe'].
 *
 * For convenience, options.stdio can be one of the following strings:
 *
 * - 'pipe': equivalent to ['pipe', 'pipe', 'pipe'] (the default).
 * - 'ignore': equivalent to ['ignore', 'ignore', 'ignore'].
 * - 'inherit': equivalent to ['inherit', 'inherit', 'inherit'] or [0, 1, 2].
 * - 'pty': equivalent to ['pty', 'pty', 'pty'].
 *
 * Otherwise, the value of options.stdio must be an array (where each index corresponds to a file descriptor in the child process). File descriptors 0, 1 and 2 correspond to stdin, stdout and stderr respectively. Other file descriptors can be specified to create additional pipes between the parent process and the child process. The value can be one of the following:
 *
 * 1. 'pipe': creates a pipe between the child process and the parent process. The parent end of the pipe is exposed to the parent process as the stdio[fd] property on the child_process object. The pipes created for file descriptors 0, 1 and 2 are also available as stdin, stdout and stderr respectively.
 * 2. 'ignore': instructs fibjs to ignore the file descriptor in the child process. Although fibjs will always open file descriptors 0, 1 and 2 for the processes it spawns, setting the file descriptor to 'ignore' makes fibjs open /dev/null and attach it to the child process's file descriptor.
 * 3. 'inherit': passes the corresponding stdio stream to or from the parent process. In the first three positions this is equivalent to process.stdin, process.stdout and process.stderr respectively. In any other position it is equivalent to 'ignore'.
 * 4. 'pty': the child process will execute in a virtual terminal. In this case only stdin and stdout are valid.
 * 5. Positive integer: the integer value is interpreted as a file descriptor currently open in the parent process. It is shared with the child process, similar to the way a <Stream> object is shared. Passing a socket is not supported on Windows.
 * 6. null or undefined: use the default value. For file descriptors 0, 1 and 2 of stdio (in other words, stdin, stdout and stderr), pipes will be created. For file descriptors 3 and greater, the default is 'ignore'.
 *
 * ```JavaScript
 * const { spawn } = require('child_process');
 *
 * // child process uses parent's stdio
 * spawn('prg', [], { stdio: 'inherit' });
 *
 * // child process uses parent's stderr
 * spawn('prg', [], { stdio: ['pipe', 'pipe', process.stderr] });
 * ```
 *
 *  The following points should be noted:
 *  - `child_process.exec(command, args)` on windows does not automatically use cmd.exe as the execution environment for the command parameter;
 *  - child_process.[spawn|exec|execFile|run] are async-style functions combining synchronous and callback styles:
 *    - if the last parameter is not a function, it is synchronous;
 *    - if a function is passed as the last parameter, it is asynchronous;
 *  - the result returned by child_process.[exec|execFile] is an object containing fields such as stdout and stderr;
 *  - `child_process.run` is an API specific to fibjs
 *
 */
declare module 'child_process' {
    /**
     * @description Spawns a child process with the given command
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "stdio": Array | String, // configure the pipes that are established between the parent and child process
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // configure the group identity of the process
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run. If timeout > 0, the process will be killed with killSignal after timeout milliseconds. Default: 0 (no timeout)
     *         "killSignal": "SIGTERM", // the signal to use when the spawned process is killed by timeout or abort signal. Default: 'SIGTERM'
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the child process object
     *
     */
    function spawn(command: string, args: any[], options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description Spawns a child process with the given command
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "stdio": Array | String, // configure the pipes that are established between the parent and child process
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // configure the group identity of the process
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run. If timeout > 0, the process will be killed with killSignal after timeout milliseconds. Default: 0 (no timeout)
     *         "killSignal": "SIGTERM", // the signal to use when the spawned process is killed by timeout or abort signal. Default: 'SIGTERM'
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the child process object
     *
     */
    function spawn(command: string, options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description Executes a command in a shell and buffers the output; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "encoding": "utf8", // specify the character encoding used to decode the stdout and stderr output
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24, // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run, default to no limit
     *         "killSignal": "SIGTERM" // the signal to be used when the spawned process will be killed by timeout, default to "SIGTERM"
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the stdio output of the child process
     *
     */
    function exec(command: string, options?: FIBJS.GeneralObject): [stdout: any, stderr: any, exitCode: number];

    function exec(command: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: [stdout: any, stderr: any, exitCode: number])=>any): void;

    /**
     * @description Directly executes the specified file and buffers the output; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "encoding": "utf8", // specify the character encoding used to decode the stdout and stderr output
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24, // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run, default to no limit
     *         "killSignal": "SIGTERM" // the signal to be used when the spawned process will be killed by timeout, default to "SIGTERM"
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the stdio output of the child process
     *
     */
    function execFile(command: string, args: any[], options?: FIBJS.GeneralObject): [stdout: any, stderr: any, exitCode: number];

    function execFile(command: string, args: any[], options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: [stdout: any, stderr: any, exitCode: number])=>any): void;

    /**
     * @description Directly executes the specified file and buffers the output; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "encoding": "utf8", // specify the character encoding used to decode the stdout and stderr output
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24, // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run, default to no limit
     *         "killSignal": "SIGTERM" // the signal to be used when the spawned process will be killed by timeout, default to "SIGTERM"
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the stdio output of the child process
     *
     */
    function execFile(command: string, options?: FIBJS.GeneralObject): [stdout: any, stderr: any, exitCode: number];

    function execFile(command: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: [stdout: any, stderr: any, exitCode: number])=>any): void;

    /**
     * @description Spawns a child process with the given command
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "stdio": Array | String, // configure the pipes that are established between the parent and child process
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // configure the group identity of the process
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24, // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run, default to no limit
     *         "killSignal": "SIGTERM" // the signal to be used when the spawned process will be killed by timeout, default to "SIGTERM"
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the child process result
     *
     */
    function spawnSync(command: string, args: any[], options?: FIBJS.GeneralObject): [pid: number, output: NArray, stdout: any, stderr: any, status: number, signal: any, error: any];

    /**
     * @description Spawns a child process with the given command
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "stdio": Array | String, // configure the pipes that are established between the parent and child process
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24, // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run, default to no limit
     *         "killSignal": "SIGTERM" // the signal to be used when the spawned process will be killed by timeout, default to "SIGTERM"
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the child process result
     *
     */
    function spawnSync(command: string, options?: FIBJS.GeneralObject): [pid: number, output: NArray, stdout: any, stderr: any, status: number, signal: any, error: any];

    /**
     * @description Synchronously executes a command in a shell and buffers the output
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "encoding": "utf8", // specify the character encoding used to decode the stdout and stderr output
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // configure the group identity of the process
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24, // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run, default to no limit
     *         "killSignal": "SIGTERM" // the signal to be used when the spawned process will be killed by timeout, default to "SIGTERM"
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the stdout output of the child process, a Buffer by default; returns a string when options.encoding specifies an encoding
     *
     */
    function execSync(command: string, options?: FIBJS.GeneralObject): any;

    /**
     * @description Directly synchronously executes the specified file and buffers the output
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "encoding": "utf8", // specify the character encoding used to decode the stdout and stderr output
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // configure the group identity of the process
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24, // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run, default to no limit
     *         "killSignal": "SIGTERM" // the signal to be used when the spawned process will be killed by timeout, default to "SIGTERM"
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the stdout output of the child process, a Buffer by default; returns a string when options.encoding specifies an encoding
     *
     */
    function execFileSync(command: string, args: any[], options?: FIBJS.GeneralObject): any;

    /**
     * @description Directly synchronously executes the specified file and buffers the output
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "encoding": "utf8", // specify the character encoding used to decode the stdout and stderr output
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // configure the group identity of the process
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24, // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *         "timeout": 0, // the maximum amount of time (in milliseconds) the process is allowed to run, default to no limit
     *         "killSignal": "SIGTERM" // the signal to be used when the spawned process will be killed by timeout, default to "SIGTERM"
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the stdout output of the child process, a Buffer by default; returns a string when options.encoding specifies an encoding
     *
     */
    function execFileSync(command: string, options?: FIBJS.GeneralObject): any;

    /**
     * @description Executes a module in a child process
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "stdio": Array | String, // configure the pipes that are established between the parent and child process
     *         "silent": false, // if true, stdin, stdout and stderr of the child will be piped to the parent, otherwise they will be inherited (fork only, ignored when stdio is provided)
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param module specifies the module to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the child process object
     *
     */
    function fork(module: string, args: any[], options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description Executes a module in a child process
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "stdio": Array | String, // configure the pipes that are established between the parent and child process
     *         "silent": false, // if true, stdin, stdout and stderr of the child will be piped to the parent, otherwise they will be inherited (fork only, ignored when stdio is provided)
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param module specifies the module to run
     *      @param options specifies the creation options
     *      @return returns the child process object
     *
     */
    function fork(module: string, options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description Directly executes the specified file and returns the exitCode; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function run(command: string, args: any[], options?: FIBJS.GeneralObject): number;

    function run(command: string, args: any[], options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Directly executes the specified file and returns the exitCode; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function runSync(command: string, args: any[], options?: FIBJS.GeneralObject): number;

    /**
     * @description Directly executes the specified file and returns the exitCode; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function runAsync(command: string, args: any[], options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description Directly executes the specified file and returns the exitCode; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function run(command: string, options?: FIBJS.GeneralObject): number;

    function run(command: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Directly executes the specified file and returns the exitCode; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function runSync(command: string, options?: FIBJS.GeneralObject): number;

    /**
     * @description Directly executes the specified file and returns the exitCode; when executed in callback style, the function returns the child process object
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *         "cwd": "", // working directory of the child process, default to current directory
     *         "env": {}, // key-value pairs of environment variables to add to the child's environment
     *         "detached": false, // child process will be a leader of a new process group, default to false
     *         "uid": 0, // configure the user identity of the process
     *         "gid": 0, // con
     *         "windowsVerbatimArguments": false, // do not execute any quote or escape processing on Windows. Ignored on Unix. When specified, the command line string is passed directly to the underlying operating system shell without any processing whatsoever. This is set to true automatically when the shell option is specified and is CMD.
     *         "windowsHide": false, // hide the subprocess console window that would normally be created on Windows systems. This option has no effect on non-Windows systems.
     *         "cols": 80, // specify the initial number of columns for the PTY (only for stdio: 'pty')
     *         "rows": 24 // specify the initial number of rows for the PTY (only for stdio: 'pty')
     *      }
     *      ```
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function runAsync(command: string, options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description Executes a command in a shell using string template syntax and buffers the output
     *
     *      sh is a wrapper around the exec method for quickly executing shell commands; it supports string template syntax, for example:
     *      ```JavaScript
     *        const $ = require("child_process").sh;
     *        var ret = $`ls -l`;
     *        console.log(ret);
     *       ```
     *       Because sh is a template function, templates can be used conveniently in commands, for example:
     *       ```JavaScript
     *        const $ = require("child_process").sh;
     *        var ret = $`ls -l ${__dirname}`;
     *        console.log(ret);
     *       ```
     *       You can also conveniently embed arrays in commands, for example:
     *       ```JavaScript
     *        const $ = require("child_process").sh;
     *        const words = [
     *          "hello",
     *          "world"
     *        ]
     *        var ret = $`echo ${words}`;
     *        console.log(ret);
     *       ```
     *       sh automatically removes the final newline returned by the command, making it convenient to use in the next command, for example:
     *       ```JavaScript
     *        const $ = require("child_process").sh;
     *        var world = $`echo world`;
     *        var ret = $`echo hello ${world}`;
     *        console.log(ret);
     *       ```
     *
     *       @param strings specifies the command to run
     *       @param args specifies the list of string arguments
     *       @return returns the stdio output of the child process
     *
     */
    function sh(strings: any[], ...args: any[]): string;

    /**
     * @description Creates an ssh execution function
     *
     *     The supported options are as follows:
     *      ```JavaScript
     *      {
     *          "user": "", // ssh user
     *          "port": 22, // ssh port
     *      }
     *      ```
     *
     *      ssh is a wrapper around the execFile method for quickly executing ssh shell commands; it supports string template syntax, for example:
     *      ```JavaScript
     *        const $ = require("child_process").ssh('remote');
     *        var ret = $`ls -l`;
     *        console.log(ret);
     *       ```
     *       Because sh is a template function, templates can be used conveniently in commands, for example:
     *       ```JavaScript
     *        const $ = require("child_process").ssh('remote');
     *        var ret = $`ls -l ${__dirname}`;
     *        console.log(ret);
     *       ```
     *       You can also conveniently embed arrays in commands, for example:
     *       ```JavaScript
     *        const $ = require("child_process").ssh('remote');
     *        const words = [
     *          "hello",
     *          "world"
     *        ]
     *        var ret = $`echo ${words}`;
     *        console.log(ret);
     *       ```
     *       sh automatically removes the final newline returned by the command, making it convenient to use in the next command, for example:
     *       ```JavaScript
     *        const $ = require("child_process").ssh('remote');
     *        var world = $`echo world`;
     *        var ret = $`echo hello ${world}`;
     *        console.log(ret);
     *       ```
     *
     *     @param host specifies the remote host address
     *     @param options specifies the ssh connection parameters
     *     @return returns the child process object
     *
     */
    function ssh(host: string, options?: FIBJS.GeneralObject): (...args: any[])=>any;

}

