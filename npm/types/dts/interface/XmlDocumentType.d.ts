/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description XmlDocumentType 对象用于访问 XML 所定义的实体
 * 
 */
declare class Class_XmlDocumentType extends Class_XmlNode {
    /**
     * @description 返回 DTD 的名称
     * 	
     */
    readonly name: string;

    /**
     * @description 可返回外部 DTD 的公共识别符
     * 	
     */
    readonly publicId: string;

    /**
     * @description 可返回外部 DTD 的系统识别符
     * 	
     */
    readonly systemId: string;

}

