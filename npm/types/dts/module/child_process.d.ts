/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ChildProcess.d.ts" />
/**
 * @description 子进程管理模块
 *  引用方式：
 *  ```JavaScript
 *  var child_process = require("child_process");
 *  var child = child_process.spawn("ls");
 *  ```
 * 
 * 在创建子进程时，options.stdio 选项用于配置在父进程和子进程之间建立的管道。 默认情况下，子进程的 stdin、 stdout 和 stderr 会被重定向到 ChildProcess 对象上相应的 stdin、stdout 和 stderr 流。 这相当于将 options.stdio 设置为 ['pipe', 'pipe', 'pipe']。
 * 
 * 为方便起见， options.stdio 可以是以下字符串之一：
 * 
 * - 'pipe'：相当于 ['pipe', 'pipe', 'pipe']（默认值）。
 * - 'ignore'：相当于 ['ignore', 'ignore', 'ignore']。
 * - 'inherit'：相当于 ['inherit', 'inherit', 'inherit'] 或 [0, 1, 2]。
 * - 'pty'：相当于 ['pty', 'pty', 'pty']。不支持 Windows。
 * 
 * 否则， options.stdio 的值需是数组（其中每个索引对应于子进程中的文件描述符）。 文件描述符 0、1 和 2 分别对应于 stdin、stdout 和 stderr。 其他的文件描述符可以被指定用于在父进程和子进程之间创建其他的管道。 值可以是以下之一：
 * 
 * 1. 'pipe'：在子进程和父进程之间创建管道。 管道的父端作为 child_process 对象上的 stdio[fd] 属性暴露给父进程。 为文件描述符 0、1 和 2 创建的管道也可分别作为 stdin、stdout 和 stderr 使用。
 * 2. 'ignore'：指示 fibjs 忽略子进程中的文件描述符。 虽然 fibjs 将会始终为其衍生的进程打开文件描述符 0、1 和 2，但将文件描述符设置为 'ignore' 可以使 fibjs 打开 /dev/null 并将其附加到子进程的文件描述符。
 * 3. 'inherit'：将相应的 stdio 流传给父进程或从父进程传入。在前三个位置中，这分别相当于 process.stdin、 process.stdout 和 process.stderr。 在任何其他位置中，则相当于 'ignore'。
 * 4. 'pty'：在子进程将在虚拟终端中执行。此时只有 stdin 和 stdout 有效。
 * 5. 正整数：整数值会被解释为当前在父进程中打开的文件描述符。 它与子进程共享，类似于共享 <Stream> 对象的方式。 在 Windows 上不支持传入 socket。
 * 6. null 或 undefined：使用默认值。 对于 stdio 的文件描述符 0、1 和 2（换句话说，stdin、stdout 和 stderr），将会创建管道。 对于文件描述符 3 及更大的值，则默认为 'ignore'。
 * 
 * ```JavaScript
 * const { spawn } = require('child_process');
 * 
 * // 子进程使用父进程的 stdio。
 * spawn('prg', [], { stdio: 'inherit' });
 * 
 * // 衍生的子进程只共享 stderr。
 * spawn('prg', [], { stdio: ['pipe', 'pipe', process.stderr] });
 * ```
 * 
 *  对于同时使用 nodejs 的用户, 需注意
 *  - fibjs 的 `child_process.exec(command, args)` 和 nodejs 的同名 api 功能类似, 但在 windows 上, 并不会自动将 cmd.exe 作为 command 参数的执行环境;
 *  - fibjs 的 child_process.[spawn|exec|execFile|run] 是同步/回调一体的 async 风格函数:
 *    - 如果最后一个参数不是函数, 则是同步的
 *    - 如果传递了函数作为最后一个参数, 则是异步的;
 *  - fibjs 的 child_process.[exec|execFile] 的返回结果是一个对象, 该对象和 nodejs 同名 api 返回的对象**完全不相同**
 *  - fibjs 的 `child_process.run` 在 nodejs 中没有对应 API
 *  
 */
declare module 'child_process' {
    /**
     * @description 用给定的命令发布一个子进程
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "stdio": Array | String, // 子进程 stdio 配置
     *         "env": {}, // 环境变量的键值对
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param command 指定要运行的命令
     *      @param args 指定字符串参数列表
     *      @param options 指定创建参数
     *      @return 返回子进程对象
     *      
     */
    function spawn(command: string, args: any[], options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description 用给定的命令发布一个子进程
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "stdio": Array | String, // 子进程 stdio 配置
     *         "env": {}, // 环境变量的键值对
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param command 指定要运行的命令
     *      @param options 指定创建参数
     *      @return 返回子进程对象
     *      
     */
    function spawn(command: string, options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description 在 shell 中执行一个命令并缓冲输出，当以回调方式执行时，函数将返回子进程对象
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "env": {}, // 环境变量的键值对
     *         "encoding": "utf8", // 指定返回结果的编码，缺省为 utf8
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param command 指定要运行的命令
     *      @param options 指定创建参数
     *      @return 返回子进程的 stdio 输出内容
     *      
     */
    function exec(command: string, options?: FIBJS.GeneralObject): [stdout: any, stderr: any];

    function exec(command: string, options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: [stdout: any, stderr: any])=>any): void;

    /**
     * @description 直接执行所指定的文件并缓冲输出，当以回调方式执行时，函数将返回子进程对象
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "env": {}, // 环境变量的键值对
     *         "encoding": "utf8", // 指定返回结果的编码，缺省为 utf8
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param command 指定要运行的命令
     *      @param args 指定字符串参数列表
     *      @param options 指定创建参数
     *      @return 返回子进程的 stdio 输出内容
     *      
     */
    function execFile(command: string, args: any[], options?: FIBJS.GeneralObject): [stdout: any, stderr: any];

    function execFile(command: string, args: any[], options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: [stdout: any, stderr: any])=>any): void;

    /**
     * @description 直接执行所指定的文件并缓冲输出，当以回调方式执行时，函数将返回子进程对象
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "env": {}, // 环境变量的键值对
     *         "encoding": "utf8", // 指定返回结果的编码，缺省为 utf8
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param command 指定要运行的命令
     *      @param options 指定创建参数
     *      @return 返回子进程的 stdio 输出内容
     *      
     */
    function execFile(command: string, options?: FIBJS.GeneralObject): [stdout: any, stderr: any];

    function execFile(command: string, options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: [stdout: any, stderr: any])=>any): void;

    /**
     * @description 在子进程中执行一个模块
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "stdio": Array | String, // 子进程 stdio 配置
     *         "env": {}, // 环境变量的键值对
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param module 指定要运行的命令
     *      @param args 指定字符串参数列表
     *      @param options 指定创建参数
     *      @return 返回子进程对象
     *      
     */
    function fork(module: string, args: any[], options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description 在子进程中执行一个模块
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "stdio": Array | String, // 子进程 stdio 配置
     *         "env": {}, // 环境变量的键值对
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param module 指定要运行的命令
     *      @param options 指定创建参数
     *      @return 返回子进程对象
     *      
     */
    function fork(module: string, options?: FIBJS.GeneralObject): Class_ChildProcess;

    /**
     * @description 直接执行所指定的文件并返回 exitCode，当以回调方式执行时，函数将返回子进程对象
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "env": {}, // 环境变量的键值对
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param command 指定要运行的命令
     *      @param args 指定字符串参数列表
     *      @param options 指定创建参数
     *      @return 返回子进程的 exitCode
     *      
     */
    function run(command: string, args: any[], options?: FIBJS.GeneralObject): number;

    function run(command: string, args: any[], options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 直接执行所指定的文件并返回 exitCode，当以回调方式执行时，函数将返回子进程对象
     *     options 支持的内容如下：
     *      ```JavaScript
     *      {
     *         "cwd": "", // 子进程的当前的工作目录，缺省使用当前目录
     *         "env": {}, // 环境变量的键值对
     *         "detached": false, // 子进程将会变成一个进程组的领导者，缺省为 false
     *         "uid": 0, // 设置用户进程的ID
     *         "gid": 0, // 设置进程组的ID
     *         "windowsVerbatimArguments": false, // 在 Windows上不执行引号或转义参数。 在 Unix 上被忽略。 当指定外壳且为 CMD 时，此选项将自动设置为true，缺省为 false
     *         "windowsHide": false // 隐藏通常在Windows系统上创建的子进程控制台窗口，缺省为 false
     *      }
     *      ```
     *      @param command 指定要运行的命令
     *      @param options 指定创建参数
     *      @return 返回子进程的 exitCode
     *      
     */
    function run(command: string, options?: FIBJS.GeneralObject): number;

    function run(command: string, options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: number)=>any): void;

}

