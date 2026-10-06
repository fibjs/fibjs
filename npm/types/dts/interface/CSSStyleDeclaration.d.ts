/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description CSSStyleDeclaration is the live view of an element's inline `style` declaration block, obtained from the `style` property of an HTML-mode element
 *
 *  The style attribute is the single source of truth: reads and writes of this object are
 *  synchronized with the attribute in real time (and therefore with the innerHTML/outerHTML
 *  serialization), and the object is cached, so `element.style === element.style`. The class is
 *  not constructible; instances exist only through `element.style` for elements created by
 *  parsing a document as text/html, and the global CSSStyleDeclaration name is provided so the
 *  object can be recognized with instanceof.
 *
 *  Concepts:
 *
 *  - **Declaration block model**: the object exposes the declarations of the style attribute, not
 *    the computed style. `length`/`item()` enumerate the declared properties, and the object is a
 *    view: changes made elsewhere on the attribute are visible immediately, and removing the
 *    attribute empties the object.
 *  - **Property names**: standard CSS property names are lowercased and case-insensitive, custom
 *    properties (--*) are case-sensitive. Named access uses camelCase (`style.maxWidth`),
 *    `cssFloat` maps to the CSS `float` property, and the string index also accepts hyphenated
 *    names (`style['max-width']`). getPropertyValue/setProperty take the hyphenated form and do
 *    not convert camelCase. There is no numeric index: style[0] is undefined, use item().
 *  - **Tolerant parsing**: a declaration without a colon, with an empty name or value, an empty
 *    segment or a comment is dropped; quoted strings and nested parentheses (url(...),
 *    calc(...)) are preserved. Duplicate declarations follow the CSS cascade rule: a later plain
 *    declaration does not override an earlier !important one, otherwise the later one replaces
 *    the earlier and moves to the end of the block.
 *  - **Values are verbatim**: fibjs stores the value string without validating or normalizing it,
 *    while browsers drop invalid values, normalize colors and expand shorthands. Assigning a
 *    number or boolean to a named property stringifies it; assigning null, undefined or an empty
 *    string removes the declaration. Setting cssText to an empty or whitespace-only string (or
 *    to null) removes the style attribute.
 *
 *  Obtained from:
 *  - `element.style` — for an element of a document parsed with DOMParser as text/html, e.g.
 *    doc.getElementById(...), doc.body, doc.documentElement or a detached createElement()
 *    result; XML-mode elements throw Error [20009] instead. The identity is stable per element.
 *
 *  Example 1 — read the inline style of a parsed element:
 *  ```JavaScript
 *  const doc = new DOMParser().parseFromString(
 *      '<div id="box" style="max-width: 100%; height: auto"></div>', 'text/html');
 *  const style = doc.getElementById('box').style;
 *
 *  console.log(style.cssText);                       // max-width: 100%; height: auto
 *  console.log(style.length);                        // 2
 *  console.log(style.item(0));                       // max-width
 *  console.log(style.getPropertyValue('max-width')); // 100%
 *  console.log(style.maxWidth);                      // 100%
 *  ```
 *
 *  Example 2 — change declarations with camelCase and methods:
 *  ```JavaScript
 *  const doc = new DOMParser().parseFromString('<img id="i">', 'text/html');
 *  const img = doc.getElementById('i');
 *
 *  img.style.width = '400px';
 *  img.style.setProperty('display', 'none', 'important');
 *  console.log(img.getAttribute('style'));
 *  // width: 400px; display: none !important;
 *
 *  console.log(img.style.getPropertyPriority('display')); // important
 *  img.style.removeProperty('display');
 *  console.log(img.getAttribute('style')); // width: 400px;
 *  ```
 *
 *  Example 3 — bulk cssText, custom properties and deletion:
 *  ```JavaScript
 *  const doc = new DOMParser().parseFromString(
 *      '<div id="d" style="color: red"></div>', 'text/html');
 *  const style = doc.getElementById('d').style;
 *
 *  style.cssText = 'color: red !important; background: blue; color: green';
 *  console.log(style.getPropertyValue('color'));      // red
 *  console.log(style.getPropertyValue('background')); // blue
 *
 *  style.setProperty('--gap', '8px');
 *  delete style.background;
 *  console.log(doc.getElementById('d').getAttribute('style'));
 *  // color: red !important; --gap: 8px;
 *  ```
 *
 */
declare class Class_CSSStyleDeclaration extends Class_object {
    /**
     * @description Queries and sets the textual form of the declaration block
     *
     *      Reading returns the raw text of the style attribute verbatim (original spacing, casing and
     *      trailing semicolon included), not a re-serialized form. Writing parses the string as a
     *      declaration block and replaces all content; an empty or whitespace-only string, or a
     *      string without a single valid declaration, removes the style attribute. Assigning null
     *      removes the attribute as well.
     *
     */
    cssText: string;

    /**
     * @description Returns the number of declarations in the declaration block
     *
     *      Counts the declarations parsed from the style attribute and is 0 when the attribute is
     *      absent or empty. Computed or inherited properties are not included.
     *
     */
    readonly length: number;

    /**
     * @description Returns the property name of the declaration at the specified index
     *
     *      The index follows declaration order. A negative index or an index beyond the last
     *      declaration returns an empty string instead of throwing. Standard names come back
     *      lowercased, custom properties in their original spelling.
     *
     *      @param index the index of the declaration
     *      @return returns the property name, or an empty string if the index is out of range
     *
     */
    item(index: number): string;

    /**
     * @description Queries the value of the specified CSS property
     *
     *      Pass the hyphenated name ('max-width'): the lookup is case-insensitive for standard
     *      properties and case-sensitive for custom properties (--*). A camelCase name is not
     *      converted and returns an empty string because it is not a declared property name. The
     *      value is returned as stored, without normalization.
     *
     *      @param property the CSS property name (hyphenated form, e.g. "max-width"), case-insensitive
     *      @return returns the property value, or an empty string if it is not set
     *
     */
    getPropertyValue(property: string): string;

    /**
     * @description Queries whether the specified CSS property has the !important priority
     *
     *      The lookup follows the same name rules as getPropertyValue. Returns the exact string
     *      'important' when the declaration carries the priority, otherwise an empty string.
     *
     *      @param property the CSS property name (hyphenated form)
     *      @return returns "important" if the !important priority is present, otherwise an empty string
     *
     */
    getPropertyPriority(property: string): string;

    /**
     * @description Sets a CSS property value
     *
     *      The property name must be hyphenated ('max-width'); standard names are lowercased, custom
     *      properties keep their case. An existing declaration is replaced in place, a new one is
     *      appended at the end of the block, and an empty value removes the declaration. Values are
     *      stored verbatim, so fibjs does not validate or normalize CSS values the way browsers do.
     *      The priority accepts only '' or 'important' (case-insensitive); any other value makes the
     *      whole call a no-op.
     *
     *      Example — priority, invalid priority and removal:
     *      ```JavaScript
     *      const doc = new DOMParser().parseFromString('<div id="d"></div>', 'text/html');
     *      const style = doc.getElementById('d').style;
     *
     *      style.setProperty('position', 'absolute', 'important');
     *      console.log(style.getPropertyValue('position'));    // absolute
     *      console.log(style.getPropertyPriority('position')); // important
     *
     *      style.setProperty('position', 'x', 'bogus');        // invalid priority: no-op
     *      console.log(style.getPropertyValue('position'));    // absolute
     *      console.log(style.removeProperty('position'));      // absolute
     *      ```
     *
     *      @param property the CSS property name (hyphenated form); "--" starts a custom property
     *      @param value the CSS property value; an empty value removes the property
     *      @param priority the priority, "important" for !important, empty by default
     *
     */
    setProperty(property: string, value: string, priority?: string): void;

    /**
     * @description Removes the specified CSS property
     *
     *      The lookup follows the same name rules as getPropertyValue. Removing the last declaration
     *      removes the style attribute from the element; removing a missing property is not an error.
     *
     *      @param property the name of the CSS property to remove (hyphenated form)
     *      @return returns the removed property value, or an empty string if the property does not exist
     *
     */
    removeProperty(property: string): string;

    /**
     * @description Accesses declarations through JS-style names (named property access)
     *
     *      Reading `style.maxWidth` converts the camelCase name to 'max-width' and returns its
     *      value, or an empty string when the declaration is absent; `style['max-width']` and
     *      `style['--gap']` work as well, and `style.cssFloat` maps to the CSS `float` property.
     *      Writing converts the name the same way and stores the stringified value; assigning null,
     *      undefined or an empty string removes the declaration, and `delete style.name` removes it
     *      too. Numeric indexes are not mapped (style[0] is undefined, use item()).
     *
     *      Example — camelCase access, cssFloat and deletion:
     *      ```JavaScript
     *      const doc = new DOMParser()
     *          .parseFromString('<img id="i" style="max-width: 100%">', 'text/html');
     *      const style = doc.getElementById('i').style;
     *
     *      console.log(style.maxWidth); // 100%
     *      style.width = '400px';
     *      style.cssFloat = 'left';
     *      console.log(doc.getElementById('i').getAttribute('style'));
     *      // max-width: 100%; width: 400px; float: left;
     *
     *      delete style.width;
     *      console.log(JSON.stringify(style.width)); // ""
     *      ```
     *
     */
    [index: string]: any;

}

