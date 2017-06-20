var test = require("test");
test.setup();

var coroutine = require('coroutine');

describe('lock', () => {
    it("Lock", () => {
        var l = new coroutine.Lock();

        assert.throws(() => {
            l.release();
        });

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

    it("Semaphore", () => {
        var sem = new coroutine.Semaphore();

        assert.equal(true, sem.trywait());
        assert.equal(false, sem.trywait());

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
    });

    it("Condition", () => {
        var v = 0;
        var cond = new coroutine.Condition();

        assert.throws(() => {
            cond.wait();
        });

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
        coroutine.sleep(1);
        assert.equal(200, v);
    });
});

argv.length && test.run(console.DEBUG);
