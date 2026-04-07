/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Deflate 压缩编解码器，使用 deflate 算法压缩数据(zlib格式) 
 */
declare class Class_Deflate extends Class_ZlibCodec {
    /**
     * @description Deflate 构造函数
     *      @param opts 压缩选项
     *      
     */
    constructor(opts?: FIBJS.GeneralObject);

}

