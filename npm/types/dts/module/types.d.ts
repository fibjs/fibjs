/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description Built-in type-tag inspection helpers, exposed as `util.types`
 *
 * The predicates answer "what is this value, really" without trusting the prototype chain: they
 * read the internal slots that the engine assigns to built-in objects, so they keep working for
 * values from another realm and for objects that fake `constructor` or `Symbol.toStringTag`. The
 * module is the complete type-check namespace in fibjs; the util module re-exports most of these
 * predicates directly as `util.is*` helpers.
 *
 * Capability groups:
 * - Primitives and nullish: `isPrimitive`, `isNull`, `isUndefined`, `isNullOrUndefined`, `isBoolean`,
 *   `isNumber`, `isBigInt`, `isString`, `isSymbol`
 * - Containers and callables: `isObject`, `isFunction`, `isArray`, `isBuffer`, `isEmpty`
 * - Built-in instances: `isDate`, `isRegExp`, `isNativeError`, `isPromise`, `isMap`, `isSet`,
 *   `isWeakMap`, `isWeakSet`, `isMapIterator`, `isSetIterator`, `isProxy`, `isArgumentsObject`,
 *   `isGeneratorFunction`, `isGeneratorObject`, `isAsyncFunction`, `isModuleNamespaceObject`
 * - Boxed primitives: `isBoxedPrimitive` and the `is*Object` family (`isBooleanObject`,
 *   `isNumberObject`, `isStringObject`, `isSymbolObject`, `isBigIntObject`)
 * - Buffers, typed arrays and views: `isArrayBuffer`, `isAnyArrayBuffer`, `isSharedArrayBuffer`,
 *   `isArrayBufferView`, `isTypedArray`, `isDataView`, `isInt8Array`, `isUint8Array`,
 *   `isUint8ClampedArray`, `isInt16Array`, `isUint16Array`, `isInt32Array`, `isUint32Array`,
 *   `isFloat16Array`, `isFloat32Array`, `isFloat64Array`, `isBigInt64Array`, `isBigUint64Array`
 * - Host objects: `isCryptoKey`, `isKeyObject`, `isExternal`
 *
 * Concepts:
 * - Type tags instead of the prototype chain: a check looks at an ECMAScript internal slot or internal
 *   type, not at `constructor`, `instanceof` or `Object.prototype.toString`.
 *   `Object.create(Date.prototype)` is not a Date, an instance of `class MyDate extends Date` is,
 *   and a Proxy that wraps a Date is not; toString tags can be spoofed through
 *   `Symbol.toStringTag`, while these predicates cannot.
 * - Cross-realm safety: values created in another context (vm, a structured clone from a worker, an ES
 *   module realm) satisfy the predicates, while `instanceof` fails across realms. Prefer these helpers
 *   over `instanceof` in library code that may receive foreign values.
 * - Choosing a check: `typeof` for primitives, `Array.isArray` when a Proxy around an array must be
 *   recognized (`isArray` does not see through a Proxy), and `types.*` when the realm of the value is
 *   unknown.
 * - Boxed primitives: `new Boolean(false)`, `new Number(1)`, `new String('')` and `Object(1n)` are
 *   objects. `isBoolean`, `isNumber`, `isString` and `isBigInt` accept both the primitive and the
 *   boxed form, `isSymbol` accepts only the primitive, and the `is*Object` family matches only boxed
 *   values; `isBoxedPrimitive` is the union of the five boxed classes.
 * - Node.js comparison: 43 of the 57 members exist in Node.js `util.types` with the same internal-slot
 *   semantics; fibjs adds `isEmpty`, `isArray`, `isBoolean`, `isNull`, `isNullOrUndefined`,
 *   `isNumber`, `isBigInt`, `isString`, `isUndefined`, `isObject`, `isPrimitive`, `isSymbol`,
 *   `isFunction` and `isBuffer`. Unlike Node.js, where a missing value simply returns false, every
 *   member here requires exactly one argument and throws TypeError otherwise.
 *
 * Import:
 * ```JavaScript
 * const util = require('util');
 * const types = util.types;   // also requirable as require('util/types')
 * ```
 *
 * Example 1 — primitives, objects and boxed primitives:
 * ```JavaScript
 * const types = require('util').types;
 *
 * console.log(types.isPrimitive('text'), types.isString('text'));      // true true
 * console.log(types.isPrimitive(new String('text')));                  // false
 * console.log(types.isStringObject(new String('text')));               // true
 * console.log(types.isNumberObject(1), types.isNumber(1));             // false true
 * console.log(types.isNull(undefined), types.isNullOrUndefined(null)); // false true
 * console.log(types.isObject(() => {}), types.isFunction(() => {}));   // true true
 * ```
 *
 * Example 2 — cross-realm values, subclasses and look-alikes:
 * ```JavaScript
 * const vm = require('vm');
 * const types = require('util').types;
 *
 * const foreign = vm.runInNewContext('({ date: new Date(0), list: [1, 2] })');
 * console.log(foreign.date instanceof Date, types.isDate(foreign.date)); // false true
 *
 * class MyDate extends Date {}
 * console.log(types.isDate(new MyDate()));                  // true
 * console.log(types.isDate(Object.create(Date.prototype))); // false
 * console.log(types.isNativeError({ name: 'Error' }));      // false
 * ```
 *
 * Example 3 — buffers, views and collection iterators:
 * ```JavaScript
 * const types = require('util').types;
 *
 * const buffer = Buffer.from('fibjs');
 * console.log(types.isBuffer(buffer), types.isTypedArray(buffer));      // true true
 * console.log(types.isBuffer(new Uint8Array(4)));                       // false
 *
 * const view = new DataView(new ArrayBuffer(8));
 * console.log(types.isArrayBufferView(view), types.isTypedArray(view)); // true false
 *
 * const map = new Map([['a', 1]]);
 * console.log(types.isMap(map), types.isMapIterator(map.keys()));       // true true
 * ```
 *
 * Example 4 — values that isEmpty treats as empty:
 * ```JavaScript
 * const types = require('util').types;
 *
 * console.log(types.isEmpty(null), types.isEmpty(''), types.isEmpty([]));   // true true true
 * console.log(types.isEmpty({ a: 1 }));                                     // false
 * console.log(types.isEmpty(new Date()), types.isEmpty(new Map()));         // true true
 * console.log(types.isEmpty(function () {}));                               // true
 * console.log(types.isEmpty(Object.defineProperty({}, 'k', { value: 1 }))); // true
 * ```
 *
 * Notes:
 * - `isEmpty` is the only member that walks property names, so on a Proxy it can run the `ownKeys`
 *   trap and propagate its error.
 * - `isArray(new Proxy([], {}))` is false while `Array.isArray` is true; `isFunction` does accept a
 *   callable Proxy.
 * - Buffer values satisfy `isTypedArray` and `isUint8Array`; `isBuffer` is the Buffer-specific tag and
 *   `Buffer.prototype` is false.
 * - `isExternal` is always false for values created from JavaScript and exists for Node.js
 *   compatibility.
 * - The namespace is not a global and `require('types')` cannot resolve it; use `util.types` or the
 *   `require('util/types')` alias.
 *
 */
declare module 'types' {
    /**
     * @description Checks whether a value counts as empty
     *
     *      A value is empty when it is null or undefined, an empty string (primitive or String object), an
     *      empty array, or an object without own enumerable string-keyed properties. Symbol keys and
     *      non-enumerable own properties are ignored, so dates, maps, sets and plain functions are empty,
     *      while any object with an enumerable own property is not. On a Proxy the check can run its
     *      `ownKeys` trap, whose error is propagated. Numbers, booleans, symbols and bigints are always
     *      empty. This member is a fibjs extension; Node.js `util.types` has no equivalent.
     *
     *      Example — values that isEmpty treats as empty:
     *      ```JavaScript
     *      const types = require('util').types;
     *
     *      console.log(types.isEmpty(null), types.isEmpty(''), types.isEmpty([]));   // true true true
     *      console.log(types.isEmpty({ a: 1 }));                                     // false
     *      console.log(types.isEmpty(new Date()), types.isEmpty(new Map()));         // true true
     *      console.log(types.isEmpty(function () {}));                               // true
     *      console.log(types.isEmpty(Object.defineProperty({}, 'k', { value: 1 }))); // true
     *      ```
     *      @param v the variable to check
     *      @return returns True if v is empty
     *
     */
    function isEmpty(v: any): boolean;

    /**
     * @description Checks whether a value is an array
     *
     *      The internal array type is tested, not the prototype chain: subclass instances are true, while
     *      `Object.create(Array.prototype)`, array-like objects and a Proxy whose target is an array are
     *      false. `Array.isArray` returns true for that last case because it looks through the
     *      Proxy; prefer `Array.isArray` when proxies may be involved. This member is a fibjs
     *      extension, Node.js only provides `Array.isArray`.
     *
     *      @param v the variable to check
     *      @return returns True if v is an array
     *
     */
    function isArray(v: any): boolean;

    /**
     * @description Checks whether a value is a boolean or a Boolean object
     *
     *      Both forms are accepted, so `isBoolean(true)` and `isBoolean(new Boolean(false))` are true;
     *      use `isBooleanObject` when only the boxed form must match. This member is a fibjs extension,
     *      and the same helper is re-exported as `util.isBoolean`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a boolean or a Boolean object
     *
     */
    function isBoolean(v: any): boolean;

    /**
     * @description Checks whether a value is null
     *
     *      Only null matches; undefined is false. This member is a fibjs extension, see
     *      `isNullOrUndefined` and `isUndefined` for the other nullish cases.
     *
     *      @param v the variable to check
     *      @return returns True if v is null
     *
     */
    function isNull(v: any): boolean;

    /**
     * @description Checks whether a value is null or undefined
     *
     *      Equivalent to `v === null || v === undefined`, the common guard before reading a property. This
     *      member is a fibjs extension, and the same helper is re-exported as `util.isNullOrUndefined`.
     *
     *      @param v the variable to check
     *      @return returns True if v is null or undefined
     *
     */
    function isNullOrUndefined(v: any): boolean;

    /**
     * @description Checks whether a value is a number or a Number object
     *
     *      NaN and Infinity are numbers and match, numeric strings are not. The boxed form `new Number(1)`
     *      is accepted as well; use `isNumberObject` when only the boxed form must match. This member is a
     *      fibjs extension, Node.js only provides `isNumberObject`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a number or a Number object
     *
     */
    function isNumber(v: any): boolean;

    /**
     * @description Checks whether a value is a bigint or a BigInt object
     *
     *      Strings such as '1' and other numeric values are false. The boxed form `Object(1n)` is
     *      accepted; use `isBigIntObject` when only the boxed form must match. This member is a fibjs
     *      extension, Node.js only provides `isBigIntObject`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a bigint or a BigInt object
     *
     */
    function isBigInt(v: any): boolean;

    /**
     * @description Checks whether a value is a string or a String object
     *
     *      Both forms are accepted, so the predicate matches literals and `new String('text')` alike; use
     *      `isStringObject` when only the boxed form must match. This member is a fibjs extension, Node.js
     *      only provides `isStringObject`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a string or a String object
     *
     */
    function isString(v: any): boolean;

    /**
     * @description Checks whether a value is undefined
     *
     *      Only undefined matches; null is false. See `isNullOrUndefined` when both nullish values are
     *      acceptable. This member is a fibjs extension, and the same helper is re-exported as
     *      `util.isUndefined`.
     *
     *      @param v the variable to check
     *      @return returns True if v is undefined
     *
     */
    function isUndefined(v: any): boolean;

    /**
     * @description Checks whether a value is a regular expression
     *
     *      The internal RegExp type is tested: subclass instances are true, while `RegExp.prototype`,
     *      `Object.create(RegExp.prototype)`, a Proxy around a RegExp and an object that only implements
     *      `Symbol.match` are false. Pattern, flags and a replaced `exec` do not affect the result.
     *
     *      Example — real regular expressions, subclasses and look-alikes:
     *      ```JavaScript
     *      const types = require('util').types;
     *
     *      console.log(types.isRegExp(/a/), types.isRegExp(RegExp('a'))); // true true
     *      class MyRegExp extends RegExp {}
     *      console.log(types.isRegExp(new MyRegExp('a')));                // true
     *      console.log(types.isRegExp({ [Symbol.match]: () => {} }));     // false
     *      console.log(types.isRegExp(Object.create(RegExp.prototype)));  // false
     *      ```
     *      @param v the variable to check
     *      @return returns True if v is a RegExp
     *
     */
    function isRegExp(v: any): boolean;

    /**
     * @description Checks whether a value is an object
     *
     *      True for any non-primitive value, including functions, arrays, boxed primitives and objects
     *      with a null prototype; false for primitives, null and undefined. It is the exact negation of
     *      `isPrimitive`. This member is a fibjs extension.
     *
     *      @param v the variable to check
     *      @return returns True if v is an object
     *
     */
    function isObject(v: any): boolean;

    /**
     * @description Checks whether a value is a Date
     *
     *      The internal date value is tested: instances of `class MyDate extends Date` are true, while
     *      `Object.create(Date.prototype)`, a Proxy around a Date and the string returned by `Date()`
     *      are false. Dates from another realm match as well, unlike `instanceof Date`.
     *
     *      Example — dates across realms, subclasses and look-alikes:
     *      ```JavaScript
     *      const vm = require('vm');
     *      const types = require('util').types;
     *
     *      console.log(types.isDate(new Date(0)), types.isDate(Date()));  // true false
     *      class MyDate extends Date {}
     *      console.log(types.isDate(new MyDate()));                       // true
     *      console.log(types.isDate(Object.create(Date.prototype)));      // false
     *      const foreign = vm.runInNewContext('new Date(0)');
     *      console.log(foreign instanceof Date, types.isDate(foreign));   // false true
     *      ```
     *      @param v the variable to check
     *      @return returns True if v is a Date
     *
     */
    function isDate(v: any): boolean;

    /**
     * @description Checks whether a value is an Error created by the language
     *
     *      True for Error, TypeError, SyntaxError, RangeError, ReferenceError, EvalError, URIError,
     *      AggregateError and for classes extending Error, in the current realm or another one; a plain
     *      object with `name` and `message`, array values and `Object.create(Error.prototype)` are false.
     *
     *      Example — native errors against look-alikes:
     *      ```JavaScript
     *      const types = require('util').types;
     *
     *      class MyError extends Error {}
     *      console.log(types.isNativeError(new Error('x')));   // true
     *      console.log(types.isNativeError(new MyError()));    // true
     *      console.log(types.isNativeError(new TypeError('x')));              // true
     *      console.log(types.isNativeError({ name: 'Error', message: 'x' })); // false
     *      console.log(types.isNativeError(Object.create(Error.prototype)));   // false
     *      ```
     *      @param v the variable to check
     *      @return returns True if v is an Error object
     *
     */
    function isNativeError(v: any): boolean;

    /**
     * @description Checks whether a value is a primitive
     *
     *      True for strings, numbers, booleans, bigints, symbols, null and undefined; false for
     *      objects, functions, arrays and boxed primitives such as `new Number(1)`. This member is a
     *      fibjs extension and the exact negation of `isObject`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a primitive
     *
     */
    function isPrimitive(v: any): boolean;

    /**
     * @description Checks whether a value is a symbol primitive
     *
     *      Ordinary and well-known symbols match; the boxed form `Object(Symbol())` is false, use
     *      `isSymbolObject` for it. This is the one type check that does not accept its boxed form.
     *      This member is a fibjs extension.
     *
     *      @param v the variable to check
     *      @return returns True if v is a symbol
     *
     */
    function isSymbol(v: any): boolean;

    /**
     * @description Checks whether a value is a DataView
     *
     *      Only DataView instances match; typed arrays and ArrayBuffer values are false. Use
     *      `isArrayBufferView` when every kind of buffer view should be accepted.
     *
     *      @param v the variable to check
     *      @return returns True if v is a DataView
     *
     */
    function isDataView(v: any): boolean;

    /**
     * @description Checks whether a value is a V8 external value
     *
     *      External values carry native pointers and cannot be created from JavaScript, so this
     *      predicate is always false for user values; it is kept for Node.js compatibility.
     *
     *      @param v the variable to check
     *      @return returns True if v is an external value
     *
     */
    function isExternal(v: any): boolean;

    /**
     * @description Checks whether a value is a Map
     *
     *      Subclass instances are true; `Map.prototype`, plain objects with `get`/`set` methods and
     *      WeakMap values are false. See `isMapIterator` for the iterator objects a Map produces.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Map
     *
     */
    function isMap(v: any): boolean;

    /**
     * @description Checks whether a value is a Map iterator
     *
     *      The iterators returned by `keys()`, `values()` and `entries()` match; the Map itself, the
     *      Map constructor, array iterators and Set iterators are false. Use `isSetIterator` for Set
     *      iterators.
     *
     *      Example — telling map iterators apart:
     *      ```JavaScript
     *      const types = require('util').types;
     *
     *      const map = new Map([['a', 1], ['b', 2]]);
     *      console.log(types.isMapIterator(map.keys()), types.isMapIterator(map.values())); // true true
     *      console.log(types.isMapIterator(map.entries()), types.isMapIterator(map));       // true false
     *      console.log(types.isMapIterator([1][Symbol.iterator]()));                        // false
     *      ```
     *      @param v the variable to check
     *      @return returns True if v is a Map iterator
     *
     */
    function isMapIterator(v: any): boolean;

    /**
     * @description Checks whether a value is a Promise
     *
     *      The internal promise state is tested: subclass instances are true, while thenables (objects
     *      that only implement `then`), `Promise.prototype` and objects that fake `Symbol.toStringTag`
     *      are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Promise
     *
     */
    function isPromise(v: any): boolean;

    /**
     * @description Checks whether a value is an async function
     *
     *      Async function declarations, expressions and arrow functions match, and so do async
     *      generator functions because their internal function kind is async generator. Plain
     *      functions, generator functions and async function instances are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is an async function
     *
     */
    function isAsyncFunction(v: any): boolean;

    /**
     * @description Checks whether a value is a Set
     *
     *      Subclass instances are true; `Set.prototype`, array values and WeakSet values are false. See
     *      `isSetIterator` for the iterator objects a Set produces.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Set
     *
     */
    function isSet(v: any): boolean;

    /**
     * @description Checks whether a value is a Set iterator
     *
     *      The iterators returned by `keys()`, `values()` and `entries()` match; the Set itself, the
     *      Set constructor and array iterators are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Set iterator
     *
     */
    function isSetIterator(v: any): boolean;

    /**
     * @description Checks whether a value is a typed array
     *
     *      Matches every typed array kind (Int8Array through Float64Array, BigInt64Array,
     *      BigUint64Array, Float16Array), including subclass instances and the fibjs Buffer, which is a
     *      Uint8Array subclass. ArrayBuffer, DataView and plain arrays are false; use
     *      `isArrayBufferView` when DataView must be included.
     *
     *      @param v the variable to check
     *      @return returns True if v is a typed array
     *
     */
    function isTypedArray(v: any): boolean;

    /**
     * @description Checks whether a value is a Float32Array
     *
     *      Only Float32Array instances, subclasses included, match; the other float kinds, integer
     *      arrays, DataView and ArrayBuffer are false. Use `isTypedArray` for the whole family.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Float32Array
     *
     */
    function isFloat32Array(v: any): boolean;

    /**
     * @description Checks whether a value is a Float64Array
     *
     *      Only Float64Array instances, subclasses included, match; Float32Array, Float16Array and the
     *      integer arrays are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Float64Array
     *
     */
    function isFloat64Array(v: any): boolean;

    /**
     * @description Checks whether a value is an Int8Array
     *
     *      Only Int8Array instances match; Uint8Array, Uint8ClampedArray and the wider integer arrays are
     *      false.
     *
     *      @param v the variable to check
     *      @return returns True if v is an Int8Array
     *
     */
    function isInt8Array(v: any): boolean;

    /**
     * @description Checks whether a value is an Int16Array
     *
     *      Only Int16Array instances match; the 8-bit and 32-bit integer arrays and DataView are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is an Int16Array
     *
     */
    function isInt16Array(v: any): boolean;

    /**
     * @description Checks whether a value is an Int32Array
     *
     *      Only Int32Array instances match; Int16Array, BigInt64Array and ArrayBuffer are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is an Int32Array
     *
     */
    function isInt32Array(v: any): boolean;

    /**
     * @description Checks whether a value is a Uint8Array
     *
     *      Only Uint8Array instances match; Uint8ClampedArray and DataView are false. A fibjs Buffer is a
     *      Uint8Array subclass and matches here even though it also has its own `isBuffer` tag.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Uint8Array
     *
     */
    function isUint8Array(v: any): boolean;

    /**
     * @description Checks whether a value is a Uint8ClampedArray
     *
     *      Only Uint8ClampedArray instances match; the clamping variant is used for image data, while
     *      Uint8Array and Buffer values are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Uint8ClampedArray
     *
     */
    function isUint8ClampedArray(v: any): boolean;

    /**
     * @description Checks whether a value is a Uint16Array
     *
     *      Only Uint16Array instances match; Uint8Array, Uint32Array and DataView are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Uint16Array
     *
     */
    function isUint16Array(v: any): boolean;

    /**
     * @description Checks whether a value is a Uint32Array
     *
     *      Only Uint32Array instances match; Uint16Array, BigUint64Array and ArrayBuffer are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Uint32Array
     *
     */
    function isUint32Array(v: any): boolean;

    /**
     * @description Checks whether a value is callable
     *
     *      Regular, arrow, async, generator and async generator functions match, as do class
     *      constructors and a Proxy that wraps a callable target. `isObject` is also true for functions.
     *      This member is a fibjs extension, where `typeof v === 'function'` gives the same answer.
     *
     *      @param v the variable to check
     *      @return returns True if v is callable
     *
     */
    function isFunction(v: any): boolean;

    /**
     * @description Checks whether a value is a fibjs Buffer
     *
     *      Matches Buffer instances created by `Buffer.alloc`/`Buffer.from`, `fs.readFile` and the
     *      other Buffer producers; `Buffer.prototype`, plain Uint8Array values and the other typed
     *      arrays are false. Buffer is a Uint8Array subclass, so `isTypedArray` and `isUint8Array` are
     *      true for it as well. This member is a fibjs extension, the Node.js equivalent is
     *      `Buffer.isBuffer`.
     *
     *      Example — Buffer against other binary values:
     *      ```JavaScript
     *      const types = require('util').types;
     *
     *      const buffer = Buffer.from('fibjs');
     *      console.log(types.isBuffer(buffer), types.isBuffer(Buffer.alloc(0))); // true true
     *      console.log(types.isBuffer(new Uint8Array(4)));                       // false
     *      console.log(types.isTypedArray(buffer), types.isUint8Array(buffer));  // true true
     *      console.log(types.isBuffer(Buffer.prototype));                        // false
     *      ```
     *      @param v the variable to check
     *      @return returns True if v is a Buffer
     *
     */
    function isBuffer(v: any): boolean;

    /**
     * @description Checks whether a value is a BigInt object
     *
     *      Only the boxed form `Object(1n)` matches; the bigint primitive is false, use `isBigInt` to
     *      accept both forms. One of the five types combined by `isBoxedPrimitive`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a BigInt object
     *
     */
    function isBigIntObject(v: any): boolean;

    /**
     * @description Checks whether a value is a Boolean object
     *
     *      Only `new Boolean(...)` and `Object(true)` match; the primitive is false, use `isBoolean` to
     *      accept both forms. One of the five types combined by `isBoxedPrimitive`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Boolean object
     *
     */
    function isBooleanObject(v: any): boolean;

    /**
     * @description Checks whether a value is a Number object
     *
     *      Only `new Number(...)` and `Object(1)` match; the primitive is false, use `isNumber` to accept
     *      both forms. One of the five types combined by `isBoxedPrimitive`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Number object
     *
     */
    function isNumberObject(v: any): boolean;

    /**
     * @description Checks whether a value is a String object
     *
     *      Only `new String(...)` and `Object('')` match; the primitive is false, use `isString` to accept
     *      both forms. One of the five types combined by `isBoxedPrimitive`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a String object
     *
     */
    function isStringObject(v: any): boolean;

    /**
     * @description Checks whether a value is a Symbol object
     *
     *      Only the boxed form `Object(Symbol())` matches; the symbol primitive is false, use `isSymbol`
     *      for it. One of the five types combined by `isBoxedPrimitive`.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Symbol object
     *
     */
    function isSymbolObject(v: any): boolean;

    /**
     * @description Checks whether a value is a WeakMap
     *
     *      Only WeakMap instances match; Map values and plain objects with `get`/`set` methods are false.
     *      WeakMap keys must be objects and the entries cannot be enumerated.
     *
     *      @param v the variable to check
     *      @return returns True if v is a WeakMap
     *
     */
    function isWeakMap(v: any): boolean;

    /**
     * @description Checks whether a value is a WeakSet
     *
     *      Only WeakSet instances match; Set values and plain objects are false. WeakSet members must be
     *      objects.
     *
     *      @param v the variable to check
     *      @return returns True if v is a WeakSet
     *
     */
    function isWeakSet(v: any): boolean;

    /**
     * @description Checks whether a value is an ArrayBuffer
     *
     *      Only ArrayBuffer instances match; typed arrays, DataView and SharedArrayBuffer are false. Use
     *      `isAnyArrayBuffer` to accept both buffer kinds.
     *
     *      @param v the variable to check
     *      @return returns True if v is an ArrayBuffer
     *
     */
    function isArrayBuffer(v: any): boolean;

    /**
     * @description Checks whether a value is an ArrayBuffer view
     *
     *      True for DataView and for every typed array, Buffer included; ArrayBuffer and SharedArrayBuffer
     *      themselves are false. It is the union of `isDataView` and `isTypedArray`.
     *
     *      @param v the variable to check
     *      @return returns True if v is an ArrayBuffer view
     *
     */
    function isArrayBufferView(v: any): boolean;

    /**
     * @description Checks whether a value is a BigInt64Array
     *
     *      Only BigInt64Array instances match; the elements are bigints, while BigUint64Array,
     *      Float64Array and DataView are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a BigInt64Array
     *
     */
    function isBigInt64Array(v: any): boolean;

    /**
     * @description Checks whether a value is a BigUint64Array
     *
     *      Only BigUint64Array instances match; the elements are bigints, while BigInt64Array,
     *      Float64Array and DataView are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a BigUint64Array
     *
     */
    function isBigUint64Array(v: any): boolean;

    /**
     * @description Checks whether a value is a Float16Array
     *
     *      Only Float16Array instances match; the half-precision array is also available in Node.js, while
     *      Float32Array, Float64Array and the integer arrays are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Float16Array
     *
     */
    function isFloat16Array(v: any): boolean;

    /**
     * @description Checks whether a value is an ArrayBuffer or a SharedArrayBuffer
     *
     *      True for the two buffer kinds that views can wrap; DataView, typed arrays and Buffer values are
     *      false. Use `isSharedArrayBuffer` to tell the two kinds apart.
     *
     *      @param v the variable to check
     *      @return returns True if v is an ArrayBuffer or a SharedArrayBuffer
     *
     */
    function isAnyArrayBuffer(v: any): boolean;

    /**
     * @description Checks whether a value is a SharedArrayBuffer
     *
     *      Only SharedArrayBuffer instances match; shared buffers can be sent to workers without being
     *      copied. ArrayBuffer is false here, use `isAnyArrayBuffer` to accept both kinds.
     *
     *      @param v the variable to check
     *      @return returns True if v is a SharedArrayBuffer
     *
     */
    function isSharedArrayBuffer(v: any): boolean;

    /**
     * @description Checks whether a value is an arguments object
     *
     *      True for the implicit `arguments` object of non-arrow functions; arrays, array-like objects
     *      and a rest parameters array are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is an arguments object
     *
     */
    function isArgumentsObject(v: any): boolean;

    /**
     * @description Checks whether a value is a boxed primitive object
     *
     *      True for the object wrappers of the five primitive types (Boolean, Number, String, Symbol
     *      and BigInt), created with `new Boolean(...)`, `Object(1)` and so on; the primitives
     *      themselves, arrays, dates and plain objects are false. Equivalent to the union of the five
     *      `is*Object` members.
     *
     *      @param v the variable to check
     *      @return returns True if v is a boxed primitive object
     *
     */
    function isBoxedPrimitive(v: any): boolean;

    /**
     * @description Checks whether a value is a generator function
     *
     *      True for generator and async generator function kinds; plain functions and async functions
     *      that are not generators are false. See `isGeneratorObject` for the objects these functions
     *      produce.
     *
     *      @param v the variable to check
     *      @return returns True if v is a generator function
     *
     */
    function isGeneratorFunction(v: any): boolean;

    /**
     * @description Checks whether a value is a generator object
     *
     *      True for objects returned by calling a generator function, paused or exhausted, and for async
     *      generator objects; the function itself and plain collection iterators are false.
     *
     *      @param v the variable to check
     *      @return returns True if v is a generator object
     *
     */
    function isGeneratorObject(v: any): boolean;

    /**
     * @description Checks whether a value is a Proxy
     *
     *      True for Proxy objects created with `new Proxy`, including revoked proxies; the check does
     *      not run any trap. Because a Proxy has no internal slot of its own, the other predicates see
     *      the Proxy rather than its target, so `isProxy` is how such a wrapper is detected.
     *
     *      @param v the variable to check
     *      @return returns True if v is a Proxy
     *
     */
    function isProxy(v: any): boolean;

    /**
     * @description Checks whether a value is a module namespace object
     *
     *      True for the namespace object produced by an ES module import (`import * as ns` or dynamic
     *      `import()`); a CommonJS exports object returned by `require` and a plain object are false.
     *
     *      Example — the namespace object of an ES module:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const types = require('util').types;
     *
     *      (async () => {
     *          const file = path.join(os.tmpdir(), 'fibjs-types-ns-' + process.pid + '.mjs');
     *          fs.writeFileSync(file, 'export const answer = 42;');
     *          try {
     *              const ns = await import(file);
     *              console.log(ns.answer, types.isModuleNamespaceObject(ns)); // 42 true
     *          } finally {
     *              fs.unlinkSync(file);
     *          }
     *      })();
     *      ```
     *      @param v the variable to check
     *      @return returns True if v is a module namespace object
     *
     */
    function isModuleNamespaceObject(v: any): boolean;

    /**
     * @description Checks whether a value is a WebCrypto CryptoKey
     *
     *      True for CryptoKey instances created by `crypto.subtle.generateKey`, `importKey` and
     *      `deriveKey`; KeyObject values are false, use `isKeyObject` for them.
     *
     *      Example — WebCrypto and Node.js style keys:
     *      ```JavaScript
     *      const crypto = require('crypto');
     *      const types = require('util').types;
     *
     *      (async () => {
     *          const key = await crypto.subtle.generateKey(
     *              { name: 'HMAC', hash: 'SHA-256', length: 256 }, true, ['sign']);
     *          console.log(types.isCryptoKey(key)); // true
     *          console.log(types.isCryptoKey(crypto.createSecretKey(Buffer.alloc(32)))); // false
     *      })();
     *      ```
     *      @param v the variable to check
     *      @return returns True if v is a CryptoKey
     *
     */
    function isCryptoKey(v: any): boolean;

    /**
     * @description Checks whether a value is a crypto KeyObject
     *
     *      True for `crypto.KeyObject` instances from `crypto.createSecretKey`, `createPrivateKey`,
     *      `createPublicKey` and the key-pair generators; CryptoKey values are false, use `isCryptoKey`
     *      for them.
     *
     *      @param v the variable to check
     *      @return returns True if v is a KeyObject
     *
     */
    function isKeyObject(v: any): boolean;

}

