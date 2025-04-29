/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description zlib 模块常用常量定义模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var constants = require('zlib').constants
 *  ```
 *  
 */
declare module 'zlib_constants' {
    /**
     * @description 不执行刷新操作 
     */
    export const Z_NO_FLUSH: 0;

    /**
     * @description 执行部分刷新操作 
     */
    export const Z_PARTIAL_FLUSH: 1;

    /**
     * @description 同步刷新，等待所有待处理的输出被刷新 
     */
    export const Z_SYNC_FLUSH: 2;

    /**
     * @description 完全刷新，等待所有输出被刷新并重置内部状态 
     */
    export const Z_FULL_FLUSH: 3;

    /**
     * @description 完成压缩或解压缩操作 
     */
    export const Z_FINISH: 4;

    /**
     * @description 在当前块结束时停止压缩 
     */
    export const Z_BLOCK: 5;

    /**
     * @description 操作成功完成 
     */
    export const Z_OK: 0;

    /**
     * @description 压缩或解压缩流结束 
     */
    export const Z_STREAM_END: 1;

    /**
     * @description 需要字典才能继续操作 
     */
    export const Z_NEED_DICT: 2;

    /**
     * @description 发生系统错误 
     */
    export const Z_ERRNO: -1;

    /**
     * @description 流状态不一致或参数无效 
     */
    export const Z_STREAM_ERROR: -2;

    /**
     * @description 输入数据损坏 
     */
    export const Z_DATA_ERROR: -3;

    /**
     * @description 内存分配失败 
     */
    export const Z_MEM_ERROR: -4;

    /**
     * @description 缓冲区错误 
     */
    export const Z_BUF_ERROR: -5;

    /**
     * @description 版本不匹配 
     */
    export const Z_VERSION_ERROR: -6;

    /**
     * @description 不压缩 
     */
    export const Z_NO_COMPRESSION: 0;

    /**
     * @description 最快速度压缩 
     */
    export const Z_BEST_SPEED: 1;

    /**
     * @description 最高压缩率 
     */
    export const Z_BEST_COMPRESSION: 9;

    /**
     * @description 默认压缩级别 
     */
    export const Z_DEFAULT_COMPRESSION: -1;

    /**
     * @description 过滤器压缩策略 
     */
    export const Z_FILTERED: 1;

    /**
     * @description 仅使用Huffman编码 
     */
    export const Z_HUFFMAN_ONLY: 2;

    /**
     * @description 运行长度编码 
     */
    export const Z_RLE: 3;

    /**
     * @description 固定哈夫曼编码 
     */
    export const Z_FIXED: 4;

    /**
     * @description 默认压缩策略 
     */
    export const Z_DEFAULT_STRATEGY: 0;

    /**
     * @description zlib 版本号 
     */
    export const ZLIB_VERNUM: 4800;

    /**
     * @description deflate 压缩 
     */
    export const DEFLATE: 1;

    /**
     * @description inflate 解压缩 
     */
    export const INFLATE: 2;

    /**
     * @description gzip 压缩 
     */
    export const GZIP: 3;

    /**
     * @description gunzip 解压缩 
     */
    export const GUNZIP: 4;

    /**
     * @description deflateRaw 压缩 
     */
    export const DEFLATERAW: 5;

    /**
     * @description inflateRaw 解压缩 
     */
    export const INFLATERAW: 6;

    /**
     * @description unzip 解压缩 
     */
    export const UNZIP: 7;

    /**
     * @description Brotli 解码 
     */
    export const BROTLI_DECODE: 8;

    /**
     * @description Brotli 编码 
     */
    export const BROTLI_ENCODE: 9;

    /**
     * @description 最小窗口大小 
     */
    export const Z_MIN_WINDOWBITS: 8;

    /**
     * @description 最大窗口大小 
     */
    export const Z_MAX_WINDOWBITS: 15;

    /**
     * @description 默认窗口大小 
     */
    export const Z_DEFAULT_WINDOWBITS: 15;

    /**
     * @description 最小块大小 
     */
    export const Z_MIN_CHUNK: 64;

    /**
     * @description 最大块大小 
     */
    export const Z_MAX_CHUNK: -1;

    /**
     * @description 默认块大小 
     */
    export const Z_DEFAULT_CHUNK: 16384;

    /**
     * @description 最小内存级别 
     */
    export const Z_MIN_MEMLEVEL: 1;

    /**
     * @description 最大内存级别 
     */
    export const Z_MAX_MEMLEVEL: 9;

    /**
     * @description 默认内存级别 
     */
    export const Z_DEFAULT_MEMLEVEL: 8;

    /**
     * @description 最低压缩级别 
     */
    export const Z_MIN_LEVEL: -1;

    /**
     * @description 最高压缩级别 
     */
    export const Z_MAX_LEVEL: 9;

    /**
     * @description 默认压缩级别 
     */
    export const Z_DEFAULT_LEVEL: -1;

}

