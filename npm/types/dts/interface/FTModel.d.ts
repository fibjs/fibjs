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
     * @description 查询当前模型是否是量化模型
     *      @return 返回是否是量化模型
     *      
     */
    isQuantized(): boolean;

    /**
     * @description 获取当前模型的维度
     *      @return 返回模型维度
     *      
     */
    getDimension(): number;

    /**
     * @description 获取给定词的词向量
     *      @param word 词
     *      @return 返回词向量
     *      
     */
    getWordVector(word: string): any[];

    /**
     * @description 获取给定句子的句向量
     *      @param text 句子
     *      @return 返回句向量
     *      
     */
    getSentenceVector(text: string): any[];

    /**
     * @description 获取给定词的近义词
     *      @param word 词
     *      @param k 返回前 k 个近义词
     *      @return 返回近义词结果
     *      
     */
    getNearestNeighbors(word: string, k?: number): any[];

    /**
     * @description 返回操作 wordA - wordB + wordC 的最近k个邻居
     *      @param wordA 词A
     *      @param wordB 词B
     *      @param wordC 词C
     *      @param k 返回前 k 个近义词
     *      @return 返回近义词结果
     *      
     */
    getAnalogies(wordA: string, wordB: string, wordC: string, k?: number): any[];

    /**
     * @description 给定一个单词，获取该单词在词典中的词ID
     *      @param word 词
     *      @return 返回词ID
     *      
     */
    getWordId(word: string): number;

    /**
     * @description 给定一个子词，获取该子词在词典中的词ID
     *      @param subword 子词
     *      @return 返回子词ID
     *      
     */
    getSubwordId(subword: string): number;

    /**
     * @description 给定一个索引，获取输入矩阵对应的向量
     *      @param id 索引
     *      @return 返回输入矩阵对应的向量
     *      
     */
    getInputVector(id: number): any[];

    /**
     * @description 返回子单词及其索引
     *      @param word 词
     *      @return 返回子单词及其索引
     *      
     */
    getSubwords(word: string): [subwords: [object Object], ngrams: [object Object]];

    /**
     * @description 预测文本的分类
     *      @param text 文本
     *      @param k 返回前 k 个分类
     *      @return 返回分类结果
     *      
     */
    predict(text: string, k?: number, threshold?: number): any[];

    predict(text: string, k?: number, threshold?: number, callback?: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 预测文本的分类
     *      @param texts 文本数组
     *      @param k 返回前 k 个分类
     *      @param threshold 阈值
     *      @return 返回分类结果
     *      
     */
    predict(texts: any[], k?: number, threshold?: number): any[];

    predict(texts: any[], k?: number, threshold?: number, callback?: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 量化模型，减小模型的大小和内存占用
     *      @param trainFile 训练文件
     *      @param args 量化参数
     *      
     */
    quantize(trainFile: string, args?: FIBJS.GeneralObject): void;

    quantize(trainFile: string, args?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 获取当前模型的词典及词频
     *      @return 返回词典及词频
     *      
     */
    getWords(): [vocabs: [object Object], freqs: [object Object]];

    /**
     * @description 获取当前模型的标签及标签频率
     *      @return 返回标签及标签频率
     *      
     */
    getLabels(): [labels: [object Object], freqs: [object Object]];

    /**
     * @description 保存当前模型
     *      @param filename 模型文件名
     *      
     */
    saveModel(filename: string): void;

}

