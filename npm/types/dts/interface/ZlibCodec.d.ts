/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description ZlibCodec 是 zlib 压缩解压缩编解码器的基类，提供 zlib 类构造函数接口
 * 
 *  ZlibCodec 继承自 EventEmitter，可被用于 minizlib 等 npm 包。子类包括 Gzip、Gunzip、Deflate、Inflate、DeflateRaw、InflateRaw、Unzip。
 * 
 *  ```JavaScript
 *  const zlib = require('zlib');
 *  const gzip = new zlib.Gzip({});
 *  const result = gzip._processChunk(Buffer.from('hello'), zlib.constants.Z_FINISH);
 *  ```
 *  
 */
declare class Class_ZlibCodec extends Class_EventEmitter {
    /**
     * @description 同步处理一块数据
     *      @param chunk 要处理的数据
     *      @param flushFlag 刷新标志，参见 zlib.constants.Z_NO_FLUSH 等
     *      @return 返回处理后的数据
     *      
     */
    _processChunk(chunk: Class_Buffer, flushFlag: number): Class_Buffer;

    /**
     * @description 关闭编解码器，释放资源 
     */
    close(): void;

    /**
     * @description 重置编解码器状态 
     */
    reset(): void;

    /**
     * @description 底层 handle 对象，用于内部兼容 
     */
    _handle: any;

    /**
     * @description 动态更新压缩参数
     *      @param level 压缩级别
     *      @param strategy 压缩策略
     *      
     */
    params(level: number, strategy: number): void;

}

