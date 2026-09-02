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
// matching the report format (see report_watchdog)
function parseWatchdogReport(r) {
    var stderr = r.stderr;
    var report = { fibers: [], nativeObjects: [] };

    // Fiber blocks (fiber #N + stack lines) and native object counts (indented lines)
    var curFiber = null;
    stderr.forEach(l => {
        if (l.startsWith('[test-watchdog] fiber #')) {
            var m2 = /fiber #(\d+) \(stack_usage=(\d+)\)/.exec(l);
            curFiber = m2 ? { id: parseInt(m2[1]), stackUsage: parseInt(m2[2]), stack: '' } : null;
            if (curFiber)
                report.fibers.push(curFiber);
        } else if (l.startsWith('[test-watchdog]       ')) {
            if (curFiber)
                curFiber.stack += (curFiber.stack ? '\n' : '') + l.substring('[test-watchdog]       '.length);
        } else if (l.startsWith('[test-watchdog]   ')) {
            var m3 = /^   (\S+): (\d+)$/.exec(l.substring('[test-watchdog]'.length));
            if (m3)
                report.nativeObjects.push({ class: m3[1], objects: parseInt(m3[2]) });
        }
    });

    return report;
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
