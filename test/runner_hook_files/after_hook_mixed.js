// Fixture for the after-hook failure semantics of the fibjs --test runner
// (test/runner_hook_test.js): one real failing test, two passing ones, an after
// hook that throws, plus an untouched suite in the same file.
// Registers nothing unless RUNNER_HOOK_FIXTURE=1 is set, so full-suite runs that
// discover this file are unaffected.
'use strict';

if (process.env.RUNNER_HOOK_FIXTURE === '1') {
    const { describe, it, after } = require('node:test');
    const assert = require('assert');

    describe('suite', () => {
        it('a', () => { });
        it('b', () => {
            assert.strictEqual(1, 2, 'intentional failure');
        });
        it('c', () => { });

        after(() => {
            throw new Error('boom in after hook');
        });
    });

    describe('clean suite', () => {
        it('d', () => { });
    });
}
