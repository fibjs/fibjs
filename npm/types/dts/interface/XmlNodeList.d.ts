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

}

