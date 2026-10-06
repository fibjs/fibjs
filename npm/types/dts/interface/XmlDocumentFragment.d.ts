/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description The XmlDocumentFragment object represents a lightweight document object that
 *  holds a group of nodes outside the document tree
 *
 *  An XmlDocumentFragment node does not belong to the document tree: its parentNode is
 *  always null and it has no siblings. It can hold child nodes (elements, text, comments
 *  and so on) like an element, and when the fragment itself is inserted into a parent, it
 *  is not the fragment that is inserted but all of its children - the parent receives them
 *  in order and the fragment is left empty. This makes the fragment a useful staging area
 *  for building a subtree once and inserting it with a single call.
 *
 *  Concepts:
 *
 *  - **Semantics**: the fragment is a node (nodeType DOCUMENT_FRAGMENT_NODE (11),
 *    nodeName `#document-fragment`, nodeValue null) but not a document: it has no
 *    documentElement, no doctype and no query methods (querySelector, querySelectorAll
 *    and getElementsByTagName are not available on it). childNodes is the live child
 *    list, textContent reads the concatenated text of the children and writing it drops
 *    the children and installs a single text node, and String(fragment) serializes the
 *    children. The insertion helpers are the generic XmlNode ones (appendChild,
 *    insertBefore) - the XmlElement append, prepend and replaceChildren methods do not
 *    exist here. before, after and replaceWith exist but do nothing, since a fragment
 *    never has a parent.
 *  - **Insertion rules**: inserting a fragment into a parent moves all of its children
 *    and empties it; insertBefore requires a real reference node (passing null throws
 *    20005, unlike a document or element where null means "append"). Cloning a fragment
 *    clones its subtree.
 *  - **HTML templates**: in HTML mode the `content` property of a `<template>` element is
 *    an XmlDocumentFragment holding the template children; the same fragment is returned
 *    on every access.
 *
 *  Obtained from:
 *  - `document.createDocumentFragment()`;
 *  - `templateElement.content` — HTML mode only, null for other elements;
 *  - `node.cloneNode()` of another fragment.
 *
 *  Example 1 — build a fragment and insert it in one operation:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<ul/>');
 *  const ul = doc.documentElement;
 *  const fragment = doc.createDocumentFragment();
 *
 *  for (const name of ['tea', 'coffee', 'milk']) {
 *      const item = doc.createElement('li');
 *      item.textContent = name;
 *      fragment.appendChild(item);
 *  }
 *
 *  console.log(fragment.childNodes.length); // 3
 *  console.log(fragment.parentNode);        // null
 *
 *  ul.appendChild(fragment);
 *  console.log(fragment.childNodes.length); // 0, the children moved to ul
 *  console.log(String(doc)); // <ul><li>tea</li><li>coffee</li><li>milk</li></ul>
 *  ```
 *
 *  Example 2 — template content is a document fragment:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<html><body><template><p>Hi</p></template></body></html>',
 *      'text/html');
 *  const template = doc.querySelector('template');
 *  const content = template.content;
 *
 *  console.log(content.nodeName);             // #document-fragment
 *  console.log(content.parentNode);           // null
 *  console.log(content.childNodes.length);    // 1
 *  console.log(String(content));              // <p>Hi</p>
 *  console.log(content === template.content); // true
 *  ```
 *
 *  Example 3 — textContent and cloning:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = new xml.Document();
 *  const fragment = doc.createDocumentFragment();
 *
 *  fragment.appendChild(doc.createTextNode('one'));
 *  fragment.appendChild(doc.createTextNode('two'));
 *  console.log(fragment.textContent); // onetwo
 *
 *  fragment.textContent = 'replaced';       // drops the children, adds a text node
 *  console.log(fragment.childNodes.length); // 1
 *  console.log(String(fragment));           // replaced
 *
 *  const copy = fragment.cloneNode();
 *  console.log(copy.textContent); // replaced
 *  console.log(copy.parentNode);  // null
 *  ```
 *
 */
declare class Class_XmlDocumentFragment extends Class_XmlNode {
}

