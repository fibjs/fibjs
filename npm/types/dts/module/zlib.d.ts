/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description zlib 是内置的压缩模块，支持 gzip、deflate、zlib 等多种压缩格式和模式
 * 
 *  zlib 主要由以下 3 个函数组成：
 * 
 * - deflate：压缩数据；
 * - inflate：解压数据；
 * - gzip：gzip 压缩格式。
 * 
 * 在使用 zlib 前，需要先根据需要使用的压缩算法选择其中一种。可以参考 zlib 的常量定义模块 `constants` 中的常量来选择相应的压缩算法。比如，我们使用 deflate 压缩算法进行模块说明：
 * 
 * ```JavaScript
 * const zlib = require('zlib');
 * const { NO_COMPRESSION, BEST_SPEED, BEST_COMPRESSION, DEFAULT_COMPRESSION } = require('constants'); // 省略引用常量的方法
 * 
 * // 压缩数据
 * const deflated = zlib.deflate('hello, world', BEST_SPEED);
 * console.log(deflated.toString());
 * 
 * // 解压数据
 * const inflated = zlib.inflate(deflated);
 * console.log(inflated.toString());
 * ```
 * 
 * 上面的代码展示了如何压缩和解压数据，先用 `zlib.deflate` 方法压缩 `hello, world` 这个字符串，并传入 `BEST_SPEED` 作为压缩级别选项，然后用 `zlib.inflate` 方法解压缩该数据，输出的结果应该与原始字符串相同。
 * 
 * `zlib.deflate` 和 `zlib.inflate` 都支持定义压缩级别，压缩级别是一个数字，取值范围为 `[NO_COMPRESSION, BEST_SPEED, DEFAULT_COMPRESSION, BEST_COMPRESSION]`，默认值为 `DEFAULT_COMPRESSION`。关于这 4 种压缩级别的含义，可以参考下面的 table：
 * 
 * | Compression Level | Meaning                                                                      |
 * | ----------------- | ---------------------------------------------------------------------------- |
 * | zlib.NO_COMPRESSION | 不压缩数据（含有压缩头完成的支持）                                    |
 * | zlib.BEST_SPEED     | 最快的压缩速度；但是压缩比也相应的差一些                           |
 * | zlib.DEFAULT_COMPRESSION | 根据压缩算法的默认值，通常情况下比 BEST_SPEED 的压缩速度慢但压缩率更高 |
 * | zlib.BEST_COMPRESSION   | 最高压缩比，但压缩速度也相应较慢。                                   |
 * 
 * 在使用 `zlib` 模块时需要注意的是，如果要同时压缩和解压数据，建议先使用 `deflate` 对数据进行压缩之后再使用 `inflate` 对数据进行解压缩，避免出现错误。而对于不同的压缩格式和算法，还有其他的类和方法进行压缩和解压缩，可以参考以下文档进行使用。
 *  
 */
declare module 'zlib' {
    /**
     * @description deflate 压缩级别，设定不压缩 
     */
    export const NO_COMPRESSION: 0;

    /**
     * @description deflate 压缩级别，设定最快压缩 
     */
    export const BEST_SPEED: 1;

    /**
     * @description deflate 压缩级别，设定最高压缩 
     */
    export const BEST_COMPRESSION: 9;

    /**
     * @description deflate 压缩级别，设定缺省设置 
     */
    export const DEFAULT_COMPRESSION: -1;

    /**
     * @description 创建一个 deflate 流对象 
     *      @param to 用于存储处理结果的流
     *      @return 返回封装过的流对象
     */
    function createDeflate(to: Class_Stream): Class_Stream;

    /**
     * @description 创建一个 deflateRaw 流对象 
     *      @param to 用于存储处理结果的流
     *      @return 返回封装过的流对象
     */
    function createDeflateRaw(to: Class_Stream): Class_Stream;

    /**
     * @description 创建一个 gunzip 流对象 
     *      @param to 用于存储处理结果的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回封装过的流对象
     */
    function createGunzip(to: Class_Stream, maxSize?: number): Class_Stream;

    /**
     * @description 创建一个 gzip 流对象 
     *      @param to 用于存储处理结果的流
     *      @return 返回封装过的流对象
     */
    function createGzip(to: Class_Stream): Class_Stream;

    /**
     * @description 创建一个 inflate 流对象 
     *      @param to 用于存储处理结果的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回封装过的流对象
     */
    function createInflate(to: Class_Stream, maxSize?: number): Class_Stream;

    /**
     * @description 创建一个 inflateRaw 流对象 
     *      @param to 用于存储处理结果的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回封装过的流对象
     */
    function createInflateRaw(to: Class_Stream, maxSize?: number): Class_Stream;

    /**
     * @description 使用 deflate 算法压缩数据(zlib格式)
     *      @param data 给定要压缩的数据
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      @return 返回压缩后的二进制数据
     *      
     */
    function deflate(data: Class_Buffer, level: number): Class_Buffer;

    function deflate(data: Class_Buffer, level: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 deflate 算法压缩数据到流对象中(zlib格式)
     *      @param data 给定要压缩的数据
     *      @param stm 指定存储压缩数据的流
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      
     */
    function deflateTo(data: Class_Buffer, stm: Class_Stream, level: number): void;

    function deflateTo(data: Class_Buffer, stm: Class_Stream, level: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 使用 deflate 算法压缩源流中的数据到流对象中(zlib格式)
     *      @param src 给定要压缩的数据所在的流
     *      @param stm 指定存储压缩数据的流
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      
     */
    function deflateTo(src: Class_Stream, stm: Class_Stream, level: number): void;

    function deflateTo(src: Class_Stream, stm: Class_Stream, level: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 解压缩 deflate 算法压缩的数据(zlib格式)
     *      @param data 给定压缩后的数据
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回解压缩后的二进制数据
     *      
     */
    function inflate(data: Class_Buffer, maxSize?: number): Class_Buffer;

    function inflate(data: Class_Buffer, maxSize?: number, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 解压缩 deflate 算法压缩的数据到流对象中(zlib格式)
     *      @param data 给定要解压缩的数据
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function inflateTo(data: Class_Buffer, stm: Class_Stream, maxSize?: number): void;

    function inflateTo(data: Class_Buffer, stm: Class_Stream, maxSize?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 解压缩源流中 deflate 算法压缩的数据到流对象中(zlib格式)
     *      @param src 给定要解压缩的数据所在的流
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function inflateTo(src: Class_Stream, stm: Class_Stream, maxSize?: number): void;

    function inflateTo(src: Class_Stream, stm: Class_Stream, maxSize?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 使用 gzip 算法压缩数据
     *      @param data 给定要压缩的数据
     *      @return 返回压缩后的二进制数据
     *      
     */
    function gzip(data: Class_Buffer): Class_Buffer;

    function gzip(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 gzip 算法压缩数据到流对象中
     *      @param data 给定要压缩的数据
     *      @param stm 指定存储压缩数据的流
     *      
     */
    function gzipTo(data: Class_Buffer, stm: Class_Stream): void;

    function gzipTo(data: Class_Buffer, stm: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 使用 gzip 算法压缩源流中的数据到流对象中
     *      @param src 给定要压缩的数据所在的流
     *      @param stm 指定存储压缩数据的流
     *      
     */
    function gzipTo(src: Class_Stream, stm: Class_Stream): void;

    function gzipTo(src: Class_Stream, stm: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 解压缩 gzip 算法压缩的数据
     *      @param data 给定压缩后的数据
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回解压缩后的二进制数据
     *      
     */
    function gunzip(data: Class_Buffer, maxSize?: number): Class_Buffer;

    function gunzip(data: Class_Buffer, maxSize?: number, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 解压缩 gzip 算法压缩的数据到流对象中
     *      @param data 给定要解压缩的数据
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function gunzipTo(data: Class_Buffer, stm: Class_Stream, maxSize?: number): void;

    function gunzipTo(data: Class_Buffer, stm: Class_Stream, maxSize?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 解压缩源流中 gzip 算法压缩的数据到流对象中
     *      @param src 给定要解压缩的数据所在的流
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function gunzipTo(src: Class_Stream, stm: Class_Stream, maxSize?: number): void;

    function gunzipTo(src: Class_Stream, stm: Class_Stream, maxSize?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 使用 deflate 算法压缩数据(deflateRaw)
     *      @param data 给定要压缩的数据
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      @return 返回压缩后的二进制数据
     *      
     */
    function deflateRaw(data: Class_Buffer, level: number): Class_Buffer;

    function deflateRaw(data: Class_Buffer, level: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 使用 deflate 算法压缩数据到流对象中(deflateRaw)
     *      @param data 给定要压缩的数据
     *      @param stm 指定存储压缩数据的流
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      
     */
    function deflateRawTo(data: Class_Buffer, stm: Class_Stream, level: number): void;

    function deflateRawTo(data: Class_Buffer, stm: Class_Stream, level: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 使用 deflate 算法压缩源流中的数据到流对象中(deflateRaw)
     *      @param src 给定要压缩的数据所在的流
     *      @param stm 指定存储压缩数据的流
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      
     */
    function deflateRawTo(src: Class_Stream, stm: Class_Stream, level: number): void;

    function deflateRawTo(src: Class_Stream, stm: Class_Stream, level: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 解压缩 deflate 算法压缩的数据(inflateRaw)
     *      @param data 给定压缩后的数据
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回解压缩后的二进制数据
     *      
     */
    function inflateRaw(data: Class_Buffer, maxSize?: number): Class_Buffer;

    function inflateRaw(data: Class_Buffer, maxSize?: number, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 解压缩 deflate 算法压缩的数据到流对象中(inflateRaw)
     *      @param data 给定要解压缩的数据
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function inflateRawTo(data: Class_Buffer, stm: Class_Stream, maxSize?: number): void;

    function inflateRawTo(data: Class_Buffer, stm: Class_Stream, maxSize?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 解压缩源流中 deflate 算法压缩的数据到流对象中(inflateRaw)
     *      @param src 给定要解压缩的数据所在的流
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function inflateRawTo(src: Class_Stream, stm: Class_Stream, maxSize?: number): void;

    function inflateRawTo(src: Class_Stream, stm: Class_Stream, maxSize?: number, callback?: (err: Error | undefined | null)=>any): void;

}

