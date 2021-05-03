/// <reference path="../interface/SeekableStream.d.ts" />
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
     * @description 查询 range 开始值 
     */
    readonly begin: number;

    /**
     * @description 查询 range 结束值 
     */
    readonly end: number;

}

