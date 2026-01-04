/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description XmlDocumentFragment 对象表示一个轻量级的文档对象，能够容纳文档的某个部分
 * 
 * XmlDocumentFragment 节点不属于文档树，它的 parentNode 始终为 null。但它可以有子节点，子节点可以是 XmlElement、XmlText 或其他类型的节点。
 * 
 * 当把一个 DocumentFragment 节点插入文档树时，插入的不是 DocumentFragment 本身，而是它的所有子孙节点。这使得 DocumentFragment 成了一个非常有用的占位符，暂时存放那些一次插入文档的节点。
 * 
 * 可以通过 XmlDocument 的 createDocumentFragment() 方法创建 XmlDocumentFragment 对象。
 * 
 */
declare class Class_XmlDocumentFragment extends Class_XmlNode {
}

