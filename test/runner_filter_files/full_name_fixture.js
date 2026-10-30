// Fixtures for the --test-name-pattern full-name and --test-skip-pattern work
// (test/runner_filter_test.js). Every fixture logs through LOG so the harness
// can compare the executed set with node's, and the shape mirrors the example in
// node's documentation: two suites that both contain a test with the same name,
// which can only be told apart by prefixing the ancestor names.
// Registers nothing unless NAME_PATTERN_FIXTURE=1 is set, so full-suite runs
// that discover this file are unaffected.
'use strict';

if (process.env.NAME_PATTERN_FIXTURE === '1') {
    const { describe, it, before, after } = require('node:test');

    const log = (name) => console.log(`[${name}]`);

    // alpha > beta > { gamma, delta }   and   alpha > gamma
    // 'beta gamma' hits only alpha > beta > gamma; 'alpha gamma' hits only the
    // second one (both tests are named gamma).
    describe('alpha', () => {
        before(() => log('alpha-before'));
        after(() => log('alpha-after'));

        describe('beta', () => {
            before(() => log('beta-before'));
            after(() => log('beta-after'));

            it('gamma', () => log('beta-gamma'));
            it('delta', () => log('beta-delta'));
        });

        it('gamma', () => log('alpha-gamma'));
    });

    // A separate top-level suite so a skip pattern can prune it whole.
    describe('omega', () => {
        before(() => log('omega-before'));
        after(() => log('omega-after'));

        it('first', () => log('omega-first'));
        it('second', () => log('omega-second'));
    });
}
