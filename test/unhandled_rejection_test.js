// Unhandled promise rejection behavior (node >= 15 compatible):
// - promise has no handler AND process has no 'unhandledRejection' listener
//   -> log error and exit with code 1 (skipping 'beforeExit')
// - process has an 'unhandledRejection' listener -> dispatch (reason, promise),
//   process keeps running
var { describe, it } = require('node:test');
var assert = require('assert');

var child_process = require('child_process');
var path = require('path');

describe('unhandledRejection', () => {
    function run(script) {
        return child_process.spawnSync(process.execPath, [
            path.join(__dirname, 'unhandled_rejection', script)
        ], { encoding: 'utf8' });
    }

    it('no listener: process exits with code 1 and logs the error', () => {
        var r = run('no_listener.js');
        assert.equal(r.status, 1, 'process should exit with code 1, got ' + r.status);
        assert.ok(r.stderr.includes('Error: boom'), 'stderr should contain the error, got: ' + r.stderr);
    });

    it('no listener with pending task: exits with code 1 immediately', () => {
        // Regression for the "process hangs until external kill" behavior:
        // a 5s timer keeps the loop alive, but the unhandled rejection must
        // terminate the process right away instead of waiting.
        var r = run('no_listener_pending_task.js');
        assert.equal(r.status, 1, 'process should exit with code 1, got ' + r.status);
        assert.ok(r.stderr.includes('Error: boom'), 'stderr should contain the error, got: ' + r.stderr);
    });

    it('with listener: event dispatched with (reason, promise), process keeps running', () => {
        var r = run('with_listener.js');
        assert.equal(r.status, 0, 'process should exit normally, got ' + r.status);
        assert.equal(r.stdout.trim(), 'reason: boom\nisPromise: true');
        assert.equal(r.stderr, '', 'stderr should be empty, got: ' + r.stderr);
    });

    it('rejection caught in the same tick: no exit, no stderr noise', () => {
        var r = run('caught_same_tick.js');
        assert.equal(r.status, 0, 'process should exit normally, got ' + r.status);
        assert.equal(r.stdout.trim(), 'caught: boom');
        assert.equal(r.stderr, '', 'stderr should be empty, got: ' + r.stderr);
    });

    it('fiber throw (path A): logs the error but does not exit with code 1', () => {
        // Fiber throws keep the fibjs "report but do not exit" semantics and
        // must NOT be affected by the unhandledRejection handling.
        var r = run('fiber_throw.js');
        assert.equal(r.status, 0, 'process should exit normally, got ' + r.status);
        assert.ok(r.stderr.includes('fiber boom'), 'stderr should contain the fiber error, got: ' + r.stderr);
    });
});
