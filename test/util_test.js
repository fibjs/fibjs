var test = require("test");
test.setup();

var util = require('util');
var coroutine = require('coroutine');

describe('util', function() {

	it("isArray", function() {
		assert.isTrue(util.isArray([]));
		assert.isTrue(util.isArray(Array()));
		assert.isTrue(util.isArray(new Array()));
		assert.isTrue(util.isArray(new Array(5)));
		assert.isTrue(util.isArray(new Array('with', 'some', 'entries')));
		assert.isFalse(util.isArray({}));
		assert.isFalse(util.isArray({
			push: function() {}
		}));
		assert.isFalse(util.isArray(/regexp/));
		assert.isFalse(util.isArray(new Error));
		assert.isFalse(util.isArray(Object.create(Array.prototype)));
	});

	it("isRegExp", function() {
		assert.isTrue(util.isRegExp(/regexp/));
		assert.isTrue(util.isRegExp(RegExp()));
		assert.isTrue(util.isRegExp(new RegExp()));
		assert.isFalse(util.isRegExp({}));
		assert.isFalse(util.isRegExp([]));
		assert.isFalse(util.isRegExp(new Date()));
		assert.isFalse(util.isRegExp(Object.create(RegExp.prototype)));
	});

	it("isObject", function() {
		assert.isTrue(util.isObject({}));
	});

	it('isNull', function() {
		assert.isTrue(util.isNull(null));
		assert.isFalse(util.isNull(undefined));
		assert.isFalse(util.isNull(100));
	});

	it('isUndefined', function() {
		assert.isFalse(util.isUndefined(null));
		assert.isTrue(util.isUndefined(undefined));
		assert.isFalse(util.isUndefined(100));
	});

	it('isNullOrUndefined', function() {
		assert.isTrue(util.isNullOrUndefined(null));
		assert.isTrue(util.isNullOrUndefined(undefined));
		assert.isFalse(util.isNullOrUndefined(100));
	});

	it('isNumber', function() {
		assert.isTrue(util.isNumber(1));
		assert.isTrue(util.isNumber(Number('3')));

		assert.isFalse(util.isNumber('1'));

		assert.isFalse(util.isNumber('hello'));
		assert.isFalse(util.isNumber([5]));
	});

	it('isString', function() {
		assert.isTrue(util.isString('Foo'));
		assert.isTrue(util.isString(new String('foo')));

		assert.isFalse(util.isString(1));

		assert.isFalse(util.isString(3));
		assert.isFalse(util.isString(['hello']));
	});

	it('isRegExp', function() {
		assert.isTrue(util.isRegExp(/a/));
		assert.isTrue(util.isRegExp(new RegExp("a")));
		assert.isFalse(util.isRegExp("a"));
	});

	it('isFunction', function() {
		assert.isTrue(util.isFunction(function() {}));
		assert.isFalse(util.isFunction({}));
		assert.isFalse(util.isFunction(5));
	});

	it('isBuffer', function() {
		assert.isTrue(util.isBuffer(new Buffer(10)));
		assert.isFalse(util.isBuffer({}));
		assert.isFalse(util.isBuffer(5));
	});

	describe('format', function() {
		it("basic", function() {
			assert.equal(util.format(), '');
			assert.equal(util.format(''), '');
			assert.equal(util.format([]), '[]');
			assert.equal(util.format({}), '{}');
			assert.equal(util.format(null), 'null');
			assert.equal(util.format(true), 'true');
			assert.equal(util.format(false), 'false');
			assert.equal(util.format('test'), 'test');

			// CHECKME this is for console.log() compatibility - but is it *right*?
			assert.equal(util.format('foo', 'bar', 'baz'), 'foo bar baz');
		});

		it("%d", function() {
			assert.equal(util.format('%d', 42.0), '42');
			assert.equal(util.format('%d', 42), '42');
			assert.equal(util.format('%s', 42), '42');
			assert.equal(util.format('%j', 42), '42');

			assert.equal(util.format('%d', '42.0'), '42');
			assert.equal(util.format('%d', '42'), '42');
			assert.equal(util.format('%s', '42'), '42');
		});

		it("%j", function() {
			assert.equal(util.format('%j', '42'), '"42"');
		});

		it("%s", function() {
			assert.equal(util.format('%%s%s', 'foo'), '%sfoo');

			assert.equal(util.format('%s'), '%s');
			assert.equal(util.format('%s', undefined), 'undefined');
			assert.equal(util.format('%s', 'foo'), 'foo');
			assert.equal(util.format('%s:%s'), '%s:%s');
			assert.equal(util.format('%s:%s', undefined), 'undefined:%s');
			assert.equal(util.format('%s:%s', 'foo'), 'foo:%s');
			assert.equal(util.format('%s:%s', 'foo', 'bar'), 'foo:bar');
			assert.equal(util.format('%s:%s', 'foo', 'bar', 'baz'), 'foo:bar baz');
			assert.equal(util.format('%%%s%%', 'hi'), '%hi%');
			assert.equal(util.format('%%%s%%%%', 'hi'), '%hi%%');
		});

		it("Circular", function() {
			(function() {
				var o = {};
				o.o = o;
				assert.equal(util.format('%j', o), '[Circular]');
			})();
		});

		it("Function", function() {
			assert.equal(util.format('%j', function() {}), '[Function]');
		});
	});

	describe('Stats', function() {
		var s;

		it("new", function() {
			s = new util.Stats(['a'], ['b', 'c']);
			assert.deepEqual(s.toJSON(), {
				"a": 0,
				"b": 0,
				"c": 0
			});

			var s1 = new util.Stats(['a', 'b', 'c']);
			assert.deepEqual(s1.toJSON(), {
				"a": 0,
				"b": 0,
				"c": 0
			});

			assert.throws(function() {
				var s = new util.Stats();
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
			assert.closeTo(100, s.uptime(), 40);
		});
	});

	describe('LruCache', function() {
		var c;

		function deepEqual(o1, o2) {
			assert.deepEqual(Object.keys(o1), Object.keys(o2));
			assert.deepEqual(o1, o2);
		}

		it("new", function() {
			c = new util.LruCache(3, 100);
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

//test.run(console.DEBUG);