/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * !
 *  * FTModel fasttext 模型对象
 *  *
 *  
 */
declare class Class_FTModel extends Class_object {
    /**
     * @description 预测文本的分类
     *      @param text 文本
     *      @param k 返回前 k 个分类
     *      @return 返回分类结果
     *      
     */
    predict(text: string, k?: number): any[];

    predict(text: string, k?: number, callback?: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 预测文本的分类
     *      @param texts 文本数组
     *      @param k 返回前 k 个分类
     *      @return 返回分类结果
     *      
     */
    predict(texts: any[], k?: number): any[];

    predict(texts: any[], k?: number, callback?: (err: Error | undefined | null, retVal: any[])=>any): void;

}

