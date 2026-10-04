/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/assert.d.ts" />
/**
 * @description Strict assertion test module; if the tested value is false, an error is reported, and the error behavior can be configured to continue running or to throw the error
 *
 *  Reference method:
 *  ```JavaScript
 *  var assert = require('assert').strict;
 *  ```
 *  Or reference it through the test module:
 *  ```JavaScript
 *  var test = require('test');
 *  var assert = test.assert.strict;
 *  ```
 *  Or configure it through test.setup:
 *  ```JavaScript
 *  require("test").setup();
 *  ```
 *
 */
declare module 'assert_strict' {
    /**
     * @description Assertion error object
     */
    const AssertionError: (...args: any[])=>any;

    /**
     * @description Tests that the value is truthy; the assertion fails if it is false
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function Function(actual?: any, msg?: any): void;

    /**
     * ! Tests that the value is truthy; the assertion fails if it is false; an alias of the assert module
     */
    const ok: typeof import ('assert');

    /**
     * @description Tests that the value is falsy; the assertion fails if it is true
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function notOk(actual: any, msg?: any): void;

    /**
     * @description Tests that the value equals the expected value; the assertion fails if they are not equal
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function equal(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value does not equal the expected value; the assertion fails if they are equal
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function notEqual(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value strictly equals the expected value; the assertion fails if they are not equal
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function strictEqual(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value does not strictly equal the expected value; the assertion fails if they are equal
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function notStrictEqual(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value deeply equals the expected value; the assertion fails if they are not equal
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function deepEqual(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value does not deeply equal the expected value; the assertion fails if they are equal
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function notDeepEqual(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value strictly deeply equals the expected value; the assertion fails if they are not equal
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function deepStrictEqual(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value does not strictly deeply equal the expected value; the assertion fails if they are equal
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function notDeepStrictEqual(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the string contains the expected string, otherwise the assertion fails
     *      @param actual the string to test
     *      @param expected the expected regular expression
     *      @param msg the message when the assertion fails
     *
     */
    function match(actual: string, expected: FIBJS.GeneralObject, msg?: any): void;

    /**
     * @description Tests that the string does not contain the expected string, otherwise the assertion fails
     *      @param actual the string to test
     *      @param expected the expected regular expression
     *      @param msg the message when the assertion fails
     *
     */
    function doesNotMatch(actual: string, expected: FIBJS.GeneralObject, msg?: any): void;

    /**
     * @description Tests that the value is approximately equal to the expected value, otherwise the assertion fails
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param delta the decimal precision of the approximation
     *      @param msg the message when the assertion fails
     *
     */
    function closeTo(actual: any, expected: any, delta: any, msg?: any): void;

    /**
     * @description Tests that the value is not approximately equal to the expected value, otherwise the assertion fails
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param delta the decimal precision of the approximation
     *      @param msg the message when the assertion fails
     *
     */
    function notCloseTo(actual: any, expected: any, delta: any, msg?: any): void;

    /**
     * @description Tests that the value is less than the expected value; the assertion fails if it is greater than or equal to it
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function lessThan(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value is not less than the expected value; the assertion fails if it is less
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function notLessThan(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value is greater than the expected value; the assertion fails if it is less than or equal to it
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function greaterThan(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the value is not greater than the expected value; the assertion fails if it is greater
     *      @param actual the value to test
     *      @param expected the expected value
     *      @param msg the message when the assertion fails
     *
     */
    function notGreaterThan(actual: any, expected: any, msg?: any): void;

    /**
     * @description Tests that the variable exists; the assertion fails if it is false
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function exist(actual: any, msg?: any): void;

    /**
     * @description Tests that the variable does not exist; the assertion fails if it is true
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function notExist(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is boolean true, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isTrue(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not boolean true, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotTrue(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is boolean false, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isFalse(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not boolean false, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotFalse(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is Null, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNull(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not Null, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotNull(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is undefined, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isUndefined(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not undefined, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isDefined(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is a function, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isFunction(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not a function, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotFunction(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is an object, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isObject(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not an object, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotObject(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is an array, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isArray(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not an array, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotArray(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is a string, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isString(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not a string, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotString(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is a number, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNumber(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not a number, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotNumber(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is a boolean, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isBoolean(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is not a boolean, otherwise the assertion fails
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function isNotBoolean(actual: any, msg?: any): void;

    /**
     * @description Tests that the value is of the given type, otherwise the assertion fails
     *      @param actual the value to test
     *      @param type the specified type
     *      @param msg the message when the assertion fails
     *
     */
    function typeOf(actual: any, type: string, msg?: any): void;

    /**
     * @description Tests that the value is not of the given type, otherwise the assertion fails
     *      @param actual the value to test
     *      @param type the specified type
     *      @param msg the message when the assertion fails
     *
     */
    function notTypeOf(actual: any, type: string, msg?: any): void;

    /**
     * @description Tests that the object contains the specified property, otherwise the assertion fails
     *      @param object the object to test
     *      @param prop the property to test
     *      @param msg the message when the assertion fails
     *
     */
    function property(object: any, prop: any, msg?: any): void;

    /**
     * @description Tests that the object does not contain the specified property, otherwise the assertion fails
     *      @param object the object to test
     *      @param prop the property to test
     *      @param msg the message when the assertion fails
     *
     */
    function notProperty(object: any, prop: any, msg?: any): void;

    /**
     * @description Deeply tests that the object contains the specified property, otherwise the assertion fails
     *      @param object the object to test
     *      @param prop the property to test, separated by "."
     *      @param msg the message when the assertion fails
     *
     */
    function deepProperty(object: any, prop: any, msg?: any): void;

    /**
     * @description Deeply tests that the object does not contain the specified property, otherwise the assertion fails
     *      @param object the object to test
     *      @param prop the property to test, separated by "."
     *      @param msg the message when the assertion fails
     *
     */
    function notDeepProperty(object: any, prop: any, msg?: any): void;

    /**
     * @description Tests that the specified property in the object has the given value, otherwise the assertion fails
     *      @param object the object to test
     *      @param prop the property to test
     *      @param value the given value
     *      @param msg the message when the assertion fails
     *
     */
    function propertyVal(object: any, prop: any, value: any, msg?: any): void;

    /**
     * @description Tests that the specified property in the object does not have the given value, otherwise the assertion fails
     *      @param object the object to test
     *      @param prop the property to test
     *      @param value the given value
     *      @param msg the message when the assertion fails
     *
     */
    function propertyNotVal(object: any, prop: any, value: any, msg?: any): void;

    /**
     * @description Deeply tests that the specified property in the object has the given value, otherwise the assertion fails
     *      @param object the object to test
     *      @param prop the property to test, separated by "."
     *      @param value the given value
     *      @param msg the message when the assertion fails
     *
     */
    function deepPropertyVal(object: any, prop: any, value: any, msg?: any): void;

    /**
     * @description Deeply tests that the specified property in the object does not have the given value, otherwise the assertion fails
     *      @param object the object to test
     *      @param prop the property to test, separated by "."
     *      @param value the given value
     *      @param msg the message when the assertion fails
     *
     */
    function deepPropertyNotVal(object: any, prop: any, value: any, msg?: any): void;

    /**
     * @description Tests that the given code throws an error; the assertion fails if nothing is thrown
     *      @param block the code to test, given as a function
     *      @param error the specified error, which can be a RegExp/Function/Object/Error
     *      @param msg the message when the assertion fails
     *
     */
    function throws(block: ()=>void, error: any, msg?: any): void;

    /**
     * @description Tests that the given code throws an error; the assertion fails if nothing is thrown
     *      @param block the code to test, given as a function
     *      @param msg the message when the assertion fails
     *
     */
    function throws(block: ()=>void, msg?: any): void;

    /**
     * @description Tests that the given code does not throw an error; the assertion fails if it throws
     *      @param block the code to test, given as a function
     *      @param msg the message when the assertion fails
     *
     */
    function doesNotThrow(block: ()=>void, msg?: any): void;

    /**
     * @description Tests that the given code throws an error; the assertion fails if nothing is thrown
     *      @param block the code to test, given as a function
     *      @param error the specified error, which can be a RegExp/Function/Object/Error
     *      @param msg the message when the assertion fails
     *      @return returns a Promise
     *
     */
    function rejects(block: ()=>void, error: any, msg?: any): Promise;

    /**
     * @description Tests that the given code throws an error; the assertion fails if nothing is thrown
     *      @param block the code to test, given as a function
     *      @param msg the message when the assertion fails
     *      @return returns a Promise
     *
     */
    function rejects(block: ()=>void, msg?: any): Promise;

    /**
     * @description Tests that the given code throws an error; the assertion fails if nothing is thrown
     *      @param result the code to test, given as a Promise
     *      @param error the specified error, which can be a RegExp/Function/Object/Error
     *      @param msg the message when the assertion fails
     *      @return returns a Promise
     *
     */
    function rejects(result: Promise, error: any, msg?: any): Promise;

    /**
     * @description Tests that the given code throws an error; the assertion fails if nothing is thrown
     *      @param result the code to test, given as a Promise
     *      @param msg the message when the assertion fails
     *      @return returns a Promise
     *
     */
    function rejects(result: Promise, msg?: any): Promise;

    /**
     * @description Throws if the argument is true
     *      @param object the argument
     *
     */
    function ifError(object?: any): void;

}

