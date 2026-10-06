/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description The XmlProcessingInstruction object represents a processing instruction
 *  (`<?target data?>`) in a document
 *
 *  A processing instruction carries the application target and an opaque string of
 *  instruction data; the parser stores both as written and never interprets the data.
 *  The XML declaration is not a processing instruction: the parser consumes
 *  `<?xml ...?>` as document metadata (see xmlVersion and the other declaration members
 *  of XmlDocument) and the declaration never appears in childNodes.
 *
 *  Concepts:
 *
 *  - **Content**: target is the name between `<?` and the first whitespace or `?>`; data
 *    is everything after the first whitespace up to `?>`, as a raw string (entities are
 *    not expanded and the value is not parsed as markup). data is read-write and
 *    nodeValue is an alias of it. The standard's textContent returns the data for this
 *    node type, but fibjs returns an empty string and ignores assignments, so use data.
 *  - **Creation and validation**: document.createProcessingInstruction(target, data)
 *    creates a detached node that must be inserted to be serialized. fibjs validates
 *    neither argument: an empty target and a data string containing `?>` are accepted
 *    and can produce malformed markup, where the standard throws InvalidCharacterError.
 *    The reserved target `xml` is not rejected either.
 *  - **Serialization**: String(pi) produces `<?target data?>` with a single space
 *    between target and data, and the class is not exported as a global.
 *
 *  Obtained from:
 *  - `xml.parse('<?xml-stylesheet ...?><r/>')` — a processing instruction is a child of
 *    the document or of an element;
 *  - `document.createProcessingInstruction(target, data)`;
 *  - `node.cloneNode()` of another processing instruction.
 *
 *  Example 1 — read a stylesheet instruction from a parsed document:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<?xml-stylesheet type="text/xsl" href="style.xsl"?><r/>');
 *  const pi = doc.firstChild;
 *
 *  console.log(pi.nodeType); // 7
 *  console.log(pi.target);   // xml-stylesheet
 *  console.log(pi.data);     // type="text/xsl" href="style.xsl"
 *  console.log(pi.nodeName); // xml-stylesheet
 *  console.log(String(pi));  // <?xml-stylesheet type="text/xsl" href="style.xsl"?>
 *  ```
 *
 *  Example 2 — create, insert and update an instruction:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<root/>');
 *  const pi = doc.createProcessingInstruction('php', 'echo "hi";');
 *
 *  doc.documentElement.appendChild(pi);
 *  console.log(String(doc)); // <root><?php echo "hi";?></root>
 *
 *  pi.data = 'echo "bye";';
 *  console.log(String(doc)); // <root><?php echo "bye";?></root>
 *  ```
 *
 *  Example 3 — the XML declaration is not a node:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<?xml version="1.0" encoding="utf-8"?><r/>');
 *
 *  console.log(doc.xmlVersion);          // 1.0
 *  console.log(doc.childNodes.length);   // 1, the root element only
 *  console.log(doc.firstChild.nodeName); // r
 *  console.log(String(doc)); // <?xml version="1.0" encoding="utf-8"?><r/>
 *  ```
 *
 */
declare class Class_XmlProcessingInstruction extends Class_XmlNode {
    /**
     * @description Returns the target of this processing instruction
     *
     *      The name between `<?` and the first whitespace or `?>` (for example `php` or
     *      `xml-stylesheet`); nodeName returns the same string. Read-only.
     *
     */
    readonly target: string;

    /**
     * @description Sets or returns the content of this processing instruction
     *
     *      The data is the raw string after the target up to `?>`; it is stored verbatim and
     *      only string values are accepted (a non-string throws 20005). nodeValue is an alias
     *      of this property and textContent is always an empty string.
     *
     *      Example — update the data and watch the serialization:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<?xml-stylesheet href="a.css"?><r/>');
     *
     *      doc.firstChild.data = 'href="b.css"';
     *      console.log(String(doc.firstChild)); // <?xml-stylesheet href="b.css"?>
     *      ```
     *
     */
    data: string;

}

