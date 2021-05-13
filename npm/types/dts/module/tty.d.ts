/// <reference path="../_import/_fibjs.d.ts" />
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
     * @description 查询是否是命令交互窗口
     *     @param fd 文件描述符
     *      @return 如果文件描述符同一个终端窗口关联则返回 true ，否则返回 false
     *     
     */
    function isatty(fd: number): boolean;

}

