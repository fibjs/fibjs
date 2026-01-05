/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description XmlNodeList 对象代表一个有顺序的节点列表
 * 
 */
declare class Class_XmlNodeList extends Class_object {
    /**
     * @description 返回节点列表中的节点数目
     *     
     */
    readonly length: number;

    /**
     * @description 返回节点列表中处于指定的索引号的节点
     *      @param index 指定要查询的索引
     *      @return 指定索引号的节点
     *     
     */
    item(index: number): Class_XmlNode;

    "[Symbol.iterator]"(): Iterator<any>;

    /**
     * @description 对列表中的每个节点执行一次指定的回调函数
     *      @param callback 对每个节点执行的回调函数，接收三个参数：当前节点、索引、节点列表本身
     *     
     */
    forEach(callback: (...args: any[])=>any): void;

    /**
     * @description 返回一个迭代器，用于遍历节点列表中每个节点的索引
     *      @return 返回索引迭代器
     *     
     */
    keys(): Iterator<any>;

    /**
     * @description 返回一个迭代器，用于遍历节点列表中每个节点的值
     *      @return 返回值迭代器
     *     
     */
    values(): Iterator<any>;

    /**
     * @description 返回一个迭代器，用于遍历节点列表中每个节点的 [索引, 值] 对
     *      @return 返回键值对迭代器
     *     
     */
    entries(): Iterator<any>;

}

