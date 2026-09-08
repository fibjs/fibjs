// only-marked interplay fixture for --test-name-pattern regression tests
// (fibjs native only-mode: the only-marked test's parent gets run_level
// TEST_ONLY; sibling normal tests take the counted-skip path).
// Registers nothing unless NAME_PATTERN_FIXTURE=1 is set (full-suite safe).
'use strict';

if (process.env.NAME_PATTERN_FIXTURE === '1') {
    const { describe, it } = require('node:test');

    describe('only-parent', () => {
        it('only-target', { only: true }, () => { });
        it('plain-leaf', () => { });
    });
}
