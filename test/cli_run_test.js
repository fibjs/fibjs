// fibjs CLI command runner regression tests — see
// plans/npm-cli-alignment-2026-10-10.md (items A1/A3/A5/A10/A12/A13).
//
//   A1  package.json and node_modules/.bin are searched walking up from the
//       working directory; a package script runs from the root of its package
//       and sees the directory the user typed in as INIT_CWD
//   A3  extra arguments reach the script verbatim (no re-splitting, no
//       shell injection)
//   A5  a script killed by a signal is reported as a signal death (128 + N),
//       and a signal sent to fibjs is forwarded to the script it runs
//   A10 only a real node/fibjs shebang makes a node_modules/.bin entry run on
//       fibjs itself
//   A12 -p/--print and --eval, plus FIBJS_NO_NODE_REWRITE to keep the real node
//   A13 bad options are reported instead of silently ignored
//
// Everything is spawned through process.execPath, so the suite is fibjs only
// (under node the same arguments would run node itself).

var { describe, it, before, after } = require('node:test');
var assert = require('assert');
var path = require('path');
var fs = require('fs');
var os = require('os');
var child_process = require('child_process');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;
const isWindows = os.platform() === 'win32';

let root;      // <tmp>/proj
let sub;       // <tmp>/proj/sub/deep

function write(file, content, mode) {
    fs.mkdirSync(path.dirname(file), { recursive: true });
    fs.writeFileSync(file, content);
    if (mode && !isWindows)
        fs.chmodSync(file, mode);
}

function run(args, opts) {
    opts = opts || {};
    var r = child_process.spawnSync(process.execPath, args, {
        encoding: 'utf8',
        cwd: opts.cwd || root,
        env: Object.assign({}, process.env, opts.env || {}),
        input: '',
        timeout: opts.timeout || 30000
    });

    return { code: r.status, signal: r.signal, stdout: r.stdout || '', stderr: r.stderr || '' };
}

// The path macOS reports for /tmp after resolving the symlink.
function realDir(p) {
    return fs.realpathSync(p);
}

before(() => {
    root = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-cli-run-'));
    sub = path.join(root, 'sub', 'deep');
    fs.mkdirSync(sub, { recursive: true });

    var pkg = {
        name: 'cli-run-fixture',
        version: '1.0.0',
        scripts: {
            hi: 'echo HI',
            args: 'node argdump.js',
            wd: 'node cwd.js',
            envp: 'node envdump.js',
            exit3: 'node exit3.js',
            long: 'node long.js',
            execp: 'node exec_path.js',
            devtool: 'tool'
        }
    };

    write(path.join(root, 'package.json'), JSON.stringify(pkg, null, 2));

    write(path.join(root, 'argdump.js'), 'console.log(JSON.stringify(process.argv.slice(2)));\n');
    write(path.join(root, 'cwd.js'), 'console.log(process.cwd());\n');
    write(path.join(root, 'envdump.js'),
        'console.log("INIT_CWD=" + (process.env.INIT_CWD || "<unset>"));\n' +
        'console.log("CWD=" + process.cwd());\n');
    write(path.join(root, 'exit3.js'), 'process.exit(3);\n');
    write(path.join(root, 'exec_path.js'), 'console.log(process.execPath);\n');
    write(path.join(root, 'long.js'), 'console.log("START"); setInterval(() => {}, 1000);\n');

    // node_modules/.bin entries: the shebang decides who runs them
    var bin = path.join(root, 'node_modules', '.bin');
    write(path.join(bin, 'hello'),
        '#!/usr/bin/env node\nconsole.log("BIN", JSON.stringify(process.argv.slice(2)));\n', 0o755);
    write(path.join(bin, 'envsplit'),
        '#!/usr/bin/env -S node --stack-size=500\nconsole.log("SPLIT-OK");\n', 0o755);
    write(path.join(bin, 'nodecap'),
        '#!/usr/bin/env Node\nconsole.log("CAP-OK");\n', 0o755);
    // not a node interpreter: has to go through the shell
    write(path.join(bin, 'envmy'),
        '#!/usr/bin/env my-node-like\nconsole.log("MY-RAN");\n', 0o755);
    write(path.join(bin, 'pycomment'),
        '#!/usr/bin/python3 # node\nconsole.log("PY-RAN");\n', 0o755);
    write(path.join(bin, 'sheet'),
        '#!/bin/sh\necho SHELL-RAN\n', 0o755);
    write(path.join(bin, 'tool'),
        '#!/bin/sh\necho TOOL-RAN\n', 0o755);
});

after(() => {
    try {
        fs.rmSync(root, { recursive: true, force: true });
    } catch (e) {
        // best effort
    }
});

describe('cli command runner', { skip: !isFibjs }, () => {

    describe('A1: package scripts and .bin are found walking up', () => {
        it('runs a package script from a subdirectory', () => {
            var r = run(['hi'], { cwd: sub });
            assert.equal(r.code, 0, r.stderr);
            assert.match(r.stdout, /HI/);
        });

        it('runs the script from the root of its package', () => {
            var r = run(['wd'], { cwd: sub });
            assert.equal(r.code, 0, r.stderr);
            assert.equal(r.stdout.trim(), realDir(root));
        });

        it('keeps the directory it was started in as INIT_CWD', () => {
            var r = run(['envp'], { cwd: sub });
            assert.equal(r.code, 0, r.stderr);
            assert.match(r.stdout, new RegExp('INIT_CWD=' + realDir(sub).replace(/[.*+?^${}()|[\]\\]/g, '\\$&')));
            assert.match(r.stdout, new RegExp('CWD=' + realDir(root).replace(/[.*+?^${}()|[\]\\]/g, '\\$&')));
        });

        it('finds a node_modules/.bin entry of an ancestor directory', () => {
            var r = run(['hello', 'A', 'b c'], { cwd: sub });
           

        it('puts the .bin of the package on PATH for its scripts', { skip: isWindows }, () => {
            var r = run(['devtool'], { cwd: sub });
            assert.equal(r.code, 0, r.stderr);
            assert.match(r.stdout, /TOOL-RAN/);
        }); assert.equal(r.code, 0, r.stderr);
            assert.equal(r.stdout.trim(), 'BIN ["A","b c"]');
        });

        it('still reports an unknown name', () => {
            var r = run(['definitely-not-a-script'], { cwd: sub });
            assert.notEqual(r.code, 0);
            assert.match(r.stderr, /ENOENT/);
        });
    });

    describe('A3: extra arguments are passed verbatim', () => {
        it('keeps blanks, quotes and shell metacharacters intact', () => {
            var args = ['a b', "q'q", 'q"q', 'c;d', '$HOME', '*', '&&'];
            var r = run(['args'].concat(args));
            assert.equal(r.code, 0, r.stderr);
            assert.deepEqual(JSON.parse(r.stdout.trim()), args);
            assert.ok(!/command not found/.test(r.stderr), r.stderr);
        });
    });

    describe('A5: exit status of the script', () => {
        // Signals are covered by child_process_test.js ("command runner and
        // signals"): the wrapper dies by the same signal as its script and
        // forwards SIGTERM to it.
        it('passes a normal exit code through', () => {
            var r = run(['exit3']);
            assert.equal(r.code, 3);
        });
    });

    describe('A10: only a node/fibjs shebang runs in-process', () => {
        it('runs a node shebang entry on fibjs itself', () => {
            var r = run(['hello']);
            assert.equal(r.code, 0, r.stderr);
            assert.equal(r.stdout.trim(), 'BIN []');
        });

        it('understands env -S and is case insensitive', () => {
            assert.equal(run(['envsplit']).stdout.trim(), 'SPLIT-OK');
            assert.equal(run(['nodecap']).stdout.trim(), 'CAP-OK');
        });

        it('does not treat a lookalike interpreter as node', { skip: isWindows }, () => {
            var r = run(['envmy']);
            assert.ok(!/MY-RAN/.test(r.stdout), 'a `my-node-like` shebang must not run on fibjs: ' + r.stdout);
        });

        it('does not treat a "node" word in the arguments as the interpreter', { skip: isWindows }, () => {
            var r = run(['pycomment']);
            assert.ok(!/PY-RAN/.test(r.stdout), 'a `python3 # node` shebang must not run on fibjs: ' + r.stdout);
        });

        it('still hands a shell script to the shell', { skip: isWindows }, () => {
            var r = run(['sheet']);
            assert.equal(r.code, 0, r.stderr);
            assert.match(r.stdout, /SHELL-RAN/);
        });
    });

    describe('A12: -p/--print, --eval and the node rewrite switch', () => {
        it('prints the value of -p/--print', () => {
            assert.equal(run(['-p', '1+1']).stdout.trim(), '2');
            assert.equal(run(['--print', '"a"+"b"']).stdout.trim(), 'ab');
            assert.equal(run(['-p', 'undefined']).stdout.trim(), 'undefined');
        });

        it('keeps -e silent and accepts --eval', () => {
            assert.equal(run(['-e', '1+1']).stdout, '');
            assert.match(run(['--eval', 'console.log("EVAL-OK")']).stdout, /EVAL-OK/);
        });

        it('runs `node` from a script on fibjs by default', () => {
            var r = run(['execp']);
            assert.equal(r.code, 0, r.stderr);
            assert.equal(r.stdout.trim(), process.execPath);
        });

        it('FIBJS_NO_NODE_REWRITE=1 keeps the real node', () => {
            var node = child_process.spawnSync('node', ['--version'], { encoding: 'utf8' });
            if (node.status !== 0) {
                return; // no other node installed: nothing to compare against
            }

            var on = run(['execp']);
            var off = run(['execp'], { env: { FIBJS_NO_NODE_REWRITE: '1' } });

            assert.equal(on.code, 0, on.stderr);
            assert.equal(off.code, 0, off.stderr);
            assert.equal(on.stdout.trim(), process.execPath);
            assert.notEqual(off.stdout.trim(), process.execPath);
            assert.match(off.stdout.trim(), /node/i);
        });
    });

    describe('A13: option errors', () => {
        it('reports an unknown option', () => {
            var r = run(['--no-such-tool']);
            assert.equal(r.code, 1);
            assert.match(r.stderr, /bad option: --no-such-tool/);
        });

        it('reports a stray option after -e', () => {
            var r = run(['-e', '1+1', '--x']);
            assert.equal(r.code, 1);
            assert.match(r.stderr, /bad option: --x/);
        });

        it('reports a missing argument', () => {
            var r = run(['-e']);
            assert.equal(r.code, 1);
            assert.match(r.stderr, /-e requires an argument/);
        });

        it('lists the new options in the help', () => {
            var r = run(['--help']);
            assert.equal(r.code, 0);
            assert.match(r.stdout, /-p, --print code/);
            assert.match(r.stdout, /-e, --eval code/);
        });
    });
});
