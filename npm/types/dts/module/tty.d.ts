/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TTYInputStream.d.ts" />
/// <reference path="../interface/TTYOutputStream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/**
 * @description tty 模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  const tty = require('tty');
 *  ```
 *  
 */
declare module 'tty' {
    /**
     * @description TTY 输入流对象，参见 TTYInputStream 
     */
    const ReadStream: typeof Class_TTYInputStream;

    /**
     * @description TTY 输出流对象，参见 TTYOutputStream 
     */
    const WriteStream: typeof Class_TTYOutputStream;

    /**
     * @description 查询是否是命令交互窗口
     *     @param fd 文件描述符
     *      @return 如果文件描述符同一个终端窗口关联则返回 true ，否则返回 false
     *     
     */
    function isatty(fd: number): boolean;

    /**
     * @description 查询是否是命令交互窗口
     *     @param fd 文件句柄对象
     *      @return 如果文件句柄同一个终端窗口关联则返回 true ，否则返回 false
     *     
     */
    function isatty(fd: Class_FileHandle): boolean;

}

