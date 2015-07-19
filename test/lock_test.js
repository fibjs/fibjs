var test = require("test");
test.setup();

var coroutine = require('coroutine');

describe('lock', function() {
	it("Lock", function() {
		var l = new coroutine.Lock();

		assert.throws(function() {
			l.release();
		});

		var v = 100;

		function f() {
			l.acquire();
			v = 200;
			coroutine.sleep();
			v = 300;
			l.release();
		}

		coroutine.start(f);
		coroutine.sleep();
		l.acquire();
		assert.equal(300, v);
		l.release();
	});

	it("Semaphore", function() {
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
		coroutine.sleep();
		assert.equal(100, v);

		sem.post();
		coroutine.sleep();
		assert.equal(101, v);

		for (var i = 0; i < 10; i++)
			sem.post();
		coroutine.sleep();
		assert.equal(111, v);
		run = false;
		sem.post();
	});

	it("Condition", function() {
		var v = 0;
		var cond = new coroutine.Condition();

		assert.throws(function() {
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
		coroutine.sleep();
		cond.notify();
		coroutine.sleep();
		assert.equal(0, v);

		cond.acquire();
		v = 100;
		cond.notify();
		coroutine.sleep();
		assert.equal(200, v);
	});
});

//test.run();