/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Stat.d.ts" />
/**
 * @description 可移动当前指针的流对象接口 
 */
declare class Class_SeekableStream extends Class_Stream {
    /**
     * @description 移动文件当前操作位置
     *       @param offset 指定新的位置
     *       @param whence 指定位置基准，允许的值为：SEEK_SET, SEEK_CUR, SEEK_END
     *      
     */
    seek(offset: number, whence: number): void;

    /**
     * @description 查询流当前位置
     *      @return 返回流当前位置
     *      
     */
    tell(): number;

    /**
     * @description 移动当前位置到流开头 
     */
    rewind(): void;

    /**
     * @description 查询流尺寸
     *      @return 返回流尺寸
     *      
     */
    size(): number;

    /**
     * @description 从流内读取剩余的全部数据
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readAll(): Class_Buffer;

    /**
     * @description 修改文件尺寸，如果新尺寸小于原尺寸，则文件被截断
     *       @param bytes 新的文件尺寸
     *      
     */
    truncate(bytes: number): void;

    /**
     * @description 查询文件是否到结尾
     *      @return 返回 True 表示结尾
     *      
     */
    eof(): boolean;

    /**
     * @description 查询当前文件的基础信息
     *      @return 返回 Stat 对象描述文件信息
     *      
     */
    stat(): Class_Stat;

}

