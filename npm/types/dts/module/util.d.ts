/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TextDecoder.d.ts" />
/// <reference path="../interface/TextEncoder.d.ts" />
/// <reference path="../module/types.d.ts" />
/// <reference path="../module/colors.d.ts" />
/// <reference path="../interface/ConsoleObject.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The util module provides practical utility functions such as data type checking, object property copying, template string parsing and event handling
 *
 * The following is a detailed introduction with examples:
 *
 * 1. Checking data types - `util.is[type]`
 * This module provides methods such as `isDate`, `isRegExp` and `isError` to check the data type of the passed parameter, for example:
 *
 * ```JavaScript
 * var util = require('util');
 * console.log(util.isDate(new Date()));
 * console.log(util.isRegExp(/some regexp/));
 * ```
 * 2. Copying object properties - `util.inherits()`
 * This method can make one constructor inherit from another, thus implementing prototype inheritance.
 *
 * ```JavaScript
 * var util = require('util');
 * function Animal() {
 *   this.name = 'Animal';
 *   this.sleep = function () {
 *     console.log(this.name + ' is sleeping!');
 *   }
 * }
 * Animal.prototype.eat = function (food) {
 *   console.log(this.name + ' is eating: ' + food);
 * };
 * function Cat() {
 *   this.name = 'cat';
 * }
 * util.inherits(Cat, Animal);
 * ```
 *
 * Using the `Cat` constructor to inherit the instance properties and prototype properties of `Animal`, print the properties and methods of a `Cat` instance
 *
 * ```JavaScript
 * var cat = new Cat();
 * console.log(cat.name);
 * console.log(cat.eat('fish'));
 * console.log(cat.sleep());
 * ```
 *
 * 3. util.format() formatted output template
 * ```JavaScript
 * const util = require('util');
 * const str1 = util.format('%s:%s', 'foo');
 * const str2 = util.format('%s:%s', 'foo', 'bar', 'baz');
 * console.log(str1) // => 'foo:%s'
 * console.log(str2) // => 'foo:bar baz'
 * ```
 *
 * The above are some commonly used methods of the `util` module, which can often be used to simplify the actual development process.
 *
 */
declare module 'util' {
    /**
     * @description The TextDecoder decoding object, see the TextDecoder object.
     */
    const TextDecoder: typeof Class_TextDecoder;

    /**
     * @description The TextEncoder encoding object, see the TextEncoder object.
     */
    const TextEncoder: typeof Class_TextEncoder;

    /**
     * @description The types module provides utility functions for data type checking.
     */
    const types: typeof import ('types');

    /**
     * @description The colors module provides a set of color constants for setting console output colors.
     */
    const colors: typeof import ('colors');

    /**
     * @description Formats variables according to the specified format
     *
     *      @param fmt format string
     *      @param args optional parameter list
     *      @return returns the formatted string
     *
     */
    function format(fmt: string, ...args: any[]): string;

    /**
     * @description Formats variables
     *
     *      @param args optional parameter list
     *      @return returns the formatted string
     *
     */
    function format(...args: any[]): string;

    /**
     * @description Formats variables according to the specified format and inspect options
     *
     *      @param options inspect options used for non-string values
     *      @param fmt format string
     *      @param args optional parameter list
     *      @return returns the formatted string
     *
     */
    function formatWithOptions(options: FIBJS.GeneralObject, fmt: string, ...args: any[]): string;

    /**
     * @description Inherits prototype functions from one constructor to another. The prototype of the constructor will be set to a new object created from the superclass (superConstructor).
     *
     *      @param constructor the initial constructor
     *      @param superConstructor the inherited superclass
     *
     */
    function inherits(constructor: any, superConstructor: any): void;

    /**
     * @description Parses the raw text of a dotenv file and returns a key-value object
     *
     *      @param content raw content of the dotenv file
     *      @return returns the parsed key-value object
     *
     */
    function parseEnv(content: string): FIBJS.GeneralObject;

    /**
     * @description Returns a string representation of obj, mainly for debugging. The additional options can be used to change certain aspects of the formatted string.
     *
     *      The following parameters are supported:
     *      ```JavaScript
     *      {
     *          "colors": false, // specify if output should be colorized, defaults to false
     *          "depth": 2, // specify the max depth of the output, defaults to 2
     *          "table": false, // specify if output should be a table, defaults to false
     *          "encode_string": true, // specify if string should be encoded, defaults to true
     *          "maxArrayLength": 100, // specify max number of array elements to show, set to 0 or negative to show no elements, defaults to 100
     *          "maxStringLength": 10000, // specify max string length to output, set to 0 or negative to show no strings, defaults to 10000
     *          "fields": [], // specify the fields to be displayed, defaults to all
     *      }
     *      ```
     *      @param obj the object to process
     *      @param options the format control options to use
     *      @return returns the formatted string
     *
     */
    function inspect(obj: any, options?: FIBJS.GeneralObject): string;

    /**
     * @description Applies ANSI color/style formatting to text
     *
     *      When color output is not supported (such as a non-TTY environment or when NO_COLOR is set), the text is returned as-is.
     *
     *      Supported formats: bold, italic, underline, strikethrough, hidden,
     *      black, red, green, yellow, blue, magenta, cyan, white,
     *      bgBlack, bgRed, bgGreen, bgYellow, bgBlue, bgMagenta, bgCyan, bgWhite,
     *      gray/grey, blackBright, redBright, greenBright, yellowBright, blueBright,
     *      magentaBright, cyanBright, whiteBright
     *
     *      @param format array of format names
     *      @param text the text to format
     *      @return returns the formatted string
     *
     */
    function styleText(format: string[], text: string): string;

    /**
     * @description Applies ANSI color/style formatting to text
     *
     *      @param format format name
     *      @param text the text to format
     *      @return returns the formatted string
     *
     */
    function styleText(format: string, text: string): string;

    /**
     * @description Creates a ConsoleObject object that conditionally outputs debug information according to the NODE_DEBUG environment variable
     *
     *      @param section the debug section to use
     *      @return returns a ConsoleObject object
     *
     */
    function debuglog(section: string): Class_ConsoleObject;

    /**
     * @description Creates a ConsoleObject object that conditionally outputs debug information according to the NODE_DEBUG environment variable
     *
     *      @param section the debug section to use
     *      @param fn callback called the first time a log function is invoked; its argument is a more optimized log function
     *      @return returns a ConsoleObject object
     *
     */
    function debuglog(section: string, fn: (...args: any[])=>any): Class_ConsoleObject;

    /**
     * @description Creates a ConsoleObject object that conditionally outputs debug information according to the NODE_DEBUG environment variable. Alias of debuglog
     *
     *      @param section the debug section to use
     *      @return returns a ConsoleObject object
     *
     */
    function debug(section: string): Class_ConsoleObject;

    /**
     * @description Creates a ConsoleObject object that conditionally outputs debug information according to the NODE_DEBUG environment variable. Alias of debuglog
     *
     *      @param section the debug section to use
     *      @param fn callback called the first time a log function is invoked; its argument is a more optimized log function
     *      @return returns a ConsoleObject object
     *
     */
    function debug(section: string, fn: (...args: any[])=>any): Class_ConsoleObject;

    /**
     * @description Wraps the given function. This function is for compatibility only and does not output a warning
     *
     *      @param fn the function to wrap
     *      @param msg the warning message
     *      @param code the warning code
     *      @return returns the wrapped result
     *
     */
    function deprecate(fn: (...args: any[])=>any, msg: string, code?: string): (...args: any[])=>any;

    /**
     * @description Checks whether the given variable contains no value (no enumerable properties)
     *
     *      @param v the variable to check
     *      @return returns True if empty
     *
     */
    function isEmpty(v: any): boolean;

    /**
     * @description Checks whether the given variable is an array
     *
     *      @param v the variable to check
     *      @return returns True if it is an array
     *
     */
    function isArray(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Boolean
     *
     *      @param v the variable to check
     *      @return returns True if it is a Boolean
     *
     */
    function isBoolean(v: any): boolean;

    /**
     * @description Checks whether the given variable is Null
     *
     *      @param v the variable to check
     *      @return returns True if it is Null
     *
     */
    function isNull(v: any): boolean;

    /**
     * @description Checks whether the given variable is Null or Undefined
     *
     *      @param v the variable to check
     *      @return returns True if it is Null or Undefined
     *
     */
    function isNullOrUndefined(v: any): boolean;

    /**
     * @description Checks whether the given variable is a number
     *
     *      @param v the variable to check
     *      @return returns True if it is a number
     *
     */
    function isNumber(v: any): boolean;

    /**
     * @description Checks whether the given variable is a BigInt
     *
     *      @param v the variable to check
     *      @return returns True if it is a number
     *
     */
    function isBigInt(v: any): boolean;

    /**
     * @description Checks whether the given variable is a string
     *
     *      @param v the variable to check
     *      @return returns True if it is a string
     *
     */
    function isString(v: any): boolean;

    /**
     * @description Checks whether the given variable is Undefined
     *
     *      @param v the variable to check
     *      @return returns True if it is Undefined
     *
     */
    function isUndefined(v: any): boolean;

    /**
     * @description Checks whether the given variable is a regular expression object
     *
     *      @param v the variable to check
     *      @return returns True if it is a regular expression object
     *
     */
    function isRegExp(v: any): boolean;

    /**
     * @description Checks whether the given variable is an object
     *
     *      @param v the variable to check
     *      @return returns True if it is an object
     *
     */
    function isObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a date object
     *
     *      @param v the variable to check
     *      @return returns True if it is a date object
     *
     */
    function isDate(v: any): boolean;

    /**
     * @description Checks whether the given variable is an error object
     *
     *      @param v the variable to check
     *      @return returns True if it is an error object
     *
     */
    function isNativeError(v: any): boolean;

    /**
     * @description Checks whether the given variable is a primitive type
     *
     *      @param v the variable to check
     *      @return returns True if it is a primitive type
     *
     */
    function isPrimitive(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Symbol type
     *
     *      @param v the variable to check
     *      @return returns True if it is a Symbol type
     *
     */
    function isSymbol(v: any): boolean;

    /**
     * @description Checks whether the given variable is a DataView type
     *
     *      @param v the variable to check
     *      @return returns True if it is a DataView type
     *
     */
    function isDataView(v: any): boolean;

    /**
     * @description Checks whether the given variable is an External type
     *
     *      @param v the variable to check
     *      @return returns True if it is an External type
     *
     */
    function isExternal(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Map type
     *
     *      @param v the variable to check
     *      @return returns True if it is a Map type
     *
     */
    function isMap(v: any): boolean;

    /**
     * @description Checks whether the given variable is a MapIterator type
     *
     *      @param v the variable to check
     *      @return returns True if it is a MapIterator type
     *
     */
    function isMapIterator(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Promise type
     *
     *      @param v the variable to check
     *      @return returns True if it is a Promise type
     *
     */
    function isPromise(v: any): boolean;

    /**
     * @description Checks whether the given variable is an AsyncFunction type
     *
     *      @param v the variable to check
     *      @return returns True if it is an AsyncFunction type
     *
     */
    function isAsyncFunction(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Set type
     *
     *      @param v the variable to check
     *      @return returns True if it is a Set type
     *
     */
    function isSet(v: any): boolean;

    /**
     * @description Checks whether the given variable is a SetIterator type
     *
     *      @param v the variable to check
     *      @return returns True if it is a SetIterator type
     *
     */
    function isSetIterator(v: any): boolean;

    /**
     * @description Checks whether the given variable is a TypedArray type
     *
     *      @param v the variable to check
     *      @return returns True if it is a TypedArray type
     *
     */
    function isTypedArray(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Uint8Array type
     *
     *      @param v the variable to check
     *      @return returns True if it is a Uint8Array type
     *
     */
    function isUint8Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is a function object
     *
     *      @param v the variable to check
     *      @return returns True if it is a function object
     *
     */
    function isFunction(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Buffer object
     *
     *      @param v the variable to check
     *      @return returns True if it is a Buffer object
     *
     */
    function isBuffer(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Float16Array type
     *
     *      @param v the variable to check
     *      @return returns True if it is a Float16Array type
     *
     */
    function isFloat16Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is an ArrayBuffer or SharedArrayBuffer type
     *
     *      @param v the variable to check
     *      @return returns True if it is an ArrayBuffer or SharedArrayBuffer type
     *
     */
    function isAnyArrayBuffer(v: any): boolean;

    /**
     * @description Checks whether the given variable is a SharedArrayBuffer type
     *
     *      @param v the variable to check
     *      @return returns True if it is a SharedArrayBuffer type
     *
     */
    function isSharedArrayBuffer(v: any): boolean;

    /**
     * @description Checks whether the given variable is an arguments object
     *
     *      @param v the variable to check
     *      @return returns True if it is an arguments object
     *
     */
    function isArgumentsObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a boxed primitive object (such as new Boolean(), new String(), etc.)
     *
     *      @param v the variable to check
     *      @return returns True if it is a boxed primitive object
     *
     */
    function isBoxedPrimitive(v: any): boolean;

    /**
     * @description Checks whether the given variable is a GeneratorFunction type
     *
     *      @param v the variable to check
     *      @return returns True if it is a GeneratorFunction type
     *
     */
    function isGeneratorFunction(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Generator object
     *
     *      @param v the variable to check
     *      @return returns True if it is a Generator object
     *
     */
    function isGeneratorObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Proxy instance
     *
     *      @param v the variable to check
     *      @return returns True if it is a Proxy instance
     *
     */
    function isProxy(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Module Namespace object
     *
     *      @param v the variable to check
     *      @return returns True if it is a Module Namespace object
     *
     */
    function isModuleNamespaceObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a CryptoKey type
     *
     *      @param v the variable to check
     *      @return returns True if it is a CryptoKey type
     *
     */
    function isCryptoKey(v: any): boolean;

    /**
     * @description Checks whether the given variable is a KeyObject type
     *
     *      @param v the variable to check
     *      @return returns True if it is a KeyObject type
     *
     */
    function isKeyObject(v: any): boolean;

    /**
     * @description Tests whether a value is deeply equal to the expected value
     *      @param actual the value to test
     *      @param expected the expected value
     *      @return returns True if deeply equal
     *
     */
    function isDeepEqual(actual: any, expected: any): boolean;

    /**
     * @description Tests whether a value is strictly deeply equal to the expected value
     *      @param actual the value to test
     *      @param expected the expected value
     *      @return returns True if strictly deeply equal
     *
     */
    function isDeepStrictEqual(actual: any, expected: any): boolean;

    /**
     * @description Queries whether the specified object contains the given key
     *
     *      @param v the object to query
     *      @param key the key to query
     *      @return returns the array of all keys of the object
     *
     */
    function has(v: any, key: string): boolean;

    /**
     * @description Queries the array of all keys of the specified object
     *
     *      @param v the object to query
     *      @return returns the array of all keys of the object
     *
     */
    function keys(v: any): any[];

    /**
     * @description Queries the array of all values of the specified object
     *
     *      @param v the object to query
     *      @return returns the array of all values of the object
     *
     */
    function values(v: any): any[];

    /**
     * @description Clones the given variable; if it is an object or array, copies the content to a new object
     *
     *      @param v the variable to clone
     *      @return returns the clone result
     *
     */
    function clone(v: any): any;

    /**
     * @description Deeply freezes an object; the frozen object and the objects it contains can no longer be modified
     *
     *      @param v the object to freeze
     *
     */
    function deepFreeze(v: any): void;

    /**
     * @description Extends the specified object with the key-values of one or more objects
     *
     *      @param v the object to extend
     *      @param objs one or more objects used for extension
     *      @return returns the extension result
     *
     */
    function extend(v: any, ...objs: any[]): any;

    /**
     * @description Extends the specified object with the key-values of one or more objects. Alias of extend
     *
     *      @param v the object to extend
     *      @param objs one or more objects used for extension
     *      @return returns the extension result
     *
     */
    function _extend(v: any, ...objs: any[]): any;

    /**
     * @description Returns a copy of an object containing only the property values of the specified keys
     *
     *      @param v the object to filter
     *      @param objs one or more keys to select
     *      @return returns the filter result
     *
     */
    function pick(v: any, ...objs: any[]): FIBJS.GeneralObject;

    /**
     * @description Returns a copy of an object excluding the property values of the specified keys
     *
     *      @param v the object to filter
     *      @param keys one or more keys to exclude
     *      @return returns the filtered result
     *
     */
    function omit(v: any, ...keys: any[]): FIBJS.GeneralObject;

    /**
     * @description Gets the first element of an array
     *
     *      @param v the array to get from
     *      @return returns the element
     *
     */
    function first(v: any): any;

    /**
     * @description Gets several elements from the beginning of an array
     *
     *      @param v the array to get from
     *      @param n the number of elements to get
     *      @return returns the array of elements
     *
     */
    function first(v: any, n: number): any;

    /**
     * @description Gets the last element of an array
     *
     *      @param v the array to get from
     *      @return returns the element
     *
     */
    function last(v: any): any;

    /**
     * @description Gets several elements from the end of an array
     *
     *      @param v the array to get from
     *      @param n the number of elements to get
     *      @return returns the array of elements
     *
     */
    function last(v: any, n: number): any;

    /**
     * @description Gets a copy of an array with duplicate elements removed
     *
     *      @param v the array to deduplicate
     *      @param sorted whether the array is sorted; if the array is sorted, a faster algorithm is used
     *      @return returns the array with duplicate elements removed
     *
     */
    function unique(v: any, sorted?: boolean): any[];

    /**
     * @description Merges the values of one or more arrays into an array of unique values
     *
     *      @param arrs one or more arrays to merge
     *      @return returns the merge result
     *
     */
    function union(...arrs: any[]): any[];

    /**
     * @description Returns the intersection of the arrays with the elements of one or more arrays excluded
     *
     *      @param arrs one or more arrays used to compute the intersection
     *      @return returns the computed intersection
     *
     */
    function intersection(...arrs: any[]): any[];

    /**
     * @description Flattens a multi-level nested array (nesting can be at any depth) into a single-level array. If the shallow parameter is passed, the array is flattened by only one level.
     *
     *      @param arr the array to convert
     *      @param shallow whether to flatten only one level, default is false
     *      @return returns the conversion result
     *
     */
    function flatten(arr: any, shallow?: boolean): any[];

    /**
     * @description Returns a copy of the array with one or more elements excluded
     *
     *      @param arr the array to exclude from
     *      @param els one or more elements to exclude
     *      @return returns the exclusion result
     *
     */
    function without(arr: any, ...els: any[]): any[];

    /**
     * @description Returns a copy of the array with the elements of the without arrays excluded
     *
     *      @param list the array to exclude from
     *      @param arrs one or more arrays to exclude
     *      @return returns the exclusion result
     *
     */
    function difference(list: any[], ...arrs: any[]): any[];

    /**
     * @description Iterates over all elements in list, outputting each element in order. If the context parameter is passed, iterator is bound to the context object. Each call to iterator is passed three parameters: (element, index, list)
     *
     *      @param list the list or object to iterate
     *      @param iterator the callback function used for iteration
     *      @param context the context object to bind when calling iterator
     *      @return returns list itself
     *
     */
    function each(list: any, iterator: (...args: any[])=>any, context?: any): any;

    /**
     * @description Maps each value in list to a new array through the transform function (iterator). If the context parameter is passed, iterator is bound to the context object. Each call to iterator is passed three parameters: (element, index, list)
     *
     *      @param list the list or object to transform
     *      @param iterator the callback function used for transformation
     *      @param context the context object to bind when calling iterator
     *      @return returns the transformation result
     *
     */
    function map(list: any, iterator: (...args: any[])=>any, context?: any): any[];

    /**
     * @description Reduces the elements in list to a single value. If the context parameter is passed, iterator is bound to the context object. Each call to iterator is passed three parameters: (memo, element, index, list)
     *
     *      @param list the list or object to reduce
     *      @param iterator the callback function used for reduction
     *      @param memo the initial value of the reduction
     *      @param context the context object to bind when calling iterator
     *      @return returns the reduction result
     *
     */
    function reduce(list: any, iterator: (...args: any[])=>any, memo: any, context?: any): any;

    /**
     * @description Parses a command line string and returns the parameter list
     *      @param command the command line string to parse
     *      @return returns the parsed parameter list
     *
     */
    function parseArgs(command: string): any[];

    /**
     * @description Compiles a script into binary code
     *      util.compile can compile a script into a v8 internal data block (not machine-executable code). After compilation, the code can be saved as *.jsc and then directly loaded and executed by run and require.
     *
     *      Since the target code cannot be reverse-engineered to obtain the source code after compilation, programs that depend on Function.toString will not work properly.
     *
     *      @param srcname the name of the script to add
     *      @param script the script code to compile
     *      @param mode compilation mode, 0: module, 1: script, 2: worker, default is 0
     *      @return returns the compiled binary code
     *
     */
    function compile(srcname: string, script: string, mode?: number): Class_Buffer;

    /**
     * @description Wraps a callback or async function for synchronous invocation
     *
     *      util.sync converts a callback function or async function into a sync function for convenient invocation.
     *
     *      Example of callback:
     *      ```JavaScript
     *      // callback
     *      var util = require('util');
     *
     *      function cb_test(a, b, cb) {
     *        setTimeout(() => {
     *           cb(null, a + b);
     *        }, 100);
     *      }
     *
     *      var fn_sync = util.sync(cb_test);
     *      console.log(fn_sync(100, 200));
     *      ```
     *      Example of async:
     *      ```JavaScript
     *      // async/await
     *      var util = require('util');
     *
     *      async function async_test(a, b) {
     *          return a + b;
     *      }
     *
     *      var fn_sync = util.sync(async_test);
     *      console.log(fn_sync(100, 200));
     *      ```
     *      For functions that return a promise but are not marked as async, the sync mode can be specified manually:
     *      ```JavaScript
     *      // async/await
     *      var util = require('util');
     *
     *      function async_test(a, b) {
     *          return new Promise(function (resolve, reject) {
     *            resolve(a + b);
     *          });
     *      }
     *
     *      var fn_sync = util.sync(async_test, true);
     *      console.log(fn_sync(100, 200));
     *      ```
     *
     *      @param func the function to wrap
     *      @param async_func whether to process func as an async function; if false, it is determined automatically
     *      @return returns a function that runs synchronously
     *
     */
    function sync(func: (...args: any[])=>any, async_func?: boolean): (...args: any[])=>any;

    /**
     * @description Wraps a callback function for async invocation
     *
     *      util.promisify converts a callback function into an async function for convenient invocation.
     *
     *      Example of callback:
     *      ```JavaScript
     *      // callback
     *      var util = require('util');
     *
     *      function cb_test(a, b, cb) {
     *        setTimeout(() => {
     *           cb(null, a + b);
     *        }, 100);
     *      }
     *
     *      var fn_sync = util.promisify(cb_test);
     *      console.log(async fn_sync(100, 200));
     *      ```
     *
     *      @param func the function to wrap
     *      @return returns an async function
     *
     */
    function promisify(func: (...args: any[])=>any): (...args: any[])=>any;

    /**
     * @description Wraps an async function for callback invocation
     *
     *      util.callbackify converts an async function into a callback function for convenient invocation.
     *
     *      Example of async:
     *      ```JavaScript
     *      // async
     *      var util = require('util');
     *
     *      async function async_test(a, b) {
     *        return a + b;
     *      }
     *
     *      var fn_callback = util.callbackify(async_test);
     *
     *      fn_callback(100, 200, (err, result) => {
     *        console.log(result);
     *      });
     *      ```
     *
     *      @param func the function to wrap
     *      @return returns a callback function
     *
     */
    function callbackify(func: (...args: any[])=>any): (...args: any[])=>any;

    /**
     * @description Queries the version information of the current engine and each component
     *
     *      ```JavaScript
     *       {
     *         "fibjs": "0.25.0",
     *         "clang": "9.1",
     *         "date": "Jun 12 2018 07:22:40",
     *         "vender": {
     *           "ev": "4.24",
     *           "expat": "2.2.5",
     *           "gd": "2.2.4",
     *           "jpeg": "8.3",
     *           "leveldb": "1.17",
     *           "mongo": "0.7",
     *           "pcre": "8.21",
     *           "png": "1.5.4",
     *           "mbedtls": "2.6.1",
     *           "snappy": "1.1.2",
     *           "sqlite": "3.23.0",
     *           "tiff": "3.9.5",
     *           "uuid": "1.6.2",
     *           "v8": "6.7.288.20",
     *           "v8-snapshot": true,
     *           "zlib": "1.2.7",
     *           "zmq": "3.1"
     *         }
     *       }
     *      ```
     *      @return returns the component version object
     *
     */
    function buildInfo(): FIBJS.GeneralObject;

    /**
     * @description Converts TypeScript code to JavaScript, removing all type annotations
     *
     *      This method uses strip-only mode, replacing TypeScript type syntax with spaces while keeping the line and column positions of the source code unchanged.
     *      This is useful for scenarios that require debugging or generating source maps.
     *
     *      Notes: strip-only mode does not support the following syntax:
     *      - enum (must be converted to an IIFE)
     *      - const enum (must be inlined)
     *      - namespace (must be converted to an IIFE)
     *      - constructor parameter properties (such as constructor(public x: string))
     *      - import = require() syntax
     *      - export = syntax
     *      - angle bracket type assertions (such as <T>expr, use the as syntax instead)
     *
     *      Example:
     *      ```JavaScript
     *      var util = require('util');
     *      var ts = 'const x: string = "hello";';
     *      var js = util.stripTypeScript(ts);
     *      console.log(js); // 'const x         = "hello";'
     *      ```
     *
     *      @param code TypeScript source code
     *      @return returns the converted JavaScript code
     *
     */
    function stripTypeScript(code: string): string;

    /**
     * @description Gets the visual width of a string, taking full-width characters, emoji and ANSI escape sequences into account
     *      Characters with East Asian Width property Fullwidth (F) or Wide (W) count as 2, and most other characters count as 1.
     *      Control characters and combining marks count as 0. ANSI escape sequences are skipped.
     *
     *      @param str the string whose width is to be calculated
     *      @return returns the visual width of the string
     *
     */
    function getStringWidth(str: string): number;

    /**
     * @description Removes ANSI escape sequences (VT control characters) from a string
     *
     *      @param str the string to process
     *      @return returns the string with ANSI escape sequences removed
     *
     */
    function stripVTControlCharacters(str: string): string;

}

