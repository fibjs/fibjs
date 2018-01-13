var test = require("test");
test.setup();

var test_util = require('./test_util');

var coroutine = require("coroutine");
var timers = require("timers");

var os = require("os");

describe("timer", () => {
    describe("setTimeout/clearTimeout", () => {
        function test(setTimeout, clearTimeout) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            var t = setTimeout((a) => n = a, 1, 1);

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no2);

            assert.equal(n, 0);
            for (var i = 0; i < 1000 && n == 0; i++)
                coroutine.sleep(10);

            assert.equal(n, 1);
            assert.isTrue(t.stopped);
            t = undefined;

            GC();
            no2 = test_util.countObject('Timer');
            assert.equal(no1, no2);

            setTimeout((a) => n = a, Math.pow(2, 31), 3);

            GC();
            var no3 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no3);
            assert.equal(n, 1);

            for (var i = 0; i < 1000 && n == 1; i++)
                coroutine.sleep(10);

            assert.equal(n, 3);

            var t = setTimeout(() => n = 5, Math.pow(2, 31) - 1);

            coroutine.sleep(100);
            assert.equal(n, 3);

            GC();
            var no4 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no4);
            clearTimeout(t);
            assert.isTrue(t.stopped);
            t = undefined;

            coroutine.sleep(200);
            GC();

            no4 = test_util.countObject('Timer');
            assert.equal(no1, no4);
        }

        it("global setTimeout/global clearTimeout", () => {
            test(setTimeout, clearTimeout);
        });

        it("global.setTimeout/global.clearTimeout", () => {
            test(global.setTimeout, global.clearTimeout);
        });

        it("timers.setTimeout/timers.clearTimeout", () => {
            test(timers.setTimeout, timers.clearTimeout);
        });
    });

    describe("double clearTimeout", () => {
        function test(setTimeout, clearTimeout) {
            var t = setTimeout(() => n = 1, 1);

            clearTimeout(t);
            coroutine.sleep(100);
            assert.doesNotThrow(() => clearTimeout(t));
        }

        it("global clearTimeout", () => {
            test(setTimeout, clearTimeout);
        });

        it("global.clearTimeout", () => {
            test(global.setTimeout, global.clearTimeout);
        });

        it("timers.clearTimeout", () => {
            test(timers.setTimeout, timers.clearTimeout);
        });
    });

    describe("setImmediate", () => {
        function test(setImmediate) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            setImmediate((a) => n = a, 1);

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

            var t = setImmediate(() => n = 1);

            assert.equal(n, 0);
            clearImmediate(t);
            assert.isTrue(t.stopped);
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
            var n1 = 0;
            var n2 = 0;
            var n3 = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            var t1 = setInterval(() => n1++, 50);
            coroutine.sleep(10);
            clearInterval(t1);
            assert.isTrue(t1.stopped);
            t1 = undefined;

            var t2 = setInterval(() => n2++, Math.pow(2, 31));
            coroutine.sleep(100);
            clearInterval(t2);
            assert.isTrue(t2.stopped);
            t2 = undefined;

            var t3 = setInterval(() => n3++, Math.pow(2, 31) - 1);

            assert.equal(n1, 0);
            assert.greaterThan(n2, 0);
            assert.equal(n3, 0);
            coroutine.sleep(100);
            assert.equal(n1, 0);
            assert.greaterThan(n2, 0);
            assert.equal(n3, 0);

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no2);

            clearInterval(t3);
            assert.isTrue(t3.stopped);
            t3 = undefined;
            coroutine.sleep(100);

            GC();
            var no3 = test_util.countObject('Timer');
            assert.equal(no2 - 1, no3);
        }

        it("global setInterval/global clearInterval", () => {
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
        function test(setInterval, clearInterval) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            setInterval(function () {
                n++;
                clearInterval(this);
                assert.isTrue(this.stopped);
            }, 1);

            setInterval(function () {
                n++;
                clearInterval(this);
                assert.isTrue(this.stopped);
            }, Math.pow(2, 31));

            var t = setInterval(() => n++, Math.pow(2, 31) - 1);

            assert.equal(n, 0);
            coroutine.sleep(100);
            assert.equal(n, 2);

            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no2);

            clearInterval(t);
            t = undefined;

            coroutine.sleep(100);

            GC();
            var no3 = test_util.countObject('Timer');
            assert.equal(no2 - 1, no3);
        }

        it("global clearInterval", () => {
            test(setInterval, clearInterval);
        });

        it("global.clearInterval", () => {
            test(global.setInterval, global.clearInterval);
        });

        it("timers.clearInterval", () => {
            test(timers.setInterval, timers.clearInterval);
        });
    });

    function deadLoop(ms) {
        var tm = new Date().getTime() + ms;
        while (new Date().getTime() < tm);
    }

    describe("setHrInterval/clearHrInterval", () => {
        function test(setHrInterval, clearHrInterval) {
            var n1 = 0;
            var n2 = 0;
            var n3 = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            var t1 = setHrInterval(() => n1++, 50);
            deadLoop(10);
            clearHrInterval(t1);
            assert.isTrue(t1.stopped);
            t1 = undefined;

            var t2 = setHrInterval(() => n2++, Math.pow(2, 31));
            deadLoop(100);
            clearHrInterval(t2);
            t2 = undefined;

            var t3 = setHrInterval(() => n3++, Math.pow(2, 31) - 1);

            assert.equal(n1, 0);
            assert.greaterThan(n2, 0);
            assert.equal(n3, 0);
            deadLoop(100);
            assert.equal(n1, 0);
            assert.greaterThan(n2, 0);
            assert.equal(n3, 0);

            coroutine.sleep(10);
            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no2);

            clearHrInterval(t3);
            t3 = undefined;
            deadLoop(100);

            coroutine.sleep(10);
            GC();
            var no3 = test_util.countObject('Timer');
            assert.equal(no2 - 1, no3);
        }

        it("global setHrInterval/global clearHrInterval", () => {
            test(setHrInterval, clearHrInterval);
        });

        it("global.setHrInterval/global.clearHrInterval", () => {
            test(global.setHrInterval, global.clearHrInterval);
        });

        it("timers.setHrInterval/timers.clearHrInterval", () => {
            test(timers.setHrInterval, timers.clearHrInterval);
        });
    });

    describe("clearHrInterval in callback", () => {
        function test(setHrInterval, clearHrInterval) {
            var n = 0;

            GC();
            var no1 = test_util.countObject('Timer');

            setHrInterval(function () {
                n++;
                clearHrInterval(this);
                assert.isTrue(this.stopped);
            }, 1);

            setHrInterval(function () {
                n++;
                clearHrInterval(this);
                assert.isTrue(this.stopped);
            }, Math.pow(2, 31));

            var t = setHrInterval(() => n++, Math.pow(2, 31) - 1);

            assert.equal(n, 0);
            deadLoop(100);
            assert.equal(n, 2);

            coroutine.sleep(10);
            GC();
            var no2 = test_util.countObject('Timer');
            assert.equal(no1 + 1, no2);

            clearHrInterval(t);
            t = undefined;

            deadLoop(100);

            coroutine.sleep(10);
            GC();
            var no3 = test_util.countObject('Timer');
            assert.equal(no2 - 1, no3);
        }

        it("global clearHrInterval", () => {
            test(setHrInterval, clearHrInterval);
        });

        it("global.clearHrInterval", () => {
            test(global.setHrInterval, global.clearHrInterval);
        });

        it("timers.clearHrInterval", () => {
            test(timers.setHrInterval, timers.clearHrInterval);
        });
    });
});

require.main === module && test.run(console.DEBUG);