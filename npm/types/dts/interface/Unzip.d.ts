/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Unzip 解压缩编解码器，自动检测 gzip 或 deflate 格式并解压 
 */
declare class Class_Unzip extends Class_ZlibCodec {
    /**
     * @description Unzip 构造函数
     *      @param opts 解压缩选项
     *      
     */
    constructor(opts?: FIBJS.GeneralObject);

}

