/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The types module provides utility functions for data type checking
 *
 * The following is a detailed introduction with examples:
 *
 * ```JavaScript
 * var util = require('util');
 * console.log(util.types.isDate(new Date()));
 * console.log(util.types.isRegExp(/some regexp/));
 * ```
 *
 */
declare module 'types' {
    /**
     * @description Checks whether the given variable contains no value (no enumerable properties)
     *      @param v the variable to check
     *      @return returns True if empty
     *
     */
    function isEmpty(v: any): boolean;

    /**
     * @description Checks whether the given variable is an array
     *      @param v the variable to check
     *      @return returns True if it is an array
     *
     */
    function isArray(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Boolean
     *      @param v the variable to check
     *      @return returns True if it is a Boolean
     *
     */
    function isBoolean(v: any): boolean;

    /**
     * @description Checks whether the given variable is Null
     *      @param v the variable to check
     *      @return returns True if it is Null
     *
     */
    function isNull(v: any): boolean;

    /**
     * @description Checks whether the given variable is Null or Undefined
     *      @param v the variable to check
     *      @return returns True if it is Null or Undefined
     *
     */
    function isNullOrUndefined(v: any): boolean;

    /**
     * @description Checks whether the given variable is a number
     *      @param v the variable to check
     *      @return returns True if it is a number
     *
     */
    function isNumber(v: any): boolean;

    /**
     * @description Checks whether the given variable is a BigInt
     *      @param v the variable to check
     *      @return returns True if it is a number
     *
     */
    function isBigInt(v: any): boolean;

    /**
     * @description Checks whether the given variable is a string
     *      @param v the variable to check
     *      @return returns True if it is a string
     *
     */
    function isString(v: any): boolean;

    /**
     * @description Checks whether the given variable is Undefined
     *      @param v the variable to check
     *      @return returns True if it is Undefined
     *
     */
    function isUndefined(v: any): boolean;

    /**
     * @description Checks whether the given variable is a regular expression object
     *      @param v the variable to check
     *      @return returns True if it is a regular expression object
     *
     */
    function isRegExp(v: any): boolean;

    /**
     * @description Checks whether the given variable is an object
     *      @param v the variable to check
     *      @return returns True if it is an object
     *
     */
    function isObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a date object
     *      @param v the variable to check
     *      @return returns True if it is a date object
     *
     */
    function isDate(v: any): boolean;

    /**
     * @description Checks whether the given variable is an error object
     *      @param v the variable to check
     *      @return returns True if it is an error object
     *
     */
    function isNativeError(v: any): boolean;

    /**
     * @description Checks whether the given variable is a primitive type
     *      @param v the variable to check
     *      @return returns True if it is a primitive type
     *
     */
    function isPrimitive(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Symbol type
     *      @param v the variable to check
     *      @return returns True if it is a Symbol type
     *
     */
    function isSymbol(v: any): boolean;

    /**
     * @description Checks whether the given variable is a DataView type
     *      @param v the variable to check
     *      @return returns True if it is a DataView type
     *
     */
    function isDataView(v: any): boolean;

    /**
     * @description Checks whether the given variable is an External type
     *      @param v the variable to check
     *      @return returns True if it is an External type
     *
     */
    function isExternal(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Map type
     *      @param v the variable to check
     *      @return returns True if it is a Map type
     *
     */
    function isMap(v: any): boolean;

    /**
     * @description Checks whether the given variable is a MapIterator type
     *      @param v the variable to check
     *      @return returns True if it is a MapIterator type
     *
     */
    function isMapIterator(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Promise type
     *      @param v the variable to check
     *      @return returns True if it is a Promise type
     *
     */
    function isPromise(v: any): boolean;

    /**
     * @description Checks whether the given variable is an AsyncFunction type
     *      @param v the variable to check
     *      @return returns True if it is an AsyncFunction type
     *
     */
    function isAsyncFunction(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Set type
     *      @param v the variable to check
     *      @return returns True if it is a Set type
     *
     */
    function isSet(v: any): boolean;

    /**
     * @description Checks whether the given variable is a SetIterator type
     *      @param v the variable to check
     *      @return returns True if it is a SetIterator type
     *
     */
    function isSetIterator(v: any): boolean;

    /**
     * @description Checks whether the given variable is a TypedArray type
     *      @param v the variable to check
     *      @return returns True if it is a TypedArray type
     *
     */
    function isTypedArray(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Float32Array type
     *      @param v the variable to check
     *      @return returns True if it is a Float32Array type
     *
     */
    function isFloat32Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Float64Array type
     *      @param v the variable to check
     *      @return returns True if it is a Float64Array type
     *
     */
    function isFloat64Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is an Int8Array type
     *      @param v the variable to check
     *      @return returns True if it is an Int8Array type
     *
     */
    function isInt8Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is an Int16Array type
     *      @param v the variable to check
     *      @return returns True if it is an Int16Array type
     *
     */
    function isInt16Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is an Int32Array type
     *      @param v the variable to check
     *      @return returns True if it is an Int32Array type
     *
     */
    function isInt32Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Uint8Array type
     *      @param v the variable to check
     *      @return returns True if it is a Uint8Array type
     *
     */
    function isUint8Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Uint8ClampedArray type
     *      @param v the variable to check
     *      @return returns True if it is a Uint8ClampedArray type
     *
     */
    function isUint8ClampedArray(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Uint16Array type
     *      @param v the variable to check
     *      @return returns True if it is a Uint16Array type
     *
     */
    function isUint16Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Uint32Array type
     *      @param v the variable to check
     *      @return returns True if it is a Uint32Array type
     *
     */
    function isUint32Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is a function object
     *      @param v the variable to check
     *      @return returns True if it is a function object
     *
     */
    function isFunction(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Buffer object
     *      @param v the variable to check
     *      @return returns True if it is a Buffer object
     *
     */
    function isBuffer(v: any): boolean;

    /**
     * @description Checks whether the given variable is a BigInt object, not a primitive type
     *      @param v the variable to check
     *      @return returns True if it is a BigInt object
     *
     */
    function isBigIntObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Boolean object, not a primitive type
     *      @param v the variable to check
     *      @return returns True if it is a Boolean object
     *
     */
    function isBooleanObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Number object, not a primitive type
     *      @param v the variable to check
     *      @return returns True if it is a Number object
     *
     */
    function isNumberObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a String object, not a primitive type
     *      @param v the variable to check
     *      @return returns True if it is a String object
     *
     */
    function isStringObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Symbol object, not a primitive type
     *      @param v the variable to check
     *      @return returns True if it is a Symbol object
     *
     */
    function isSymbolObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a WeakMap type
     *      @param v the variable to check
     *      @return returns True if it is a WeakMap type
     *
     */
    function isWeakMap(v: any): boolean;

    /**
     * @description Checks whether the given variable is a WeakSet type
     *      @param v the variable to check
     *      @return returns True if it is a WeakSet type
     *
     */
    function isWeakSet(v: any): boolean;

    /**
     * @description Checks whether the given variable is an ArrayBuffer type
     *      @param v the variable to check
     *      @return returns True if it is an ArrayBuffer type
     *
     */
    function isArrayBuffer(v: any): boolean;

    /**
     * @description Checks whether the given variable is an ArrayBufferView type
     *      @param v the variable to check
     *      @return returns True if it is an ArrayBufferView type
     *
     */
    function isArrayBufferView(v: any): boolean;

    /**
     * @description Checks whether the given variable is a BigInt64Array type
     *      @param v the variable to check
     *      @return returns True if it is a BigInt64Array type
     *
     */
    function isBigInt64Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is a BigUint64Array type
     *      @param v the variable to check
     *      @return returns True if it is a BigUint64Array type
     *
     */
    function isBigUint64Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Float16Array type
     *      @param v the variable to check
     *      @return returns True if it is a Float16Array type
     *
     */
    function isFloat16Array(v: any): boolean;

    /**
     * @description Checks whether the given variable is an ArrayBuffer or SharedArrayBuffer type
     *      @param v the variable to check
     *      @return returns True if it is an ArrayBuffer or SharedArrayBuffer type
     *
     */
    function isAnyArrayBuffer(v: any): boolean;

    /**
     * @description Checks whether the given variable is a SharedArrayBuffer type
     *      @param v the variable to check
     *      @return returns True if it is a SharedArrayBuffer type
     *
     */
    function isSharedArrayBuffer(v: any): boolean;

    /**
     * @description Checks whether the given variable is an arguments object
     *      @param v the variable to check
     *      @return returns True if it is an arguments object
     *
     */
    function isArgumentsObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a boxed primitive object (such as new Boolean(), new String(), etc.)
     *      @param v the variable to check
     *      @return returns True if it is a boxed primitive object
     *
     */
    function isBoxedPrimitive(v: any): boolean;

    /**
     * @description Checks whether the given variable is a GeneratorFunction type
     *      @param v the variable to check
     *      @return returns True if it is a GeneratorFunction type
     *
     */
    function isGeneratorFunction(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Generator object
     *      @param v the variable to check
     *      @return returns True if it is a Generator object
     *
     */
    function isGeneratorObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Proxy instance
     *      @param v the variable to check
     *      @return returns True if it is a Proxy instance
     *
     */
    function isProxy(v: any): boolean;

    /**
     * @description Checks whether the given variable is a Module Namespace object
     *      @param v the variable to check
     *      @return returns True if it is a Module Namespace object
     *
     */
    function isModuleNamespaceObject(v: any): boolean;

    /**
     * @description Checks whether the given variable is a CryptoKey type
     *      @param v the variable to check
     *      @return returns True if it is a CryptoKey type
     *
     */
    function isCryptoKey(v: any): boolean;

    /**
     * @description Checks whether the given variable is a KeyObject type
     *      @param v the variable to check
     *      @return returns True if it is a KeyObject type
     *
     */
    function isKeyObject(v: any): boolean;

}

