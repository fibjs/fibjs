/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Inflate 解压缩编解码器，解压 deflate 算法压缩的数据(zlib格式) 
 */
declare class Class_Inflate extends Class_ZlibCodec {
    /**
     * @description Inflate 构造函数
     *      @param opts 解压缩选项
     *      
     */
    constructor(opts?: FIBJS.GeneralObject);

}

