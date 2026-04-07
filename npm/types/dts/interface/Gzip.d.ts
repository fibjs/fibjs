/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Gzip 压缩编解码器，使用 gzip 算法压缩数据 
 */
declare class Class_Gzip extends Class_ZlibCodec {
    /**
     * @description Gzip 构造函数
     *      @param opts 压缩选项
     *      
     */
    constructor(opts?: FIBJS.GeneralObject);

}

