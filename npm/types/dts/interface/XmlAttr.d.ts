/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description XmlAttr 对象表示 XmlElement 对象的属性 
 */
declare class Class_XmlAttr extends Class_object {
    /**
     * @description 查询元素的本地名称。如果选定的节点无命名空间，则该属性等同于 nodeName
     *     
     */
    readonly localName: string;

    /**
     * @description 属性的值
     *     
     */
    value: string;

    /**
     * @description 属性的名称
     *     
     */
    readonly name: string;

    /**
     * @description 查询元素的命名空间的 URI。如果选定的节点无命名空间，则该属性返回 NULL
     *     
     */
    readonly namespaceURI: string;

    /**
     * @description 查询和设置元素的命名空间前缀。如果选定的节点无命名空间，则该属性返回 NULL
     *     
     */
    prefix: string;

    /**
     * @description 属性的名称，为兼容的目的
     *     
     */
    readonly nodeName: string;

    /**
     * @description 属性的值，为兼容的目的
     *     
     */
    nodeValue: string;

}

