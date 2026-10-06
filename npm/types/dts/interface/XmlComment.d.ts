/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlCharacterData.d.ts" />
/**
 * @description The XmlComment object represents the content of a comment node in a document
 *
 *  An XmlComment node represents a comment in an XML document. It is a leaf node
 *  (nodeType COMMENT_NODE (8), nodeName `#comment`) that extends XmlCharacterData, so the
 *  comment text is available through data and nodeValue and can be edited with
 *  substringData, appendData, insertData, deleteData and replaceData. The interface
 *  declares no members of its own: everything it offers comes from XmlCharacterData and
 *  XmlNode.
 *
 *  Concepts:
 *
 *  - **Content**: the text between `<!--` and `-->` is stored verbatim - entities are not
 *    expanded in a comment and the parser accepts `--` inside the text. fibjs also does
 *    not validate data passed to createComment, so a comment can be serialized into
 *    markup that is no longer well formed. textContent is an empty string on this class
 *    (the standard returns the comment text); read data or nodeValue instead.
 *  - **Position**: a comment may appear in a document, inside an element or inside a
 *    document fragment; comments are children and count in childNodes, and normalize
 *    never removes them.
 *  - **Creation**: document.createComment(data) creates a detached comment owned by the
 *    document, and the parser creates one for every `<!--...-->` in the source. fibjs
 *    does not export an XmlComment constructor and has no splitText on comments.
 *
 *  Obtained from:
 *  - `xml.parse('<r><!--c--></r>')` — the comment child of the parsed node;
 *  - `document.createComment(data)`;
 *  - `node.cloneNode()` of another comment.
 *
 *  Example 1 — create a comment and edit it through the inherited members:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = new xml.Document();
 *  const comment = doc.createComment('draft');
 *
 *  doc.appendChild(doc.createElement('r')).appendChild(comment);
 *  console.log(String(doc));    // <r><!--draft--></r>
 *  console.log(comment.data);   // draft
 *  console.log(comment.length); // 5
 *
 *  comment.appendData('!');
 *  comment.replaceData(0, 5, 'final');
 *  console.log(String(doc));    // <r><!--final!--></r>
 *  ```
 *
 *  Example 2 — read and remove a parsed comment:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<r><!--keep--><a/></r>');
 *  const comment = doc.documentElement.firstChild;
 *
 *  console.log(comment.nodeType);    // 8
 *  console.log(comment.nodeName);    // #comment
 *  console.log(comment.nodeValue);   // keep
 *  console.log(comment.textContent); // empty string (fibjs difference)
 *
 *  const removed = doc.documentElement.removeChild(comment);
 *  console.log(removed.data);        // keep
 *  console.log(String(doc));         // <r><a/></r>
 *  ```
 *
 */
declare class Class_XmlComment extends Class_XmlCharacterData {
}

