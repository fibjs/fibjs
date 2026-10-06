/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The DOMTokenList object represents a set of space-separated tokens, commonly
 *  used for the classList property of an element
 *
 *  DOMTokenList is the interface representing a set of space-separated tokens. In fibjs it
 *  wraps the class attribute of an element: reading re-parses the attribute and the
 *  mutation methods write the token set back, so the object is a live view rather than a
 *  copy. Token matching is case-sensitive. The interface is not constructible and not
 *  exported as a global (typeof DOMTokenList is undefined); the only way to obtain one is
 *  `element.classList` in HTML mode, while reading classList on an XML element throws an
 *  invalid-call error (20009).
 *
 *  Concepts:
 *
 *  - **Token parsing**: tokens are separated by space, tab, line feed, carriage return or
 *    form feed; leading, trailing and repeated separators are ignored and empty tokens are
 *    dropped. value keeps the raw attribute string while length and item() report the
 *    parsed view, so a class attribute ` b   a ` yields value `" b   a "` but two tokens,
 *    b and a.
 *  - **Live view**: the same DOMTokenList object is cached per element and every read
 *    re-parses the current class attribute. Changing className or the class attribute
 *    through the attribute methods is immediately visible through the list, and the
 *    mutation methods write the normalized token set back to the attribute (single spaces
 *    between tokens, no leading or trailing separator).
 *  - **Differences from MDN**: fibjs implements only the class-attribute use case.
 *    supports(), forEach(), keys(), values(), entries() and the iteration protocol are not
 *    available, so use length, item() and indexed access. add() silently ignores empty
 *    tokens and tokens containing whitespace, where the standard throws a
 *    SyntaxError/InvalidCharacterError. toggle() and replace() perform no validation at
 *    all: an empty token can be written (it is invisible to later parses) and a token
 *    containing spaces is written verbatim (it parses as several tokens later). value is
 *    read-only here, while the standard defines it as settable.
 *
 *  Obtained from:
 *  - `element.classList` — a cached, live DOMTokenList for the class attribute; HTML mode
 *    only. There is no constructor and no module export.
 *
 *  Example 1 — read and inspect the tokens of a class attribute:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<html><body><p class="note  wide">Hi</p></body></html>',
 *      'text/html');
 *  const para = doc.querySelector('p');
 *  const list = para.classList;
 *
 *  console.log(list.value);            // note  wide
 *  console.log(list.length);           // 2
 *  console.log(list.item(0));          // note
 *  console.log(list[1]);               // wide
 *  console.log(list.item(2));          // null
 *  console.log(list.contains('wide')); // true
 *  ```
 *
 *  Example 2 — change the class of an element through the list:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<html><body><button class="btn">Go</button></body></html>',
 *      'text/html');
 *  const button = doc.querySelector('button');
 *
 *  button.classList.add('primary', 'btn'); // the duplicate is ignored
 *  button.classList.remove('btn');
 *  button.classList.toggle('disabled');
 *  console.log(button.className); // primary disabled
 *  console.log(String(button));   // <button class="primary disabled">Go</button>
 *  ```
 *
 *  Example 3 — the view is live and className stays authoritative:
 *  ```JavaScript
 *  const xml = require('xml');
 *
 *  const doc = xml.parse('<html><body><div class="a b"></div></body></html>',
 *      'text/html');
 *  const div = doc.querySelector('div');
 *  const list = div.classList;
 *
 *  console.log(list === div.classList); // true, the object is cached
 *
 *  div.className = 'x y z';             // an external change is picked up
 *  console.log(list.length);            // 3
 *  console.log(list.toggle('y'));       // false, y was removed
 *  console.log(div.className);          // x z
 *  ```
 *
 */
declare class Class_DOMTokenList extends Class_object {
    /**
     * @description Returns the token at the specified index, or undefined when the index is
     *      out of range
     *
     *      The index is 0-based and applied to the parsed token set at the time of the read.
     *      Unlike item(), an out-of-range index yields undefined rather than null.
     *
     */
    [index: number]: string;

    /**
     * @description Returns the number of tokens in the set
     *
     *      The value is recomputed on every read from the current class attribute, so it
     *      reflects external changes to className or to the class attribute. Empty tokens and
     *      duplicate separators are not counted.
     *
     */
    readonly length: number;

    /**
     * @description Returns the raw class attribute string
     *
     *      Contrary to the standard the property is read-only (an assignment is silently
     *      ignored) and the string is returned verbatim, including repeated or leading
     *      separators: it is not the normalized form that the mutation methods write.
     *      An empty string is returned when the class attribute is absent. toString() and
     *      String(list) return the same value.
     *
     */
    readonly value: string;

    /**
     * @description Returns the token at the specified index
     *
     *      The index is 0-based; a negative index or an index greater than or equal to length
     *      returns null. A numeric string is accepted and converted.
     *
     *      @param index the index of the token
     *      @return returns the token string, or null if the index is out of range
     *
     */
    item(index: number): string;

    /**
     * @description Checks whether the set contains the specified token
     *
     *      The comparison is case-sensitive and exact; an empty string never matches. A value
     *      that is not a string throws a type error (20005) instead of being converted.
     *
     *      @param token the token to check
     *      @return returns true if the token is contained, otherwise false
     *
     */
    contains(token: string): boolean;

    /**
     * @description Adds one or more tokens to the set
     *
     *      Tokens are appended in argument order and are written back to the class attribute
     *      separated by single spaces. Tokens that are empty, already present or contain
     *      whitespace are silently skipped (the standard throws for the last two cases).
     *      Arguments are converted with the usual type checks, so a number throws 20005.
     *
     *      Example — add tokens and observe the normalized attribute:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<html><body><div class="a b"></div></body></html>',
     *          'text/html');
     *      const div = doc.querySelector('div');
     *
     *      div.classList.add('c', 'a', '', 'x y');
     *      console.log(div.className); // a b c
     *      ```
     *
     *      @param tokens the tokens to add, variadic parameter
     *
     */
    add(...tokens: any[]): void;

    /**
     * @description Removes one or more tokens from the set
     *
     *      Each named token is removed once (naming a duplicated token removes a single
     *      occurrence) and unknown tokens are ignored; the remaining tokens are written back
     *      separated by single spaces. Removing the last token leaves an empty class
     *      attribute. Arguments are converted with the usual type checks, so a number throws
     *      20005.
     *
     *      @param tokens the tokens to remove, variadic parameter
     *
     */
    remove(...tokens: any[]): void;

    /**
     * @description Removes the token if it exists, otherwise adds it
     *
     *      Without force the return value tells whether the token is present after the call.
     *      With force the operation only adds (truthy) or only removes (falsy); the value is
     *      coerced with the usual boolean rules, so any non-empty string means true. fibjs
     *      performs no token validation here: an empty token is appended as-is and a token
     *      containing whitespace is written verbatim.
     *
     *      Example — conditional toggling with the force argument:
     *      ```JavaScript
     *      const xml = require('xml');
     *
     *      const doc = xml.parse('<html><body><div class="menu"></div></body></html>',
     *          'text/html');
     *      const div = doc.querySelector('div');
     *
     *      div.classList.toggle('menu', true);  // already there, no change
     *      div.classList.toggle('open', true);  // force add
     *      console.log(div.className);          // menu open
     *      console.log(div.classList.toggle('open', false)); // false
     *      console.log(div.className);          // menu
     *      ```
     *
     *      @param token the token to toggle
     *      @param force optional. If true, only adds the token; if false, only removes the token
     *      @return returns true if the token is present after the operation, otherwise false
     *
     */
    toggle(token: string, ...force: any[]): boolean;

    /**
     * @description Replaces an existing token with a new token
     *
     *      Returns false and leaves the set unchanged when oldToken is absent. When newToken
     *      is already present at another position, oldToken is simply removed; otherwise
     *      oldToken is overwritten in place, so its position is preserved. No token validation
     *      is performed: newToken is written verbatim even when it contains whitespace.
     *      Argument type errors throw 20005.
     *
     *      @param oldToken the token to replace
     *      @param newToken the new token
     *      @return returns true if the replacement succeeded, otherwise false
     *
     */
    replace(oldToken: string, newToken: string): boolean;

    /**
     * @description Returns the string representation of the set
     *
     *      Equivalent to value: the raw class attribute string, not a normalized token list.
     *      Called implicitly by string concatenation and by String(list).
     *
     *      @return returns the token string
     *
     */
    toString(): string;

}

