/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Gunzip 解压缩编解码器，解压 gzip 压缩的数据 
 */
declare class Class_Gunzip extends Class_ZlibCodec {
    /**
     * @description Gunzip 构造函数
     *      @param opts 解压缩选项
     *      
     */
    constructor(opts?: FIBJS.GeneralObject);

}

