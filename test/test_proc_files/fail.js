/*
 * fail.js — scene: 2 failing tests
 *
 * Expects: exit code 1 (after the P0 exit code fix; previously the failure count).
 */
var { describe, it } = require('node:test');
var assert = require('assert');

describe('fail', () => {
    it('a', () => {
        assert.equal(1, 2);
    });
    it('b', () => {
        throw new Error('boom');
    });
});
