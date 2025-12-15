var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

describe('ECMAScript modules', () => {
    it("BUGFIX: Crash when importing coroutine module after top-level await module", async () => {
        var m = await import('./test.mjs');
    });
});


