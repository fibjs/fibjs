/*
 * json_report.js — scene: input for the built-in JSON reporter
 *
 * Registers nothing unless FIBJS_TEST_PROC_FIXTURE=1 is set, so full-suite runs
 * that discover this file are unaffected.
 *
 * Expects: every case carries a status (passed/skipped/todo/cancelled) and the
 * file that defined it; a failing suite before hook leaves its cases cancelled
 * and the suite failed with the hook trace.
 */
if (process.env.FIBJS_TEST_PROC_FIXTURE === '1') {
    var { describe, it, before } = require('node:test');

    describe('passing suite', () => {
        it('ok', () => { });
        it('declarative skip', { skip: true }, () => { });
        it('declarative todo', { todo: true }, () => { });
        it('runtime skip', (ctx) => { ctx.skip(); });
    });

    describe('failing before', () => {
        before(() => {
            throw new Error('boom before');
        });
        it('cancelled case', () => { });
    });
}
