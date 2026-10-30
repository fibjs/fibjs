// Fixtures for the hook lifecycle parity work (test/runner_hook_test.js):
// declaration-order hooks, afterEach short-circuiting, before-failure gating
// and todo bodies. Every block logs through STEP so the harness can compare the
// exact sequence; node produces the same sequence under
// `node --test --test-isolation=none <file>`.
// Registers nothing unless RUNNER_HOOK_FIXTURE=1 is set, so full-suite runs
// that discover this file are unaffected.
'use strict';

if (process.env.RUNNER_HOOK_FIXTURE === '1') {
    const { describe, it, before, after, beforeEach, afterEach } = require('node:test');

    const step = (name) => console.log(`STEP ${name}`);

    // Same-level hooks run in declaration order (node: createHook pushes, and
    // afterEach is spliced before ownAfterEachCount).
    describe('order', () => {
        before(() => step('order:before#1'));
        before(() => step('order:before#2'));
        beforeEach(() => step('order:beforeEach#1'));
        beforeEach(() => step('order:beforeEach#2'));
        afterEach(() => step('order:afterEach#1'));
        afterEach(() => step('order:afterEach#2'));
        after(() => step('order:after#1'));
        after(() => step('order:after#2'));

        it('case', () => step('order:case'));

        // Inner level runs before the outer one once, after the case.
        describe('inner', () => {
            afterEach(() => step('inner:afterEach'));
            it('inner case', () => step('inner:case'));
        });
    });

    // A throwing afterEach stops the rest of the chain, including ancestors
    // (node's runHook throws out of the loop).
    describe('afterEach throw', () => {
        afterEach(() => step('throw:outer afterEach'));

        describe('inner', () => {
            afterEach(() => {
                step('throw:inner afterEach#1');
                throw new Error('boom in afterEach');
            });
            afterEach(() => step('throw:inner afterEach#2'));
            it('case', () => step('throw:case'));
        });
    });

    // A failed before keeps the subtests from starting at all, so their
    // afterEach never runs (node: `const subtests = this.skipped || this.error ? [] : this.subtests`).
    describe('before throw', () => {
        before(() => {
            step('beforethrow:before');
            throw new Error('boom in before');
        });
        after(() => step('beforethrow:after'));
        afterEach(() => step('beforethrow:afterEach'));
        it('case', () => step('beforethrow:case'));

        describe('clean', () => {
            before(() => step('clean:before'));
            after(() => step('clean:after'));
            it('clean case', () => step('clean:case'));
        });
    });

    // todo is a mark, not a skip: the body and the per-case hooks still run.
    describe('todo', () => {
        before(() => step('todo:before'));
        after(() => step('todo:after'));
        beforeEach(() => step('todo:beforeEach'));
        afterEach(() => step('todo:afterEach'));

        it('todo ok', { todo: true }, () => step('todo:body ok'));
        it('plain', () => step('todo:body plain'));
    });

    // A describe.todo marks the whole subtree as todo, and its cases still run.
    describe.todo('todo suite', () => {
        before(() => step('todosuite:before'));
        it('case', () => step('todosuite:case'));
    });
}
