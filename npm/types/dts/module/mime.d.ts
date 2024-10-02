/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description MIME 解析模块 
 */
declare module 'mime' {
    /**
     * @description 根据提供的文件名解析对应的 MIME 类型
     * 
     *      @param fname 指定要解析的文件名
     *      @return 返回解析出的 MIME 类型
     *      
     */
    function getType(fname: string): string;

    /**
     * @description 添加 MIME 类型
     * 
     *      @param ext 指定的扩展名
     *      @param type 指定的 MIME 类型
     *      
     */
    function addType(ext: string, type: string): void;

}

