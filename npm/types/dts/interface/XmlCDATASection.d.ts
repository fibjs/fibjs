/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlText.d.ts" />
/**
 * @description The XmlCDATASection object represents a CDATA section in a document
 *
 *  The XmlCDATASection interface is a subinterface of XmlText and declares no properties
 *  or methods of its own: the text content is available through data (inherited from
 *  XmlCharacterData), nodeValue (XmlNode) and the inherited character-data members,
 *  including splitText. nodeType is CDATA_SECTION_NODE (4) and nodeName is
 *  `#cdata-section`.
 *
 *  Concepts:
 *
 *  - **What CDATA means**: a CDATA section contains text that the parser does not
 *    interpret - markup inside it is not parsed as tags and entities are not expanded.
 *    Its only delimiter is `]]>`, which ends the section, and sections cannot be nested.
 *    This is the way to embed a markup fragment without escaping every `<`, `&` and `"`.
 *  - **Not merged by normalize**: although a CDATA node can usually be treated as a text
 *    node, XmlNode.normalize does not merge adjacent CDATA parts (it merges only XmlText
 *    nodes), so `a<![CDATA[b]]>c` stays three children after normalize.
 *  - **Differences from the standard**: textContent is an always-empty string on this
 *    class - and, as a consequence, CDATA text does not contribute to the textContent of
 *    its ancestors either; read data or nodeValue instead. fibjs does not reject a `]]>`
 *    sequence in the data passed to createCDATASection or written through data, so the
 *    caller must keep it out to avoid producing malformed markup (the standard throws
 *    InvalidCharacterError). A splitText on a CDATA node returns another
 *    XmlCDATASection, not an XmlText.
 *
 *  Obtained from:
 *  - `xml.parse('<r><![CDATA[d]]></r>')` — the CDATA child of the parsed node;
 *  - `document.createCDATASection(data)` — a detached node owned by that document;
 *  - `cdataNode.splitText(offset)` — the created second half is a CDATA node;
 *  - `node.cloneNode()` of another CDATA node.
 *
 *  Example 1 — read a CDATA section without markup interpretation:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<r><![CDATA[<b> & raw]]></r>');
 *  const cdata = doc.documentElement.firstChild;
 *
 *  console.log(cdata.nodeType); // 4
 *  console.log(cdata.nodeName); // #cdata-section
 *  console.log(cdata.data);     // <b> & raw
 *  console.log(String(doc));    // <r><![CDATA[<b> & raw]]></r>
 *  ```
 *
 *  Example 2 — create a CDATA section and embed markup text safely:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = new xml.Document();
 *  const root = doc.createElement('script');
 *  const cdata = doc.createCDATASection('if (a < b && c > d) run();');
 *
 *  root.appendChild(cdata);
 *  doc.appendChild(root);
 *
 *  console.log(String(doc));
 *  // <script><![CDATA[if (a < b && c > d) run();]]></script>
 *  console.log(root.textContent); // empty, CDATA text is not part of textContent
 *  ```
 *
 *  Example 3 — splitText keeps the CDATA type and normalize leaves it alone:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<r><![CDATA[abcdef]]></r>');
 *  const cdata = doc.documentElement.firstChild;
 *  const tail = cdata.splitText(3);
 *
 *  console.log(cdata.constructor.name); // XmlCDATASection
 *  console.log(tail.constructor.name);  // XmlCDATASection
 *  console.log(String(doc)); // <r><![CDATA[abc]]><![CDATA[def]]></r>
 *
 *  doc.documentElement.normalize();
 *  console.log(doc.documentElement.childNodes.length); // 2, CDATA is not merged
 *  ```
 *
 */
declare class Class_XmlCDATASection extends Class_XmlText {
}

