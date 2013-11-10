var test = require("test");
test.setup();

var os = require('os');
var coroutine = require('coroutine');

var n;

describe('fiber', function() {
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

	it('onexit', function() {
		n = 0;
		function t_fiber4(a1, a2) {
			return a1 + a2 + 123;
		}

		t_fiber4.start(100, 200).onexit(function(r) {
			n = r + 1000;
		});
		coroutine.sleep();
		assert.equal(n, 1423);
	});

	it('on[exit, error]', function() {
		function t_fiber5(a1, a2) {
			return a1 + a2 + 123 + notExistsValue;
		}

		var n1 = 0;
		n = 0;

		t_fiber5.start(100, 200).on({
			exit : function(r) {
				assert.equal(r, undefined);
				n = 1000;
			},
			error : function(e) {
				n1 = 2000;
			}
		});
		coroutine.sleep();
		assert.equal(n, 1000);
		assert.equal(n1, 2000);
	});

	it('parallel', function() {
		var funs = [ function() {
			coroutine.sleep(100);
			return 1;
		}, function() {
			coroutine.sleep(100);
			return 2;
		}, function() {
			coroutine.sleep(100);
			return 3;
		}, function() {
			coroutine.sleep(100);
		}, function() {
			coroutine.sleep(100);
		} ];

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

		assert.deepEqual(coroutine.parallel([ 1, 2, 3, 4, 5 ], function(v) {
			return v + 1;
		}), [ 2, 3, 4, 5, 6 ]);
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
});

//test.run();
