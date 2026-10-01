/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description The XmlCharacterData interface provides the common functionality of XmlText and XmlComment nodes
 *
 * XmlCharacterData is the superinterface of XmlText and XmlComment nodes. Documents never contain XmlCharacterData nodes; they contain only XmlText nodes and XmlComment nodes. But since these two kinds of nodes have similar functionality, the functions are defined here so that XmlText and XmlComment can inherit them.
 *
 */
declare class Class_XmlCharacterData extends Class_XmlNode {
    /**
     * @description The text contained in this node
     *
     */
    data: string;

    /**
     * @description The number of characters contained in this node
     *
     */
    readonly length: number;

    /**
     * @description Extracts a substring from the node
     *      @param offset the position of the first character to return
     *      @param count the number of characters in the substring to return
     *      @return returns the extracted string
     *
     */
    substringData(offset: number, count: number): string;

    /**
     * @description Appends a string to the node
     *      @param arg the string to append to the node
     *
     */
    appendData(arg: string): void;

    /**
     * @description Inserts a string into the node
     *      @param offset the character position at which to insert the string into the node
     *      @param arg the string to insert
     *
     */
    insertData(offset: number, arg: string): void;

    /**
     * @description Deletes text from the node
     *      @param offset the position of the first character to delete
     *      @param count the number of characters to delete
     *
     */
    deleteData(offset: number, count: number): void;

    /**
     * @description Replaces the characters of the node with the given string
     *      @param offset the character position in the node to replace
     *      @param count the number of characters to replace
     *      @param arg the string to insert
     *
     */
    replaceData(offset: number, count: number, arg: string): void;

}

