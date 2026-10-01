/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/// <reference path="../interface/DOMTokenList.d.ts" />
/// <reference path="../interface/DOMStringMap.d.ts" />
/// <reference path="../interface/CSSStyleDeclaration.d.ts" />
/// <reference path="../interface/XmlDocumentFragment.d.ts" />
/// <reference path="../interface/XmlNamedNodeMap.d.ts" />
/// <reference path="../interface/XmlAttr.d.ts" />
/// <reference path="../interface/XmlNodeList.d.ts" />
/**
 * @description The XmlElement object represents an element in an XML document
 *
 */
declare class Class_XmlElement extends Class_XmlNode {
    /**
     * @description Queries the URI of the element's namespace. If the selected node has no namespace, the property returns NULL
     *
     */
    readonly namespaceURI: string;

    /**
     * @description Queries and sets the namespace prefix of the element. If the selected node has no namespace, the property returns NULL
     *
     */
    prefix: string;

    /**
     * @description Queries the local name of the element. If the selected node has no namespace, this property is equivalent to nodeName
     *
     */
    readonly localName: string;

    /**
     * @description Returns the tag name of the element
     *
     */
    readonly tagName: string;

    /**
     * ! Queries and sets the id attribute of the element
     *
     */
    id: string;

    /**
     * ! Queries and sets the src attribute of the element; only valid in html mode. When read, returns the regular attribute value other than style; when written, is synchronized to the attribute; an empty string removes the attribute
     *
     */
    src: string;

    /**
     * ! Queries and sets the alt attribute of the element; only valid in html mode
     *
     */
    alt: string;

    /**
     * ! Queries and sets the href attribute of the element; only valid in html mode
     *
     */
    href: string;

    /**
     * ! Queries and sets the title attribute of the element; only valid in html mode
     *
     */
    title: string;

    /**
     * ! Queries and sets the value attribute of the element; only valid in html mode
     *
     */
    value: string;

    /**
     * ! Queries and sets the name attribute of the element; only valid in html mode
     *
     */
    name: string;

    /**
     * ! Queries and sets the type attribute of the element; only valid in html mode
     *
     */
    type: string;

    /**
     * ! Queries and sets the rel attribute of the element; only valid in html mode
     *
     */
    rel: string;

    /**
     * ! Queries and sets the target attribute of the element; only valid in html mode
     *
     */
    target: string;

    /**
     * ! Queries and sets the placeholder attribute of the element; only valid in html mode
     *
     */
    placeholder: string;

    /**
     * ! Queries and sets the HTML text of the descendants of the selected element; only valid in html mode. When queried, returns the HTML encoding of all child nodes inside the element node; when set, deletes all child nodes and replaces them with the specified HTML after decoding.
     *
     */
    innerHTML: string;

    /**
     * ! Queries the HTML text of the selected element and its descendants; only valid in html mode. When queried, returns the HTML encoding of the element and all child nodes inside the node.
     *
     */
    outerHTML: string;

    /**
     * ! Queries and sets the class attribute of the element; only valid in html mode
     *
     */
    className: string;

    /**
     * @description Returns a DOMTokenList object containing the token list of the element's class attribute; only valid in html mode
     *
     */
    readonly classList: Class_DOMTokenList;

    /**
     * @description Returns a DOMStringMap object containing the key-value pairs of all data-* attributes of the element; only valid in html mode. Attribute names are converted from the data-xxx-yyy format to the xxxYyy camelCase format, and reads and writes on the object are synchronized to the element's data-* attributes in real time
     *
     */
    readonly dataset: Class_DOMStringMap;

    /**
     * @description Returns the CSSStyleDeclaration object corresponding to the element's style attribute; only valid in html mode
     *
     *     Through this object the inline style of the element can be read or modified; the modification result is synchronized to the element's style attribute and reflected in the outerHTML/innerHTML serialization. CSS property names use the camelCase format, for example style.maxWidth corresponds to max-width.
     *
     */
    readonly style: Class_CSSStyleDeclaration;

    /**
     * @description Returns the content of a template element; only valid for template elements; returns a DocumentFragment containing its child nodes
     *
     */
    readonly content: Class_XmlDocumentFragment;

    /**
     * @description Returns the NamedNodeMap containing the attributes of the selected node. If the selected node is not an element, the property returns NULL.
     *
     */
    readonly attributes: Class_XmlNamedNodeMap;

    /**
     * @description Queries whether the current element has any attributes
     *      @return returns true if the current element has attributes, otherwise returns false
     *
     */
    hasAttributes(): boolean;

    /**
     * @description Queries the value of an attribute by name
     *      @param name the name of the attribute to query
     *      @return returns the value of the attribute
     *
     */
    getAttribute(name: string): string;

    /**
     * @description Gets the attribute value by namespace URI and name
     *      @param namespaceURI the namespace URI to query
     *      @param localName the name of the attribute to query
     *      @return returns the value of the attribute
     *
     */
    getAttributeNS(namespaceURI: string, localName: string): string;

    /**
     * @description Returns the attribute node with the specified name
     *
     *      This method returns an XmlAttr object representing the attribute with the specified name of the current element. If there is no attribute with the specified name, returns NULL.
     *      @param name the name of the attribute to query
     *      @return returns the XmlAttr object with the specified name, or NULL if there is no attribute with the specified name
     *
     */
    getAttributeNode(name: string): Class_XmlAttr;

    /**
     * @description Returns the attribute node with the specified namespace URI and name
     *
     *      This method returns an XmlAttr object representing the attribute with the specified namespace URI and name of the current element. If there is no attribute with the specified name, returns NULL.
     *      @param namespaceURI the namespace URI to query
     *      @param localName the name of the attribute to query
     *      @return returns the XmlAttr object with the specified name, or NULL if there is no attribute with the specified name
     *
     */
    getAttributeNodeNS(namespaceURI: string, localName: string): Class_XmlAttr;

    /**
     * @description Creates or changes an attribute
     *
     *      This method sets the specified attribute to the specified value. If there is no attribute with the specified name, this method creates a new attribute
     *      @param name the name of the attribute to set
     *      @param value the value of the attribute to set
     *
     */
    setAttribute(name: string, value: string): void;

    /**
     * @description Creates or changes an attribute with a namespace
     *
     *      This method is similar to the setAttribute method, except that the attribute to create or set is specified jointly by the namespace URI and a qualified name (composed of the namespace prefix, a colon and the local name in the namespace). Besides changing the value of an attribute, this method can also change the namespace prefix of the attribute
     *      @param namespaceURI the namespace URI to set
     *      @param qualifiedName the name of the attribute to set
     *      @param value the value of the attribute to set
     *
     */
    setAttributeNS(namespaceURI: string, qualifiedName: string, value: string): void;

    /**
     * @description Sets the specified attribute object
     *
     *      This method sets the specified XmlAttr object as an attribute of the current element. If the current element already has an attribute with the same name, this method replaces it
     *      @param attr the XmlAttr object to set
     *      @return returns the replaced XmlAttr object, or NULL if nothing was replaced
     *
     */
    setAttributeNode(attr: Class_XmlAttr): Class_XmlAttr;

    /**
     * @description Removes the specified attribute by name
     *      @param name the name of the attribute to remove
     *
     */
    removeAttribute(name: string): void;

    /**
     * @description Removes the specified attribute by namespace and name
     *      @param namespaceURI the namespace URI to remove
     *      @param localName the name of the attribute to remove
     *
     */
    removeAttributeNS(namespaceURI: string, localName: string): void;

    /**
     * @description Removes the specified attribute node
     *
     *      This method removes the specified XmlAttr object from the attribute list of the current element. If the current element does not have the specified attribute, this method has no effect
     *      @param attr the XmlAttr object to remove
     *      @return returns the removed XmlAttr object, or NULL if nothing was removed
     *
     */
    removeAttributeNode(attr: Class_XmlAttr): Class_XmlAttr;

    /**
     * @description Queries whether the current node has an attribute with the specified name
     *      @param name the name of the attribute to query
     *      @return returns true if the current element node has the specified attribute, otherwise returns false
     *
     */
    hasAttribute(name: string): boolean;

    /**
     * @description Queries whether the current node has an attribute with the specified namespace and name
     *      @param namespaceURI the namespace URI to query
     *      @param localName the name of the attribute to query
     *      @return returns true if the current element node has the specified attribute, otherwise returns false
     *
     */
    hasAttributeNS(namespaceURI: string, localName: string): boolean;

    /**
     * @description Returns an XmlNodeList of all elements with the specified name
     *
     *      This method traverses the descendant nodes of the specified element and returns an XmlNodeList object of XmlElement nodes representing all document elements with the specified tag name. The order of the elements in the returned array is the order in which they appear in the document source code.
     *
     *      The XmlDocument interface also defines a getElementsByTagName method; it is similar to this method but traverses the whole document instead of the descendant nodes of one element.
     *      @param tagName the tag name to retrieve. The value "*" matches all tags
     *      @return an XmlNodeList collection of XmlElement nodes with the specified tag in the node tree. The order of the returned element nodes is the order in which they appear in the source document.
     *
     */
    getElementsByTagName(tagName: string): Class_XmlNodeList;

    /**
     * @description Returns an XmlNodeList of all elements with the specified namespace and name
     *
     *      This method is similar to the getElementsByTagName method, except that the tag name of the elements to get is specified as a combination of the namespace URI and the local name defined in the namespace.
     *      @param namespaceURI the namespace URI to query
     *      @param localName the tag name to retrieve. The value "*" matches all tags
     *      @return an XmlNodeList collection of XmlElement nodes with the specified tag in the node tree. The order of the returned element nodes is the order in which they appear in the source document.
     *
     */
    getElementsByTagNameNS(namespaceURI: string, localName: string): Class_XmlNodeList;

    /**
     * @description Returns the element with the specified id attribute
     *
     *      This method traverses the descendant nodes of the specified element and returns an XmlElement node object representing the first document element with the specified id attribute.
     *
     *      The XmlDocument interface also defines a getElementsByTagName method; it is similar to this method but traverses the whole document instead of the descendant nodes of one element.
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

    /**
     * @description Queries whether the current element matches the specified CSS selector
     *      @param selectors the CSS selector
     *      @return returns true if the current element matches the specified selector, otherwise returns false
     *
     */
    matches(selectors: string): boolean;

    /**
     * @description Searches upward for an ancestor element matching the specified CSS selector
     *      @param selectors the CSS selector
     *      @return returns the nearest matching ancestor element, or null if there is no match
     *
     */
    closest(selectors: string): Class_XmlElement;

    /**
     * @description Adds one or more nodes to the end of the current element's child nodes
     *
     *      This method adds the specified nodes to the end of the current element's child node list. String parameters are automatically converted to text nodes.
     *      @param nodes one or more nodes to add; can be node objects or strings
     *
     */
    append(...nodes: any[]): void;

    /**
     * @description Adds one or more nodes to the beginning of the current element's child nodes
     *
     *      This method adds the specified nodes to the beginning of the current element's child node list. String parameters are automatically converted to text nodes.
     *      @param nodes one or more nodes to add; can be node objects or strings
     *
     */
    prepend(...nodes: any[]): void;

    /**
     * @description Replaces all child nodes of the current element
     *
     *      This method replaces all child nodes of the current element with the specified nodes. String parameters are automatically converted to text nodes. If no parameter is passed, all child nodes are cleared.
     *      @param nodes one or more nodes to set; can be node objects or strings
     *
     */
    replaceChildren(...nodes: any[]): void;

    /**
     * @description Inserts an element node at the specified position
     *
     *      The position parameter can be one of the following values:
     *      - 'beforebegin': inserts before the current element
     *      - 'afterbegin': inserts before the first child node of the current element
     *      - 'beforeend': inserts after the last child node of the current element
     *      - 'afterend': inserts after the current element
     *
     *      @param position the insertion position
     *      @param element the element node to insert
     *      @return returns the inserted element, or null if the insertion fails
     *
     */
    insertAdjacentElement(position: string, element: Class_XmlElement): Class_XmlElement;

    /**
     * @description Inserts HTML text at the specified position
     *
     *      The position parameter can be one of the following values:
     *      - 'beforebegin': inserts before the current element
     *      - 'afterbegin': inserts before the first child node of the current element
     *      - 'beforeend': inserts after the last child node of the current element
     *      - 'afterend': inserts after the current element
     *
     *      @param position the insertion position
     *      @param html the HTML text to insert
     *
     */
    insertAdjacentHTML(position: string, html: string): void;

    /**
     * @description Inserts a text node at the specified position
     *
     *      The position parameter can be one of the following values:
     *      - 'beforebegin': inserts before the current element
     *      - 'afterbegin': inserts before the first child node of the current element
     *      - 'beforeend': inserts after the last child node of the current element
     *      - 'afterend': inserts after the current element
     *
     *      @param position the insertion position
     *      @param text the text to insert
     *
     */
    insertAdjacentText(position: string, text: string): void;

    /**
     * @description Toggles a boolean attribute on the element
     *
     *      If the attribute exists, it is removed; if it does not exist, it is added.
     *      @param name the name of the attribute to toggle
     *      @return returns true if the attribute exists after the operation, otherwise returns false
     *
     */
    toggleAttribute(name: string): boolean;

    /**
     * @description Toggles a boolean attribute on the element
     *
     *      Adds or removes the attribute forcibly according to the force parameter.
     *      @param name the name of the attribute to toggle
     *      @param force if true, adds the attribute forcibly; if false, removes the attribute forcibly
     *      @return returns true if the attribute exists after the operation, otherwise returns false
     *
     */
    toggleAttribute(name: string, force: boolean): boolean;

}

