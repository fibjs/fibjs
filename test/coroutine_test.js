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

		var f = coroutine.start(t_fiber, 100, 200);
		assert.equal(n, 123);
		coroutine.sleep(1);
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
		var f = coroutine.start(function(v) {}, new Buffer());
		GC();
		assert.equal(no1 + 2, os.memoryUsage().nativeObjects.objects);
		f.join();

		GC();
		assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);

		f = undefined;
		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);
	});

	it('Fiber-local storage', function() {
		function t_fiber1(v1, v2) {
			n = v1 + v2 + this.v;
		}

		n = 323;

		var f = coroutine.start(t_fiber1, 100, 200);
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

		var f = coroutine.start(t_fiber2, 100, 200);
		assert.equal(n, 1300);
		f.v = 2000;
		f.join();
		assert.equal(n, 2300);

		f = coroutine.start(t_fiber2, 100, 200);
		assert.equal(n, 2300);
		f.v = 1000;
		coroutine.sleep(10);
		f.join();
		assert.equal(n, 1300);
	});

	it('caller', function() {
		function t_fiber3(v1, v2) {
			n = v1 + v2 + this.caller.v;
		}

		n = 1300;

		var f = coroutine.start(t_fiber3, 100, 200);
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

		var f = coroutine.start(t_fiber4);
		assert.equal(n, 0);

		coroutine.current().v = 2000;

		f.join();
		assert.equal(n, 1000);
	});

	it('parallel', function() {
		var funs = [
			function() {
				coroutine.sleep(10);
				return 1;
			},
			function() {
				coroutine.sleep(10);
				return 2;
			},
			function() {
				coroutine.sleep(10);
				return 3;
			},
			function() {
				coroutine.sleep(10);
			},
			function() {
				coroutine.sleep(10);
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

		assert.deepEqual(coroutine.parallel([], function(v) {
			return 1;
		}), []);

		assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], function(v) {
			return v + 1;
		}), [2, 3, 4, 5, 6]);

		assert.deepEqual(coroutine.parallel(function(v) {
			return v + 1;
		}, 5), [1, 2, 3, 4, 5]);
	});

	it('parallel fibers limit', function() {
		var num = 0;
		var funs = [
			function() {
				var v = num++;
				coroutine.sleep(10);
				num--;
				return v;
			},
			function() {
				var v = num++;
				coroutine.sleep(10);
				num--;
				return v;
			},
			function() {
				var v = num++;
				coroutine.sleep(10);
				num--;
				return v;
			},
			function() {
				var v = num++;
				coroutine.sleep(10);
				num--;
				return v;
			},
			function() {
				var v = num++;
				coroutine.sleep(10);
				num--;
				return v;
			},
			function() {
				var v = num++;
				coroutine.sleep(10);
				num--;
				return v;
			}
		];

		var rs = coroutine.parallel(funs);
		assert.deepEqual(rs, [0, 1, 2, 3, 4, 5]);

		var rs = coroutine.parallel(funs, 2);
		assert.deepEqual(rs, [0, 1, 1, 1, 1, 1]);

		var rs = coroutine.parallel(funs, 3);
		assert.deepEqual(rs, [0, 1, 2, 2, 2, 2]);

		var rs = coroutine.parallel(funs, 4);
		assert.deepEqual(rs, [0, 1, 2, 3, 3, 3]);

		var num = 0;
		assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], function(v) {
			var n = num++;
			coroutine.sleep(10);
			num--;
			return v + n;
		}), [1, 3, 5, 7, 9]);

		var num = 0;
		assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], function(v) {
			var n = num++;
			coroutine.sleep(10);
			num--;
			return v + n;
		}, 2), [1, 3, 4, 5, 6]);

		var num = 0;
		assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], function(v) {
			var n = num++;
			coroutine.sleep(10);
			num--;
			return v + n;
		}, 3), [1, 3, 5, 6, 7]);
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
		coroutine.start(stack_size);
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

			coroutine.start(function() {
				q.put(400);
			});
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

			coroutine.start(function() {
				e = q.take();
			});
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