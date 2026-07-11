var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var util = require('util');
var path = require('path');
var child_process = require('child_process');

describe('promise', () => {
    function t(f, t) {
        var p = child_process.spawn(process.execPath, [path.join(__dirname, 'promise', f)]);
        var d = p.stderr.read() || p.stdout.read();
        var t1 = d ? d.toString() : "";

        for (var i = 0; i < 5; i++)
            t1 = t1.replace(__dirname, '');

        if (process.platform == "win32") {
            t1 = t1.replace(/\\/g, '/');
            t1 = t1.replace(/\r/g, '');
        }

        assert.equal(t1, t);
    }

    it("error in promise.then", () => {
        t('promise1.js', '/promise/promise1.js:2:11\n    throw new Error(1000);\n          ^\nError: 1000\n    at /promise/promise1.js:2:11 {}\n');
    });

    it("error in promise.then with catch", () => {
        t('promise2.js', 'catch promise error.\n');
    });

    it("error in promise", () => {
        t('promise3.js', '/promise/promise3.js:2:11\n    throw new Error(1000);\n          ^\nError: 1000\n    at /promise/promise3.js:2:11\n    at new Promise (<anonymous>)\n    at /promise/promise3.js:1:1 {}\n');
    });

    it("error in promise with catch", () => {
        t('promise4.js', 'catch promise error.\n');
    });

    it("error in promise with reject", () => {
        t('promise5.js', 'catch promise error.\n');
    });

    it("error in async function", () => {
        // require(esm) support: top-level await in a .js file is now retried as
        // an ES module, so the async function actually runs and throws.
        t('async1.js', '/promise/async1.js:2:11\n    throw new Error(100);\n          ^\nError: 100\n    at t (/promise/async1.js:2:11)\n    at /promise/async1.js:5:7 {}\n');
    });

    it("error in async function with sync", () => {
        t('async2.js', '/promise/async2.js:4:11\n    throw new Error(100);\n          ^\nError: 100\n    at t (/promise/async2.js:4:11)\n    at /promise/async2.js:7:13 {}\n');
    });

    it("error in async function without await", () => {
        t('async3.js', '/promise/async3.js:7:11\n    throw new Error(100);\n          ^\nError: 100\n    at a_test (/promise/async3.js:7:11) {}\n');
    });

    it("async method should reject promise instead of sync throw", () => {
        t('promise_async_throw.js', 'rejected: [20002] Parameter not optional.\n');
    });

    it("caught rejection should not produce stderr noise", () => {
        var p = child_process.spawn(process.execPath, [path.join(__dirname, 'promise', 'promise_catch_no_stderr.js')]);
        var stderr = p.stderr.read();
        var stdout = p.stdout.read();
        assert.equal(stdout ? stdout.toString() : "", "caught\n");
        assert.equal(stderr ? stderr.toString() : "", "");
    });

    it("microtask should run before next macrotask", done => {
        setImmediate(() => {
            let microtaskRan = false;

            Promise.resolve().then(() => {
                microtaskRan = true;
            });

            setImmediate(() => {
                assert.ok(microtaskRan, "Promise.then should drain before next setImmediate");
                done();
            });
        });
    });
});

