/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description tty 写流对象, 用于处理 tty 输出
 * 
 *  没有途径可以单独创建该类, 全局只有 `process.stdout` 实例
 * 
 *  ```JavaScript
 *  // 清除屏幕上光标以右的内容
 *  process.stdout.clearLine(1)
 *  ```
 *  
 */
declare class Class_TTYOutputStream extends Class_Stream {
    /**
     * @description 恒为 true
     *     
     */
    readonly isTTY: boolean;

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

}

