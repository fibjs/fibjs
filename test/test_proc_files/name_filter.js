/*
 * name_filter.js — scene: --test-name-pattern with the JSON reporter
 *
 * Registers nothing unless FIBJS_TEST_PROC_FIXTURE=1 is set, so full-suite runs
 * that discover this file are unaffected.
 *
 * Expects: non-matching top-level suites are omitted from the JSON `tests`
 * arrays (no null placeholders), whether the match is the second suite or a
 * case inside it.
 */
if (process.env.FIBJS_TEST_PROC_FIXTURE === '1') {
    var { describe, it } = require('node:test');

    describe('AAA not matched', () => {
        it('a-1', () => { });
        it('a-2', () => { });
    });

    describe('BBB matched', () => {
        it('b-1', () => { });
        it('b-2', () => { });
    });
}
