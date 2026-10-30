// Fixture for the retired single-parameter callback style
// (test/runner_hook_test.js).
//
// fibjs used to hand `done` to a one-parameter body, which node never did: node
// dispatches on arity (`fn.length >= 2` => (ctx, done), otherwise (ctx)). The
// old shape is retired, so these bodies now receive the context and calling it
// throws; the runner only emits one diagnostic so the migration is easy to
// spot. Two offending cases prove the warning is emitted once per run.
//
// Registers nothing unless RUNNER_HOOK_FIXTURE=1 is set, so full-suite runs
// that discover this file are unaffected.
'use strict';

if (process.env.RUNNER_HOOK_FIXTURE === '1') {
    const { describe, it } = require('node:test');

    describe('legacy', () => {
        it('callback style', (done) => {
            done();
        });

        it('context style', (ctx) => {
            console.log(`LEGACY ctx-name=${ctx.name}`);
        });

        it('callback style again', (callback) => {
            callback();
        });

        it('two parameters', (ctx, done) => {
            console.log(`LEGACY two-arg-name=${ctx.name}`);
            done();
        });
    });
}
