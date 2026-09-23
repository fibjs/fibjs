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
