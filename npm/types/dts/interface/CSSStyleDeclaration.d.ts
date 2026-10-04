/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The CSSStyleDeclaration object represents a CSS declaration block, commonly used for the style property of an element
 *
 *  CSSStyleDeclaration is the programmatic access interface to the style property (inline styles) of an element.
 *
 *  Example:
 *  ```JavaScript
 *  var doc = new DOMParser().parseFromString('<div></div>', 'text/html');
 *  var el = doc.documentElement;
 *
 *  // Set inline styles in bulk through cssText
 *  el.style.cssText = 'max-width: 100%; height: auto;';
 *
 *  // Read and write individual properties; property names use camelCase, consistent with browsers
 *  el.style.width = '400px';
 *  console.log(el.style.width);        // "400px"
 *  console.log(el.getAttribute('style')); // "max-width: 100%; height: auto; width: 400px"
 *
 *  // Operate on declarations using the standard methods
 *  el.style.setProperty('display', 'none', 'important');
 *  console.log(el.style.getPropertyValue('display')); // "none"
 *  console.log(el.style.getPropertyPriority('display')); // "important"
 *  el.style.removeProperty('display');
 *
 *  // Custom properties (CSS variables) are also supported
 *  el.style.setProperty('--gap', '8px');
 *  console.log(el.style.getPropertyValue('--gap')); // "8px"
 *  ```
 *
 */
declare class Class_CSSStyleDeclaration extends Class_object {
    /**
     * @description Queries and sets the textual form of the declaration block. When queried, returns the current style property value; when set, parses it as a CSS declaration block and replaces all content
     *
     */
    cssText: string;

    /**
     * @description Returns the number of declarations in the declaration block
     */
    readonly length: number;

    /**
     * @description Returns the property name of the declaration at the specified index
     *      @param index the index of the declaration
     *      @return returns the property name, or an empty string if the index is out of range
     *
     */
    item(index: number): string;

    /**
     * @description Queries the value of the specified CSS property
     *      @param property the CSS property name (hyphenated form, e.g. "max-width"), case-insensitive
     *      @return returns the property value, or an empty string if it is not set
     *
     */
    getPropertyValue(property: string): string;

    /**
     * @description Queries whether the specified CSS property has the !important priority
     *      @param property the CSS property name (hyphenated form)
     *      @return returns "important" if the !important priority is present, otherwise an empty string
     *
     */
    getPropertyPriority(property: string): string;

    /**
     * @description Sets a CSS property value
     *
     *      When setting an existing property, its value is replaced in place; a new property is appended to the end of the declaration block. An empty value is equivalent to removing the property.
     *      @param property the CSS property name (hyphenated form); custom properties starting with "--" are also supported
     *      @param value the CSS property value; an empty value removes the property
     *      @param priority the priority, which can be set to "important" to mean !important, default is empty
     *
     */
    setProperty(property: string, value: string, priority?: string): void;

    /**
     * @description Removes the specified CSS property
     *      @param property the name of the CSS property to remove (hyphenated form)
     *      @return returns the removed property value, or an empty string if the property does not exist
     *
     */
    removeProperty(property: string): string;

    /**
     * @description Supports accessing CSS properties with camelCase property names, such as style.width and style.maxWidth
     *
     */
    [index: string]: any;

}

