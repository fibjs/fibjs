var test = require("test");
test.setup();

var coroutine = require('coroutine');

describe('lock', () => {
    it("Lock", () => {
        var l = new coroutine.Lock();

        var v = 100;

        function f() {
            l.acquire();
            v = 200;
            coroutine.sleep(1);
            v = 300;
            l.release();
        }

        coroutine.start(f);
        coroutine.sleep(1);
        l.acquire();
        assert.equal(300, v);
        l.release();
    });

    it("async Lock", async () => {
        var l = new coroutine.Lock();

        var v = 100;

        async function f() {
            await l.acquireAsync();
            v = 200;
            await coroutine.sleepAsync(1);
            v = 300;
            l.release();
        }

        coroutine.start(f);
        coroutine.sleep(1);
        await l.acquire();
        assert.equal(300, v);
        l.release();
    });

    it("Semaphore", () => {
        var sem = new coroutine.Semaphore();

        assert.equal(true, sem.trywait());
        assert.equal(false, sem.trywait());

        setImmediate(() => {
            sem.post();
        });
        assert.equal(true, sem.wait());

        var v = 100;
        var run = true;

        function f1() {
            while (run) {
                sem.wait();
                v++;
            }
        }

        coroutine.start(f1);
        coroutine.sleep(1);
        assert.equal(100, v);

        sem.post();
        coroutine.sleep(1);
        assert.equal(101, v);

        for (var i = 0; i < 10; i++)
            sem.post();
        coroutine.sleep(1);
        assert.equal(111, v);
        run = false;
        sem.post();

        var sem1 = new coroutine.Semaphore(0);

        setImmediate(() => {
            sem1.post();
        });
        assert.equal(true, sem1.wait(10));
        assert.equal(false, sem1.wait(10));

        setImmediate(() => {
            sem1.post();
        });
        assert.equal(true, sem1.wait(10));
    });

    it("async Semaphore", async () => {
        var sem = new coroutine.Semaphore();

        assert.equal(true, sem.trywait());
        assert.equal(false, sem.trywait());

        setImmediate(() => {
            sem.post();
        });
        assert.equal(true, await sem.waitAsync());

        var v = 100;
        var run = true;

        async function f1() {
            while (run) {
                await sem.waitAsync();
                v++;
            }
        }

        coroutine.start(f1);
        await coroutine.sleepAsync(10);
        assert.equal(100, v);

        sem.post();
        await coroutine.sleepAsync(1);
        assert.equal(101, v);

        for (var i = 0; i < 10; i++)
            sem.post();
        await coroutine.sleepAsync(1);
        assert.equal(111, v);
        run = false;
        sem.post();

        var sem1 = new coroutine.Semaphore(0);

        setImmediate(() => {
            sem1.post();
        });
        assert.equal(true, await sem1.waitAsync(10));
        assert.equal(false, await sem1.waitAsync(10));

        setImmediate(() => {
            sem1.post();
        });
        assert.equal(true, await sem1.waitAsync(10));
    });

    it("Condition", () => {
        var v = 0;
        var cond = new coroutine.Condition();
        
        function f3() {
            cond.acquire();
            while (v <= 0)
                cond.wait();
            assert.equal(100, v);
            v += 100;
            cond.release();
        }

        coroutine.start(f3);

        cond.acquire();
        coroutine.sleep(1);
        cond.notify();
        coroutine.sleep(1);
        assert.equal(0, v);

        cond.acquire();
        v = 100;
        cond.notify();

        for (var i = 0; i < 100 && v === 100; i++)
            coroutine.sleep(1);
        assert.equal(200, v);
    });

    it("async Condition", async () => {
        var v = 0;
        var cond = new coroutine.Condition();

        async function f3() {
            await cond.acquireAsync();
            while (v <= 0)
                await cond.waitAsync();
            assert.equal(100, v);
            v += 100;
            cond.release();
        }

        coroutine.start(f3);

        await cond.acquireAsync();
        await coroutine.sleepAsync(1);
        cond.notify();
        await coroutine.sleepAsync(1);
        assert.equal(0, v);

        await cond.acquireAsync();
        v = 100;
        cond.notify();

        for (var i = 0; i < 100 && v === 100; i++)
            await coroutine.sleepAsync(1);
        assert.equal(200, v);
    });
});

require.main === module && test.run(console.DEBUG);