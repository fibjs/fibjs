/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlDocument.d.ts" />
/// <reference path="../interface/XmlElement.d.ts" />
/// <reference path="../interface/XmlNodeList.d.ts" />
/**
 * @description The XmlNode object is the basic data type of the whole DOM
 *
 */
declare class Class_XmlNode extends Class_object {
    /**
     * @description Returns the node type of the node
     *
     *      Different objects return different values of nodeType:
     *      - XmlElement: ELEMENT_NODE(1)
     *      - XmlAttr: ATTRIBUTE_NODE(2)
     *      - XmlText: TEXT_NODE(3)
     *      - XmlCDATASection: CDATA_SECTION_NODE(4)
     *      - XmlProcessingInstruction: PROCESSING_INSTRUCTION_NODE(7)
     *      - XmlComment: COMMENT_NODE(8)
     *      - XmlDocument: DOCUMENT_NODE(9)
     *      - XmlDocumentType: DOCUMENT_TYPE_NODE(10)
     *
     */
    readonly nodeType: number;

    /**
     * @description Returns the name of the node, according to its type
     *
     *      Different objects return different values of nodeName:
     *      - XmlElement: element name
     *      - XmlAttr: attribute name
     *      - XmlText: \#text
     *      - XmlCDATASection: \#cdata-section
     *      - XmlProcessingInstruction: returns the specified target
     *      - XmlComment: \#comment
     *      - XmlDocument: \#document
     *      - XmlDocumentType: doctype name
     *
     */
    readonly nodeName: string;

    /**
     * @description Returns the name of the node, according to its type
     *
     *      Different objects return different values of nodeName:
     *      - XmlElement: null
     *      - XmlAttr: the value of the attribute
     *      - XmlText: the content of the node
     *      - XmlCDATASection: the content of the node
     *      - XmlProcessingInstruction: returns the specified content data
     *      - XmlComment: the comment text
     *      - XmlDocument: null
     *      - XmlDocumentType: null
     *
     */
    nodeValue: string;

    /**
     * @description Returns the root element of the node (an XmlDocument object)
     *
     */
    readonly ownerDocument: Class_XmlDocument;

    /**
     * @description Returns the parent node of a node
     *
     */
    readonly parentNode: Class_XmlNode;

    /**
     * @description Returns the parent element of a node; returns null if the parent node is not an element node
     *
     */
    readonly parentElement: Class_XmlElement;

    /**
     * @description Queries whether child nodes exist
     *      @return returns true if any child node exists, otherwise returns false
     *
     */
    hasChildNodes(): boolean;

    /**
     * @description Returns the node list of the child nodes of the specified node
     *
     */
    readonly childNodes: Class_XmlNodeList;

    /**
     * @description Returns the node list of the child element nodes of the specified node
     *
     */
    readonly children: Class_XmlNodeList;

    /**
     * @description Returns the first child node of the node
     *
     */
    readonly firstChild: Class_XmlNode;

    /**
     * @description Returns the last child node of the node
     *
     */
    readonly lastChild: Class_XmlNode;

    /**
     * @description Returns the node immediately preceding a node (at the same tree level); if there is no such node, the property returns null
     *
     */
    readonly previousSibling: Class_XmlNode;

    /**
     * @description Returns the node immediately following a node (at the same tree level); if there is no such node, the property returns null
     *
     */
    readonly nextSibling: Class_XmlNode;

    /**
     * @description Returns the first child element node of the node
     *
     */
    readonly firstElementChild: Class_XmlNode;

    /**
     * @description Returns the last child element node of the node
     *
     */
    readonly lastElementChild: Class_XmlNode;

    /**
     * @description Returns the element node immediately preceding a node (at the same tree level); if there is no such node, the property returns null
     *
     */
    readonly previousElementSibling: Class_XmlNode;

    /**
     * @description Returns the element node immediately following a node (at the same tree level); if there is no such node, the property returns null
     *
     */
    readonly nextElementSibling: Class_XmlNode;

    /**
     * ! Queries and sets the text of the selected element. When queried, returns the values of all text nodes inside the element node; when set, deletes all child nodes and replaces them with a single text node.
     *
     */
    textContent: string;

    /**
     * @description Merges adjacent Text nodes and removes empty Text nodes
     *
     *      This method traverses all descendant nodes of the current node and normalizes the document by removing empty Text nodes and merging all adjacent Text nodes. This method is useful for simplifying the document tree structure after node insertion or deletion operations.
     *
     */
    normalize(): void;

    /**
     * @description Creates an exact copy of the specified node
     *
     *      This method copies and returns a copy of the node on which it is called. If the parameter passed to it is true, it also recursively copies all descendant nodes of the current node; otherwise, it copies only the current node. The returned node does not belong to the document tree and its parentNode property is null. When an Element node is copied, all of its attributes are copied as well.
     *      @param deep whether to make a deep copy; when true, the cloned node clones all child nodes of the original node
     *      @return returns the copied node
     *
     */
    cloneNode(deep?: boolean): Class_XmlNode;

    /**
     * @description Returns the prefix matching the specified namespace URI on the current node
     *      @param namespaceURI the namespace URI to match
     *      @return returns the matched prefix, or null if no match is found
     *
     */
    lookupPrefix(namespaceURI: string): string;

    /**
     * @description Returns the namespace URI matching the specified prefix on the current node
     *      @param prefix the prefix to match
     *      @return returns the matched namespace URI, or null if no match is found
     *
     */
    lookupNamespaceURI(prefix: string): string;

    /**
     * @description Inserts a new child node before an existing child node
     *
     *      If newChild already exists in the document tree, it is removed from the document tree and then reinserted at its new position. A node from one document (or a node created by one document) cannot be inserted into another document. That is, the ownerDocument property of newChild must be the same as the ownerDocument property of the current node.
     *      @param newChild the new node to insert
     *      @param refChild inserts the new node before this node
     *      @return returns the new child node
     *
     */
    insertBefore(newChild: Class_XmlNode, refChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Inserts a new child node after an existing child node
     *
     *      If newChild already exists in the document tree, it is removed from the document tree and then reinserted at its new position. A node from one document (or a node created by one document) cannot be inserted into another document. That is, the ownerDocument property of newChild must be the same as the ownerDocument property of the current node.
     *      @param newChild the new node to insert
     *      @param refChild inserts the new node after this node
     *      @return returns the new child node
     *
     */
    insertAfter(newChild: Class_XmlNode, refChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Adds a new child node to the end of the node's child node list
     *
     *      If newChild already exists in the document tree, it is removed from the document tree and then reinserted at its new position. A node from one document (or a node created by one document) cannot be inserted into another document. That is, the ownerDocument property of newChild must be the same as the ownerDocument property of the current node.
     *      @param newChild the node to add
     *      @return returns this new child node
     *
     */
    appendChild(newChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Replaces a child node with another one
     *
     *      If newChild already exists in the document tree, it is removed from the document tree and then reinserted at its new position. A node from one document (or a node created by one document) cannot be inserted into another document. That is, the ownerDocument property of newChild must be the same as the ownerDocument property of the current node.
     *      @param newChild the new node
     *      @param oldChild the node to be replaced
     *      @return if the replacement succeeds, this method returns the replaced node; if it fails, returns null
     *
     */
    replaceChild(newChild: Class_XmlNode, oldChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Removes a node from the child node list
     *      @param oldChild the node to remove
     *      @return if the removal succeeds, this method returns the removed node; if it fails, returns null
     *
     */
    removeChild(oldChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Removes itself from the current node
     *
     *      This method removes the current node from its parent node and returns the current node. Notes: if the current node has no parent node, this method has no effect.
     *      @return returns the removed node
     *
     */
    remove(): Class_XmlNode;

    /**
     * @description Replaces the current node with one or more nodes
     *
     *      This method removes the current node from its parent node and inserts the specified new node at the original position. If the current node has no parent node, this method has no effect.
     *      @param nodes one or more nodes that replace the current node
     *
     */
    replaceWith(...nodes: any[]): void;

    /**
     * @description Inserts one or more nodes before the current node
     *
     *      This method inserts the specified nodes before the current node, under the same parent node as the current node. If the current node has no parent node, this method has no effect.
     *      @param nodes one or more nodes to insert; can be node objects or strings
     *
     */
    before(...nodes: any[]): void;

    /**
     * @description Inserts one or more nodes after the current node
     *
     *      This method inserts the specified nodes after the current node, under the same parent node as the current node. If the current node has no parent node, this method has no effect.
     *      @param nodes one or more nodes to insert; can be node objects or strings
     *
     */
    after(...nodes: any[]): void;

    /**
     * @description Checks whether the current node contains the specified node
     *      @param node the node to check
     *      @return returns true if the current node contains the specified node, otherwise returns false
     *
     */
    contains(node: Class_XmlNode): boolean;

    /**
     * @description Returns the root node of the current node
     *      @return returns the root node
     *
     */
    getRootNode(): Class_XmlNode;

    /**
     * @description Returns whether the current node is connected to the document
     *
     */
    readonly isConnected: boolean;

    /**
     * @description Compares the position relationship of two nodes in the document
     *
     *      Returns a bitmask representing the position relationship of the two nodes:
     *      - DOCUMENT_POSITION_DISCONNECTED (1): the two nodes are not in the same document
     *      - DOCUMENT_POSITION_PRECEDING (2): the parameter node precedes the current node
     *      - DOCUMENT_POSITION_FOLLOWING (4): the parameter node follows the current node
     *      - DOCUMENT_POSITION_CONTAINS (8): the parameter node contains the current node
     *      - DOCUMENT_POSITION_CONTAINED_BY (16): the current node contains the parameter node
     *      - DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC (32): the position relationship is implementation-specific
     *
     *      @param other the node to compare
     *      @return returns the position relationship represented by the bitmask
     *
     */
    compareDocumentPosition(other: Class_XmlNode): number;

    /**
     * @description Checks whether two nodes are structurally equal
     *
     *      Two nodes are structurally equal when they have the same type, the same attribute values, the same child node structure and so on.
     *      @param other the node to compare
     *      @return returns true if the two nodes are structurally equal, otherwise returns false
     *
     */
    isEqualNode(other: Class_XmlNode): boolean;

    /**
     * @description Checks whether two nodes are the same node
     *
     *      Same as the === operator: checks whether two references point to the same object.
     *      @param other the node to compare
     *      @return returns true if they are the same node, otherwise returns false
     *
     */
    isSameNode(other: Class_XmlNode): boolean;

}

