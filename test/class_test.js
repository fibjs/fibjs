var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

describe("class test", () => {
    it("toString", () => {
        o = Buffer.from('aaa');

        assert.equal(Object.prototype.toString.call(o), "[object Uint8Array]");

        assert.equal(Object.prototype.toString.call(process), "[object process]");
        assert.equal(Object.prototype.toString.call(console), "[object console]");
    });
});

