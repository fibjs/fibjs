/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlAttr.d.ts" />
/**
 * @description XmlNamedNodeMap 对象表示一个无顺序的属性列表
 * 
 */
declare class Class_XmlNamedNodeMap extends Class_object {
    /**
     * @description 返回属性列表中的属性数目
     *     
     */
    readonly length: number;

    /**
     * @description 返回属性列表中处于指定的索引号的属性
     *      @param index 指定要查询的索引
     *      @return 指定索引号的属性
     *     
     */
    item(index: number): Class_XmlAttr;


    /**
     * @description 查询指定名称的属性
     *      @param name 指定要查询的名称
     *      @return 返回查询出的属性
     *     
     */
    getNamedItem(name: string): Class_XmlAttr;

}

