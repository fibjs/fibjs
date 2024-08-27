var test = require("test");
test.setup();

var test_util = require('./test_util');

var util = require('util');
var fs = require('fs');
var mq = require('mq');
var coroutine = require('coroutine');

describe('util', () => {
    it("inherits", () => {
        const inherits = util.inherits;

        function Child() {
            console.log("in child");
        }

        function Parent() {
            console.log("in parent");
        }

        inherits(Child, Parent);

        var child = new Child();

        assert.ok(child.constructor === Child)
        assert.ok(child.constructor.super_ === Parent)
        assert.ok(Object.getPrototypeOf(child) === Child.prototype)
        assert.ok(Object.getPrototypeOf(Object.getPrototypeOf(child)) === Parent.prototype)
        assert.ok(child instanceof Child)
        assert.ok(child instanceof Parent)

        // super constructor
        function A() {
            this._a = 'a';
        }
        A.prototype.a = function () {
            return this._a;
        };

        // one level of inheritance
        function B(value) {
            A.call(this);
            this._b = value;
        }
        inherits(B, A);
        B.prototype.b = function () {
            return this._b;
        };

        assert.strictEqual(B.super_, A);

        const b = new B('b');
        assert.strictEqual(b.a(), 'a');
        assert.strictEqual(b.b(), 'b');
        assert.strictEqual(b.constructor, B);

        // two levels of inheritance
        function C() {
            B.call(this, 'b');
            this._c = 'c';
        }
        inherits(C, B);
        C.prototype.c = function () {
            return this._c;
        };
        C.prototype.getValue = function () {
            return this.a() + this.b() + this.c();
        };

        assert.strictEqual(C.super_, B);

        const c = new C();
        assert.strictEqual(c.getValue(), 'abc');
        assert.strictEqual(c.constructor, C);

        // inherits can be called after setting prototype properties
        function D() {
            C.call(this);
            this._d = 'd';
        }

        D.prototype.d = function () {
            return this._d;
        };
        inherits(D, C);

        assert.strictEqual(D.super_, C);

        const d = new D();
        assert.strictEqual(d.c(), 'c');
        assert.strictEqual(d.d(), 'd');
        assert.strictEqual(d.constructor, D);

        // ES6 classes can inherit from a constructor function
        class E {
            constructor() {
                D.call(this);
                this._e = 'e';
            }
            e() {
                return this._e;
            }
        }
        inherits(E, D);

        assert.strictEqual(E.super_, D);

        const e = new E();
        assert.strictEqual(e.getValue(), 'abc');
        assert.strictEqual(e.d(), 'd');
        assert.strictEqual(e.e(), 'e');
        assert.strictEqual(e.constructor, E);

        // should throw with invalid arguments
        assert.throws(function () {
            inherits(A, {});
        });
        assert.throws(function () {
            inherits(A, null);
        });
        assert.throws(function () {
            inherits(null, A);
        });
    });

    it('has', () => {
        var obj = {
            foo: 'bar',
            func: () => { }
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

    it('keys', () => {
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

    it('values', () => {
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

    it('clone', () => {
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

        var moe = {
            name: 'moe',
            lucky: [13, 27, 34]
        };
        var clone = util.clone(moe);
        assert.strictEqual(clone.name, 'moe', 'the clone as the attributes of the original');

        clone.name = 'curly';
        assert.ok(clone.name === 'curly' && moe.name === 'moe', 'clones can change shallow attributes without affecting the original');

        clone.lucky.push(101);
        assert.strictEqual(util.last(moe.lucky), 101, 'changes to deep attributes are shared with the original');

        assert.strictEqual(util.clone(void 0), void 0, 'non objects should not be changed by clone');
        assert.strictEqual(util.clone(1), 1, 'non objects should not be changed by clone');
        assert.strictEqual(util.clone(null), null, 'non objects should not be changed by clone');
    });

    it('clone crash', () => {
        util.clone(new Map());
    });

    it('deepFreeze', () => {
        var o = {
            a: {
                b: 100
            },
            b: [200]
        };

        util.deepFreeze(o);
        o.c = 200;
        o.a.c = 300;

        assert.throws(() => {
            o.b.push(400);
        })

        assert.deepEqual(o, {
            a: {
                b: 100
            },
            b: [200]
        });

        assert.throws(() => {
            util.deepFreeze(new Buffer(10));
        });
    });

    it('extend', () => {
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
        } catch (ex) { }

        assert.equal(result.a, 1, 'should not error on `null` or `undefined` sources');

        assert.strictEqual(util.extend(null, {
            a: 1
        }), null, 'extending null results in null');
        assert.strictEqual(util.extend(undefined, {
            a: 1
        }), undefined, 'extending undefined results in undefined');
    });

    it("first", () => {
        assert.equal(util.first([1, 2, 3]), 1);
        assert.deepEqual(util.first([1, 2, 3], 0), []);
        assert.deepEqual(util.first([1, 2, 3], 2), [1, 2]);
        assert.deepEqual(util.first([1, 2, 3], 5), [1, 2, 3]);

        var result = (function () {
            return util.first([1, 2, 3], 2);
        }());
        assert.deepEqual(result, [1, 2]);

        assert.equal(util.first(null), undefined);
        assert.strictEqual(util.first([1, 2, 3], -1).length, 0);
    });

    it("last", () => {
        assert.equal(util.last([1, 2, 3]), 3);
        assert.deepEqual(util.last([1, 2, 3], 0), []);
        assert.deepEqual(util.last([1, 2, 3], 2), [2, 3]);
        assert.deepEqual(util.last([1, 2, 3], 5), [1, 2, 3]);

        assert.equal(util.last(null), undefined);
        assert.strictEqual(util.last([1, 2, 3], -1).length, 0);
    });

    it('unique', () => {
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
        var iterator = (value) => {
            return value.name;
        };

        var a = {},
            b = {},
            c = {};
        assert.deepEqual(util.unique([a, b, a, b, c]), [a, b, c]);
        assert.deepEqual(util.unique(null), []);
    });

    it('union', () => {
        var result = util.union([1, 2, 3], [2, 30, 1], [1, 40]);
        assert.deepEqual(result, [1, 2, 3, 30, 40]);

        result = util.union([1, 2, 3], [2, 30, 1], [1, 40, [1]]);
        assert.deepEqual(result, [1, 2, 3, 30, 40, [1]]);
    });

    it('flatten', () => {
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
        var result = (function () {
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

    it('without', () => {
        var list = [1, 2, 1, 0, 3, 1, 4];
        assert.deepEqual(util.without(list, 0, 1), [2, 3, 4]);

        var result = (function () {
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

    it('difference', () => {
        var result = util.difference([1, 2, 3], [2, 30, 40]);
        assert.deepEqual(result, [1, 3]);

        result = util.difference([1, 2, 3, 4], [2, 30, 40], [1, 11, 111]);
        assert.deepEqual(result, [3, 4]);
    });

    it("intersection", () => {
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

    it("FIX: intersection crash when valueOf throw error", () => {
        util.intersection([{
            valueOf() {
                throw new Error('error');
            }
        }, 0]);
    });

    it("pick", () => {
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

        var Obj = function () { };
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

    it("omit", () => {
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

        var Obj = function () { };
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

    it('each', () => {
        util.each([1, 2, 3], (num, i) => {
            assert.equal(num, i + 1);
        });

        var answers = [];
        util.each([1, 2, 3], function (num) {
            answers.push(num * this.multiplier);
        }, {
            multiplier: 5
        });
        assert.deepEqual(answers, [5, 10, 15]);

        answers = [];
        util.each([1, 2, 3], (num) => {
            answers.push(num);
        });
        assert.deepEqual(answers, [1, 2, 3]);

        var answer = null;
        util.each([1, 2, 3], (num, index, arr) => {
            assert.equal(arr[index], num);
        });

        answers = 0;
        util.each(null, () => {
            ++answers;
        });
        assert.equal(answers, 0);

        util.each(false, () => { });

        var a = [1, 2, 3];
        assert.strictEqual(util.each(a, () => { }), a);
        assert.strictEqual(util.each(null, () => { }), null);

        var b = [1, 2, 3];
        b.length = 100;
        answers = 0;
        util.each(b, () => {
            ++answers;
        });
        assert.equal(answers, 100);
    });

    it('map', () => {
        var doubled = util.map([1, 2, 3], (num) => {
            return num * 2;
        });
        assert.deepEqual(doubled, [2, 4, 6]);

        var tripled = util.map([1, 2, 3], function (num) {
            return num * this.multiplier;
        }, {
            multiplier: 3
        });
        assert.deepEqual(tripled, [3, 6, 9]);

        doubled = util.map([1, 2, 3], (num) => {
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
        }, (n) => {
            return n.id;
        });
        assert.deepEqual(ids, ['1', '2']);

        assert.deepEqual(util.map(null, () => { }), []);

        assert.deepEqual(util.map([1], function () {
            return this.length;
        }, [5]), [1]);
    });


    it('reduce', () => {
        var sum = util.reduce([1, 2, 3], (sum, num) => {
            return sum + num;
        }, 0);
        assert.equal(sum, 6);

        var context = {
            multiplier: 3
        };
        sum = util.reduce([1, 2, 3], function (sum, num) {
            return sum + num * this.multiplier;
        }, 0, context);
        assert.equal(sum, 18);

        assert.equal(util.reduce(null, () => { }, 138), 138);
        assert.equal(util.reduce([], () => { }, undefined), undefined);

        assert.throws(() => {
            util.reduce([], () => { });
        });

        assert.throws(() => {
            util.reduce(null, () => { });
        });
    });

    describe('format', () => {
        it("basic", () => {
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

        it("date", () => {
            assert.equal(util.format(new Date("2016-03-09T07:58:57.303Z")), '2016-03-09T07:58:57.303Z');
        });

        it("array", () => {
            assert.equal(util.format([]), '[]');
            assert.equal(util.format(["1"]), '[\n  "1"\n]');
            assert.equal(util.format([100, 200]), '[\n  100,\n  200\n]');
        });

        it("typedarray", () => {
            assert.equal(util.format(new Uint8Array([])), '[Uint8Array] []');
            assert.equal(util.format(new Uint8Array([100, 200])), '[Uint8Array] [\n  100,\n  200\n]');
        });

        it('Uint8Array, offset', () => {
            var buf = Buffer.from([1, 2, 3, 4, 5, 6, 7, 8, 9]);
            var arr = new Uint8Array(buf.buffer, 2, 4);

            assert.equal(util.format(arr), "[Uint8Array] [\n  3,\n  4,\n  5,\n  6\n]");
        });

        it("object", () => {
            assert.equal(util.format({}), '{}');
            assert.equal(util.format({
                a: 100,
                b: 200
            }), '{\n  "a": 100,\n  "b": 200\n}');
        });

        it("Buffer", () => {
            assert.equal(util.format(new Buffer('fibjs')), '<Buffer 66 69 62 6a 73>');
        });

        it("Map", () => {
            var m = new Map();
            assert.equal(util.format(m), '[Map] {}');
            m.set("b", "foo");
            assert.equal(util.format(m), '[Map] {\n  \"b\" => \"foo\"\n}');
        });

        it("Set", () => {
            var s = new Set();
            assert.equal(util.format(s), '[Set] []');
            s.add("b");
            assert.equal(util.format(s), '[Set] [\n  \"b\"\n]');
        });

        it("levels", () => {
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

        it("regexp", () => {
            assert.equal(util.format(/aaa/), '/aaa/');
            assert.equal(util.format(/aaa/igm), '/aaa/igm');
            assert.equal(util.format(/a\\aa/igm), '/a\\\\aa/igm');
            assert.equal(util.format([/aaa/]), '[\n  /aaa/\n]');
            assert.equal(util.format({
                a: /aaa/
            }), '{\n  "a": /aaa/\n}');
        });

        it("Circular", () => {
            var a = [];
            a[0] = a;
            assert.equal(util.format(a), '[\n  [Circular]\n]');

            var a1 = [100, 200];
            a1[2] = a1;
            assert.equal(util.format(a1), '[\n  100,\n  200,\n  [Circular]\n]');

            var o = {};
            o.o = o;
            assert.equal(util.format(o), '{\n  \"o\": [Circular]\n}');

            var o1 = {
                a: 100,
                b: 200
            };
            o1["c"] = o1;
            assert.equal(util.format(o1), '{\n  "a": 100,\n  "b": 200,\n  "c": [Circular]\n}');
        });

        it("deep object", () => {
            var data = {
                l1: {
                    l2_0: { a: 100 },
                    l2: {
                        l3: {
                            l4:
                            {
                            }
                        },
                        l3_0: {},
                        l3_1: [1, 2, 3, 4],
                        l3_2: []
                    }
                }
            }

            assert.equal(util.format(data), '{\n  \"l1\": {\n    \"l2_0\": {\n      \"a\": 100\n    },\n    \"l2\": {\n      \"l3\": [Object],\n      \"l3_0\": {},\n      \"l3_1\": [Array],\n      \"l3_2\": []\n    }\n  }\n}');
        });

        it("huge array", () => {
            var arr = [];
            var txts = [];

            for (var i = 0; i < 101; i++)
                arr.push(i);

            txts.push('[');
            for (var i = 0; i < 100; i++)
                txts.push(`  ${i},`);
            txts.push('  ... 1 more item');
            txts.push(']');

            assert.equal(util.format(arr), txts.join('\n'));

            arr.push(i);
            txts[101] = '  ... 2 more items';

            assert.equal(util.format(arr), txts.join('\n'));
        });

        it("huge Buffer", () => {
            var b = Buffer.alloc(51);
            var txts = [];

            txts.push('<Buffer');
            for (var i = 0; i < 50; i++)
                txts.push('00');
            txts.push('... 1 more byte>');

            assert.equal(util.format(b), txts.join(' '));

            var b = Buffer.alloc(52);
            var txts = [];

            txts.push('<Buffer');
            for (var i = 0; i < 50; i++)
                txts.push('00');
            txts.push('... 2 more bytes>');

            assert.equal(util.format(b), txts.join(' '));
        });

        it("Function", () => {
            assert.equal(util.format(() => { }), '[Function]');

            assert.equal(util.format([
                () => { }
            ]), '[\n  [Function]\n]');

            assert.equal(util.format([
                async () => { }
            ]), '[\n  [AsyncFunction]\n]');

            assert.equal(util.format({
                a: () => { }
            }), '{\n  "a": [Function a]\n}');

            assert.equal(util.format(util.format), "[Function format]");

            function a1() { }
            a1.v = 100;

            assert.equal(util.format(a1), "[Function a1] {\n  \"v\": 100\n}");
        });

        it("Symbol", () => {
            assert.equal(util.format(Symbol()), 'Symbol()');
            assert.equal(util.format(Symbol('debug')), 'Symbol(debug)');
            const o = {};
            o[Symbol('debug')] = 100;
            assert.equal(util.format(o), '{\n  [Symbol(debug)]: 100\n}');
        });

        it("Error", () => {
            var e = new Error;
            assert.equal(util.format(e), e.stack);
        });

        it("%d", () => {
            assert.equal(util.format('%d', 42.0), '42');
            assert.equal(util.format('%d', 42), '42');
            assert.equal(util.format('%s', 42), '42');
            assert.equal(util.format('%j', 42), '42');

            assert.equal(util.format('%d', '42.0'), '42');
            assert.equal(util.format('%d', '42'), '42');
            assert.equal(util.format('%s', '42'), '42');
        });

        it("%j", () => {
            assert.equal(util.format('%j', '42'), '"42"');
        });

        it("%s", () => {
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

        it("fix: crash on error.", () => {
            util.format(new mq.Message());
        });

        it("fix: crash on %d.", () => {
            util.format("%d", 2n);
        });
    });

    describe('inspect', () => {
        it('depth', () => {
            assert.equal(util.inspect([[[[[]]]]], {
                colors: false
            }), "[\n  [\n    [\n      [Array]\n    ]\n  ]\n]");
            assert.equal(util.inspect([[[[[]]]]], {
                colors: false,
                depth: 3
            }), "[\n  [\n    [\n      [\n        []\n      ]\n    ]\n  ]\n]");
        });

        it('maxArrayLength', () => {
            assert.equal(util.inspect(["1", "2", "3"], {
                colors: false,
                maxArrayLength: 2
            }), "[\n  \"1\",\n  \"2\",\n  ... 1 more item\n]");
            assert.equal(util.inspect(["1", "2", "3", "4"], {
                colors: false,
                maxArrayLength: 2
            }), "[\n  \"1\",\n  \"2\",\n  ... 2 more items\n]");
            assert.equal(util.inspect(["1", "2", "3", "4"], {
                colors: false,
                maxArrayLength: -1
            }), "[\n  ... 4 more items\n]");
            assert.equal(util.inspect(["1", "2", "3", "4"], {
                colors: false,
            }), "[\n  \"1\",\n  \"2\",\n  \"3\",\n  \"4\"\n]");
        });

        it('maxStringLength', () => {
            assert.equal(util.inspect("1234567890", {
                colors: false,
                maxStringLength: 5
            }), "\"12345\"... 5 more characters");
            assert.equal(util.inspect("1234567890", {
                colors: false,
                maxStringLength: 9
            }), "\"123456789\"... 1 more character");
            assert.equal(util.inspect("1234567890", {
                colors: false,
                maxStringLength: -1
            }), "\"123456789\"... 11 more characters");
            assert.equal(util.inspect("1234567890", {
                colors: false,
            }), "\"1234567890\"");
            assert.equal(util.inspect("测\n试123", {
                colors: false,
                maxStringLength: 4
            }), "\"测\\n试1\"... 2 more characters");
        })
    });

    describe("table", () => {
        function test_table(data, only, expected) {
            if (arguments.length === 2) {
                expected = only;
                only = undefined;
            }
            var result = util.inspect(data, {
                table: true,
                fields: only
            });
            assert.equal('\n' + result + '\n', expected);
        }

        function test_table1(data, opt, expected) {
            opt.table = true;
            var result = util.inspect(data, opt);
            assert.equal('\n' + result + '\n', expected);
        }

        it("simple value", () => {
            test_table(null, '\nnull\n');
            test_table(undefined, '\nundefined\n');
            test_table(false, '\nfalse\n');
            test_table('hi', '\n"hi"\n');
            test_table(Symbol(), '\nSymbol()\n');
            test_table(util.inspect, '\n[Function inspect]\n');
        });

        it("simple table", () => {
            test_table([1, 2, 3], `
┌─────────┬────────┐
│ (index) │ Values │
├─────────┼────────┤
│    0    │   1    │
│    1    │   2    │
│    2    │   3    │
└─────────┴────────┘
`);

            test_table([Symbol(), 5, [10]], `
┌─────────┬────┬──────────┐
│ (index) │ 0  │  Values  │
├─────────┼────┼──────────┤
│    0    │    │ Symbol() │
│    1    │    │    5     │
│    2    │ 10 │          │
└─────────┴────┴──────────┘
`);

            test_table([null, 5], `
┌─────────┬────────┐
│ (index) │ Values │
├─────────┼────────┤
│    0    │  null  │
│    1    │   5    │
└─────────┴────────┘
`);

            test_table([undefined, 5], `
┌─────────┬───────────┐
│ (index) │  Values   │
├─────────┼───────────┤
│    0    │ undefined │
│    1    │     5     │
└─────────┴───────────┘
`);

            test_table({ a: 1, b: Symbol(), c: [10] }, `
┌─────────┬────┬──────────┐
│ (index) │ 0  │  Values  │
├─────────┼────┼──────────┤
│    a    │    │    1     │
│    b    │    │ Symbol() │
│    c    │ 10 │          │
└─────────┴────┴──────────┘
`);

            test_table({ a: { a: 1, b: 2, c: 3 } }, `
┌─────────┬───┬───┬───┐
│ (index) │ a │ b │ c │
├─────────┼───┼───┼───┤
│    a    │ 1 │ 2 │ 3 │
└─────────┴───┴───┴───┘
`);

            // test_table(new Map([['a', 1], [Symbol(), [2]]]), `
            // ┌───────────────────┬──────────┬────────┐
            // │ (iteration index) │   Key    │ Values │
            // ├───────────────────┼──────────┼────────┤
            // │         0         │   'a'    │   1    │
            // │         1         │ Symbol() │ [ 2 ]  │
            // └───────────────────┴──────────┴────────┘
            // `);

            // test_table(new Set([1, 2, Symbol()]), `
            // ┌───────────────────┬──────────┐
            // │ (iteration index) │  Values  │
            // ├───────────────────┼──────────┤
            // │         0         │    1     │
            // │         1         │    2     │
            // │         2         │ Symbol() │
            // └───────────────────┴──────────┘
            // `);

            // test_table(new Map([[1, 1], [2, 2], [3, 3]]).entries(), `
            // ┌───────────────────┬─────┬────────┐
            // │ (iteration index) │ Key │ Values │
            // ├───────────────────┼─────┼────────┤
            // │         0         │  1  │   1    │
            // │         1         │  2  │   2    │
            // │         2         │  3  │   3    │
            // └───────────────────┴─────┴────────┘
            // `);

            // test_table(new Map([[1, 1], [2, 2], [3, 3]]).values(), `
            // ┌───────────────────┬────────┐
            // │ (iteration index) │ Values │
            // ├───────────────────┼────────┤
            // │         0         │   1    │
            // │         1         │   2    │
            // │         2         │   3    │
            // └───────────────────┴────────┘
            // `);

            // test_table(new Map([[1, 1], [2, 2], [3, 3]]).keys(), `
            // ┌───────────────────┬────────┐
            // │ (iteration index) │ Values │
            // ├───────────────────┼────────┤
            // │         0         │   1    │
            // │         1         │   2    │
            // │         2         │   3    │
            // └───────────────────┴────────┘
            // `);

            // test_table(new Set([1, 2, 3]).values(), `
            // ┌───────────────────┬────────┐
            // │ (iteration index) │ Values │
            // ├───────────────────┼────────┤
            // │         0         │   1    │
            // │         1         │   2    │
            // │         2         │   3    │
            // └───────────────────┴────────┘
            // `);
        });

        it("fields", () => {
            test_table({ a: 1, b: 2 }, ['a'], `
┌─────────┬───┐
│ (index) │ a │
├─────────┼───┤
│    a    │   │
│    b    │   │
└─────────┴───┘
`);

            test_table([{ a: 1, b: 2 }, { a: 3, c: 4 }], ['a'], `
┌─────────┬───┐
│ (index) │ a │
├─────────┼───┤
│    0    │ 1 │
│    1    │ 3 │
└─────────┴───┘
`);

        });

        it("fields", () => {
            test_table({ a: { a: [] } }, `
┌─────────┬────┐
│ (index) │ a  │
├─────────┼────┤
│    a    │ [] │
└─────────┴────┘
`);

            test_table({ a: { a: {} } }, `
┌─────────┬────┐
│ (index) │ a  │
├─────────┼────┤
│    a    │ {} │
└─────────┴────┘
`);
        });

        it("object/array", () => {
            test_table({ a: { a: { a: 1, b: 2, c: 3 } } }, `
┌─────────┬──────────┐
│ (index) │    a     │
├─────────┼──────────┤
│    a    │ [Object] │
└─────────┴──────────┘
`);

            test_table({ a: { x: [1, 2, 3] } }, `
┌─────────┬─────────────┐
│ (index) │      x      │
├─────────┼─────────────┤
│    a    │ [ 1, 2, 3 ] │
└─────────┴─────────────┘
`);

            test_table({ a: { x: [1, 2, 3, 4] } }, `
┌─────────┬──────────────────────────────┐
│ (index) │              x               │
├─────────┼──────────────────────────────┤
│    a    │ [ 1, 2, 3, ... 1 more item ] │
└─────────┴──────────────────────────────┘
`);

            test_table({ a: { x: [1, 2, 3, 4, 5, 6, 7] } }, `
┌─────────┬───────────────────────────────┐
│ (index) │               x               │
├─────────┼───────────────────────────────┤
│    a    │ [ 1, 2, 3, ... 4 more items ] │
└─────────┴───────────────────────────────┘
`);

            test_table({ a: { x: [1, [], {}] } }, `
┌─────────┬───────────────┐
│ (index) │       x       │
├─────────┼───────────────┤
│    a    │ [ 1, [], {} ] │
└─────────┴───────────────┘
`);

            test_table({ a: { x: [{ a: 1 }, [1]] } }, `
┌─────────┬───────────────────────┐
│ (index) │           x           │
├─────────┼───────────────────────┤
│    a    │ [ [Object], [Array] ] │
└─────────┴───────────────────────┘
`);

            test_table({ a: { x: { a: 100 } } }, `
┌─────────┬──────────────┐
│ (index) │      x       │
├─────────┼──────────────┤
│    a    │ { "a": 100 } │
└─────────┴──────────────┘
`);

            test_table({ a: { a: { a: 100, b: [1, 2, 3] }, b: 200 } }, `
┌─────────┬────────────────────────────┬─────┐
│ (index) │             a              │  b  │
├─────────┼────────────────────────────┼─────┤
│    a    │ { "a": 100, "b": [Array] } │ 200 │
└─────────┴────────────────────────────┴─────┘
`);

            test_table({ a: { x: { a: 100, b: 200 } } }, `
┌─────────┬────────────────────────┐
│ (index) │           x            │
├─────────┼────────────────────────┤
│    a    │ { "a": 100, "b": 200 } │
└─────────┴────────────────────────┘
`);

            test_table({ a: { x: { a: 100, b: 200, c: 300 } } }, `
┌─────────┬──────────┐
│ (index) │    x     │
├─────────┼──────────┤
│    a    │ [Object] │
└─────────┴──────────┘
`);

            test_table({ a: { x: Buffer.from([1, 2, 3, 4, 5]) } }, `
┌─────────┬─────────────────────────┐
│ (index) │            x            │
├─────────┼─────────────────────────┤
│    a    │ <Buffer 01 02 03 04 05> │
└─────────┴─────────────────────────┘
`);
        });

        it("field order", () => {
            test_table({ a: [1, 2] }, `
┌─────────┬───┬───┐
│ (index) │ 0 │ 1 │
├─────────┼───┼───┤
│    a    │ 1 │ 2 │
└─────────┴───┴───┘
`);

            test_table({ a: [1, 2, 3, 4, 5], b: 5, c: { e: 5 } }, `
┌─────────┬───┬───┬───┬───┬───┬───┬────────┐
│ (index) │ 0 │ 1 │ 2 │ 3 │ 4 │ e │ Values │
├─────────┼───┼───┼───┼───┼───┼───┼────────┤
│    a    │ 1 │ 2 │ 3 │ 4 │ 5 │   │        │
│    b    │   │   │   │   │   │   │   5    │
│    c    │   │   │   │   │   │ 5 │        │
└─────────┴───┴───┴───┴───┴───┴───┴────────┘
`);
        });

        it("unicode", () => {
            test_table({ foo: '￥', bar: '¥' }, `
┌─────────┬────────┐
│ (index) │ Values │
├─────────┼────────┤
│   foo   │  "￥"  │
│   bar   │  "¥"   │
└─────────┴────────┘
`);

            test_table({ foo: '你好', bar: 'hello' }, `
┌─────────┬─────────┐
│ (index) │ Values  │
├─────────┼─────────┤
│   foo   │ "你好"  │
│   bar   │ "hello" │
└─────────┴─────────┘
`);
        });

        it("encode_string", () => {
            test_table1([1, 2, "asd"], { encode_string: false }, `
┌─────────┬────────┐
│ (index) │ Values │
├─────────┼────────┤
│    0    │   1    │
│    1    │   2    │
│    2    │  asd   │
└─────────┴────────┘
`);

            test_table1([1, 2, ["asd"]], { encode_string: false }, `
┌─────────┬─────┬────────┐
│ (index) │  0  │ Values │
├─────────┼─────┼────────┤
│    0    │     │   1    │
│    1    │     │   2    │
│    2    │ asd │        │
└─────────┴─────┴────────┘
`);
        });

        it("other", () => {
            test_table(new Uint8Array([1, 2, 3]), `
┌─────────┬────────┐
│ (index) │ Values │
├─────────┼────────┤
│    0    │   1    │
│    1    │   2    │
│    2    │   3    │
└─────────┴────────┘
`);

            test_table(Buffer.from([1, 2, 3]), `
┌─────────┬────────┐
│ (index) │ Values │
├─────────┼────────┤
│    0    │   1    │
│    1    │   2    │
│    2    │   3    │
└─────────┴────────┘
`);

            test_table({ a: undefined }, ['x'], `
┌─────────┬───┐
│ (index) │ x │
├─────────┼───┤
│    a    │   │
└─────────┴───┘
`);

            test_table([], `
┌─────────┐
│ (index) │
├─────────┤
└─────────┘
`);

            // test_table(new Map(), `
            // ┌───────────────────┬─────┬────────┐
            // │ (iteration index) │ Key │ Values │
            // ├───────────────────┼─────┼────────┤
            // └───────────────────┴─────┴────────┘
            // `);

            test_table([{ a: 1, b: 'Y' }, { a: 'Z', b: 2 }], `
┌─────────┬─────┬─────┐
│ (index) │  a  │  b  │
├─────────┼─────┼─────┤
│    0    │  1  │ "Y" │
│    1    │ "Z" │  2  │
└─────────┴─────┴─────┘
`);

            {
                const line = '─'.repeat(79);
                const header = `${' '.repeat(37)}name${' '.repeat(40)}`;
                const name = 'very long long long long long long long long long long long ' +
                    'long long long long';
                test_table([{ name }], `
┌─────────┬──${line}──┐
│ (index) │  ${header}│
├─────────┼──${line}──┤
│    0    │ "${name}" │
└─────────┴──${line}──┘
`);
            }
        });
    });

    describe('parseArgs', () => {
        it("a single key", () => {
            assert.deepEqual(util.parseArgs("-test"), ["-test"]);
        });

        it("a single key with a value", () => {
            assert.deepEqual(util.parseArgs("-test testing"), ["-test", "testing"]);
        });

        it("a single key=value", () => {
            assert.deepEqual(util.parseArgs("-test=testing"), ["-test=testing"]);
        });

        it("a single value with quotes", () => {
            assert.deepEqual(util.parseArgs('"test quotes"'), ["test quotes"]);
        });

        it("a single value with empty quotes", () => {
            assert.deepEqual(util.parseArgs('""'), [""]);
        });

        it("a complex string with quotes", () => {
            assert.deepEqual(util.parseArgs('-testing test -valid=true --quotes "test quotes"'), [
                "-testing",
                "test",
                "-valid=true",
                "--quotes",
                "test quotes",
            ]);

            assert.deepEqual(util.parseArgs('fibjs test "aaa "aaa "sss"'), [
                "fibjs",
                "test",
                "aaa aaa sss"
            ]);

            assert.deepEqual(util.parseArgs('fibjs test "home aaaa" "bbb bb"'), [
                "fibjs",
                "test",
                "home aaaa",
                "bbb bb"
            ]);
        });

        it("a complex string with empty quotes", () => {
            assert.deepEqual(util.parseArgs('-testing test -valid=true --quotes ""'), [
                "-testing",
                "test",
                "-valid=true",
                "--quotes",
                "",
            ]);
        });

        it("a complex string include escape", () => {
            assert.deepEqual(util.parseArgs('fibjs test\\"quote test\\ space "aaa \\"  sss"'), [
                "fibjs",
                "test\"quote",
                "test space",
                "aaa \"  sss"
            ]);
        });
    });

    describe('sync', () => {
        it('callback', () => {
            var t = 0;

            function cb_test(cb) {
                setTimeout(function () {
                    t = 2;
                    cb(null, t);
                }, 100);
                t = 1;
            }

            var t1 = util.sync(cb_test)();
            assert.equal(t1, 2);
            assert.equal(t, 2);

            var t2 = 0;
            try {
                util.sync((done) => {
                    done(500);
                })();
            } catch (e) {
                t2 = e;
            }

            assert.equal(t2, 500);
        });

        it('promise', () => {
            var promise = new Promise(function (resolve, reject) {
                resolve(100);
            });

            function cb_test(cb) {
                promise.then(function (value) {
                    cb(null, value);
                });
            }

            var t1 = util.sync(cb_test)();
            assert.equal(t1, 100);
        });

        it('async/await', () => {
            function async_proc(a, b) {
                return new Promise(function (resolve, reject) {
                    resolve(a + b);
                });
            }

            async function async_test(a, b) {
                return await async_proc(a, b);
            }

            var t1 = util.sync(async_test)(100, 200);
            assert.equal(t1, 300);

            var t2 = 0;
            try {
                util.sync(async () => {
                    throw 500;
                })();
            } catch (e) {
                t2 = e;
            }

            assert.equal(t2, 500);
        });

        it('custom async/await', () => {
            function async_test(a, b) {
                return new Promise(function (resolve, reject) {
                    resolve(a + b);
                });
            }

            var t1 = util.sync(async_test, true)(100, 200);
            assert.equal(t1, 300);
        });

        it('custom promise', () => {
            class SimplePromise {
                constructor(fn) {
                    this.callbacks = [];
                    this.failbacks = [];

                    setImmediate(() => {
                        fn(this.resolve.bind(this), this.reject.bind(this));
                    });
                }
                resolve(res) {
                    if (this.callbacks.length > 0) this.callbacks.shift()(res, this.resolve.bind(this), this.reject.bind(this));
                }
                reject(res) {
                    this.callbacks = [];
                    if (this.failbacks.length > 0) this.failbacks.shift()(res, this.resolve.bind(this), this.reject.bind(this));
                }
                catch(fn) {
                    this.failbacks.push(fn);
                }
                then(fn) {
                    this.callbacks.push(fn);
                    return this;
                }
            };

            function async_test(a, b) {
                return new SimplePromise(function (resolve, reject) {
                    resolve(a + b);
                });
            }

            var t1 = util.sync(async_test, true)(100, 200);
            assert.equal(t1, 300);
        });

        it("err func", () => {
            function async_test(a, b) {
                return 100;
            }

            assert.throws(() => {
                util.sync(async_test, true)(100, 200);
            });

            function async_test1(a, b) {
                throw 100;
            }

            assert.throws(() => {
                util.sync(async_test1, true)(100, 200);
            });
        });

        todo("BUGFIX: deadlock when calling sync functions after await", async function () {
            async function test1() {
            }

            async function test2() {
            }

            var test1_sync = util.sync(test1);
            var test2_sync = util.sync(test2);

            try {
                await test1();
            } catch (e) {
                console.log(e);
            }

            try {
                test2_sync();
            } catch (e) {
                console.log(e);
            }
        });
    });

    it('promisify', async () => {
        var t = 0;

        function cb_test(cb) {
            setTimeout(function () {
                t = 2;
                cb(null, t);
            }, 100);
            t = 1;
        }

        var t1 = await util.promisify(cb_test)();
        assert.equal(t1, 2);
        assert.equal(t, 2);

        var t2 = 0;
        try {
            await util.promisify((done) => {
                done(500);
            })();
        } catch (e) {
            t2 = e;
        }

        assert.equal(t2, 500);
    });

    it('callbackify', () => {
        const values = [
            'hello world',
            null,
            undefined,
            false,
            0,
            {},
            { key: 'value' },
            Symbol('I am a symbol'),
            function ok() { },
            ['array', 'with', 4, 'values'],
            new Error('boo')
        ];

        var value1;
        var ev;

        // Test that the resolution value is passed as second argument to callback
        for (const value of values) {
            // Test and `async function`
            async function asyncFn() {
                return value;
            }

            ev = new coroutine.Event();
            value1 = undefined;

            const cbAsyncFn = util.callbackify(asyncFn);
            cbAsyncFn((err, ret) => {
                value1 = ret;
                ev.set();
            });

            ev.wait();
            assert.strictEqual(value1, value);

            // Test Promise factory
            function promiseFn() {
                return Promise.resolve(value);
            }

            ev = new coroutine.Event();
            value1 = undefined;

            const cbPromiseFn = util.callbackify(promiseFn);
            cbPromiseFn((err, ret) => {
                value1 = ret;
                ev.set();
            });

            ev.wait();
            assert.strictEqual(value1, value);
        }
    });

    describe("async wrap cache", () => {
        it("multi sync", () => {
            function cb_test(cb) { }
            async function async_test() { }

            assert.equal(util.sync(cb_test), util.sync(cb_test));
            assert.equal(util.sync(async_test), util.sync(async_test));
        });

        it("multi promisify", () => {
            function cb_test(cb) { }
            const async_test = util.promisify(cb_test);

            assert.equal(async_test, util.promisify(cb_test));
            assert.equal(util.callbackify(async_test), cb_test);
        });

        it("multi callbackify", () => {
            async function async_test() { }
            const cb_test = util.callbackify(async_test);

            assert.equal(cb_test, util.callbackify(async_test));
            assert.equal(async_test, util.promisify(cb_test));
        });

        it("sync with promisify", () => {
            function cb_test(cb) { }
            const async_test = util.promisify(cb_test);

            assert.equal(util.sync(cb_test), util.sync(async_test));
        });

        it("sync with callbackify", () => {
            async function async_test() { }
            const cb_test = util.callbackify(async_test);

            assert.equal(util.sync(cb_test), util.sync(async_test));
        });

        it("promisify with sync", () => {
            function cb_test(cb) { }
            const sync_test = util.sync(cb_test);
            const async_test = util.promisify(cb_test);

            assert.equal(sync_test, util.sync(async_test));
        });

        it("callbackify with sync", () => {
            async function async_test() { }
            const sync_test = util.sync(async_test);
            const cb_test = util.callbackify(async_test);

            assert.equal(sync_test, util.sync(cb_test));
        });

        describe("static native function", () => {
            it("sync function equal async function", () => {
                assert.equal(fs.open, fs.openSync);
                assert.equal(fs.open, util.sync(fs.open));
            });

            it("cache promise function", () => {
                assert.equal(fs.promises.openSync, fs.open);

                assert.equal(fs.promises.open, util.promisify(fs.open));
                assert.equal(util.callbackify(fs.promises.open), fs.open);
                assert.equal(util.sync(fs.promises.open), fs.open);
            });
        });

        describe("member native function", () => {
            it("sync function equal async function", () => {
                const f = fs.open(__filename);

                assert.equal(f.read, f.readSync);
                assert.equal(f.read, util.sync(f.read));
            });

            it("cache promise function", async () => {
                const f = fs.open(__filename);
                const f1 = await fs.promises.open(__filename);

                assert.equal(f1.readSync, f.read);

                assert.equal(f1.read, util.promisify(f.read));
                assert.equal(util.callbackify(f1.read), f.read);
                assert.equal(util.sync(f1.read), f.read);
            });
        });
    });

    describe('buildInfo', () => {
        it('properties', () => {
            assert.property(util.buildInfo(), 'fibjs');
            switch (process.platform) {
                case 'win32':
                    var keys = Object.keys(util.buildInfo());
                    assert.isTrue(keys.includes('msvc') || keys.includes('clang'));
                    break
                case 'darwin':
                case 'freebsd':
                case 'linux':
                    var keys = Object.keys(util.buildInfo());
                    assert.isTrue(keys.includes('gcc') || keys.includes('clang'));
                    break
            }
            // git
            assert.property(util.buildInfo(), 'date');
            // debug
            assert.property(util.buildInfo(), 'vender');
            assert.property(util.buildInfo(), 'modules');
        });

        it('venders', () => {
            assert.property(util.buildInfo().vender, 'ev');
            assert.property(util.buildInfo().vender, 'uv');
            assert.property(util.buildInfo().vender, 'expat');
            assert.property(util.buildInfo().vender, 'leveldb');
            assert.property(util.buildInfo().vender, 'pcre');
            assert.property(util.buildInfo().vender, 'snappy');
            assert.property(util.buildInfo().vender, 'sqlite');
            assert.property(util.buildInfo().vender, 'uuid');
            assert.property(util.buildInfo().vender, 'v8');
            assert.property(util.buildInfo().vender, 'v8-snapshot');
            assert.property(util.buildInfo().vender, 'zlib');
        });

        it('builtins', () => {
            const builtins = util.buildInfo().builtins;

            // builtins
            [
                "zlib",
                "zip",
                "xml",
                "ws",
                "vm",
                "uuid",
                "util",
                "url",
                "tty",
                "timers",
                "test",
                "string_decoder",
                "tls",
                "ssl",
                "querystring",
                "punycode",
                "profiler",
                "process",
                "path",
                "os",
                "net",
                "mq",
                "json",
                "io",
                "iconv",
                "https",
                "http",
                "hex",
                "fs",
                "events",
                "encoding",
                "dns",
                "dgram",
                "db",
                "crypto",
                "coroutine",
                "buffer",
                "base64",
                "base32",
                "assert"
            ].forEach(moduleName => {
                assert.isTrue(builtins.includes(moduleName))
                assert.isObject(require(moduleName))
            });
        });
    });

    it("FIX: flatten a circular reference object will cause fibjs to crash", () => {
        var arr = [100, 200];
        arr.push(arr);

        assert.throws(() => {
            var v = util.flatten(arr);
        });
    });

    it("FIX: util.format(process.env) will cause fibjs to crash", () => {
        util.format(process.env);
    });
});

require.main === module && test.run(console.DEBUG);