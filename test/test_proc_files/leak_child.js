/*
 * leak_child.js — scene: leaked ChildProcess + residual fiber
 *
 * The subprocess never exits (a timer inside keeps it alive), the parent never
 * kills or waits for it, and a separate fiber blocks on cp.join() (residual
 * fiber).
 * Expects: watchdog fires, exit 124; report shows the ChildProcess class count
 * and the residual fiber (>1, blocked in ChildProcess.join).
 */
var { describe, it } = require('node:test');
var child_process = require('child_process');
var coroutine = require('coroutine');

describe('leak child', () => {
    it('leak a child process with a waiting fiber', () => {
        var cp = child_process.spawn(process.execPath, ['-e', 'setInterval(() => {}, 1000)']);
        global.leakedChild = cp;

        // Residual fiber: blocks waiting for the subprocess to exit (never returns)
        coroutine.start(() => {
            cp.join();
        });
    });
});
