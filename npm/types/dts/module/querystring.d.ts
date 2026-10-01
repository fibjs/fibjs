/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description the querystring module provides some utility functions for parsing and serializing URL query parameters; with the querystring module, URL query parameters can be conveniently parsed into objects or strings, and objects can be serialized into URL query parameter strings
 *
 * The commonly used functions of the `querystring` module are as follows:
 *
 * - `querystring.parse(str[, sep[, eq[, options]]])`: parses URL query parameters into an object
 * - `querystring.stringify(obj[, sep[, eq[, options]]])`: serializes an object into a URL query parameter string
 *
 * Here, `str` is the URL query parameter string to parse and `obj` is the object to serialize.
 *
 * The following example shows how to use the `querystring` module to parse query parameters from a URL into an object:
 *
 * ```JavaScript
 * const querystring = require('querystring');
 *
 * const url = 'https://www.example.com/path/to/page?foo=bar&baz=qux';
 *
 * const search = new URL(url).search; // return '?foo=bar&baz=qux'
 * const query = querystring.parse(search.slice(1)); // parse query string
 *
 * console.log(query); // output { foo: 'bar', baz: 'qux' }
 * ```
 *
 * The code above first obtains a URL, then extracts the query parameter part from it, parses it into an object with the `querystring.parse()` function, and finally prints the object.
 *
 * Next, the example shows how to use the `querystring` module to serialize an object into a URL query parameter string:
 *
 * ```JavaScript
 * const querystring = require('querystring');
 *
 * const obj = {
 *   foo: 'bar',
 *   baz: 'qux'
 * };
 *
 * const query = querystring.stringify(obj);
 *
 * console.log(query); // output "foo=bar&baz=qux"
 * ```
 *
 * In the code above, an object is first defined, then serialized into a URL query parameter string with the `querystring.stringify()` function, and finally the string is printed.
 *
 * As can be seen, the `querystring` module makes it convenient to parse and serialize URL query parameters, reducing tedious string handling and improving code readability and maintainability.
 *
 */
declare module 'querystring' {
    /**
     * @description safely encodes a url component string
     *      @param str the url to encode
     *      @return returns the encoded string
     *
     */
    function escape(str: string): string;

    /**
     * @description safely decodes a url string
     *      @param str the url to decode
     *      @return returns the decoded string
     *
     */
    function unescape(str: string): string;

    /**
     * @description parses a query string
     *      @param str the string to parse
     *      @param sep the separator string used when parsing, default is &
     *      @param eq the assignment string used when parsing, default is =
     *      @param opt parse options, not supported yet
     *      @return returns the decoded object
     *
     */
    function parse(str: string, sep?: string, eq?: string, opt?: FIBJS.GeneralObject): Class_HttpCollection;

    /**
     * @description serializes an object into a query string
     *      @param obj the object to serialize
     *      @param sep the separator string used when serializing, default is &
     *      @param eq the assignment string used when serializing, default is =
     *      @param opt parse options, not supported yet
     *      @return returns the serialized string
     *
     */
    function stringify(obj: FIBJS.GeneralObject, sep?: string, eq?: string, opt?: FIBJS.GeneralObject): string;

}

