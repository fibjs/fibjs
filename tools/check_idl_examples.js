/**
 * @description runner for the runnable example blocks of the IDL corpus (B0-02).
 *
 * `idl/*.idl` is the only source of the generated manual, so every
 * ```JavaScript block of a definition or a member is treated as an example and
 * the convention of plans/idl-doc-completion-plan-2026-10-05.md section 5.5 is
 * enforced here:
 *
 *   - an unmarked block runs as a complete program in its own temp directory
 *     (cwd = that directory, `fibjs <file>`, default timeout 10s, exit 0 = ok);
 *   - `// fragment: <reason>` is a partial snippet: it is only checked for
 *     JavaScript syntax (a top-level await is retried inside an async IIFE);
 *   - `// requires: <service>` needs an environment: the block is skipped
 *     unless the service is on the --requires list. The whitelist is
 *     redis/mysql/sqlite/network/windows/long-running; a `long-running` block
 *     is accepted when it is still alive after the survival window.
 *
 * The report written by --json (temp/idl_examples_report.json) is consumed by
 * rule X8 of tools/util/check_idl_docs.js: when the report exists, every entry
 * whose status is not ok fails the documentation check. Skipped entries carry
 * `skipped: true` because X8 does not know the `skip` vocabulary, and the
 * block classification lives in `class` (never `kind`/`category`/`mode`,
 * which X8 treats as a skip flag).
 *
 * CLI:
 *   node tools/check_idl_examples.js [--files idl/fs.idl,...] [--filter <name>]
 *        [--batch Bn] [--requires redis,...] [--timeout 10000]
 *        [--concurrency 4] [--list] [--json] [--report <path>]
 *
 *   --files     only the definitions of the given idl files
 *   --filter    only the blocks of a definition (or of a definition.member)
 *   --batch     the definitions of a batch in plans/idl-doc-todos.md
 *   --requires  services to execute instead of skipping (csv)
 *   --list      print the blocks and their classification, run nothing
 *   --json      write the report to temp/idl_examples_report.json (or --report)
 *
 * The examples are executed by a fibjs binary: the one hosting the tool when
 * it runs under fibjs, else bin/Linux_x64_release/fibjs (FIBJS overrides).
 *
 * The exit code is 1 when a runnable example fails or times out, when a
 * fragment is not valid JavaScript, or when a marker is malformed.
 */

'use strict';

var fs = require('fs');
var path = require('path');
var os = require('os');
var vm = require('vm');
var cp = require('child_process');

var REPO_ROOT = path.resolve(__dirname, '..');
var IDL_FOLDER = path.join(REPO_ROOT, 'idl');
var TODOS_FILE = path.join(REPO_ROOT, 'plans/idl-doc-todos.md');
var DEFAULT_REPORT = path.join(REPO_ROOT, 'temp/idl_examples_report.json');

// the examples must run under a real fibjs: when the tool itself runs under
// fibjs (`fibjs tools/check_idl_examples.js`) that is the binary hosting the
// tool; a node run (`node tools/check_idl_examples.js`, the CI host) keeps the
// conventional build location. FIBJS overrides both.
var DEFAULT_FIBJS = process.versions.fibjs ?
    process.execPath :
    path.join(REPO_ROOT, 'bin/Linux_x64_release/fibjs');

var SERVICES = ['redis', 'mysql', 'sqlite', 'network', 'windows', 'long-running'];
var DEFAULT_TIMEOUT = 10000;      // ms, per runnable example
var DEFAULT_CONCURRENCY = 4;
var LONG_RUNNING_MS = 5000;       // a `long-running` block must survive this long
var OUTPUT_CAP = 8192;            // captured stdout/stderr per example
var CODE_CAP_IN_REPORT = 500;     // failed blocks are quoted in the report

var CODE_BLOCK_RE = /```[Jj]ava[Ss]cript[^\n]*\n([\s\S]*?)```/g;

/* ------------------------------- extraction ------------------------------ */

function firstLine(value) {
    return String(value === undefined || value === null ? '' : value).split('\n')[0];
}

// the first line of stderr is often the source location (fibjs prints
// `<file>:<line>:<col>` and a trailing `Error: <location>`), so prefer the last
// line that names the error itself; fall back to the first non-empty line
function errorLine(stderr) {
    var lines = String(stderr || '').split('\n');
    var first = '';
    var named = '';
    var ERROR_RE = /^(?:[A-Z][A-Za-z]*Error|Error)\s*:/;
    for (var i = 0; i < lines.length; i++) {
        var line = lines[i].trim();
        if (!line) continue;
        if (!first) first = line;
        if (ERROR_RE.test(line)) named = line;
    }
    return named || first;
}

// the extraction formula of temp/idl_audit.js and tools/util/check_idl_docs.js:
// the brief line plus the detail lines, only ```JavaScript blocks
function codeBlocks(doc) {
    var all = ((doc && doc.descript) || '') + '\n' + (((doc && doc.detail) || []).join('\n'));
    var out = [];
    var m;
    CODE_BLOCK_RE.lastIndex = 0;
    while ((m = CODE_BLOCK_RE.exec(all)) !== null)
        out.push(m[1]);
    return out;
}

// the first non-empty line carries the marker; anything that starts with
// `// fragment` / `// requires` but is not `// <name>: <value>` is malformed
function classifyBlock(code) {
    var lines = String(code).split('\n');
    var marker = '';
    for (var i = 0; i < lines.length; i++) {
        if (lines[i].trim() !== '') {
            marker = lines[i].trim();
            break;
        }
    }

    var m = /^\/\/\s*(fragment|requires)\s*:\s*(.*)$/.exec(marker);
    if (m)
        return { class: m[1], value: m[2].trim(), marker: marker };
    if (/^\/\/\s*(fragment|requires)\b/.test(marker))
        return { class: 'malformed', value: marker, marker: marker };
    return { class: 'runnable', value: null, marker: null };
}

// vm.Script parses a classic script; a top-level await (or return) is retried
// inside an async IIFE, the form fibjs executes the example in
function fragmentSyntaxError(code) {
    try {
        new vm.Script(code);
        return null;
    } catch (e) { /* retried below */ }

    try {
        new vm.Script('(async()=>{\n' + code + '\n})();');
        return null;
    } catch (e) {
        return firstLine((e && e.message) || e);
    }
}

// every example block of every definition, owner = `def` or `def.member`;
// parser output keeps each overload as its own member (there is no `overs`
// field), so the same owner may repeat and `index` counts within the doc
function extractBlocks(defs) {
    var blocks = [];

    Object.keys(defs || {}).sort().forEach(function (name) {
        var def = defs[name];

        function add(doc, member, where) {
            codeBlocks(doc).forEach(function (code, index) {
                var c = classifyBlock(code);
                blocks.push({
                    seq: blocks.length,
                    def: name,
                    member: member,
                    owner: name + (member ? '.' + member : ''),
                    where: where,
                    index: index,
                    marker: c.marker,
                    class: c.class,
                    value: c.value,
                    code: code
                });
            });
        }

        add(def.declare.doc, null, 'declare');
        (def.members || []).forEach(function (m) { add(m.doc, m.name, 'member'); });
    });

    return blocks;
}

/* -------------------------------- selection ------------------------------ */

// a definition (or definition.member) name from `--filter`; the block matches
// when its definition is the filter, its owner is the filter, or its owner
// starts with `<filter>.`
function filterMatch(block, filter) {
    if (!filter) return true;
    return block.def === filter || block.owner === filter ||
        block.owner.indexOf(filter + '.') === 0;
}

function selectBlocks(blocks, opts) {
    opts = opts || {};
    var names = null;
    if (opts.definitions) {
        names = {};
        opts.definitions.forEach(function (n) { names[n] = true; });
    }

    return blocks.filter(function (b) {
        if (names && !names[b.def]) return false;
        return filterMatch(b, opts.filter);
    });
}

// `--files idl/fs.idl,...`: the definition name of an idl file (a file holds
// exactly one top-level declaration)
function defNameFromSource(src) {
    var code = String(src)
        .replace(/\/\*[\s\S]*?\*\//g, ' ')
        .replace(/\/\/[^\n]*/g, ' ');
    var m = /\b(?:module|interface)\s+([A-Za-z_$][\w$]*)/.exec(code);
    return m ? m[1] : null;
}

function definitionsFromFiles(defs, csv) {
    var names = {};
    String(csv).split(',').forEach(function (f) {
        f = f.trim();
        if (!f) return;
        var abs = path.isAbsolute(f) ? f : path.join(REPO_ROOT, f);
        if (!fs.existsSync(abs))
            throw new Error('file not found: ' + f);
        var name = defNameFromSource(fs.readFileSync(abs, 'utf8'));
        if (!name || !defs[name])
            throw new Error('cannot map ' + f + ' to a parsed IDL definition');
        names[name] = true;
    });
    var out = Object.keys(names).sort();
    if (!out.length)
        throw new Error('no definition selected from --files');
    return out;
}

// the checked-off definitions of plans/idl-doc-todos.md, the default scope of
// test/idl_examples_test.js; the file is the single source of progress
function checkedDefinitions(todosFile) {
    var text;
    try {
        text = fs.readFileSync(todosFile || TODOS_FILE, 'utf8');
    } catch (e) {
        return [];
    }

    var names = {};
    text.split('\n').forEach(function (line) {
        var m = /^- \[[xX]\] (B\d+)-\d+ `([^`]+)`/.exec(line.trim());
        if (m) names[m[2]] = true;
    });
    return Object.keys(names).sort();
}

// `--batch B1`: the definitions of that batch, checked or not
function batchDefinitions(batch, todosFile) {
    var text;
    try {
        text = fs.readFileSync(todosFile || TODOS_FILE, 'utf8');
    } catch (e) {
        throw new Error('cannot read ' + (todosFile || TODOS_FILE) + ': ' + e.message);
    }

    var names = {};
    text.split('\n').forEach(function (line) {
        var m = /^- \[[ xX]\] (B\d+)-\d+ `([^`]+)`/.exec(line.trim());
        if (m && m[1] === batch) names[m[2]] = true;
    });
    var out = Object.keys(names).sort();
    if (!out.length)
        throw new Error('no definition found for batch ' + batch + ' in ' + (todosFile || TODOS_FILE));
    return out;
}

function parseRequires(csv) {
    if (!csv) return [];
    var out = String(csv).split(',').map(function (s) { return s.trim(); }).filter(Boolean);
    out.forEach(function (s) {
        if (SERVICES.indexOf(s) < 0)
            throw new Error('unknown --requires service `' + s + '` (expected one of ' + SERVICES.join('/') + ')');
    });
    return out;
}

/* ------------------------------- execution ------------------------------- */

function normalizeRunOptions(opts) {
    opts = opts || {};
    var out = {
        fibjs: opts.fibjs || process.env.FIBJS || DEFAULT_FIBJS,
        timeout: parseInt(opts.timeout, 10) > 0 ? parseInt(opts.timeout, 10) : DEFAULT_TIMEOUT,
        concurrency: parseInt(opts.concurrency, 10) > 0 ? parseInt(opts.concurrency, 10) : DEFAULT_CONCURRENCY,
        requires: opts.requires || []
    };
    out.longRunningMs = parseInt(opts.longRunningMs, 10) > 0 ? parseInt(opts.longRunningMs, 10) :
        Math.min(LONG_RUNNING_MS, out.timeout);
    return out;
}

function appendChunk(buffer, data) {
    if (buffer.length >= OUTPUT_CAP) return buffer;
    return buffer + String(data).slice(0, OUTPUT_CAP - buffer.length);
}

function makeTempDir() {
    var root = path.join(os.tmpdir(), 'fibjs-idl-examples');
    fs.mkdirSync(root, { recursive: true });
    return fs.mkdtempSync(path.join(root, 'case-'));
}

function removeTempDir(dir) {
    try {
        fs.rmSync(dir, { recursive: true, force: true });
    } catch (e) { /* a busy temp dir must not fail the run */ }
}

// spawn the example file in its own directory; `long-running` resolves ok when
// the process is still alive after the survival window and is then killed
function executeFile(file, dir, opts, longRunning) {
    return new Promise(function (resolve) {
        var started = Date.now();
        var child;
        try {
            child = cp.spawn(opts.fibjs, [file], {
                cwd: dir,
                stdio: ['ignore', 'pipe', 'pipe']
            });
        } catch (e) {
            resolve({
                status: 'fail', exit: null,
                error: 'spawn failed: ' + firstLine((e && e.message) || e),
                stdout: '', stderr: ''
            });
            return;
        }

        var stdout = '';
        var stderr = '';
        var settled = false;
        var timedOut = false;
        var survived = false;
        var timer = null;

        if (child.stdout) child.stdout.on('data', function (d) { stdout = appendChunk(stdout, d); });
        if (child.stderr) child.stderr.on('data', function (d) { stderr = appendChunk(stderr, d); });

        function finish(code, signal, spawnError) {
            if (settled) return;
            settled = true;
            if (timer) clearTimeout(timer);

            var status;
            var error = null;
            if (spawnError) {
                status = 'fail';
                error = 'spawn failed: ' + firstLine((spawnError && spawnError.message) || spawnError);
            } else if (survived) {
                status = 'ok';
            } else if (timedOut) {
                status = 'timeout';
                error = 'timed out after ' + opts.timeout + 'ms' +
                    (errorLine(stderr) ? ': ' + errorLine(stderr) : '');
            } else if (code === 0) {
                status = 'ok';
            } else {
                status = 'fail';
                error = errorLine(stderr) || (signal ? 'killed by ' + signal : 'exit code ' + code);
            }

            resolve({
                status: status,
                exit: code === undefined ? null : code,
                signal: signal || null,
                error: error,
                stdout: stdout,
                stderr: stderr,
                elapsed: Date.now() - started
            });
        }

        timer = setTimeout(function () {
            if (longRunning) survived = true;
            else timedOut = true;
            try {
                child.kill('SIGKILL');
            } catch (e) { /* the process may have exited meanwhile */ }
        }, longRunning ? opts.longRunningMs : opts.timeout);

        child.on('exit', function (code, signal) { finish(code, signal, null); });
        if (child.on) child.on('error', function (e) { finish(null, null, e); });
    });
}

function resultOf(block, status, extra) {
    var out = {
        owner: block.owner,
        def: block.def,
        member: block.member === undefined ? null : block.member,
        where: block.where || 'member',
        index: block.index || 0,
        marker: block.marker || null,
        class: block.class || classifyBlock(block.code).class,
        status: status,
        exit: null,
        error: null,
        problem: false,
        code: block.code
    };
    Object.keys(extra || {}).forEach(function (k) { out[k] = extra[k]; });
    return out;
}

// run one block: classify, validate the marker, syntax-check a fragment, skip
// a non-whitelisted require, or execute it
function runBlock(block, opts) {
    var o = normalizeRunOptions(opts);
    var c = classifyBlock(block.code);

    if (c.class === 'malformed')
        return Promise.resolve(resultOf(block, 'fail', {
            class: 'malformed',
            error: 'malformed example marker `' + firstLine(c.value) +
                '` (use `// fragment: <reason>` or `// requires: <service>`)',
            problem: true
        }));

    if (c.class === 'fragment') {
        if (!c.value)
            return Promise.resolve(resultOf(block, 'fail', {
                class: 'fragment',
                error: 'fragment marker needs a non-empty reason (`// fragment: <reason>`)',
                problem: true
            }));
        var syntaxError = fragmentSyntaxError(block.code);
        if (syntaxError)
            return Promise.resolve(resultOf(block, 'fail', {
                class: 'fragment',
                error: 'fragment is not valid JavaScript: ' + syntaxError,
                problem: true
            }));
        return Promise.resolve(resultOf(block, 'skip', { class: 'fragment' }));
    }

    if (c.class === 'requires') {
        if (!c.value)
            return Promise.resolve(resultOf(block, 'fail', {
                class: 'requires',
                error: 'requires marker needs a service (`// requires: <service>`)',
                problem: true
            }));
        if (SERVICES.indexOf(c.value) < 0)
            return Promise.resolve(resultOf(block, 'fail', {
                class: 'requires',
                error: 'requires service `' + c.value + '` is not one of ' + SERVICES.join('/'),
                problem: true
            }));
        if (o.requires.indexOf(c.value) < 0)
            return Promise.resolve(resultOf(block, c.value === 'long-running' ? 'skip-long' : 'skip', {
                class: 'requires',
                service: c.value
            }));
    }

    if (!fs.existsSync(o.fibjs))
        return Promise.resolve(resultOf(block, 'fail', {
            error: 'fibjs binary not found: ' + o.fibjs + ' (set FIBJS to a build of fibjs)',
            problem: true
        }));

    var dir;
    try {
        dir = makeTempDir();
        fs.writeFileSync(path.join(dir, 'main.js'), block.code);
    } catch (e) {
        if (dir) removeTempDir(dir);
        return Promise.resolve(resultOf(block, 'fail', {
            error: 'cannot prepare the temp example: ' + firstLine((e && e.message) || e),
            problem: true
        }));
    }

    var longRunning = c.class === 'requires' && c.value === 'long-running';
    return executeFile(path.join(dir, 'main.js'), dir, o, longRunning).then(function (r) {
        removeTempDir(dir);
        return resultOf(block, r.status, {
            class: c.class,
            exit: r.exit,
            signal: r.signal,
            error: r.error,
            stdout: r.stdout,
            stderr: r.stderr,
            elapsed: r.elapsed,
            service: c.class === 'requires' ? c.value : null
        });
    });
}

// a small promise pool; the results keep the input order
function runBlocks(blocks, opts) {
    var o = normalizeRunOptions(opts);
    var started = Date.now();

    return new Promise(function (resolve) {
        var results = new Array(blocks.length);
        var next = 0;

        if (!blocks.length) {
            resolve({ results: [], summary: summarize([]), elapsed: 0 });
            return;
        }

        var active = 0;
        var done = 0;

        function pump() {
            while (active < o.concurrency && next < blocks.length) {
                (function (i) {
                    active++;
                    var b = blocks[i];
                    var p;
                    try {
                        p = runBlock(b, o);
                    } catch (e) {
                        p = Promise.resolve(resultOf(b, 'fail', {
                            error: 'runner error: ' + firstLine((e && e.message) || e),
                            problem: true
                        }));
                    }
                    p.then(function (r) {
                        results[i] = r;
                    }, function (e) {
                        results[i] = resultOf(b, 'fail', {
                            error: 'runner error: ' + firstLine((e && e.message) || e),
                            problem: true
                        });
                    }).then(function () {
                        active--;
                        done++;
                        if (done === blocks.length)
                            resolve({ results: results, summary: summarize(results), elapsed: Date.now() - started });
                        else
                            pump();
                    });
                })(next++);
            }
        }

        pump();
    });
}

function summarize(results) {
    var s = {
        total: results.length,
        runnable: 0, fragment: 0, requires: 0, malformed: 0,
        ok: 0, fail: 0, timeout: 0, skip: 0, skipLong: 0,
        problems: 0
    };
    results.forEach(function (r) {
        if (s[r.class] !== undefined) s[r.class]++;
        if (r.status === 'skip-long') s.skipLong++;
        else if (s[r.status] !== undefined) s[r.status]++;
        if (r.problem) s.problems++;
    });
    return s;
}

function isFailure(result) {
    return result.status === 'fail' || result.status === 'timeout';
}

/* --------------------------------- report -------------------------------- */

function buildReport(results, summary, opts) {
    var entries = results.map(function (r) {
        var e = {
            owner: r.owner,
            def: r.def,
            member: r.member,
            where: r.where,
            index: r.index,
            marker: r.marker,
            class: r.class,
            status: r.status,
            skipped: r.status === 'skip' || r.status === 'skip-long',
            exit: r.exit === undefined ? null : r.exit,
            error: r.error || null,
            elapsed: r.elapsed || 0
        };
        if (r.service) e.service = r.service;
        if (isFailure(r) && r.code) e.code = String(r.code).slice(0, CODE_CAP_IN_REPORT);
        return e;
    });

    return {
        generated: new Date().toISOString(),
        fibjs: opts.fibjs,
        scope: {
            definitions: opts.definitions || null,
            filter: opts.filter || null,
            batch: opts.batch || null,
            requires: opts.requires || [],
            timeout: opts.timeout,
            concurrency: opts.concurrency
        },
        summary: summary,
        problems: results.filter(function (r) { return r.problem; }).map(function (r) {
            return { owner: r.owner, where: r.where, message: r.error };
        }),
        results: entries
    };
}

function writeReport(target, report) {
    fs.mkdirSync(path.dirname(target), { recursive: true });
    fs.writeFileSync(target, JSON.stringify(report, null, 2) + '\n');
}

/* ---------------------------------- CLI ---------------------------------- */

function usage(message) {
    if (message)
        console.error('[check_idl_examples] ' + message);
    console.error('usage: node tools/check_idl_examples.js [--files idl/fs.idl,...] [--filter <name>] ' +
        '[--batch Bn] [--requires redis,...] [--timeout 10000] [--concurrency 4] [--list] [--json] [--report <path>]');
    return 1;
}

function parseArgs(argv) {
    var opts = {
        files: null, filter: null, batch: null, requires: null,
        timeout: null, concurrency: null, list: false, json: false, report: null
    };

    for (var i = 0; i < argv.length; i++) {
        var a = argv[i];
        var v = null;
        var eq = a.indexOf('=');
        if (a.slice(0, 2) === '--' && eq > 2) {
            v = a.slice(eq + 1);
            a = a.slice(0, eq);
        }

        switch (a) {
            case '--list': opts.list = true; break;
            case '--json': opts.json = true; break;
            case '--help':
            case '-h': return null;
            case '--files':
            case '--filter':
            case '--batch':
            case '--requires':
            case '--timeout':
            case '--concurrency':
            case '--report':
                if (v === null) {
                    v = argv[++i];
                    if (v === undefined || v.slice(0, 2) === '--') throw new Error('missing value for ' + a);
                }
                opts[a.slice(2)] = v;
                break;
            default: throw new Error('unknown option: ' + a);
        }
    }
    return opts;
}

function describeBlock(b) {
    var detail = b.class;
    if (b.class === 'requires') detail += ': ' + b.value;
    else if (b.class === 'fragment' && b.value) detail += ': ' + b.value;
    return b.owner + ' [' + b.where + '] #' + b.index + ' ' + detail;
}

function printList(blocks) {
    var s = summarize(blocks);
    console.log('IDL examples: ' + s.total + ' block(s): runnable ' + s.runnable + ', fragment ' + s.fragment +
        ', requires ' + s.requires + ', malformed ' + s.malformed);
    blocks.forEach(function (b) { console.log(describeBlock(b)); });
}

function printRun(summary, results, reportPath) {
    console.log('IDL example check: ' + summary.total + ' block(s): runnable ' + summary.runnable +
        ', fragment ' + summary.fragment + ', requires ' + summary.requires + ', malformed ' + summary.malformed);
    console.log('executed: ok ' + summary.ok + ', fail ' + summary.fail + ', timeout ' + summary.timeout +
        ', skipped ' + summary.skip + ' (long-running ' + summary.skipLong + '), problems ' + summary.problems);

    if (reportPath)
        console.log('report: ' + reportPath);

    var problems = results.filter(function (r) { return r.problem; });
    if (problems.length) {
        console.log('\n== marker and fragment problems (' + problems.length + ') ==');
        problems.slice(0, 50).forEach(function (r) { console.log('  ' + r.owner + ': ' + r.error); });
        if (problems.length > 50)
            console.log('  ... and ' + (problems.length - 50) + ' more');
    }

    var failures = results.filter(isFailure);
    if (failures.length) {
        console.log('\n== failed runnable examples (' + failures.length + ') ==');
        failures.slice(0, 100).forEach(function (r) {
            console.log('  ' + r.owner + ' [' + r.status + ', exit ' + (r.exit === null ? '-' : r.exit) + ']: ' +
                (r.error || '(no stderr)'));
        });
        if (failures.length > 100)
            console.log('  ... and ' + (failures.length - 100) + ' more');
    }

    console.log('\n' + (summary.fail + summary.timeout + summary.problems ?
        'FAIL: ' + (summary.fail + summary.timeout) + ' failure(s), ' + summary.problems + ' problem(s)' :
        'OK: all executed examples passed'));
}

function loadIdl(folder) {
    return require('./util/parser')(folder || IDL_FOLDER);
}

function cli(argv) {
    return new Promise(function (resolve) {
        var opts;
        try {
            opts = parseArgs(argv);
        } catch (e) {
            resolve(usage(firstLine((e && e.message) || e)));
            return;
        }
        if (!opts) {
            resolve(usage(null));
            return;
        }

        var defs = loadIdl();
        var blocks = extractBlocks(defs);

        var definitions = null;
        if (opts.files) definitions = definitionsFromFiles(defs, opts.files);
        else if (opts.batch) definitions = batchDefinitions(opts.batch);

        blocks = selectBlocks(blocks, { definitions: definitions, filter: opts.filter });

        if (opts.list) {
            printList(blocks);
            resolve(0);
            return;
        }

        var o = normalizeRunOptions({
            timeout: opts.timeout,
            concurrency: opts.concurrency,
            requires: parseRequires(opts.requires)
        });

        runBlocks(blocks, o).then(function (res) {
            var reportPath = null;
            if (opts.json || opts.report) {
                reportPath = path.resolve(opts.report || DEFAULT_REPORT);
                writeReport(reportPath, buildReport(res.results, res.summary, {
                    fibjs: o.fibjs,
                    definitions: definitions,
                    filter: opts.filter,
                    batch: opts.batch,
                    requires: o.requires,
                    timeout: o.timeout,
                    concurrency: o.concurrency
                }));
            }

            if (opts.json) {
                console.log(JSON.stringify({
                    ok: res.summary.fail + res.summary.timeout + res.summary.problems === 0,
                    report: reportPath,
                    summary: res.summary
                }, null, 2));
            } else {
                printRun(res.summary, res.results, reportPath);
            }

            resolve(res.summary.fail + res.summary.timeout + res.summary.problems ? 1 : 0);
        }, function (e) {
            console.error('[check_idl_examples] ' + firstLine((e && e.message) || e));
            resolve(1);
        });
    });
}

// fibjs has no require.main, so the entry point is recognized from argv[1]
function isEntryPoint() {
    var entry = process.argv && process.argv[1];
    if (!entry) return false;
    try {
        return path.resolve(entry) === __filename;
    } catch (e) {
        return false;
    }
}

if (isEntryPoint()) {
    cli(process.argv.slice(2)).then(function (code) {
        process.exit(code);
    }, function (e) {
        console.error('[check_idl_examples] ' + firstLine((e && e.message) || e));
        process.exit(1);
    });
}

module.exports = {
    extractBlocks: extractBlocks,
    classifyBlock: classifyBlock,
    fragmentSyntaxError: fragmentSyntaxError,
    selectBlocks: selectBlocks,
    runBlock: runBlock,
    runBlocks: runBlocks,
    summarize: summarize,
    buildReport: buildReport,
    loadIdl: loadIdl,
    cli: cli,
    checkedDefinitions: checkedDefinitions,
    batchDefinitions: batchDefinitions,
    definitionsFromFiles: definitionsFromFiles,
    parseRequires: parseRequires,
    SERVICES: SERVICES,
    IDL_FOLDER: IDL_FOLDER,
    TODOS_FILE: TODOS_FILE,
    DEFAULT_REPORT: DEFAULT_REPORT,
    DEFAULT_FIBJS: DEFAULT_FIBJS,
    DEFAULT_TIMEOUT: DEFAULT_TIMEOUT,
    DEFAULT_CONCURRENCY: DEFAULT_CONCURRENCY,
    LONG_RUNNING_MS: LONG_RUNNING_MS
};
