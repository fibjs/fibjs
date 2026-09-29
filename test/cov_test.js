// --cov / FIBJS_COV CLI regression tests.
//
// Spawns the fibjs binary in a scratch directory and checks the switch itself:
// the default log name (fibjs-<local date>-<time>-<pid>.lcov), the FIBJS_COV
// environment variable, its precedence against --cov and the glob merge of
// --cov-process. fibjs only (node has no --cov), hence the skip on node.

var { describe, it, after } = require('node:test');
var assert = require('assert');
var path = require('path');
var fs = require('fs');
var os = require('os');
var child_process = require('child_process');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

const LCOV_RE = /^fibjs-\d{8}-\d{6}-\d+\.lcov$/;
// fibjs console colours stderr, even into a pipe
const ANSI_RE = /\u001b\[[0-9;]*m/g;

// Two branches, one per run, so the difference between "merged" and "the last
// log wins" is visible in the report.
const DEMO = [
    "var mode = process.argv[2];",
    "",
    "function alpha() {",
    "    return 'alpha';",
    "}",
    "",
    "function beta() {",
    "    return 'beta';",
    "}",
    "",
    "if (mode === 'a')",
    "    console.log(alpha());",
    "else",
    "    console.log(beta());",
    ""
].join('\n');

describe('coverage CLI', { skip: !isFibjs }, () => {
    var scratch;
    var seq = 0;

    // one working directory per case: the logs of a case never leak into the
    // glob of another one
    function dir() {
        if (scratch === undefined)
            scratch = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-cov-'));

        var d = path.join(scratch, 'case' + (seq++));
        fs.mkdirSync(d);
        fs.writeFileSync(path.join(d, 'demo.js'), DEMO);

        return d;
    }

    function run(cwd, args, cov) {
        var env = Object.assign({}, process.env);

        // the value under test must never be inherited from the outer process
        delete env.FIBJS_COV;

        if (cov !== undefined)
            env.FIBJS_COV = cov;

        var r = child_process.spawnSync(process.execPath, args, {
            encoding: 'utf8',
            cwd: cwd,
            env: env,
            input: ''
        });

        return {
            code: r.status,
            stdout: r.stdout || '',
            stderr: (r.stderr || '').replace(ANSI_RE, '')
        };
    }

    function lcovs(cwd) {
        return fs.readdirSync(cwd).filter(f => LCOV_RE.test(f)).sort();
    }

    function readJson(file) {
        return JSON.parse(fs.readFileSync(file, 'utf8'));
    }

    after(() => {
        if (scratch !== undefined)
            fs.rmSync(scratch, { recursive: true, force: true });
    });

    it('--cov writes fibjs-<local date>-<time>-<pid>.lcov', () => {
        var cwd = dir();
        var r = run(cwd, ['--cov', 'demo.js', 'a']);

        assert.equal(r.code, 0, r.stderr);

        var files = lcovs(cwd);
        assert.equal(files.length, 1, files.join(', '));

        var content = fs.readFileSync(path.join(cwd, files[0]), 'utf8');
        assert.ok(/^SF:.*demo\.js$/m.test(content), content.slice(0, 200));

        // local time stamp: a name written now must be close to the test clock
        var m = /^fibjs-(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})(\d{2})-\d+\.lcov$/.exec(files[0]);
        var stamped = new Date(+m[1], +m[2] - 1, +m[3], +m[4], +m[5], +m[6]).getTime();
        assert.ok(Math.abs(Date.now() - stamped) < 120000, files[0]);
    });

    it('FIBJS_COV=1 selects the default name and stamps the pid', () => {
        var cwd = dir();
        var r = run(cwd, ['-e', 'console.log(process.pid)'], '1');

        assert.equal(r.code, 0, r.stderr);

        var files = lcovs(cwd);
        assert.equal(files.length, 1, files.join(', '));
        assert.ok(files[0].indexOf('-' + r.stdout.trim() + '.lcov') > 0,
            `${files[0]} does not carry the pid ${r.stdout.trim()}`);
    });

    it('FIBJS_COV=<path> creates the directories and falsy values stay off', () => {
        var cwd = dir();

        // the directory part is created on demand, like the output directory of
        // --cov-process
        assert.equal(run(cwd, ['-e', '0'], 'logs/nested/custom.lcov').code, 0);
        assert.deepEqual(fs.readdirSync(path.join(cwd, 'logs')), ['nested']);
        assert.deepEqual(fs.readdirSync(path.join(cwd, 'logs', 'nested')), ['custom.lcov']);

        // a path that cannot be created is reported, not silently dropped
        var r = run(cwd, ['--cov=demo.js/x.lcov', '-e', '0']);
        assert.equal(r.code, 1, r.stdout);
        assert.ok(r.stderr.indexOf('Cannot open coverage file') >= 0, r.stderr);

        ['0', 'false', 'no', 'off', ''].forEach(value => {
            assert.equal(run(cwd, ['-e', '0'], value).code, 0, value);
        });
        assert.deepEqual(lcovs(cwd), [], 'a falsy FIBJS_COV must not write a log');
    });

    it('--cov wins over FIBJS_COV', () => {
        var cwd = dir();
        var r = run(cwd, ['--cov=explicit.lcov', '-e', '0'], '1');

        assert.equal(r.code, 0, r.stderr);
        assert.deepEqual(fs.readdirSync(cwd).sort(), ['demo.js', 'explicit.lcov']);
    });

    it('writes an lcov record that describes the file and nothing else', () => {
        var cwd = dir();
        var r = run(cwd, ['--cov=out.lcov', 'demo.js', 'a']);

        assert.equal(r.code, 0, r.stderr);

        var lcov = fs.readFileSync(path.join(cwd, 'out.lcov'), 'utf8');
        var demo = fs.readFileSync(path.join(cwd, 'demo.js'), 'utf8').split('\n');
        var fileLines = demo[demo.length - 1] === '' ? demo.length - 1 : demo.length;

        // the file is named relative to the working directory
        assert.ok(lcov.indexOf('SF:demo.js\n') >= 0, lcov);

        // the wrapper fibjs compiles the module with is not a function of the
        // file, and line 0 does not exist
        assert.equal(lcov.indexOf('FN:0,'), -1, lcov);
        assert.ok(/^FN:\d+,/m.test(lcov), lcov);

        // the summary lines are there, and they agree with the record
        ['FNF:', 'FNH:', 'BRDA:', 'BRF:', 'BRH:', 'LH:', 'LF:'].forEach(k => {
            assert.ok(lcov.indexOf(k) >= 0, k + ' is missing:\n' + lcov);
        });

        var das = [];
        (lcov.match(/^DA:\d+,\d+$/gm) || []).forEach(line => {
            var m = /^DA:(\d+),(\d+)$/.exec(line);

            das.push(Number(m[1]));
            assert.ok(Number(m[1]) <= fileLines, `line ${m[1]} is past the file (${fileLines}):\n` + lcov);
        });

        assert.equal(das.length, fileLines, 'one DA line per line of the file:\n' + lcov);
        assert.equal(Number(/^LF:(\d+)$/m.exec(lcov)[1]), fileLines, lcov);

        var lh = Number(/^LH:(\d+)$/m.exec(lcov)[1]);
        assert.ok(lh > 0 && lh < fileLines, `LH must sit between 0 and ${fileLines}: ${lh}\n` + lcov);

        (lcov.match(/^BRDA:\d+,/gm) || []).forEach(line => {
            assert.ok(Number(/^BRDA:(\d+),/.exec(line)[1]) <= fileLines, lcov);
        });
    });

    it('leaves the tests of a --test run out of the record', () => {
        var cwd = dir();

        // an aggregate entry that imports a test module: neither the entry nor
        // the imported module is what the report is about, the code they run is
        fs.writeFileSync(path.join(cwd, 'impl.js'),
            "module.exports = {\n    half: function (n) {\n        return n / 2;\n    }\n};\n");
        fs.writeFileSync(path.join(cwd, 'inner.test.js'),
            "var { test } = require('node:test');\n" +
                "var { half } = require('./impl.js');\n" +
                "test('inner', () => { require('node:assert').equal(half(4), 2); });\n");
        fs.writeFileSync(path.join(cwd, 'all.test.js'), "require('./inner.test.js');\n");

        var r = run(cwd, ['--cov=all.lcov', '--test', 'all.test.js']);

        assert.equal(r.code, 0, r.stderr);

        var lcov = fs.readFileSync(path.join(cwd, 'all.lcov'), 'utf8');

        assert.ok(lcov.indexOf('SF:impl.js\n') >= 0, lcov);
        assert.equal(lcov.indexOf('SF:inner.test.js'), -1,
            'an imported test module must not be counted:\n' + lcov);
        assert.equal(lcov.indexOf('SF:all.test.js'), -1,
            'the entry of the run must not be counted:\n' + lcov);
    });

    it('--cov-process merges a glob of logs', () => {
        var cwd = dir();

        assert.equal(run(cwd, ['--cov=a.lcov', 'demo.js', 'a']).code, 0);
        assert.equal(run(cwd, ['--cov=b.lcov', 'demo.js', 'b']).code, 0);

        // one log alone leaves the branch of the other run uncovered
        assert.equal(run(cwd, ['--cov-process', 'a.lcov', 'out_one']).code, 0);
        var one = readJson(path.join(cwd, 'out_one', 'coverage.json'));
        assert.equal(one.length, 1, JSON.stringify(one));
        assert.ok(one[0].uncovered_lines.length > 0, JSON.stringify(one));

        // the glob report is the union of both runs: nothing left uncovered
        var both = run(cwd, ['--cov-process', '*.lcov', 'out_both']);
        assert.equal(both.code, 0, both.stderr);
        assert.deepEqual(readJson(path.join(cwd, 'out_both', 'coverage.json')), []);
    });

    it('--cov-process rejects an input it cannot match', () => {
        var cwd = dir();

        [
            ['--cov-process'],
            ['--cov-process', 'nomatch-*.lcov', 'out'],
            ['--cov-process', 'missing.lcov', 'out']
        ].forEach(args => {
            var label = args.join(' ');
            var r = run(cwd, args);

            assert.equal(r.code, 1, `${label}: expected exit code 1`);
            assert.equal(r.stdout, '', `${label}: an error must not write to stdout`);
            assert.ok(r.stderr.indexOf('fibjs --cov-process:') === 0, r.stderr);
            assert.ok(r.stderr.indexOf('Usage: fibjs --cov-process') > 0, r.stderr);
        });
    });
});
