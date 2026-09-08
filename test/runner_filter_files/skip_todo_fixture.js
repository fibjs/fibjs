// skip/todo interplay fixture for --test-name-pattern regression tests.
// Registers nothing unless NAME_PATTERN_FIXTURE=1 is set (full-suite safe).
'use strict';

if (process.env.NAME_PATTERN_FIXTURE === '1') {
    const { describe, it } = require('node:test');

    describe('st-suite', () => {
        it('run-hit', () => { });
        it('skip-hit', { skip: true }, () => { });
        it('skip-miss', { skip: true }, () => { });
        it('todo-hit', { todo: true }, () => { });
        it('todo-miss', { todo: true }, () => { });
    });
}
