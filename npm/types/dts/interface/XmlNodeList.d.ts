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


}

