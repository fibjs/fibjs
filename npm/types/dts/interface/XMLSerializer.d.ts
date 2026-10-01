/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description The XMLSerializer interface provides the ability to serialize a DOM tree into an XML string
 *
 *  XMLSerializer can serialize DOM nodes into XML strings:
 *  ```JavaScript
 *  const serializer = new XMLSerializer();
 *
 *  // serializes an XML document
 *  const parser = new DOMParser();
 *  const doc = parser.parseFromString('<root><item>data</item></root>', 'text/xml');
 *  const xmlStr = serializer.serializeToString(doc);
 *  console.log(xmlStr); // output: <root><item>data</item></root>
 *  ```
 *
 */
declare class Class_XMLSerializer extends Class_object {
    /**
     * @description Constructs an XMLSerializer object
     */
    constructor();

    /**
     * @description Serializes a DOM node into an XML string
     *      @param node the DOM node to serialize
     *      @return returns the serialized XML string
     *
     */
    serializeToString(node: Class_XmlNode): string;

}

