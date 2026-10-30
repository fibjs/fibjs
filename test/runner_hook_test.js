// after-hook failure semantics of the fibjs --test runner.
//
// Node (v23.9, measured): a throwing `after` hook does NOT change the status of
// the suite's tests (they keep their own result) and is NOT counted in the
// `pass`/`fail` counters - it is reported as one separate failing entry for the
// suite.  fibjs used to do the opposite: it rewrote every already-passed
// sibling result to "failed" and did `pass--/fail++`, so a single broken hook
// printed a fully green tree followed by "N tests failed" (the Windows worker
// suite reported "× 72 tests failed" with 72 √ lines).
//
// fibjs now matches Node's reporting, with one deliberate deviation: the hook
// failure still fails the run (exit code 1), because an uncounted failure would
// otherwise slip through CI unnoticed.
//
// The assertions are fibjs reporter specific, so the suites are skipped on node.
var { describe, it } = require('node:test');
var assert = require('assert');
var path = require('path');
var child_process = require('child_process');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

const FIXTURES_DIR = path.join(__dirname, 'runner_hook_files');
const ALL_PASS = path.join(FIXTURES_DIR, 'after_hook_all_pass.js');
const MIXED = path.join(FIXTURES_DIR, 'after_hook_mixed.js');

function runFile(file) {
    const r = child_process.spawnSync(process.execPath, ['--test', file], {
        encoding: 'utf8',
        env: {
            ...process.env,
            RUNNER_HOOK_FIXTURE: '1'
        }
    });

    return { code: r.status, out: (r.stdout || '') + (r.stderr || '') };
}

// "√ 3 tests passed" / "× 1 tests failed" / "3 tests completed"
function summary(out) {
    const get = (re) => {
        const m = re.exec(out);
        return m ? +m[1] : 0;
    };

    return {
        completed: get(/(\d+) tests completed/),
        passed: get(/√ (\d+) tests passed/),
        failed: get(/× (\d+) tests failed/)
    };
}

describe('runner after hook failure', { skip: !isFibjs }, () => {
    it('keeps passed tests passed and reports the hook failure separately', () => {
        const { code, out } = runFile(ALL_PASS);
        const sum = summary(out);

        // Node: tests 3 / pass 3 / fail 0 (the hook failure is not a test)
        assert.strictEqual(sum.completed, 3, out);
        assert.strictEqual(sum.passed, 3, out);
        assert.strictEqual(sum.failed, 0, out);

        // ...but it is still reported, and it still fails the run
        assert.ok(/\[after failed\] suite/.test(out), out);
        assert.ok(/boom in after hook/.test(out), out);
        assert.ok(/1\) suite \(after hook\)/.test(out), out);
        assert.strictEqual(code, 1, out);
    });

    it('leaves a real failure as the only counted failure', () => {
        const { code, out } = runFile(MIXED);
        const sum = summary(out);

        // Node: tests 4 / pass 3 / fail 1 - only the really failing test counts
        assert.strictEqual(sum.completed, 4, out);
        assert.strictEqual(sum.passed, 3, out);
        assert.strictEqual(sum.failed, 1, out);

        assert.ok(/intentional failure/.test(out), out);
        assert.ok(/boom in after hook/.test(out), out);
        assert.ok(/\d\) suite \(after hook\)/.test(out), out);
        assert.strictEqual(code, 1, out);
    });
});

// ---------------------------------------------------------------------------
// hook_lifecycle_fixture.js: declaration-order hooks, afterEach short-circuit,
// before-failure gating and todo bodies. The expected STEP sequences were
// measured with `node --test --test-isolation=none <fixture>` on the same file.
// ---------------------------------------------------------------------------

const LIFECYCLE = path.join(FIXTURES_DIR, 'hook_lifecycle_fixture.js');

function steps(out) {
    return out.split('\n')
        .filter((line) => line.indexOf('STEP ') === 0)
        .map((line) => line.slice(5));
}

describe('runner hook lifecycle', { skip: !isFibjs }, () => {
    it('runs same-level hooks in declaration order and inner level first', () => {
        const { out } = runFile(LIFECYCLE);
        const sequence = steps(out);
        const order = sequence.filter((s) => s.indexOf('order:') === 0 || s.indexOf('inner:') === 0);

        assert.deepEqual(order, [
            'order:before#1', 'order:before#2',
            'order:beforeEach#1', 'order:beforeEach#2', 'order:case',
            'order:afterEach#1', 'order:afterEach#2',
            'order:beforeEach#1', 'order:beforeEach#2', 'inner:case',
            'inner:afterEach', 'order:afterEach#1', 'order:afterEach#2',
            'order:after#1', 'order:after#2'
        ], out);
    });

    it('stops the afterEach chain when a hook throws', () => {
        const { out } = runFile(LIFECYCLE);
        const sequence = steps(out);
        const thrown = sequence.filter((s) => s.indexOf('throw:') === 0);

        // node: the throwing afterEach aborts the rest, including the ancestor's
        assert.deepEqual(thrown, ['throw:case', 'throw:inner afterEach#1'], out);
    });

    it('a failed before keeps the subtests and their afterEach from running', () => {
        const { out } = runFile(LIFECYCLE);
        const sequence = steps(out);
        const failing = sequence.filter((s) => s.indexOf('beforethrow:') === 0 || s.indexOf('clean:') === 0);

        // node: beforethrow:after only (the owner's own after() still runs from
        // the catch path); the nested clean suite never starts
        assert.deepEqual(failing, ['beforethrow:before', 'beforethrow:after'], out);
    });

    it('runs todo bodies and their per-case hooks', () => {
        const { out } = runFile(LIFECYCLE);
        const sequence = steps(out);
        const todo = sequence.filter((s) => s.indexOf('todo:') === 0);

        assert.deepEqual(todo, [
            'todo:before',
            'todo:beforeEach', 'todo:body ok', 'todo:afterEach',
            'todo:beforeEach', 'todo:body plain', 'todo:afterEach',
            'todo:after'
        ], out);
    });

    it('a describe.todo still runs its body and is counted as todo', () => {
        const { out } = runFile(LIFECYCLE);
        const sequence = steps(out);

        assert.deepEqual(sequence.filter((s) => s.indexOf('todosuite:') === 0),
            ['todosuite:before', 'todosuite:case'], out);
        assert.ok(/☐ case/.test(out), out);
    });
});

// ---------------------------------------------------------------------------
// legacy_style_fixture.js: the single-parameter callback style is retired.
// ---------------------------------------------------------------------------

const LEGACY_STYLE = path.join(FIXTURES_DIR, 'legacy_style_fixture.js');

function warnings(out) {
    return out.split('\n').filter((line) => line.indexOf('warning: test ') === 0);
}

describe('runner retired callback style', { skip: !isFibjs }, () => {
    it('warns once when a one-parameter body looks like a callback', () => {
        const { out } = runFile(LEGACY_STYLE);
        const lines = warnings(out);

        assert.strictEqual(lines.length, 1, out);
        assert.ok(/retired callback style/.test(lines[0]), out);
        assert.ok(/parameter named 'done'/.test(lines[0]), out);
        assert.ok(/\(ctx, done\) => \{\}/.test(lines[0]), out);
    });

    it('does not warn for the context or the two-parameter form', () => {
        const { out } = runFile(LEGACY_STYLE);

        assert.ok(out.indexOf("warning: test 'context style'") < 0, out);
        assert.ok(out.indexOf("warning: test 'two parameters'") < 0, out);
    });

    it('hands the context to a one-parameter body and lets the call fail', () => {
        const { code, out } = runFile(LEGACY_STYLE);
        const sum = summary(out);

        // node: the same file fails the two callback-shaped cases with
        // "done is not a function"; only the context and two-arg forms pass.
        assert.strictEqual(sum.completed, 4, out);
        assert.strictEqual(sum.passed, 2, out);
        assert.strictEqual(sum.failed, 2, out);
        assert.ok(/LEGACY ctx-name=context style/.test(out), out);
        assert.ok(/LEGACY two-arg-name=two parameters/.test(out), out);
        assert.strictEqual(code, 1, out);
    });
});

// ---------------------------------------------------------------------------
// late_error_fixture.js: an error raised after the test ended fails the run
// (Node: "generated asynchronous activity after the test ended").
// ---------------------------------------------------------------------------

const LATE_ERROR = path.join(FIXTURES_DIR, 'late_error_fixture.js');

describe('runner late asynchronous error', { skip: !isFibjs }, () => {
    it('keeps the green tree but fails the run', () => {
        const { code, out } = runFile(LATE_ERROR);
        const sum = summary(out);

        assert.strictEqual(sum.completed, 2, out);
        assert.strictEqual(sum.passed, 2, out);
        assert.strictEqual(sum.failed, 0, out);

        assert.ok(/late boom/.test(out), out);
        assert.ok(/asynchronous activity after the tests ended/.test(out), out);
        assert.strictEqual(code, 1, out);
    });
});

// ---------------------------------------------------------------------------
// context_fixture.js: the context object handed to bodies and hooks.
// The expected CTX records were measured with
// `node --test --test-isolation=none <fixture>` on the same file.
// ---------------------------------------------------------------------------

const CONTEXT_FIXTURE = path.join(FIXTURES_DIR, 'context_fixture.js');

function ctxRecords(out) {
    return out.split('\n').filter((line) => line.indexOf('CTX ') === 0);
}

describe('runner context object', { skip: !isFibjs }, () => {
    it('passes name/fullName to suites, hooks and cases', () => {
        const { out } = runFile(CONTEXT_FIXTURE);
        const records = ctxRecords(out);

        assert.deepEqual(records.filter((r) => r.indexOf('CTX api') !== 0), [
            'CTX suite name=ctx-suite fullName=ctx-suite',
            'CTX before name=ctx-suite',
            'CTX beforeEach name=ctx-case',
            'CTX case name=ctx-case fullName=ctx-suite > ctx-case',
            'CTX afterEach name=ctx-case',
            'CTX beforeEach name=ctx-skip',
            'CTX skip called',
            'CTX beforeEach name=ctx-todo',
            'CTX todo called',
            'CTX afterEach name=ctx-todo',
            'CTX beforeEach name=ctx-plain',
            'CTX plain',
            'CTX afterEach name=ctx-plain'
        ], out);
    });

    it('exposes skip/todo/diagnostic/assert on the case context', () => {
        const { out } = runFile(CONTEXT_FIXTURE);
        assert.ok(out.indexOf('CTX api skip=function todo=function diagnostic=function assert=function') >= 0, out);
    });

    it('a runtime skip suppresses afterEach; a runtime todo does not fail', () => {
        const { code, out } = runFile(CONTEXT_FIXTURE);
        const sum = summary(out);

        // node: tests 4 / pass 2 / fail 0 / skipped 1 / todo 1
        assert.strictEqual(sum.completed, 4, out);
        assert.strictEqual(sum.passed, 2, out);
        assert.strictEqual(sum.failed, 0, out);
        assert.ok(/○ 1 tests skipped/.test(out), out);
        assert.ok(/☐ 1 todo tests/.test(out), out);
        assert.strictEqual(code, 0, out);
    });
});
