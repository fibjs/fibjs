/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlCharacterData.d.ts" />
/**
 * @description The XmlText object represents a run of plain text in a document
 *
 *  An XmlText node represents a run of plain text in an XML document. Because plain text
 *  appears in XML elements and attributes, XmlText nodes usually appear as children of
 *  XmlElement nodes and XmlAttr nodes. XmlText is one of the abstract character-data
 *  classes: fibjs does not export it (typeof XmlText is undefined) and it cannot be
 *  constructed; instances come from the document factory methods and from parsing. The
 *  XmlCDATASection interface extends XmlText, so a CDATA node is an XmlText as well.
 *
 *  The XmlText node inherits the XmlCharacterData interface: the text is read and written
 *  with data, length, substringData, appendData, insertData, deleteData and replaceData,
 *  and nodeValue from XmlNode is an alias of data. textContent also reads and writes the
 *  same storage on this class. Use splitText to break a run of text in two, and
 *  XmlNode.normalize to merge adjacent text nodes and drop empty ones (whitespace-only
 *  text is preserved).
 *
 *  XmlText nodes have no child nodes and fibjs has no wholeText member.
 *
 *  Obtained from:
 *  - text parsed inside an element: `xml.parse('<a>t</a>').documentElement.firstChild`;
 *  - `document.createTextNode(data)` — a detached node owned by that document;
 *  - element methods that accept strings (innerHTML, insertAdjacentHTML, append with a
 *    string and so on) create text nodes internally;
 *  - `textNode.splitText(offset)` — the newly created second half;
 *  - `node.cloneNode()` of another text node.
 *
 *  Example 1 — create a text node and edit its data:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = new xml.Document();
 *  const root = doc.createElement('p');
 *  const text = doc.createTextNode('hello');
 *
 *  root.appendChild(text);
 *  doc.appendChild(root);
 *
 *  text.appendData(', world');
 *  console.log(text.data);   // hello, world
 *  console.log(text.length); // 12
 *  console.log(String(doc)); // <p>hello, world</p>
 *  ```
 *
 *  Example 2 — split a text run and let normalize merge it back:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<p>one two</p>');
 *  const text = doc.documentElement.firstChild;
 *  const second = text.splitText(3);
 *
 *  console.log(text.data);   // one
 *  console.log(second.data); //  two
 *  console.log(String(doc)); // <p>one two</p>
 *
 *  doc.documentElement.normalize();
 *  console.log(doc.documentElement.childNodes.length); // 1
 *  console.log(doc.documentElement.firstChild.data);   // one two
 *  ```
 *
 *  Example 3 — textContent and nodeValue share the data storage:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<a>before</a>');
 *  const text = doc.documentElement.firstChild;
 *
 *  text.nodeValue = 'after';
 *  console.log(text.data);      // after
 *  text.textContent = 'last';
 *  console.log(text.nodeValue); // last
 *  console.log(String(doc));    // <a>last</a>
 *  ```
 *
 */
declare class Class_XmlText extends Class_XmlCharacterData {
    /**
     * @description Splits the text node into two nodes at the given offset
     *
     *      The original node keeps the text before offset (excluding the character at that
     *      position); a new node holding the text from offset to the end is returned and,
     *      when the original node has a parent, inserted immediately after it. offset ===
     *      length creates an empty second node; an offset greater than length throws a
     *      RangeError (20012). The XmlCDATASection interface inherits this method and returns
     *      a CDATA node instead of a text node.
     *
     *      Example — split a text node and inspect the two halves:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<r/>');
     *      const text = doc.createTextNode('abcdef');
     *      doc.documentElement.appendChild(text);
     *      const tail = text.splitText(3);
     *
     *      console.log(text.data);                             // abc
     *      console.log(tail.data);                             // def
     *      console.log(tail.previousSibling === text);         // true
     *      console.log(doc.documentElement.childNodes.length); // 2
     *      console.log(String(doc));                           // <r>abcdef</r>
     *      ```
     *
     *      @param offset specifies where to split the text node. The starting value begins at 0
     *      @return the Text node split from the current node
     *
     */
    splitText(offset: number): Class_XmlText;

}

