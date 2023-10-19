var test = require("test");
test.setup();

var test_util = require('./test_util');

var types = require('util').types;

describe('types', () => {
    it("isEmpty", () => {
        assert.isTrue(types.isEmpty(null));
        assert.isTrue(types.isEmpty(undefined));

        assert.isTrue(types.isEmpty([]));
        assert.isFalse(types.isEmpty([100]));

        assert.isTrue(types.isEmpty(""));
        assert.isFalse(types.isEmpty("a"));

        assert.isTrue(types.isEmpty({}));
        assert.isFalse(types.isEmpty({
            a: 100
        }));
    });

    it("isArray", () => {
        assert.isTrue(types.isArray([]));
        assert.isTrue(types.isArray(Array()));
        assert.isTrue(types.isArray(new Array()));
        assert.isTrue(types.isArray(new Array(5)));
        assert.isTrue(types.isArray(new Array('with', 'some', 'entries')));
        assert.isFalse(types.isArray({}));
        assert.isFalse(types.isArray({
            push: () => { }
        }));
        assert.isFalse(types.isArray(/regexp/));
        assert.isFalse(types.isArray(new Error));
        assert.isFalse(types.isArray(Object.create(Array.prototype)));
    });

    it("isRegExp", () => {
        assert.isTrue(types.isRegExp(/regexp/));
        assert.isTrue(types.isRegExp(RegExp()));
        assert.isTrue(types.isRegExp(new RegExp()));
        assert.isFalse(types.isRegExp({}));
        assert.isFalse(types.isRegExp([]));
        assert.isFalse(types.isRegExp(new Date()));
        assert.isFalse(types.isRegExp(Object.create(RegExp.prototype)));
    });

    it("isObject", () => {
        assert.isTrue(types.isObject({}));
    });

    it('isNull', () => {
        assert.isTrue(types.isNull(null));
        assert.isFalse(types.isNull(undefined));
        assert.isFalse(types.isNull(100));
    });

    it('isUndefined', () => {
        assert.isFalse(types.isUndefined(null));
        assert.isTrue(types.isUndefined(undefined));
        assert.isFalse(types.isUndefined(100));
    });

    it('isNullOrUndefined', () => {
        assert.isTrue(types.isNullOrUndefined(null));
        assert.isTrue(types.isNullOrUndefined(undefined));
        assert.isFalse(types.isNullOrUndefined(100));
    });

    it('isNumber', () => {
        assert.isTrue(types.isNumber(1));
        assert.isTrue(types.isNumber(Number('3')));

        assert.isFalse(types.isNumber('1'));

        assert.isFalse(types.isNumber('hello'));
        assert.isFalse(types.isNumber([5]));
    });

    it('isBigInt', () => {
        assert.isTrue(types.isBigInt(1n));
        assert.isTrue(types.isBigInt(BigInt('3')));

        assert.isFalse(types.isBigInt('1n'));

        assert.isFalse(types.isBigInt('hello'));
        assert.isFalse(types.isBigInt([5n]));
    });

    it('isString', () => {
        assert.isTrue(types.isString('Foo'));
        assert.isTrue(types.isString(new String('foo')));

        assert.isFalse(types.isString(1));

        assert.isFalse(types.isString(3));
        assert.isFalse(types.isString(['hello']));
    });

    it('isRegExp', () => {
        assert.isTrue(types.isRegExp(/a/));
        assert.isTrue(types.isRegExp(new RegExp("a")));
        assert.isFalse(types.isRegExp("a"));
    });

    it('isFunction', () => {
        assert.isTrue(types.isFunction(() => { }));
        assert.isFalse(types.isFunction({}));
        assert.isFalse(types.isFunction(5));
    });

    it('isBuffer', () => {
        assert.isTrue(types.isBuffer(Buffer.alloc(10)));
        assert.isFalse(types.isBuffer({}));
        assert.isFalse(types.isBuffer(5));
    });

    it('isNativeError', () => {
        assert.ok(types.isNativeError(new Error()));
        assert.ok(types.isNativeError(new TypeError()));
        assert.ok(types.isNativeError(new SyntaxError()));

        assert.notOk(types.isNativeError({}));
        assert.notOk(types.isNativeError({
            name: 'Error',
            message: ''
        }));
        assert.notOk(types.isNativeError([]));
        assert.notOk(types.isNativeError(Object.create(Error.prototype)));
    });

    it('isDate', () => {
        assert.ok(types.isDate(new Date()));
        assert.ok(types.isDate(new Date(0)));
        assert.notOk(0);
        assert.notOk(types.isDate(Date()));
        assert.notOk(types.isDate({}));
        assert.notOk(types.isDate([]));
        assert.notOk(types.isDate(new Error()));
        assert.notOk(types.isDate(Object.create(Date.prototype)));
    });

    it('isPrimitive', () => {
        assert.notOk(types.isPrimitive({}));
        assert.notOk(types.isPrimitive(new Error()));
        assert.notOk(types.isPrimitive(new Date()));
        assert.notOk(types.isPrimitive([]));
        assert.notOk(types.isPrimitive(/regexp/));
        assert.notOk(types.isPrimitive(function () { }));
        assert.notOk(types.isPrimitive(new Number(1)));
        assert.notOk(types.isPrimitive(new String('bla')));
        assert.notOk(types.isPrimitive(new Boolean(true)));
        assert.ok(types.isPrimitive(1));
        assert.ok(types.isPrimitive('bla'));
        assert.ok(types.isPrimitive(true));
        assert.ok(types.isPrimitive(undefined));
        assert.ok(types.isPrimitive(null));
        assert.ok(types.isPrimitive(Infinity));
        assert.ok(types.isPrimitive(NaN));
        assert.ok(types.isPrimitive(Symbol('symbol')));
    });

    it('isSymbol', () => {
        assert.ok(types.isSymbol(Symbol()));
        assert.notOk(types.isSymbol('string'));
    });

    it('isDataView', () => {
        var buffer = new ArrayBuffer(2);
        assert.ok(types.isDataView(new DataView(buffer)));
        assert.notOk(types.isDataView(buffer));
    });

    it('isMap', () => {
        assert.ok(types.isMap(new Map()));
        assert.notOk(types.isMap(Map));
        assert.notOk(types.isMap(Map.prototype));
        assert.notOk(types.isMap(new WeakMap()));
        class AMap extends Map { }

        assert.ok(types.isMap(new AMap()));
        assert.notOk(types.isMap(AMap));
        assert.notOk(types.isMap(AMap.prototype));
    });

    it('isMapIterator', () => {
        var m = new Map();
        assert.ok(types.isMapIterator(m[Symbol.iterator]()));
        assert.notOk(types.isMapIterator(m));
        assert.notOk(types.isMapIterator(Map));
        assert.notOk(types.isMapIterator(Map.prototype));
    });

    it('isPromise', () => {
        var p = new Promise(() => { });
        var p1 = () => { };
        p1.then = () => { };

        var p2 = () => { };
        p2.toString = () => '[object Promise]';

        class APromise extends Promise { }

        var p3 = new Promise(() => { });

        assert.ok(types.isPromise(p));
        assert.notOk(types.isPromise(Promise));
        assert.notOk(types.isPromise(Promise.prototype));
        assert.notOk(types.isPromise(p.prototype));
        assert.notOk(types.isPromise(p1));
        assert.notOk(types.isPromise(p2));
        assert.ok(types.isPromise(p3));
    });

    it('isAsyncFunction', () => {
        assert.isTrue(types.isAsyncFunction(async () => { }));
        assert.isTrue(types.isAsyncFunction(async function () { }));
        assert.isTrue(types.isAsyncFunction(async function demo() { }));

        assert.isFalse(types.isAsyncFunction(() => { }));
        assert.isFalse(types.isAsyncFunction(function () { }));
        assert.isFalse(types.isAsyncFunction(function demo() { }));

        assert.isFalse(types.isAsyncFunction(function* () { }));
        assert.isFalse(types.isAsyncFunction(function* demo() { }));
    });

    it('isSet', () => {
        assert.ok(types.isSet(new Set()));
        assert.notOk(types.isSet(Set));
        assert.notOk(types.isSet(Set.prototype));
        assert.notOk(types.isSet(new WeakSet()));
        class ASet extends Set { }

        assert.ok(types.isSet(new ASet()));
        assert.notOk(types.isSet(ASet));
        assert.notOk(types.isSet(ASet.prototype));
    });

    it('isSetIterator', () => {
        var m = new Set();
        assert.ok(types.isSetIterator(m[Symbol.iterator]()));
        assert.notOk(types.isSetIterator(m));
        assert.notOk(types.isSetIterator(Set));
        assert.notOk(types.isSetIterator(Set.prototype));
    });

    it('isTypedArray', () => {
        assert.ok(types.isTypedArray(new Int8Array()));
        assert.ok(types.isTypedArray(new Uint8Array()));
        assert.ok(types.isTypedArray(new Uint8ClampedArray()));
        assert.ok(types.isTypedArray(new Int16Array()));
        assert.ok(types.isTypedArray(new Uint16Array()));
        assert.ok(types.isTypedArray(new Int32Array()));
        assert.ok(types.isTypedArray(new Uint32Array()));
        assert.ok(types.isTypedArray(new Float32Array()));
        assert.ok(types.isTypedArray(new Float64Array()));

        assert.notOk(types.isTypedArray(Int8Array));
        assert.notOk(types.isTypedArray(Uint8Array));
        assert.notOk(types.isTypedArray(Uint8ClampedArray));
        assert.notOk(types.isTypedArray(Int16Array));
        assert.notOk(types.isTypedArray(Uint16Array));
        assert.notOk(types.isTypedArray(Int32Array));
        assert.notOk(types.isTypedArray(Uint32Array));
        assert.notOk(types.isTypedArray(Float32Array));
        assert.notOk(types.isTypedArray(Float64Array));
    });

    it('isFloat32Array', () => {
        assert.notOk(types.isFloat32Array(new Uint8Array()));
        assert.notOk(types.isFloat32Array(Uint8Array));

        assert.notOk(types.isFloat32Array(new Int8Array()));
        assert.notOk(types.isFloat32Array(new Uint8ClampedArray()));
        assert.notOk(types.isFloat32Array(new Int16Array()));
        assert.notOk(types.isFloat32Array(new Uint16Array()));
        assert.notOk(types.isFloat32Array(new Int32Array()));
        assert.notOk(types.isFloat32Array(new Uint32Array()));
        assert.ok(types.isFloat32Array(new Float32Array()));
        assert.notOk(types.isFloat32Array(new Float64Array()));

        assert.notOk(types.isFloat32Array(Int8Array));
        assert.notOk(types.isFloat32Array(Uint8ClampedArray));
        assert.notOk(types.isFloat32Array(Int16Array));
        assert.notOk(types.isFloat32Array(Uint16Array));
        assert.notOk(types.isFloat32Array(Int32Array));
        assert.notOk(types.isFloat32Array(Uint32Array));
        assert.notOk(types.isFloat32Array(Float32Array));
        assert.notOk(types.isFloat32Array(Float64Array));
    });

    it('isFloat64Array', () => {
        assert.notOk(types.isFloat64Array(new Uint8Array()));
        assert.notOk(types.isFloat64Array(Uint8Array));

        assert.notOk(types.isFloat64Array(new Int8Array()));
        assert.notOk(types.isFloat64Array(new Uint8ClampedArray()));
        assert.notOk(types.isFloat64Array(new Int16Array()));
        assert.notOk(types.isFloat64Array(new Uint16Array()));
        assert.notOk(types.isFloat64Array(new Int32Array()));
        assert.notOk(types.isFloat64Array(new Uint32Array()));
        assert.notOk(types.isFloat64Array(new Float32Array()));
        assert.ok(types.isFloat64Array(new Float64Array()));

        assert.notOk(types.isFloat64Array(Int8Array));
        assert.notOk(types.isFloat64Array(Uint8ClampedArray));
        assert.notOk(types.isFloat64Array(Int16Array));
        assert.notOk(types.isFloat64Array(Uint16Array));
        assert.notOk(types.isFloat64Array(Int32Array));
        assert.notOk(types.isFloat64Array(Uint32Array));
        assert.notOk(types.isFloat64Array(Float32Array));
        assert.notOk(types.isFloat64Array(Float64Array));
    });

    it('isInt8Array', () => {
        assert.notOk(types.isInt8Array(new Uint8Array()));
        assert.notOk(types.isInt8Array(Uint8Array));

        assert.ok(types.isInt8Array(new Int8Array()));
        assert.notOk(types.isInt8Array(new Uint8ClampedArray()));
        assert.notOk(types.isInt8Array(new Int16Array()));
        assert.notOk(types.isInt8Array(new Uint16Array()));
        assert.notOk(types.isInt8Array(new Int32Array()));
        assert.notOk(types.isInt8Array(new Uint32Array()));
        assert.notOk(types.isInt8Array(new Float32Array()));
        assert.notOk(types.isInt8Array(new Float64Array()));

        assert.notOk(types.isInt8Array(Int8Array));
        assert.notOk(types.isInt8Array(Uint8ClampedArray));
        assert.notOk(types.isInt8Array(Int16Array));
        assert.notOk(types.isInt8Array(Uint16Array));
        assert.notOk(types.isInt8Array(Int32Array));
        assert.notOk(types.isInt8Array(Uint32Array));
        assert.notOk(types.isInt8Array(Float32Array));
        assert.notOk(types.isInt8Array(Float64Array));
    });

    it('isInt16Array', () => {
        assert.notOk(types.isInt16Array(new Uint8Array()));
        assert.notOk(types.isInt16Array(Uint8Array));

        assert.notOk(types.isInt16Array(new Int8Array()));
        assert.notOk(types.isInt16Array(new Uint8ClampedArray()));
        assert.ok(types.isInt16Array(new Int16Array()));
        assert.notOk(types.isInt16Array(new Uint16Array()));
        assert.notOk(types.isInt16Array(new Int32Array()));
        assert.notOk(types.isInt16Array(new Uint32Array()));
        assert.notOk(types.isInt16Array(new Float32Array()));
        assert.notOk(types.isInt16Array(new Float64Array()));

        assert.notOk(types.isInt16Array(Int8Array));
        assert.notOk(types.isInt16Array(Uint8ClampedArray));
        assert.notOk(types.isInt16Array(Int16Array));
        assert.notOk(types.isInt16Array(Uint16Array));
        assert.notOk(types.isInt16Array(Int32Array));
        assert.notOk(types.isInt16Array(Uint32Array));
        assert.notOk(types.isInt16Array(Float32Array));
        assert.notOk(types.isInt16Array(Float64Array));
    });

    it('isInt32Array', () => {
        assert.notOk(types.isInt32Array(new Uint8Array()));
        assert.notOk(types.isInt32Array(Uint8Array));

        assert.notOk(types.isInt32Array(new Int8Array()));
        assert.notOk(types.isInt32Array(new Uint8ClampedArray()));
        assert.notOk(types.isInt32Array(new Int16Array()));
        assert.notOk(types.isInt32Array(new Uint16Array()));
        assert.ok(types.isInt32Array(new Int32Array()));
        assert.notOk(types.isInt32Array(new Uint32Array()));
        assert.notOk(types.isInt32Array(new Float32Array()));
        assert.notOk(types.isInt32Array(new Float64Array()));

        assert.notOk(types.isInt32Array(Int8Array));
        assert.notOk(types.isInt32Array(Uint8ClampedArray));
        assert.notOk(types.isInt32Array(Int16Array));
        assert.notOk(types.isInt32Array(Uint16Array));
        assert.notOk(types.isInt32Array(Int32Array));
        assert.notOk(types.isInt32Array(Uint32Array));
        assert.notOk(types.isInt32Array(Float32Array));
        assert.notOk(types.isInt32Array(Float64Array));
    });

    it('isUint8Array', () => {
        assert.ok(types.isUint8Array(new Uint8Array()));
        assert.notOk(types.isUint8Array(Uint8Array));

        assert.notOk(types.isUint8Array(new Int8Array()));
        assert.notOk(types.isUint8Array(new Uint8ClampedArray()));
        assert.notOk(types.isUint8Array(new Int16Array()));
        assert.notOk(types.isUint8Array(new Uint16Array()));
        assert.notOk(types.isUint8Array(new Int32Array()));
        assert.notOk(types.isUint8Array(new Uint32Array()));
        assert.notOk(types.isUint8Array(new Float32Array()));
        assert.notOk(types.isUint8Array(new Float64Array()));

        assert.notOk(types.isUint8Array(Int8Array));
        assert.notOk(types.isUint8Array(Uint8ClampedArray));
        assert.notOk(types.isUint8Array(Int16Array));
        assert.notOk(types.isUint8Array(Uint16Array));
        assert.notOk(types.isUint8Array(Int32Array));
        assert.notOk(types.isUint8Array(Uint32Array));
        assert.notOk(types.isUint8Array(Float32Array));
        assert.notOk(types.isUint8Array(Float64Array));
    });

    it('isUint8ClampedArray', () => {
        assert.notOk(types.isUint8ClampedArray(new Uint8Array()));
        assert.notOk(types.isUint8ClampedArray(Uint8Array));

        assert.notOk(types.isUint8ClampedArray(new Int8Array()));
        assert.ok(types.isUint8ClampedArray(new Uint8ClampedArray()));
        assert.notOk(types.isUint8ClampedArray(new Int16Array()));
        assert.notOk(types.isUint8ClampedArray(new Uint16Array()));
        assert.notOk(types.isUint8ClampedArray(new Int32Array()));
        assert.notOk(types.isUint8ClampedArray(new Uint32Array()));
        assert.notOk(types.isUint8ClampedArray(new Float32Array()));
        assert.notOk(types.isUint8ClampedArray(new Float64Array()));

        assert.notOk(types.isUint8ClampedArray(Int8Array));
        assert.notOk(types.isUint8ClampedArray(Uint8ClampedArray));
        assert.notOk(types.isUint8ClampedArray(Int16Array));
        assert.notOk(types.isUint8ClampedArray(Uint16Array));
        assert.notOk(types.isUint8ClampedArray(Int32Array));
        assert.notOk(types.isUint8ClampedArray(Uint32Array));
        assert.notOk(types.isUint8ClampedArray(Float32Array));
        assert.notOk(types.isUint8ClampedArray(Float64Array));
    });

    it('isUint16Array', () => {
        assert.notOk(types.isUint16Array(new Uint8Array()));
        assert.notOk(types.isUint16Array(Uint8Array));

        assert.notOk(types.isUint16Array(new Int8Array()));
        assert.notOk(types.isUint16Array(new Uint8ClampedArray()));
        assert.notOk(types.isUint16Array(new Int16Array()));
        assert.ok(types.isUint16Array(new Uint16Array()));
        assert.notOk(types.isUint16Array(new Int32Array()));
        assert.notOk(types.isUint16Array(new Uint32Array()));
        assert.notOk(types.isUint16Array(new Float32Array()));
        assert.notOk(types.isUint16Array(new Float64Array()));

        assert.notOk(types.isUint16Array(Int8Array));
        assert.notOk(types.isUint16Array(Uint8ClampedArray));
        assert.notOk(types.isUint16Array(Int16Array));
        assert.notOk(types.isUint16Array(Uint16Array));
        assert.notOk(types.isUint16Array(Int32Array));
        assert.notOk(types.isUint16Array(Uint32Array));
        assert.notOk(types.isUint16Array(Float32Array));
        assert.notOk(types.isUint16Array(Float64Array));
    });

    it('isUint32Array', () => {
        assert.notOk(types.isUint32Array(new Uint8Array()));
        assert.notOk(types.isUint32Array(Uint8Array));

        assert.notOk(types.isUint32Array(new Int8Array()));
        assert.notOk(types.isUint32Array(new Uint8ClampedArray()));
        assert.notOk(types.isUint32Array(new Int16Array()));
        assert.notOk(types.isUint32Array(new Uint16Array()));
        assert.notOk(types.isUint32Array(new Int32Array()));
        assert.ok(types.isUint32Array(new Uint32Array()));
        assert.notOk(types.isUint32Array(new Float32Array()));
        assert.notOk(types.isUint32Array(new Float64Array()));

        assert.notOk(types.isUint32Array(Int8Array));
        assert.notOk(types.isUint32Array(Uint8ClampedArray));
        assert.notOk(types.isUint32Array(Int16Array));
        assert.notOk(types.isUint32Array(Uint16Array));
        assert.notOk(types.isUint32Array(Int32Array));
        assert.notOk(types.isUint32Array(Uint32Array));
        assert.notOk(types.isUint32Array(Float32Array));
        assert.notOk(types.isUint32Array(Float64Array));
    });
});

require.main === module && test.run(console.DEBUG);