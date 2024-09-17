var test = require("test");
test.setup();

var util = require('util');
var path = require('path');
var child_process = require('child_process');

describe('promise', () => {
    function t(f, t) {
        var p = child_process.spawn(process.execPath, [path.join(__dirname, 'promise', f)]);
        var d = p.stderr.read();
        var t1 = d ? d.toString() : "";

        for (var i = 0; i < 5; i++)
            t1 = t1.replace(__dirname, '');

        if (process.platform == "win32") {
            t1 = t1.replace(/\\/g, '/');
            t1 = t1.replace(/\r/g, '');
        }

        assert.equal(t, t1);
    }

    it("error in promise.then", () => {
        t('promise1.js', '/promise/promise1.js:2:11\n    throw new Error(1000);\n          ^\nError: 1000\n    at Promise.<anonymous> (/promise/promise1.js:2:11)\n');
    });

    it("error in promise.then with catch", () => {
        t('promise2.js', '/promise/promise2.js:2:11\n    throw new Error(1000);\n          ^\nError: 1000\n    at Promise.<anonymous> (/promise/promise2.js:2:11)\n');
    });

    it("error in promise", () => {
        t('promise3.js', 'Error: 1000\n    at /promise/promise3.js:2:11\n    at new Promise (<anonymous>)\n    at /promise/promise3.js:1:1\n');
    });

    it("error in promise with catch", () => {
        t('promise4.js', '');
    });

    it("error in promise with reject", () => {
        t('promise5.js', '');
    });

    xit("error in async function", () => {
        t('async1.js', '/promise/async1.js:2:11\n    throw new Error(100);\n          ^\nError: 100\n    at t (/promise/async1.js:2:11)\n    at /promise/async1.js:5:7\n');
    });

    it("error in async function with sync", () => {
        t('async2.js', '/promise/async2.js:4:11\n    throw new Error(100);\n          ^\nError: 100\n    at t (/promise/async2.js:4:11)\n    at /promise/async2.js:7:13\n');
    });

    it("error in async function without await", () => {
        t('async3.js', 'Error: 100\n    at a_test (/promise/async3.js:7:11)\n');
    });
});

require.main === module && test.run(console.DEBUG);