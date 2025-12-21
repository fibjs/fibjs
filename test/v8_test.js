var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var coroutine = require('coroutine');

var v8 = require("v8");
var fs = require("fs");
var path = require("path");
var os = require("os");

var vmid = coroutine.vmid;

function unlink(pathname) {
    try {
        fs.unlink(pathname);
    } catch (e) { }
}

describe("v8", () => {
    after(() => {
        unlink(path.join(__dirname, "test.heapsnapshot" + vmid));
        unlink(path.join(__dirname, "test1.heapsnapshot" + vmid));
        unlink(path.join(__dirname, "test2.heapsnapshot" + vmid));
    });

    it("take snapshot & dispose", () => {
        var ss = v8.takeSnapshot();
    });

    it("crash when double take", () => {
        v8.takeSnapshot();
        v8.takeSnapshot();
    });

    it("serialize", () => {
        v8.saveSnapshot(path.join(__dirname, "test.heapsnapshot" + vmid));

        var ss = v8.loadSnapshot(path.join(__dirname, "test.heapsnapshot" + vmid));
        ss.save(path.join(__dirname, "test1.heapsnapshot" + vmid));

        var ss = v8.loadSnapshot(path.join(__dirname, "test1.heapsnapshot" + vmid));
        ss.save(path.join(__dirname, "test2.heapsnapshot" + vmid));

        assert.equal(fs.readTextFile(path.join(__dirname, "test1.heapsnapshot" + vmid)),
            fs.readTextFile(path.join(__dirname, "test2.heapsnapshot" + vmid)));
    });

    it("diff", () => {
        var ss = v8.loadSnapshot(path.join(__dirname, "test.heapsnapshot" + vmid));
        var ss1 = v8.loadSnapshot(path.join(__dirname, "test.heapsnapshot" + vmid));

        assert.deepEqual(ss.diff(ss1).change, {
            "size_bytes": 0,
            "size": "0 bytes",
            "freed_nodes": 0,
            "allocated_nodes": 0,
            "details": []
        });
    });

    it("getHeapCodeStatistics", () => {
        var hcs = v8.getHeapCodeStatistics();

        assert.property(hcs, "code_and_metadata_size");
        assert.property(hcs, "bytecode_and_metadata_size");
        assert.property(hcs, "external_script_source_size");
    });

    it("getHeapSpaceStatistics", () => {
        var hss = v8.getHeapSpaceStatistics();

        assert.property(hss[0], "space_name");
        assert.property(hss[0], "space_size");
        assert.property(hss[0], "space_used_size");
        assert.property(hss[0], "space_available_size");
        assert.property(hss[0], "physical_space_size");
    });

    it("getHeapStatistics", () => {
        var hs = v8.getHeapStatistics();

        assert.property(hs, "total_heap_size");
        assert.property(hs, "total_heap_size_executable");
        assert.property(hs, "total_physical_size");
        assert.property(hs, "total_available_size");
        assert.property(hs, "used_heap_size");
        assert.property(hs, "heap_size_limit");
        assert.property(hs, "malloced_memory");
        assert.property(hs, "external_memory");
        assert.property(hs, "peak_malloced_memory");
        assert.property(hs, "number_of_native_contexts");
        assert.property(hs, "number_of_detached_contexts");
    });

    describe("serialize/deserialize", () => {
        it("serialize primitives", () => {
            assert.strictEqual(v8.deserialize(v8.serialize(undefined)), undefined);
            assert.strictEqual(v8.deserialize(v8.serialize(null)), null);
            assert.strictEqual(v8.deserialize(v8.serialize(true)), true);
            assert.strictEqual(v8.deserialize(v8.serialize(false)), false);
            assert.strictEqual(v8.deserialize(v8.serialize('')), '');
            assert.strictEqual(v8.deserialize(v8.serialize('hello')), 'hello');
            assert.strictEqual(v8.deserialize(v8.serialize(42)), 42);
            assert.strictEqual(v8.deserialize(v8.serialize(-3.14)), -3.14);
            assert.strictEqual(v8.deserialize(v8.serialize(Infinity)), Infinity);
            assert.ok(Number.isNaN(v8.deserialize(v8.serialize(NaN))));
            assert.ok(Object.is(v8.deserialize(v8.serialize(-0)), -0));
        });

        it("serialize BigInt", () => {
            assert.strictEqual(v8.deserialize(v8.serialize(0n)), 0n);
            assert.strictEqual(v8.deserialize(v8.serialize(9007199254740993n)), 9007199254740993n);
            assert.strictEqual(v8.deserialize(v8.serialize(-9007199254740993n)), -9007199254740993n);
        });

        it("serialize objects", () => {
            const obj = { a: 1, b: 'hello', c: true };
            assert.deepStrictEqual(v8.deserialize(v8.serialize(obj)), obj);

            const nested = { a: { b: { c: 1 } } };
            assert.deepStrictEqual(v8.deserialize(v8.serialize(nested)), nested);
        });

        it("serialize arrays", () => {
            const arr = [1, 2, 3, 'hello', true, null];
            assert.deepStrictEqual(v8.deserialize(v8.serialize(arr)), arr);
        });

        it("serialize Date", () => {
            const date = new Date('2025-12-21T00:00:00Z');
            const result = v8.deserialize(v8.serialize(date));
            assert.ok(result instanceof Date);
            assert.strictEqual(result.getTime(), date.getTime());
        });

        it("serialize RegExp", () => {
            const regex = /foo/gim;
            const result = v8.deserialize(v8.serialize(regex));
            assert.ok(result instanceof RegExp);
            assert.strictEqual(result.source, regex.source);
            assert.strictEqual(result.flags, regex.flags);
        });

        it("serialize Map", () => {
            const map = new Map([['a', 1], ['b', 2]]);
            const result = v8.deserialize(v8.serialize(map));
            assert.ok(result instanceof Map);
            assert.strictEqual(result.get('a'), 1);
            assert.strictEqual(result.get('b'), 2);
        });

        it("serialize Set", () => {
            const set = new Set([1, 2, 3]);
            const result = v8.deserialize(v8.serialize(set));
            assert.ok(result instanceof Set);
            assert.ok(result.has(1));
            assert.ok(result.has(2));
            assert.strictEqual(result.size, 3);
        });

        it("serialize TypedArrays", () => {
            const uint8 = new Uint8Array([1, 2, 3, 255]);
            const result1 = v8.deserialize(v8.serialize(uint8));
            assert.ok(result1 instanceof Uint8Array);
            assert.deepStrictEqual([...result1], [...uint8]);

            const int32 = new Int32Array([-2147483648, 0, 2147483647]);
            const result2 = v8.deserialize(v8.serialize(int32));
            assert.ok(result2 instanceof Int32Array);
            assert.deepStrictEqual([...result2], [...int32]);

            const float64 = new Float64Array([Math.PI, Math.E]);
            const result3 = v8.deserialize(v8.serialize(float64));
            assert.ok(result3 instanceof Float64Array);
            assert.deepStrictEqual([...result3], [...float64]);

            const bigint64 = new BigInt64Array([0n, 9223372036854775807n]);
            const result4 = v8.deserialize(v8.serialize(bigint64));
            assert.ok(result4 instanceof BigInt64Array);
            assert.deepStrictEqual([...result4], [...bigint64]);
        });

        it("serialize ArrayBuffer", () => {
            const buffer = new ArrayBuffer(8);
            new Uint8Array(buffer).set([1, 2, 3, 4, 5, 6, 7, 8]);
            const result = v8.deserialize(v8.serialize(buffer));
            assert.ok(result instanceof ArrayBuffer);
            assert.strictEqual(result.byteLength, 8);
            assert.deepStrictEqual([...new Uint8Array(result)], [1, 2, 3, 4, 5, 6, 7, 8]);
        });

        it("serialize circular references", () => {
            const obj = { name: 'test' };
            obj.self = obj;
            const result = v8.deserialize(v8.serialize(obj));
            assert.strictEqual(result.name, 'test');
            assert.strictEqual(result.self, result);
        });

        it("serialize shared references", () => {
            const shared = { value: 42 };
            const obj = { a: shared, b: shared };
            const result = v8.deserialize(v8.serialize(obj));
            assert.strictEqual(result.a.value, 42);
            assert.strictEqual(result.a, result.b);
        });

        it("serialize Error objects", () => {
            const err = new Error('test message');
            const result = v8.deserialize(v8.serialize(err));
            assert.ok(result instanceof Error);
            assert.strictEqual(result.message, 'test message');

            const typeErr = new TypeError('type error');
            const result2 = v8.deserialize(v8.serialize(typeErr));
            assert.ok(result2 instanceof TypeError);
            assert.strictEqual(result2.message, 'type error');
        });

        it("serialize returns Buffer", () => {
            const buf = v8.serialize({ test: 1 });
            assert.ok(Buffer.isBuffer(buf));
        });

        it("throw for non-serializable values", () => {
            assert.throws(() => v8.serialize(() => {}));
            assert.throws(() => v8.serialize(Symbol('test')));
            assert.throws(() => v8.serialize(new WeakMap()));
            assert.throws(() => v8.serialize(new WeakSet()));
        });
    });
});
