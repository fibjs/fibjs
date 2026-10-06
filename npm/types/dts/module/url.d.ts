/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/**
 * @description The url module parses, formats and resolves URLs; it provides the WHATWG URL and URLSearchParams classes together with the legacy UrlObject API, file path conversion and internationalized domain name conversion
 *
 * The WHATWG classes are aliases of the global `URL` and `URLSearchParams`, and the legacy
 * functions (parse, format and resolve) are kept for Node.js compatibility.
 *
 * Concepts:
 *
 * - **Two APIs**: `URL` implements the WHATWG URL standard and is the recommended API;
 *   `parse` returns the legacy UrlObject with the fields protocol, slashes, auth, host, port,
 *   hostname, hash, search, query, pathname, path and href. Node.js deprecates the legacy
 *   functions (DEP0169) because their parsing is not standardized.
 * - **Encoding rules**: the WHATWG parser percent-encodes characters that are invalid in the
 *   current component and normalizes the host (lower case, internationalized names to ASCII);
 *   the query serializer writes a space as `+`. The legacy formatter applies the same
 *   encoding to component values, encodes user names and passwords, and leaves an already
 *   encoded string unchanged.
 * - **Relative resolution**: a relative reference is resolved against a base URL with the
 *   standard algorithm; resolve and `new URL(relative, base)` share that behavior.
 * - **Query parameters**: `URL#searchParams` is a live URLSearchParams view and changing it
 *   rewrites the URL; `url.parse(str, true)` returns a legacy object whose query is a
 *   URLSearchParams instead of the raw string.
 * - **File URLs**: pathToFileURL and fileURLToPath convert between platform paths and
 *   `file:` URLs, percent-encoding or decoding the path and rejecting a non-empty host on
 *   POSIX.
 * - **Internationalized domain names**: domainToASCII and domainToUnicode convert a domain
 *   between Unicode and the ACE (`xn--`) form with the UTS #46 mapping; `URL` applies the
 *   same conversion to hosts automatically.
 *
 * Import:
 * ```JavaScript
 * const url = require('url'); // URL and URLSearchParams are also global
 * ```
 *
 * Example 1 — parse and modify a URL with the WHATWG API:
 * ```JavaScript
 * const url = require('url');
 *
 * const myURL = new url.URL('https://example.com:8080/path?key=value#hash');
 * console.log(myURL.protocol, myURL.hostname, myURL.port); // https: example.com 8080
 * console.log(myURL.pathname, myURL.search, myURL.hash); // /path ?key=value #hash
 *
 * myURL.pathname = '/new-path';
 * myURL.searchParams.set('q', 'a b');
 * console.log(myURL.href); // https://example.com:8080/new-path?key=value&q=a+b
 * ```
 *
 * Example 2 — the legacy parse/format/resolve API:
 * ```JavaScript
 * const url = require('url');
 *
 * const parsed = url.parse('https://user:pass@example.com:8080/p/a?q=1#frag');
 * console.log(parsed.hostname, parsed.port, parsed.path); // example.com 8080 /p/a?q=1
 *
 * const formatted = url.format({
 *     protocol: 'https:',
 *     hostname: 'example.com',
 *     pathname: '/p'
 * });
 * console.log(formatted); // https://example.com/p
 *
 * console.log(url.resolve('https://example.com/foo/', '../bar'));
 * // https://example.com/bar
 * ```
 *
 * Example 3 — convert between file paths and file URLs:
 * ```JavaScript
 * const url = require('url');
 *
 * const fileURL = url.pathToFileURL('/tmp/fibjs url test.txt');
 * console.log(fileURL.href); // file:///tmp/fibjs%20url%20test.txt
 * console.log(url.fileURLToPath(fileURL)); // /tmp/fibjs url test.txt
 * ```
 *
 * Example 4 — convert internationalized domain names:
 * ```JavaScript
 * const url = require('url');
 *
 * console.log(url.domainToASCII('mañana.com')); // xn--maana-pta.com
 * console.log(url.domainToUnicode('xn--maana-pta.com')); // mañana.com
 * console.log(url.domainToASCII('example.com')); // example.com, unchanged
 * ```
 *
 */
declare module 'url' {
    /**
     * @description Constructs a URL string from a URL components object, a UrlObject or a URL string
     *
     *      The overloads share this name and differ in the accepted input; this first entry
     *      documents the differences so they stay visible in `fibjs --man`:
     *
     *      - format(args) builds a URL from a components object. Accepted fields are protocol,
     *        slashes, auth, username, password, host, hostname, port, pathname, path, query
     *        (string, array of pairs or plain object), search and hash. When hostname is present
     *        but protocol is missing, `http:` is assumed, so http.request-style options can be
     *        formatted directly; user names and passwords are percent-encoded.
     *      - format(urlObject, options) serializes a UrlObject, a URL string or a components
     *        object; `options` is accepted for Node.js compatibility (fragment, unicode, auth)
     *        but is currently ignored and the full href is returned.
     *      - format(href) parses href with the WHATWG parser and returns the normalized href; a
     *        relative string is normalized as an absolute path (`not a url` becomes
     *        `/not%20a%20url`) while an invalid absolute URL throws a type error.
     *
     *      Prefer `new URL(...).href` for new code; this function is the legacy formatter.
     *
     *      Example — format a components object:
     *      ```JavaScript
     *      const url = require('url');
     *
     *      console.log(url.format({
     *          protocol: 'https:',
     *          hostname: 'example.com',
     *          pathname: '/p',
     *          query: { a: 1 }
     *      })); // https://example.com/p?a=1
     *      ```
     *
     *      @param args URL components object to format
     *      @return the constructed URL string
     *
     */
    function format(args: FIBJS.GeneralObject): string;

    /**
     * @description Formats a UrlObject, a URL string or a URL components object into a string
     *
     *      See the first format overload for the differences between the call forms. urlObject
     *      may be a UrlObject, a URL string (parsed first) or a components object (the same
     *      fields the UrlObject constructor accepts).
     *
     *      @param urlObject URL to format
     *      @param options Node.js formatting options; accepted but currently ignored
     *      @return the formatted URL string
     *
     */
    function format(urlObject: Class_UrlObject | string | FIBJS.GeneralObject, options?: FIBJS.GeneralObject): string;

    /**
     * @description Parses a URL string with the WHATWG parser and returns the normalized href
     *
     *      An absolute URL is normalized component by component; a string without a scheme is
     *      treated as a relative reference and normalized as an absolute path. An invalid
     *      absolute URL throws a type error (20024).
     *
     *      @param href URL string to normalize
     *      @return the normalized URL string
     *
     */
    function format(href: string): string;

    /**
     * @description Parses a URL string into a legacy UrlObject
     *
     *      The returned object follows the Node.js legacy API with the fields protocol, slashes,
     *      auth, host, port, hostname, hash, search, query, pathname, path and href. By default
     *      query is the raw query text without the leading `?`; with parseQueryString it is a
     *      URLSearchParams built from the query. The slashesDenoteHost argument is accepted for
     *      compatibility but ignored, so a protocol-relative reference such as `//host/path` is
     *      parsed with `host` as the host. Invalid percent-encoding throws `url: URI malformed`
     *      and an unparsable URL throws `url: Invalid URL '<input>'.` (20024). Node.js returns a
     *      plain object when parseQueryString is true and deprecates the function (DEP0169).
     *
     *      Example — legacy parse with and without query parsing:
     *      ```JavaScript
     *      const url = require('url');
     *
     *      const parsed = url.parse('https://example.com/a?x=1&x=2#top');
     *      console.log(parsed.hostname, parsed.query, parsed.hash); // example.com x=1&x=2 #top
     *
     *      const withQuery = url.parse('https://example.com/a?x=1&x=2', true);
     *      console.log(withQuery.query.getAll('x')); // [ '1', '2' ]
     *      ```
     *
     *      @param url URL string to parse
     *      @param parseQueryString parse the query into a URLSearchParams, default false
     *      @param slashesDenoteHost accepted for Node.js compatibility; ignored by fibjs
     *      @return the parsed UrlObject
     *
     */
    function parse(url: string, parseQueryString?: boolean, slashesDenoteHost?: boolean): Class_UrlObject;

    /**
     * @description Resolves a relative reference against a base URL
     *
     *      The two arguments are `from` (the base URL) and `to` (the reference); the result is
     *      normalized with the WHATWG algorithm. An empty base resolves the reference on its own,
     *      so a rooted path stays a path and a relative path is returned as a relative path. An
     *      unparsable input throws `url: Invalid URL '<input>'.` (20024). Node.js deprecates this
     *      function (DEP0169).
     *
     *      Example — resolve against a file base:
     *      ```JavaScript
     *      const url = require('url');
     *
     *      console.log(url.resolve('https://example.com/a/b', '../c')); // https://example.com/c
     *      console.log(url.resolve('', '/a/b')); // /a/b
     *      ```
     *
     *      @param _from base URL string
     *      @param to relative URL string to resolve
     *      @return the resolved absolute URL string
     *
     */
    function resolve(_from: string, to: string): string;

    /**
     * @description The WHATWG URL class, re-exported from the global scope
     *
     *      `url.URL` is the same class as the global `URL`; `new url.URL(...)` produces a
     *      UrlObject with the standard properties and methods, including the live `searchParams`
     *      view. Prefer this class over parse/format for new code.
     *
     *      Example — the alias and the resulting class:
     *      ```JavaScript
     *      const url = require('url');
     *
     *      console.log(url.URL === URL); // true
     *      console.log(new url.URL('https://example.com/a').constructor.name); // UrlObject
     *      ```
     *
     *      @return a new UrlObject instance
     *
     */
    const URL: typeof Class_UrlObject;

    /**
     * @description The WHATWG URLSearchParams class, re-exported from the global scope
     *
     *      `url.URLSearchParams` is the same class as the global `URLSearchParams` and derives
     *      from HttpCollection; see URLSearchParams for the query parameter container.
     *
     *      @return a new URLSearchParams instance
     *
     */
    const URLSearchParams: typeof Class_URLSearchParams;

    /**
     * @description Converts a file URL into a platform-specific file path
     *
     *      url may be a UrlObject, a URL string or a components object (the same fields the
     *      UrlObject constructor accepts). The scheme must be `file:`; the path is
     *      percent-decoded and, on Windows, a drive letter or UNC host is handled. A non-empty
     *      host on POSIX throws. The `windows` option forces the Windows or POSIX path format.
     *      Errors carry a code: ERR_INVALID_URL_SCHEME for a non-file URL,
     *      ERR_INVALID_FILE_URL_HOST for a host on POSIX and ERR_INVALID_FILE_URL_PATH for an
     *      invalid path such as one containing an encoded slash.
     *
     *      Example — round-trip a path that contains spaces:
     *      ```JavaScript
     *      const url = require('url');
     *
     *      const href = url.pathToFileURL('/tmp/a b.txt').href;
     *      console.log(href); // file:///tmp/a%20b.txt
     *      console.log(url.fileURLToPath(href)); // /tmp/a b.txt
     *      ```
     *
     *      @param url file URL to convert
     *      @param options conversion options; the windows field forces the Windows path format
     *      @return the platform-specific file path
     *
     */
    function fileURLToPath(url: Class_UrlObject | string | FIBJS.GeneralObject, options?: FIBJS.GeneralObject): string;

    /**
     * @description Converts a file path into a file URL object
     *
     *      The path is resolved to an absolute path and percent-encoded with the path rules; a
     *      trailing path separator is preserved as a trailing slash, which marks the URL as a
     *      directory for the consumer. On Windows the `windows` option forces Windows handling
     *      and a UNC path becomes the URL host. The result is a UrlObject, so read its href for
     *      the string form.
     *
     *      Example — build a URL and read its href:
     *      ```JavaScript
     *      const url = require('url');
     *
     *      const fileURL = url.pathToFileURL('/tmp/a b.txt');
     *      console.log(fileURL.href); // file:///tmp/a%20b.txt
     *      ```
     *
     *      @param path file path to convert
     *      @param options conversion options; the windows field forces Windows path handling
     *      @return the converted file URL object
     *
     */
    function pathToFileURL(path: string, options?: FIBJS.GeneralObject): Class_UrlObject;

    /**
     * @description Converts an internationalized domain name to its ASCII (ACE) form
     *
     *      The conversion applies the UTS #46 mapping of the URL parser and adds the `xn--`
     *      prefix to non-ASCII labels; an ASCII name is returned unchanged. Unsupported input is
     *      returned unchanged instead of raising an error, while Node.js returns an empty string
     *      for a domain that fails the conversion. Use punycode.toASCII for the raw RFC 3492
     *      conversion of a single label.
     *
     *      Example — convert a Unicode domain:
     *      ```JavaScript
     *      const url = require('url');
     *
     *      console.log(url.domainToASCII('mañana.com')); // xn--maana-pta.com
     *      ```
     *
     *      @param domain domain name to convert, possibly containing Unicode characters
     *      @return the ASCII (ACE) form of the domain name
     *
     */
    function domainToASCII(domain: string): string;

    /**
     * @description Converts an ASCII (ACE) domain name to its Unicode display form
     *
     *      Only labels that carry the `xn--` prefix are converted; all other labels are copied
     *      unchanged. The prefix test is case-sensitive: an upper-case `XN--` label is returned
     *      as is, while Node.js converts it. Invalid encoded labels are returned unchanged.
     *
     *      Example — convert an ACE domain:
     *      ```JavaScript
     *      const url = require('url');
     *
     *      console.log(url.domainToUnicode('xn--maana-pta.com')); // mañana.com
     *      ```
     *
     *      @param domain ASCII domain name to convert
     *      @return the domain name in Unicode form
     *
     */
    function domainToUnicode(domain: string): string;

}

