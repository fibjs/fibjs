/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description Headers is a container class dedicated to handling HTTP header information, inheriting from HttpCollection
 *
 * Headers implements the standard HTTP Headers API and serves both as the global Headers object and as the implementation class of http.Headers. It provides complete HTTP header management functionality, supports standard HTTP header field operations, and inherits all the functionality of HttpCollection, including adding, setting, querying and deleting header fields.
 *
 * Headers supports the following ways of use:
 *
 * 1. Use as the global Headers API (Web standard):
 *
 * ```JavaScript
 * // Create empty Headers object
 * const headers = new Headers();
 *
 * // Initialize with object
 * const headers = new Headers({
 *     'Content-Type': 'application/json',
 *     'Accept': 'application/json'
 * });
 *
 * // Initialize with array
 * const headers = new Headers([
 *     ['Content-Type', 'application/json'],
 *     ['Accept', 'application/json']
 * ]);
 *
 * // Copy from another Headers object
 * const copy = new Headers(headers);
 * ```
 *
 * 2. Use as http.Headers (fibjs extension):
 *
 * ```JavaScript
 * const headers = new http.Headers({
 *     'User-Agent': 'fibjs/1.0',
 *     'Accept': 'text/html'
 * });
 * ```
 *
 * Example of standard Headers API methods:
 *
 * ```JavaScript
 * // Standard Headers API methods
 * headers.set('Content-Type', 'text/html; charset=utf-8');
 * headers.append('Accept', 'application/json');
 * headers.get('Content-Type');  // 'text/html; charset=utf-8'
 * headers.has('Accept');        // true
 * headers.delete('User-Agent');
 *
 * // Iterator support
 * for (const [name, value] of headers) {
 *     console.log(`${name}: ${value}`);
 * }
 *
 * // Iterate over keys
 * for (const name of headers.keys()) {
 *     console.log(name);
 * }
 *
 * // Iterate over values
 * for (const value of headers.values()) {
 *     console.log(value);
 * }
 *
 * // forEach method
 * headers.forEach((value, name) => {
 *     console.log(`${name}: ${value}`);
 * });
 * ```
 *
 * Example of fibjs extension methods (inherited from HttpCollection):
 *
 * ```JavaScript
 * // Add multiple values (without overwriting existing)
 * headers.add('Cache-Control', 'no-cache');
 *
 * // Get first value
 * const auth = headers.first('Authorization');
 *
 * // Get all values
 * const cookies = headers.all('Set-Cookie');
 *
 * // Set multiple cookies
 * headers.set('Set-Cookie', [
 *     'sessionId=abc123; Path=/',
 *     'userId=456; Path=/; HttpOnly'
 * ]);
 * ```
 *
 * Headers automatically handles the case-insensitive nature of header field names, fully following the HTTP protocol specification and the Web standard Headers API.
 *
 */
declare class Class_Headers extends Class_HttpCollection {
    /**
     * @description Headers constructor, creates a new empty HTTP headers container
     */
    constructor();

    /**
     * @description Headers constructor, initializes the HTTP headers container from an object, an array of pairs or another Headers
     *      @param init the initial headers: an object whose keys are header field names and values are header field values, an array whose elements are [name, value] pairs, or another Headers container to copy
     *
     */
    constructor(init: FIBJS.GeneralObject | any[] | Class_Headers);

    /**
     * @description returns an array composed of all Set-Cookie header values
     *      @return returns an array containing all Set-Cookie values
     *
     */
    getSetCookie(): string[];

}

