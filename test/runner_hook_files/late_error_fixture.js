// Fixture for asynchronous activity that outlives its test
// (test/runner_hook_test.js).
//
// Node reports "generated asynchronous activity after the test ended" and
// fails the run; fibjs used to log the error and exit 0, which let a case that
// asserted inside a callback pass unseen. The late throw is the only error in
// the file, so the runner still prints a fully green tree and the failure is
// carried by the exit code alone - exactly like node.
//
// Registers nothing unless RUNNER_HOOK_FIXTURE=1 is set, so full-suite runs
// that discover this file are unaffected.
'use strict';

if (process.env.RUNNER_HOOK_FIXTURE === '1') {
    const { it } = require('node:test');

    it('body that finishes early', () => {
        setTimeout(() => {
            throw new Error('late boom');
        }, 10);
    });

    it('plain body', () => { });
}
