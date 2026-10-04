/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlDocument.d.ts" />
/// <reference path="../interface/DOMParser.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description The xml processing module; the xml module can be used to parse and process xml and html files
 *
 * To parse an xml file, you can use the following code:
 * ```JavaScript
 * var xml = require('xml');
 * var fs = require('fs');
 *
 * var xmlStr = fs.readFile('test.xml');
 * var xmlDoc = xml.parse(xmlStr);
 *
 * console.log(xmlDoc.documentElement.nodeName);  // output root node name
 * ```
 * In the above code, we use the readFile method of the fs module to read an xml file, then use the parse method of the xml module to parse the xml file and return an XmlDocument object xmlDoc. Then we can access the root element of the xml document through xmlDoc.documentElement.
 *
 * To parse an html file, you only need to modify the code slightly:
 * ```JavaScript
 * var xml = require('xml');
 * var fs = require('fs');
 *
 * var htmlStr = fs.readFile('test.html');
 * var xmlDoc = xml.parse(htmlStr, 'text/html');
 *
 * console.log(xmlDoc.documentElement.nodeName);  // output root node name
 * ```
 * Here we also use the readFile method of the fs module to read an html file, but we specify the second parameter as 'text/html' when calling the parse method of the xml module, so that the xml module parses the file according to the syntax rules of html.
 *
 * The parsed Xml document objects are all of type XmlDocument, and their properties and methods can be used by referring to the xml object model (DOM).
 *
 */
declare module 'xml' {
    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlElement object
     *
     */
    export const ELEMENT_NODE: 1;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlAttr object
     *
     */
    export const ATTRIBUTE_NODE: 2;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlText object
     *
     */
    export const TEXT_NODE: 3;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlCDATASection object
     *
     */
    export const CDATA_SECTION_NODE: 4;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an EntityReference object (deprecated)
     *
     */
    export const ENTITY_REFERENCE_NODE: 5;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an Entity object (deprecated)
     *
     */
    export const ENTITY_NODE: 6;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlProcessingInstruction object
     *
     */
    export const PROCESSING_INSTRUCTION_NODE: 7;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlComment object
     *
     */
    export const COMMENT_NODE: 8;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlDocument object
     *
     */
    export const DOCUMENT_NODE: 9;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlDocumentType object
     *
     */
    export const DOCUMENT_TYPE_NODE: 10;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is an XmlDocumentFragment object
     *
     */
    export const DOCUMENT_FRAGMENT_NODE: 11;

    /**
     * @description The nodeType property constant of XmlNode, indicating that the node is a Notation object (deprecated)
     *
     */
    export const NOTATION_NODE: 12;

    /**
     * @description The xml document object, see the XmlDocument object
     */
    const Document: typeof Class_XmlDocument;

    /**
     * @description The DOMParser interface, used to parse a string into a DOM document, see the DOMParser object
     */
    const DOMParser: typeof Class_DOMParser;

    /**
     * @description Parses xml/html and creates an XmlDocument object; converts according to the specified language during parsing
     *      @param source the xml/html data to parse; a string is encoded as utf8
     *      @param type the text type, default text/xml; can also be set to text/html
     *      @param options the parsing limits, default { maxElementDepth: 1000, maxNodeCount: 1000000 }
     *      @return returns the created XmlDocument object
     *
     */
    function parse(source: Class_Buffer | string, type?: string, options?: FIBJS.GeneralObject): Class_XmlDocument;

    /**
     * @description Serializes an XmlNode to a string
     *      @param node the XmlNode to serialize
     *      @return returns the serialized string
     *
     */
    function serialize(node: Class_XmlNode): string;

}

