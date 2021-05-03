/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description zlib 压缩解压模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  var zlib = require('zlib');
 *  ```
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
    function createGunzip(to: Class_Stream, maxSize: number): Class_Stream;

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
    function createInflate(to: Class_Stream, maxSize: number): Class_Stream;

    /**
     * @description 创建一个 inflateRaw 流对象 
     *      @param to 用于存储处理结果的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回封装过的流对象
     */
    function createInflateRaw(to: Class_Stream, maxSize: number): Class_Stream;

    /**
     * @description 使用 deflate 算法压缩数据(zlib格式)
     *      @param data 给定要压缩的数据
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      @return 返回压缩后的二进制数据
     *      
     */
    function deflate(data: Class_Buffer, level: number): Class_Buffer;

    /**
     * @description 使用 deflate 算法压缩数据到流对象中(zlib格式)
     *      @param data 给定要压缩的数据
     *      @param stm 指定存储压缩数据的流
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      
     */
    function deflateTo(data: Class_Buffer, stm: Class_Stream, level: number): void;

    /**
     * @description 解压缩 deflate 算法压缩的数据(zlib格式)
     *      @param data 给定压缩后的数据
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回解压缩后的二进制数据
     *      
     */
    function inflate(data: Class_Buffer, maxSize: number): Class_Buffer;

    /**
     * @description 解压缩 deflate 算法压缩的数据到流对象中(zlib格式)
     *      @param data 给定要解压缩的数据
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function inflateTo(data: Class_Buffer, stm: Class_Stream, maxSize: number): void;

    /**
     * @description 使用 gzip 算法压缩数据
     *      @param data 给定要压缩的数据
     *      @return 返回压缩后的二进制数据
     *      
     */
    function gzip(data: Class_Buffer): Class_Buffer;

    /**
     * @description 使用 gzip 算法压缩数据到流对象中
     *      @param data 给定要压缩的数据
     *      @param stm 指定存储压缩数据的流
     *      
     */
    function gzipTo(data: Class_Buffer, stm: Class_Stream): void;

    /**
     * @description 解压缩 gzip 算法压缩的数据
     *      @param data 给定压缩后的数据
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回解压缩后的二进制数据
     *      
     */
    function gunzip(data: Class_Buffer, maxSize: number): Class_Buffer;

    /**
     * @description 解压缩 gzip 算法压缩的数据到流对象中
     *      @param data 给定要解压缩的数据
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function gunzipTo(data: Class_Buffer, stm: Class_Stream, maxSize: number): void;

    /**
     * @description 使用 deflate 算法压缩数据(deflateRaw)
     *      @param data 给定要压缩的数据
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      @return 返回压缩后的二进制数据
     *      
     */
    function deflateRaw(data: Class_Buffer, level: number): Class_Buffer;

    /**
     * @description 使用 deflate 算法压缩数据到流对象中(deflateRaw)
     *      @param data 给定要压缩的数据
     *      @param stm 指定存储压缩数据的流
     *      @param level 指定压缩级别，缺省为 DEFAULT_COMPRESSION
     *      
     */
    function deflateRawTo(data: Class_Buffer, stm: Class_Stream, level: number): void;

    /**
     * @description 解压缩 deflate 算法压缩的数据(inflateRaw)
     *      @param data 给定压缩后的数据
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      @return 返回解压缩后的二进制数据
     *      
     */
    function inflateRaw(data: Class_Buffer, maxSize: number): Class_Buffer;

    /**
     * @description 解压缩 deflate 算法压缩的数据到流对象中(inflateRaw)
     *      @param data 给定要解压缩的数据
     *      @param stm 指定存储解压缩数据的流
     *      @param maxSize 指定解压缩尺寸限制，缺省为 -1，不限制
     *      
     */
    function inflateRawTo(data: Class_Buffer, stm: Class_Stream, maxSize: number): void;

}

