// --test-name-pattern CLI regression tests (fibjs --test runner).
// Spawns the fibjs binary on the runner_filter_files/ fixtures with various
// --test-name-pattern values and asserts run counts, hook gating, regex
// semantics and exit codes. Counts were cross-checked against node v23.9
// where node has an equivalent; the assertions are fibjs reporter specific,
// so the suites are skipped on node.

var { describe, it } = require('node:test');
var assert = require('assert');
var path = require('path');
var child_process = require('child_process');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

const FIXTURES_DIR = path.join(__dirname, 'runner_filter_files');
const FIXTURE = path.join(FIXTURES_DIR, 'name_pattern_fixture.js');
const HOOKS_FIXTURE = path.join(FIXTURES_DIR, 'hooks_fixture.js');
const SKIP_TODO_FIXTURE = path.join(FIXTURES_DIR, 'skip_todo_fixture.js');
const ONLY_FIXTURE = path.join(FIXTURES_DIR, 'only_fixture.js');

function runFiles(files, patterns, extraEnv) {
    const args = ['--test'];
    for (const p of patterns)
        args.push(`--test-name-pattern=${p}`);
    args.push(...files);

    const r = child_process.spawnSync(process.execPath, args, {
        encoding: 'utf8',
        env: {
            ...process.env,
            NAME_PATTERN_FIXTURE: '1',
            ...(extraEnv || {})
        }
    });
    return { code: r.status, stdout: r.stdout || '', stderr: r.stderr || '' };
}

function runWith(pattern, extraEnv) {
    return runFiles([FIXTURE], pattern === undefined ? [] : [pattern], extraEnv);
}

// Counters from the fibjs spec-like summary
function summary(out) {
    const m = {
        completed: /(\d+) tests completed/.exec(out),
        passed: /√ (\d+) tests passed/.exec(out),
        skipped: /○ (\d+) tests skipped/.exec(out),
        todo: /☐ (\d+) todo tests/.exec(out)
    };
    return {
        completed: m.completed ? +m.completed[1] : 0,
        passed: m.passed ? +m.passed[1] : 0,
        skipped: m.skipped ? +m.skipped[1] : 0,
        todo: m.todo ? +m.todo[1] : 0
    };
}

function countOf(out, marker) {
    return out.split(marker).length - 1;
}

describe('test runner --test-name-pattern', { skip: !isFibjs }, () => {
    it('leaf name match runs only that test; ancestor hooks gate', () => {
        const r = runWith('a1');
        assert.equal(r.code, 0, r.stderr);
        assert.ok(r.stdout.indexOf('[A-before]') >= 0, 'A container hooks must run');
        assert.ok(r.stdout.indexOf('[A-after]') >= 0);
        assert.ok(r.stdout.indexOf('[B-before]') < 0, 'B hooks must not run');
        assert.ok(r.stdout.indexOf('[C-before]') < 0, 'C hooks must not run');
        const s = summary(r.stdout);
        assert.equal(s.passed, 1);
        assert.equal(s.skipped, 0);
        assert.equal(s.todo, 0);
    });

    it('deep leaf match keeps container chain only', () => {
        const r = runWith('b1');
        assert.equal(r.code, 0, r.stderr);
        assert.ok(r.stdout.indexOf('[A-before]') >= 0);
        assert.ok(r.stdout.indexOf('[B-before]') >= 0);
        assert.ok(r.stdout.indexOf('[C-before]') < 0);
        assert.equal(summary(r.stdout).passed, 1);
    });

    it('suite name match runs the whole subtree', () => {
        const r = runWith('suite-A');
        assert.equal(r.code, 0, r.stderr);
        assert.ok(r.stdout.indexOf('[B-before]') >= 0);
        assert.ok(r.stdout.indexOf('[C-before]') >= 0);
        assert.equal(summary(r.stdout).passed, 4); // a1, b1, c1, fail-zzz
    });

    it('suite title match enables its whole subtree (chain rule)', () => {
        const r = runWith('suite-B');
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).passed, 1); // only b1
    });

    it('no match: zero tests, exit 0', () => {
        const r = runWith('zzz-no-match');
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).completed, 0);
    });

    it('a failing test outside the pattern does not fail the run', () => {
        const r = runWith('a1', { RUN_FAIL: '1' });
        assert.equal(r.code, 0, r.stdout);
        assert.equal(summary(r.stdout).passed, 1);
    });

    it('a failing test inside the pattern fails the run', () => {
        const r = runWith('fail-zzz', { RUN_FAIL: '1' });
        assert.notEqual(r.code, 0);
    });

    it('repeated flags OR together', () => {
        const all = runWith(undefined, {});
        assert.equal(all.code, 0, all.stderr);
        assert.equal(summary(all.stdout).passed, 4); // a1, b1, c1, fail-zzz

        const two = runFiles([FIXTURE], ['a1', 'c1']);
        assert.equal(two.code, 0, two.stderr);
        assert.equal(summary(two.stdout).passed, 2);
    });
});

// ---------------------------------------------------------------------------
// regex semantics: anchoring, case sensitivity, alternation
// ---------------------------------------------------------------------------

describe('--test-name-pattern regex semantics', { skip: !isFibjs }, () => {
    it('anchored pattern ^hx1$ matches exactly that test', () => {
        const r = runFiles([HOOKS_FIXTURE], ['^hx1$']);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.passed, 1);
        assert.equal(s.completed, 1);
    });

    it('suffix and substring semantics', () => {
        const r = runFiles([HOOKS_FIXTURE], ['my2$']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).passed, 1);

        // bare '1' hits every test title ending in 1 across both suites
        const all = runFiles([HOOKS_FIXTURE], ['1$']);
        assert.equal(all.code, 0, all.stderr);
        assert.equal(summary(all.stdout).passed, 4); // hx1, my1, my2, z1
        assert.ok(all.stdout.indexOf('[H-before]') >= 0);
        assert.ok(all.stdout.indexOf('[O-before]') >= 0);
    });

    it('matching is case sensitive', () => {
        const r = runFiles([HOOKS_FIXTURE], ['A1']); // no title contains 'A1'
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).completed, 0);

        const t = runFiles([FIXTURE], ['^SUITE-A$']); // uppercase suite title
        assert.equal(summary(t.stdout).completed, 0);
    });

    it('alternation inside a single flag', () => {
        const r = runFiles([HOOKS_FIXTURE], ['hx1|z1']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).passed, 2);
        assert.ok(r.stdout.indexOf('[H-before]') >= 0);
        assert.ok(r.stdout.indexOf('[O-before]') >= 0);
    });

    it('invalid regular expression fails fast with a clear message', () => {
        const r = child_process.spawnSync(process.execPath,
            ['--test', '--test-name-pattern=[a', FIXTURE], {
            encoding: 'utf8',
            env: { ...process.env, NAME_PATTERN_FIXTURE: '1' }
        });
        assert.notEqual(r.status, 0);
        assert.ok((r.stderr + r.stdout).indexOf('--test-name-pattern must be a valid regular expression') >= 0);
    });
});

// ---------------------------------------------------------------------------
// hooks_fixture.js: hook-suite{ H before/beforeEach, hx1,
//                    mid{ M, my1, my2 }, L1{ L1, L2{ L2, deepleaf } } },
//                    other-suite{ O, z1 }
// ---------------------------------------------------------------------------

describe('--test-name-pattern hook gating', { skip: !isFibjs }, () => {
    it('unfiltered control runs everything with all hooks', () => {
        const r = runFiles([HOOKS_FIXTURE], []);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.passed, 5); // hx1, my1, my2, deepleaf, z1
        assert.equal(s.skipped, 0);
        // z1 lives under other-suite, outside hook-suite's beforeEach scope
        assert.equal(countOf(r.stdout, '[H-beforeEach]'), 4);
        assert.equal(countOf(r.stdout, '[M-before]'), 1);
        assert.equal(countOf(r.stdout, '[L1-before]'), 1);
        assert.equal(countOf(r.stdout, '[L2-before]'), 1);
        assert.equal(countOf(r.stdout, '[O-before]'), 1);
    });

    it('beforeEach only wraps the executed test, not pruned siblings', () => {
        const r = runFiles([HOOKS_FIXTURE], ['my2']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).passed, 1);
        assert.ok(r.stdout.indexOf('[H-before]') >= 0);
        assert.ok(r.stdout.indexOf('[M-before]') >= 0);
        assert.equal(countOf(r.stdout, '[H-beforeEach]'), 1);
    });

    it('hooks of an invisible sibling suite never run', () => {
        const r = runFiles([HOOKS_FIXTURE], ['hx1']);
        assert.equal(r.code, 0, r.stderr);
        assert.ok(r.stdout.indexOf('[H-before]') >= 0);
        assert.ok(r.stdout.indexOf('[M-before]') < 0);
        assert.ok(r.stdout.indexOf('[O-before]') < 0);
        assert.equal(countOf(r.stdout, '[H-beforeEach]'), 1);
    });

    it('matching a test in the second suite keeps the first fully invisible', () => {
        const r = runFiles([HOOKS_FIXTURE], ['z1']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).passed, 1);
        assert.ok(r.stdout.indexOf('[O-before]') >= 0);
        assert.ok(r.stdout.indexOf('[H-before]') < 0);
        assert.ok(r.stdout.indexOf('[M-before]') < 0);
    });

    it('three-level deep leaf match runs every container hook', () => {
        const r = runFiles([HOOKS_FIXTURE], ['deepleaf']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).passed, 1);
        assert.ok(r.stdout.indexOf('[H-before]') >= 0);
        assert.ok(r.stdout.indexOf('[L1-before]') >= 0);
        assert.ok(r.stdout.indexOf('[L2-before]') >= 0);
        assert.ok(r.stdout.indexOf('[M-before]') < 0);
    });

    it('suite title match on an inner suite runs that subtree only', () => {
        const r = runFiles([HOOKS_FIXTURE], ['^mid$']);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.passed, 2); // my1, my2
        assert.equal(countOf(r.stdout, '[H-beforeEach]'), 2);
        assert.ok(r.stdout.indexOf('[O-before]') < 0);
    });
});

// ---------------------------------------------------------------------------
// skip_todo_fixture.js: st-suite{ run-hit, skip-hit, skip-miss, todo-hit,
//                        todo-miss } (counts verified against node v23.9)
// ---------------------------------------------------------------------------

describe('--test-name-pattern skip/todo interplay', { skip: !isFibjs }, () => {
    it('unfiltered control: pass + skip + todo all counted', () => {
        const r = runFiles([SKIP_TODO_FIXTURE], []);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.completed, 5);
        assert.equal(s.passed, 1);
        assert.equal(s.skipped, 2);
        assert.equal(s.todo, 2);
    });

    it('matched skip/todo tests keep their counted semantics', () => {
        const r = runFiles([SKIP_TODO_FIXTURE], ['hit']);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.completed, 3);
        assert.equal(s.passed, 1); // run-hit
        assert.equal(s.skipped, 1); // skip-hit
        assert.equal(s.todo, 1); // todo-hit
    });

    it('pattern-missed skip/todo tests are invisible, not counted', () => {
        const r = runFiles([SKIP_TODO_FIXTURE], ['miss']);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.completed, 2);
        assert.equal(s.passed, 0);
        assert.equal(s.skipped, 1); // skip-miss
        assert.equal(s.todo, 1); // todo-miss

        const none = runFiles([SKIP_TODO_FIXTURE], ['zzz']);
        assert.equal(none.code, 0);
        assert.equal(summary(none.stdout).completed, 0);
    });

    it('pattern hitting the suite title exposes its skip/todo content', () => {
        const r = runFiles([SKIP_TODO_FIXTURE], ['st-suite']);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.passed, 1);
        assert.equal(s.skipped, 2);
        assert.equal(s.todo, 2);
    });
});

// ---------------------------------------------------------------------------
// only_fixture.js: only-parent{ only-target(only), plain-leaf }
// fibjs has native only-mode (node needs --test-only / non-process isolation),
// so the unfiltered column is fibjs' own documented counted-skip behaviour.
// While a name filter is active the two runtimes must agree again: a filtered
// run drops the counted-skip entries entirely (node: `tests 1 / skipped 0` for
// only-target, `tests 0` for a pattern that misses the only-marked case —
// measured with `node --test-isolation=none`).
// ---------------------------------------------------------------------------

describe('--test-name-pattern with only-marked tests', { skip: !isFibjs }, () => {
    it('unfiltered control: only-marked test runs, sibling counted-skip', () => {
        const r = runFiles([ONLY_FIXTURE], []);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.passed, 1);
        assert.equal(s.skipped, 1);
    });

    it('pattern matching the only-marked test keeps it running', () => {
        const r = runFiles([ONLY_FIXTURE], ['only-target']);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.passed, 1);
        assert.equal(s.skipped, 0); // filtered runs drop the counted-skip entry
        assert.equal(s.completed, 1);
    });

    it('pattern missing the only-marked test leaves nothing behind', () => {
        for (const pattern of ['plain-leaf', 'zzz']) {
            const r = runFiles([ONLY_FIXTURE], [pattern]);
            assert.equal(r.code, 0, r.stderr);
            const s = summary(r.stdout);
            assert.equal(s.passed, 0, pattern);
            assert.equal(s.skipped, 0, pattern); // node: tests 0
            assert.equal(s.completed, 0, pattern);
        }
    });
});

// ---------------------------------------------------------------------------
// multiple files in one invocation
// ---------------------------------------------------------------------------

describe('--test-name-pattern across multiple files', { skip: !isFibjs }, () => {
    it('pattern from file A leaves file B with zero tests', () => {
        const r = runFiles([FIXTURE, HOOKS_FIXTURE], ['a1']);
        assert.equal(r.code, 0, r.stderr);
        const s = summary(r.stdout);
        assert.equal(s.passed, 1);
        assert.equal(s.completed, 1);
    });

    it('pattern from file B leaves file A with zero tests', () => {
        const r = runFiles([FIXTURE, HOOKS_FIXTURE], ['my1']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).passed, 1);
    });

    it('combined across both files in one run', () => {
        const r = runFiles([FIXTURE, HOOKS_FIXTURE], ['a1', 'my1']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).passed, 2);
    });
});

// ---------------------------------------------------------------------------
// full_name_fixture.js: alpha{ beta{gamma, delta}, gamma }, omega{first, second}
// Both gamma tests exist, so only the ancestor-joined full name can address one
// of them. Every expected number below was measured with
//   node --test --test-isolation=none [--test-name-pattern=<p>] <fixture>
// ---------------------------------------------------------------------------

const FULL_NAME_FIXTURE = path.join(FIXTURES_DIR, 'full_name_fixture.js');

function logs(out) {
    return out.split('\n')
        .filter((line) => line.charAt(0) === '[' && line.charAt(line.length - 1) === ']')
        .map((line) => line.slice(1, -1));
}

describe('--test-name-pattern full name', { skip: !isFibjs }, () => {
    it('unfiltered control runs everything', () => {
        const r = runFiles([FULL_NAME_FIXTURE], []);
        assert.equal(r.code, 0, r.stderr);
        assert.deepEqual(logs(r.stdout), [
            'alpha-before', 'beta-before',
            'beta-gamma', 'beta-delta',
            'beta-after', 'alpha-gamma',
            'alpha-after',
            'omega-before', 'omega-first', 'omega-second', 'omega-after'
        ]);
    });

    it("'beta gamma' addresses beta > gamma only", () => {
        const r = runFiles([FULL_NAME_FIXTURE], ['beta gamma']);
        assert.equal(r.code, 0, r.stderr);
        assert.deepEqual(logs(r.stdout), [
            'alpha-before', 'beta-before', 'beta-gamma', 'beta-after', 'alpha-after'
        ]);
        assert.equal(summary(r.stdout).completed, 1);
    });

    it("'alpha gamma' addresses alpha > gamma (the sibling with the same name)", () => {
        const r = runFiles([FULL_NAME_FIXTURE], ['alpha gamma']);
        assert.equal(r.code, 0, r.stderr);
        assert.deepEqual(logs(r.stdout), ['alpha-before', 'alpha-gamma', 'alpha-after']);
        assert.equal(summary(r.stdout).completed, 1);
    });

    it("a suite-name prefix runs that whole subtree", () => {
        const r = runFiles([FULL_NAME_FIXTURE], ['alpha beta']);
        assert.equal(r.code, 0, r.stderr);
        assert.deepEqual(logs(r.stdout), [
            'alpha-before', 'beta-before',
            'beta-gamma', 'beta-delta',
            'beta-after', 'alpha-after'
        ]);
        assert.equal(summary(r.stdout).completed, 2);
    });

    it("the ' > ' joined form does not match", () => {
        // node joins ancestors with a single space; TestContext.fullName uses
        // ' > ', and that string must not be what the filter matches.
        for (const pattern of ['alpha > beta', 'beta > gamma']) {
            const r = runFiles([FULL_NAME_FIXTURE], [pattern]);
            assert.equal(r.code, 0, r.stderr);
            assert.deepEqual(logs(r.stdout), [], pattern);
        }
    });

    it('a regular expression literal keeps its flags', () => {
        const r = runFiles([FULL_NAME_FIXTURE], ['/ALPHA BETA/i']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).completed, 2);

        // Without the flag the same source matches nothing (case sensitive).
        const strict = runFiles([FULL_NAME_FIXTURE], ['/ALPHA BETA/']);
        assert.equal(strict.code, 0, strict.stderr);
        assert.equal(summary(strict.stdout).completed, 0);
    });

    it('the root context is addressable as <root>', () => {
        // node names the root test '<root>', and testMatchesPattern recurses into
        // it, so the pattern matches every case.
        const r = runFiles([FULL_NAME_FIXTURE], ['<root>']);
        assert.equal(r.code, 0, r.stderr);
        assert.equal(summary(r.stdout).completed, 5);
    });
});

// ---------------------------------------------------------------------------
// --test-skip-pattern (same fixture; expected counts from node)
//   node --test-isolation=none --test-skip-pattern='<p>' <fixture>
// ---------------------------------------------------------------------------

function runSkip(files, skipPatterns, namePatterns) {
    const args = ['--test'];
    for (const p of namePatterns || [])
        args.push(`--test-name-pattern=${p}`);
    for (const p of skipPatterns)
        args.push(`--test-skip-pattern=${p}`);
    args.push(...files);

    const r = child_process.spawnSync(process.execPath, args, {
        encoding: 'utf8',
        env: { ...process.env, NAME_PATTERN_FIXTURE: '1' }
    });
    return { code: r.status, stdout: r.stdout || '', stderr: r.stderr || '' };
}

describe('--test-skip-pattern', { skip: !isFibjs }, () => {
    it('a skip pattern on a suite prunes the whole subtree', () => {
        const r = runSkip([FULL_NAME_FIXTURE], ['^omega$']);
        assert.equal(r.code, 0, r.stderr);
        assert.deepEqual(logs(r.stdout), [
            'alpha-before', 'beta-before',
            'beta-gamma', 'beta-delta',
            'beta-after', 'alpha-gamma',
            'alpha-after'
        ]);
        // node: tests 3 / pass 3 (only omega's two cases are dropped)
        assert.equal(summary(r.stdout).completed, 3);
    });

    it('a skip pattern on a leaf only drops that leaf', () => {
        const r = runSkip([FULL_NAME_FIXTURE], ['^delta$']);
        assert.equal(r.code, 0, r.stderr);
        assert.deepEqual(logs(r.stdout), [
            'alpha-before', 'beta-before',
            'beta-gamma',
            'beta-after', 'alpha-gamma',
            'alpha-after',
            'omega-before', 'omega-first', 'omega-second', 'omega-after'
        ]);
        // node: tests 4 / pass 4
        assert.equal(summary(r.stdout).completed, 4);
    });

    it('a skip pattern pruned suite does not run its hooks', () => {
        const r = runSkip([FULL_NAME_FIXTURE], ['omega']);
        assert.equal(r.code, 0, r.stderr);
        assert.ok(r.stdout.indexOf('omega-before') < 0, 'omega hooks must not run');
        assert.ok(r.stdout.indexOf('alpha-before') >= 0);
    });

    it('skip wins over a matching name pattern', () => {
        const r = runSkip([FULL_NAME_FIXTURE], ['beta'], ['alpha beta']);
        assert.equal(r.code, 0, r.stderr);
        assert.deepEqual(logs(r.stdout), []);
        assert.equal(summary(r.stdout).completed, 0);
    });

    it('repeated skip patterns OR together', () => {
        const r = runSkip([FULL_NAME_FIXTURE], ['^delta$', '^first$']);
        assert.equal(r.code, 0, r.stderr);
        const names = logs(r.stdout);
        assert.ok(names.indexOf('beta-delta') < 0);
        assert.ok(names.indexOf('omega-first') < 0);
        assert.ok(names.indexOf('beta-gamma') >= 0);
        assert.ok(names.indexOf('omega-second') >= 0);
        // node: tests 3 / pass 3
        assert.equal(summary(r.stdout).completed, 3);
    });

    it('a skip pattern on a full name works like the name pattern', () => {
        const r = runSkip([FULL_NAME_FIXTURE], ['alpha gamma']);
        assert.equal(r.code, 0, r.stderr);
        const names = logs(r.stdout);
        assert.ok(names.indexOf('alpha-gamma') < 0, 'alpha > gamma is skipped');
        assert.ok(names.indexOf('beta-gamma') >= 0, 'beta > gamma still runs');
    });

    it('an invalid skip pattern fails fast', () => {
        const r = runSkip([FULL_NAME_FIXTURE], ['[a']);
        assert.notEqual(r.code, 0);
        assert.ok((r.stderr + r.stdout).indexOf('--test-skip-pattern must be a valid regular expression') >= 0);
    });
});
