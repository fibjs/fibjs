/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/MemoryStream.d.ts" />
/// <reference path="../interface/BufferedStream.d.ts" />
/// <reference path="../interface/RangeStream.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description 输入输出处理模块，提供流对象的创建与流间数据搬运能力
 * 
 *  模块的主要能力：
 * 
 *  - **流对象**：`MemoryStream` 内存流、`BufferedStream` 缓存流、`RangeStream` 范围流；
 *  - **数据搬运**：`copyStream` 将流数据复制到目标流，`bridge` 双向复制流数据。
 * 
 *  使用方法：
 *  ```JavaScript
 *  var io = require('io');
 *  ```
 * 
 *  复制流数据示例：
 * 
 *  ```JavaScript
 *  var io = require('io');
 * 
 *  var src = new io.MemoryStream();
 *  src.write(new Buffer('hello world'));
 *  src.rewind();
 * 
 *  var dst = new io.MemoryStream();
 *  io.copyStream(src, dst);
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
     * 
     *      bytes 指定复制的字节数，缺省为 -1，表示复制源流中的全部数据；复制完成后返回实际复制的字节数。
     *      @param from 源流对象
     *      @param to 目标流对象
     *      @param bytes 复制的字节数
     *      @return 返回复制的字节数
     *      
     */
    function copyStream(from: Class_Stream, to: Class_Stream, bytes?: number): number;

    function copyStream(from: Class_Stream, to: Class_Stream, bytes?: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 复制流数据到目标流中
     * 
     *      bytes 指定复制的字节数，缺省为 -1，表示复制源流中的全部数据；复制完成后返回实际复制的字节数。
     *      @param from 源流对象
     *      @param to 目标流对象
     *      @param bytes 复制的字节数
     *      @return 返回复制的字节数
     *      
     */
    function copyStreamSync(from: Class_Stream, to: Class_Stream, bytes?: number): number;

    /**
     * @description 复制流数据到目标流中
     * 
     *      bytes 指定复制的字节数，缺省为 -1，表示复制源流中的全部数据；复制完成后返回实际复制的字节数。
     *      @param from 源流对象
     *      @param to 目标流对象
     *      @param bytes 复制的字节数
     *      @return 返回复制的字节数
     *      
     */
    function copyStreamAsync(from: Class_Stream, to: Class_Stream, bytes?: number): Promise<number>;

    /**
     * @description 双向复制流数据，直到流中无数据，或者流被关闭
     * 
     *      stm1 与 stm2 互为对方的输入与输出，任一方向的数据传输结束后整体停止。
     *      @param stm1 流对象一
     *      @param stm2 流对象二
     *      
     */
    function bridge(stm1: Class_Stream, stm2: Class_Stream): void;

    function bridge(stm1: Class_Stream, stm2: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 双向复制流数据，直到流中无数据，或者流被关闭
     * 
     *      stm1 与 stm2 互为对方的输入与输出，任一方向的数据传输结束后整体停止。
     *      @param stm1 流对象一
     *      @param stm2 流对象二
     *      
     */
    function bridgeSync(stm1: Class_Stream, stm2: Class_Stream): void;

    /**
     * @description 双向复制流数据，直到流中无数据，或者流被关闭
     * 
     *      stm1 与 stm2 互为对方的输入与输出，任一方向的数据传输结束后整体停止。
     *      @param stm1 流对象一
     *      @param stm2 流对象二
     *      
     */
    function bridgeAsync(stm1: Class_Stream, stm2: Class_Stream): Promise<void>;

}

