/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/XmlDocument.d.ts" />
/// <reference path="../interface/XmlElement.d.ts" />
/// <reference path="../interface/XmlNodeList.d.ts" />
/**
 * @description The abstract base interface of every node in the fibjs XML/HTML DOM: it defines
 *  the tree protocol - navigation, mutation, cloning and comparison - shared by all node
 *  classes
 *
 *  XmlNode is not constructible. fibjs does not export an XmlNode class (nor a global of that
 *  name), so a node can never be created with `new XmlNode`; a document produces every node:
 *  `xml.parse()` and `new xml.Document()` create an XmlDocument, and createElement,
 *  createTextNode, createComment, createCDATASection, createProcessingInstruction and
 *  createDocumentFragment, documentElement, the document query methods and the tree navigation
 *  members return concrete nodes. Code that only walks or reshapes a tree can work against
 *  this interface; switch to XmlElement (or another leaf interface) when the node-specific API
 *  is needed.
 *
 *  The concrete node classes and their nodeType values (the constants live in the xml
 *  module):
 *  - XmlElement: ELEMENT_NODE (1);
 *  - XmlText: TEXT_NODE (3);
 *  - XmlCDATASection: CDATA_SECTION_NODE (4);
 *  - XmlProcessingInstruction: PROCESSING_INSTRUCTION_NODE (7);
 *  - XmlComment: COMMENT_NODE (8);
 *  - XmlDocument: DOCUMENT_NODE (9);
 *  - XmlDocumentType: DOCUMENT_TYPE_NODE (10);
 *  - XmlDocumentFragment: DOCUMENT_FRAGMENT_NODE (11).
 *
 *  ATTRIBUTE_NODE (2) is special: the XmlAttr interface declares nodeName and nodeValue for
 *  compatibility, but an attribute is not an XmlNode - it has no nodeType, parentNode,
 *  ownerDocument, isConnected or any other member of this interface, and attributes are never
 *  part of the child list.
 *
 *  Concepts:
 *
 *  - **The tree protocol**: a node joins a tree only when it is inserted (appendChild,
 *    insertBefore, insertAfter, replaceChild or the variadic before/after/replaceWith
 *    helpers). Inserting a node that already has a parent moves it; inserting a node into its
 *    own descendant is rejected; a DocumentFragment is not inserted itself - its children are
 *    spliced into the parent and the fragment is left empty.
 *  - **Ownership**: every node records the document that created it, and inserting a node
 *    that belongs to another document adopts it automatically (its ownerDocument changes and
 *    it leaves the old tree). importNode copies a foreign subtree into the document;
 *    adoptNode moves it without copying. See the XmlDocument interface.
 *  - **Child rules**: a document may hold one element, one doctype, processing instructions,
 *    comments and a document fragment, but no text; an element may hold elements, text,
 *    CDATA, entity references, processing instructions, comments and fragments; text,
 *    comment, CDATA and processing-instruction nodes are leaves.
 *  - **Views**: childNodes is the live structural list of the node (the same XmlNodeList
 *    object on every access, updated by later mutations); children is the cached element-only
 *    view; query results and children are snapshots taken at call time.
 *  - **nodeName and nodeValue**: nodeName is fixed per node type; nodeValue is the data of
 *    the leaf nodes and null for element, document, doctype and fragment. `textContent` is
 *    the readable and writable text view of the node.
 *  - **Comparison**: compareDocumentPosition returns a bitmask of position bits (0 for the
 *    node itself, 4 FOLLOWING, 2 PRECEDING, 8 CONTAINS, 16 CONTAINED_BY, 1 DISCONNECTED; a
 *    disconnected node yields 3 = DISCONNECTED | PRECEDING, and the
 *    implementation-specific bit is never set). isEqualNode compares type, name, value,
 *    attributes and the whole child shape; isSameNode is the `===` operator.
 *  - **Cloning and normalization**: cloneNode defaults to a deep copy (unlike the standard,
 *    which defaults to shallow) and copies element attributes; the copy has no parent and
 *    keeps the source ownerDocument. normalize merges adjacent text nodes of every child list
 *    in the subtree and drops empty text nodes, while whitespace-only text is preserved.
 *  - **Namespaces**: lookupNamespaceURI and lookupPrefix walk the element ancestors (only
 *    elements carry xmlns declarations; on other node types the walk starts at the parent)
 *    and also resolve the built-in xml and xmlns prefixes; a document falls back to its root
 *    element. createElementNS stores namespaceURI/prefix/localName but adds no declaration to
 *    the tree, so the lookup finds it only after the subtree is parsed with an explicit xmlns
 *    (serialization adds the missing declaration).
 *
 *  Obtained from:
 *  - `xml.parse(source[, type][, options])` - an XmlDocument, itself a node;
 *  - `new xml.Document([type])` or the global `new XMLDocument([type])` - an empty XML
 *    document, or an html/head/body skeleton in HTML mode;
 *  - `document.createElement(...)`, `createTextNode`, `createComment`, `createCDATASection`,
 *    `createProcessingInstruction`, `createDocumentFragment` - detached nodes owned by that
 *    document until inserted;
 *  - `document.documentElement`, `document.doctype`, the document query methods,
 *    `node.childNodes`/`children`, `parentNode`/`firstChild`/`nextSibling` and the other
 *    navigation members, `cloneNode`, `importNode` and `adoptNode`.
 *
 *  Example 1 - build a tree on a concrete document:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = new xml.Document();
 *  const root = doc.createElement('root');
 *  const first = doc.createElement('first');
 *  const second = doc.createElement('second');
 *  root.appendChild(first);
 *  root.appendChild(second);
 *  doc.appendChild(root);
 *
 *  console.log(first.parentNode === root);        // true
 *  console.log(first.nextElementSibling.nodeName); // second
 *  console.log(root.childNodes.length);           // 2
 *  ```
 *
 *  Example 2 - navigate and mutate a parsed tree:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<list><item>a</item><item>b</item></list>');
 *  const list = doc.documentElement;
 *  const item = list.firstChild;
 *
 *  item.textContent = 'A';
 *  list.insertBefore(doc.createElement('item'), item).textContent = 'start';
 *  const moved = list.removeChild(list.lastChild);
 *
 *  console.log(moved.textContent);   // b
 *  console.log(list.textContent);    // startA
 *  console.log(xml.serialize(list)); // <list><item>start</item><item>A</item></list>
 *  ```
 *
 *  Example 3 - clone a subtree and compare nodes:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<r><a>1</a></r>');
 *  const r = doc.documentElement;
 *  const copy = r.cloneNode();
 *
 *  console.log(copy.parentNode);                 // null
 *  console.log(copy.isEqualNode(r));             // true
 *  console.log(r.compareDocumentPosition(copy)); // 3 (DISCONNECTED | PRECEDING)
 *  console.log(r.contains(copy));                // false
 *  ```
 *
 */
declare class Class_XmlNode extends Class_object {
    /**
     * @description Returns the type of the node, as one of the node type constants of the xml
     *      module
     *
     *      The value is fixed when the node is created and cannot be changed. See the class
     *      comment for the value of every concrete node class; the deprecated ENTITY_NODE,
     *      ENTITY_REFERENCE_NODE and NOTATION_NODE constants are declared for completeness but no
     *      fibjs node reports them. The XmlAttr interface is not an XmlNode and has no nodeType,
     *      even though its conceptual type is ATTRIBUTE_NODE (2).
     *
     */
    readonly nodeType: number;

    /**
     * @description Returns the name of the node, which is fixed per node type
     *
     *      The value per concrete class:
     *      - XmlElement: the tag name (tagName; upper-cased for elements parsed in HTML mode);
     *      - XmlText: `\#text`;
     *      - XmlCDATASection: `\#cdata-section`;
     *      - XmlProcessingInstruction: the target;
     *      - XmlComment: `\#comment`;
     *      - XmlDocument: `\#document`;
     *      - XmlDocumentType: the doctype name;
     *      - XmlDocumentFragment: `\#document-fragment`.
     *      The XmlAttr interface declares its own nodeName (the attribute name).
     *
     */
    readonly nodeName: string;

    /**
     * @description Reads and writes the data carried by a leaf node
     *
     *      The value per node type:
     *      - XmlText, XmlCDATASection and XmlComment: the character data;
     *      - XmlProcessingInstruction: the instruction data (not the target);
     *      - XmlElement, XmlDocument, XmlDocumentType and XmlDocumentFragment: null, and
     *        assigning to them is silently ignored.
     *
     *      The XmlAttr interface declares its own nodeValue (the attribute value); the
     *      XmlCharacterData interface adds data, length and the substring/append/insert/delete/
     *      replace members shared by text, CDATA and comment nodes.
     *
     */
    nodeValue: string;

    /**
     * @description Returns the document that owns the node
     *
     *      The owner is the document that created the node, the importing document for importNode,
     *      the target document for an insertion across documents, and the new document after
     *      adoptNode; it survives detaching the node, so a detached node still reports its owner.
     *      A document node returns itself here. Not in the standard: the DOM defines
     *      Document.ownerDocument as null.
     *
     */
    readonly ownerDocument: Class_XmlDocument;

    /**
     * @description Returns the parent node, or null when the node is the root of its tree or is
     *      detached
     *
     *      A document node always reports null; the parent of documentElement is the document
     *      itself.
     *
     */
    readonly parentNode: Class_XmlNode;

    /**
     * @description Returns the parent when it is an element, otherwise null
     *
     *      A document, document fragment or detached node as parent yields null, so the member
     *      answers "is my parent an element" rather than "what is my parent"; use parentNode for
     *      the parent whatever its type.
     *
     */
    readonly parentElement: Class_XmlElement;

    /**
     * @description Queries whether the node has at least one child node
     *
     *      Equivalent to `childNodes.length > 0`; text, comment, CDATA, processing-instruction
     *      and element children all count.
     *
     *      @return returns true when the child node list is not empty, otherwise false
     *
     */
    hasChildNodes(): boolean;

    /**
     * @description Returns the live list of the child nodes
     *
     *      The same XmlNodeList object is returned on every access, and it is the structural list
     *      of the node itself, so later insertions and removals are visible immediately
     *      (childNodes.length grows and shrinks). Reading it does not take a snapshot; iterate
     *      with the XmlNodeList members (length, item, the index and iterator members) or
     *      serialize it to markup with toString().
     *
     */
    readonly childNodes: Class_XmlNodeList;

    /**
     * @description Returns the element-only view of the child nodes
     *
     *      Text, CDATA, comment and processing-instruction children are filtered out, so
     *      children.length is the number of child elements. The view is cached and rebuilt after
     *      a mutation; for the live structural list of every child node use childNodes.
     *
     */
    readonly children: Class_XmlNodeList;

    /**
     * @description Returns the first child node, or null when the node has no children
     *
     *      Shorthand for `childNodes[0]`; the child can be of any node type.
     *
     */
    readonly firstChild: Class_XmlNode;

    /**
     * @description Returns the last child node, or null when the node has no children
     *
     *      Shorthand for the last entry of childNodes; the child can be of any node type.
     *
     */
    readonly lastChild: Class_XmlNode;

    /**
     * @description Returns the sibling immediately before the node at the same tree level, or
     *      null when it is the first child or the node is detached
     *
     */
    readonly previousSibling: Class_XmlNode;

    /**
     * @description Returns the sibling immediately after the node at the same tree level, or
     *      null when it is the last child or the node is detached
     *
     */
    readonly nextSibling: Class_XmlNode;

    /**
     * @description Returns the first child element, skipping non-element children, or null when
     *      there is none
     *
     */
    readonly firstElementChild: Class_XmlNode;

    /**
     * @description Returns the last child element, skipping non-element children, or null when
     *      there is none
     *
     */
    readonly lastElementChild: Class_XmlNode;

    /**
     * @description Returns the nearest preceding sibling element, or null when there is none
     *
     *      Non-element siblings between the node and the result are skipped; the walk happens in
     *      the parent's child list.
     *
     */
    readonly previousElementSibling: Class_XmlNode;

    /**
     * @description Returns the nearest following sibling element, or null when there is none
     *
     *      Non-element siblings between the node and the result are skipped; the walk happens in
     *      the parent's child list.
     *
     */
    readonly nextElementSibling: Class_XmlNode;

    /**
     * @description Reads and writes the text content of the node
     *
     *      Reading returns the concatenation of the data of all descendant text nodes in document
     *      order (CDATA sections included); text, CDATA, comment and processing-instruction nodes
     *      return their own data, and document, doctype and fragment nodes return an empty string.
     *      Writing to an element removes all of its children and appends a single new text node
     *      with the given value (an empty value creates an empty text node); writing to a document
     *      is silently ignored. The member never joins adjacent text nodes - call normalize() when
     *      a canonical shape is needed.
     *
     */
    textContent: string;

    /**
     * @description Merges adjacent text nodes and removes empty text nodes in the whole subtree
     *
     *      Every child list of the subtree is normalized, the root's included: runs of directly
     *      adjacent text nodes are merged into the first one (through appendData) and text nodes
     *      whose value is empty are removed. A comment, element, CDATA section or processing
     *      instruction between two text nodes keeps them apart, and whitespace-only text nodes are
     *      preserved because they are not empty. The walk is iterative, so very deep documents do
     *      not overflow the native stack.
     *
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<r>a<!--c-->b</r>');
     *      const root = doc.documentElement;
     *      root.appendChild(doc.createTextNode('X'));
     *      root.appendChild(doc.createTextNode(''));
     *
     *      root.normalize();
     *      console.log(root.childNodes.length); // 3
     *      console.log(root.textContent);       // abX
     *      ```
     *
     */
    normalize(): void;

    /**
     * @description Creates a detached copy of the node
     *
     *      The copy keeps the ownerDocument of the source and has no parent. An element always
     *      carries a copy of its attributes; the deep parameter defaults to true, so
     *      `cloneNode()` copies the whole subtree, while the standard defaults to a shallow copy
     *      (pass `cloneNode(false)` for that). Cloning a document also copies its declaration
     *      (version, encoding, standalone); cloning a document or a fragment produces a detached
     *      tree.
     *
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<r x="1"><a>t</a></r>');
     *      const root = doc.documentElement;
     *      const shallow = root.cloneNode(false);
     *      const deep = root.cloneNode();
     *
     *      console.log(shallow.getAttribute('x')); // 1
     *      console.log(shallow.childNodes.length); // 0
     *      console.log(deep.childNodes.length);    // 1
     *      console.log(deep.parentNode);           // null
     *      ```
     *
     *      @param deep whether to copy the subtree of the node as well; the default is a deep copy
     *      @return returns the copied node
     *
     */
    cloneNode(deep?: boolean): Class_XmlNode;

    /**
     * @description Returns the namespace prefix bound to a namespace URI
     *
     *      The search starts at the node itself for elements and at the parent for the other node
     *      types (only elements store xmlns declarations); it walks up the ancestors and then
     *      falls back to the built-in xml and xmlns bindings. A document checks the built-in
     *      bindings and then its root element. Returns null when nothing matches. A node created
     *      with createElementNS stores the namespace fields without adding an xmlns declaration,
     *      so such a detached element reports null until its subtree is parsed with an explicit
     *      declaration (serialization adds the missing declaration).
     *
     *      @param namespaceURI the namespace URI to match
     *      @return returns the matching prefix, or null when no binding is found
     *
     */
    lookupPrefix(namespaceURI: string): string;

    /**
     * @description Returns the namespace URI bound to a prefix
     *
     *      The walk is the same as lookupPrefix: from the element itself (or from the parent for
     *      the other node types) up through the ancestors, with the built-in xml and xmlns
     *      bindings resolved first. A document checks the built-in bindings and then its root
     *      element. Returns null when nothing matches.
     *
     *      @param prefix the prefix to match
     *      @return returns the matching namespace URI, or null when no binding is found
     *
     */
    lookupNamespaceURI(prefix: string): string;

    /**
     * @description Inserts a node before an existing child of this node
     *
     *      refChild must already be a child of this node, otherwise an Error (20024) is thrown.
     *      If newChild already has a parent it is moved here (it leaves the old tree first), a
     *      DocumentFragment is spliced as its children, and a node owned by another document is
     *      adopted (its ownerDocument changes). Inserting a node into its own descendant is
     *      rejected. A non-node argument, including null, throws a TypeError (20005) - the
     *      argument is not coerced.
     *
     *      @param newChild the node to insert
     *      @param refChild the existing child the new node is inserted before
     *      @return returns the inserted node
     *
     */
    insertBefore(newChild: Class_XmlNode, refChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Inserts a node after an existing child of this node (fibjs extension)
     *
     *      This member has no counterpart in the standard DOM (use insertBefore with the next
     *      sibling instead). It behaves like insertBefore, but the node is placed after refChild,
     *      which must already be a child of this node (Error 20024 otherwise). Moving, fragment
     *      splicing and cross-document adoption follow the same rules.
     *
     *      @param newChild the node to insert
     *      @param refChild the existing child the new node is inserted after
     *      @return returns the inserted node
     *
     */
    insertAfter(newChild: Class_XmlNode, refChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Appends a node as the last child of this node
     *
     *      If newChild already has a parent it is moved here, a DocumentFragment is spliced as its
     *      children (the fragment is emptied), and a node owned by another document is adopted.
     *      The child rules of the parent still apply: a document accepts one element, one doctype,
     *      processing instructions, comments and a fragment, but no text; an element accepts
     *      elements, text, CDATA, entity references, processing instructions, comments and
     *      fragments. A cycle (inserting an ancestor) or a disallowed node type throws an Error
     *      (20024); a non-node argument throws a TypeError (20005).
     *
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = new xml.Document();
     *      const list = doc.createElement('list');
     *      doc.appendChild(list);
     *
     *      const added = list.appendChild(doc.createElement('item'));
     *      console.log(added.nodeName);            // item
     *      console.log(added.parentNode === list); // true
     *      ```
     *
     *      @param newChild the node to append
     *      @return returns the appended node (for a DocumentFragment, the fragment itself)
     *
     */
    appendChild(newChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Replaces an existing child with another node
     *
     *      oldChild must be a child of this node (Error 20024 otherwise); it is detached and
     *      returned. newChild is inserted at its position under the same rules as insertBefore,
     *      including fragment splicing, moving and cross-document adoption. When newChild equals
     *      oldChild the call is a no-op that returns the node.
     *
     *      @param newChild the replacement node
     *      @param oldChild the child to be replaced
     *      @return returns the replaced (old) child node
     *
     */
    replaceChild(newChild: Class_XmlNode, oldChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Removes a child node from this node
     *
     *      oldChild must be a child of this node, otherwise an Error (20024) is thrown; the child
     *      is detached with its subtree intact and returned. Removing the document element from a
     *      document sets documentElement to null, after which another element may be appended. A
     *      non-node argument throws a TypeError (20005).
     *
     *      @param oldChild the child to remove
     *      @return returns the removed node
     *
     */
    removeChild(oldChild: Class_XmlNode): Class_XmlNode;

    /**
     * @description Detaches the node from its parent and returns it
     *
     *      Unlike removeChild, the node knows its parent: the call finds it and removes itself.
     *      When the node has no parent (it is detached, or it is a document) the call has no
     *      effect and returns null. For the document element this member only detaches the node:
     *      the document still reports it through documentElement and refuses to accept another
     *      element, because remove() bypasses the document-level slot release - use
     *      document.removeChild(root) to clear the slot as well (see XmlDocument.documentElement).
     *      In the standard this convenience lives on ChildNode, not on Node.
     *
     *      @return returns the detached node, or null when the node has no parent
     *
     */
    remove(): Class_XmlNode;

    /**
     * @description Replaces the node with one or more nodes
     *
     *      The new values are inserted in order before the node, which is then removed. Each
     *      argument may be a node or a string (a string becomes a text node created by the owner
     *      document); values of other types are ignored. When the node has no parent the call is a
     *      no-op. The member returns nothing; ChildNode.replaceWith in the standard follows the
     *      same rules.
     *
     *      @param nodes one or more nodes or strings that replace the current node
     *
     */
    replaceWith(...nodes: any[]): void;

    /**
     * @description Inserts one or more nodes before the current node
     *
     *      The values are inserted under the same parent, before this node. Each argument may be a
     *      node or a string (converted to a text node); values of other types are ignored, and
     *      with no parent the call is a no-op. See insertBefore for the single-node form.
     *
     *      @param nodes one or more nodes or strings to insert before the current node
     *
     */
    before(...nodes: any[]): void;

    /**
     * @description Inserts one or more nodes after the current node
     *
     *      The values are inserted under the same parent, after this node, in argument order. Each
     *      argument may be a node or a string (converted to a text node); values of other types
     *      are ignored, and with no parent the call is a no-op. See insertAfter for the
     *      single-node form.
     *
     *      @param nodes one or more nodes or strings to insert after the current node
     *
     */
    after(...nodes: any[]): void;

    /**
     * @description Checks whether this node is the given node or one of its ancestors
     *
     *      The node itself counts as contained, so `x.contains(x)` is true; the check is
     *      reference-based and does not compare structure. A cross-tree argument simply returns
     *      false; a non-node argument throws a TypeError (20005).
     *
     *      @param node the node to look for among the descendants
     *      @return returns true when the given node is this node or a descendant, otherwise false
     *
     */
    contains(node: Class_XmlNode): boolean;

    /**
     * @description Returns the topmost ancestor of the node
     *
     *      The result is the document for a connected node and the node itself when it is
     *      detached. For a document the call returns the document, and for the document element it
     *      returns the document as well.
     *
     *      @return returns the root of the tree the node belongs to
     *
     */
    getRootNode(): Class_XmlNode;

    /**
     * @description Queries whether the node is part of a document tree
     *
     *      A node is connected when the root of its tree is a document, which includes the
     *      document node itself; a freshly created or detached node, and every node of a detached
     *      subtree, is not connected. This is a read-only computed property.
     *
     */
    readonly isConnected: boolean;

    /**
     * @description Compares the position of another node against this node
     *
     *      The result is a bitmask; test it with the bits below (they are not exported as
     *      constants by the xml module):
     *      - 0: the two references are the same node;
     *      - 4 (FOLLOWING): the other node comes after this node in document order;
     *      - 2 (PRECEDING): the other node comes before this node;
     *      - 8 (CONTAINS): the other node is an ancestor of this node;
     *      - 16 (CONTAINED_BY): the other node is a descendant of this node;
     *      - 1 (DISCONNECTED): the two nodes are not in the same tree;
     *      - 32 (IMPLEMENTATION_SPECIFIC): declared by the standard but never set here.
     *
     *      An ancestor/descendant result also carries the direction bit: 10 = CONTAINS |
     *      PRECEDING when the other node is an ancestor, 20 = CONTAINED_BY | FOLLOWING when it is
     *      a descendant. Two nodes in different trees - for example a detached node - yield
     *      3 = DISCONNECTED | PRECEDING. An XmlAttr is not an XmlNode and cannot be passed here
     *      (TypeError 20005); a non-node argument throws the same error.
     *
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<r><a/><b/></r>');
     *      const a = doc.documentElement.firstChild;
     *      const b = doc.documentElement.lastChild;
     *
     *      console.log(a.compareDocumentPosition(b));                   // 4 (following)
     *      console.log(b.compareDocumentPosition(a));                   // 2 (preceding)
     *      console.log(a.compareDocumentPosition(doc.documentElement)); // 10 (contains)
     *      ```
     *
     *      @param other the node to compare against
     *      @return returns the position bitmask
     *
     */
    compareDocumentPosition(other: Class_XmlNode): number;

    /**
     * @description Checks whether two nodes are structurally equal
     *
     *      Two nodes are equal when they have the same node type and node name, the same node
     *      value, the same attributes (an element's attributes are compared by name and value,
     *      independent of order) and the same child list, compared recursively; text nodes are
     *      compared by their character data, so whitespace and CDATA-versus-text differences are
     *      significant. The comparison ignores the owning document and node identity, and it does
     *      not see namespace declarations as attributes. A non-node argument throws a TypeError
     *      (20005).
     *
     *      @param other the node to compare with
     *      @return returns true when the two nodes are structurally equal, otherwise false
     *
     */
    isEqualNode(other: Class_XmlNode): boolean;

    /**
     * @description Checks whether two references point to the same node
     *
     *      Equivalent to the `===` operator; use isEqualNode for a structural comparison. A
     *      non-node argument throws a TypeError (20005).
     *
     *      @param other the node to compare with
     *      @return returns true when both references point to the same node, otherwise false
     *
     */
    isSameNode(other: Class_XmlNode): boolean;

}

