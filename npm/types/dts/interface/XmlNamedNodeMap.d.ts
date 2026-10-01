/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlAttr.d.ts" />
/**
 * @description The XmlNamedNodeMap object represents an unordered list of attributes
 *
 */
declare class Class_XmlNamedNodeMap extends Class_object {
    /**
     * @description Returns the number of attributes in the attribute list
     *
     */
    readonly length: number;

    /**
     * @description Returns the attribute at the given index in the attribute list
     *      @param index the index to query
     *      @return the attribute at the given index
     *
     */
    item(index: number): Class_XmlAttr;

    /**
     * @description Queries the attribute with the given name
     *      @param name the name to query
     *      @return returns the queried attribute
     *
     */
    getNamedItem(name: string): Class_XmlAttr;

}

