// Coverage for the IDL example runner:
//
//   tools/check_idl_examples.js   extraction, marker classification, fragment
//                                 syntax checks and the execution sandbox
//
// The expensive part is opt-in and cheap by default: the exhaustive run only
// covers the definitions checked off in plans/idl-doc-todos.md (none yet, so
// it is skipped), while the fixed smoke runs the self-contained fs example
// from the real corpus. IDL_EXAMPLES_SCOPE=all widens the exhaustive run to
// the whole corpus and IDL_EXAMPLES_LIMIT=n truncates it.

var { describe, it } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var os = require('os');
var path = require('path');

var examples = require('../tools/check_idl_examples');
var check_idl_docs = require('../tools/util/check_idl_docs');

var FIBJS = process.env.FIBJS || path.resolve(__dirname, '../bin/Linux_x64_release/fibjs');

// the parser dependency (pegjs) is provided by the build environment; without
// it only the synthetic cases below can run
var parser = null;
try {
    parser = require('../tools/util/parser');
} catch (e) { /* see above */ }

function realDefs() {
    return parser(path.resolve(__dirname, '../idl'));
}

function blockOf(owner, code, extra) {
    var parts = owner.split('.');
    var b = {
        owner: owner,
        def: parts[0],
        member: parts.length > 1 ? parts.slice(1).join('.') : null,
        where: parts.length > 1 ? 'member' : 'declare',
        index: 0,
        marker: null,
        class: null,
        value: null,
        code: code
    };
    Object.keys(extra || {}).forEach(function (k) { b[k] = extra[k]; });
    return b;
}

describe('check_idl_examples', () => {
    it('classifies unmarked, fragment, requires and malformed markers', () => {
        var runnable = examples.classifyBlock('var a = 1;');
        assert.equal(runnable.class, 'runnable');
        assert.equal(runnable.marker, null);
        assert.equal(runnable.value, null);

        var fragment = examples.classifyBlock('// fragment: constructor shape\nnew Foo(1, 2);');
        assert.equal(fragment.class, 'fragment');
        assert.equal(fragment.value, 'constructor shape');
        assert.equal(fragment.marker, '// fragment: constructor shape');

        var requires = examples.classifyBlock('// requires: redis\nredis.connect();');
        assert.equal(requires.class, 'requires');
        assert.equal(requires.value, 'redis');

        var malformed = examples.classifyBlock('// requires mysql\nvar a = 1;');
        assert.equal(malformed.class, 'malformed');
        assert.equal(malformed.marker, '// requires mysql');
    });

    it('detects fragment syntax errors (top-level await is legal)', () => {
        assert.ok(examples.fragmentSyntaxError('const broken = ;'));
        assert.equal(examples.fragmentSyntaxError('var ok = 1;'), null);
        assert.equal(examples.fragmentSyntaxError('await Promise.resolve();'), null);
    });

    it('reports a fragment without a reason and a malformed marker', async () => {
        var empty = await examples.runBlock(blockOf('Syn.empty', '// fragment:\nvar a = 1;'), { fibjs: FIBJS });
        assert.equal(empty.status, 'fail');
        assert.equal(empty.problem, true);
        assert.ok(/reason/.test(empty.error));

        var bad = await examples.runBlock(blockOf('Syn.bad', '// requires mysql\nvar a = 1;'), { fibjs: FIBJS });
        assert.equal(bad.status, 'fail');
        assert.equal(bad.problem, true);
        assert.ok(/malformed/.test(bad.error));

        var service = await examples.runBlock(blockOf('Syn.service', '// requires: postgres\nvar a = 1;'), { fibjs: FIBJS });
        assert.equal(service.status, 'fail');
        assert.equal(service.problem, true);
        assert.ok(/not one of/.test(service.error));
    });

    it('syntax-checks a fragment without executing it', async () => {
        var ok = await examples.runBlock(blockOf('Syn.fragment', '// fragment: call shape\nnew Date(0);'), { fibjs: FIBJS });
        assert.equal(ok.status, 'skip');
        assert.equal(ok.problem, false);

        var bad = await examples.runBlock(blockOf('Syn.fragment', '// fragment: call shape\nconst a = ;'), { fibjs: FIBJS });
        assert.equal(bad.status, 'fail');
        assert.equal(bad.problem, true);
        assert.ok(/not valid JavaScript/.test(bad.error));
    });

    it('skips a requires block unless its service is whitelisted', async () => {
        var skipped = await examples.runBlock(blockOf('Syn.redis', '// requires: redis\nvar a = 1;'), { fibjs: FIBJS });
        assert.equal(skipped.status, 'skip');
        assert.equal(skipped.service, 'redis');

        var empty = await examples.runBlock(blockOf('Syn.redis', '// requires:\nvar a = 1;'), { fibjs: FIBJS });
        assert.equal(empty.status, 'fail');
        assert.equal(empty.problem, true);
    });

    it('runs a passing runnable example', async () => {
        var res = await examples.runBlock(blockOf('Syn.good', 'console.log("passed");'), { fibjs: FIBJS });
        assert.equal(res.status, 'ok');
        assert.equal(res.exit, 0);
        assert.ok(/passed/.test(res.stdout));
    });

    it('reports a failing runnable example with its exit code and stderr', async () => {
        var res = await examples.runBlock(blockOf('Syn.throwing', 'throw new Error("boom");'), { fibjs: FIBJS });
        assert.equal(res.status, 'fail');
        assert.equal(res.exit, 1);
        assert.ok(/boom/.test(res.error), res.error);
    });

    it('times out a runnable example that never exits', async () => {
        var res = await examples.runBlock(blockOf('Syn.hanging', 'setInterval(function () {}, 1000);'),
            { fibjs: FIBJS, timeout: 600 });
        assert.equal(res.status, 'timeout');
        assert.ok(/timed out/.test(res.error), res.error);
    });

    it('accepts a long-running example that survives the window', async () => {
        var res = await examples.runBlock(
            blockOf('Syn.server', '// requires: long-running\nsetInterval(function () {}, 1000);'),
            { fibjs: FIBJS, requires: ['long-running'], longRunningMs: 600 });
        assert.equal(res.status, 'ok');
    });

    it('selects blocks by definition, owner and filter', () => {
        var blocks = [
            blockOf('fs', 'console.log(1);'),
            blockOf('fs.readFile', 'console.log(2);'),
            blockOf('path', 'console.log(3);'),
            blockOf('path.resolve', 'console.log(4);')
        ];
        assert.equal(examples.selectBlocks(blocks, {}).length, 4);
        assert.deepStrictEqual(examples.selectBlocks(blocks, { definitions: ['fs'] }).map(function (b) { return b.owner; }),
            ['fs', 'fs.readFile']);
        assert.deepStrictEqual(examples.selectBlocks(blocks, { filter: 'path.resolve' }).map(function (b) { return b.owner; }),
            ['path.resolve']);
        assert.deepStrictEqual(examples.selectBlocks(blocks, { filter: 'fs' }).map(function (b) { return b.owner; }),
            ['fs', 'fs.readFile']);
    });

    it('rejects an unknown --requires service', () => {
        assert.deepStrictEqual(examples.parseRequires('redis,mysql'), ['redis', 'mysql']);
        assert.throws(function () { examples.parseRequires('postgres'); });
    });

    it('lists the checked definitions of the todo file (possibly empty)', () => {
        var checked = examples.checkedDefinitions();
        assert.ok(Array.isArray(checked));
    });

    it('writes a report X8 accepts: failures count, skipped entries do not', () => {
        var results = [
            {
                owner: 'Syn.good', def: 'Syn', member: 'good', where: 'member', index: 0,
                marker: null, class: 'runnable', status: 'ok', exit: 0, error: null
            },
            {
                owner: 'Syn.redis', def: 'Syn', member: 'redis', where: 'member', index: 0,
                marker: '// requires: redis', class: 'requires', status: 'skip', exit: null,
                error: null, service: 'redis'
            },
            {
                owner: 'Syn.bad', def: 'Syn', member: 'bad', where: 'member', index: 0,
                marker: null, class: 'runnable', status: 'fail', exit: 1,
                error: 'SyntaxError: unexpected token', code: 'throw new Error("boom");'
            }
        ];
        var summary = examples.summarize(results);
        var report = examples.buildReport(results, summary, {
            fibjs: FIBJS, timeout: 10000, concurrency: 1, requires: []
        });

        report.results.forEach(function (e) {
            assert.ok(e.owner && e.where && e.class && e.status, JSON.stringify(e));
            assert.ok(e.exit !== undefined && e.error !== undefined, JSON.stringify(e));
        });
        assert.equal(report.results[1].skipped, true);
        assert.equal(report.results[0].skipped, false);

        var dir = fs.mkdtempSync(path.join(os.tmpdir(), 'idl-examples-report-'));
        var reportPath = path.join(dir, 'report.json');
        fs.writeFileSync(reportPath, JSON.stringify(report));
        try {
            // X8 must flag the failed entry and ignore the skipped one
            var x8 = check_idl_docs({}, { baseline: null, report: reportPath })
                .filter(function (p) { return p.rule === 'X8'; });
            assert.equal(x8.length, 1, JSON.stringify(x8));
            assert.equal(x8[0].def, 'Syn');
            assert.equal(x8[0].member, 'bad');
        } finally {
            try { fs.rmSync(dir, { recursive: true, force: true }); } catch (e) { /* disposable */ }
        }
    });

    describe('real corpus', { skip: !parser }, () => {
        it('extracts the blocks with owners and classifications', () => {
            var blocks = examples.extractBlocks(realDefs());
            assert.ok(blocks.length >= 300, 'the corpus carries hundreds of blocks, got ' + blocks.length);

            blocks.forEach(function (b) {
                assert.ok(b.owner === b.def || b.owner.indexOf(b.def + '.') === 0, b.owner);
                assert.ok(['declare', 'member'].indexOf(b.where) >= 0, b.where);
                assert.ok(['runnable', 'fragment', 'requires', 'malformed'].indexOf(b.class) >= 0, b.class);
            });

            var fs = blocks.filter(function (b) { return b.def === 'fs' && b.where === 'declare'; });
            assert.ok(fs.length >= 2, 'fs must carry its declaration examples');
        });

        it('runs the self-contained fs declaration example', async () => {
            var fs = examples.extractBlocks(realDefs()).filter(function (b) {
                return b.def === 'fs' && b.member === null;
            });
            assert.ok(fs.length, 'fs declaration examples are missing');

            // the first fs example reads a test.txt that the block does not
            // create (a known red-line defect of the corpus), so the smoke
            // asserts the self-contained write/read example and falls back to
            // "at least one fs declaration example passes"
            var selected = fs.filter(function (b) { return /fs\.writeFile\(/.test(b.code); }).slice(0, 1);
            var res = await examples.runBlocks(selected.length ? selected : fs.slice(0, 2),
                { fibjs: FIBJS, timeout: 10000, concurrency: 2 });
            assert.equal(res.results.length, selected.length ? 1 : Math.min(2, fs.length));
            assert.ok(res.results.some(function (r) { return r.status === 'ok'; }),
                'no fs declaration example passed: ' + JSON.stringify(res.results.map(function (r) {
                    return r.owner + ' [' + r.status + ']: ' + r.error;
                })));
        });
    });

    // the default scope: only the runnable blocks of the definitions checked
    // off in plans/idl-doc-todos.md; nothing is checked yet, so this is skipped
    // (and the suite stays fast) until the doc batches start landing
    var scoped = process.env.IDL_EXAMPLES_SCOPE === 'all' ? null : examples.checkedDefinitions();
    it('runs the runnable examples of the checked definitions',
        { skip: !parser || (scoped !== null && !scoped.length) }, async () => {
            var all = examples.extractBlocks(realDefs());
            var blocks = scoped === null ? all : examples.selectBlocks(all, { definitions: scoped });
            blocks = blocks.filter(function (b) { return b.class === 'runnable'; });

            var limit = parseInt(process.env.IDL_EXAMPLES_LIMIT || '0', 10);
            if (limit > 0) blocks = blocks.slice(0, limit);
            if (!blocks.length) return;

            var res = await examples.runBlocks(blocks, { fibjs: FIBJS, timeout: 10000, concurrency: 4 });
            var failures = res.results.filter(function (r) { return r.status === 'fail' || r.status === 'timeout'; });
            assert.equal(failures.length, 0, JSON.stringify(failures.map(function (r) {
                return r.owner + ' [' + r.status + ']: ' + r.error;
            })));
        });
});
