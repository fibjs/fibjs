/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlDocument.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description xml 处理模块，可以使用 xml 模块解析和处理 xml 和 html 文件
 * 
 * 要解析 xml 文件，你可以使用如下代码：
 * ```JavaScript
 * var xml = require('xml');
 * var fs = require('fs');
 * 
 * var xmlStr = fs.readFile('test.xml');
 * var xmlDoc = xml.parse(xmlStr);
 * 
 * console.log(xmlDoc.documentElement.nodeName);  // output root node name
 * ```
 * 在上面的代码中，我们使用 fs 模块的 readFile 方法读取了一个 xml 文件，然后使用 xml 模块的 parse 方法解析该 xml 文件，并返回一个 XmlDocument 对象 xmlDoc。然后，我们就可以通过 xmlDoc.documentElement 访问 xml 文档的根元素了。
 * 
 * 要解析 html 文件，你只需稍微修改一下代码：
 * ```JavaScript
 * var xml = require('xml');
 * var fs = require('fs');
 * 
 * var htmlStr = fs.readFile('test.html');
 * var xmlDoc = xml.parse(htmlStr, 'text/html');
 * 
 * console.log(xmlDoc.documentElement.nodeName);  // output root node name
 * ```
 * 在这里，我们同样使用了 fs 模块的 readFile 方法读取了一个 html 文件，但我们在调用 xml 模块的 parse 方法时指定了第二个参数为 ‘text/html’，这样 xml 模块就会按照 html 的语法规则解析文件。
 * 
 * 解析后的 Xml 文档对象都是 XmlDocument 类型，其属性和方法都可以参考 xml 对象模型（DOM）进行操作。
 * 
 */
declare module 'xml' {
    /**
     * @description XmlNode 的 nodeType 属性常量，表示节点为 XmlElement 对象
     *     
     */
    export const ELEMENT_NODE: 1;

    /**
     * @description XmlNode 的 nodeType 属性常量，表示节点为 XmlAttr 对象
     *     
     */
    export const ATTRIBUTE_NODE: 2;

    /**
     * @description XmlNode 的 nodeType 属性常量，表示节点为 XmlText 对象
     *     
     */
    export const TEXT_NODE: 3;

    /**
     * @description XmlNode 的 nodeType 属性常量，表示节点为 XmlCDATASection 对象
     *     
     */
    export const CDATA_SECTION_NODE: 4;

    /**
     * @description XmlNode 的 nodeType 属性常量，表示节点为 XmlProcessingInstruction 对象
     *     
     */
    export const PROCESSING_INSTRUCTION_NODE: 7;

    /**
     * @description XmlNode 的 nodeType 属性常量，表示节点为 XmlComment 对象
     *     
     */
    export const COMMENT_NODE: 8;

    /**
     * @description XmlNode 的 nodeType 属性常量，表示节点为 XmlDocument 对象
     *     
     */
    export const DOCUMENT_NODE: 9;

    /**
     * @description XmlNode 的 nodeType 属性常量，表示节点为 XmlDocumentType 对象
     *     
     */
    export const DOCUMENT_TYPE_NODE: 10;

    /**
     * @description xml 文档对象，参见 XmlDocument 对象
     */
    const Document: typeof Class_XmlDocument;

    /**
     * @description 解析 xml/html 文本，并创建 XmlDocument 对象，不支持多语种
     *      @param source 指定需要解析的 xml/html 文本
     *      @param type 指定文本类型，缺省为 text/xml，也可指定为 text/html
     *      @return 返回创建的 XmlDocument 对象
     *     
     */
    function parse(source: string, type?: string): Class_XmlDocument;

    /**
     * @description 解析 xml/html，并创建 XmlDocument 对象，解析时会根据指定的语种转换
     *      @param source 指定需要解析的 xml/html 二进制数据
     *      @param type 指定文本类型，缺省为 text/xml，也可指定为 text/html
     *      @return 返回创建的 XmlDocument 对象
     *     
     */
    function parse(source: Class_Buffer, type?: string): Class_XmlDocument;

    /**
     * @description 序列化 XmlNode 为字符串
     *      @param node 指定需要序列化的 XmlNode
     *      @return 返回序列化的字符串
     *     
     */
    function serialize(node: Class_XmlNode): string;

}

