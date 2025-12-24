/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/**
 * @description tty 读流对象, 用于对 tty 进行读取, 写入
 * 
 *  没有途径可以单独创建该类, 全局只有 `process.stdin` 实例
 * 
 *  ```JavaScript
 *  process.stdin.read(1)
 *  ```
 *  
 */
declare class Class_TTYInputStream extends Class_Stream {
    /**
     * @description 创建一个新的 TTYInputStream 对象, 参数 fd 指定底层的文件描述符
     *      @param fd 指定底层的文件描述符, 该描述符必须是一个 tty 设备
     *      @param opts 传递给 Stream 构造函数的选项对象
     *      
     */
    constructor(fd: number, opts?: FIBJS.GeneralObject);

    /**
     * @description 创建一个新的 TTYInputStream 对象, 参数 fd 指定底层的文件对象
     *      @param fd 指定底层的文件对象, 该文件对象必须是一个 tty 设备
     *      @param opts 传递给 Stream 构造函数的选项对象
     *      
     */
    constructor(fd: Class_FileHandle, opts?: FIBJS.GeneralObject);

    /**
     * @description 恒为 true
     *     
     */
    readonly isTTY: boolean;

    /**
     * @description 恒为 true，表示流可读
     *     
     */
    readonly readable: boolean;

    /**
     * @description 查询是否是处于 raw mode, 为 true 时, 表示 tty 被配置为以 raw device 操作时
     *     
     */
    readonly isRaw: boolean;

    /**
     * @description 设置 tty 是否以 raw mode 工作
     *      @param isRawMode 为 true, 表示以 raw mode 工作; 否则以默认模式工作. `readStream.isRaw` 会受该方法设置
     *      @return 返回自身
     *      
     */
    setRawMode(isRawMode: boolean): Class_TTYInputStream;

}

