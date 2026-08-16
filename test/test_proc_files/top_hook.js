/*
 * top_hook.js — scene: top-level before/after
 *
 * The top-level hooks must be the first test API calls in the file (before any
 * describe/it) to expose the "set_hook without init -> [20009]" defect.
 *
 * Expects (after the P1 fix): registration does not throw, hooks run once
 * around the whole suite, exit code 0.
 */
var { describe, it, before, after } = require('node:test');

before(() => {
    console.log('[top before RAN]');
});
after(() => {
    console.log('[top after RAN]');
});

describe('d1', () => {
    it('a', () => {});
});
