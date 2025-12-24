/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/**
 * @description tty 写流对象, 用于处理 tty 输出
 * 
 *  没有途径可以单独创建该类, 全局只有 `process.stdout` 实例
 * 
 *  ```JavaScript
 *  // clear line
 *  process.stdout.clearLine(1)
 *  ```
 *  
 */
declare class Class_TTYOutputStream extends Class_Stream {
    /**
     * @description 创建一个新的 TTYOutputStream 对象, 参数 fd 指定底层的文件描述符
     *      @param fd 指定底层的文件描述符, 该描述符必须是一个 tty 设备
     *      @param opts 传递给 Stream 构造函数的选项对象
     *      
     */
    constructor(fd: number, opts?: FIBJS.GeneralObject);

    /**
     * @description 创建一个新的 TTYOutputStream 对象, 参数 fd 指定底层的文件对象
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
     * @description 恒为 true，表示流可写
     *     
     */
    readonly writable: boolean;

    /**
     * @description 返回此 TTYOutputStream 对应的终端的列数
     *     
     */
    readonly columns: number;

    /**
     * @description 返回此 TTYOutputStream 对应的终端的行数
     *     
     */
    readonly rows: number;

    /**
     * @description 返回此 TTYOutputStream 对应的终端的可写状态对象
     *      
     */
    readonly _writableState: FIBJS.GeneralObject;

    /**
     * @description 根据 dir 指示的方向 clear line
     * 
     *      dir 的方向:
     *      - -1: 从光标起往行起始清理
     *      - 0: 清理整行
     *      - 1: 从光标起往行结尾清理
     * 
     *      @param dir 清理方向
     *      
     */
    clearLine(dir?: number): void;

    /**
     * @description 清除从光标处起到屏幕结尾的字符
     *      
     */
    clearScreenDown(): void;

    /**
     * @description 将光标移动到指定位置
     *      @param x 指定的列数
     *      @param y 指定的行数，缺省为 -1，表示不改变行数
     *      
     */
    cursorTo(x: number, y?: number): void;

    cursorTo(x: number, y?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 将光标移动到指定位置
     *      @param x 指定的列数
     *      @param y 指定的行数，缺省为 -1，表示不改变行数
     *      
     */
    cursorToSync(x: number, y?: number): void;

    /**
     * @description 将光标移动到指定位置
     *      @param x 指定的列数
     *      @param y 指定的行数，缺省为 -1，表示不改变行数
     *      
     */
    cursorToAsync(x: number, y?: number): Promise<void>;

    /**
     * @description 将光标移动指定的偏移量
     *      @param dx 指定的列偏移量
     *      @param dy 指定的行偏移量
     *      
     */
    moveCursor(dx: number, dy: number): void;

    moveCursor(dx: number, dy: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 将光标移动指定的偏移量
     *      @param dx 指定的列偏移量
     *      @param dy 指定的行偏移量
     *      
     */
    moveCursorSync(dx: number, dy: number): void;

    /**
     * @description 将光标移动指定的偏移量
     *      @param dx 指定的列偏移量
     *      @param dy 指定的行偏移量
     *      
     */
    moveCursorAsync(dx: number, dy: number): Promise<void>;

    /**
     * @description 返回此 TTYOutputStream 对应的终端的尺寸
     *      @return 返回数组 [numColumns, numRows]，其中 numColumns 和 numRows 表示相应终端中的列数和行数
     *      
     */
    getWindowSize(): any[];

    /**
     * @description 当终端尺寸改变时触发 
     */
    on(event: "resize", listener: ()=>void): this;

}

