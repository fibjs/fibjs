/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description The XmlNodeList object represents an ordered list of nodes
 *
 */
declare class Class_XmlNodeList extends Class_object {
    /**
     * @description Returns the number of nodes in the node list
     *
     */
    readonly length: number;

    /**
     * @description Returns the node at the given index in the node list
     *      @param index the index to query
     *      @return the node at the given index
     *
     */
    item(index: number): Class_XmlNode;

    /**
     * @description Data can be accessed directly with an index
     *
     */
    [index: number]: Class_XmlNode;

    "[Symbol.iterator]"(): Iterator<any>;

    /**
     * @description Calls the given callback function once for each node in the list
     *      @param callback the callback function called for each node, receiving three parameters: the current node, the index and the node list itself
     *
     */
    forEach(callback: (...args: any[])=>any): void;

    /**
     * @description Returns an iterator for traversing the index of each node in the node list
     *      @return returns the index iterator
     *
     */
    keys(): Iterator<any>;

    /**
     * @description Returns an iterator for traversing the value of each node in the node list
     *      @return returns the value iterator
     *
     */
    values(): Iterator<any>;

    /**
     * @description Returns an iterator for traversing the [index, value] pairs of each node in the node list
     *      @return returns the key-value pair iterator
     *
     */
    entries(): Iterator<any>;

}

