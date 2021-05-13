/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlDocument.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description xml 处理模块
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

