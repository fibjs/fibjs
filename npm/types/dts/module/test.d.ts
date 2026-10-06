/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/test_suite.d.ts" />
/// <reference path="../module/assert.d.ts" />
/**
 * @description The test module is fibjs's built-in test framework: it provides the describe/it style
 * case and suite definitions, hooks and call-count helpers used to write test cases, and it also
 * drives the `fibjs --test` command line runner
 *
 *  The module itself is a callable object: `test(name, fn)` and `test.it(name, fn)` define a
 *  case, and the suite container is exported as `test.suite` / `test.describe` (see test_suite).
 *  No package is required; the framework is part of the runtime.
 *
 *  Main capabilities:
 *
 *  - **Test items**: the callable module, `it` (an alias of the module), `skip`/`xit`,
 *    `only`/`oit`, `todo` (with or without a body) and their options forms, plus the `test`
 *    property that points back to the module;
 *  - **Suites**: `describe` and `suite`, the test_suite object, nested to any depth;
 *  - **Hooks**: `before`, `after`, `beforeEach` and `afterEach`, attached to the suite that is
 *    being collected (the file root when registered at top level);
 *  - **Call-count helpers**: `mustCall` and `mustNotCall` wrap a function and record whether the
 *    current case called it;
 *  - **Reporting**: `slow` sets the case duration warning threshold;
 *  - **Assertions**: `assert` is the assert module, so `test.assert` is `require('assert')`.
 *
 *  Concepts:
 *
 *  - **Collection and run**: defining a case or suite only records it in a tree; the tree is
 *    executed once after the entry script finishes. Under `fibjs --test` the test files are
 *    loaded in-process one after another first, and the whole tree runs at the end. A suite body
 *    runs at execution time, and an async suite body is awaited, so its cases are collected then.
 *  - **Case bodies**: a body may be synchronous, an async function (fibjs waits for the returned
 *    promise on a fiber), or callback style; a one-argument body receives a `done` callback and
 *    a two-argument body receives `(context, done)` where context is a plain empty object. A
 *    plain function that returns a promise is not awaited, so async bodies must be declared
 *    `async`.
 *  - **Options**: the options object supports `skip`, `todo` and `only`, each of which must be
 *    the literal `true`; any other option, including `timeout`, is ignored. A skipped case is
 *    counted in the skipped total and its body is not executed; `skip` and `only` require a
 *    body, while `todo(name)` declares a planned case without one.
 *  - **`only` scope**: the selection applies to the enclosing suite: its other direct children
 *    are skipped, and unselected sub-suites are pruned without being counted.
 *  - **Hooks**: `before`/`after` run once per suite, `beforeEach`/`afterEach` around every case;
 *    outer hooks run first for `before`/`beforeEach`, inner hooks first for `after`/`afterEach`.
 *    A failing `before` fails all cases of its suite without running them; a failing `afterEach`
 *    turns the current case into a failure; a failing `after` is reported separately.
 *  - **Failure reporting and exit code**: failures are collected during the run and printed at
 *    the end as a numbered list with the source location and stack, after the `N tests
 *    completed` line and the passed/failed/todo/skipped counters. Any counted failure sets the
 *    process exit code to 1, except a failure coming only from an `afterEach` hook, which
 *    appears in the summary but leaves the exit code at 0 (Node.js exits 1).
 *  - **Leaked resources**: `FIBJS_TEST_WATCHDOG_MS` (default 10000) arms a watchdog after the
 *    run; if the process is still alive then, a fibers/memory report is printed and the process
 *    exits with code 124, so cases must release handles, timers and servers.
 *  - **CLI**: `fibjs --test [files|dirs|globs]` discovers test files with the Node.js
 *    test-file conventions (files named `*.test.js`, `*-test.js`, `*_test.js`, `test-*.js` or
 *    `test.js`, and any `.js`/`.mjs`/`.cjs` under a directory named `test`) and loads them
 *    sequentially in one process. `--test-name-pattern=<regex>` keeps the cases whose name, or
 *    an ancestor suite's name, matches; `--test-concurrency` and `--test-isolation` are
 *    accepted for compatibility (files always run in-process and sequentially);
 *    `--test-timeout`, `--test-reporter` and watch/coverage modes are not supported.
 *
 *  Import:
 *  ```JavaScript
 *  const test = require('test');
 *  // the same builtin is reachable under the Node.js id and can be destructured:
 *  const { describe, it, suite, before, after, beforeEach, afterEach } = require('node:test');
 *  ```
 *
 *  Example 1 — a suite with hooks and assertions:
 *  ```JavaScript
 *  const { describe, it, before, beforeEach, after } = require('node:test');
 *  const assert = require('assert');
 *
 *  describe('inventory', () => {
 *      let store;
 *
 *      before(() => { store = {}; });
 *      beforeEach(() => { store.count = 1; });
 *      after(() => console.log('suite done'));
 *
 *      it('gets a fresh counter', () => {
 *          assert.equal(store.count, 1);
 *      });
 *      it('gets a fresh counter again', () => {
 *          assert.equal(store.count, 1);
 *      });
 *  });
 *  ```
 *
 *  Example 2 — skip, todo and `only` selection:
 *  ```JavaScript
 *  const { describe, it, skip, todo, only } = require('node:test');
 *  const assert = require('assert');
 *
 *  describe('variants', () => {
 *      only('selected case', () => assert.ok(true)); // siblings are skipped
 *      it('skipped by only', () => assert.ok(false));
 *      skip('disabled case', () => assert.ok(false));
 *      todo('planned case', () => assert.ok(false));
 *  });
 *  ```
 *
 *  Example 3 — async and callback-style cases:
 *  ```JavaScript
 *  const { describe, it } = require('node:test');
 *  const assert = require('assert');
 *
 *  describe('async forms', () => {
 *      it('async body', async () => {
 *          await new Promise(resolve => setTimeout(resolve, 10));
 *          assert.ok(true);
 *      });
 *
 *      it('done callback', (done) => {
 *          setTimeout(() => { assert.ok(true); done(); }, 10);
 *      });
 *
 *      it('context and done', (context, done) => {
 *          assert.equal(typeof context, 'object');
 *          done();
 *      });
 *  });
 *  ```
 *
 *  Notes:
 *
 *  - `require('test')` and `require('node:test')` return the same object; the `node:` prefixed id
 *    is the builtin, not a user-installed package.
 *  - Node.js extras are not provided: there is no `run()`, `mock`, `snapshot`, subtests, test
 *    context, reporters, watch mode or coverage; the case options only honor
 *    `skip`/`todo`/`only`, and `timeout` is ignored where Node.js fails the case.
 *  - In Node.js a todo case body runs and its failure is silenced, while in fibjs it does not
 *    run at all; a todo suite in fibjs is counted as planned but its cases still run and can
 *    fail the run.
 *  - `mustCall` must be used inside a running case (error 20009 outside), and a wrapped function
 *    that is never called leaves the run waiting forever: the watchdog is armed only after the
 *    run finishes.
 *  - Use the assert module (loose comparison) or assert_strict (strict) for the checks; both are
 *    available as `assert` objects and `test.assert` points to the loose one.
 *
 */
declare module 'test' {
    /**
     * @description Defines a test case; the test module itself is callable, so this two-argument
     *      form is equivalent to it(name, block)
     *
     *      Defining a case only records it; the tree runs after the entry script finishes. The body
     *      may throw to fail the case, may be an async function (fibjs waits for the returned
     *      promise), or may take a callback: a one-argument body receives `done` and a two-argument
     *      body receives `(context, done)`. A plain function returning a promise is not awaited.
     *
     *      Example — the module and its it alias are the same definition function:
     *      ```JavaScript
     *      const test = require('test');
     *      test('module call form', () => { });
     *      test.it('it call form', () => { });
     *      ```
     *
     *      @param name the case title shown in the report
     *      @param block the case body
     *
     */
    function test(name: string, block: ()=>void): void;

    /**
     * @description Defines a test case with an options object; the options object supports `skip`,
     *      `todo` and `only`, and each of them only takes effect when it is the literal `true`
     *
     *      The options form is shared by the callable module and by `it`, `skip`, `only` and `todo`.
     *      `timeout` and other Node.js options are accepted but ignored; see the two-argument form
     *      for the body execution model. `skip: true` wins over `todo: true`.
     *
     *      @param name the case title shown in the report
     *      @param options the case options: { skip, todo, only }
     *      @param block the case body
     *
     */
    function test(name: string, options: FIBJS.GeneralObject, block: ()=>void): void;

    namespace test {
        /**
         * @description The test module itself, so `test.test` is the callable module and can be
         *      destructured like the Node.js export
         *
         *      It exists because `it` is an alias of `test`: `test.test`, `test.it` and the module
         *      returned by `require('test')` are the same callable object. Use it when a reference to
         *      the definition function is needed, for example to pass it around.
         *
         */
        const test: typeof import ('test');

        /**
         * @description Defines a test case; `it` points to the test module itself, so `it(name,
         *      block)`, `it.skip`, `it.only` and `it.todo` behave exactly like the module-level
         *      functions
         *
         *      `it` and `test` are the same callable object, which matches the Node.js `node:test`
         *      aliases; `it.it` is therefore `it` again.
         *
         */
        const it: typeof import ('test');

        /**
         * @description The test_suite object used to group cases into nested suites; `suite(name,
         *      block)` defines a suite, and `suite.skip`/`suite.only`/`suite.todo` are the marked
         *      variants
         *
         *      See the test_suite module for the suite semantics; `describe` is the same object under
         *      its Node.js name.
         *
         */
        const suite: typeof import ('test_suite');

        /**
         * @description The describe alias of suite; `describe(name, block)` defines a suite that can
         *      be nested, and `describe.skip`/`describe.only`/`describe.todo` are the marked variants
         *
         *      `describe` and `suite` are the same object, which matches the Node.js `node:test`
         *      aliases. See the test_suite module for the suite semantics.
         *
         */
        const describe: typeof import ('test_suite');

        /**
         * @description The assert module object, so `test.assert` is the same export as
         *      `require('assert')` and assertions can be written without a second require
         *
         *      The object uses the loose comparison mode; see the assert module for the checks and the
         *      assert_strict module for the strict variants reachable as `assert.strict`.
         *
         */
        const assert: typeof import ('assert');

        /**
         * @description Defines a skipped suite; equivalent to `suite.skip(name, block)`, so the whole
         *      subtree is pruned and its cases are neither executed nor counted
         *
         *      In Node.js the equivalent is `describe.skip`; fibjs keeps `xdescribe` as the legacy
         *      alias next to `suite.skip`.
         *
         *      @param name the suite title shown in the report
         *      @param block the suite body that declares the nested cases
         *
         */
        function xdescribe(name: string, block: ()=>void): void;

        /**
         * @description Defines a suite selected by `only`; equivalent to `suite.only(name, block)`,
         *      so the other direct children of the enclosing suite are skipped
         *
         *      In Node.js the equivalent is `describe.only`; the options form `{ only: true }` has the
         *      same effect. See the `only` member of the test module for the selection scope.
         *
         *      @param name the suite title shown in the report
         *      @param block the suite body that declares the nested cases
         *
         */
        function odescribe(name: string, block: ()=>void): void;

        /**
         * @description Defines a skipped case; equivalent to `skip(name, block)`, the case is counted
         *      in the skipped total and its body is not executed
         *
         *      In Node.js the equivalent is `it.skip`; fibjs keeps `xit` as the legacy alias. A body is
         *      required here, exactly as for `skip`.
         *
         *      @param name the case title shown in the report
         *      @param block the case body, which is never executed
         *
         */
        function xit(name: string, block: ()=>void): void;

        /**
         * @description Defines a skipped case; the body is not executed and the case is counted in
         *      the skipped total instead of passing or failing
         *
         *      A body is required: `skip(name)` without one throws error 20002. The options form
         *      `{ skip: true }` and `it.skip` do the same thing; a non-literal value such as
         *      `{ skip: 'reason' }` is ignored and the case runs normally.
         *
         *      Example — skipped cases leave the run green:
         *      ```JavaScript
         *      const { it, skip } = require('node:test');
         *      it('runs', () => { });
         *      skip('not yet implemented', () => { throw new Error('never runs'); });
         *      it('also runs', { skip: true }, () => { throw new Error('never runs'); });
         *      ```
         *
         *      @param name the case title shown in the report
         *      @param block the case body, which is never executed
         *
         */
        function skip(name: string, block: ()=>void): void;

        /**
         * @description Defines a case selected by `only`; equivalent to `only(name, block)`, the
         *      other direct children of the enclosing suite are skipped
         *
         *      A body is required: `oit(name)` without one throws error 20002. See the `only` member
         *      for the selection scope and the counted-skipped behavior.
         *
         *      @param name the case title shown in the report
         *      @param block the case body
         *
         */
        function oit(name: string, block: ()=>void): void;

        /**
         * @description Defines a case selected by `only`; the selection applies to the enclosing
         *      suite, whose other direct children are skipped
         *
         *      The scope is the suite that collects the case: marking a case inside a nested suite
         *      does not affect the parent's sibling suites. Selected cases run; unselected cases are
         *      counted as skipped, and unselected sub-suites are pruned without being counted. The
         *      options form `{ only: true }` and `it.only` do the same thing.
         *
         *      Example — only selects within the enclosing suite:
         *      ```JavaScript
         *      const { describe, it, only } = require('node:test');
         *      describe('selection', () => {
         *          only('selected', () => { });
         *          it('skipped, counted as skipped', () => { throw new Error('never runs'); });
         *      });
         *      ```
         *
         *      @param name the case title shown in the report
         *      @param block the case body
         *
         */
        function only(name: string, block: ()=>void): void;

        /**
         * @description Defines a planned case; it is counted in the todo total and its body is not
         *      executed, which differs from Node.js where a todo body runs and its failure is silenced
         *
         *      The one-argument form declares a planned case without a body. `skip: true` takes
         *      precedence over the todo level in the options form.
         *
         *      @param name the case title shown in the report
         *      @param block the case body, which is never executed
         *
         */
        function todo(name: string, block: ()=>void): void;

        /**
         * @description Defines a planned case with an options object; `todo` is the default level
         *      and `skip: true` takes precedence, while `only: true` selects the case
         *
         *      The body is not executed for a todo case; see the two-argument form for the shared
         *      semantics.
         *
         *      @param name the case title shown in the report
         *      @param options the case options: { skip, todo, only }
         *      @param block the case body, which is never executed
         *
         */
        function todo(name: string, options: FIBJS.GeneralObject, block: ()=>void): void;

        /**
         * @description Declares a planned case without a body; the case is counted in the todo total
         *      and nothing is executed
         *
         *      Use it to keep a placeholder for a case that is not written yet; the title still appears
         *      in the report with the todo marker, and the run stays green.
         *
         *      @param name the case title shown in the report
         *
         */
        function todo(name: string): void;

        /**
         * @description Registers a hook that runs once before the cases of the suite being collected;
         *      registered at top level it belongs to the file's root suite
         *
         *      Multiple `before` hooks run in registration order, outer suite first; a hook may be an
         *      async function, in which case it is awaited. If a `before` hook throws, every case of
         *      the suite is reported as failed without being executed.
         *
         *      Example — setup before a suite:
         *      ```JavaScript
         *      const { describe, it, before } = require('node:test');
         *      const assert = require('assert');
         *      let ready = false;
         *      describe('with setup', () => {
         *          before(() => { ready = true; });
         *          it('sees the setup', () => assert.ok(ready));
         *      });
         *      ```
         *
         *      @param func the hook function
         *
         */
        function before(func: ()=>void): void;

        /**
         * @description Registers a hook that runs once after the cases of the suite being collected;
         *      registered at top level it belongs to the file's root suite
         *
         *      Hooks run inner suite first, and multiple hooks on the same suite run in reverse
         *      registration order. A failing `after` hook is reported as a separate failure and does
         *      not change the already printed per-case results.
         *
         *      @param func the hook function
         *
         */
        function after(func: ()=>void): void;

        /**
         * @description Registers a hook that runs before every case of the suite being collected;
         *      registered at top level it applies to every case of the file
         *
         *      Hooks run outer suite first and in registration order. A hook failure fails the current
         *      case and skips its body, while the remaining cases of the suite keep running.
         *
         *      @param func the hook function
         *
         */
        function beforeEach(func: ()=>void): void;

        /**
         * @description Registers a hook that runs after every case of the suite being collected;
         *      registered at top level it applies to every case of the file
         *
         *      Hooks run inner suite first. A failing `afterEach` hook turns the current case into a
         *      failure; when that is the only failure, the summary reports it but the process exit
         *      code stays 0 (Node.js exits 1 in this situation).
         *
         *      @param func the hook function
         *
         */
        function afterEach(func: ()=>void): void;

        /**
         * @description Wraps a function so that the current case must call it; the runner waits for
         *      the wrapper after the case body and the case fails if it was called from another case
         *
         *      mustCall must be used inside a running case: outside one it throws error 20009. If the
         *      wrapper is never called, the case never finishes and the run waits forever, because the
         *      watchdog is armed only after the run; the process must then be killed.
         *
         *      Example — the wrapped function is called inside the case:
         *      ```JavaScript
         *      const { it, mustCall } = require('node:test');
         *      const assert = require('assert');
         *      it('calls the callback', () => {
         *          const done = mustCall(function (value) { return value + 1; });
         *          assert.equal(done(1), 2);
         *      });
         *      ```
         *
         *      @param func the function to wrap
         *      @return the wrapped function, which forwards arguments and the return value
         *
         */
        function mustCall(func: (...args: any[])=>any): (...args: any[])=>any;

        /**
         * @description Wraps a function so that the current case must not call it; calling the
         *      wrapper records a failure and throws "This function must never be called." (error 20002)
         *
         *      The wrapped function is not invoked through the wrapper: the wrapper only records the
         *      violation. The function argument is ignored, so this form and `mustNotCall()` behave
         *      the same; both must be used inside a running case (error 20009 outside one).
         *
         *      @param func the function to ignore
         *      @return the wrapped function that throws when called
         *
         */
        function mustNotCall(func: (...args: any[])=>any): (...args: any[])=>any;

        /**
         * @description Returns a function that throws "This function must never be called." (error
         *      20002) and records a failure in the current case when it is called
         *
         *      Use it where an API is expected to stay silent, for example to assert that a callback
         *      is not invoked; the case passes as long as the wrapper is not called.
         *
         *      Example — the forbidden callback is never invoked:
         *      ```JavaScript
         *      const { it, mustNotCall } = require('node:test');
         *      it('does not call back', () => {
         *          const onError = mustNotCall();
         *          // onError is never invoked, so the case passes
         *      });
         *      ```
         *
         *      @return the wrapped function that throws when called
         *
         */
        function mustNotCall(): (...args: any[])=>any;

        /**
         * @description The slow-case warning threshold in milliseconds, default 75; a case whose
         *      duration exceeds half of it is printed with its duration, and above the full threshold
         *      the line is marked in the error color
         *
         *      The property only drives the report: it never fails a case and is not a timeout. The
         *      value is read when a case's line is printed, so setting it at definition time applies
         *      to the whole run.
         *
         *      Example — tighten the duration warning threshold:
         *      ```JavaScript
         *      const test = require('test');
         *      test.slow = 10;
         *      test('quick case', () => { });
         *      ```
         *
         *
         */
        var slow: number;

    }

    export = test;

}

