/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlCharacterData.d.ts" />
/**
 * @description The XmlText object represents the text content of an element or attribute
 *
 * An XmlText node represents a run of plain text in an XML document. Because plain text appears in XML elements and attributes, XmlText nodes usually appear as children of XmlElement nodes and XmlAttr nodes.
 *
 * The XmlText node inherits the XmlCharacterData interface; the text content of an XmlText node can be accessed through the data property inherited from the XmlCharacterData interface or the nadevalue property inherited from the XmlNode interface.
 *
 * XmlText nodes can be manipulated with the methods inherited from XmlCharacterData or with the splitText() method defined by the XmlText interface itself. Use createTextNode of XmlDocument to create a new XmlText node.
 *
 * XmlText nodes have no child nodes.
 *
 * For how to remove empty XmlText nodes from a document subtree and merge adjacent XmlText nodes, see the XmlNode.normalize method.
 *
 */
declare class Class_XmlText extends Class_XmlCharacterData {
    /**
     * @description Splits the text node into two nodes at the given offset
     *
     *      This method splits the XmlText node into two nodes at the given offset. The original XmlText node is modified to contain the text before the position given by offset (excluding the text at that position). A new XmlText node is created to hold all characters from the offset position (including the character at that position) to the end of the original text. The new XmlText node is the return value of this method. In addition, if the original XmlText node has a parentNode, the new XmlText node is inserted into this parent, immediately after the original node.
     *
     *      The XmlCDATASection interface inherits the XmlText interface, and XmlCDATASection nodes can also use this method; the only difference is that the newly created node is an XmlCDATASection node rather than an XmlText node.
     *      @param offset specifies where to split the text node. The starting value begins at 0
     *      @return the Text node split from the current node
     *
     */
    splitText(offset: number): Class_XmlText;

}

