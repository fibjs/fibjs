/*
 * promise_body.js — scene: plain (non-async) bodies return promises
 *
 * Expects: the runner awaits a returned promise or thenable like Node's
 * PromiseResolve. The resolved case settles during its own case, and the two
 * rejections fail their cases (exit 1) instead of passing in 0ms and surfacing
 * as asynchronous activity after the summary.
 */
var { describe, it } = require('node:test');

describe('promise body', () => {
    it('resolves later', () => new Promise((resolve) => {
        setTimeout(() => {
            console.log('[RESOLVED RAN]');
            resolve();
        }, 100);
    }));

    it('rejects later', () => new Promise((resolve, reject) => {
        setTimeout(() => reject(new Error('plain boom')), 100);
    }));

    it('thenable rejects', () => ({
        then(resolve, reject) {
            setTimeout(() => reject(new Error('thenable boom')), 100);
        }
    }));
});
