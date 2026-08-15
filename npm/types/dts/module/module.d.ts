/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description 基础模块管理 
 */
declare module 'module' {
    /**
     * !@description 创建一个模块引用函数
     *      @param base 模块的基础路径
     *      @return 返回一个模块引用函数
     *     
     */
    function createRequire(base: string): (...args: any[])=>any;

    /**
     * !@description 内建模块名称列表
     *      内建模块名称列表。包含了所有的 fibjs 内建模块名称，以及带 node: 前缀的版本。
     *     
     */
    const builtinModules: any[];

    /**
     * !@description 启用模块编译缓存
     *      在 fibjs 中为空操作，用于将 V8 编译字节码缓存到磁盘以加快后续启动速度。
     *      @param cacheDir 缓存目录路径，可选
     *      @return 返回包含 status 和 message 的对象，status 为 2 表示不支持
     *     
     */
    function enableCompileCache(cacheDir?: string): FIBJS.GeneralObject;

}

