/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/assert.d.ts" />
/**
 * @description The assert_strict module provides the strict comparison-mode assertion functions;
 * every member that compares values uses strict equality
 *
 *  It is the strict counterpart of the assert module: `equal`/`notEqual` compare with
 *  `===`/`!==` and `deepEqual`/`notDeepEqual` compare deeply with strict leaf values, while
 *  every other member (`ok`, `throws`, `rejects`, `property`, ...) behaves exactly like its
 *  assert counterpart. Prefer this module for new code, as Node.js does.
 *
 *  Main capabilities:
 *
 *  - **Strict comparison**: `equal`, `notEqual` (mapped to strictEqual/notStrictEqual),
 *    `strictEqual`, `notStrictEqual`;
 *  - **Strict deep comparison**: `deepEqual`, `notDeepEqual` (mapped to
 *    deepStrictEqual/notDeepStrictEqual), `deepStrictEqual`, `notDeepStrictEqual`;
 *  - **Truthiness and failure**: `ok`, `notOk`, `exist`, `notExist`, `fail`;
 *  - **Type and shape checks**: `isTrue` through `isNotBoolean`, `typeOf`, `notTypeOf`,
 *    `property`, `deepProperty`, `propertyVal`, `deepPropertyVal` and their negations;
 *  - **Regular expression matching**: `match`, `doesNotMatch`;
 *  - **Ordered comparison**: `closeTo`, `notCloseTo`, `lessThan`, `notLessThan`,
 *    `greaterThan`, `notGreaterThan`;
 *  - **Exception checks**: `throws`, `doesNotThrow`, `rejects`; **error helpers**:
 *    `AssertionError`, `ifError`.
 *
 *  Concepts:
 *
 *  - **Difference from assert**: this module is the same object as `require('assert').strict`,
 *    so `strict.equal(a, b)` is `assert.strictEqual(a, b)`. The table lists every member whose
 *    semantics change; every member not listed is identical to its assert counterpart.
 *
 *    | assert_strict | behaves as assert |
 *    | --- | --- |
 *    | `equal` | `strictEqual` (`===`) |
 *    | `notEqual` | `notStrictEqual` (`!==`) |
 *    | `deepEqual` | `deepStrictEqual` |
 *    | `notDeepEqual` | `notDeepStrictEqual` |
 *    | `strictEqual`, `notStrictEqual` | unchanged |
 *    | `deepStrictEqual`, `notDeepStrictEqual` | unchanged |
 *
 *  - **Strict equality**: leaves are compared with `===`, so `1` and `'1'` are never equal and
 *    object identity is required for the non-deep checks. Deep comparison still compares Dates
 *    by time value, RegExps by source and flags and Buffers by content, and it still ignores
 *    prototypes and symbol-keyed properties; see assert for the full deep comparison rules.
 *  - **AssertionError**: failures use the same class as assert
 *    (`strict.AssertionError === assert.AssertionError`) with name 'AssertionError' and code
 *    'ERR_ASSERTION'; the operator field names the underlying strict operator.
 *    `generatedMessage` is false when a message was supplied.
 *  - **Message argument**: as in assert, a string (or String object) is used as-is, a value
 *    with its own string form is rendered, and other values are ignored.
 *
 *  Import:
 *  ```JavaScript
 *  const strict = require('assert/strict');
 *  // equivalent: require('assert').strict, require('fibjs:assert/strict'),
 *  // require('node:assert/strict')
 *  ```
 *
 *  Example 1 — strict equality and the mapped names:
 *  ```JavaScript
 *  const strict = require('assert/strict');
 *
 *  strict.equal(1, 1);                   // same as strictEqual
 *  strict.deepEqual({ a: 1 }, { a: 1 }); // same as deepStrictEqual
 *  strict.notEqual(1, '1');              // different types are unequal
 *  console.log('strict checks passed');
 *  ```
 *
 *  Example 2 — a caught failure reports the strict operator:
 *  ```JavaScript
 *  const strict = require('assert/strict');
 *
 *  try {
 *      strict.equal(1, '1');
 *  } catch (err) {
 *      console.log(err.name);                 // AssertionError
 *      console.log(err.operator);             // strictEqual
 *      console.log(JSON.stringify([err.actual, err.expected])); // [1,"1"]
 *  }
 *  ```
 *
 *  Example 3 — the exception checks are unchanged:
 *  ```JavaScript
 *  const strict = require('assert/strict');
 *
 *  strict.throws(() => JSON.parse('{'), SyntaxError);
 *  strict.doesNotThrow(() => JSON.parse('{}'));
 *
 *  (async () => {
 *      await strict.rejects(Promise.reject(new Error('down')), /down/);
 *      console.log('rejection matched');
 *  })();
 *  ```
 *
 *  Notes:
 *
 *  - `strict.ok` is the loose assert module object itself (`strict.ok === require('assert')`),
 *    because the alias points at that module; calling it behaves like assert ok.
 *  - `strict.strict` does not exist; require the loose module directly to mix both modes.
 *  - Node.js exposes the same `assert/strict` entry point with the same mapping, and fibjs also
 *    registers the aliases `fibjs:assert/strict` and `node:assert/strict`.
 *  - Besides `strictEqual`-style checks, every other member shares the implementation of
 *    assert, including the crash of `property` on a string receiver and the leniency of the
 *    `throws` property filter.
 *
 */
declare module 'assert/strict' {
    /**
     * @description The callable module object itself; tests that the value is truthy
     *
     *      `strict(value, message)` is the same check as `ok` and the failure carries the
     *      operator 'ok'; the parameter defaults to undefined, so `strict()` fails.
     *
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function assert_strict(actual?: any, msg?: any): void;

    namespace assert_strict {
        /**
         * @description The AssertionError constructor used for every failed assertion
         *
         *      The same class as assert.AssertionError (`strict.AssertionError ===
         *      assert.AssertionError`): name is 'AssertionError', code is 'ERR_ASSERTION' and the
         *      instances carry the actual, expected, operator and generatedMessage fields. Every
         *      failure of this module, including the aliased equal/deepEqual ones, throws it; see
         *      assert.AssertionError for the constructor options and fields.
         *
         */
        const AssertionError: (...args: any[])=>any;

        /**
         * ! Tests that the value is truthy; the assertion fails if it is false; an alias of the module
         *
         *      Unlike the other members, `strict.ok` is the loose assert module object itself
         *      (`strict.ok === require('assert')`), because the alias points at that module; calling
         *      it behaves exactly like the loose `ok`. The failure carries the operator 'ok'.
         *
         */
        const ok: typeof import ('assert');

        /**
         * @description Tests that the value is falsy; the assertion fails if it is true
         *
         *      The exact negation of `ok`, shared with the loose assert module; the failure carries
         *      the operator 'notOk'. Node.js has no notOk, the equivalent is
         *      `assert.ok(!value)`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function notOk(actual: any, msg?: any): void;

        /**
         * @description Tests that the value strictly equals the expected value
         *
         *      In this module `equal` is an alias of `strictEqual`: it uses `===`, so `equal(1,
         *      '1')` fails, and the failure carries the operator 'strictEqual' (not '==', which the
         *      loose module uses). `strict.equal` and `strict.strictEqual` are the same behavior;
         *      Node.js applies the same alias.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function equal(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value does not strictly equal the expected value
         *
         *      In this module `notEqual` is an alias of `notStrictEqual`: two references to the same
         *      object are equal and fail the check, while `1` and `'1'` are unequal and pass it. The
         *      failure carries the operator 'notStrictEqual'.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function notEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value strictly equals the expected value
         *
         *      Uses the JavaScript `===` operator, exactly as in the loose module: `1` and `'1'`
         *      are not equal, NaN is not equal to NaN, +0 and -0 are equal, and objects must be the
         *      same reference. A custom message is prepended to the generated diff; the failure
         *      carries the operator 'strictEqual'.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function strictEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value does not strictly equal the expected value
         *
         *      The exact negation of `strictEqual`, shared with the loose module;
         *      `notStrictEqual(1, '1')` passes and `notStrictEqual(obj, obj)` fails. The failure
         *      carries the operator 'notStrictEqual'.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function notStrictEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value strictly deeply equals the expected value
         *
         *      In this module `deepEqual` is an alias of `deepStrictEqual`: leaf values are
         *      compared with `===`, so `deepEqual({ a: 1 }, { a: '1' })` fails, and the failure
         *      carries the operator 'deepStrictEqual'. Dates are compared by time value, RegExps by
         *      source and flags and Buffers by content, while prototypes and symbol-keyed
         *      properties are ignored; a custom message is prepended to the generated diff.
         *
         *      Example — strict deep comparison:
         *      ```JavaScript
         *      const strict = require('assert/strict');
         *
         *      strict.deepEqual({ a: 1, b: [2, 3] }, { a: 1, b: [2, 3] });
         *      strict.deepEqual(new Date(1000), new Date(1000));
         *      try {
         *          strict.deepEqual({ a: 1 }, { a: '1' });
         *      } catch (err) {
         *          console.log(err.operator); // deepStrictEqual
         *      }
         *      ```
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function deepEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value does not strictly deeply equal the expected value
         *
         *      In this module `notDeepEqual` is an alias of `notDeepStrictEqual`;
         *      `notDeepEqual({ a: 1 }, { a: '1' })` passes while structurally and strictly
         *      identical values fail. The failure carries the operator 'notDeepStrictEqual'.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function notDeepEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value strictly deeply equals the expected value
         *
         *      Shared with the loose module: leaf values are compared with `===` and the rules for
         *      Dates, RegExps, arrays and Buffers are the same as `deepEqual`; prototypes and
         *      symbol-keyed properties are still not compared, which differs from Node.js. A custom
         *      message is prepended to the generated diff; the failure carries the operator
         *      'deepStrictEqual'.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function deepStrictEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value does not strictly deeply equal the expected value
         *
         *      The exact negation of `deepStrictEqual`, shared with the loose module: values whose
         *      leaves differ in type pass and strictly identical values fail. The failure carries
         *      the operator 'notDeepStrictEqual'.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function notDeepStrictEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the string matches the expected regular expression
         *
         *      Shared with the loose module: the actual value is declared String, so a String
         *      object, a Date (its ISO form), a Buffer (its utf8 bytes) and an object with its own
         *      toString are converted, while values without a string form fail with a TypeError
         *      [20005]; Node.js requires a real string. The failure carries the operator 'match'.
         *
         *      @param actual the string to test
         *      @param expected the expected regular expression
         *      @param msg the message when the assertion fails
         *
         */
        function match(actual: string, expected: FIBJS.GeneralObject, msg?: any): void;

        /**
         * @description Tests that the string does not match the expected regular expression
         *
         *      The exact negation of `match`, shared with the loose module, including the string
         *      conversion rules and the TypeError [20005] for values without a string form. The
         *      failure carries the operator 'doesNotMatch'.
         *
         *      @param actual the string to test
         *      @param expected the expected regular expression
         *      @param msg the message when the assertion fails
         *
         */
        function doesNotMatch(actual: string, expected: FIBJS.GeneralObject, msg?: any): void;

        /**
         * @description Tests that the value is approximately equal to the expected value
         *
         *      Shared with the loose module: the check is `Math.abs(actual - expected) <= delta`
         *      with the bound inclusive, and a value that converts to NaN throws a TypeError [20004]
         *      instead of failing the assertion. Node.js has no closeTo.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param delta the allowed absolute difference
         *      @param msg the message when the assertion fails
         *
         */
        function closeTo(actual: any, expected: any, delta: any, msg?: any): void;

        /**
         * @description Tests that the value is not approximately equal to the expected value
         *
         *      The exact negation of `closeTo` (`Math.abs(actual - expected) > delta`), shared
         *      with the loose module and with the same TypeError [20004] for NaN conversions.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param delta the allowed absolute difference
         *      @param msg the message when the assertion fails
         *
         */
        function notCloseTo(actual: any, expected: any, delta: any, msg?: any): void;

        /**
         * @description Tests that the value is less than the expected value
         *
         *      Shared with the loose module: numeric when either side is a number (or a numeric
         *      string next to a number), lexicographic UTF-8 when both sides are non-numeric
         *      strings, and a TypeError [20004] for values that cannot be converted to a number.
         *      Node.js has no lessThan.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function lessThan(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value is not less than the expected value
         *
         *      The negation of `lessThan` (actual >= expected), shared with the loose module and
         *      using the same conversion rules and TypeError [20004].
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function notLessThan(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value is greater than the expected value
         *
         *      The mirror of `lessThan` (actual > expected), shared with the loose module; the
         *      generated failure message and operator field reuse the notLessThan wording ('to be
         *      at least'). Node.js has no greaterThan.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function greaterThan(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value is not greater than the expected value
         *
         *      The negation of `greaterThan` (actual <= expected), shared with the loose module and
         *      using the same conversion rules and TypeError [20004].
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function notGreaterThan(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the variable exists
         *
         *      Shared with the loose module: a value exists when it is neither null nor undefined,
         *      so false, 0, '' and NaN exist. Node.js has no exist.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function exist(actual: any, msg?: any): void;

        /**
         * @description Tests that the variable does not exist
         *
         *      The exact negation of `exist`, shared with the loose module: only null and
         *      undefined do not exist, so 0, '' and false all fail this check.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function notExist(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is boolean true
         *
         *      Shared with the loose module and strict by nature: 1, 'true' and new Boolean(true)
         *      all fail. Node.js has no isTrue.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isTrue(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not boolean true
         *
         *      The exact negation of `isTrue`, shared with the loose module: every value except
         *      the true primitive passes, including 1 and new Boolean(true).
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotTrue(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is boolean false
         *
         *      Shared with the loose module: only the false primitive passes, so 0, '' and
         *      new Boolean(false) fail. Node.js has no isFalse.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isFalse(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not boolean false
         *
         *      The exact negation of `isFalse`, shared with the loose module: every value except
         *      the false primitive passes, including 0 and new Boolean(false).
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotFalse(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is Null
         *
         *      Shared with the loose module: only the null primitive passes; undefined is not null
         *      and fails this check, use `isUndefined` for it. Node.js has no isNull.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNull(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not Null
         *
         *      The exact negation of `isNull`, shared with the loose module: every value except
         *      null passes, undefined included.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotNull(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is undefined
         *
         *      Shared with the loose module: only the undefined primitive passes; null is defined
         *      for this check and fails it, use `isNull` for null. Node.js has no isUndefined.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isUndefined(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not undefined
         *
         *      The exact negation of `isUndefined`, shared with the loose module: null and every
         *      other value pass. Node.js has no isDefined.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isDefined(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is a function
         *
         *      Shared with the loose module: classes, async functions and generator functions are
         *      functions too. Node.js has no isFunction.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isFunction(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not a function
         *
         *      The exact negation of `isFunction`, shared with the loose module: every value that
         *      cannot be called passes, so objects and arrays pass while classes fail.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotFunction(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is an object
         *
         *      Shared with the loose module: arrays and functions count as objects, boxed
         *      primitives count too, while null, undefined, primitives and symbols do not.
         *      `typeOf(value, 'object')` delegates to this check. Node.js has no isObject.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isObject(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not an object
         *
         *      The exact negation of `isObject`, shared with the loose module: primitives, null,
         *      undefined and symbols pass, while arrays, functions and boxed primitives fail.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotObject(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is an array
         *
         *      Shared with the loose module: an Array subclass passes, while typed arrays and
         *      Buffer do not. Node.js has no isArray; the equivalent is
         *      `assert.ok(Array.isArray(value))`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isArray(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not an array
         *
         *      The exact negation of `isArray`, shared with the loose module: any value that is
         *      not an Array passes, including typed arrays, Buffer and arguments objects.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotArray(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is a string
         *
         *      Shared with the loose module: both a string primitive and a String object pass,
         *      while a Buffer does not (it is a Uint8Array). Node.js has no isString.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isString(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not a string
         *
         *      The exact negation of `isString`, shared with the loose module: a Buffer, a number
         *      and a String-like object without the String prototype all pass.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotString(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is a number
         *
         *      Shared with the loose module: NaN passes and a Number object does not. Node.js has
         *      no isNumber.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNumber(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not a number
         *
         *      The exact negation of `isNumber`, shared with the loose module: any value that is
         *      not a number primitive passes, including a Number object and numeric strings.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotNumber(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is a boolean
         *
         *      Shared with the loose module: only the true and false primitives pass, a Boolean
         *      object does not. Node.js has no isBoolean.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isBoolean(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not a boolean
         *
         *      The exact negation of `isBoolean`, shared with the loose module: everything except
         *      the true and false primitives passes, including 0, '' and a Boolean object.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotBoolean(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is of the given type
         *
         *      Shared with the loose module: the accepted names are 'array', 'function', 'string',
         *      'object', 'number', 'boolean', 'null' and 'undefined', where 'object' also matches
         *      arrays and functions; any other name throws a TypeError [20004]. Node.js has no
         *      typeOf.
         *
         *      @param actual the value to test
         *      @param type the specified type
         *      @param msg the message when the assertion fails
         *
         */
        function typeOf(actual: any, type: string, msg?: any): void;

        /**
         * @description Tests that the value is not of the given type
         *
         *      The exact negation of `typeOf`, shared with the loose module, with the same eight
         *      type names and TypeError [20004] for anything else.
         *
         *      @param actual the value to test
         *      @param type the specified type
         *      @param msg the message when the assertion fails
         *
         */
        function notTypeOf(actual: any, type: string, msg?: any): void;

        /**
         * @description Tests that the object contains the specified property
         *
         *      Shared with the loose module: the lookup includes inherited properties, the receiver
         *      must be an object and the property name a string, and other values throw a TypeError
         *      [20004]. A primitive string receiver passes the type check but crashes the process
         *      in this version, so pass objects only. Node.js has no property.
         *
         *      @param object the object to test
         *      @param prop the property to test
         *      @param msg the message when the assertion fails
         *
         */
        function property(object: any, prop: any, msg?: any): void;

        /**
         * @description Tests that the object does not contain the specified property
         *
         *      The exact negation of `property`, shared with the loose module: an inherited
         *      property fails this check and the same TypeError [20004] applies to non-object
         *      receivers and non-string names.
         *
         *      @param object the object to test
         *      @param prop the property to test
         *      @param msg the message when the assertion fails
         *
         */
        function notProperty(object: any, prop: any, msg?: any): void;

        /**
         * @description Deeply tests that the object contains the specified property
         *
         *      Shared with the loose module: the path is split on '.', so 'a.b.0' reaches array
         *      elements and a property name containing a dot cannot be addressed; a missing
         *      intermediate value fails the assertion instead of throwing. Node.js has no
         *      deepProperty.
         *
         *      @param object the object to test
         *      @param prop the property to test, separated by "."
         *      @param msg the message when the assertion fails
         *
         */
        function deepProperty(object: any, prop: any, msg?: any): void;

        /**
         * @description Deeply tests that the object does not contain the specified property
         *
         *      The exact negation of `deepProperty`, shared with the loose module: a path through a
         *      missing intermediate value counts as absent and passes.
         *
         *      @param object the object to test
         *      @param prop the property to test, separated by "."
         *      @param msg the message when the assertion fails
         *
         */
        function notDeepProperty(object: any, prop: any, msg?: any): void;

        /**
         * @description Tests that the specified property in the object has the given value
         *
         *      Shared with the loose module and already strict there: the value is compared with
         *      `===`, so `propertyVal(obj, 'a', 1)` fails when the property holds '1' and a missing
         *      property yields undefined and fails. Node.js has no propertyVal.
         *
         *      @param object the object to test
         *      @param prop the property to test
         *      @param value the given value
         *      @param msg the message when the assertion fails
         *
         */
        function propertyVal(object: any, prop: any, value: any, msg?: any): void;

        /**
         * @description Tests that the specified property in the object does not have the given value
         *
         *      The exact negation of `propertyVal`, shared with the loose module and using the same
         *      strict comparison.
         *
         *      @param object the object to test
         *      @param prop the property to test
         *      @param value the given value
         *      @param msg the message when the assertion fails
         *
         */
        function propertyNotVal(object: any, prop: any, value: any, msg?: any): void;

        /**
         * @description Deeply tests that the specified property in the object has the given value
         *
         *      Shared with the loose module: the dotted path is walked and the leaf is compared
         *      with strict equality; the receiver must be an object and the path a string,
         *      otherwise a TypeError [20004] is thrown.
         *
         *      @param object the object to test
         *      @param prop the property to test, separated by "."
         *      @param value the given value
         *      @param msg the message when the assertion fails
         *
         */
        function deepPropertyVal(object: any, prop: any, value: any, msg?: any): void;

        /**
         * @description Deeply tests that the specified property in the object does not have the given value
         *
         *      The exact negation of `deepPropertyVal`, shared with the loose module: a missing
         *      intermediate value or a different (strict) value passes.
         *
         *      @param object the object to test
         *      @param prop the property to test, separated by "."
         *      @param value the given value
         *      @param msg the message when the assertion fails
         *
         */
        function deepPropertyNotVal(object: any, prop: any, value: any, msg?: any): void;

        /**
         * @description Tests that the given code throws an error
         *
         *      Shared with the loose module and unaffected by the strict mode: the block is called
         *      synchronously without arguments and a return without a throw fails with 'Missing
         *      expected exception'. The error argument can be a RegExp matching the string form of
         *      the thrown value, an error class checked with instanceof, an arrow validation
         *      function that must return exactly true, or a property filter object compared with
         *      deepStrictEqual (a property whose actual value is undefined is skipped).
         *
         *      An error thrown inside a validation function is propagated as-is. The failure
         *      carries the operator 'throws'; a custom message is used when a filter was supplied
         *      and did not match. Node.js accepts the same forms but also treats normal functions
         *      as validators and appends a 'Caught error' excerpt.
         *
         *      Example — a class and an arrow validation function:
         *      ```JavaScript
         *      const strict = require('assert/strict');
         *
         *      strict.throws(() => JSON.parse('{'), SyntaxError);
         *      strict.throws(() => { throw new TypeError('bad'); }, (err) => {
         *          return err.message === 'bad';
         *      });
         *      console.log('exceptions matched');
         *      ```
         *      @param block the code to test, given as a function
         *      @param error the expected error as a RegExp, error class, arrow validator or filter object
         *      @param msg the message when the assertion fails
         *
         */
        function throws(block: ()=>void, error: any, msg?: any): void;

        /**
         * ! Tests that the given code throws an error, with only a message
         *
         *      Equivalent to calling the three-argument overload with an undefined error filter;
         *      the message is ignored when nothing is thrown, because that failure always reports
         *      the generated 'Missing expected exception' text.
         *      @param block the code to test, given as a function
         *      @param msg the message when the assertion fails
         *
         */
        function throws(block: ()=>void, msg?: any): void;

        /**
         * @description Tests that the given code does not throw an error
         *
         *      Shared with the loose module: the block is called synchronously without arguments
         *      and any thrown value fails the assertion, with the caught value in the actual field.
         *      There is no error filter; use `throws` with a validation function to allow only some
         *      errors, and a custom message replaces the default 'Got unwanted exception'.
         *
         *      @param block the code to test, given as a function
         *      @param msg the message when the assertion fails
         *
         */
        function doesNotThrow(block: ()=>void, msg?: any): void;

        /**
         * @description Tests that the given code rejects, and returns a promise
         *
         *      Shared with the loose module and unaffected by the strict mode. The block is called
         *      synchronously without arguments: a synchronous error rejects the returned promise
         *      with that error, a non-promise return rejects it with a TypeError ('The rsult of
         *      the function is not a promise.' - the typo is in the generated message), and a
         *      returned promise is awaited. The error argument filters the rejection exactly like
         *      `throws` (RegExp, error class, arrow validator or property filter object).
         *
         *      When the awaited promise resolves the assertion fails with 'Missing expected
         *      rejection'. The returned promise resolves to undefined when the rejection matched
         *      and rejects with the generated AssertionError otherwise; the operator is 'rejects'.
         *      Node.js returns a promise with the same shape but rejects with
         *      ERR_INVALID_RETURN_VALUE for a non-promise result.
         *
         *      Example — await a function and a promise rejection:
         *      ```JavaScript
         *      const strict = require('assert/strict');
         *
         *      (async () => {
         *          await strict.rejects(Promise.reject(new TypeError('bad input')), TypeError);
         *          await strict.rejects(async () => { throw new Error('later'); }, /later/);
         *          console.log('all rejections matched');
         *      })();
         *      ```
         *      @param block the code to test, given as a function returning a promise
         *      @param error the expected error as a RegExp, error class, arrow validator or filter object
         *      @param msg the message when the assertion fails
         *      @return returns a Promise
         *
         */
        function rejects(block: ()=>void, error: any, msg?: any): Promise;

        /**
         * ! Tests that the given code rejects, with only a message
         *
         *      Equivalent to the three-argument overload with an undefined error filter; the
         *      message is ignored when nothing is rejected.
         *      @param block the code to test, given as a function returning a promise
         *      @param msg the message when the assertion fails
         *      @return returns a Promise
         *
         */
        function rejects(block: ()=>void, msg?: any): Promise;

        /**
         * @description Tests that the given promise rejects
         *
         *      Shared with the loose module: the promise is awaited and its rejection is filtered
         *      by the error argument as in the function overload, while a resolution fails the
         *      assertion with 'Missing expected rejection'. The returned promise resolves to
         *      undefined when the rejection matched and rejects with the generated AssertionError
         *      otherwise; Node.js has the same form.
         *
         *      @param result the code to test, given as a Promise
         *      @param error the expected error as a RegExp, error class, arrow validator or filter object
         *      @param msg the message when the assertion fails
         *      @return returns a Promise
         *
         */
        function rejects(result: Promise, error: any, msg?: any): Promise;

        /**
         * ! Tests that the given promise rejects, with only a message
         *
         *      Equivalent to the three-argument overload with an undefined error filter; the
         *      message is ignored when nothing is rejected.
         *      @param result the code to test, given as a Promise
         *      @param msg the message when the assertion fails
         *      @return returns a Promise
         *
         */
        function rejects(result: Promise, msg?: any): Promise;

        /**
         * @description Throws the given value when it is truthy
         *
         *      Shared with the loose module: the value is rethrown as-is, not wrapped, which makes
         *      it the standard trailing callback helper (`if (err) throw err`). Falsy values -
         *      undefined, null, false, 0, '' and NaN - pass. Node.js wraps the value in an
         *      AssertionError instead, so a caught value that is not an Error differs.
         *
         *      @param object the argument
         *
         */
        function ifError(object?: any): void;

    }

    export = assert_strict;

}

