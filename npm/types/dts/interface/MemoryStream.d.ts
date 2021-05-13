/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description 内存流对象
 * 
 *  MemoryStream 对象创建一个基于内存的流对象，创建方法：
 *  ```JavaScript
 *  var ms = new io.MemoryStream();
 *  ```
 *  
 */
declare class Class_MemoryStream extends Class_SeekableStream {
    /**
     * @description MemoryStream 构造函数 
     */
    constructor();

    /**
     * @description 强制设定内存流对象的最后更新时间
     *      @param d 指定要设定的时间
     *      
     */
    setTime(d: typeof Date): void;

    /**
     * @description 创建当前内存流的一个只读副本
     *      @return 返回只读的内存流对象
     *      
     */
    clone(): Class_MemoryStream;

    /**
     * @description 清空内存文件数据，复位指针 
     */
    clear(): void;

}

