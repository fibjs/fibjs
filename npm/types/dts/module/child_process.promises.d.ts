/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ChildProcess.d.ts" />
/**
 * The promise variant of the child_process module: async members return a Promise as their primary form.
 */
declare module 'child_process/promises' {
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
    function exec(command: string, options?: FIBJS.GeneralObject): Promise<{
        stdout: any;
        stderr: any;
        exitCode: number;
    }>;

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
    function execAsync(command: string, options?: FIBJS.GeneralObject): Promise<{
        stdout: any;
        stderr: any;
        exitCode: number;
    }>;

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
    function execFile(command: string, args: any[], options?: FIBJS.GeneralObject): Promise<{
        stdout: any;
        stderr: any;
        exitCode: number;
    }>;

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
    function execFileAsync(command: string, args: any[], options?: FIBJS.GeneralObject): Promise<{
        stdout: any;
        stderr: any;
        exitCode: number;
    }>;

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
    function execFile(command: string, options?: FIBJS.GeneralObject): Promise<{
        stdout: any;
        stderr: any;
        exitCode: number;
    }>;

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
    function execFileAsync(command: string, options?: FIBJS.GeneralObject): Promise<{
        stdout: any;
        stderr: any;
        exitCode: number;
    }>;

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
    function spawnSync(command: string, args: any[], options?: FIBJS.GeneralObject): {
        pid: number;
        output: any[];
        stdout: any;
        stderr: any;
        status: number;
        signal: any;
        error: any;
    };

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
    function spawnSync(command: string, options?: FIBJS.GeneralObject): {
        pid: number;
        output: any[];
        stdout: any;
        stderr: any;
        status: number;
        signal: any;
        error: any;
    };

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
    function run(command: string, args: any[], options?: FIBJS.GeneralObject): Promise<number>;

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
    function run(command: string, options?: FIBJS.GeneralObject): Promise<number>;

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
    function sh(strings: any[], ...args: any[]): Promise<string>;

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
    function shSync(strings: any[], ...args: any[]): string;

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
    function shAsync(strings: any[], ...args: any[]): Promise<string>;

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


declare module "child_process" {
    const promises: typeof import("child_process/promises");
}
