/*
 * fail_hang.js — scene: failing test + leaked resource
 *
 * Expects: the watchdog takes precedence over the failure exit code — exit 124
 * (not 1); the stdout summary shows failed=1 passed=1.
 */
var { describe, it } = require('node:test');
var assert = require('assert');

describe('fail + leak', () => {
    it('fail a test', () => {
        assert.equal(1, 2);
    });
    it('leak a timer', () => {
        setInterval(() => {}, 1000);
    });
});
