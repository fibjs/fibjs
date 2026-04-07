/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description DeflateRaw 压缩编解码器，使用 deflate 算法压缩数据(raw格式，不含zlib头) 
 */
declare class Class_DeflateRaw extends Class_ZlibCodec {
    /**
     * @description DeflateRaw 构造函数
     *      @param opts 压缩选项
     *      
     */
    constructor(opts?: FIBJS.GeneralObject);

}

