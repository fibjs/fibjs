/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description 文件路径处理模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var path = require('path').win32;
 *  ```
 *  
 */
declare module 'path_win32' {
    /**
     * @description 标准化路径，处理路径中父目录等信息
     * 
     *      @param path 给定的未处理的路径
     *      @return 返回经过处理的路径
     *      
     */
    function normalize(path: string): string;

    /**
     * @description 查询路径中的文件名称，若指定扩展名，则自动取消匹配的扩展名
     * 
     *      @param path 给定查询的路径
     *      @param ext 指定扩展名，若文件名中有符合条件的扩展名，将自动取消
     *      @return 返回文件名称
     *      
     */
    function basename(path: string, ext?: string): string;

    /**
     * @description 查询路径中的文件扩展名
     * 
     *      @param path 给定查询的路径
     *      @return 返回得到的扩展名
     *      
     */
    function extname(path: string): string;

    /**
     * @description 尝试将一个对象格式化为路径
     * 
     *      pathObject 支持的字段如下：
     *      ```JavaScript
     *      {
     *          "dir": "", // 指定路径的目录
     *          "root": "", // 指定路径的根目录
     *          "base": "", // 指定路径的文件名，等同于 ${name}.${ext}
     *          "name": "", // 指定路径的主文件名
     *          "ext": "", // 指定路径的扩展名
     *      }
     *      ```
     *      @param pathObject 指定参数
     *      @return 返回格式化后的路径
     *      
     */
    function format(pathObject: FIBJS.GeneralObject): string;

    /**
     * @description 解析路径为路径对象
     * 
     *      @param path 路径
     *      @return 返回 pathObject 对象
     *      
     */
    function parse(path: string): FIBJS.GeneralObject;

    /**
     * @description 查询路径中的目录路径
     * 
     *      @param path 给定查询的路径
     *      @return 返回得到的目录的路径
     *      
     */
    function dirname(path: string): string;

    /**
     * @description 转换给定路径为全路径
     * 
     *      @param path 给定转换的路径
     *      @return 返回转换的全路径
     *      
     */
    function fullpath(path: string): string;

    /**
     * @description 识别给定的路径是否是绝对路径
     * 
     *      @param path 给定需要识别的路径
     *      @return 是绝对路径则返回 true
     *      
     */
    function isAbsolute(path: string): boolean;

    /**
     * @description 合并一系列路径成为一个单一路径
     * 
     *      @param ps 一个或多个相关的路径
     *      @return 返回得到的新路径
     *      
     */
    function join(...ps: any[]): string;

    /**
     * @description 合并一系列路径成为一个绝对路径
     * 
     *      @param ps 一个或多个相关的路径
     *      @return 返回得到的新路径
     *      
     */
    function resolve(...ps: any[]): string;

    /**
     * @description 求 _from 到 to 的相对路径
     * 
     *      @param _from 源路径
     *      @param to 目标路径
     *      @return 返回得到的相对路径
     *      
     */
    function relative(_from: string, to: string): string;

    /**
     * @description 转换成 namespace-prefixed 路径。只在 windows 有效，其他系统直接返回。
     *     see: https://msdn.microsoft.com/library/windows/desktop/aa365247(v=vs.85).aspx#namespaces
     *      @param path 给定的路径。
     *      @return 返回得到的新路径
     *      
     */
    function toNamespacedPath(path?: any): any;

    /**
     * @description 查询当前操作系统的路径分割字符，posix 返回 '/', windows 返回  '\\'
     *      
     */
    const sep: string;

    /**
     * @description 查询当前操作系统的多路径组合字符，posix 返回 ':', windows 返回  ';'
     *      
     */
    const delimiter: string;

    /**
     * @description posix 实现，参见 path_posix
     *      
     */
    const posix: FIBJS.GeneralObject;

    /**
     * @description windows 实现，参见 path_win32
     *      
     */
    const win32: FIBJS.GeneralObject;

}

