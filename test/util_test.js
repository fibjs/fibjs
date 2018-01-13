var test = require("test");
test.setup();

var util = require('util');
var mq = require('mq');
var coroutine = require('coroutine');
var os = require('os');

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

    it("isEmpty", () => {
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

    it("isArray", () => {
        assert.isTrue(util.isArray([]));
        assert.isTrue(util.isArray(Array()));
        assert.isTrue(util.isArray(new Array()));
        assert.isTrue(util.isArray(new Array(5)));
        assert.isTrue(util.isArray(new Array('with', 'some', 'entries')));
        assert.isFalse(util.isArray({}));
        assert.isFalse(util.isArray({
            push: () => {}
        }));
        assert.isFalse(util.isArray(/regexp/));
        assert.isFalse(util.isArray(new Error));
        assert.isFalse(util.isArray(Object.create(Array.prototype)));
    });

    it("isRegExp", () => {
        assert.isTrue(util.isRegExp(/regexp/));
        assert.isTrue(util.isRegExp(RegExp()));
        assert.isTrue(util.isRegExp(new RegExp()));
        assert.isFalse(util.isRegExp({}));
        assert.isFalse(util.isRegExp([]));
        assert.isFalse(util.isRegExp(new Date()));
        assert.isFalse(util.isRegExp(Object.create(RegExp.prototype)));
    });

    it("isObject", () => {
        assert.isTrue(util.isObject({}));
    });

    it('isNull', () => {
        assert.isTrue(util.isNull(null));
        assert.isFalse(util.isNull(undefined));
        assert.isFalse(util.isNull(100));
    });

    it('isUndefined', () => {
        assert.isFalse(util.isUndefined(null));
        assert.isTrue(util.isUndefined(undefined));
        assert.isFalse(util.isUndefined(100));
    });

    it('isNullOrUndefined', () => {
        assert.isTrue(util.isNullOrUndefined(null));
        assert.isTrue(util.isNullOrUndefined(undefined));
        assert.isFalse(util.isNullOrUndefined(100));
    });

    it('isNumber', () => {
        assert.isTrue(util.isNumber(1));
        assert.isTrue(util.isNumber(Number('3')));

        assert.isFalse(util.isNumber('1'));

        assert.isFalse(util.isNumber('hello'));
        assert.isFalse(util.isNumber([5]));
    });

    it('isString', () => {
        assert.isTrue(util.isString('Foo'));
        assert.isTrue(util.isString(new String('foo')));

        assert.isFalse(util.isString(1));

        assert.isFalse(util.isString(3));
        assert.isFalse(util.isString(['hello']));
    });

    it('isRegExp', () => {
        assert.isTrue(util.isRegExp(/a/));
        assert.isTrue(util.isRegExp(new RegExp("a")));
        assert.isFalse(util.isRegExp("a"));
    });

    it('isFunction', () => {
        assert.isTrue(util.isFunction(() => {}));
        assert.isFalse(util.isFunction({}));
        assert.isFalse(util.isFunction(5));
    });

    it('isBuffer', () => {
        assert.isTrue(util.isBuffer(new Buffer(10)));
        assert.isFalse(util.isBuffer({}));
        assert.isFalse(util.isBuffer(5));
    });

    it('isNativeError', () => {
        assert.ok(util.isNativeError(new Error()));
        assert.ok(util.isNativeError(new TypeError()));
        assert.ok(util.isNativeError(new SyntaxError()));

        assert.notOk(util.isNativeError({}));
        assert.notOk(util.isNativeError({
            name: 'Error',
            message: ''
        }));
        assert.notOk(util.isNativeError([]));
        assert.notOk(util.isNativeError(Object.create(Error.prototype)));
    });

    it('isDate', () => {
        assert.ok(util.isDate(new Date()));
        assert.ok(util.isDate(new Date(0)));
        assert.notOk(0);
        assert.notOk(util.isDate(Date()));
        assert.notOk(util.isDate({}));
        assert.notOk(util.isDate([]));
        assert.notOk(util.isDate(new Error()));
        assert.notOk(util.isDate(Object.create(Date.prototype)));
    });

    it('isPrimitive', () => {
        assert.notOk(util.isPrimitive({}));
        assert.notOk(util.isPrimitive(new Error()));
        assert.notOk(util.isPrimitive(new Date()));
        assert.notOk(util.isPrimitive([]));
        assert.notOk(util.isPrimitive(/regexp/));
        assert.notOk(util.isPrimitive(function () {}));
        assert.notOk(util.isPrimitive(new Number(1)));
        assert.notOk(util.isPrimitive(new String('bla')));
        assert.notOk(util.isPrimitive(new Boolean(true)));
        assert.ok(util.isPrimitive(1));
        assert.ok(util.isPrimitive('bla'));
        assert.ok(util.isPrimitive(true));
        assert.ok(util.isPrimitive(undefined));
        assert.ok(util.isPrimitive(null));
        assert.ok(util.isPrimitive(Infinity));
        assert.ok(util.isPrimitive(NaN));
        assert.ok(util.isPrimitive(Symbol('symbol')));
    });

    it('isSymbol', () => {
        assert.ok(util.isSymbol(Symbol()));
        assert.notOk(util.isSymbol('string'));
    });

    it('isDataView', () => {
        var buffer = new ArrayBuffer(2);
        assert.ok(util.isDataView(new DataView(buffer)));
        assert.notOk(util.isDataView(buffer));
    });

    it('isMap', () => {
        assert.ok(util.isMap(new Map()));
        assert.notOk(util.isMap(Map));
        assert.notOk(util.isMap(Map.prototype));
        assert.notOk(util.isMap(new WeakMap()));
        class AMap extends Map {}

        assert.ok(util.isMap(new AMap()));
        assert.notOk(util.isMap(AMap));
        assert.notOk(util.isMap(AMap.prototype));
    });

    it('isMapIterator', () => {
        var m = new Map();
        assert.ok(util.isMapIterator(m[Symbol.iterator]()));
        assert.notOk(util.isMapIterator(m));
        assert.notOk(util.isMapIterator(Map));
        assert.notOk(util.isMapIterator(Map.prototype));
    });

    it('isPromise', () => {
        var p = new Promise(() => {});
        var p1 = () => {};
        p1.then = () => {};

        var p2 = () => {};
        p2.toString = () => '[object Promise]';

        class APromise extends Promise {}

        var p3 = new Promise(() => {});

        assert.ok(util.isPromise(p));
        assert.notOk(util.isPromise(Promise));
        assert.notOk(util.isPromise(Promise.prototype));
        assert.notOk(util.isPromise(p.prototype));
        assert.notOk(util.isPromise(p1));
        assert.notOk(util.isPromise(p2));
        assert.ok(util.isPromise(p3));
    });

    it('isAsyncFunction', () => {
        assert.isTrue(util.isAsyncFunction(async() => {}));
        assert.isTrue(util.isAsyncFunction(async function () {}));
        assert.isTrue(util.isAsyncFunction(async function demo() {}));

        assert.isFalse(util.isAsyncFunction(() => {}));
        assert.isFalse(util.isAsyncFunction(function () {}));
        assert.isFalse(util.isAsyncFunction(function demo() {}));

        assert.isFalse(util.isAsyncFunction(function* () {}));
        assert.isFalse(util.isAsyncFunction(function* demo() {}));
    });

    it('isSet', () => {
        assert.ok(util.isSet(new Set()));
        assert.notOk(util.isSet(Set));
        assert.notOk(util.isSet(Set.prototype));
        assert.notOk(util.isSet(new WeakSet()));
        class ASet extends Set {}

        assert.ok(util.isSet(new ASet()));
        assert.notOk(util.isSet(ASet));
        assert.notOk(util.isSet(ASet.prototype));
    });

    it('isSetIterator', () => {
        var m = new Set();
        assert.ok(util.isSetIterator(m[Symbol.iterator]()));
        assert.notOk(util.isSetIterator(m));
        assert.notOk(util.isSetIterator(Set));
        assert.notOk(util.isSetIterator(Set.prototype));
    });

    it('isTypedArray', () => {
        assert.ok(util.isTypedArray(new Int8Array()));
        assert.ok(util.isTypedArray(new Uint8Array()));
        assert.ok(util.isTypedArray(new Uint8ClampedArray()));
        assert.ok(util.isTypedArray(new Int16Array()));
        assert.ok(util.isTypedArray(new Uint16Array()));
        assert.ok(util.isTypedArray(new Int32Array()));
        assert.ok(util.isTypedArray(new Uint32Array()));
        assert.ok(util.isTypedArray(new Float32Array()));
        assert.ok(util.isTypedArray(new Float64Array()));

        assert.notOk(util.isTypedArray(Int8Array));
        assert.notOk(util.isTypedArray(Uint8Array));
        assert.notOk(util.isTypedArray(Uint8ClampedArray));
        assert.notOk(util.isTypedArray(Int16Array));
        assert.notOk(util.isTypedArray(Uint16Array));
        assert.notOk(util.isTypedArray(Int32Array));
        assert.notOk(util.isTypedArray(Uint32Array));
        assert.notOk(util.isTypedArray(Float32Array));
        assert.notOk(util.isTypedArray(Float64Array));
    });

    it('isUint8Array', () => {
        assert.ok(util.isUint8Array(new Uint8Array()));
        assert.notOk(util.isUint8Array(Uint8Array));

        assert.notOk(util.isUint8Array(new Int8Array()));
        assert.notOk(util.isUint8Array(new Uint8ClampedArray()));
        assert.notOk(util.isUint8Array(new Int16Array()));
        assert.notOk(util.isUint8Array(new Uint16Array()));
        assert.notOk(util.isUint8Array(new Int32Array()));
        assert.notOk(util.isUint8Array(new Uint32Array()));
        assert.notOk(util.isUint8Array(new Float32Array()));
        assert.notOk(util.isUint8Array(new Float64Array()));

        assert.notOk(util.isUint8Array(Int8Array));
        assert.notOk(util.isUint8Array(Uint8ClampedArray));
        assert.notOk(util.isUint8Array(Int16Array));
        assert.notOk(util.isUint8Array(Uint16Array));
        assert.notOk(util.isUint8Array(Int32Array));
        assert.notOk(util.isUint8Array(Uint32Array));
        assert.notOk(util.isUint8Array(Float32Array));
        assert.notOk(util.isUint8Array(Float64Array));
    });

    it('has', () => {
        var obj = {
            foo: 'bar',
            func: () => {}
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
        } catch (ex) {}

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

        var Obj = function () {};
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

        var Obj = function () {};
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

        util.each(false, () => {});

        var a = [1, 2, 3];
        assert.strictEqual(util.each(a, () => {}), a);
        assert.strictEqual(util.each(null, () => {}), null);

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

        assert.deepEqual(util.map(null, () => {}), []);

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

        assert.equal(util.reduce(null, () => {}, 138), 138);
        assert.equal(util.reduce([], () => {}, undefined), undefined);

        assert.throws(() => {
            util.reduce([], () => {});
        });

        assert.throws(() => {
            util.reduce(null, () => {});
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

        it("array", () => {
            assert.equal(util.format([]), '[]');
            assert.equal(util.format(["1"]), '[\n  "1"\n]');
            assert.equal(util.format([100, 200]), '[\n  100,\n  200\n]');
        });

        it("typedarray", () => {
            assert.equal(util.format(new Uint8Array([])), '[]');
            assert.equal(util.format(new Uint8Array([100, 200])), '[\n  100,\n  200\n]');
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

        it("Int64", () => {
            assert.equal(util.format(new Int64(12345678)), '12345678');
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

        it("Function", () => {
            assert.equal(util.format(() => {}), '[Function]');

            assert.equal(util.format([

                () => {}
            ]), '[\n  [Function]\n]');

            assert.equal(util.format({
                a: () => {}
            }), '{\n  "a": [Function]\n}');
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
    });

    describe('Stats', () => {
        var s;

        it("new", () => {
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

            assert.throws(() => {
                var s = new util.Stats();
            });
        });

        it("inc", () => {
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

        it("dec", () => {
            s.dec('a');
            assert.deepEqual(s.toJSON(), {
                "a": 0,
                "b": 0,
                "c": 1
            });
        });

        it("add", () => {
            s.add('a', 100);
            assert.deepEqual(s.toJSON(), {
                "a": 100,
                "b": 0,
                "c": 1
            });
        });

        it("reset", () => {
            s.reset();
            assert.deepEqual(s.toJSON(), {
                "a": 100,
                "b": 0,
                "c": 0
            });
        });

        it("named", () => {
            assert.equal(s.a, 100);
            assert.equal(s["a"], 100);
        });

        it("uptime", () => {
            coroutine.sleep(100);
            assert.closeTo(100, s.uptime(), 100);
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
                util.sync(async() => {
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
                catch (fn) {
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
    });

    describe('LruCache', () => {
        var c;

        function deepEqual(o1, o2) {
            assert.deepEqual(Object.keys(o1), Object.keys(o2));
            assert.deepEqual(o1, o2);
        }

        it("new", () => {
            c = new util.LruCache(3);
            deepEqual(c.toJSON(), {});
        });

        it("set", () => {
            c.set("b", 100);
            deepEqual(c.toJSON(), {
                "b": 100
            });

            c.set({
                "b": 200,
                "a": 100
            });
            deepEqual(c.toJSON(), {
                "a": 100,
                "b": 200
            });
        });

        it("has", () => {
            assert.isTrue(c.has('a'));
            assert.isFalse(c.has('c'));
        });

        it("get", () => {
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

        it("remove", () => {
            c.remove("b");
            deepEqual(c.toJSON(), {
                "a": 100
            });

            c.remove('a');
            deepEqual(c.toJSON(), {});
        });

        it("size", () => {
            c = new util.LruCache(3);
            var vs = [],
                ks = [];

            c.onexpire = evt => {
                ks.push(evt.key);
                vs.push(evt.value);
            };

            assert.equal(c.size, 0);

            c.set('a', 100);
            assert.equal(c.size, 1);

            c.set('b', 100);
            c.set('c', 200);
            c.set('d', 300);
            c.set('e', 400);

            assert.equal(c.size, 3);
            coroutine.sleep(1);
            assert.deepEqual(ks, ['a', 'b']);
            assert.deepEqual(vs, [100, 100]);

            deepEqual(c.toJSON(), {
                "e": 400,
                "d": 300,
                "c": 200
            });
        });

        it("timeout", () => {
            c = new util.LruCache(3, 1000);

            c.set('f', 100);
            c.set('d', 300);
            c.set('e', 400);

            coroutine.sleep(500);
            c.set('f', 500);
            deepEqual(c.toJSON(), {
                "f": 500,
                "e": 400,
                "d": 300
            });

            coroutine.sleep(200);
            deepEqual(c.toJSON(), {
                "f": 500,
                "e": 400,
                "d": 300
            });

            c.set('e', 700);

            assert.equal(c.get('d'), 300);
            coroutine.sleep(510);
            deepEqual(c.toJSON(), {
                "e": 700,
                "f": 500
            });

            assert.equal(c.get('f'), 500);
            coroutine.sleep(300);
            deepEqual(c.toJSON(), {
                "e": 700
            });
        });

        it("change timeout", () => {
            c = new util.LruCache(3);

            c.set('d', 300);
            c.set('e', 400);
            c.set('f', 500);

            coroutine.sleep(500);
            deepEqual(c.toJSON(), {
                "f": 500,
                "e": 400,
                "d": 300
            });

            c.timeout = 100;

            coroutine.sleep(500);
            deepEqual(c.toJSON(), {});
        });

        it("updater", () => {
            var call_num = 0;

            function updater(name) {
                coroutine.sleep(30);
                call_num++;
                return name + "_value";
            }
            c = new util.LruCache(3);

            assert.equal(c.get("a", updater), "a_value");
            assert.equal(call_num, 1);

            assert.isUndefined(c.get("a1", function () {}));
            assert.isFalse(c.has("a1"));

            coroutine.start(() => {
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

            assert.throws(() => {
                c.get("d", () => {
                    throw "some error";
                })
            });

            assert.equal(c.get("c1", (k) => {
                c.set("c1", 200);
                return 100;
            }), 100);
            assert.equal(c.get("c1"), 200);
        });

        it("BUG: not lock object in updater", () => {
            var call_num = 0;
            var enter_num = 0;

            function updater(name) {
                enter_num++;
                coroutine.sleep(30);
                call_num++;
                return name + "_value";
            }
            c = new util.LruCache(3);

            coroutine.start(() => {
                c.get("a", updater);
            });

            assert.equal(call_num, 0);
            coroutine.sleep(1);
            assert.isUndefined(c.get("b"));
            assert.equal(call_num, 0);
            assert.equal(c.get("a"), "a_value");
            assert.equal(call_num, 1);

            coroutine.start(() => {
                c.get("b", updater);
            });
            coroutine.start(() => {
                c.get("c", updater);
            });

            assert.equal(enter_num, 1);
            coroutine.sleep(1);
            assert.equal(enter_num, 3);
            assert.equal(call_num, 1);
        });

        it("Garbage Collection", () => {
            GC();
            var no1 = os.memoryUsage().nativeObjects.objects;

            var lc = new util.LruCache(1024);
            lc.set("test", lc);
            assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);

            lc.set("test1", new Buffer());
            assert.equal(no1 + 2, os.memoryUsage().nativeObjects.objects);

            lc.remove("test1");
            lc = undefined;

            GC();
            assert.equal(no1, os.memoryUsage().nativeObjects.objects);
        });
    });
});

require.main === module && test.run(console.DEBUG);