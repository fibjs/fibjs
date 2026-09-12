/*
 * test_proc_test.js
 *
 * Process-level behavior tests for the test framework: run standalone fixture
 * processes via child_process and observe exit code / stdout / stderr for
 * behaviors that cannot be verified in-process:
 *
 *   - pass -> exit 0
 *   - fail -> exit 1 (not the failure count, see plans/test-run-watchdog.md §4.4)
 *   - leaked resources hang -> watchdog exit 124 + diagnostic report (§4.2/§4.4)
 *   - hook errors no longer abort the rest of the run (§2.5, P1)
 *   - top-level before/after support (§2.5, P1)
 *
 * Cases depending on P0/P1 fixes were initially marked xit and enabled one by
 * one as the fixes landed.
 */
var { describe, it, xit, before, after } = require('node:test');
var assert = require('assert');

var child_process = require('child_process');
var path = require('path');
var util = require('util');
var io = require('io');
var fs = require('fs');
var coroutine = require('coroutine');

const FIXTURE_DIR = path.join(__dirname, 'test_proc_files');

// Scene -> fixture file (one file per scene; add a new scene by adding a file)
const FIXTURES = {
    'pass': 'pass.js',
    'fail': 'fail.js',
    'leak': 'leak.js',
    'leak_socket': 'leak_socket.js',
    'leak_worker': 'leak_worker.js',
    'leak_fswatcher': 'leak_fswatcher.js',
    'leak_child': 'leak_child.js',
    'fail_hang': 'fail_hang.js',
    'hook_before_error': 'hook_before_error.js',
    'top_hook': 'top_hook.js'
};

const DEFAULT_TIMEOUT = 20000; // protective timeout (ms) when the subprocess hangs

function stripAnsi(s) {
    return s.replace(/\x1b\[[0-9;?]*[a-zA-Z]/g, '');
}

// Parse the summary lines emitted by the test framework on stdout
// (result info is produced by the framework itself)
function testSummary(stdout) {
    var s = { completed: 0, passed: 0, failed: 0, todo: 0, skipped: 0 };

    var line = stdout.find(l => /\d+ tests completed/.test(l));
    if (line) {
        var m = /(\d+) tests completed/.exec(line);
        s.completed = parseInt(m[1]);
    }
    line = stdout.find(l => /\d+ tests passed/.test(l));
    if (line) {
        var m = /(\d+) tests passed/.exec(line);
        s.passed = parseInt(m[1]);
    }
    line = stdout.find(l => /\d+ tests failed/.test(l));
    if (line) {
        var m = /(\d+) tests failed/.exec(line);
        s.failed = parseInt(m[1]);
    }
    line = stdout.find(l => /\d+ todo tests/.test(l));
    if (line) {
        var m = /(\d+) todo tests/.exec(line);
        s.todo = parseInt(m[1]);
    }
    line = stdout.find(l => /\d+ tests skipped/.test(l));
    if (line) {
        var m = /(\d+) tests skipped/.exec(line);
        s.skipped = parseInt(m[1]);
    }

    return s;
}

// Parse the watchdog report (stderr text lines), returns { fibers, nativeObjects },
// matching the report format (see report_watchdog). The report is one tagged
// header line plus an untagged body (the watchdog writes it in one block and
// exits right after), with sections:
//
//   [test-watchdog] process alive 10000ms after test run ended
//   fibers: N
//     fiber #<id> (stack_usage=<n>):
//       at <frame> (file:line:col)          <- indented by the stack formatter
//   nativeObjects (after gc):
//     # count includes derived classes, direct = instances of the exact class
//     object: 1986
//     <tree glyphs>Stream: 60 (direct: 4)
//     <tree glyphs><tree glyphs>Socket: 42
//   memory: rss=... heapTotal=... heapUsed=... external=... ExtStrings=...
//
// nativeObjects is a depth-first walk of the class tree, so each line is
// indented by three columns per level (tree glyphs U+251C/U+2514/U+2500 for the
// branch, U+2502 for continuation): the indentation encodes the inheritance
// chain and is decoded back into depth/parent below.
function parseWatchdogReport(r) {
    var stderr = r.stderr;
    var report = { fibers: [], nativeObjects: [] };

    var start = stderr.findIndex(l => l.startsWith('[test-watchdog]'));
    if (start < 0)
        return report;

    var section = '';
    var curFiber = null;
    var stack = []; // ancestors of the line being parsed: [{ indent, node }]

    stderr.slice(start + 1).forEach(l => {
        if (/^fibers: \d+$/.test(l)) {
            section = 'fibers';
            return;
        }

        if (l === 'nativeObjects (after gc):') {
            section = 'native';
            return;
        }

        if (/^memory: /.test(l)) {
            section = '';
            return;
        }

        if (section === 'fibers') {
            var m = /^ {2}fiber #(\d+) \(stack_usage=(\d+)\):$/.exec(l);

            if (m) {
                curFiber = { id: parseInt(m[1]), stackUsage: parseInt(m[2]), stack: '' };
                report.fibers.push(curFiber);
            } else if (curFiber && /^ {4}/.test(l)) {
                // stack frames come pre-indented (4 spaces) from traceInfo
                curFiber.stack += (curFiber.stack ? '\n' : '') + l.substring(4);
            } else
                curFiber = null;
            return;
        }

        if (section === 'native') {
            // tree line: <indent><glyphs>Class: N (direct: M)
            var m2 = /^([\s\u2502\u251c\u2514\u2500]*?)(\S+): (\d+)(?: \(direct: (\d+)\))?$/.exec(l);
            if (!m2)
                return; // legend line, blanks

            var node = { class: m2[2], objects: parseInt(m2[3]), depth: 0, parent: null };
            if (m2[4] !== undefined)
                node.direct = parseInt(m2[4]);

            // indentation defines the tree: drop ancestors that are not deeper
            // than the current line, the top of the stack is the parent
            var indent = m2[1].length;
            while (stack.length && stack[stack.length - 1].indent >= indent)
                stack.pop();

            node.depth = stack.length;
            if (stack.length)
                node.parent = stack[stack.length - 1].node.class;

            stack.push({ indent: indent, node: node });
            report.nativeObjects.push(node);
        }
    });

    return report;
}

// Verify the native object tree returned by parseWatchdogReport: a class count
// includes its derived classes, so every parent count must equal the sum of its
// children's counts plus the instances of its exact class ('direct', annotated
// only when positive). Returns the number of classes seen below the root.
function assertClassTree(report) {
    var nodes = report.nativeObjects;
    var childSum = {};
    var nested = 0;

    assert.ok(nodes.length > 0, "expect a native object tree, got: " + JSON.stringify(report));
    assert.equal(nodes[0].depth, 0, "expect the root first, got: " + JSON.stringify(nodes[0]));

    nodes.forEach(o => {
        if (o.depth === 0)
            return;
        nested++;
        childSum[o.parent] = (childSum[o.parent] || 0) + o.objects;
    });

    nodes.forEach(o => {
        if (childSum[o.class] === undefined)
            return;

        var direct = o.objects - childSum[o.class];
        assert.ok(direct >= 0, o.class + ": children (" + childSum[o.class]
            + ") exceed the class count (" + o.objects + ")");
        assert.equal(o.direct || 0, direct, o.class + ": direct mismatch, got: " + JSON.stringify(o));
    });

    return nested;
}

// Liveness probe: process.kill(pid, 0) throws when the pid is gone. On Linux a
// zombie (state Z in /proc/pid/stat) is treated as gone too: the process is
// dead and merely awaits reaping by init (container PID 1 may not reap).
function pidAlive(pid) {
    try {
        process.kill(pid, 0);
    } catch (e) {
        return false;
    }

    if (process.platform === 'linux') {
        try {
            // state is the first field after "comm)" — comm itself may contain
            // ')', so read from the LAST occurrence
            var stat = fs.readFileSync('/proc/' + pid + '/stat', 'utf8');
            var idx = stat.lastIndexOf(') ');
            var state = idx >= 0 ? stat.charAt(idx + 2) : '';
            return state !== 'Z';
        } catch (e) {
            // /proc entry already gone
            return false;
        }
    }

    return true;
}

// Run a fixture subprocess and wait for it to exit.
// If the subprocess hangs (e.g. a leak scenario), a setTimeout kill backs us up
// and the returned timedout flag tells the caller apart, so the parent test
// never gets stuck.
function runFixture(scene, opts) {
    opts = opts || {};

    var fixture = FIXTURES[scene];
    assert.ok(fixture, "unknown scene: " + scene);

    var env = util.pick(process.env, [
        "HOME",
        "TMPDIR",
        "PATH",
        "SYSTEMROOT",
        "SystemRoot",
        "TEMP",
        "TMP",
        "CommonProgramFiles",
        "CommonProgramFiles(x86)",
        "CommonProgramW6432",
        "ProgramFiles",
        "ProgramFiles(x86)",
        "ProgramW6432"
    ]);

    // Watchdog duration is configurable (FIBJS_TEST_WATCHDOG_MS, P0 implementation)
    if (opts.watchdogMs)
        env.FIBJS_TEST_WATCHDOG_MS = String(opts.watchdogMs);

    var bs = child_process.spawn(process.execPath, [path.join(FIXTURE_DIR, fixture)], { env: env });
    var stdout = new io.BufferedStream(bs.stdout);
    var stderr = new io.BufferedStream(bs.stderr);

    var timedout = false;
    var timer = setTimeout(() => {
        timedout = true;
        bs.kill();
    }, opts.timeoutMs || DEFAULT_TIMEOUT);

    bs.join();
    clearTimeout(timer);

    return {
        exitCode: bs.exitCode,
        timedout: timedout,
        stdout: stdout.readLines().map(stripAnsi),
        stderr: stderr.readLines().map(stripAnsi)
    };
}

describe("test framework process-level behavior", () => {
    it("pass: exit 0", () => {
        var r = runFixture('pass');

        assert.equal(r.timedout, false, "subprocess must exit normally");
        assert.equal(r.exitCode, 0);
        assert.ok(r.stdout.some(l => l.includes('2 tests completed')),
            "expect test summary in stdout, got: " + JSON.stringify(r.stdout));
        assert.ok(r.stdout.some(l => l.includes('2 tests passed')),
            "expect passed summary in stdout, got: " + JSON.stringify(r.stdout));
    });

    // === Cases below depend on the P0/P1 fixes in plans/test-run-watchdog.md ===
    //
    // P0: exit code fix (failure = 1, not the failure count)
    it("fail: exit 1 (P0: exit code fix)", () => {
        var r = runFixture('fail');

        assert.equal(r.timedout, false, "subprocess must exit normally");
        assert.equal(r.exitCode, 1);
        assert.ok(r.stdout.some(l => l.includes('2 tests failed')),
            "expect failed summary in stdout, got: " + JSON.stringify(r.stdout));
    });

    // P0: post-run watchdog (hang -> exit 124 + diagnostic report after 10s)
    // FIBJS_TEST_WATCHDOG_MS shortens the watchdog so the case does not wait 10s
    it("leak: watchdog exit 124 + report (P0: watchdog)", () => {
        var r = runFixture('leak', { watchdogMs: 800 });

        assert.equal(r.timedout, false, "watchdog must fire before protective timeout");
        assert.equal(r.exitCode, 124);
        assert.ok(r.stdout.some(l => l.includes('1 tests completed')),
            "expect test summary in stdout, got: " + JSON.stringify(r.stdout));

        var report = parseWatchdogReport(r);

        // Result: summary from the test framework on stdout (1 total / 1 passed)
        var s = testSummary(r.stdout);
        assert.deepEqual({ completed: s.completed, passed: s.passed },
            { completed: 1, passed: 1 }, "expect test summary, got: " + JSON.stringify(s));

        // Leaked timer: the watchdog's own timer is excluded (non-object_base
        // timer task), so this is the leaked setInterval (>= 1)
        assert.ok(report.nativeObjects.some(o => o.class === 'Timer' && o.objects >= 1),
            "expect leaked Timer in report, got: " + JSON.stringify(report.nativeObjects));

        assertClassTree(report);

        // Residual fibers: watchdog itself is excluded, no leaked fiber in this scene
        assert.equal(report.fibers.length, 0,
            "expect no residual fiber, got: " + JSON.stringify(report.fibers));
    });

    // P0: leaked TCP connection
    it("leak socket: watchdog reports Socket + TcpServer (P0: watchdog)", () => {
        var r = runFixture('leak_socket', { watchdogMs: 800 });

        assert.equal(r.timedout, false, "watchdog must fire before protective timeout");
        assert.equal(r.exitCode, 124);

        var report = parseWatchdogReport(r);

        var s = testSummary(r.stdout);
        assert.deepEqual({ completed: s.completed, passed: s.passed },
            { completed: 1, passed: 1 }, "expect test summary, got: " + JSON.stringify(s));
        assert.ok(report.nativeObjects.some(o => o.class === 'Socket'),
            "expect leaked Socket class, got: " + JSON.stringify(report.nativeObjects));
        assert.ok(report.nativeObjects.some(o => o.class === 'TcpServer'),
            "expect leaked TcpServer class, got: " + JSON.stringify(report.nativeObjects));

        // Hierarchy: the report paints the class tree (indentation = inheritance
        // depth), so each count sits under its base class instead of being a flat
        // list of class names
        assert.ok(assertClassTree(report) > 0,
            "expect a nested class tree, got: " + JSON.stringify(report.nativeObjects));

        var root = report.nativeObjects[0];
        assert.equal(root.class, 'object', "expect object_base as the root, got: " + JSON.stringify(root));

        var socket = report.nativeObjects.find(o => o.class === 'Socket');
        var tcpServer = report.nativeObjects.find(o => o.class === 'TcpServer');
        assert.ok(socket && tcpServer,
            "expect Socket and TcpServer in the report, got: " + JSON.stringify(report.nativeObjects));
        assert.equal(socket.parent, 'Stream',
            "expect Socket nested under Stream, got: " + JSON.stringify(socket));
        assert.equal(tcpServer.parent, 'EventEmitter',
            "expect TcpServer nested under EventEmitter, got: " + JSON.stringify(tcpServer));
    });

    // P0: leaked worker_threads Worker
    it("leak worker: watchdog reports Worker (P0: watchdog)", () => {
        var r = runFixture('leak_worker', { watchdogMs: 800 });

        assert.equal(r.timedout, false, "watchdog must fire before protective timeout");
        assert.equal(r.exitCode, 124);

        var report = parseWatchdogReport(r);
        assert.ok(report.nativeObjects.some(o => o.class === 'Worker'),
            "expect leaked Worker class, got: " + JSON.stringify(report.nativeObjects));
    });

    // P0: leaked FSWatcher
    it("leak fswatcher: watchdog reports FSWatcher (P0: watchdog)", () => {
        var r = runFixture('leak_fswatcher', { watchdogMs: 800 });

        assert.equal(r.timedout, false, "watchdog must fire before protective timeout");
        assert.equal(r.exitCode, 124);

        var report = parseWatchdogReport(r);
        assert.ok(report.nativeObjects.some(o => o.class === 'FSWatcher'),
            "expect leaked FSWatcher class, got: " + JSON.stringify(report.nativeObjects));
    });

    // P0: leaked ChildProcess (subprocess never exits) + residual join fiber
    it("leak child: watchdog reports ChildProcess + residual fiber (P0: watchdog)", () => {
        var r = runFixture('leak_child', { watchdogMs: 800 });

        assert.equal(r.timedout, false, "watchdog must fire before protective timeout");
        assert.equal(r.exitCode, 124);

        var report = parseWatchdogReport(r);
        assert.ok(report.nativeObjects.some(o => o.class === 'ChildProcess'),
            "expect leaked ChildProcess class, got: " + JSON.stringify(report.nativeObjects));

        // Residual fiber: watchdog itself is excluded, only the fiber blocked in
        // ChildProcess.join remains, and its stack points at the hang point
        assert.equal(report.fibers.length, 1,
            "expect 1 residual fiber (blocked join), got: " + JSON.stringify(report.fibers));
        assert.ok(report.fibers[0].stack.includes('ChildProcess.join'),
            "expect residual fiber stack to point at ChildProcess.join, got: " + JSON.stringify(report.fibers));

        // The watchdog must kill the leaked child before exiting 124: without
        // this cleanup the child is orphaned and keeps running (holding ports,
        // consuming CPU) and can interfere with later test runs.
        var pidLine = r.stdout.find(l => l.startsWith('LEAKED_CHILD_PID='));
        assert.ok(pidLine, "expect leaked child pid line in stdout, got: " + JSON.stringify(r.stdout));
        var childPid = parseInt(pidLine.split('=')[1]);
        assert.ok(childPid > 0, "expect a valid leaked child pid, got: " + JSON.stringify(pidLine));

        assert.ok(r.stderr.some(l => l.includes('killed 1 leaked child process(es)')),
            "expect watchdog child-cleanup line in stderr, got: " + JSON.stringify(r.stderr));

        // Poll briefly: the SIGKILLed child must disappear (zombie tolerated,
        // it is dead and only awaits reaping)
        var deadline = Date.now() + 3000;
        var stillAlive = true;
        while (Date.now() < deadline && (stillAlive = pidAlive(childPid)))
            coroutine.sleep(50);
        assert.equal(stillAlive, false,
            "leaked child process " + childPid + " must be killed by the watchdog");
    });

    // P0: when failure and hang coexist, watchdog exit code 124 wins over failure code 1
    it("fail + hang: watchdog exit 124 takes precedence (P0: watchdog)", () => {
        var r = runFixture('fail_hang', { watchdogMs: 800 });

        assert.equal(r.timedout, false, "watchdog must fire before protective timeout");
        assert.equal(r.exitCode, 124, "watchdog exit code must take precedence");

        var report = parseWatchdogReport(r);
        var s = testSummary(r.stdout);

        assert.equal(s.completed, 2, "expect 2 tests in summary, got: " + JSON.stringify(s));
        assert.equal(s.failed, 1, "expect 1 failed in summary, got: " + JSON.stringify(s));
        assert.equal(s.passed, 1, "expect 1 passed in summary, got: " + JSON.stringify(s));
    });

    // P1: hook errors no longer abort the rest of the run
    it("hook before error: other suites keep running (P1)", () => {
        var r = runFixture('hook_before_error');

        assert.equal(r.timedout, false, "subprocess must exit normally");
        assert.equal(r.exitCode, 1);
        assert.ok(r.stdout.some(l => l.includes('[b RAN]')),
            "suite d2 must still run after d1.before error, got: " + JSON.stringify(r.stdout));
        assert.ok(!r.stdout.some(l => l.includes('[a RAN]')),
            "it of failed suite must be skipped, got: " + JSON.stringify(r.stdout));
    });

    // P1: top-level before/after support
    it("top-level before/after works (P1)", () => {
        var r = runFixture('top_hook');

        assert.equal(r.timedout, false, "subprocess must exit normally");
        assert.equal(r.exitCode, 0);
        assert.ok(r.stdout.some(l => l.includes('[top before RAN]')),
            "expect top-level before to run, got: " + JSON.stringify(r.stdout));
        assert.ok(r.stdout.some(l => l.includes('[top after RAN]')),
            "expect top-level after to run, got: " + JSON.stringify(r.stdout));
    });
});
