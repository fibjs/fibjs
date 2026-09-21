// Fixture for the after-hook failure semantics of the fibjs --test runner
// (test/runner_hook_test.js): every direct test passes, the after hook throws.
// Registers nothing unless RUNNER_HOOK_FIXTURE=1 is set, so full-suite runs that
// discover this file are unaffected.
'use strict';

if (process.env.RUNNER_HOOK_FIXTURE === '1') {
    const { describe, it, after } = require('node:test');

    describe('suite', () => {
        it('a', () => { });
        it('b', () => { });
        it('c', () => { });

        after(() => {
            throw new Error('boom in after hook');
        });
    });
}
