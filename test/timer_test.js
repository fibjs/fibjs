var test = require("test");
test.setup();

var test_util = require('test_util');

var coroutine = require("coroutine");
var timers = require("timers");

var os = require("os");

describe("timer", () => {
    describe("setTimeout", () => {
        function test(setTimeout) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            setTimeout(() => {
                n = 1;
            }, 1);

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no2);

            assert.equal(n, 0);
            for (var i = 0; i < 1000 && n == 0; i++)
                coroutine.sleep(1);
            assert.equal(n, 1);

            GC();
            no2 = test_util.countObject('Timer');
            assert.equal(no1, no2);
        }

        it("global setTimeout", () => {
            test(setTimeout);
        });

        it("global.setTimeout", () => {
            test(global.setTimeout);
        });

        it("timers.setTimeout", () => {
            test(timers.setTimeout);
        });
    });

    describe("clearTimeout", () => {
        function test(setTimeout) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            var t = setTimeout(() => {
                n = 1;
            }, 1);

            assert.equal(n, 0);
            clearTimeout(t);
            t = undefined;
            coroutine.sleep(100);
            assert.equal(n, 0);

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1, no2);
        }

        it("global clearTimeout", () => {
            test(setTimeout);
        });

        it("global.clearTimeout", () => {
            test(global.setTimeout);
        });

        it("timers.clearTimeout", () => {
            test(timers.setTimeout);
        });
    });

    describe("double clearTimeout", () => {
        function test(setTimeout) {
            var t = setTimeout(() => {
                n = 1;
            }, 1);

            clearTimeout(t);
            coroutine.sleep(100);
            assert.doesNotThrow(() => {
                clearTimeout(t);
            });
        }

        it("double global clearTimeout", () => {
            test(setTimeout);
        });

        it("double global.clearTimeout", () => {
            test(global.setTimeout);
        });

        it("double timers.clearTimeout", () => {
            test(timers.setTimeout);
        });
    });

    describe("setImmediate", () => {
        function test(setImmediate) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            setImmediate(() => {
                n = 1;
            });

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no2);

            assert.equal(n, 0);
            for (var i = 0; i < 1000 && n == 0; i++)
                coroutine.sleep(1);
            assert.equal(n, 1);

            GC();
            no2 = test_util.countObject('Timer');
            assert.equal(no1, no2);
        }

        it("global setImmediate", () => {
            test(setImmediate);
        });

        it("global.setImmediate", () => {
            test(global.setImmediate);
        });

        it("timers.setImmediate", () => {
            test(timers.setImmediate);
        });
    });

    describe("clearImmediate", () => {
        function test(clearImmediate) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            var t = setImmediate(() => {
                n = 1;
            });

            assert.equal(n, 0);
            clearImmediate(t);
            t = undefined;
            coroutine.sleep(100);
            assert.equal(n, 0);

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1, no2);
        }

        it("global clearImmediate", () => {
            test(clearImmediate);
        });

        it("global.clearImmediate", () => {
            test(global.clearImmediate);
        });

        it("timers.clearImmediate", () => {
            test(timers.clearImmediate);
        });
    });

    describe("setInterval/clearInterval", () => {
        function test(setInterval, clearInterval) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            var t = setImmediate(() => {
                n = 1;
            });

            assert.equal(n, 0);
            clearImmediate(t);
            t = undefined;
            coroutine.sleep(100);
            assert.equal(n, 0);

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1, no2);
        }

        it("global setInterval/clearInterval", () => {
            test(setInterval, clearInterval);
        });

        it("global.setInterval/global.clearInterval", () => {
            test(global.setInterval, global.clearInterval);
        });

        it("timers.setInterval/timers.clearInterval", () => {
            test(timers.setInterval, timers.clearInterval);
        });
    });

    describe("clearInterval in callback", () => {
        function test(clearInterval) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            setInterval(function() {
                n++;
                clearInterval(this);
            }, 1);

            assert.equal(n, 0);
            for (var i = 0; i < 1000 && n == 0; i++)
                coroutine.sleep(1);
            assert.equal(n, 1);

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1, no2);
        }

        it("global setInterval", () => {
            test(clearInterval);
        });

        it("global.clearInterval", () => {
            test(global.clearInterval);
        });

        it("timers.clearInterval", () => {
            test(timers.clearInterval);
        });
    });
});

// test.run(console.DEBUG);