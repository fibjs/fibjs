/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The DOMTokenList object represents a set of space-separated tokens, commonly used for the classList property
 *
 *  DOMTokenList is the interface representing a set of space-separated tokens. It can be used to represent a CSS class list.
 *
 *  Example:
 *  ```JavaScript
 *  var xml = require('xml');
 *  var doc = xml.parse('<div class="foo bar"></div>', 'text/html');
 *  var div = doc.documentElement;
 *  var classList = div.classList;
 *  console.log(classList.length); // 2
 *  console.log(classList.item(0)); // "foo"
 *  console.log(classList.contains("bar")); // true
 *  classList.add("baz");
 *  console.log(div.className); // "foo bar baz"
 *  ```
 *
 */
declare class Class_DOMTokenList extends Class_object {
    /**
     * @description Returns the number of tokens in the set
     */
    readonly length: number;

    /**
     * @description Returns the string representation of all tokens in the set, separated by spaces
     */
    readonly value: string;

    /**
     * @description Returns the token at the specified index
     *      @param index the index of the token
     *      @return returns the token string, or null if the index is out of range
     *
     */
    item(index: number): string;

    /**
     * @description Checks whether the set contains the specified token
     *      @param token the token to check
     *      @return returns true if the token is contained, otherwise false
     *
     */
    contains(token: string): boolean;

    /**
     * @description Adds one or more tokens to the set
     *      @param tokens the tokens to add, variadic parameter
     *
     */
    add(...tokens: any[]): void;

    /**
     * @description Removes one or more tokens from the set
     *      @param tokens the tokens to remove, variadic parameter
     *
     */
    remove(...tokens: any[]): void;

    /**
     * @description Removes the token if it exists, otherwise adds it
     *      @param token the token to toggle
     *      @param force optional. If true, only adds the token; if false, only removes the token
     *      @return returns true if the token is present after the operation, otherwise false
     *
     */
    toggle(token: string, ...force: any[]): boolean;

    /**
     * @description Replaces an existing token with a new token
     *      @param oldToken the token to replace
     *      @param newToken the new token
     *      @return returns true if the replacement succeeded, otherwise false
     *
     */
    replace(oldToken: string, newToken: string): boolean;

    /**
     * @description Returns the string representation of all tokens in the set, separated by spaces
     *      @return returns the token string
     *
     */
    toString(): string;

}

