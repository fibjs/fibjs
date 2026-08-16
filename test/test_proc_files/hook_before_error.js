/*
 * hook_before_error.js — scene: d1.before throws
 *
 * Expects (after the P1 hook alignment): d1's tests are skipped and marked
 * failed, d2 keeps running, exit code 1. Previously the whole run aborted and
 * d2 never ran.
 *
 * Assertions: stdout must contain [b RAN] and must not contain [a RAN].
 */
var { describe, it, before } = require('node:test');

describe('d1', () => {
    before(() => {
        throw new Error('boom before');
    });
    it('a', () => {
        console.log('[a RAN]');
    });
});

describe('d2', () => {
    it('b', () => {
        console.log('[b RAN]');
    });
});
