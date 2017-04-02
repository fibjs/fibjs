var test = require("test");
test.setup();

var coroutine = require("coroutine");
var os = require("os");

function timer_count() {
    var cnt = 0;
    os.memoryUsage().nativeObjects.inherits.forEach((v) => {
        if (v['class'] === 'Timer')
            cnt += v.objects;
    });
    return cnt;
}

describe("timer", () => {
    it("setTimeout", () => {
        var n = 0;

        GC();
        var no1 = timer_count();

        setTimeout(() => {
            n = 1;
        }, 1);

        GC();
        var no2 = timer_count();
        assert.equal(no1 + 1, no2);

        assert.equal(n, 0);
        for (var i = 0; i < 1000 && n == 0; i++)
            coroutine.sleep(1);
        assert.equal(n, 1);

        GC();
        no2 = timer_count();
        assert.equal(no1, no2);
    });

    it("clearTimeout", () => {
        var n = 0;

        GC();
        var no1 = timer_count();

        var t = setTimeout(() => {
            n = 1;
        }, 1);

        assert.equal(n, 0);
        clearTimeout(t);
        t = undefined;
        coroutine.sleep(100);
        assert.equal(n, 0);

        GC();
        var no2 = timer_count();
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
        var no1 = timer_count();

        setImmediate(() => {
            n = 1;
        });

        GC();
        var no2 = timer_count();
        assert.equal(no1 + 1, no2);

        assert.equal(n, 0);
        for (var i = 0; i < 1000 && n == 0; i++)
            coroutine.sleep(1);
        assert.equal(n, 1);

        GC();
        no2 = timer_count();
        assert.equal(no1, no2);
    });

    it("clearImmediate", () => {
        var n = 0;

        GC();
        var no1 = timer_count();

        var t = setImmediate(() => {
            n = 1;
        });

        assert.equal(n, 0);
        clearImmediate(t);
        t = undefined;
        coroutine.sleep(100);
        assert.equal(n, 0);

        GC();
        var no2 = timer_count();
        assert.equal(no1, no2);
    });

    it("setInterval/clearInterval", () => {
        var n = 0;

        GC();
        var no1 = timer_count();

        var t = setInterval(() => {
            if (n < 2)
                n++;
        }, 1);

        GC();
        var no2 = timer_count();
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
        var no2 = timer_count();
        assert.equal(no1, no2);
    });

    it("clearInterval in callback", () => {
        var n = 0;

        GC();
        var no1 = timer_count();

        setInterval(function() {
            n++;
            clearInterval(this);
        }, 1);

        assert.equal(n, 0);
        for (var i = 0; i < 1000 && n == 0; i++)
            coroutine.sleep(1);
        assert.equal(n, 1);

        GC();
        var no2 = timer_count();
        assert.equal(no1, no2);
    });
});

// test.run(console.DEBUG);
