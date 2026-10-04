/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/**
 * @description URL processing module, providing functions such as URL parsing, formatting, file path conversion and internationalized domain name handling
 *
 * The url module implements complete URL processing functionality and is compatible with the WHATWG URL standard and the traditional URL API.
 * It provides the URL object, the URLSearchParams object and various practical URL manipulation functions.
 *
 * ## Main features
 *
 * - **URL parsing and formatting**: supports parsing and formatting various URL formats
 * - **File URL handling**: provides conversion between file paths and file:// URLs
 * - **Internationalized domain names**: supports conversion between ASCII and Unicode domain names
 * - **Query parameter handling**: integrates URLSearchParams to provide powerful query parameter operations
 * - **Relative path resolution**: supports resolution and merging of relative URLs
 *
 * ## Basic usage
 *
 * ### 1. Create and manipulate URL objects
 *
 * ```JavaScript
 * const { URL, URLSearchParams } = require('url');
 *
 * // Create URL object
 * const myURL = new URL('https://example.com:8080/path?key=value#hash');
 *
 * // Access URL parts
 * console.log(myURL.protocol);  // 'https:'
 * console.log(myURL.hostname);  // 'example.com'
 * console.log(myURL.port);      // '8080'
 * console.log(myURL.pathname);  // '/path'
 * console.log(myURL.search);    // '?key=value'
 * console.log(myURL.hash);      // '#hash'
 *
 * // Modify URL
 * myURL.pathname = '/new-path';
 * myURL.searchParams.set('new-key', 'new-value');
 * console.log(myURL.href);      // 'https://example.com:8080/new-path?key=value&new-key=new-value#hash'
 * ```
 *
 * ### 2. Traditional API compatibility
 *
 * ```JavaScript
 * const url = require('url');
 *
 * // Parse URL string
 * const parsed = url.parse('https://example.com/path?key=value#hash');
 * console.log(parsed.hostname);  // 'example.com'
 *
 * // Format URL object
 * const formatted = url.format({
 *   protocol: 'https:',
 *   hostname: 'example.com',
 *   pathname: '/path'
 * });
 * console.log(formatted);  // 'https://example.com/path'
 *
 * // Resolve relative URL
 * const resolved = url.resolve('https://example.com/foo/', '../bar');
 * console.log(resolved);   // 'https://example.com/bar'
 * ```
 *
 * ### 3. File URL handling
 *
 * ```JavaScript
 * const url = require('url');
 *
 * // Convert path to file URL
 * const fileURL = url.pathToFileURL('/path/to/file.txt');
 * console.log(fileURL.href);  // 'file:///path/to/file.txt'
 *
 * // Convert file URL to path
 * const filePath = url.fileURLToPath('file:///path/to/file.txt');
 * console.log(filePath);      // '/path/to/file.txt'
 * ```
 *
 * ### 4. Internationalized domain name handling
 *
 * ```JavaScript
 * const url = require('url');
 *
 * // Convert Unicode domain to ASCII
 * const ascii = url.domainToASCII('bücher.com');
 * console.log(ascii);         // 'xn--bcher-kva.com'
 *
 * // Convert ASCII domain to Unicode
 * const unicode = url.domainToUnicode('xn--bcher-kva.com');
 * console.log(unicode);       // 'bücher.com'
 * ```
 *
 */
declare module 'url' {
    /**
     * @description constructs a URL string from a URL components object
     *      @param args the URL components object, supporting the fields: protocol, slashes, username, password, hostname, port, pathname, query, hash
     *      @return the constructed URL string
     *
     */
    function format(args: FIBJS.GeneralObject): string;

    /**
     * @description formats a URL object into a string, with formatting options
     *      @param urlObject the URL object to format
     *      @param options formatting options, supporting the fields: fragment (whether to include the fragment), unicode (whether to display domain names in Unicode), auth (whether to include authentication information)
     *      @return the formatted URL string
     *
     */
    function format(urlObject: Class_UrlObject, options?: FIBJS.GeneralObject): string;

    /**
     * @description formats a URL string into a standard URL string
     *      @param href the URL string
     *      @return the formatted URL string
     *
     */
    function format(href: string): string;

    /**
     * @description parses a URL string into a URL object (traditional API)
     *      @param url the URL string to parse
     *      @param parseQueryString whether to parse the query string into an object, default is false
     *      @param slashesDenoteHost whether to parse the string after '//' up to the next '/' as the host, default is false
     *      @return the parsed UrlObject object
     *
     */
    function parse(url: string, parseQueryString?: boolean, slashesDenoteHost?: boolean): Class_UrlObject;

    /**
     * @description resolves a relative URL and merges it into an absolute URL
     *      @param _from the base URL string
     *      @param to the relative URL string to resolve
     *      @return the merged absolute URL string
     *
     */
    function resolve(_from: string, to: string): string;

    /**
     * @description creates a URL object, see UrlObject
     *      @return a new UrlObject instance
     *
     */
    const URL: typeof Class_UrlObject;

    /**
     * @description creates a URLSearchParams object, see URLSearchParams
     *      @return a new URLSearchParams instance
     *
     */
    const URLSearchParams: typeof Class_URLSearchParams;

    /**
     * @description converts a file URL (object or string) into a platform-specific file path
     *      @param url the file URL, a UrlObject or a string (must use the file: protocol)
     *      @param options conversion options; the windows field specifies whether to force the Windows path format
     *      @return the converted file path string
     *
     */
    function fileURLToPath(url: Class_UrlObject | string, options?: FIBJS.GeneralObject): string;

    /**
     * @description converts a file path into a file URL object
     *      @param path the file path to convert
     *      @param options conversion options; the windows field specifies whether the path is in Windows format
     *      @return the converted file URL object
     *
     */
    function pathToFileURL(path: string, options?: FIBJS.GeneralObject): Class_UrlObject;

    /**
     * @description converts an internationalized domain name to ASCII encoding (Punycode)
     *      @param domain the domain name to convert (may contain Unicode characters)
     *      @return the ASCII-encoded domain name
     *
     */
    function domainToASCII(domain: string): string;

    /**
     * @description converts an ASCII-encoded domain name to Unicode display format
     *      @param domain the ASCII domain name to convert
     *      @return the domain name in Unicode format
     *
     */
    function domainToUnicode(domain: string): string;

}

