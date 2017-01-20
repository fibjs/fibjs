var test = require("test");
test.setup();

var coroutine = require("coroutine");
var os = require("os");

describe("timer", () => {
    it("setTimeout", () => {
        var n = 0;

        GC();
        var no1 = os.memoryUsage().nativeObjects.objects;

        setTimeout(() => {
            n = 1;
        }, 1);

        GC();
        var no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1 + 1, no2);

        assert.equal(n, 0);
        for (var i = 0; i < 1000 && n == 0; i++)
            coroutine.sleep(1);
        assert.equal(n, 1);

        GC();
        no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1, no2);
    });

    it("clearTimeout", () => {
        var n = 0;

        GC();
        var no1 = os.memoryUsage().nativeObjects.objects;

        var t = setTimeout(() => {
            n = 1;
        }, 1);

        assert.equal(n, 0);
        clearTimeout(t);
        t = undefined;
        coroutine.sleep(10);
        assert.equal(n, 0);

        GC();
        var no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1, no2);
    });

    it("double clearTimeout", () => {
        var t = setTimeout(() => {
            n = 1;
        }, 1);

        clearTimeout(t);
        coroutine.sleep(10);
        assert.doesNotThrow(() => {
            clearTimeout(t);
        });
    });

    it("setImmediate", () => {
        var n = 0;

        GC();
        var no1 = os.memoryUsage().nativeObjects.objects;

        setImmediate(() => {
            n = 1;
        });

        GC();
        var no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1 + 1, no2);

        assert.equal(n, 0);
        for (var i = 0; i < 1000 && n == 0; i++)
            coroutine.sleep(1);
        assert.equal(n, 1);

        GC();
        no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1, no2);
    });

    it("clearImmediate", () => {
        var n = 0;

        GC();
        var no1 = os.memoryUsage().nativeObjects.objects;

        var t = setImmediate(() => {
            n = 1;
        });

        assert.equal(n, 0);
        clearImmediate(t);
        t = undefined;
        coroutine.sleep(10);
        assert.equal(n, 0);

        GC();
        var no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1, no2);
    });

    it("setInterval/clearInterval", () => {
        var n = 0;

        GC();
        var no1 = os.memoryUsage().nativeObjects.objects;

        var t = setInterval(() => {
            if (n < 2)
                n++;
        }, 1);

        GC();
        var no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1 + 1, no2);

        assert.equal(n, 0);
        for (var i = 0; i < 1000 && n != 2; i++)
            coroutine.sleep(1);
        assert.equal(n, 2);

        n = 0;
        clearInterval(t);
        t = undefined;
        coroutine.sleep(10);
        assert.equal(n, 0);

        GC();
        var no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1, no2);
    });

    it("clearInterval in callback", () => {
        var n = 0;

        GC();
        var no1 = os.memoryUsage().nativeObjects.objects;

        setInterval(function() {
            n++;
            clearInterval(this);
        }, 1);

        assert.equal(n, 0);
        coroutine.sleep(10);
        assert.equal(n, 1);

        GC();
        var no2 = os.memoryUsage().nativeObjects.objects;
        assert.equal(no1, no2);
    });
});

// test.run(console.DEBUG);
