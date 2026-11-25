// Fixture for the load-failure regression tests (test/runner_filter_test.js).
// Registers nothing unless LOAD_FAILURE_FIXTURE=1 is set, so full-suite runs
// that happen to load this file are unaffected.
//
// The throw happens while the file is being loaded, before any test from it is
// registered: the runner turns it into a synthetic '<file> failed to load'
// case, which must stay visible under --test-name-pattern / --test-skip-pattern.
'use strict';

if (process.env.LOAD_FAILURE_FIXTURE === '1') {
    throw new Error('load failure fixture: boom');
}
