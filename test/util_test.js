var test = require("test");
test.setup();

var util = require('util');
var mq = require('mq');
var coroutine = require('coroutine');
var collection = require("collection");
var os = require('os');

describe('util', function() {

	it("isEmpty", function() {
		assert.isTrue(util.isEmpty(null));
		assert.isTrue(util.isEmpty(undefined));

		assert.isTrue(util.isEmpty([]));
		assert.isFalse(util.isEmpty([100]));

		assert.isTrue(util.isEmpty(""));
		assert.isFalse(util.isEmpty("a"));

		assert.isTrue(util.isEmpty({}));
		assert.isFalse(util.isEmpty({
			a: 100
		}));
	});

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

	it('has', function() {
		var obj = {
			foo: 'bar',
			func: function() {}
		};
		assert.ok(util.has(obj, 'foo'));
		assert.ok(!util.has(obj, 'baz'));
		assert.ok(util.has(obj, 'func'));
		obj.hasOwnProperty = null;
		assert.ok(util.has(obj, 'foo'));
		var child = {};
		child.prototype = obj;
		assert.ok(!util.has(child, 'foo'));
		assert.strictEqual(util.has(null, 'foo'), false);
		assert.strictEqual(util.has(undefined, 'foo'), false);
	});

	it('keys', function() {
		assert.deepEqual(util.keys({
			one: 1,
			two: 2
		}), ['one', 'two'], 'can extract the keys from an object');
		// the test above is not safe because it relies on for-in enumeration order
		var a = [];
		a[1] = 0;
		assert.deepEqual(util.keys(a), ['1']);
		assert.deepEqual(util.keys(null), []);
		assert.deepEqual(util.keys(void 0), []);
		assert.deepEqual(util.keys(1), []);
		assert.deepEqual(util.keys('a'), []);
		assert.deepEqual(util.keys(true), []);
	});

	it('values', function() {
		assert.deepEqual(util.values({
			one: 1,
			two: 2
		}), [1, 2], 'can extract the values from an object');
		assert.deepEqual(util.values({
			one: 1,
			two: 2,
			length: 3
		}), [1, 2, 3], '... even when one of them is "length"');
	});

	it('clone', function() {
		assert.equal(util.clone(100), 100);

		var a = [100, 200];
		var a1 = util.clone(a);
		assert.notEqual(a, a1);
		assert.deepEqual(a, a1);
		a[0] = 150;
		assert.notDeepEqual(a, a1);

		var o = {
			a: 100,
			b: 200
		};
		var o1 = util.clone(o);
		assert.notEqual(o, o1);
		assert.deepEqual(o, o1);
		o["a"] = 150;
		assert.notDeepEqual(o, o1);
	});

	it('extend', function() {
		var result;
		assert.equal(util.extend({}, {
			a: 'b'
		}).a, 'b', 'can extend an object with the attributes of another');
		assert.equal(util.extend({
			a: 'x'
		}, {
			a: 'b'
		}).a, 'b', 'properties in source override destination');
		assert.equal(util.extend({
			x: 'x'
		}, {
			a: 'b'
		}).x, 'x', "properties not in source don't get overriden");
		result = util.extend({
			x: 'x'
		}, {
			a: 'a'
		}, {
			b: 'b'
		});
		assert.deepEqual(result, {
			x: 'x',
			a: 'a',
			b: 'b'
		}, 'can extend from multiple source objects');
		result = util.extend({
			x: 'x'
		}, {
			a: 'a',
			x: 2
		}, {
			a: 'b'
		});
		assert.deepEqual(result, {
			x: 2,
			a: 'b'
		}, 'extending from multiple source objects last property trumps');
		result = util.extend({}, {
			a: void 0,
			b: null
		});
		assert.deepEqual(util.keys(result), ['a', 'b'], 'extend copies undefined values');

		try {
			result = {};
			util.extend(result, null, undefined, {
				a: 1
			});
		} catch (ex) {}

		assert.equal(result.a, 1, 'should not error on `null` or `undefined` sources');

		assert.strictEqual(util.extend(null, {
			a: 1
		}), null, 'extending null results in null');
		assert.strictEqual(util.extend(undefined, {
			a: 1
		}), undefined, 'extending undefined results in undefined');
	});

	it("first", function() {
		assert.equal(util.first([1, 2, 3]), 1);
		assert.deepEqual(util.first([1, 2, 3], 0), []);
		assert.deepEqual(util.first([1, 2, 3], 2), [1, 2]);
		assert.deepEqual(util.first([1, 2, 3], 5), [1, 2, 3]);

		var result = (function() {
			return util.first([1, 2, 3], 2);
		}());
		assert.deepEqual(result, [1, 2]);

		assert.equal(util.first(null), undefined);
		assert.strictEqual(util.first([1, 2, 3], -1).length, 0);
	});

	it("last", function() {
		assert.equal(util.last([1, 2, 3]), 3);
		assert.deepEqual(util.last([1, 2, 3], 0), []);
		assert.deepEqual(util.last([1, 2, 3], 2), [2, 3]);
		assert.deepEqual(util.last([1, 2, 3], 5), [1, 2, 3]);

		assert.equal(util.last(null), undefined);
		assert.strictEqual(util.last([1, 2, 3], -1).length, 0);
	});

	it('unique', function() {
		var list = [1, 2, 1, 3, 1, 4];
		assert.deepEqual(util.unique(list), [1, 2, 3, 4]);

		list = [1, 1, 1, 2, 2, 3];
		assert.deepEqual(util.unique(list, true), [1, 2, 3]);

		list = [{
			name: 'moe'
		}, {
			name: 'curly'
		}, {
			name: 'larry'
		}, {
			name: 'curly'
		}];
		var iterator = function(value) {
			return value.name;
		};

		var a = {},
			b = {},
			c = {};
		assert.deepEqual(util.unique([a, b, a, b, c]), [a, b, c]);
		assert.deepEqual(util.unique(null), []);
	});

	it('union', function() {
		var result = util.union([1, 2, 3], [2, 30, 1], [1, 40]);
		assert.deepEqual(result, [1, 2, 3, 30, 40]);

		result = util.union([1, 2, 3], [2, 30, 1], [1, 40, [1]]);
		assert.deepEqual(result, [1, 2, 3, 30, 40, [1]]);
	});

	it('flatten', function() {
		var list = [1, [2],
			[3, [
				[
					[4]
				]
			]]
		];
		assert.deepEqual(util.flatten(list), [1, 2, 3, 4]);
		assert.deepEqual(util.flatten(list, true), [1, 2, 3, [
			[
				[4]
			]
		]]);
		var result = (function() {
			return util.flatten(arguments);
		}(1, [2], [3, [
			[
				[4]
			]
		]]));
		assert.deepEqual(result, [1, 2, 3, 4]);
		list = [
			[1],
			[2],
			[3],
			[
				[4]
			]
		];
		assert.deepEqual(util.flatten(list, true), [1, 2, 3, [4]]);
	});

	it('without', function() {
		var list = [1, 2, 1, 0, 3, 1, 4];
		assert.deepEqual(util.without(list, 0, 1), [2, 3, 4]);

		var result = (function() {
			return util.without(arguments, 0, 1);
		}(1, 2, 1, 0, 3, 1, 4));
		assert.deepEqual(result, [2, 3, 4]);

		list = [{
			one: 1
		}, {
			two: 2
		}];
		assert.equal(util.without(list, {
			one: 1
		}).length, 2);
		assert.equal(util.without(list, list[0]).length, 1);
	});

	it('difference', function() {
		var result = util.difference([1, 2, 3], [2, 30, 40]);
		assert.deepEqual(result, [1, 3]);

		result = util.difference([1, 2, 3, 4], [2, 30, 40], [1, 11, 111]);
		assert.deepEqual(result, [3, 4]);
	});

	it("intersection", function() {
		var stooges = ['moe', 'curly', 'larry'],
			leaders = ['moe', 'groucho'];
		var result;
		assert.deepEqual(util.intersection(stooges, leaders), ['moe']);
		var theSixStooges = ['moe', 'moe', 'curly', 'curly', 'larry', 'larry'];
		assert.deepEqual(util.intersection(theSixStooges, leaders), ['moe']);
		result = util.intersection([2, 4, 3, 1], [1, 2, 3]);
		assert.deepEqual(result, [2, 3, 1]);
		result = util.intersection(null, [1, 2, 3]);
		assert.equal(Object.prototype.toString.call(result), '[object Array]');
		assert.equal(result.length, 0);
	});

	it("pick", function() {
		var result;
		result = util.pick({
			a: 1,
			b: 2,
			c: 3
		}, 'a', 'c');
		assert.deepEqual(result, {
			a: 1,
			c: 3
		});
		result = util.pick({
			a: 1,
			b: 2,
			c: 3
		}, ['b', 'c']);
		assert.deepEqual(result, {
			b: 2,
			c: 3
		});
		result = util.pick({
			a: 1,
			b: 2,
			c: 3
		}, ['a'], 'b');
		assert.deepEqual(result, {
			a: 1,
			b: 2
		});
		result = util.pick(['a', 'b'], 1);
		assert.deepEqual(result, {
			1: 'b'
		});

		assert.deepEqual(util.pick(null, 'a', 'b'), {});
		assert.deepEqual(util.pick(undefined, 'toString'), {});

		var Obj = function() {};
		Obj.prototype = {
			a: 1,
			b: 2,
			c: 3
		};
		var instance = new Obj();
		assert.deepEqual(util.pick(instance, 'a', 'c'), {
			a: 1,
			c: 3
		});
	});

	it("omit", function() {
		var result;
		result = util.omit({
			a: 1,
			b: 2,
			c: 3
		}, 'b');
		assert.deepEqual(result, {
			a: 1,
			c: 3
		});
		result = util.omit({
			a: 1,
			b: 2,
			c: 3
		}, 'a', 'c');
		assert.deepEqual(result, {
			b: 2
		});
		result = util.omit({
			a: 1,
			b: 2,
			c: 3
		}, ['b', 'c']);
		assert.deepEqual(result, {
			a: 1
		});
		result = util.omit(['a', 'b'], 0);
		assert.deepEqual(result, {
			1: 'b'
		});

		assert.deepEqual(util.omit(null, 'a', 'b'), {});
		assert.deepEqual(util.omit(undefined, 'toString'), {});

		var Obj = function() {};
		Obj.prototype = {
			a: 1,
			b: 2,
			c: 3
		};
		var instance = new Obj();
		assert.deepEqual(util.omit(instance, 'b'), {
			a: 1,
			c: 3
		});
	});

	it('each', function() {
		util.each([1, 2, 3], function(num, i) {
			assert.equal(num, i + 1);
		});

		var answers = [];
		util.each([1, 2, 3], function(num) {
			answers.push(num * this.multiplier);
		}, {
			multiplier: 5
		});
		assert.deepEqual(answers, [5, 10, 15]);

		answers = [];
		util.each([1, 2, 3], function(num) {
			answers.push(num);
		});
		assert.deepEqual(answers, [1, 2, 3]);

		var answer = null;
		util.each([1, 2, 3], function(num, index, arr) {
			assert.equal(arr[index], num);
		});

		answers = 0;
		util.each(null, function() {
			++answers;
		});
		assert.equal(answers, 0);

		util.each(false, function() {});

		var a = [1, 2, 3];
		assert.strictEqual(util.each(a, function() {}), a);
		assert.strictEqual(util.each(null, function() {}), null);

		var b = [1, 2, 3];
		b.length = 100;
		answers = 0;
		util.each(b, function() {
			++answers;
		});
		assert.equal(answers, 100);
	});


	it('map', function() {
		var doubled = util.map([1, 2, 3], function(num) {
			return num * 2;
		});
		assert.deepEqual(doubled, [2, 4, 6]);

		var tripled = util.map([1, 2, 3], function(num) {
			return num * this.multiplier;
		}, {
			multiplier: 3
		});
		assert.deepEqual(tripled, [3, 6, 9]);

		doubled = util.map([1, 2, 3], function(num) {
			return num * 2;
		});
		assert.deepEqual(doubled, [2, 4, 6]);

		var ids = util.map({
			length: 2,
			0: {
				id: '1'
			},
			1: {
				id: '2'
			}
		}, function(n) {
			return n.id;
		});
		assert.deepEqual(ids, ['1', '2']);

		assert.deepEqual(util.map(null, function() {}), []);

		assert.deepEqual(util.map([1], function() {
			return this.length;
		}, [5]), [1]);
	});


	it('reduce', function() {
		var sum = util.reduce([1, 2, 3], function(sum, num) {
			return sum + num;
		}, 0);
		assert.equal(sum, 6);

		var context = {
			multiplier: 3
		};
		sum = util.reduce([1, 2, 3], function(sum, num) {
			return sum + num * this.multiplier;
		}, 0, context);
		assert.equal(sum, 18);

		assert.equal(util.reduce(null, function() {}, 138), 138);
		assert.equal(util.reduce([], function() {}, undefined), undefined);

		assert.throws(function() {
			util.reduce([], function() {});
		});

		assert.throws(function() {
			util.reduce(null, function() {});
		});
	});

	describe('format', function() {
		it("basic", function() {
			assert.equal(util.format(), '');
			assert.equal(util.format(''), '');
			assert.equal(util.format(null), 'null');
			assert.equal(util.format(true), 'true');
			assert.equal(util.format(false), 'false');
			assert.equal(util.format('test'), 'test');
			assert.equal(util.format(Infinity), 'Infinity');
			assert.equal(util.format(new Number("aaa")), 'NaN');

			// CHECKME this is for console.log() compatibility - but is it *right*?
			assert.equal(util.format('foo', 'bar', 'baz'), 'foo bar baz');
		});

		it("array", function() {
			assert.equal(util.format([]), '[]');
			assert.equal(util.format(["1"]), '[\n  "1"\n]');
			assert.equal(util.format([100, 200]), '[\n  100,\n  200\n]');
		});

		it("object", function() {
			assert.equal(util.format({}), '{}');
			assert.equal(util.format({
				a: 100,
				b: 200
			}), '{\n  "a": 100,\n  "b": 200\n}');
		});

		it("Buffer", function() {
			assert.equal(util.format(new Buffer("aaaaaaaa")), 'YWFhYWFhYWE=');
		});

		it("Int64", function() {
			assert.equal(util.format(new Int64(12345678)), '12345678');
		});

		it("List", function() {
			var a = new collection.List();
			a.push(100, 200);
			assert.equal(util.format(a), '[\n  100,\n  200\n]');
		});

		it("levels", function() {
			assert.equal(util.format({}), '{}');
			assert.equal(util.format([
				[
					[]
				]
			]), '[\n  [\n    []\n  ]\n]');
			assert.equal(util.format({
				a: {
					b: {}
				}
			}), '{\n  "a": {\n    "b": {}\n  }\n}');
		});

		it("regexp", function() {
			assert.equal(util.format(/aaa/), '/aaa/');
			assert.equal(util.format(/aaa/igm), '/aaa/igm');
			assert.equal(util.format(/a\\aa/igm), '/a\\\\aa/igm');
			assert.equal(util.format([/aaa/]), '[\n  /aaa/\n]');
			assert.equal(util.format({
				a: /aaa/
			}), '{\n  "a": /aaa/\n}');
		});

		it("Circular", function() {
			var a = [];
			a[0] = a;
			assert.equal(util.format(a), '[Circular]');

			var a1 = [100, 200];
			a1[2] = a1;
			assert.equal(util.format(a1), '[\n  100,\n  200,\n  [Circular]\n]');

			var o = {};
			o.o = o;
			assert.equal(util.format(o), '[Circular]');

			var o1 = {
				a: 100,
				b: 200
			};
			o1["c"] = o1;
			assert.equal(util.format(o1), '{\n  "a": 100,\n  "b": 200,\n  "c": [Circular]\n}');
		});

		it("Function", function() {
			assert.equal(util.format(function() {}), '[Function]');

			assert.equal(util.format([

				function() {}
			]), '[\n  [Function]\n]');

			assert.equal(util.format({
				a: function() {}
			}), '{\n  "a": [Function]\n}');
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

		it("fix: crash on error.", function() {
			util.format(new mq.Message());
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
			c = new util.LruCache(3);
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
			c = new util.LruCache(3, 100);

			c.put('f', 100);
			c.put('d', 300);
			c.put('e', 400);

			coroutine.sleep(50);
			c.put('f', 500);
			deepEqual(c.toJSON(), {
				"f": 500,
				"e": 400,
				"d": 300
			});

			coroutine.sleep(20);
			deepEqual(c.toJSON(), {
				"f": 500,
				"e": 400,
				"d": 300
			});

			c.put('e', 700);

			assert.equal(c.get('d'), 300);
			coroutine.sleep(51);
			deepEqual(c.toJSON(), {
				"e": 700,
				"f": 500
			});

			assert.equal(c.get('f'), 500);
			coroutine.sleep(30);
			deepEqual(c.toJSON(), {
				"e": 700
			});
		});

		it("updater", function() {
			var call_num = 0;

			function updater(name) {
				coroutine.sleep(30);
				call_num++;
				return name + "_value";
			}
			c = new util.LruCache(3);

			assert.equal(c.get("a", updater), "a_value");
			assert.equal(call_num, 1);

			coroutine.start(function() {
				c.get("b", updater);
			});
			coroutine.sleep(1);
			assert.equal(c.get("b"), "b_value");
			assert.equal(call_num, 2);

			function test_c() {
				c.get("c", updater);
			}

			coroutine.start(test_c);
			coroutine.start(test_c);
			coroutine.start(test_c);
			coroutine.sleep(1);
			assert.equal(c.get("c"), "c_value");
			assert.equal(call_num, 3);

			assert.throws(function() {
				c.get("d", function() {
					throw "some error";
				})
			});

		});

		it("Garbage Collection", function() {
			GC();
			var no1 = os.memoryUsage().nativeObjects.objects;

			var lc = new util.LruCache(1024);
			lc.put("test", lc);
			assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);

			lc.put("test1", new Buffer());
			assert.equal(no1 + 2, os.memoryUsage().nativeObjects.objects);

			lc.remove("test1");
			GC();
			assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);

			lc = undefined;

			GC();
			assert.equal(no1, os.memoryUsage().nativeObjects.objects);


		});
	});
});

//test.run(console.DEBUG);