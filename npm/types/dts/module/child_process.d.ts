/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ChildProcess.d.ts" />
/**
 * @description The child_process module runs external programs and JavaScript modules in child processes; it provides streaming, buffered and synchronous call forms
 *
 *  Main capabilities:
 *
 *  - **Streaming**: `spawn` starts a process immediately and exposes its stdio pipes through a
 *    ChildProcess; `spawnSync` is the blocking variant with a complete result object;
 *  - **Buffered execution**: `exec` runs a command in the platform shell and `execFile` runs a file
 *    directly; both return `{ stdout, stderr, exitCode }` synchronously or a ChildProcess in the
 *    callback form;
 *  - **Convenience**: `execSync` and `execFileSync` return stdout and throw on a non-zero exit;
 *    `run` executes a command and returns only its exit code (fibjs extension);
 *  - **Modules and IPC**: `fork` runs a JavaScript module in a new fibjs process with a message
 *    channel;
 *  - **Shell helpers**: `sh` runs shell commands as a template tag and `ssh` returns a template
 *    function that executes commands on a remote host (fibjs extensions).
 *
 *  Concepts:
 *
 *  - **Call forms**: `exec`, `execFile` and `run` run synchronously when the last argument is not a
 *    function and asynchronously when a trailing callback is passed. The asynchronous form returns
 *    a ChildProcess and the callback receives `(err, stdout, stderr, exitCode)` (`run` receives
 *    `(err, exitCode)`). `spawn` has no callback overload and always returns a ChildProcess
 *    immediately; `fork` and the `*Sync` functions are synchronous.
 *  - **spawn vs exec vs execFile**: `spawn` passes the program and the argv array to the operating
 *    system without a shell; `exec` passes one string to the platform shell (`/bin/sh -c` on POSIX,
 *    `cmd.exe /d /s /c` on Windows), so pipes, redirection and quoting are interpreted by the
 *    shell; `execFile` starts a file with an explicit argv array and no shell.
 *  - **stdio**: `options.stdio` configures the child descriptors. It may be the string `'pipe'`,
 *    `'ignore'`, `'inherit'` or `'pty'`, or an array with one entry per descriptor: `'pipe'`,
 *    `'ignore'`, `'inherit'`, `'ipc'`, a positive integer naming a parent descriptor, or
 *    null/undefined for the default. Descriptors 0, 1 and 2 become ChildProcess stdin, stdout and
 *    stderr; extra pipe descriptors appear in the ChildProcess stdio array. `'pty'` allocates a
 *    pseudo terminal (stdin and stdout only) with the initial `cols`/`rows` size.
 *  - **Environment and lifetime**: `options.cwd` sets the working directory, `options.env` adds
 *    environment entries, `options.uid`/`gid` set the POSIX identity, `options.detached` starts the
 *    child as a process group leader, and `windowsHide`/`windowsVerbatimArguments` apply on
 *    Windows. `options.timeout` kills the child with `killSignal` (SIGTERM by default) after the
 *    given milliseconds and `options.signal` (AbortSignal) kills it on abort. Children are not
 *    killed when the parent exits, and a running child keeps the fibjs process alive until it is
 *    unref'd; kill children explicitly and always join or observe them.
 *  - **Exit codes and errors**: a non-zero exit is normal data for exec, execFile, run and
 *    spawnSync (`exitCode`/`status`); execSync and execFileSync throw an Error carrying
 *    Node-compatible `status`, `stdout`, `stderr` and `output` fields. A child killed by a signal
 *    reports the negative signal number (-15 for SIGTERM, -9 for SIGKILL). Spawn failures throw
 *    (spawn/exec/execFile/fork) or set `error` (spawnSync) with `code`/`errno`/`syscall`/`path`.
 *  - **Node.js differences**: fibjs adds `run`, `sh`, `ssh`, the `input` option (write to the
 *    child's stdin), the `encoding` option accepting `'buffer'`, the four-argument callback with
 *    exitCode and pty stdio. It has no `options.shell` (only exec uses a shell) and no
 *    `options.maxBuffer`; `spawn` throws on failure instead of emitting an 'error' event, and a
 *    signal death is reported as a negative exit code instead of Node's `signalCode`.
 *
 *  Import:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *  ```
 *
 *  Example 1 — run a command and capture its output with exec:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *
 *  // synchronous form: returns { stdout, stderr, exitCode }
 *  const command = `"${process.execPath}" -e "console.log('exec')"`;
 *  const ret = child_process.exec(command);
 *  console.log(ret.stdout.trim(), ret.exitCode); // exec 0
 *
 *  // callback form: returns a ChildProcess
 *  const child = child_process.exec(command, (err, stdout, stderr, exitCode) => {
 *      console.log(stdout.trim(), exitCode); // exec 0
 *  });
 *  child.join();
 *  ```
 *
 *  Example 2 — stream data through a spawned process:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *
 *  const child = child_process.spawn(process.execPath, ['-e', 'process.stdin.pipe(process.stdout)']);
 *  child.stdin.write('ping\n');
 *  child.stdin.close();
 *
 *  console.log(child.stdout.readAll().toString().trim()); // ping
 *  console.log(child.join()); // 0
 *  ```
 *
 *  Example 3 — handle exit codes and spawn errors:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *
 *  // a non-zero exit is reported through exitCode, not thrown
 *  const ret = child_process.execFile(process.execPath, ['-e', 'process.exit(3)']);
 *  console.log(ret.exitCode); // 3
 *
 *  // a spawn failure throws a Node-compatible error
 *  try {
 *      child_process.spawn('definitely-not-a-command');
 *  } catch (err) {
 *      console.log(err.code, err.syscall); // ENOENT spawn definitely-not-a-command
 *  }
 *
 *  // the sync helpers throw on a non-zero exit
 *  try {
 *      child_process.execSync(`"${process.execPath}" -e "process.exit(4)"`);
 *  } catch (err) {
 *      console.log(err.status); // 4
 *  }
 *  ```
 *
 *  Example 4 — run a module file with fork:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-fork-'));
 *  fs.writeFile(path.join(dir, 'child.js'), 'console.log("child", process.argv[2]);');
 *
 *  const child = child_process.fork(path.join(dir, 'child.js'), ['arg1'], { silent: true });
 *  console.log(child.stdout.readAll().toString().trim()); // child arg1
 *  console.log(child.join()); // 0
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Notes:
 *
 *  - The callback forms run on a separate fiber; the synchronous forms block the calling fiber.
 *  - `sh` removes the trailing newline of the command output.
 *  - On Windows an executable without a path is resolved by the operating system; prefer exec for
 *    shell built-ins and commands that need quoting.
 *
 */
declare module 'child_process' {
    /**
     * @description Spawns a child process with the given command and argument list
     *
     *      Starts the program without a shell and returns immediately with a ChildProcess; stdio is
     *      configured by options.stdio (see the module concepts). The command and every element of args
     *      are converted to their string form. A failure to start throws a Node-compatible Error with
     *      code, errno, syscall, path and args fields.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // working directory of the child process
     *          "stdio": "pipe", // pipe/ignore/inherit/pty or an array with one entry per descriptor
     *          "env": {}, // environment variables added to the child's environment
     *          "detached": false, // child process will be a leader of a new process group
     *          "uid": 0, // POSIX user identity of the child
     *          "gid": 0, // POSIX group identity of the child
     *          "windowsVerbatimArguments": false, // Windows: pass the command line without quoting
     *          "windowsHide": false, // Windows: hide the console window
     *          "timeout": 0, // kill the child with killSignal after this many milliseconds
     *          "killSignal": "SIGTERM", // signal used by timeout and signal abort
     *          "signal": null, // AbortSignal that kills the child when aborted
     *          "cols": 80, // pty only: initial number of terminal columns
     *          "rows": 24 // pty only: initial number of terminal rows
     *      })
     *      ```
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *
     *      const child = child_process.spawn(process.execPath, ['-e', 'console.log("hi")']);
     *      console.log(child.stdout.readAll().toString().trim()); // hi
     *      console.log(child.join()); // 0
     *      ```
     *
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the child process object
     *
     */
    function spawn(command: string, args: any[], options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description Spawns a child process with the given command and no arguments
     *
     *      Equivalent to spawn(command, [], options): the program is started without a shell and the
     *      call returns a ChildProcess immediately. All creation options of the argv overload apply,
     *      including stdio configuration, cwd, env, timeout and killSignal.
     *
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the child process object
     *
     */
    function spawn(command: string, options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description Executes a command in the platform shell and buffers the output
     *
     *      The command string is interpreted by the shell (`/bin/sh -c` on POSIX, `cmd.exe /d /s /c` on
     *      Windows), so pipes, redirection and quoting are available. Without a callback the function
     *      blocks the current fiber and returns an object with stdout, stderr and exitCode; stdout and
     *      stderr are strings decoded with options.encoding (utf8 by default) and null when empty. A
     *      non-zero exit is not an error. With a callback the function returns a ChildProcess and the
     *      callback receives (err, stdout, stderr, exitCode); a failure to start throws.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // working directory of the child process
     *          "env": {}, // environment variables added to the child's environment
     *          "encoding": "utf8", // decode stdout/stderr with this encoding; 'buffer' keeps Buffers
     *          "input": null, // string or Buffer written to the child's stdin
     *          "detached": false, // child process will be a leader of a new process group
     *          "uid": 0, // POSIX user identity of the child
     *          "gid": 0, // POSIX group identity of the child
     *          "windowsVerbatimArguments": false, // Windows: pass the command line without quoting
     *          "windowsHide": false, // Windows: hide the console window
     *          "timeout": 0, // kill the child with killSignal after this many milliseconds
     *          "killSignal": "SIGTERM", // signal used by timeout and signal abort
     *          "signal": null // AbortSignal that kills the child when aborted
     *      })
     *      ```
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *
     *      const command = `"${process.execPath}" -e "process.stdin.pipe(process.stdout)"`;
     *      const ret = child_process.exec(command, { input: 'hi\n' });
     *      console.log(JSON.stringify(ret.stdout)); // "hi\n"
     *      ```
     *
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the stdio output of the child process
     *
     */
    function exec(command: string, options?: FIBJS.GeneralObject): {
        stdout: any;
        stderr: any;
        exitCode: number;
    };

    function exec(command: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: {
        stdout: any;
        stderr: any;
        exitCode: number;
    })=>any): void;

    /**
     * @description Executes a command in the platform shell and buffers the output
     *
     *      The command string is interpreted by the shell (`/bin/sh -c` on POSIX, `cmd.exe /d /s /c` on
     *      Windows), so pipes, redirection and quoting are available. Without a callback the function
     *      blocks the current fiber and returns an object with stdout, stderr and exitCode; stdout and
     *      stderr are strings decoded with options.encoding (utf8 by default) and null when empty. A
     *      non-zero exit is not an error. With a callback the function returns a ChildProcess and the
     *      callback receives (err, stdout, stderr, exitCode); a failure to start throws.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // working directory of the child process
     *          "env": {}, // environment variables added to the child's environment
     *          "encoding": "utf8", // decode stdout/stderr with this encoding; 'buffer' keeps Buffers
     *          "input": null, // string or Buffer written to the child's stdin
     *          "detached": false, // child process will be a leader of a new process group
     *          "uid": 0, // POSIX user identity of the child
     *          "gid": 0, // POSIX group identity of the child
     *          "windowsVerbatimArguments": false, // Windows: pass the command line without quoting
     *          "windowsHide": false, // Windows: hide the console window
     *          "timeout": 0, // kill the child with killSignal after this many milliseconds
     *          "killSignal": "SIGTERM", // signal used by timeout and signal abort
     *          "signal": null // AbortSignal that kills the child when aborted
     *      })
     *      ```
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *
     *      const command = `"${process.execPath}" -e "process.stdin.pipe(process.stdout)"`;
     *      const ret = child_process.exec(command, { input: 'hi\n' });
     *      console.log(JSON.stringify(ret.stdout)); // "hi\n"
     *      ```
     *
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
     * @description Directly executes a file with the given arguments and buffers the output
     *
     *      The program is started without a shell, so args is passed to it verbatim. The call forms are
     *      the same as exec: without a callback the function blocks and returns stdout, stderr and
     *      exitCode (decoded with options.encoding, utf8 by default, null when empty); with a callback
     *      it returns a ChildProcess and the callback receives (err, stdout, stderr, exitCode). A
     *      failure to start throws a Node-compatible Error.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // working directory of the child process
     *          "env": {}, // environment variables added to the child's environment
     *          "encoding": "utf8", // decode stdout/stderr with this encoding; 'buffer' keeps Buffers
     *          "input": null, // string or Buffer written to the child's stdin
     *          "detached": false, // child process will be a leader of a new process group
     *          "uid": 0, // POSIX user identity of the child
     *          "gid": 0, // POSIX group identity of the child
     *          "windowsVerbatimArguments": false, // Windows: pass the command line without quoting
     *          "windowsHide": false, // Windows: hide the console window
     *          "timeout": 0, // kill the child with killSignal after this many milliseconds
     *          "killSignal": "SIGTERM", // signal used by timeout and signal abort
     *          "signal": null // AbortSignal that kills the child when aborted
     *      })
     *      ```
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *
     *      const ret = child_process.execFile(process.execPath, ['-e', 'console.log("ef")']);
     *      console.log(ret.stdout.trim()); // ef
     *      ```
     *
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the stdio output of the child process
     *
     */
    function execFile(command: string, args: any[], options?: FIBJS.GeneralObject): {
        stdout: any;
        stderr: any;
        exitCode: number;
    };

    function execFile(command: string, args: any[], options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: {
        stdout: any;
        stderr: any;
        exitCode: number;
    })=>any): void;

    /**
     * @description Directly executes a file with the given arguments and buffers the output
     *
     *      The program is started without a shell, so args is passed to it verbatim. The call forms are
     *      the same as exec: without a callback the function blocks and returns stdout, stderr and
     *      exitCode (decoded with options.encoding, utf8 by default, null when empty); with a callback
     *      it returns a ChildProcess and the callback receives (err, stdout, stderr, exitCode). A
     *      failure to start throws a Node-compatible Error.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // working directory of the child process
     *          "env": {}, // environment variables added to the child's environment
     *          "encoding": "utf8", // decode stdout/stderr with this encoding; 'buffer' keeps Buffers
     *          "input": null, // string or Buffer written to the child's stdin
     *          "detached": false, // child process will be a leader of a new process group
     *          "uid": 0, // POSIX user identity of the child
     *          "gid": 0, // POSIX group identity of the child
     *          "windowsVerbatimArguments": false, // Windows: pass the command line without quoting
     *          "windowsHide": false, // Windows: hide the console window
     *          "timeout": 0, // kill the child with killSignal after this many milliseconds
     *          "killSignal": "SIGTERM", // signal used by timeout and signal abort
     *          "signal": null // AbortSignal that kills the child when aborted
     *      })
     *      ```
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *
     *      const ret = child_process.execFile(process.execPath, ['-e', 'console.log("ef")']);
     *      console.log(ret.stdout.trim()); // ef
     *      ```
     *
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
     * @description Directly executes a file with no arguments and buffers the output
     *
     *      Equivalent to execFile(command, [], options); see the argv overload for the call forms, the
     *      returned stdout/stderr/exitCode object and the Node-compatible error thrown on a spawn
     *      failure. The options are those of execFile, including encoding and input.
     *
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the stdio output of the child process
     *
     */
    function execFile(command: string, options?: FIBJS.GeneralObject): {
        stdout: any;
        stderr: any;
        exitCode: number;
    };

    function execFile(command: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: {
        stdout: any;
        stderr: any;
        exitCode: number;
    })=>any): void;

    /**
     * @description Directly executes a file with no arguments and buffers the output
     *
     *      Equivalent to execFile(command, [], options); see the argv overload for the call forms, the
     *      returned stdout/stderr/exitCode object and the Node-compatible error thrown on a spawn
     *      failure. The options are those of execFile, including encoding and input.
     *
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
     * @description Synchronously spawns a child process and returns its complete result
     *
     *      Blocks the current fiber until the child exits while buffering stdout and stderr in memory.
     *      It never throws for a non-zero exit or a failed spawn: the returned object contains pid,
     *      output (an array of three entries: [null, stdout, stderr]), stdout, stderr, status, signal
     *      and error. stdout/stderr are Buffers unless options.encoding decodes them and are null for
     *      descriptors that are not piped; signal is null on a normal exit and carries the signal name
     *      when the child was killed by a signal. When the process cannot be started, pid is 0, status
     *      is 0, the streams are null and error holds a Node-compatible error object with
     *      code/errno/syscall/path.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // working directory of the child process
     *          "stdio": "pipe", // pipe/ignore/inherit/pty or an array with one entry per descriptor
     *          "input": null, // string or Buffer written to the child's stdin
     *          "env": {}, // environment variables added to the child's environment
     *          "encoding": "buffer", // decode stdout/stderr with this encoding; 'buffer' keeps Buffers
     *          "detached": false, // child process will be a leader of a new process group
     *          "uid": 0, // POSIX user identity of the child
     *          "gid": 0, // POSIX group identity of the child
     *          "windowsVerbatimArguments": false, // Windows: pass the command line without quoting
     *          "windowsHide": false, // Windows: hide the console window
     *          "timeout": 0, // kill the child with killSignal after this many milliseconds
     *          "killSignal": "SIGTERM", // signal used by timeout and signal abort
     *          "signal": null, // AbortSignal that kills the child when aborted
     *          "cols": 80, // pty only: initial number of terminal columns
     *          "rows": 24 // pty only: initial number of terminal rows
     *      })
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
     * @description Synchronously spawns a child process with the given command and no arguments
     *
     *      Equivalent to spawnSync(command, [], options); see the argv overload for the returned
     *      result object, the buffered streams and the error field. All creation options apply, in
     *      particular stdio, input and encoding.
     *
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
     * @description Synchronously executes a command in a shell and returns its stdout
     *
     *      The command string is interpreted by the platform shell like exec. stdout is returned as a
     *      Buffer by default, or as a string when options.encoding is given; an empty output is null.
     *      When the exit code is not 0 the function throws an Error with Node-compatible status,
     *      stdout, stderr and output fields instead of returning. The options are those of exec,
     *      including input and encoding.
     *
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the stdout output of the child process, a Buffer by default; returns a string when options.encoding specifies an encoding
     *
     */
    function execSync(command: string, options?: FIBJS.GeneralObject): any;

    /**
     * @description Directly synchronously executes a file and returns its stdout
     *
     *      The program is started without a shell and args is passed verbatim, like execFile. The
     *      return value and the thrown error match execSync: stdout as a Buffer by default or a string
     *      with options.encoding, and an Error with status/stdout/stderr/output fields on a non-zero
     *      exit. The options are those of execFile, including input and encoding.
     *
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the stdout output of the child process, a Buffer by default; returns a string when options.encoding specifies an encoding
     *
     */
    function execFileSync(command: string, args: any[], options?: FIBJS.GeneralObject): any;

    /**
     * @description Directly synchronously executes a file with no arguments and returns its stdout
     *
     *      Equivalent to execFileSync(command, [], options); see the argv overload for the return
     *      value and the error behavior on a non-zero exit. The options are those of execFile,
     *      including input and encoding.
     *
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the stdout output of the child process, a Buffer by default; returns a string when options.encoding specifies an encoding
     *
     */
    function execFileSync(command: string, options?: FIBJS.GeneralObject): any;

    /**
     * @description Executes a module in a child process
     *
     *      The child is started with process.execPath, the module path as its first argument and the
     *      elements of args after it, so the module is resolved by the child against options.cwd (pass
     *      an absolute path unless cwd is set). fork always creates an IPC message channel; by default
     *      the child inherits the parent's stdio, while `silent: true` pipes stdin, stdout and stderr so
     *      they can be read through the ChildProcess streams. The options are those of spawn plus
     *      silent; ChildProcess send, message, connected and disconnect operate the channel.
     *
     *      options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "cwd": "", // working directory of the child process
     *          "stdio": "inherit", // overrides silent when provided
     *          "silent": false, // true pipes stdin/stdout/stderr instead of inheriting them
     *          "env": {}, // environment variables added to the child's environment
     *          "detached": false, // child process will be a leader of a new process group
     *          "uid": 0, // POSIX user identity of the child
     *          "gid": 0, // POSIX group identity of the child
     *          "windowsVerbatimArguments": false, // Windows: pass the command line without quoting
     *          "windowsHide": false, // Windows: hide the console window
     *          "cols": 80, // pty only: initial number of terminal columns
     *          "rows": 24 // pty only: initial number of terminal rows
     *      })
     *      ```
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-fork-'));
     *      fs.writeFile(path.join(dir, 'child.js'), 'console.log("child", process.argv[2]);');
     *
     *      const child = child_process.fork(path.join(dir, 'child.js'), ['arg1'], { silent: true });
     *      console.log(child.stdout.readAll().toString().trim()); // child arg1
     *      console.log(child.join()); // 0
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      @param module specifies the module to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the child process object
     *
     */
    function fork(module: string, args: any[], options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description Executes a module in a child process with no arguments
     *
     *      Equivalent to fork(module, [], options); see the argv overload for the module resolution,
     *      the IPC channel and the silent/stdio behavior. The module path is resolved by the child
     *      against options.cwd, so pass an absolute path unless cwd points at the module directory.
     *
     *      @param module specifies the module to run
     *      @param options specifies the creation options
     *      @return returns the child process object
     *
     */
    function fork(module: string, options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description Directly executes a file and returns its exit code
     *
     *      fibjs extension. The child inherits the parent's stdio (the stdio option is forced to
     *      'inherit'), so its output is written to the current terminal instead of being returned; use
     *      exec or execFile to capture output. Without a callback the function blocks the current fiber
     *      and returns the exit code (0-255, or the negative signal number for a signal death); with a
     *      callback it returns a ChildProcess and the callback receives (err, exitCode). The options
     *      are those of execFile.
     *
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function run(command: string, args: any[], options?: FIBJS.GeneralObject): number;

    function run(command: string, args: any[], options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Directly executes a file and returns its exit code
     *
     *      fibjs extension. The child inherits the parent's stdio (the stdio option is forced to
     *      'inherit'), so its output is written to the current terminal instead of being returned; use
     *      exec or execFile to capture output. Without a callback the function blocks the current fiber
     *      and returns the exit code (0-255, or the negative signal number for a signal death); with a
     *      callback it returns a ChildProcess and the callback receives (err, exitCode). The options
     *      are those of execFile.
     *
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function runSync(command: string, args: any[], options?: FIBJS.GeneralObject): number;

    /**
     * @description Directly executes a file and returns its exit code
     *
     *      fibjs extension. The child inherits the parent's stdio (the stdio option is forced to
     *      'inherit'), so its output is written to the current terminal instead of being returned; use
     *      exec or execFile to capture output. Without a callback the function blocks the current fiber
     *      and returns the exit code (0-255, or the negative signal number for a signal death); with a
     *      callback it returns a ChildProcess and the callback receives (err, exitCode). The options
     *      are those of execFile.
     *
     *      @param command specifies the command to run
     *      @param args specifies the list of string arguments
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function runAsync(command: string, args: any[], options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description Directly executes a file with no arguments and returns its exit code
     *
     *      Equivalent to run(command, [], options); see the argv overload for the callback form, the
     *      forced inherit stdio and the return value. Use it when only the exit status matters and the
     *      output should go to the terminal.
     *
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function run(command: string, options?: FIBJS.GeneralObject): number;

    function run(command: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Directly executes a file with no arguments and returns its exit code
     *
     *      Equivalent to run(command, [], options); see the argv overload for the callback form, the
     *      forced inherit stdio and the return value. Use it when only the exit status matters and the
     *      output should go to the terminal.
     *
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function runSync(command: string, options?: FIBJS.GeneralObject): number;

    /**
     * @description Directly executes a file with no arguments and returns its exit code
     *
     *      Equivalent to run(command, [], options); see the argv overload for the callback form, the
     *      forced inherit stdio and the return value. Use it when only the exit status matters and the
     *      output should go to the terminal.
     *
     *      @param command specifies the command to run
     *      @param options specifies the creation options
     *      @return returns the exitCode of the child process
     *
     */
    function runAsync(command: string, options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description Executes a shell command as a template tag and returns its stdout
     *
     *      `sh` is a template tag: the interpolated values are inserted into the command string (arrays
     *      are joined with spaces, other values use their string form) and the command is executed
     *      through the shell like exec. The returned stdout is decoded as utf8 and the trailing newline
     *      is removed, so the result can be embedded in the next command. A failing command throws an
     *      Error whose message is the shell error. fibjs extension for short synchronous commands.
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const $ = child_process.sh;
     *
     *      const word = 'world';
     *      console.log($`echo hello ${word}`); // hello world
     *      ```
     *
     *       @param strings specifies the command to run
     *       @param args specifies the list of string arguments
     *       @return returns the stdio output of the child process
     *
     */
    function sh(strings: any[], ...args: any[]): string;

    /**
     * @description Executes a shell command as a template tag and returns its stdout
     *
     *      `sh` is a template tag: the interpolated values are inserted into the command string (arrays
     *      are joined with spaces, other values use their string form) and the command is executed
     *      through the shell like exec. The returned stdout is decoded as utf8 and the trailing newline
     *      is removed, so the result can be embedded in the next command. A failing command throws an
     *      Error whose message is the shell error. fibjs extension for short synchronous commands.
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const $ = child_process.sh;
     *
     *      const word = 'world';
     *      console.log($`echo hello ${word}`); // hello world
     *      ```
     *
     *       @param strings specifies the command to run
     *       @param args specifies the list of string arguments
     *       @return returns the stdio output of the child process
     *
     */
    function shSync(strings: any[], ...args: any[]): string;

    /**
     * @description Executes a shell command as a template tag and returns its stdout
     *
     *      `sh` is a template tag: the interpolated values are inserted into the command string (arrays
     *      are joined with spaces, other values use their string form) and the command is executed
     *      through the shell like exec. The returned stdout is decoded as utf8 and the trailing newline
     *      is removed, so the result can be embedded in the next command. A failing command throws an
     *      Error whose message is the shell error. fibjs extension for short synchronous commands.
     *
     *      Example:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const $ = child_process.sh;
     *
     *      const word = 'world';
     *      console.log($`echo hello ${word}`); // hello world
     *      ```
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
     *      Returns a template function that runs a command on a remote host through the ssh executable
     *      using execFile, with the same template syntax as sh, for example `ssh('user@host')` followed
     *      by a tagged template with `ls -l`. Arrays are joined with spaces and the trailing newline is
     *      removed, like sh. An ssh binary and a reachable host are required; fibjs extension.
     *
     *      options supports:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "user": "", // ssh user
     *          "port": 22 // ssh port
     *      })
     *      ```
     *
     *      Example:
     *      ```JavaScript
     *      // fragment: requires a reachable ssh host
     *      const child_process = require('child_process');
     *      const $ = child_process.ssh('user@host');
     *
     *      console.log($`uname -a`);
     *      ```
     *
     *     @param host specifies the remote host address
     *     @param options specifies the ssh connection parameters
     *     @return returns a template function that executes the command and returns its stdout
     *
     */
    function ssh(host: string, options?: FIBJS.GeneralObject): (...args: any[])=>any;

}

