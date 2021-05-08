/// <reference path="../interface/MemoryStream.d.ts" />
/// <reference path="../interface/BufferedStream.d.ts" />
/// <reference path="../interface/RangeStream.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description 输入输出处理模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  var io = require('io');
 *  ```
 *  
 */
declare module 'io' {
    /**
     * @description 创建一个内存流对象，参见 MemoryStream 
     */
    const MemoryStream: typeof Class_MemoryStream;

    /**
     * @description 创建一个缓存流读取对象，参见 BufferedStream 
     */
    const BufferedStream: typeof Class_BufferedStream;

    /**
     * @description 创建一个 Range 查询流读取对象，参见 RangeStream 
     */
    const RangeStream: typeof Class_RangeStream;

    /**
     * @description 复制流数据到目标流中
     *      @param from 源流对象
     *      @param to 目标流对象
     *      @param bytes 复制的字节数
     *      @return 返回复制的字节数
     *      
     */
    function copyStream(from: Class_Stream, to: Class_Stream, bytes: number): number;

    function copyStream(from: Class_Stream, to: Class_Stream, bytes: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 双向复制流数据，直到流中无数据，或者流被关闭
     *      @param stm1 流对象一
     *      @param stm2 流对象二
     *      
     */
    function bridge(stm1: Class_Stream, stm2: Class_Stream): void;

    function bridge(stm1: Class_Stream, stm2: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

}

