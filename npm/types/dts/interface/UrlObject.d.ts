/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/**
 * @description URL object, implements the WHATWG URL standard, used to parse, construct and manipulate URLs
 *
 * UrlObject provides complete URL processing functionality and is compatible with the modern Web standard URL API. It supports parsing, constructing, modifying and formatting URLs, and provides a rich set of properties and methods to operate on the various parts of a URL.
 *
 * ## Main features
 *
 * - **Standard compatibility**: implements the WHATWG URL standard
 * - **Unicode support**: full support for internationalized domain names (IDN) and Unicode characters
 * - **Query parameters**: integrates URLSearchParams to provide powerful query parameter operations
 * - **Path handling**: automatically handles path normalization and relative path resolution
 *
 * ## Creating a URL object
 *
 * ### 1. Create with a string
 *
 * ```JavaScript
 * const url = require('url');
 *
 * // Use full URL string
 * const myURL = new URL('https://user:pass@example.com:8080/path?query=value#hash');
 *
 * // Use relative URL and base URL
 * const relativeURL = new URL('/api/users', 'https://example.com');
 * console.log(relativeURL.href); // 'https://example.com/api/users'
 * ```
 *
 * ### 2. Construct with an object
 *
 * ```JavaScript
 * const myURL = new URL({
 *   protocol: 'https:',
 *   hostname: 'example.com',
 *   port: '8080',
 *   pathname: '/api/data',
 *   search: '?format=json'
 * });
 * ```
 *
 * ## URL components
 *
 * A complete URL consists of the following parts:
 * ```
 * https://user:pass@example.com:8080/path/to/resource?query=value#fragment
 *  \___/   \______/ \_________/ \__/\________________/\___________/ \______/
 *    |        |         |        |          |             |          |
 * protocol   auth      host     port     pathname        search      hash
 *           \___________________/
 *                    origin
 * ```
 *
 * ## Common methods
 *
 * ```JavaScript
 * const myURL = new URL('https://example.com/old-path');
 *
 * // Parse URL string
 * const parsed = URL.parse('https://example.com/path');
 *
 * // Check if URL is valid
 * const isValid = URL.canParse('https://example.com');
 *
 * // Redirect to new path
 * const newURL = myURL.resolve('../new-path');
 * ```
 *
 */
declare class Class_UrlObject extends Class_object {
    /**
     * @description constructs a URL object from an arguments object
     *      @param args the construction arguments object, supporting the fields: protocol, slashes, username, password, hostname, port, pathname, query, hash
     *
     */
    constructor(args?: FIBJS.GeneralObject);

    /**
     * @description constructs a URL object from a URL string
     *      @param url the URL string to parse, which can be an absolute or relative URL
     *      base is used when url is a relative URL; it may be a URL string, a UrlObject, or a URL components object (the same fields the UrlObject constructor accepts).
     *      @param base the base URL
     *
     */
    constructor(url: string, base?: string | Class_UrlObject | FIBJS.GeneralObject);

    /**
     * @description parses a URL string and returns a URL object, or null if parsing fails
     *      @param url the URL string to parse
     *      @param base the base URL string, used when url is a relative URL
     *      @return returns a UrlObject on success, or null if parsing fails
     *
     */
    static parse(url: string, base?: string): Class_UrlObject;

    /**
     * @description checks whether a URL string can be parsed successfully
     *      @param url the URL string to check
     *      @param base the base URL string, used when url is a relative URL
     *      @return returns true if it can be parsed, otherwise returns false
     *
     */
    static canParse(url: string, base?: string): boolean;

    /**
     * @description resolves a relative URL and returns a new absolute URL object
     *      @param url the relative or absolute URL string to resolve
     *      @return returns the new resolved UrlObject object
     *
     */
    resolve(url: string): Class_UrlObject;

    /**
     * @description the complete URL string
     *
     *      Gets or sets the complete URL string. Setting this property automatically parses and updates the other properties.
     *
     */
    href: string;

    /**
     * @description the protocol part of the URL (including the colon)
     *
     *      For example: 'http:', 'https:', 'ftp:', 'file:', etc.
     *
     */
    protocol: string;

    /**
     * @description whether double slashes are included
     *
     *      Indicates whether the URL uses the double-slash format (such as http://)
     *
     */
    slashes: boolean;

    /**
     * @description the origin of the URL (protocol + host + port)
     *
     *      Read-only property, returned in a format such as: 'https://example.com:8080'
     *      Returns 'null' for non-network protocols (such as file:)
     *
     */
    readonly origin: string;

    /**
     * @description authentication information (username:password)
     *
     *      Read-only property, returned in a format such as: 'username:password'
     *
     */
    readonly auth: string;

    /**
     * @description the username part
     *
     *      The user name in the URL, used for HTTP basic authentication
     *
     */
    username: string;

    /**
     * @description the password part
     *
     *      The password in the URL, used for HTTP basic authentication
     *
     */
    password: string;

    /**
     * @description the host part (host name + port)
     *
     *      Contains the host name and port number, in a format such as: 'example.com:8080'
     *
     */
    host: string;

    /**
     * @description the host name part
     *
     *      The host name without the port number; supports IPv4, IPv6 and domain names
     *
     */
    hostname: string;

    /**
     * @description the port number
     *
     *      The port number as a string; an empty string means the default port is used
     *
     */
    port: string;

    /**
     * @description the complete path (path + query string)
     *
     *      Read-only property, containing pathname and search, in a format such as: '/path?query=value'
     *
     */
    readonly path: string;

    /**
     * @description the path part of the URL
     *
     *      The path part of the URL, always starting with '/'
     *
     */
    pathname: string;

    /**
     * @description the query string (including the question mark)
     *
     *      In a format such as: '?key1=value1&key2=value2'; an empty string when there is no query
     *
     */
    search: string;

    /**
     * @description the query parameter value
     *
     *      Can be a string or an object; setting an object automatically serializes it into a query string
     *
     */
    query: any;

    /**
     * @description the URL fragment identifier (including the hash sign)
     *
     *      In a format such as: '#section'; an empty string when there is no fragment
     *
     */
    hash: string;

    /**
     * @description the URL query parameters object
     *
     *      Read-only property, returns a URLSearchParams object for manipulating query parameters
     *      Two-way bound to the URL object; modifications automatically update the search and query properties
     *
     */
    readonly searchParams: Class_URLSearchParams;

}

