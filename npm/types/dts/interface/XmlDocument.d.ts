/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/XmlDocumentType.d.ts" />
/// <reference path="../interface/XmlElement.d.ts" />
/// <reference path="../interface/XmlNodeList.d.ts" />
/// <reference path="../interface/XmlText.d.ts" />
/// <reference path="../interface/XmlComment.d.ts" />
/// <reference path="../interface/XmlCDATASection.d.ts" />
/// <reference path="../interface/XmlProcessingInstruction.d.ts" />
/// <reference path="../interface/XmlDocumentFragment.d.ts" />
/**
 * @description XmlDocument is an object of the xml module; it represents the whole XML document and provides the entry point for accessing the whole document
 *
 * XmlDocument is the root of a document tree and contains all nodes in the whole XML document. The XmlDocument object also provides the following functions:
 *
 * 1. Creates element nodes, text nodes, comments, processing instructions, etc.
 * 2. Accesses and modifies document properties and related information (such as DTD comments and the document declaration)
 * 3. Parses XML documents
 *
 * The following is sample code that uses the XmlDocument object to parse an XML document:
 *
 * ```JavaScript
 * var xml = require('xml');
 * var fs = require('fs');
 *
 * var xmlStr = fs.readFile('test.xml');
 * var xmlDoc = xml.parse(xmlStr);
 *
 * // get document root node name
 * var rootName = xmlDoc.documentElement.nodeName;
 * console.log(`the document root node name is ${rootName}`);
 * ```
 *
 * In the above code, we first use the `readFile()` method of the `fs` module to read an XML file and assign the file stream to the variable `xmlStr`. Then we use the `parse()` method of the `xml` module to parse the XML file and assign the parsed `XmlDocument` object to the variable `xmlDoc`. Finally we use the `documentElement` property of `xmlDoc` to get the document root node and obtain its node name, which is output to the console.
 *
 * Since XmlDocument is the entry point of the whole XML document, we can get and modify related information of the document through it. For example, we can get and modify the XML version and the standalone property of the document through `xmlDoc.xmlVersion` and `xmlDoc.xmlStandalone` respectively. We can also create new processing instruction nodes with the `xmlDoc.createProcessingInstruction()` method.
 *
 * The XmlDocument object is a very powerful type that provides great convenience for parsing XML files.
 *
 */
declare class Class_XmlDocument extends Class_XmlNode {
    /**
     * @description Constructs an XmlDocument object
     *      @param type the type of the document object, default "text/xml"; to handle html, you need to specify "text/html"
     *
     */
    constructor(type?: string);

    /**
     * @description Forms the document by parsing an XML/HTML string; multilingual text is not supported
     *      @param source the XML/HTML text to parse, depending on the type when the document was created
     *      @param options the parsing limits, same as xml.parse, default { maxElementDepth: 1000, maxNodeCount: 1000000 }
     *
     */
    load(source: string, options?: FIBJS.GeneralObject): void;

    /**
     * @description Forms the document by parsing binary XML/HTML data and converts automatically according to the language
     *      @param source the XML/HTML text to parse, depending on the type when the document was created
     *      @param options the parsing limits, same as xml.parse, default { maxElementDepth: 1000, maxNodeCount: 1000000 }
     *
     */
    load(source: Class_Buffer, options?: FIBJS.GeneralObject): void;

    /**
     * @description Returns the encoding used for the document (at parse time)
     *
     */
    readonly inputEncoding: string;

    /**
     * @description Sets or returns whether the document is standalone
     *
     */
    xmlStandalone: boolean;

    /**
     * @description Sets or returns the XML version of the document
     *
     */
    xmlVersion: string;

    /**
     * @description Returns the Document Type Declaration related to the document
     *
     *     For an XML document without a DTD, returns null. This property provides direct access to the XmlDocumentType object (a child node of XmlDocument).
     *
     */
    readonly doctype: Class_XmlDocumentType;

    /**
     * @description Returns the root node of the document
     *
     */
    readonly documentElement: Class_XmlElement;

    /**
     * @description Returns the head node of an HTML document; only valid in html mode
     *
     */
    readonly head: Class_XmlElement;

    /**
     * @description Returns the content of the title node of an HTML document; only valid in html mode
     *
     */
    readonly title: string;

    /**
     * @description Returns the body node of an HTML document; only valid in html mode
     *
     */
    readonly body: Class_XmlElement;

    /**
     * @description Returns a node list of all elements with the specified name
     *
     *      This method returns an XmlNodeList object (which can be treated as a read-only array) containing all XmlElement nodes in the document with the specified tag name, stored in the order in which they appear in the source document. The XmlNodeList object is "live", that is, if elements with the specified tag name are added to or removed from the document, its content is updated automatically as necessary.
     *      @param tagName the tag name to retrieve. The value "*" matches all tags
     *      @return an XmlNodeList collection of XmlElement nodes with the specified tag in the document tree. The order of the returned element nodes is the order in which they appear in the source document.
     *
     */
    getElementsByTagName(tagName: string): Class_XmlNodeList;

    /**
     * @description Returns a node list of all elements with the specified namespace and name
     *
     *      This method is similar to the getElementsByTagName() method, except that it retrieves elements by namespace and name.
     *      @param namespaceURI the namespace URI to retrieve. The value "*" matches all tags
     *      @param localName the tag name to retrieve. The value "*" matches all tags
     *      @return an XmlNodeList collection of XmlElement nodes with the specified tag in the document tree. The order of the returned element nodes is the order in which they appear in the source document.
     *
     */
    getElementsByTagNameNS(namespaceURI: string, localName: string): Class_XmlNodeList;

    /**
     * @description Returns the element with the specified id attribute
     *
     *      This method traverses the descendant nodes of the document and returns an XmlElement node object representing the first document element with the specified id attribute.
     *      @param id the id to retrieve
     *      @return the XmlElement node with the specified id attribute in the node tree
     *
     */
    getElementById(id: string): Class_XmlElement;

    /**
     * @description Returns a node list of all elements with the specified class name
     *
     *      This method returns an XmlNodeList object (which can be treated as a read-only array) containing all XmlElement nodes in the document with the specified class name, stored in the order in which they appear in the source document. The XmlNodeList object is "live", that is, if elements with the specified tag name are added to or removed from the document, its content is updated automatically as necessary.
     *      @param className the class name to retrieve
     *      @return an XmlNodeList collection of XmlElement nodes with the specified class name in the document tree. The order of the returned element nodes is the order in which they appear in the source document.
     *
     */
    getElementsByClassName(className: string): Class_XmlNodeList;

    /**
     * @description Creates an element node
     *      @param tagName the specified name of the element node
     *      @return returns the newly created XmlElement node with the specified tag name
     *
     */
    createElement(tagName: string): Class_XmlElement;

    /**
     * @description Creates an element node with the specified namespace
     *      @param namespaceURI the namespace URI of the element node
     *      @param qualifiedName the qualified name of the element node
     *      @return returns the newly created XmlElement node with the specified tag name
     *
     */
    createElementNS(namespaceURI: string, qualifiedName: string): Class_XmlElement;

    /**
     * @description Creates a text node
     *      @param data the text of this node
     *      @return returns the newly created XmlText node representing the specified data string
     *
     */
    createTextNode(data: string): Class_XmlText;

    /**
     * @description Creates a comment node
     *      @param data the comment text of this node
     *      @return returns the newly created XmlComment node whose comment text is the specified data
     *
     */
    createComment(data: string): Class_XmlComment;

    /**
     * @description Creates an XmlCDATASection node
     *      @param data the CDATA data of this node
     *      @return returns the newly created XmlCDATASection node whose content is the specified data
     *
     */
    createCDATASection(data: string): Class_XmlCDATASection;

    /**
     * @description Creates an XmlProcessingInstruction node
     *      @param target the target of the processing instruction
     *      @param data the content text of the processing instruction
     *      @return the newly created ProcessingInstruction node
     *
     */
    createProcessingInstruction(target: string, data: string): Class_XmlProcessingInstruction;

    /**
     * @description Creates an empty XmlDocumentFragment node
     *
     *      DocumentFragment is a lightweight document object that can contain multiple child nodes. When a DocumentFragment is inserted into a document, what is inserted is not the DocumentFragment itself but all of its child nodes.
     *      @return the newly created XmlDocumentFragment node
     *
     */
    createDocumentFragment(): Class_XmlDocumentFragment;

    /**
     * @description Imports a node from another document into the current document
     *
     *      This method creates a copy of the source node and can insert it into the current document. The source node remains unchanged. If you need to move a node from another document to the current document instead of copying it, use the adoptNode method.
     *      @param importedNode the node to import
     *      @param deep if true, imports the whole subtree of the node recursively; if false, imports only the node itself
     *      @return returns the new node imported into the current document
     *
     */
    importNode(importedNode: Class_XmlNode, deep?: boolean): Class_XmlNode;

    /**
     * @description Adopts a node from another document into the current document
     *
     *      This method moves a node from another document to the current document. The node is removed from the original document and its ownerDocument property is changed to the current document. Unlike importNode, adoptNode does not create a copy.
     *      @param adoptedNode the node to adopt
     *      @return returns the adopted node
     *
     */
    adoptNode(adoptedNode: Class_XmlNode): Class_XmlNode;

    /**
     * @description Returns an XmlNodeList of elements matching the specified CSS selector
     *
     *      This method returns an XmlNodeList object (which can be treated as a read-only array) containing all XmlElement nodes in the document that match the specified CSS selector, stored in the order in which they appear in the source document. The XmlNodeList object is "live", that is, if elements matching the specified selector are added to or removed from the document, its content is updated automatically as necessary.
     *      @param selectors the CSS selector
     *      @return the XmlElement node matching the specified CSS selector
     *
     */
    querySelector(selectors: string): Class_XmlElement;

    /**
     * @description Returns an XmlNodeList of all elements matching the specified CSS selector
     *
     *      This method returns an XmlNodeList object (which can be treated as a read-only array) containing all XmlElement nodes in the document that match the specified CSS selector, stored in the order in which they appear in the source document. The XmlNodeList object is "live", that is, if elements matching the specified selector are added to or removed from the document, its content is updated automatically as necessary.
     *      @param selectors the CSS selector
     *      @return an XmlNodeList collection of XmlElement nodes matching the specified CSS selector. The order of the returned element nodes is the order in which they appear in the source document.
     *
     */
    querySelectorAll(selectors: string): Class_XmlNodeList;

}

