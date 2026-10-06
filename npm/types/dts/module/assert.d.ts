/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/assert_strict.d.ts" />
/**
 * @description The assert module provides the legacy comparison-mode assertion functions used to
 * test invariants in unit tests and in application code
 *
 *  A failing assertion throws an AssertionError; a passing assertion returns undefined, so
 *  assertions are statements rather than conditions. Every check has a negated counterpart
 *  (`equal`/`notEqual`, `property`/`notProperty`, `throws`/`doesNotThrow`), and the strict
 *  comparison variants live in the assert_strict module, available as `assert.strict`.
 *
 *  Main capabilities:
 *
 *  - **Truthiness and failure**: `ok` (the callable module itself), `notOk`, `exist`, `notExist`,
 *    `fail`;
 *  - **Loose comparison**: `equal`, `notEqual`, `closeTo`, `notCloseTo`, `lessThan`,
 *    `notLessThan`, `greaterThan`, `notGreaterThan`;
 *  - **Strict comparison**: `strictEqual`, `notStrictEqual`;
 *  - **Deep comparison**: `deepEqual`, `notDeepEqual`, `deepStrictEqual`, `notDeepStrictEqual`;
 *  - **Regular expression matching**: `match`, `doesNotMatch`;
 *  - **Type and shape checks**: `isTrue` through `isNotBoolean`, `typeOf`, `notTypeOf`,
 *    `property`, `deepProperty`, `propertyVal`, `deepPropertyVal` and their negations;
 *  - **Exception checks**: `throws`, `doesNotThrow`, `rejects`;
 *  - **Error helpers**: `AssertionError`, `ifError`, and `strict`, the strict module.
 *
 *  Concepts:
 *
 *  - **Loose comparison mode**: `equal`/`notEqual` use the JavaScript `==`/`!=` operators, so
 *    `1` equals `'1'` and `null` equals `undefined`; `deepEqual`/`notDeepEqual` apply the same
 *    loose comparison to nested values. `strictEqual`/`notStrictEqual` and
 *    `deepStrictEqual`/`notDeepStrictEqual` use `===`, and the strict module maps the loose
 *    names onto them (see assert_strict).
 *  - **AssertionError**: every failure throws an AssertionError whose fields are `name`
 *    ('AssertionError'), `code` ('ERR_ASSERTION'), `message`, `generatedMessage` and the
 *    comparison data `actual`, `expected` and `operator`. `assert.AssertionError` is the
 *    constructor, so `err instanceof assert.AssertionError` identifies a failure.
 *  - **Message argument**: the optional last argument is the failure message; a string (or
 *    String object) is used as-is, a value with its own string form (a Date, a Buffer, an
 *    object with its own toString) is rendered, and other values are ignored. When a message
 *    was supplied `generatedMessage` is false, except for a `throws`/`rejects` failure where
 *    nothing was thrown or rejected - that always reports the generated message.
 *  - **Deep comparison**: `deepEqual` and `deepStrictEqual` compare Dates by time value,
 *    RegExps by source and flags, arrays element by element, Buffers through their equals
 *    method, and other objects by their enumerable property names - including inherited ones -
 *    where both objects must have the same names; cyclic structures are supported,
 *    prototypes are not compared and symbol-keyed properties are ignored.
 *  - **Exception checks**: `throws`/`doesNotThrow` call the block synchronously and require a
 *    function; `throws` accepts an optional expected error - a RegExp, an error class, an
 *    arrow validation function or a property filter object. `rejects` accepts a promise or a
 *    function returning a promise and returns a promise, so it must be awaited.
 *
 *  Import:
 *  ```JavaScript
 *  const assert = require('assert');
 *  // the strict variant: require('assert/strict'), or assert.strict below
 *  ```
 *
 *  Example 1 — truthiness checks and loose comparison:
 *  ```JavaScript
 *  const assert = require('assert');
 *
 *  assert.ok(1);
 *  assert.notOk('');
 *  assert.equal(1, '1');    // loose equality: passes
 *  assert.notEqual({}, {}); // distinct references: passes
 *  console.log('checks passed');
 *  ```
 *
 *  Example 2 — catch a failure and inspect its AssertionError fields:
 *  ```JavaScript
 *  const assert = require('assert');
 *
 *  try {
 *      assert.strictEqual(1, '1');
 *  } catch (err) {
 *      console.log(err.name);     // AssertionError
 *      console.log(err.code);     // ERR_ASSERTION
 *      console.log(err.operator); // strictEqual
 *      console.log(JSON.stringify([err.actual, err.expected])); // [1,"1"]
 *      console.log(err instanceof assert.AssertionError);  // true
 *  }
 *  ```
 *
 *  Example 3 — exception and rejection checks:
 *  ```JavaScript
 *  const assert = require('assert');
 *
 *  assert.throws(() => JSON.parse('{'), SyntaxError);
 *  assert.throws(() => { throw new Error('boom'); }, /boom/);
 *  assert.doesNotThrow(() => JSON.parse('{}'));
 *
 *  (async () => {
 *      await assert.rejects(Promise.reject(new TypeError('bad')), TypeError);
 *      await assert.rejects(async () => { throw new Error('later'); }, /later/);
 *      console.log('all exceptions matched');
 *  })();
 *  ```
 *
 *  Notes:
 *
 *  - `assert` and `assert.ok` are the same callable object; both are aliases of the `operator`
 *    member, so `assert(value, message)` is the truthiness check.
 *  - A comparison whose internal conversion throws (for example an object with a throwing
 *    valueOf) is reported as a failed AssertionError, while Node.js propagates the conversion
 *    error.
 *  - `throws` treats a function whose `prototype.constructor` is itself as an error class,
 *    not as a validation function; pass an arrow function to validate an error.
 *  - Node.js `doesNotReject` and `partialDeepStrictEqual` are not provided; `fail` takes
 *    only a message in both implementations.
 *  - See the assert_strict module for the strict comparison mode and the differences it maps.
 *
 */
declare module 'assert' {
    /**
     * @description The callable module object itself; tests that the value is truthy
     *
     *      `assert(value, message)` is the same check as `ok` and the failure carries the
     *      operator 'ok'. The parameter defaults to undefined, so `assert()` fails.
     *
     *      @param actual the value to test
     *      @param msg the message when the assertion fails
     *
     */
    function assert(actual?: any, msg?: any): void;

    namespace assert {
        /**
         * @description The AssertionError constructor used for every failed assertion
         *
         *      It is the class behind all assertion failures, so `err instanceof
         *      assert.AssertionError` identifies one; `name` is 'AssertionError' and `code` is
         *      'ERR_ASSERTION'. Constructing an instance directly is supported for testing error
         *      handling; the options object supports:
         *
         *      ```JavaScript
         *      // fragment: options
         *      ({
         *          "message": "custom message", // optional; when omitted a message is generated
         *          "actual": 1,                 // the value that failed the check
         *          "expected": 2,               // the expected value
         *          "operator": "strictEqual",   // selects the generated message and diff form
         *          "property": "name"           // optional property that was checked
         *      })
         *      ```
         *
         *      `generatedMessage` is false when a message was given, and `toString()` renders as
         *      `AssertionError [ERR_ASSERTION]: <message>`. Node.js exposes the same constructor and
         *      fields; the strict module re-exports the same class.
         *
         */
        const AssertionError: (...args: any[])=>any;

        /**
         * ! Tests that the value is truthy; the assertion fails if it is false; an alias of the module
         *
         *      Falsy values are false, 0, '', null, undefined and NaN. `assert.ok` is the assert
         *      module object itself (`assert.ok === assert`), so `assert.ok(value, message)` is the
         *      same call as `assert(value, message)`; the failure carries the operator 'ok'.
         *
         *      Example — pass a truthy value and catch a falsy one:
         *      ```JavaScript
         *      const assert = require('assert');
         *
         *      assert.ok(1);
         *      assert.ok('text', 'a non-empty string is truthy');
         *      try {
         *          assert.ok(0);
         *      } catch (err) {
         *          console.log(err.message); // Expected the expression to be truthy
         *      }
         *      ```
         *
         */
        const ok: typeof import ('assert');

        /**
         * ! Strict testing module, see the assert_strict module
         *
         *      The same object as `require('assert/strict')`, also reachable as
         *      `require('fibjs:assert/strict')` and `require('node:assert/strict')`; its `equal` and
         *      `deepEqual` use strict comparison.
         *
         */
        const strict: typeof import ('assert/strict');

        /**
         * @description Fails unconditionally and throws a generated AssertionError
         *
         *      The error carries the operator 'fail' and no actual/expected values; without a
         *      message the generated message is 'Failed'. The signature matches the current
         *      Node.js `assert.fail(message)`; the legacy four-argument form of older releases is
         *      not supported.
         *
         *      @param msg the message when the assertion fails
         *
         */
        function fail(msg?: any): void;

        /**
         * @description Tests that the value is falsy; the assertion fails if it is true
         *
         *      Falsy values are false, 0, '', null, undefined and NaN; the check is the exact
         *      negation of `ok` and the failure carries the operator 'notOk'. Node.js has no notOk,
         *      the equivalent is `assert.ok(!value)`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function notOk(actual: any, msg?: any): void;

        /**
         * @description Tests that the value loosely equals the expected value
         *
         *      Uses the JavaScript `==` operator: `1`, `'1'` and `new Number(1)` are equal, null
         *      equals undefined, and objects are equal only when they are the same reference. NaN
         *      is never equal to anything, including itself. For type-safe comparison use
         *      `strictEqual`, and see `deepEqual` for nested values. Node.js marks assert.equal as
         *      legacy; the failure carries the operator '=='.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function equal(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value does not loosely equal the expected value
         *
         *      The exact negation of `equal`: two references to the same object are equal, so
         *      `notEqual(obj, obj)` fails. The failure carries the operator '!=' and, like Node.js,
         *      the default message renders as `Expected X != Y`.
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
         *      Uses the JavaScript `===` operator: `1` and `'1'` are not equal, NaN is not equal to
         *      NaN, +0 and -0 are equal, and objects must be the same reference. On failure a diff
         *      between actual and expected is generated and a custom message is prepended to it,
         *      matching Node.js. The failure carries the operator 'strictEqual'.
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
         *      The exact negation of `strictEqual`: `notStrictEqual(1, '1')` passes while
         *      `notStrictEqual(obj, obj)` fails. The failure carries the operator 'notStrictEqual'
         *      and a custom message replaces the generated one.
         *
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function notStrictEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value loosely deeply equals the expected value
         *
         *      A structural comparison extending the loose `==` rule: Dates are compared by time
         *      value, RegExps by source and flags, arrays by length and elements, Buffers through
         *      their equals method, and other objects by their enumerable property names - including
         *      inherited ones - where both objects must have the same names and loosely equal
         *      values. Prototypes are not compared, symbol-keyed properties are ignored, cyclic
         *      structures are supported and functions are compared by reference. So
         *      `deepEqual({ a: 1 }, { a: '1' })` passes while `deepStrictEqual` fails. Node.js marks
         *      deepEqual as legacy. The failure carries the operator 'deepEqual' and a custom
         *      message replaces the generated diff.
         *
         *      Example — loose deep comparison of nested values, Dates and Buffers:
         *      ```JavaScript
         *      const assert = require('assert');
         *
         *      assert.deepEqual({ a: 1, b: [2, '3'] }, { a: '1', b: ['2', 3] });
         *      assert.deepEqual(new Date(1000), new Date(1000));
         *      assert.deepEqual(Buffer.from('abc'), Buffer.from('abc'));
         *      assert.notDeepEqual({ a: 1 }, { a: 2 });
         *      console.log('loose deep comparison passed');
         *      ```
         *      @param actual the value to test
         *      @param expected the expected value
         *      @param msg the message when the assertion fails
         *
         */
        function deepEqual(actual: any, expected: any, msg?: any): void;

        /**
         * @description Tests that the value does not loosely deeply equal the expected value
         *
         *      The exact negation of `deepEqual`; structurally different values pass and
         *      structurally identical values fail. The failure carries the operator 'notDeepEqual'
         *      and a custom message replaces the generated one.
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
         *      Like `deepEqual`, but leaf values are compared with `===`, so
         *      `deepStrictEqual({ a: 1 }, { a: '1' })` fails. Prototypes are still not compared and
         *      symbol-keyed properties are still ignored, which differs from Node.js where
         *      deepStrictEqual also compares prototypes and symbols. A custom message is prepended
         *      to the generated diff; the failure carries the operator 'deepStrictEqual'.
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
         *      The exact negation of `deepStrictEqual`: values that differ in type at the leaves
         *      pass and structurally, strictly identical values fail. The failure carries the
         *      operator 'notDeepStrictEqual' and a custom message replaces the generated one.
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
         *      The actual value is declared String, so a String object, a Date (its ISO form), a
         *      Buffer (its utf8 bytes) and an object with its own toString are converted; values
         *      without a string form (numbers, arrays, plain objects, null, undefined) fail with a
         *      TypeError [20005]. Node.js requires a real string and throws ERR_INVALID_ARG_TYPE.
         *      The failure carries the operator 'match', with the actual string and the RegExp in the
         *      respective fields.
         *
         *      Example — matching and the negated form:
         *      ```JavaScript
         *      const assert = require('assert');
         *
         *      assert.match('fibjs 2026', /\d{4}/);
         *      assert.doesNotMatch('fibjs', /^node/);
         *      try {
         *          assert.match('abc', /^\d+$/);
         *      } catch (err) {
         *          console.log(err.message); // Expected "abc" to match /^\d+$/
         *      }
         *      ```
         *      @param actual the string to test
         *      @param expected the expected regular expression
         *      @param msg the message when the assertion fails
         *
         */
        function match(actual: string, expected: FIBJS.GeneralObject, msg?: any): void;

        /**
         * @description Tests that the string does not match the expected regular expression
         *
         *      The exact negation of `match`, with the same string conversion rules and TypeError
         *      [20005] for values without a string form. The failure carries the operator
         *      'doesNotMatch' and the default message renders as `Expected "X" not to match /re/`.
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
         *      The check is `Math.abs(actual - expected) <= delta`, so the bound is inclusive; both
         *      values and the delta are converted with Number(), and a value that converts to NaN
         *      (including a non-numeric string) throws a TypeError [20004] instead of failing the
         *      assertion. Node.js has no closeTo.
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
         *      The exact negation of `closeTo` (`Math.abs(actual - expected) > delta`); the same
         *      Number() conversion applies and a NaN conversion throws a TypeError [20004].
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
         *      When either side is a number (or a string that converts to one, such as '10' next to
         *      a number) the comparison is numeric; when both sides are non-numeric strings it is a
         *      lexicographic UTF-8 comparison; a value that cannot be converted to a number (an
         *      object or an invalid numeric string against a number) throws a TypeError [20004].
         *      Node.js has no lessThan; with Node.js `assert.ok(a < b)` is the equivalent.
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
         *      The negation of `lessThan` (actual >= expected) with the same numeric/lexicographic
         *      conversion rules and TypeError [20004] for values that cannot be compared.
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
         *      The mirror of `lessThan` (actual > expected) with the same numeric/lexicographic
         *      conversion rules and TypeError [20004] for values that cannot be compared. The
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
         *      The negation of `greaterThan` (actual <= expected) with the same numeric/lexicographic
         *      conversion rules and TypeError [20004] for values that cannot be compared.
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
         *      A value exists when it is neither null nor undefined; false, 0, '' and NaN exist.
         *      Node.js has no exist; the equivalent is `assert.ok(value !== null && value !==
         *      undefined)`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function exist(actual: any, msg?: any): void;

        /**
         * @description Tests that the variable does not exist
         *
         *      The exact negation of `exist`: only null and undefined do not exist, so 0, '' and
         *      false all fail this check.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function notExist(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is boolean true
         *
         *      The check is strict: 1, 'true' and new Boolean(true) all fail. Node.js has no isTrue;
         *      the equivalent is `assert.strictEqual(value, true)`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isTrue(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not boolean true
         *
         *      The exact negation of `isTrue`: every value except the true primitive passes,
         *      including 1 and new Boolean(true).
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotTrue(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is boolean false
         *
         *      The check is strict: 0, '' and new Boolean(false) all fail. Node.js has no isFalse;
         *      the equivalent is `assert.strictEqual(value, false)`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isFalse(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not boolean false
         *
         *      The exact negation of `isFalse`: every value except the false primitive passes,
         *      including 0 and new Boolean(false).
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotFalse(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is Null
         *
         *      Only the null primitive passes; undefined is not null and fails this check, use
         *      `isUndefined` for it. Node.js has no isNull.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNull(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not Null
         *
         *      The exact negation of `isNull`; since only null fails isNull, every other value
         *      passes, undefined included.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotNull(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is undefined
         *
         *      Only the undefined primitive passes; null is defined for this check and fails it,
         *      use `isNull` for null. Node.js has no isUndefined.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isUndefined(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not undefined
         *
         *      The exact negation of `isUndefined`; null and every other value pass. Node.js has no
         *      isDefined.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isDefined(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is a function
         *
         *      Classes, async functions and generator functions are functions too. Node.js has no
         *      isFunction; the equivalent is `assert.strictEqual(typeof value, 'function')`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isFunction(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not a function
         *
         *      The exact negation of `isFunction`: every value that cannot be called passes, so
         *      objects and arrays pass while classes fail.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotFunction(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is an object
         *
         *      Arrays and functions count as objects, and boxed primitives (new Number(1)) count
         *      too; null, undefined, primitives and symbols do not. `typeOf(value, 'object')`
         *      delegates to this check, so the same widening applies. Node.js has no isObject.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isObject(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not an object
         *
         *      The exact negation of `isObject`: primitives, null, undefined and symbols pass,
         *      while arrays, functions and boxed primitives fail.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotObject(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is an array
         *
         *      Uses Array.isArray semantics: an Array subclass passes, while typed arrays and
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
         *      The exact negation of `isArray`: any value that is not an Array passes, including
         *      typed arrays, Buffer and arguments objects.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotArray(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is a string
         *
         *      Both a string primitive and a String object pass; a Buffer does not (it is a
         *      Uint8Array). Node.js has no isString.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isString(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not a string
         *
         *      The exact negation of `isString`: a Buffer, a number and a String-like object
         *      without the String prototype all pass.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotString(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is a number
         *
         *      The check is on the primitive type: NaN passes, while a Number object does not.
         *      Node.js has no isNumber; the equivalent is `assert.strictEqual(typeof value,
         *      'number')`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNumber(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not a number
         *
         *      The exact negation of `isNumber`: any value that is not a number primitive passes,
         *      including a Number object and NaN-free numeric strings.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotNumber(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is a boolean
         *
         *      Only the true and false primitives pass; a Boolean object does not. Node.js has no
         *      isBoolean; the equivalent is `assert.strictEqual(typeof value, 'boolean')`.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isBoolean(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is not a boolean
         *
         *      The exact negation of `isBoolean`: everything except the true and false primitives
         *      passes, including 0, '' and a Boolean object.
         *
         *      @param actual the value to test
         *      @param msg the message when the assertion fails
         *
         */
        function isNotBoolean(actual: any, msg?: any): void;

        /**
         * @description Tests that the value is of the given type
         *
         *      The accepted type names are 'array', 'function', 'string', 'object', 'number',
         *      'boolean', 'null' and 'undefined'; 'object' matches arrays and functions as well
         *      because it delegates to `isObject`. Any other name throws a TypeError [20004]
         *      instead of failing the assertion. Node.js has no typeOf.
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
         *      The exact negation of `typeOf`, accepting the same eight type names and throwing a
         *      TypeError [20004] for anything else; arrays fail `notTypeOf(value, 'object')`
         *      because typeOf/object delegates to `isObject`.
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
         *      The property is looked up with the JavaScript in operator, so inherited properties
         *      count as well. The receiver must be an object and the property name must be a
         *      string; other values throw a TypeError [20004]. A primitive string receiver passes
         *      the type check but crashes the process in this version, so pass objects only. The
         *      failure carries the operator 'property'; Node.js has no property.
         *
         *      Example — own, inherited and nested properties:
         *      ```JavaScript
         *      const assert = require('assert');
         *      const user = { name: 'lion', address: { city: 'shanghai' } };
         *
         *      assert.property(user, 'name');
         *      assert.property(user, 'toString'); // inherited from Object.prototype
         *      assert.notProperty(user, 'email');
         *      assert.deepProperty(user, 'address.city');
         *      assert.deepPropertyVal(user, 'address.city', 'shanghai');
         *      assert.propertyNotVal(user, 'name', 'tiger');
         *      console.log('property checks passed');
         *      ```
         *      @param object the object to test
         *      @param prop the property to test
         *      @param msg the message when the assertion fails
         *
         */
        function property(object: any, prop: any, msg?: any): void;

        /**
         * @description Tests that the object does not contain the specified property
         *
         *      The exact negation of `property`; the same object/property type requirements and
         *      TypeError [20004] apply, and an inherited property fails this check.
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
         *      The property name is split on '.', so each segment names one level; a path like
         *      'a.b.0' reaches array elements, while a property name containing a dot cannot be
         *      addressed. A missing intermediate value fails the assertion (it does not throw); the
         *      receiver must be an object and the path a string, otherwise a TypeError [20004] is
         *      thrown. Node.js has no deepProperty.
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
         *      The exact negation of `deepProperty`; a path through a missing intermediate value
         *      counts as absent and passes this check.
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
         *      The value is compared with strict equality even though the module is loose, so
         *      `propertyVal(obj, 'a', 1)` fails when the property holds '1'; a missing property
         *      yields undefined and therefore fails. Node.js has no propertyVal.
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
         *      The exact negation of `propertyVal` with the same strict comparison: a missing
         *      property (undefined) passes whenever the given value is not undefined.
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
         *      Like `propertyVal` with a dotted path: the value is compared with strict equality,
         *      so a numeric property compared against its string form fails. The receiver must be
         *      an object and the path a string, otherwise a TypeError [20004] is thrown.
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
         *      The exact negation of `deepPropertyVal`; a missing intermediate value or a
         *      different (strict) value passes, and the same TypeError [20004] applies to
         *      non-object receivers and non-string paths.
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
         *      The block is called synchronously without arguments; when it returns instead of
         *      throwing, the assertion fails with 'Missing expected exception'. The error argument
         *      filters the thrown value:
         *
         *      - a RegExp matches the string form of the thrown value;
         *      - an error class - a function whose `prototype.constructor` is itself, such as
         *        TypeError - passes when the thrown value is an instance of it;
         *      - any other function is a validation function: it is called with the thrown value and
         *        must return exactly true; use an arrow function for this form, because a normal
         *        function is treated as a class (Node.js accepts normal functions as validators);
         *      - an object is a property filter: each own property of the filter is compared with
         *        the same property of the thrown value, RegExp values match the string form, other
         *        values use deepStrictEqual, and a property whose actual value is undefined is
         *        skipped (Node.js compares it).
         *
         *      An error thrown inside a validation function is propagated as-is. The failure carries
         *      the operator 'throws' with the filter in expected and the thrown value in actual; a
         *      custom message is used when a filter was supplied and did not match. Node.js has the
         *      same filter forms but also appends a 'Caught error' excerpt.
         *
         *      Example — class, RegExp and validation function filters:
         *      ```JavaScript
         *      const assert = require('assert');
         *
         *      assert.throws(() => JSON.parse('{'), SyntaxError);
         *      assert.throws(() => { throw new Error('boom'); }, /boom/);
         *      assert.throws(() => { throw new TypeError('bad'); }, (err) => err.message === 'bad');
         *      try {
         *          assert.throws(() => { });
         *      } catch (err) {
         *          console.log(err.message); // Missing expected exception
         *      }
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
         *      Equivalent to calling the three-argument overload with an undefined error filter.
         *      The message is ignored when nothing is thrown, because that failure always reports
         *      the generated 'Missing expected exception' text; use a filter to have the message
         *      rendered. Node.js treats `assert.throws(block, 'message')` as a message, while fibjs
         *      binds the second argument to the error filter, so prefer passing the filter
         *      explicitly.
         *      @param block the code to test, given as a function
         *      @param msg the message when the assertion fails
         *
         */
        function throws(block: ()=>void, msg?: any): void;

        /**
         * @description Tests that the given code does not throw an error
         *
         *      The block is called synchronously without arguments and any thrown value fails the
         *      assertion; there is no error filter, so to allow some errors and reject others use
         *      `throws` with a validation function. The caught value is kept in the actual field,
         *      the failure carries the operator 'doesNotThrow' and a custom message replaces the
         *      default 'Got unwanted exception'. Node.js has the same shape.
         *
         *      @param block the code to test, given as a function
         *      @param msg the message when the assertion fails
         *
         */
        function doesNotThrow(block: ()=>void, msg?: any): void;

        /**
         * @description Tests that the given code throws an error; the assertion fails if nothing is thrown
         *
         *      The block is called synchronously without arguments: an error thrown by the call
         *      rejects the returned promise with that error, and a return value that is not a
         *      promise rejects it with a TypeError whose message is 'The rsult of the function is
         *      not a promise.' (the typo is in the generated message). When the awaited promise
         *      rejects, the error argument filters the rejection exactly like `throws` (RegExp,
         *      error class, arrow validator or property filter object).
         *
         *      The returned promise resolves to undefined when the rejection matched and rejects
         *      with the generated AssertionError otherwise; the operator is 'rejects'. Node.js
         *      returns a promise with the same shape but rejects with ERR_INVALID_RETURN_VALUE for
         *      a non-promise result.
         *
         *      Example — await a function and a promise rejection with different filters:
         *      ```JavaScript
         *      const assert = require('assert');
         *
         *      (async () => {
         *          await assert.rejects(Promise.reject(new TypeError('bad input')), TypeError);
         *          await assert.rejects(async () => { throw new Error('later'); }, /later/);
         *          await assert.rejects(() => Promise.reject(new Error('from fn')), (err) => {
         *              return err.message === 'from fn';
         *          });
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
         *      message is ignored when nothing is rejected, because that failure always reports
         *      the generated 'Missing expected rejection' text.
         *      @param block the code to test, given as a function returning a promise
         *      @param msg the message when the assertion fails
         *      @return returns a Promise
         *
         */
        function rejects(block: ()=>void, msg?: any): Promise;

        /**
         * @description Tests that the given promise rejects
         *
         *      The promise is awaited; a rejection is filtered by the error argument as in the
         *      function overload, and a resolution fails the assertion with 'Missing expected
         *      rejection'. The returned promise resolves to undefined when the rejection matched
         *      and rejects with the generated AssertionError otherwise. Node.js has the same form.
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
         *      The value is rethrown as-is, not wrapped, which makes it the standard trailing
         *      callback helper (`if (err) throw err`). Falsy values - undefined, null, false, 0, ''
         *      and NaN - pass. Node.js wraps the value in an AssertionError, so a caught value that
         *      is not an Error differs between the two implementations.
         *
         *      Example — pass a falsy value and rethrow an error:
         *      ```JavaScript
         *      const assert = require('assert');
         *
         *      assert.ifError(null);
         *      try {
         *          assert.ifError(new Error('from callback'));
         *      } catch (err) {
         *          console.log(err.message); // from callback
         *      }
         *      ```
         *      @param object the argument
         *
         */
        function ifError(object?: any): void;

    }

    export = assert;

}

