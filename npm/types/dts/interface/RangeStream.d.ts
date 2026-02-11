/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description Range 查询流读取对象
 * 
 *  RangeStream 对象用于对 SeekableStream 对象数据进行截取。创建方法：
 *  ```JavaScript
 *  var stm = new io.RangeStream(stream, '0-10');
 *  stm.end // 11
 * 
 *  var stm = new io.RangeStream(stream, 0, 10);
 *  stm.end // 10
 *  ```
 * 
 *  也可以对普通 Stream 对象进行长度限制读取，此时 begin 固定为 0，仅限制读取的字节数：
 *  ```JavaScript
 *  var stm = new io.RangeStream(stream, 1024);
 *  stm.begin // 0
 *  stm.end   // 1024
 *  ```
 *  如果传入的 stm 是 SeekableStream，则等同于 RangeStream(stm, 0, end)。
 *  
 */
declare class Class_RangeStream extends Class_SeekableStream {
    /**
     * @description RangeStream 构造函数
     *       @param stm RangeStream 的二进制基础流对象, 必须是 SeekableStream
     *       @param range 描述 range 的字符串, 格式为 'begin-[end]', '[begin]-end'
     *      
     */
    constructor(stm: Class_SeekableStream, range: string);

    /**
     * @description RangeStream 构造函数
     *       @param stm RangeStream 的二进制基础流对象, 必须是 SeekableStream
     *       @param begin 从 stm 读取内容的起始位置
     *       @param end 从 stm 读取内容的结束位置
     *      
     */
    constructor(stm: Class_SeekableStream, begin: number, end: number);

    /**
     * @description RangeStream 构造函数, 用于对普通 Stream 进行长度限制读取
     *       @param stm 基础流对象, 如果是 SeekableStream 则等同于 RangeStream(stm, 0, end)
     *       @param end 从 stm 读取内容的最大字节数
     *      
     */
    constructor(stm: Class_Stream, end: number);

    /**
     * @description 查询 range 开始值 
     */
    readonly begin: number;

    /**
     * @description 查询 range 结束值 
     */
    readonly end: number;

}

