/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The XmlAttr object represents an attribute of an XmlElement object
 */
declare class Class_XmlAttr extends Class_object {
    /**
     * @description Queries the local name of the element. If the selected node has no namespace, this property is equivalent to nodeName
     *
     */
    readonly localName: string;

    /**
     * @description The value of the attribute
     *
     */
    value: string;

    /**
     * @description The name of the attribute
     *
     */
    readonly name: string;

    /**
     * @description Queries the namespace URI of the element. If the selected node has no namespace, this property returns NULL
     *
     */
    readonly namespaceURI: string;

    /**
     * @description Queries and sets the namespace prefix of the element. If the selected node has no namespace, this property returns NULL
     *
     */
    prefix: string;

    /**
     * @description The name of the attribute, for compatibility purposes
     *
     */
    readonly nodeName: string;

    /**
     * @description The value of the attribute, for compatibility purposes
     *
     */
    nodeValue: string;

    /**
     * @description Clones the XmlAttr object
     *     @return returns a copy of the XmlAttr object
     *
     */
    cloneNode(): Class_XmlAttr;

}

