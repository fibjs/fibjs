/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/// <reference path="../interface/DOMTokenList.d.ts" />
/// <reference path="../interface/DOMStringMap.d.ts" />
/// <reference path="../interface/CSSStyleDeclaration.d.ts" />
/// <reference path="../interface/XmlDocumentFragment.d.ts" />
/// <reference path="../interface/XmlNamedNodeMap.d.ts" />
/// <reference path="../interface/XmlAttr.d.ts" />
/// <reference path="../interface/XmlNodeList.d.ts" />
/**
 * @description XmlElement is the element node type of the fibjs XML/HTML DOM: the only node that
 *  carries a tag name, namespace information and attributes, and the main handle used to
 *  navigate, query, mutate and serialize a document
 *
 *  XmlElement extends XmlNode; use the XmlNode members for the generic tree operations
 *  (parentNode, childNodes, siblings, cloneNode, insertBefore, removeChild and so on) and the
 *  XmlElement members below when a subtree is identified by an element — reading or changing
 *  its attributes, searching its descendants or producing markup. Text, comment, attribute
 *  and document nodes are separate interfaces.
 *
 *  Concepts:
 *
 *  - **XML and HTML mode**: the document chooses the mode when it is parsed or created. In XML
 *    mode tag and attribute names are case-sensitive, selectors are case-sensitive and empty
 *    elements serialize as `<tag/>`. In HTML mode tagName is upper-cased, attribute names are
 *    lower-cased, tag lookups and type selectors are case-insensitive and empty elements
 *    serialize as `<tag></tag>` or as a void tag such as `<br>`. Some members exist only in
 *    HTML mode (outerHTML, classList, style and dataset) and throw an invalid-call error
 *    (20009) in XML mode, and template content is null outside HTML mode; innerHTML, the
 *    attribute methods and the query methods work in both modes.
 *  - **Node tree and ownership**: an element becomes part of a tree only when it is inserted.
 *    A node created by createElement has no parent until then, and every node records its
 *    ownerDocument. Inserting a node that belongs to another document adopts it (the
 *    ownerDocument changes and the node leaves the old tree); inserting a node that contains
 *    the element itself is rejected; re-inserting an existing node moves it.
 *  - **Attributes vs properties**: attributes form an ordered map accessible through
 *    attributes, getAttribute, setAttribute and removeAttribute. Names are used as-is in XML
 *    mode and lower-cased in HTML mode. The reflection properties (id, src, className and the
 *    other name reflectors) are convenience views of the same attributes: reading an absent
 *    one gives an empty string and writing an empty string removes the attribute. fibjs
 *    exposes the ten HTML name reflectors on every element in both modes, so they also work on
 *    XML elements; the standard defines them only on specific HTML elements.
 *  - **Namespaces**: namespaceURI, prefix and localName describe an element's namespace; an
 *    element without a namespace reports null for namespaceURI and prefix, and its localName
 *    equals tagName. The `*NS` attribute and query methods match a namespace URI plus local
 *    name (`*` is a wildcard for either), and a prefix is only a serialization detail:
 *    declarations missing on the ancestors are added when the subtree is serialized.
 *  - **Querying**: getElementsByTagName, getElementsByClassName, getElementById, querySelector
 *    and querySelectorAll search the descendants of the element and never return the element
 *    itself; matches tests the element itself and closest walks from the element up through
 *    its ancestors. Selectors support type, `#id`, `.class`, `[attr]` with `=`, `^=`, `$=`,
 *    `*=`, `~=`, `|=`, the descendant/`>`/`+`/`~` combinators and the `:first-child`,
 *    `:last-child`, `:nth-child()`, `:only-child`, `:not()`, `:is()`, `:where()` and `:has()`
 *    pseudo-classes; pseudo-elements never match. Query results are snapshots: they do not
 *    change when the document is mutated, so query again afterwards.
 *  - **Insertion and serialization**: append, prepend, replaceChildren and the insertAdjacent*
 *    family accept nodes and strings (strings become text nodes) and ignore values of any
 *    other type. Most return undefined; insertAdjacentElement returns the inserted element.
 *    Serialize with innerHTML (children only), outerHTML (the element and its children, HTML
 *    only), toString()/String(el) (a fibjs extension shared by the whole document model, the
 *    element and its children) or the XMLSerializer interface.
 *  - **No events and no layout**: fibjs has no DOM dispatch pipeline and no rendering, so an
 *    XmlElement is not an EventTarget (no addEventListener, on or emit) and has no geometry
 *    members such as getBoundingClientRect. See the DOMEvent interface for the standalone
 *    event object.
 *
 *  Obtained from:
 *  - `xml.parse(source[, type][, options])` — returns an XmlDocument; `documentElement` and
 *    the document query methods give XmlElement objects;
 *  - `new xml.Document([type])` or `new XMLDocument([type])` — then createElement(name) /
 *    createElementNS(namespaceURI, qualifiedName) and insert the result yourself;
 *  - `new DOMParser().parseFromString(source, mimeType)` — the same document API applies;
 *  - tree operations: navigation properties (parentElement, children, firstElementChild,
 *    nextElementSibling ...), cloneNode, importNode, adoptNode and the XmlNodeList items
 *    returned by the query methods.
 *
 *  Example 1 — parse inline XML and read an element's name and attributes:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<library><book id="b1" lang="en" pages="320">Dune</book></library>');
 *  const book = doc.documentElement.firstElementChild;
 *
 *  console.log(book.tagName);                  // book
 *  console.log(book.localName);                // book
 *  console.log(book.getAttribute('id'));       // b1
 *  console.log(book.getAttribute('missing'));  // null
 *  console.log(book.hasAttribute('pages'));    // true
 *  console.log(book.attributes.length);        // 3
 *  console.log(book.textContent);              // Dune
 *  ```
 *
 *  Example 2 — navigate and query the tree:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<catalog>'
 *      + '<book id="b1" class="fiction sale"><title>Dune</title></book>'
 *      + '<book id="b2" class="fiction"><title>Neuromancer</title></book>'
 *      + '</catalog>');
 *  const catalog = doc.documentElement;
 *
 *  console.log(catalog.children.length);                       // 2
 *  console.log(catalog.getElementsByTagName('book').length);   // 2
 *  console.log(catalog.getElementsByClassName('sale').length); // 1
 *  console.log(catalog.querySelector('book.sale > title').textContent); // Dune
 *  console.log(catalog.querySelectorAll('book').length);       // 2
 *
 *  const second = catalog.getElementById('b2');
 *  console.log(second.matches('book.fiction'));                // true
 *  console.log(second.closest('catalog') === catalog);         // true
 *  ```
 *
 *  Example 3 — mutate attributes and children, then serialize:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<menu><item>tea</item></menu>');
 *  const menu = doc.documentElement;
 *  const item = menu.firstElementChild;
 *
 *  item.setAttribute('price', '3');
 *  item.append(' + milk');
 *  item.insertAdjacentHTML('afterend', '<item>coffee</item>');
 *  item.toggleAttribute('sold-out');
 *
 *  console.log(String(menu));
 *  // <menu><item price="3" sold-out="">tea + milk</item><item>coffee</item></menu>
 *  console.log(menu.querySelector('item[price]').textContent); // tea + milk
 *  ```
 *
 *  Example 4 — HTML mode reflection objects:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<html><body>'
 *      + '<div id="hero" class="card wide" data-role="banner">Hi</div>'
 *      + '</body></html>', 'text/html');
 *  const hero = doc.getElementById('hero');
 *
 *  console.log(hero.tagName);                       // DIV
 *  console.log(hero.classList.contains('wide'));    // true
 *  console.log(hero.dataset.role);                  // banner
 *
 *  hero.classList.add('active');
 *  hero.style.color = 'red';
 *  console.log(hero.outerHTML);
 *  // <div id="hero" class="card wide active" data-role="banner" style="color: red;">Hi</div>
 *  ```
 *
 */
declare class Class_XmlElement extends Class_XmlNode {
    /**
     * @description The namespace URI of the element, or null when the element has no namespace
     *
     *      The URI identifies the namespace; the prefix is only a serialization detail, so
     *      elements with different prefixes and the same URI are in the same namespace. Elements
     *      created with createElement(name) and elements of an HTML document report null
     *      (browsers put HTML elements in http://www.w3.org/1999/xhtml instead). Read it together
     *      with localName and prefix, and use the NS query and attribute methods to match by URI.
     *
     */
    readonly namespaceURI: string;

    /**
     * @description Queries and sets the namespace prefix of the element
     *
     *      The value is null when the element has no namespace. The prefix is the short name
     *      written before the colon (`p` in `<p:item>`); the pair namespaceURI plus localName
     *      identifies the element. Assigning a non-empty prefix makes the serializer declare
     *      `xmlns:<prefix>` when the prefix is not already in scope (walking up the ancestor
     *      chain), and assigning an empty string clears the prefix. The value is not validated
     *      against the URI, and assigning null or a non-string throws a type error (20005).
     *
     */
    prefix: string;

    /**
     * @description The local name of the element, without the namespace prefix
     *
     *      For a prefixed element localName is the part after the colon (`item` for `<p:item>`);
     *      for an element without a namespace it equals tagName. In HTML mode the name is
     *      lower-cased (div for `<DIV>`) while tagName is upper-cased; in XML mode the parsed case
     *      is preserved.
     *
     */
    readonly localName: string;

    /**
     * @description The tag name of the element, including the namespace prefix
     *
     *      In HTML mode the name is upper-cased (DIV for `<div>`); in XML mode the parsed case is
     *      preserved, so `<Item>` and `<item>` are different elements. tagName equals nodeName;
     *      use localName when the prefix must be excluded and the NS query methods to match by
     *      namespace.
     *
     */
    readonly tagName: string;

    /**
     * @description Queries and sets the id attribute of the element
     *
     *      A reflection of the id attribute: reading is equivalent to getAttribute('id') and
     *      returns an empty string when the attribute is absent; assigning an empty string removes
     *      the attribute, any other string creates or updates it. The id is used by
     *      getElementById and by the `#name` CSS selector; matches are exact and case-sensitive in
     *      both modes. Works on XML elements as well.
     *
     */
    id: string;

    /**
     * @description Queries and sets the src attribute of the element
     *
     *      One of the ten HTML attribute reflectors that fibjs exposes on every element (see the
     *      XmlElement class notes): reading is equivalent to getAttribute('src') and returns an
     *      empty string when the attribute is absent, writing creates or updates it and writing an
     *      empty string removes it. Unlike a browser the value is not resolved against the
     *      document base URL and no element-specific processing (loading, decoding) happens, so it
     *      behaves like a plain string attribute on any element and in XML mode as well.
     *
     */
    src: string;

    /**
     * @description Queries and sets the alt attribute of the element
     *
     *      Generic attribute reflection like src: reading gives the raw attribute value or an
     *      empty string, writing syncs to the attribute and an empty string removes it. No
     *      element-specific behavior is applied, so it works on any element in both XML and HTML
     *      mode.
     *
     */
    alt: string;

    /**
     * @description Queries and sets the href attribute of the element
     *
     *      Generic attribute reflection like src: reading gives the raw attribute value or an
     *      empty string, writing syncs to the attribute and an empty string removes it. The value
     *      is not resolved against the document base URL. Works on any element and in both modes.
     *
     */
    href: string;

    /**
     * @description Queries and sets the title attribute of the element
     *
     *      Generic attribute reflection like src; it never falls back to an ancestor title or to
     *      the text content the way a browser tooltip would. Reading an absent attribute gives an
     *      empty string and writing an empty string removes it.
     *
     */
    title: string;

    /**
     * @description Queries and sets the value attribute of the element
     *
     *      Generic attribute reflection like src, with one difference from browsers: it always
     *      reads and writes the value attribute and never the current form value property
     *      (input.value in a browser changes with typing without touching the attribute). Works on
     *      any element and in both modes.
     *
     */
    value: string;

    /**
     * @description Queries and sets the name attribute of the element
     *
     *      Generic attribute reflection like src: the raw attribute is read and written, and no
     *      element-specific semantics (form submission, radio grouping, window lookup) are
     *      applied. Works on any element and in both modes.
     *
     */
    name: string;

    /**
     * @description Queries and sets the type attribute of the element
     *
     *      Generic attribute reflection like src: the attribute string is read and written as-is;
     *      no known-values validation is performed (a browser reflects input.type only for the
     *      values in its type table). Works on any element and in both modes.
     *
     */
    type: string;

    /**
     * @description Queries and sets the rel attribute of the element
     *
     *      Generic attribute reflection like src: the raw attribute is read and written, with no
     *      link-type parsing. Works on any element and in both modes.
     *
     */
    rel: string;

    /**
     * @description Queries and sets the target attribute of the element
     *
     *      Generic attribute reflection like src: the raw attribute is read and written, with no
     *      browsing-context resolution. Works on any element and in both modes.
     *
     */
    target: string;

    /**
     * @description Queries and sets the placeholder attribute of the element
     *
     *      Generic attribute reflection like src: the raw attribute is read and written, with no
     *      element applicability checks. Works on any element and in both modes.
     *
     */
    placeholder: string;

    /**
     * @description Queries and sets the markup of the element's children
     *
     *      Reading serializes all child nodes (an empty string when there are none); empty
     *      elements follow the document mode (`<b/>` in XML, `<b></b>` or a void tag such as
     *      `<br>` in HTML) and special characters are escaped. Assigning replaces every child
     *      node with the parsed content: in XML mode the fragment is parsed inside a temporary
     *      root element, so it must be well-formed XML and a parse error (20024) leaves the
     *      element unchanged; in HTML mode the fragment is parsed as HTML. Assigning a non-string
     *      throws a type error (20005), and an empty string only clears the children. For a
     *      `<template>` element in HTML mode, reading returns the serialization of the children
     *      kept in content.
     *
     *      Example — replace the children and read the result back:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<div><i>old</i></div>');
     *      const div = doc.documentElement;
     *
     *      console.log(div.innerHTML);          // <i>old</i>
     *      div.innerHTML = '<b>new</b><u>text</u>';
     *      console.log(div.innerHTML);          // <b>new</b><u>text</u>
     *      div.innerHTML = '';
     *      console.log(String(div));            // <div/>
     *      console.log(div.childNodes.length);  // 0
     *      ```
     *
     */
    innerHTML: string;

    /**
     * @description Queries and sets the markup of the element and its children; HTML mode only
     *
     *      Reading serializes the element like toString(): HTML void elements are written without
     *      an end tag (`<br>`) and empty non-void elements as `<tag></tag>`. Assigning parses the
     *      HTML fragment and replaces the element in its parent with the parsed nodes; when the
     *      element has no parent the assignment does nothing, and assigning to the document
     *      element (a direct child of the document) throws an error (20024). XML mode has no
     *      concept of HTML fragments, so both reading and writing throw an invalid-call error
     *      (20009); use toString or the XMLSerializer interface instead.
     *
     */
    outerHTML: string;

    /**
     * @description Queries and sets the class attribute of the element
     *
     *      A reflection of the class attribute: reading is equivalent to getAttribute('class') and
     *      returns an empty string when absent, writing an empty string removes the attribute.
     *      The value is an opaque string here; the classList object tokenizes it in HTML mode.
     *      Works on XML elements as well.
     *
     */
    className: string;

    /**
     * @description Returns the DOMTokenList wrapping the element's class attribute; HTML mode only
     *
     *      The object is created on first access and cached for the lifetime of the element, so
     *      repeated reads return the same reference; changes made through it (add, remove,
     *      toggle, replace) are written back to the class attribute and appear in the
     *      serialization immediately. Reading the property in XML mode throws an invalid-call
     *      error (20009); use className to read or write the whole attribute there.
     *
     *      Example — token-level class manipulation:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<html><body><div class="card wide">Hi</div></body></html>',
     *          'text/html');
     *      const div = doc.querySelector('div');
     *
     *      console.log(div.className);                  // card wide
     *      console.log(div.classList.contains('wide')); // true
     *      div.classList.remove('wide');
     *      div.classList.add('active');
     *      console.log(div.className);                  // card active
     *      console.log(String(div));                    // <div class="card active">Hi</div>
     *      ```
     *
     */
    readonly classList: Class_DOMTokenList;

    /**
     * @description Returns the DOMStringMap exposing the element's data-* attributes; HTML mode only
     *
     *      Keys are the data-* attribute names converted to camelCase (data-user-id becomes
     *      userId) and values are strings; assigning a value creates or updates the attribute,
     *      assigning an empty string or deleting the key removes it, and a non-string assignment
     *      is converted with String(). The object is cached per element and stays in sync with
     *      attributes changed through the attribute methods. Reading the property in XML mode
     *      throws an invalid-call error (20009).
     *
     *      Example — read and write data-* attributes through dataset:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<html><body><div data-role="banner">Hi</div></body></html>',
     *          'text/html');
     *      const div = doc.querySelector('div');
     *
     *      console.log(div.dataset.role); // banner
     *      div.dataset.role = 'main';
     *      div.dataset.count = 2;
     *      console.log(String(div));
     *      // <div data-role="main" data-count="2">Hi</div>
     *      ```
     *
     */
    readonly dataset: Class_DOMStringMap;

    /**
     * @description Returns the CSSStyleDeclaration of the element's style attribute; HTML mode only
     *
     *      The object is cached per element and is a live view of the inline style: changing a
     *      camelCase property (style.maxWidth) or cssText updates the style attribute, and
     *      changes made to the attribute are visible through the object. Declarations are
     *      serialized back into the attribute in the order they were set, and values set through
     *      the object keep a trailing semicolon (`color: red;`) while a parsed attribute may not
     *      have one. Reading the property in XML mode throws an invalid-call error (20009).
     *
     *      Example — read a parsed declaration and add one:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<html><body><div style="margin: 0">Hi</div></body></html>',
     *          'text/html');
     *      const div = doc.querySelector('div');
     *
     *      console.log(div.style.margin);         // 0
     *      div.style.color = 'red';
     *      console.log(div.getAttribute('style')); // margin: 0; color: red;
     *      ```
     *
     */
    readonly style: Class_CSSStyleDeclaration;

    /**
     * @description Returns the fragment holding the children of a template element; HTML mode only
     *
     *      For a `<template>` element the children are moved into the returned fragment on first
     *      access and `childNodes` on the template itself becomes empty; modifying the fragment
     *      changes what innerHTML returns, and the same fragment is returned on every access. For
     *      any other element, and for XML documents, the property is null.
     *
     */
    readonly content: Class_XmlDocumentFragment;

    /**
     * @description Returns the named node map containing all attributes of the element
     *
     *      The attributes are in document order. The returned XmlNamedNodeMap is live: it is the
     *      element's attribute storage, so setAttribute/removeAttribute calls and attribute value
     *      assignments are visible through it (and vice versa), and indexed access
     *      (`attributes[0]`), length, item() and getNamedItem() all work. setNamedItem() and
     *      removeNamedItem() are not implemented — mutate through setAttribute/removeAttribute
     *      instead. It is empty (length 0) for an element without attributes; namespace
     *      declarations are ordinary attributes here.
     *
     *      Example — read the map and change a value through it:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<book id="b1" lang="en"/>');
     *      const book = doc.documentElement;
     *
     *      console.log(book.attributes.length);                     // 2
     *      console.log(book.attributes[0].name);                    // id
     *      console.log(book.attributes.getNamedItem('lang').value); // en
     *
     *      book.attributes[0].value = 'b2';
     *      console.log(book.getAttribute('id'));                    // b2
     *      ```
     *
     */
    readonly attributes: Class_XmlNamedNodeMap;

    /**
     * @description Checks whether the element has any attributes
     *
     *      Equivalent to `attributes.length > 0`; a freshly created element has none. Namespace
     *      declarations (xmlns and xmlns:*) count as attributes.
     *      @return returns true if the current element has attributes, otherwise returns false
     *
     */
    hasAttributes(): boolean;

    /**
     * @description Returns the value of an attribute by name
     *
     *      Returns null when the attribute is absent, the same as a browser. Names are matched
     *      exactly in XML mode and lower-cased in HTML mode, so getAttribute('ID') finds the id
     *      attribute of an HTML element; an empty name returns null. The raw attribute text is
     *      returned — no URL resolution or element-specific conversion is applied. Use
     *      hasAttribute when only the presence matters.
     *
     *      Example — read present and absent attributes:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const img = xml.parse('<img src="a.png" alt="pic"/>').documentElement;
     *
     *      console.log(img.getAttribute('src'));   // a.png
     *      console.log(img.getAttribute('title')); // null
     *      console.log(img.hasAttribute('alt'));   // true
     *      ```
     *      @param name the name of the attribute to query
     *      @return returns the value of the attribute, or null if there is no such attribute
     *
     */
    getAttribute(name: string): string;

    /**
     * @description Returns the value of an attribute by namespace URI and local name
     *
     *      The lookup uses the namespace identity instead of the serialized prefix, so an
     *      attribute written as `x:k` is found by its URI regardless of the prefix used in the
     *      markup. Pass the empty string or null as namespaceURI to match an attribute without a
     *      namespace. Returns null when no matching attribute exists.
     *      @param namespaceURI the namespace URI to query
     *      @param localName the name of the attribute to query
     *      @return returns the value of the attribute, or null if there is no such attribute
     *
     */
    getAttributeNS(namespaceURI: string, localName: string): string;

    /**
     * @description Returns the attribute node with the specified name
     *
     *      Returns the XmlAttr object owned by the element (its name, value and namespace
     *      properties are readable and value is writable) or null when the attribute is absent.
     *      The name is matched like getAttribute. Detach the node with removeAttributeNode to move
     *      it to another element; an attribute still owned by an element cannot be attached
     *      elsewhere.
     *      @param name the name of the attribute to query
     *      @return returns the XmlAttr object with the specified name, or null if there is no such attribute
     *
     */
    getAttributeNode(name: string): Class_XmlAttr;

    /**
     * @description Returns the attribute node with the specified namespace URI and local name
     *
     *      Like getAttributeNode but matching by namespace URI and local name instead of the
     *      serialized name; returns null when no attribute matches. A namespace URI with an empty
     *      local name (or vice versa) never matches.
     *      @param namespaceURI the namespace URI to query
     *      @param localName the name of the attribute to query
     *      @return returns the XmlAttr object with the specified name, or null if there is no such attribute
     *
     */
    getAttributeNodeNS(namespaceURI: string, localName: string): Class_XmlAttr;

    /**
     * @description Creates or changes an attribute
     *
     *      If the element already has an attribute with that name its value is replaced in place
     *      (the position in the attribute list is kept); otherwise a new attribute is appended.
     *      The name is used as-is in XML mode and lower-cased in HTML mode. Both arguments must be
     *      strings: a number, boolean, null or undefined throws a type error (20005) instead of
     *      being converted, which differs from the Web IDL coercion browsers apply — convert the
     *      value explicitly.
     *      @param name the name of the attribute to set
     *      @param value the value of the attribute to set
     *
     */
    setAttribute(name: string, value: string): void;

    /**
     * @description Creates or changes an attribute with a namespace
     *
     *      Like setAttribute, but the attribute is identified by namespace URI plus qualified name
     *      (prefix:localName): an existing attribute with the same URI and local name gets the new
     *      value and prefix, otherwise a new attribute is created. The prefix is a serialization
     *      hint only — getAttributeNS matches by URI and local name. Declarations for the reserved
     *      `xml` and `xmlns` prefixes are ignored. All three arguments must be strings or a type
     *      error (20005) is thrown.
     *
     *      Example — create a namespaced attribute and read it back:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<root xmlns:p="urn:p"><p:item/></root>');
     *      const item = doc.documentElement.firstElementChild;
     *
     *      item.setAttributeNS('urn:p', 'p:code', 'X1');
     *      console.log(item.getAttributeNS('urn:p', 'code')); // X1
     *      console.log(item.getAttribute('p:code'));          // X1
     *      console.log(String(item));                         // <p:item p:code="X1"/>
     *      ```
     *      @param namespaceURI the namespace URI to set
     *      @param qualifiedName the name of the attribute to set
     *      @param value the value of the attribute to set
     *
     */
    setAttributeNS(namespaceURI: string, qualifiedName: string, value: string): void;

    /**
     * @description Attaches an XmlAttr object to the element
     *
     *      The attribute's name and namespace decide which attribute it replaces: when the element
     *      already has one with the same name, the replaced XmlAttr is returned, otherwise null is
     *      returned. The attribute must belong to no element or to this one; an attribute owned by
     *      another element is rejected with an error (20024) — remove it there first — and passing
     *      one of the element's own attributes just returns it without changing anything.
     *      @param attr the XmlAttr object to set
     *      @return returns the replaced XmlAttr object, or NULL if nothing was replaced
     *
     */
    setAttributeNode(attr: Class_XmlAttr): Class_XmlAttr;

    /**
     * @description Removes an attribute by name
     *
     *      Does nothing (no error) when the element has no attribute with that name; the name is
     *      lower-cased first in HTML mode. Use removeAttributeNS to remove by namespace URI and
     *      local name, or removeAttributeNode to remove a specific XmlAttr object.
     *      @param name the name of the attribute to remove
     *
     */
    removeAttribute(name: string): void;

    /**
     * @description Removes an attribute by namespace URI and local name
     *
     *      The attribute is located by namespace identity, not by the serialized prefix, so it is
     *      removed regardless of how the prefix was written; nothing happens when no attribute
     *      matches. Pass the empty string or null as namespaceURI for an attribute without a
     *      namespace.
     *      @param namespaceURI the namespace URI to remove
     *      @param localName the name of the attribute to remove
     *
     */
    removeAttributeNS(namespaceURI: string, localName: string): void;

    /**
     * @description Detaches an attribute node from the element
     *
     *      The XmlAttr object must currently belong to this element: the removed node is returned
     *      and becomes detached, so it can be attached to another element with setAttributeNode.
     *      An attribute owned by another element, or one that was already removed, throws an error
     *      (20024), which is stricter than a browser's DOMException — check hasAttribute or
     *      getAttributeNode before calling.
     *      @param attr the XmlAttr object to remove
     *      @return returns the removed XmlAttr object
     *
     */
    removeAttributeNode(attr: Class_XmlAttr): Class_XmlAttr;

    /**
     * @description Checks whether the element has an attribute with the specified name
     *
     *      Matching follows getAttribute: exact in XML mode, lower-cased in HTML mode; an empty
     *      name is never present. Namespace declarations count as attributes.
     *      @param name the name of the attribute to query
     *      @return returns true if the current element node has the specified attribute, otherwise returns false
     *
     */
    hasAttribute(name: string): boolean;

    /**
     * @description Checks whether the element has an attribute with the given namespace and name
     *
     *      The namespace URI takes precedence over the serialized prefix; pass the empty string or
     *      null as namespaceURI to test an attribute without a namespace.
     *      @param namespaceURI the namespace URI to query
     *      @param localName the name of the attribute to query
     *      @return returns true if the current element node has the specified attribute, otherwise returns false
     *
     */
    hasAttributeNS(namespaceURI: string, localName: string): boolean;

    /**
     * @description Returns a snapshot list of all descendant elements with the specified tag name
     *
     *      Only descendants are returned — the element itself is never part of the result — and
     *      the value "*" matches every element. HTML mode compares tag names case-insensitively
     *      (`DIV` and `div` both match) while XML mode is case-sensitive. The returned XmlNodeList
     *      is a snapshot taken at call time, so later insertions, removals and renames are not
     *      reflected in the same list: query again after mutating. getElementsByTagNameNS matches
     *      by namespace and local name, getElementsByClassName by class, and the querySelector
     *      methods by arbitrary CSS selectors.
     *
     *      Example — subtree scope and the wildcard:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<root><a><b><c/></b></a><b/><d/></root>');
     *      const root = doc.documentElement;
     *
     *      console.log(root.getElementsByTagName('b').length);    // 2
     *      console.log(root.getElementsByTagName('*').length);    // 5
     *      console.log(root.getElementsByTagName('root').length); // 0: self is not searched
     *      ```
     *      @param tagName the tag name to retrieve. The value "*" matches all tags
     *      @return an XmlNodeList collection of XmlElement nodes with the specified tag in the node tree
     *
     */
    getElementsByTagName(tagName: string): Class_XmlNodeList;

    /**
     * @description Returns a snapshot list of all descendant elements matching a namespace and name
     *
     *      Element names are matched by namespace identity rather than the serialized prefix, and
     *      "*" works as a wildcard for either argument: "*" as namespaceURI matches elements in
     *      any namespace including none, and "*" as localName matches every local name. Like
     *      getElementsByTagName the result is a snapshot and excludes the element itself. Elements
     *      created by createElement(name) have no namespace, so they only match the "*" namespace
     *      form.
     *      @param namespaceURI the namespace URI to query
     *      @param localName the tag name to retrieve. The value "*" matches all tags
     *      @return an XmlNodeList collection of XmlElement nodes with the specified tag in the node tree
     *
     */
    getElementsByTagNameNS(namespaceURI: string, localName: string): Class_XmlNodeList;

    /**
     * @description Returns the first descendant element with the specified id attribute
     *
     *      A fibjs extension: the DOM standard defines getElementById on Document only; here the
     *      search is limited to the descendants of this element, so the element itself and its
     *      ancestors can never be returned. The value is compared exactly and case-sensitively in
     *      both modes, the first element in document order wins when an id is duplicated, and null
     *      is returned for an empty id or no match. Elements whose id was assigned through the id
     *      property or setAttribute are found.
     *
     *      Example — element-scoped lookup does not see ancestors:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<root><item id="i1"/><section><item id="i2"/></section></root>');
     *      const section = doc.getElementsByTagName('section')[0];
     *
     *      console.log(String(section.getElementById('i2'))); // <item id="i2"/>
     *      console.log(section.getElementById('i1'));         // null: i1 is outside the subtree
     *      console.log(doc.getElementById('i1') !== null);    // true
     *      ```
     *      @param id the id to retrieve
     *      @return the XmlElement node with the specified id attribute, or null if there is no match
     *
     */
    getElementById(id: string): Class_XmlElement;

    /**
     * @description Returns a snapshot list of all descendant elements with the specified class name
     *
     *      The element itself is excluded. The argument may list several class names separated by
     *      whitespace; an element matches only when it carries every one of them. Class names are
     *      compared case-sensitively in both XML and HTML mode. The returned XmlNodeList is a
     *      snapshot rather than a live collection, so query again after changing the tree or the
     *      class attributes.
     *      @param className the class name to retrieve
     *      @return an XmlNodeList collection of XmlElement nodes with the specified class name in the document tree
     *
     */
    getElementsByClassName(className: string): Class_XmlNodeList;

    /**
     * @description Returns the first descendant element matching a CSS selector
     *
     *      The element itself is not a candidate and the first match in document order is
     *      returned; when nothing matches, null is returned. An empty selector throws an error
     *      (20024); other malformed selectors either throw the same error or yield no match,
     *      depending on where the parse fails. Selector support: type, `#id`, `.class`, `[attr]`
     *      with `=`, `^=`, `$=`, `*=`, `~=`, `|=`, the descendant/`>`/`+`/`~` combinators and the
     *      `:first-child`, `:last-child`, `:nth-child()`, `:only-child`, `:not()`, `:is()`,
     *      `:where()` and `:has()` pseudo-classes; pseudo-elements never match. Tag matching is
     *      case-insensitive in HTML mode and case-sensitive in XML mode, while class and id
     *      matching is case-sensitive in both modes. Use querySelectorAll for every match.
     *
     *      Example — select descendants by tag, class and position:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<list><item class="odd">a</item><item class="even">b</item></list>');
     *      const list = doc.documentElement;
     *
     *      console.log(list.querySelector('item.odd').textContent);          // a
     *      console.log(list.querySelector('item:nth-child(2)').textContent); // b
     *      console.log(list.querySelectorAll('item').length);                // 2
     *      console.log(list.querySelector('list'));                          // null: self is not searched
     *      ```
     *      @param selectors the CSS selector
     *      @return the XmlElement node matching the specified CSS selector, or null if there is no match
     *
     */
    querySelector(selectors: string): Class_XmlElement;

    /**
     * @description Returns a snapshot list of all descendant elements matching a CSS selector
     *
     *      The results are in document order and exclude the element itself; the list is a
     *      snapshot, so elements added after the call do not appear in it and nodes removed from
     *      the document stay reachable through it while it is referenced. The selector syntax and
     *      error behavior are those of querySelector; an empty selector throws (20024). The list
     *      supports iteration, indexed access and a toString() that serializes the matched
     *      elements.
     *      @param selectors the CSS selector
     *      @return an XmlNodeList collection of XmlElement nodes matching the specified CSS selector
     *
     */
    querySelectorAll(selectors: string): Class_XmlNodeList;

    /**
     * @description Tests whether the element itself matches a CSS selector
     *
     *      Unlike querySelector this method does not search the descendants: it answers whether
     *      the element would be selected by the selector, which makes it useful for conditional
     *      logic. Selector syntax and case rules match querySelector, and a pseudo-element
     *      selector always returns false. An empty or malformed selector throws (20024), and only
     *      a string is accepted — a non-string argument throws a type error (20005) instead of
     *      being converted. Use closest to find the nearest ancestor that matches.
     *
     *      Example — test the element and find a matching ancestor:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<list><item class="odd">x</item></list>');
     *      const item = doc.querySelector('item');
     *
     *      console.log(item.matches('item.odd'));      // true
     *      console.log(item.matches('item.even'));     // false
     *      console.log(item.matches('list > item'));   // true
     *      console.log(item.closest('list') !== null); // true
     *      console.log(item.closest('missing'));       // null
     *      ```
     *      @param selectors the CSS selector
     *      @return returns true if the current element matches the specified selector, otherwise returns false
     *
     */
    matches(selectors: string): boolean;

    /**
     * @description Searches the element and its ancestors for the nearest one matching a CSS selector
     *
     *      The walk starts at the element itself (so closest can return the element) and continues
     *      through parent elements until a match is found; null is returned when the root is
     *      reached without a match, and non-element ancestors (a document or document fragment)
     *      end the walk. The selector syntax and the error behavior are those of matches: an empty
     *      selector throws (20024) and a non-string argument throws a type error (20005).
     *      @param selectors the CSS selector
     *      @return returns the nearest matching ancestor element (possibly the element itself), or null if there is no match
     *
     */
    closest(selectors: string): Class_XmlElement;

    /**
     * @description Appends one or more nodes to the end of the element's children
     *
     *      Each argument is either a node — which is moved here from its previous position — or a
     *      string, which becomes a new text node; arguments of any other type (number, boolean,
     *      null, undefined, plain object) are ignored silently, and a node that contains the
     *      element is rejected. Arguments keep their order, so append('a', node) puts the text
     *      node before the moved node. The method returns undefined. See prepend to insert at the
     *      beginning, insertAdjacentElement/HTML/Text for the four relative positions, and
     *      replaceChildren to replace the whole child list.
     *
     *      Example — append nodes and strings in order:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<ul><li>1</li></ul>');
     *      const ul = doc.documentElement;
     *      const li = doc.createElement('li');
     *      li.append('2');
     *
     *      ul.append(li, 'tail');
     *      ul.prepend(doc.createElement('head'));
     *      console.log(String(ul));
     *      // <ul><head/><li>1</li><li>2</li>tail</ul>
     *      ```
     *      @param nodes one or more nodes to add; can be node objects or strings
     *
     */
    append(...nodes: any[]): void;

    /**
     * @description Inserts one or more nodes at the beginning of the element's children
     *
     *      Like append but the arguments are inserted before the current first child; their order
     *      is preserved, so prepend('a', 'b') produces the text "ab" in front of the previous
     *      children. Strings become text nodes, other non-node values are ignored, and the method
     *      returns undefined. Use append to insert at the end.
     *      @param nodes one or more nodes to add; can be node objects or strings
     *
     */
    prepend(...nodes: any[]): void;

    /**
     * @description Replaces all children of the element with the specified nodes
     *
     *      Every existing child is removed first, then the arguments are appended exactly as
     *      append does (strings become text nodes, other non-node values are ignored, nodes are
     *      moved from their previous position). Called with no arguments it just empties the
     *      element. Returns undefined.
     *      @param nodes one or more nodes to set; can be node objects or strings
     *
     */
    replaceChildren(...nodes: any[]): void;

    /**
     * @description Inserts an element node at a position relative to this element
     *
     *      The position argument is case-insensitive; the four values follow the DOM. 'beforebegin'
     *      and 'afterend' insert into the parent at the sibling position and return null when the
     *      element has no parent; 'afterbegin' and 'beforeend' insert as the first or last child.
     *      On success the inserted element is returned. An unknown position throws an error
     *      (20024) and a non-element argument throws a type error (20005). Use insertAdjacentText
     *      for text and insertAdjacentHTML for markup; the XmlNode methods before/after handle the
     *      general case.
     *      @param position the insertion position
     *      @param element the element node to insert
     *      @return returns the inserted element, or null if there is no parent to insert into
     *
     */
    insertAdjacentElement(position: string, element: Class_XmlElement): Class_XmlElement;

    /**
     * @description Parses markup and inserts the resulting nodes at a position relative to this element
     *
     *      The position values are those of insertAdjacentElement ('beforebegin', 'afterbegin',
     *      'beforeend', 'afterend', case-insensitive); 'beforebegin' and 'afterend' need a parent
     *      — with a detached element they do not return (fibjs limitation, avoid that call) —
     *      while 'afterbegin' and 'beforeend' always work. The markup is parsed according to the
     *      document mode: an XML document parses the fragment as well-formed XML (a parse error
     *      throws 20024 and nothing is inserted) and an HTML document parses it as HTML, so
     *      several top-level nodes and unclosed tags are accepted. The parsed nodes are inserted
     *      in order. An empty string does nothing and an unknown position throws (20024) before
     *      any parsing. Use insertAdjacentElement for one element object and insertAdjacentText
     *      for plain text.
     *
     *      Example — insert parsed markup as first and last child:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<p><b>x</b></p>');
     *      const p = doc.documentElement;
     *
     *      p.insertAdjacentHTML('afterbegin', '<i>a</i>');
     *      p.insertAdjacentHTML('beforeend', '<i>b</i>');
     *      console.log(String(p)); // <p><i>a</i><b>x</b><i>b</i></p>
     *      ```
     *      @param position the insertion position
     *      @param html the HTML text to insert
     *
     */
    insertAdjacentHTML(position: string, html: string): void;

    /**
     * @description Inserts a text node at a position relative to this element
     *
     *      The text becomes one new text node at the position (the same four case-insensitive
     *      values as insertAdjacentElement); characters are stored literally and escaped only when
     *      the tree is serialized, so markup-looking text is safe. Inserts at
     *      'beforebegin'/'afterend' are skipped when the element has no parent (the call still
     *      returns). The method returns undefined.
     *      @param position the insertion position
     *      @param text the text to insert
     *
     */
    insertAdjacentText(position: string, text: string): void;

    /**
     * @description Toggles a boolean attribute on the element
     *
     *      If the attribute exists it is removed, otherwise it is added with an empty value — the
     *      serialization shows `name=""`, which is how browsers represent boolean attributes. In
     *      HTML mode the name is lower-cased first. Returns whether the attribute exists after the
     *      operation. Use the two-argument form to force the state instead of toggling.
     *
     *      Example — toggle and force:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const input = xml.parse('<input/>').documentElement;
     *
     *      console.log(input.toggleAttribute('checked'));       // true: added
     *      console.log(input.getAttribute('checked'));          // "" (empty string)
     *      console.log(input.toggleAttribute('checked'));       // false: removed
     *      console.log(input.toggleAttribute('checked', true)); // true: forced on
     *      ```
     *      @param name the name of the attribute to toggle
     *      @return returns true if the attribute exists after the operation, otherwise returns false
     *
     */
    toggleAttribute(name: string): boolean;

    /**
     * @description Toggles a boolean attribute on the element with an explicit state
     *
     *      `force` true adds the attribute with an empty value when it is missing and keeps it
     *      otherwise; `force` false removes it when present and does nothing otherwise. The return
     *      value is the resulting existence, so a force-true call always returns true and a
     *      force-false call always returns false. In HTML mode the name is lower-cased first.
     *      @param name the name of the attribute to toggle
     *      @param force if true, adds the attribute forcibly; if false, removes the attribute forcibly
     *      @return returns true if the attribute exists after the operation, otherwise returns false
     *
     */
    toggleAttribute(name: string, force: boolean): boolean;

}

