var test = require("test");
test.setup();

var utils = require('utils');
var coroutine = require('coroutine');

describe('utils', function() {
	describe('Stats', function() {
		var s;

		it("new", function() {
			s = new utils.Stats(['a'], ['b', 'c']);
			assert.deepEqual(s.toJSON(), {
				"a": 0,
				"b": 0,
				"c": 0
			});

			var s1 = new utils.Stats(['a', 'b', 'c']);
			assert.deepEqual(s1.toJSON(), {
				"a": 0,
				"b": 0,
				"c": 0
			});

			assert.throws(function() {
				var s = new utils.Stats();
			});
		});

		it("inc", function() {
			s.inc('a');
			assert.deepEqual(s.toJSON(), {
				"a": 1,
				"b": 0,
				"c": 0
			});

			s.inc('c');
			assert.deepEqual(s.toJSON(), {
				"a": 1,
				"b": 0,
				"c": 1
			});
		});

		it("dec", function() {
			s.dec('a');
			assert.deepEqual(s.toJSON(), {
				"a": 0,
				"b": 0,
				"c": 1
			});
		});

		it("add", function() {
			s.add('a', 100);
			assert.deepEqual(s.toJSON(), {
				"a": 100,
				"b": 0,
				"c": 1
			});
		});

		it("reset", function() {
			s.reset();
			assert.deepEqual(s.toJSON(), {
				"a": 100,
				"b": 0,
				"c": 0
			});
		});

		it("named", function() {
			assert.equal(s.a, 100);
			assert.equal(s["a"], 100);
		});

		it("uptime", function() {
			coroutine.sleep(100);
			assert.closeTo(100, s.uptime(), 10);
		});
	});

	describe('LruCache', function() {
		var c;

		function deepEqual(o1, o2) {
			assert.deepEqual(Object.keys(o1), Object.keys(o2));
			assert.deepEqual(o1, o2);
		}

		it("new", function() {
			c = new utils.LruCache(3, 100);
			deepEqual(c.toJSON(), {});
		});

		it("put", function() {
			c.put("b", 100);
			deepEqual(c.toJSON(), {
				"b": 100
			});

			c.put({
				"b": 200,
				"a": 100
			});
			deepEqual(c.toJSON(), {
				"a": 100,
				"b": 200
			});
		});

		it("set", function() {
			c.set("c", 100);
			deepEqual(c.toJSON(), {
				"a": 100,
				"b": 200
			});
		});

		it("has", function() {
			assert.isTrue(c.has('a'));
			assert.isFalse(c.has('c'));
		});

		it("get", function() {
			assert.equal(c.get('a'), 100);
			assert.isUndefined(c.get('c'));

			deepEqual(c.toJSON(), {
				"a": 100,
				"b": 200
			});

			assert.equal(c.get('b'), 200);
			deepEqual(c.toJSON(), {
				"b": 200,
				"a": 100
			});
		});

		it("remove", function() {
			c.remove("b");
			deepEqual(c.toJSON(), {
				"a": 100
			});

			c.remove('a');
			deepEqual(c.toJSON(), {});
		});

		it("size", function() {
			assert.equal(c.size, 0);

			c.put('a', 100);
			assert.equal(c.size, 1);

			c.put('b', 100);
			c.put('c', 200);
			c.put('d', 300);
			c.put('e', 400);

			assert.equal(c.size, 3);

			deepEqual(c.toJSON(), {
				"e": 400,
				"d": 300,
				"c": 200
			});
		});

		it("timeout", function() {
			coroutine.sleep(50);
			c.put('f', 500);
			deepEqual(c.toJSON(), {
				"f": 500,
				"e": 400,
				"d": 300
			});

			coroutine.sleep(51);
			deepEqual(c.toJSON(), {
				"f": 500
			});
		});
	});
});

// test.run();