var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var os = require('os');

describe("console", () => {
    it("add", () => {
        console.use({
            type: "console"
        });

        if (process.platform === 'win32') {
            // win32-only case
            assert.throws(() => {
                console.use("syslog");
            }, { number: 20024 });
        } else {
            console.use("syslog");

            console.use(["console", {
                type: "syslog"
            }]);
        }

        console.reset();
    });

    it("too many logger", () => {
        console.use(["console", "console", "console", "console",
            "console", "console", "console", "console", "console", "console"
        ]);

        assert.throws(() => {
            console.use("console");
        }, { number: 20024 });

        console.reset();
    });

    it("log levels", () => {
        console.use({
            type: "console",
            levels: [console.DEBUG, console.ERROR]
        });

        // console.use({
        //     type: "console",
        //     levels: [console.DEBUG, {}]
        // });

        // console.use({
        //     type: "console",
        //     levels: [console.DEBUG, NaN]
        // });

        console.use({
            type: "console",
            levels: [console.DEBUG, '1']
        });

        assert.throws(() => {
            console.use({
                type: "console",
                levels: [console.DEBUG, 100]
            });
        }, { number: 20024 });

        console.reset();
    });

    it("add file logger", () => {
        console.use("console");

        console.use({
            type: "file",
            path: "test_log",
            split: "day",
            count: 12
        });

        assert.throws(() => {
            console.use({
                type: "file"
            });
        }, { number: 20024 });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                count: 10
            });
        }, { number: 20024 });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                count: 1
            });
        }, { number: 20024 });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                count: 129
            });
        }, { number: 20024 });

        console.use({
            type: "file",
            path: "test_log",
            split: "day"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "hour"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "minute"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "1k"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "10k"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "100k"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "100m"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "100g"
        });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                split: "100n"
            });
        }, { number: 20024 });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                split: "100n"
            });
        }, { number: 20024 });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                split: "1000k"
            });
        }, { number: 20024 });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                split: "k"
            });
        }, { number: 20024 });

        console.reset();
    });

    it("console.assert is assert", () => {
        assert.equal(console.assert, assert);
    });

    it("fix: eval scriptname crash", () => {
        eval('console.log("Rock Lee")');
    })
});


describe("console counting, grouping and time logging", () => {
    var { Console } = require('console');
    var io = require('io');

    function capture(fn) {
        var out = new io.MemoryStream();
        var c = new Console(out, out);
        fn(c);
        out.rewind();
        return out.readAll().toString();
    }

    it("count increments per instance", () => {
        var text = capture((c) => {
            c.count('items');
            c.count('items');
        });
        assert.equal(text, 'items: 1\nitems: 2\n');
    });

    it("count default label is 'default'", () => {
        assert.equal(capture((c) => c.count()), 'default: 1\n');
    });

    it("countReset restarts the counter", () => {
        var text = capture((c) => {
            c.count('x');
            c.countReset('x');
            c.count('x');
        });
        assert.equal(text, 'x: 1\nx: 1\n');
    });

    it("countReset of an unknown label warns instead of throwing", () => {
        var text = capture((c) => c.countReset('never'));
        assert.ok(text.includes("Count for 'never' does not exist"));
    });

    it("two instances keep separate counters", () => {
        var a = capture((c) => { c.count('n'); c.count('n'); });
        var b = capture((c) => c.count('n'));
        assert.equal(a, 'n: 1\nn: 2\n');
        assert.equal(b, 'n: 1\n');
    });

    it("group indents until groupEnd", () => {
        var text = capture((c) => {
            c.group('outer');
            c.log('inside');
            c.groupEnd();
            c.log('outside');
        });
        assert.equal(text, 'outer\n  inside\noutside\n');
    });

    it("groupCollapsed behaves like group", () => {
        var text = capture((c) => {
            c.groupCollapsed();
            c.log('in');
            c.groupEnd();
        });
        assert.equal(text, '  in\n');
    });

    it("groupEnd at the outermost level does nothing", () => {
        assert.equal(capture((c) => { c.groupEnd(); c.log('x'); }), 'x\n');
    });

    it("nested groups add two columns per level", () => {
        var text = capture((c) => {
            c.group();
            c.group();
            c.log('deep');
            c.groupEnd();
            c.groupEnd();
        });
        assert.equal(text, '    deep\n');
    });

    it("timeLog samples without stopping the timer", () => {
        var text = capture((c) => {
            c.time('t');
            c.timeLog('t', 'extra');
            c.timeEnd('t');
        });
        var lines = text.trim().split('\n');
        assert.equal(lines.length, 2);
        assert.ok(/^t: [\d.]+ms extra$/.test(lines[0]), lines[0]);
        assert.ok(/^t: [\d.]+ms$/.test(lines[1]), lines[1]);
    });

    it("module-level members exist", () => {
        ['count', 'countReset', 'group', 'groupEnd', 'groupCollapsed', 'timeLog'].forEach((n) => {
            assert.equal(typeof console[n], 'function', n);
        });
    });
});
