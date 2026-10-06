/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlAttr.d.ts" />
/**
 * @description The XmlNamedNodeMap object represents the attributes of an element as an
 *  index- and name-addressable collection
 *
 *  fibjs keeps the attributes of every element in a map object and returns the same live
 *  map from `element.attributes`. The map is not a node list: its entries are XmlAttr
 *  objects, and an element without attributes yields an empty map, not null.
 *
 *  Concepts:
 *
 *  - **Live collection**: the object is the attribute storage itself, not a copy.
 *    setAttribute, removeAttribute, setAttributeNode and value writes through an XmlAttr
 *    are immediately visible in length and item(); the object identity does not change.
 *    Indexes follow document order (the order in which the attributes were set or
 *    parsed), even though the standard describes a NamedNodeMap as unordered.
 *  - **Subset of the standard**: only length, item(), indexed access and getNamedItem()
 *    are exposed. setNamedItem, removeNamedItem, getNamedItemNS and the other
 *    NamedNodeMap methods of the standard are not available in fibjs; use the element
 *    methods setAttribute, setAttributeNS, removeAttribute, getAttributeNode and
 *    getAttributeNodeNS instead. The map is not iterable (no Symbol.iterator, forEach or
 *    entries).
 *  - **Name lookup**: getNamedItem matches the qualified name as written (`n:v`,
 *    `xmlns:p`). In XML mode the comparison is exact and case-sensitive; in HTML mode a
 *    namespace-less attribute is matched case-insensitively (the parser lower-cases names
 *    anyway). A missing name returns null.
 *  - **Namespace declarations** are ordinary entries of the map, so `xmlns` and `xmlns:p`
 *    can be found with getNamedItem and removed with removeAttribute.
 *
 *  Obtained from:
 *  - `element.attributes` — the live attribute map of that element (the same object on
 *    every access); there is no constructor and no global export.
 *
 *  Example 1 — enumerate the attributes of an element:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<book id="b1" lang="en" pages="320"/>');
 *  const book = doc.documentElement;
 *
 *  console.log(book.attributes.length); // 3
 *  for (let i = 0; i < book.attributes.length; i++) {
 *      const attr = book.attributes.item(i);
 *      console.log(attr.name + '=' + attr.value); // id=b1, lang=en, pages=320
 *  }
 *  console.log(book.attributes[1].name);                     // lang
 *  console.log(book.attributes.getNamedItem('pages').value); // 320
 *  console.log(book.attributes.getNamedItem('missing'));     // null
 *  ```
 *
 *  Example 2 — the map is live and follows the element methods:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<a x="1"/>');
 *  const element = doc.documentElement;
 *  const map = element.attributes;
 *
 *  console.log(map.length); // 1
 *
 *  element.setAttribute('y', '2');
 *  console.log(map.length);                 // 2
 *  console.log(map.item(1).name);           // y
 *  console.log(map === element.attributes); // true
 *
 *  element.removeAttribute('x');
 *  console.log(map.length);       // 1
 *  console.log(map.item(0).name); // y
 *  ```
 *
 *  Example 3 — getNamedItem and qualified names:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<svg xmlns:xlink="http://www.w3.org/1999/xlink" '
 *      + 'xlink:href="#a" viewBox="0 0 8 8"/>');
 *  const map = doc.documentElement.attributes;
 *
 *  console.log(map.getNamedItem('xlink:href').value); // #a
 *  console.log(map.getNamedItem('viewBox').value);    // 0 0 8 8
 *  console.log(map.getNamedItem('viewbox'));          // null, XML is case-sensitive
 *  console.log(map.getNamedItem('xmlns:xlink').namespaceURI);
 *  // the xmlns namespace URI, http://www.w3.org/2000/xmlns/
 *  ```
 *
 */
declare class Class_XmlNamedNodeMap extends Class_object {
    /**
     * @description Returns the number of attributes in the attribute list
     *
     *      The number is read live from the element, so it follows setAttribute,
     *      removeAttribute and attribute removals performed through the map entries.
     *
     */
    readonly length: number;

    /**
     * @description Returns the attribute at the given index in the attribute list
     *
     *      Attributes are in document order; a negative index or an index greater than or
     *      equal to length returns null, while indexed access returns undefined. A numeric
     *      string is accepted.
     *
     *      @param index the index to query
     *      @return the attribute at the given index
     *
     */
    item(index: number): Class_XmlAttr;

    /**
     * @description Data can be accessed directly with an index
     *
     *      Equivalent to item(index) except that an out-of-range index yields undefined
     *      rather than null.
     *
     */
    [index: number]: Class_XmlAttr;

    /**
     * @description Queries the attribute with the given name
     *
     *      The name is the qualified name (`v`, `n:v`, `xmlns:p`). The lookup is exact in XML
     *      mode and case-insensitive in HTML mode for namespace-less attributes; a missing
     *      attribute returns null. Use getAttributeNodeNS on the element for a lookup by
     *      namespace URI plus local name, which the map does not provide.
     *
     *      Example — look up a namespaced attribute:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<r xmlns:n="urn:k" n:v="1" v="2"/>');
     *      const map = doc.documentElement.attributes;
     *
     *      console.log(map.getNamedItem('n:v').value); // 1
     *      console.log(map.getNamedItem('v').value);   // 2
     *      console.log(map.getNamedItem('urn:k:v'));   // null
     *      ```
     *
     *      @param name the name to query
     *      @return returns the queried attribute
     *
     */
    getNamedItem(name: string): Class_XmlAttr;

}

