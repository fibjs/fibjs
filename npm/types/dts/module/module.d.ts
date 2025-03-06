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

}

