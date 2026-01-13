/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description XMLSerializer 接口提供将 DOM 树序列化为 XML 字符串的能力
 * 
 *  XMLSerializer 可以将 DOM 节点序列化为 XML 字符串：
 *  ```JavaScript
 *  const serializer = new XMLSerializer();
 *  
 *  // 序列化 XML 文档
 *  const parser = new DOMParser();
 *  const doc = parser.parseFromString('<root><item>data</item></root>', 'text/xml');
 *  const xmlStr = serializer.serializeToString(doc);
 *  console.log(xmlStr); // 输出: <root><item>data</item></root>
 *  ```
 * 
 */
declare class Class_XMLSerializer extends Class_object {
    /**
     * @description 构造一个 XMLSerializer 对象 
     */
    constructor();

    /**
     * @description 将 DOM 节点序列化为 XML 字符串
     *      @param node 要序列化的 DOM 节点
     *      @return 返回序列化后的 XML 字符串
     *     
     */
    serializeToString(node: Class_XmlNode): string;

}

