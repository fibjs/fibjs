/*
 * leak.js — scene: leaked resource hangs the process
 *
 * setInterval is never cleared: the timer holds an isolate ref, so the process
 * does not exit after the tests finish.
 * Expects: watchdog (FIBJS_TEST_WATCHDOG_MS shortens it) fires, exit 124 +
 * diagnostic report.
 */
var { describe, it } = require('node:test');

describe('leak', () => {
    it('leak a timer', () => {
        setInterval(() => {}, 1000);
    });
});
