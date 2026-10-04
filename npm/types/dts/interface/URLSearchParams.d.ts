/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description URLSearchParams is a container class dedicated to handling URL query parameters, inheriting from HttpCollection
 *
 * URLSearchParams implements the standard URLSearchParams API, used to parse and manipulate URL query strings. It provides complete query parameter management functionality, supports standard query parameter operations, and inherits all the functionality of HttpCollection, including adding, setting, querying and deleting parameters.
 *
 * URLSearchParams supports the following ways of use:
 *
 * 1. Use as the global URLSearchParams API (Web standard):
 *
 * ```JavaScript
 * // Create empty URLSearchParams object
 * const params = new URLSearchParams();
 *
 * // Initialize with query string
 * const params = new URLSearchParams('name=John&age=30&city=Beijing');
 *
 * // Initialize with object
 * const params = new URLSearchParams({
 *     name: 'John',
 *     age: '30',
 *     city: 'Beijing'
 * });
 *
 * // Initialize with array
 * const params = new URLSearchParams([
 *     ['name', 'John'],
 *     ['age', '30'],
 *     ['city', 'Beijing']
 * ]);
 *
 * // Copy from another URLSearchParams object
 * const copy = new URLSearchParams(params);
 * ```
 *
 * Example of standard URLSearchParams API methods:
 *
 * ```JavaScript
 * // Standard URLSearchParams API methods
 * params.set('name', 'Alice');
 * params.append('hobby', 'reading');
 * params.append('hobby', 'coding');  // Support multiple parameters with same name
 * params.get('name');        // 'Alice'
 * params.getAll('hobby');    // ['reading', 'coding']
 * params.has('age');         // true
 * params.has('hobby', 'reading'); // true
 * params.delete('city');
 * params.delete('hobby', 'coding');
 *
 * // Convert to string
 * params.toString();         // 'name=Alice&hobby=reading&hobby=coding'
 *
 * // Iterator support
 * for (const [name, value] of params) {
 *     console.log(`${name}: ${value}`);
 * }
 *
 * // Iterate over keys
 * for (const name of params.keys()) {
 *     console.log(name);
 * }
 *
 * // Iterate over values
 * for (const value of params.values()) {
 *     console.log(value);
 * }
 *
 * // forEach method
 * params.forEach((value, name) => {
 *     console.log(`${name}: ${value}`);
 * });
 *
 * // Sort parameters
 * params.sort();
 * ```
 *
 * Example of fibjs extension methods (inherited from HttpCollection):
 *
 * ```JavaScript
 * // Add multiple values (without overwriting existing)
 * params.add('tags', 'javascript');
 *
 * // Get first value
 * const firstName = params.first('name');
 *
 * // Get all values
 * const allHobbies = params.all('hobby');
 *
 * // Set multiple values
 * params.set('colors', ['red', 'green', 'blue']);
 * ```
 *
 * URLSearchParams automatically handles URL encoding and decoding, fully following the Web standard URLSearchParams API specification.
 *
 */
declare class Class_URLSearchParams extends Class_HttpCollection {
    /**
     * @description URLSearchParams constructor, creates a new empty query parameter container
     */
    constructor();

    /**
     * @description URLSearchParams constructor, initializes the parameter container from an object, an array of pairs, another container, a query string or an iterable
     *      @param init the initial parameters: an object whose keys are parameter names, an array of [name, value] pairs, another URLSearchParams container, a query string such as "name=value&key=val", or any iterable of [name, value] pairs
     *
     */
    constructor(init: FIBJS.GeneralObject | any[] | Class_URLSearchParams | string | any);

    /**
     * @description the number of parameter pairs (multiple values with the same name are counted separately, consistent with the Web standard)
     */
    readonly size: number;

    /**
     * @description checks whether a combination of the specified parameter name and parameter value exists in the container
     *      @param name specifies the parameter name to check
     *      @param value specifies the parameter value to check; when undefined is passed, the behavior is the same as has(name)
     *      @return returns whether the specified parameter name and parameter value combination exists
     *
     */
    has(name: string, value: any): boolean;

    /**
     * @description deletes the combination of the specified parameter name and parameter value
     *      @param name specifies the parameter name to delete
     *      @param value specifies the parameter value to delete; when undefined is passed, the behavior is the same as delete(name)
     *
     */
    delete(name: string, value: any): void;

}

