/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description The XmlDocumentType object is used to access the entities defined by XML
 *
 */
declare class Class_XmlDocumentType extends Class_XmlNode {
    /**
     * @description Returns the name of the DTD
     *
     */
    readonly name: string;

    /**
     * @description Returns the public identifier of the external DTD
     *
     */
    readonly publicId: string;

    /**
     * @description Returns the system identifier of the external DTD
     *
     */
    readonly systemId: string;

}

