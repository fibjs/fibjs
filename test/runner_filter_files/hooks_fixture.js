// Hook/expansion gating fixture for --test-name-pattern regression tests.
// Registers nothing unless NAME_PATTERN_FIXTURE=1 is set (full-suite safe).
'use strict';

if (process.env.NAME_PATTERN_FIXTURE === '1') {
    const { describe, it, before, beforeEach } = require('node:test');

    describe('hook-suite', () => {
        before(() => console.log('[H-before]'));
        beforeEach(() => console.log('[H-beforeEach]'));
        it('hx1', () => { });
        describe('mid', () => {
            before(() => console.log('[M-before]'));
            it('my1', () => { });
            it('my2', () => { });
        });
        describe('L1', () => {
            before(() => console.log('[L1-before]'));
            describe('L2', () => {
                before(() => console.log('[L2-before]'));
                it('deepleaf', () => { });
            });
        });
    });

    describe('other-suite', () => {
        before(() => console.log('[O-before]'));
        it('z1', () => { });
    });
}
