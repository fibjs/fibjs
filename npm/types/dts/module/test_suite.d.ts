/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The test_suite module defines the nested suites of the test framework: a suite
 *  groups cases and hooks under a title, and suites can be nested to any depth
 *
 *  A suite is a callable object collected by the test module. It is exported as `test.suite`
 *  and `test.describe` (destructuring `suite`/`describe` from `test` or `node:test` also
 *  returns it); there is no standalone `test_suite` builtin, `require('test_suite')` throws
 *  MODULE_NOT_FOUND. Cases and hooks are declared in the suite body, and the body runs at
 *  execution time, so an async body is awaited before its cases are collected.
 *
 *  Main capabilities:
 *
 *  - **Suites**: the callable module itself, the same object as `describe`, nesting arbitrary;
 *  - **Skipping**: `skip` (same as `describe.skip`) prunes the subtree without counting it;
 *  - **Selection**: `only` (same as `describe.only`) selects the subtree against its
 *    siblings;
 *  - **Planning**: `todo` (same as `describe.todo`) marks the suite as planned.
 *
 *  Concepts:
 *
 *  - **Title path**: the report prints the title path from the root suite to the case,
 *    indented two spaces per nesting level; the failure list repeats the full path.
 *  - **Suite body timing**: the body runs when the suite is reached during the run, not when
 *    it is declared, and its cases are registered at that moment; an async body is awaited
 *    (see the test module for the fiber model).
 *  - **Options**: the options object supports `skip`, `todo` and `only` with the literal
 *    `true`; any other value is ignored, exactly as for cases.
 *  - **Hooks**: suites do not declare their own hooks; `before`/`after`/`beforeEach`/
 *    `afterEach` from the test module attach to the suite that is being collected.
 *  - **`todo` suites**: a todo suite is counted in the todo total, but its body is still
 *    executed and its cases still run and can fail the run; a skip suite is pruned entirely.
 *    This differs from Node.js, where a todo suite silences its subtree.
 *
 *  Import:
 *  ```JavaScript
 *  const test = require('test');
 *  const suite = test.suite;                  // === test.describe
 *  const { describe } = require('node:test'); // the same object
 *  ```
 *
 *  Example 1 — nested suites in a title path:
 *  ```JavaScript
 *  const { describe, it } = require('node:test');
 *  const assert = require('assert');
 *  describe('math', () => {
 *      describe('integers', () => {
 *          it('adds', () => assert.equal(1 + 1, 2));
 *      });
 *      it('floats', () => assert.equal(0.5 + 0.5, 1));
 *  });
 *  ```
 *
 *  Example 2 — a skipped suite keeps its cases out of the run:
 *  ```JavaScript
 *  const { describe, it } = require('node:test');
 *  describe('disabled area', () => {
 *      describe.skip('not implemented', () => {
 *          it('never runs', () => { throw new Error('never runs'); });
 *      });
 *      it('still runs', () => { });
 *  });
 *  ```
 *
 *  Notes:
 *
 *  - `describe`/`suite` return undefined; unlike Node.js they do not return a promise,
 *    because the suite body is collected during the run rather than awaited at the call site.
 *  - A suite marked `todo` with a body is counted in the todo total while its cases still run;
 *    prefer `it.todo`/`todo(name)` for planned cases that must not run.
 *  - Depth is unlimited; the report indents two spaces per level.
 *
 */
declare module 'test_suite' {
    /**
     * @description Defines a suite; the suite object itself is callable, so `suite(name, block)`
     *      is the same as `describe(name, block)` and nests inside any enclosing suite
     *
     *      The body declares cases and nested suites through the test module functions; it runs
     *      when the suite is reached during the run and its result promise is awaited. Returning
     *      nothing is the common form.
     *
     *      Example — a suite groups cases under a title:
     *      ```JavaScript
     *      const { describe, suite, it } = require('node:test');
     *      describe('outer', () => {
     *          suite('inner', () => {
     *              it('nested case', () => { });
     *          });
     *      });
     *      ```
     *
     *      @param name the suite title shown in the report
     *      @param block the suite body
     *
     */
    function test_suite(name: string, block: ()=>void): void;

    /**
     * @description Defines a suite with an options object supporting `skip`, `todo` and `only`,
     *      each taking effect only when it is the literal `true`
     *
     *      The options form is the base of `skip`, `only` and `todo`; see the two-argument form for
     *      the body semantics. A skipped suite is pruned, a selected suite keeps its direct
     *      siblings out, and a todo suite is counted as planned while its body still runs.
     *
     *      @param name the suite title shown in the report
     *      @param options the suite options: { skip, todo, only }
     *      @param block the suite body
     *
     */
    function test_suite(name: string, options: FIBJS.GeneralObject, block: ()=>void): void;

    namespace test_suite {
        /**
         * @description Defines a suite that is skipped; the whole subtree is pruned, so its cases are
         *      not executed, not counted and not printed
         *
         *      `suite.skip(name, block)` and `describe.skip(name, block)` are the same call; the
         *      `xdescribe` member of the test module is the legacy alias. A suite can also be skipped
         *      with `{ skip: true }` as its options.
         *
         *      Example — a skipped suite leaves no trace in the report:
         *      ```JavaScript
         *      const { describe, suite, it } = require('node:test');
         *      describe('area', () => {
         *          suite.skip('planned', () => {
         *              it('never runs', () => { throw new Error('never runs'); });
         *          });
         *          it('runs', () => { });
         *      });
         *      ```
         *
         *      @param name the suite title shown in the report
         *      @param block the suite body, which is never executed
         *
         */
        function skip(name: string, block: ()=>void): void;

        /**
         * @description Defines a suite selected by `only`; the other direct children of the enclosing
         *      suite are skipped and unselected sub-suites are pruned
         *
         *      `suite.only(name, block)` and `describe.only(name, block)` are the same call; the
         *      equivalent options form is `{ only: true }`. The scope is the enclosing suite only, so
         *      an `only` in a nested suite does not affect the parent's sibling suites.
         *
         *      @param name the suite title shown in the report
         *      @param block the suite body
         *
         */
        function only(name: string, block: ()=>void): void;

        /**
         * @description Defines a planned suite; it is counted in the todo total, but the body is still
         *      executed and its cases still run, which differs from Node.js where a todo suite
         *      silences its subtree
         *
         *      `suite.todo(name, block)` and `describe.todo(name, block)` are the same call; prefer
         *      `it.todo`/`todo(name)` for planned cases that must not run.
         *
         *      @param name the suite title shown in the report
         *      @param block the suite body, executed with its cases running normally
         *
         */
        function todo(name: string, block: ()=>void): void;

        /**
         * @description Defines a planned suite with an options object; `todo` is the default level
         *      and `skip: true` takes precedence, in which case the subtree is pruned entirely
         *
         *      See the two-argument form for the todo suite semantics.
         *
         *      @param name the suite title shown in the report
         *      @param options the suite options: { skip, todo, only }
         *      @param block the suite body, executed unless skipped
         *
         */
        function todo(name: string, options: FIBJS.GeneralObject, block: ()=>void): void;

    }

    export = test_suite;

}

