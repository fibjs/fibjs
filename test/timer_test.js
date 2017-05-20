var test = require("test");
test.setup();

var test_util = require('test_util');

var coroutine = require("coroutine");
var os = require("os");

describe("timer", () => {
    it("setTimeout", () => {
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
    });

    it("clearTimeout", () => {
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
    });

    it("double clearTimeout", () => {
        var t = setTimeout(() => {
            n = 1;
        }, 1);

        clearTimeout(t);
        coroutine.sleep(100);
        assert.doesNotThrow(() => {
            clearTimeout(t);
        });
    });

    it("setImmediate", () => {
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
    });

    it("clearImmediate", () => {
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
    });

    it("setInterval/clearInterval", () => {
        var n = 0;

        GC();
        var no1 = test_util.countObject('Timer');

        var t = setInterval(() => {
            if (n < 2)
                n++;
        }, 1);

        GC();
        var no2 = test_util.countObject('Timer');
        assert.equal(no1 + 1, no2);

        assert.equal(n, 0);
        for (var i = 0; i < 1000 && n != 2; i++)
            coroutine.sleep(1);
        assert.equal(n, 2);

        n = 0;
        clearInterval(t);
        t = undefined;
        coroutine.sleep(100);
        assert.equal(n, 0);

        GC();
        var no2 = test_util.countObject('Timer');
        assert.equal(no1, no2);
    });

    it("clearInterval in callback", () => {
        var n = 0;

        GC();
        var no1 = test_util.countObject('Timer');

        setInterval(function () {
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
    });
});

// test.run(console.DEBUG);