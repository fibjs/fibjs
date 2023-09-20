/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/FTModel.d.ts" />
/**
 * @description fasttext 文本处理与分类模块
 * 
 * 使用方法：
 * ```JavaScript
 * var fasttext = require('fasttext');
 * ```
 *  
 */
declare module 'fasttext' {
    /**
     * @description 加载 fasttext 模型
     *      @param path 模型文件路径
     *      @return 返回加载的模型对象
     *      
     */
    function loadModel(path: string): Class_FTModel;

    function loadModel(path: string, callback: (err: Error | undefined | null, retVal: Class_FTModel)=>any): void;

}

