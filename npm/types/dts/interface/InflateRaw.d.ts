/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description InflateRaw 解压缩编解码器，解压 deflate 算法压缩的数据(raw格式) 
 */
declare class Class_InflateRaw extends Class_ZlibCodec {
    /**
     * @description InflateRaw 构造函数
     *      @param opts 解压缩选项
     *      
     */
    constructor(opts?: FIBJS.GeneralObject);

}

