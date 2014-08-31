var test = require("test");
test.setup();

var os = require('os');
var coroutine = require('coroutine');

var n;

describe('coroutine', function() {
	function t_fiber(v1, v2) {
		n = v1 + v2;
	}

	it('start', function() {
		n = 123;

		var f = t_fiber.start(100, 200);
		assert.equal(n, 123);
		coroutine.sleep();
		assert.equal(n, 300);

	});

	it('join', function() {
		n = 300;

		var f = coroutine.start(t_fiber, 123, 200);
		assert.equal(n, 300);
		f.join();
		assert.equal(n, 323);
	});

	it("Memory Leak detect", function() {
		GC();
		var no1 = os.memoryUsage().nativeObjects.objects;
		var f = (function(v) {}).start(new Buffer());
		GC();
		assert.equal(no1 + 2, os.memoryUsage().nativeObjects.objects);
		f.join();
		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);
	});

	it('Fiber-local storage', function() {
		function t_fiber1(v1, v2) {
			n = v1 + v2 + this.v;
		}

		n = 323;

		var f = t_fiber1.start(100, 200);
		assert.equal(n, 323);
		f.v = 1000;
		f.join();
		assert.equal(n, 1300);
	});

	it('current', function() {
		function t_fiber2(v1, v2) {
			n = v1 + v2 + coroutine.current().v;
		}

		n = 1300;

		var f = t_fiber2.start(100, 200);
		assert.equal(n, 1300);
		f.v = 2000;
		f.join();
		assert.equal(n, 2300);

		f = t_fiber2.start(100, 200);
		assert.equal(n, 2300);
		f.v = 1000;
		coroutine.sleep(100);
		f.join();
		assert.equal(n, 1300);
	});

	it('caller', function() {
		function t_fiber3(v1, v2) {
			n = v1 + v2 + this.caller.v;
		}

		n = 1300;

		var f = t_fiber3.start(100, 200);
		assert.equal(n, 1300);
		coroutine.current().v = 1234;
		f.join();
		assert.equal(n, 1534);
	});

	it('inherit local storage', function() {
		function t_fiber4() {
			n = coroutine.current().v;
		}

		n = 0;

		coroutine.current().v = 1000;

		var f = t_fiber4.start();
		assert.equal(n, 0);

		coroutine.current().v = 2000;

		f.join();
		assert.equal(n, 1000);
	});

	it('parallel', function() {
		var funs = [

			function() {
				coroutine.sleep(100);
				return 1;
			},
			function() {
				coroutine.sleep(100);
				return 2;
			},
			function() {
				coroutine.sleep(100);
				return 3;
			},
			function() {
				coroutine.sleep(100);
			},
			function() {
				coroutine.sleep(100);
			}
		];

		var rs = coroutine
			.parallel(funs[0], funs[1], funs[2], funs[3], funs[4]);
		assert.equal(rs[0], 1);
		assert.equal(rs[1], 2);
		assert.equal(rs[2], 3);

		rs = coroutine.parallel(funs);
		assert.equal(rs[0], 1);
		assert.equal(rs[1], 2);
		assert.equal(rs[2], 3);

		assert.throws(function() {
			coroutine.parallel(funs[0], funs[1], funs[2], funs[3], funs[4],
				function() {
					console.log(notExistsValue);
				});
		});

		assert.throws(function() {
			coroutine.parallel(function() {
				console.log(notExistsValue);
			}, funs[0], funs[1], funs[2], funs[3], funs[4]);
		});

		assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], function(v) {
			return v + 1;
		}), [2, 3, 4, 5, 6]);
	});

	it('stack overflow', function() {
		function stack_size() {
			function t() {
				t();
			}

			assert.throws(function() {
				t();
			});
		}

		stack_size.start();
		coroutine.sleep();
	});

	describe('BlockQueue', function() {
		it("add", function() {
			var q = new coroutine.BlockQueue(3);

			assert.equal(q.add(100), true);
			assert.equal(q.add(200), true);
			assert.equal(q.add(300), true);
			assert.deepEqual(q.toArray(), [100, 200, 300]);

			assert.throws(function() {
				q.add(400);
			});
			assert.deepEqual(q.toArray(), [100, 200, 300]);
		});

		it("offer", function() {
			var q = new coroutine.BlockQueue(3);

			assert.equal(q.offer(100), true);
			assert.equal(q.offer(200), true);
			assert.equal(q.offer(300), true);
			assert.deepEqual(q.toArray(), [100, 200, 300]);

			assert.equal(q.offer(400), false);
			assert.deepEqual(q.toArray(), [100, 200, 300]);
		});

		it("remove", function() {
			var q = new coroutine.BlockQueue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.remove(), 100);
			assert.deepEqual(q.toArray(), [200]);

			assert.equal(q.remove(), 200);
			assert.deepEqual(q.toArray(), []);

			assert.throws(function() {
				q.remove();
			});
			assert.deepEqual(q.toArray(), []);
		});

		it("poll", function() {
			var q = new coroutine.BlockQueue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.poll(), 100);
			assert.deepEqual(q.toArray(), [200]);

			assert.equal(q.poll(), 200);
			assert.deepEqual(q.toArray(), []);

			assert.isNull(q.poll());
			assert.deepEqual(q.toArray(), []);
		});

		it("clear/length", function() {
			var q = new coroutine.BlockQueue(3);

			q.add(100);
			q.add(200);
			assert.deepEqual(q.toArray(), [100, 200]);

			q.clear();
			assert.deepEqual(q.toArray(), []);
			assert.equal(q.length, 0);
		});

		it("element", function() {
			var q = new coroutine.BlockQueue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.element(), 100);
			assert.deepEqual(q.toArray(), [100, 200]);

			q.clear();
			assert.throws(function() {
				q.element();
			});
		});

		it("peek", function() {
			var q = new coroutine.BlockQueue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.element(), 100);
			assert.deepEqual(q.toArray(), [100, 200]);

			q.clear();
			assert.isNull(q.peek());
		});

		it("put", function() {
			var q = new coroutine.BlockQueue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.element(), 100);
			assert.deepEqual(q.toArray(), [100, 200]);

			q.put(300);
			assert.deepEqual(q.toArray(), [100, 200, 300]);

			(function() {
				q.put(400);
			}).start();
			coroutine.sleep(10);
			assert.deepEqual(q.toArray(), [100, 200, 300]);
			q.remove();
			assert.deepEqual(q.toArray(), [200, 300]);
			coroutine.sleep(10);
			assert.deepEqual(q.toArray(), [200, 300, 400]);
		});

		it("take", function() {
			var q = new coroutine.BlockQueue(3);
			var e;

			(function() {
				e = q.take();
			}).start();
			coroutine.sleep(10);

			q.add(100);
			q.add(200);
			assert.deepEqual(q.toArray(), [100, 200]);

			coroutine.sleep(10);
			assert.equal(e, 100);
			assert.deepEqual(q.toArray(), [200]);
		});
	});
});

// test.run(console.DEBUG);