// Fixture for --test-name-pattern CLI regression tests (test/runner_filter_test.js).
// Registers nothing unless NAME_PATTERN_FIXTURE=1 is set, so full-suite runs
// that discover this file are unaffected.
'use strict';

if (process.env.NAME_PATTERN_FIXTURE === '1') {
    const { describe, it, before, after } = require('node:test');

    describe('suite-A', () => {
        before(() => console.log('[A-before]'));
        after(() => console.log('[A-after]'));
        it('a1', () => { });
        describe('suite-B', () => {
            before(() => console.log('[B-before]'));
            it('b1', () => { });
        });
        describe('suite-C', () => {
            before(() => console.log('[C-before]'));
            it('c1', () => { });
        });
        it('fail-zzz', () => {
            if (process.env.RUN_FAIL === '1')
                throw new Error('expected failure');
        });
    });
}
