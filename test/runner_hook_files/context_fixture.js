// Fixture for the context object handed to bodies and hooks
// (test/runner_hook_test.js). Every line is a CTX record so the harness can
// compare the exact values; node produces the same records under
// `node --test --test-isolation=none <file>`.
// Registers nothing unless RUNNER_HOOK_FIXTURE=1 is set, so full-suite runs that
// discover this file are unaffected.
'use strict';

if (process.env.RUNNER_HOOK_FIXTURE === '1') {
    const { describe, it, before, beforeEach, afterEach } = require('node:test');

    describe('ctx-suite', (suite) => {
        console.log(`CTX suite name=${suite.name} fullName=${suite.fullName}`);

        before((hookCtx) => {
            console.log(`CTX before name=${hookCtx.name}`);
        });

        beforeEach((hookCtx) => {
            console.log(`CTX beforeEach name=${hookCtx.name}`);
        });

        afterEach((hookCtx) => {
            console.log(`CTX afterEach name=${hookCtx.name}`);
        });

        it('ctx-case', (c) => {
            console.log(`CTX case name=${c.name} fullName=${c.fullName}`);
            console.log('CTX api ' + [
                'skip=' + typeof c.skip,
                'todo=' + typeof c.todo,
                'diagnostic=' + typeof c.diagnostic,
                'assert=' + typeof c.assert.equal
            ].join(' '));
            c.diagnostic('a diagnostic');
            c.assert.equal(1, 1);
        });

        // A runtime skip counts as skipped and suppresses afterEach.
        it('ctx-skip', (c) => {
            console.log('CTX skip called');
            c.skip();
        });

        // A runtime todo classifies the outcome as todo; a throwing body does
        // not fail the run.
        it('ctx-todo', (c) => {
            console.log('CTX todo called');
            c.todo();
            throw new Error('ignored because the case is todo');
        });

        it('ctx-plain', () => {
            console.log('CTX plain');
        });
    });
}
