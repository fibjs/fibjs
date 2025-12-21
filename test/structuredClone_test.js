const assert = require('assert');
const { describe, it } = require('node:test');

describe('structuredClone', () => {
  describe('Primitive Types', () => {
    it('should clone undefined', () => {
      assert.strictEqual(structuredClone(undefined), undefined);
    });

    it('should clone null', () => {
      assert.strictEqual(structuredClone(null), null);
    });

    it('should clone boolean true', () => {
      assert.strictEqual(structuredClone(true), true);
    });

    it('should clone boolean false', () => {
      assert.strictEqual(structuredClone(false), false);
    });

    it('should clone empty string', () => {
      assert.strictEqual(structuredClone(''), '');
    });

    it('should clone string with content', () => {
      assert.strictEqual(structuredClone('hello'), 'hello');
    });

    it('should clone lone high surrogate', () => {
      assert.strictEqual(structuredClone('\uD800'), '\uD800');
    });

    it('should clone lone low surrogate', () => {
      assert.strictEqual(structuredClone('\uDC00'), '\uDC00');
    });

    it('should clone NUL character', () => {
      assert.strictEqual(structuredClone('\u0000'), '\u0000');
    });

    it('should clone astral character', () => {
      assert.strictEqual(structuredClone('\uDBFF\uDFFD'), '\uDBFF\uDFFD');
    });

    it('should clone number 0', () => {
      assert.strictEqual(structuredClone(0), 0);
    });

    it('should clone number -0', () => {
      assert.strictEqual(Object.is(structuredClone(-0), -0), true);
    });

    it('should clone number 0.2', () => {
      assert.strictEqual(structuredClone(0.2), 0.2);
    });

    it('should clone NaN', () => {
      assert.strictEqual(Number.isNaN(structuredClone(NaN)), true);
    });

    it('should clone Infinity', () => {
      assert.strictEqual(structuredClone(Infinity), Infinity);
    });

    it('should clone -Infinity', () => {
      assert.strictEqual(structuredClone(-Infinity), -Infinity);
    });

    it('should clone large numbers', () => {
      assert.strictEqual(structuredClone(9007199254740992), 9007199254740992);
      assert.strictEqual(structuredClone(-9007199254740992), -9007199254740992);
    });

    it('should clone BigInt', () => {
      assert.strictEqual(structuredClone(0n), 0n);
      assert.strictEqual(structuredClone(-0n), -0n);
      assert.strictEqual(structuredClone(9007199254740994n), 9007199254740994n);
      assert.strictEqual(structuredClone(-9007199254740994000n), -9007199254740994000n);
    });
  });

  describe('Object Types', () => {
    it('should clone plain object', () => {
      const obj = { a: 1, b: 'hello', c: true };
      const cloned = structuredClone(obj);
      assert.deepStrictEqual(cloned, obj);
      assert.notStrictEqual(cloned, obj);
    });

    it('should clone nested object', () => {
      const obj = { a: { b: { c: 1 } } };
      const cloned = structuredClone(obj);
      assert.deepStrictEqual(cloned, obj);
      assert.notStrictEqual(cloned.a, obj.a);
      assert.notStrictEqual(cloned.a.b, obj.a.b);
    });

    it('should clone array', () => {
      const arr = [1, 2, 3, 'hello', true];
      const cloned = structuredClone(arr);
      assert.deepStrictEqual(cloned, arr);
      assert.notStrictEqual(cloned, arr);
    });

    it('should clone nested array', () => {
      const arr = [[1, 2], [3, 4], [[5, 6]]];
      const cloned = structuredClone(arr);
      assert.deepStrictEqual(cloned, arr);
      assert.notStrictEqual(cloned[0], arr[0]);
      assert.notStrictEqual(cloned[2][0], arr[2][0]);
    });

    it('should clone Date', () => {
      const date = new Date(1234567890123);
      const cloned = structuredClone(date);
      assert.ok(cloned instanceof Date);
      assert.strictEqual(cloned.getTime(), date.getTime());
      assert.notStrictEqual(cloned, date);
    });

    it('should clone RegExp', () => {
      const regex = /foo/gim;
      const cloned = structuredClone(regex);
      assert.ok(cloned instanceof RegExp);
      assert.strictEqual(cloned.source, regex.source);
      assert.strictEqual(cloned.flags, regex.flags);
      assert.notStrictEqual(cloned, regex);
    });

    it('should clone RegExp with lastIndex reset to 0', () => {
      const regex = /foo/g;
      regex.lastIndex = 5;
      const cloned = structuredClone(regex);
      assert.strictEqual(cloned.lastIndex, 0);
    });

    it('should clone Map', () => {
      const map = new Map([['a', 1], ['b', 2]]);
      const cloned = structuredClone(map);
      assert.ok(cloned instanceof Map);
      assert.strictEqual(cloned.get('a'), 1);
      assert.strictEqual(cloned.get('b'), 2);
      assert.notStrictEqual(cloned, map);
    });

    it('should clone Set', () => {
      const set = new Set([1, 2, 3]);
      const cloned = structuredClone(set);
      assert.ok(cloned instanceof Set);
      assert.ok(cloned.has(1));
      assert.ok(cloned.has(2));
      assert.ok(cloned.has(3));
      assert.notStrictEqual(cloned, set);
    });

    it('should clone Boolean object', () => {
      const bool = new Boolean(true);
      const cloned = structuredClone(bool);
      assert.ok(cloned instanceof Boolean);
      assert.strictEqual(cloned.valueOf(), true);
      assert.notStrictEqual(cloned, bool);
    });

    it('should clone Number object', () => {
      const num = new Number(42);
      const cloned = structuredClone(num);
      assert.ok(cloned instanceof Number);
      assert.strictEqual(cloned.valueOf(), 42);
      assert.notStrictEqual(cloned, num);
    });

    it('should clone String object', () => {
      const str = new String('hello');
      const cloned = structuredClone(str);
      assert.ok(cloned instanceof String);
      assert.strictEqual(cloned.valueOf(), 'hello');
      assert.notStrictEqual(cloned, str);
    });
  });

  describe('ArrayBuffer and TypedArrays', () => {
    it('should clone ArrayBuffer', () => {
      const buffer = new ArrayBuffer(8);
      const view = new Uint8Array(buffer);
      view[0] = 1;
      view[7] = 255;
      const cloned = structuredClone(buffer);
      assert.ok(cloned instanceof ArrayBuffer);
      assert.strictEqual(cloned.byteLength, 8);
      assert.notStrictEqual(cloned, buffer);
      const clonedView = new Uint8Array(cloned);
      assert.strictEqual(clonedView[0], 1);
      assert.strictEqual(clonedView[7], 255);
    });

    it('should clone Uint8Array', () => {
      const arr = new Uint8Array([1, 2, 3, 255]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof Uint8Array);
      assert.deepStrictEqual([...cloned], [...arr]);
      assert.notStrictEqual(cloned, arr);
      assert.notStrictEqual(cloned.buffer, arr.buffer);
    });

    it('should clone Int8Array', () => {
      const arr = new Int8Array([-128, 0, 127]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof Int8Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone Uint16Array', () => {
      const arr = new Uint16Array([0, 65535]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof Uint16Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone Int16Array', () => {
      const arr = new Int16Array([-32768, 32767]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof Int16Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone Uint32Array', () => {
      const arr = new Uint32Array([0, 4294967295]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof Uint32Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone Int32Array', () => {
      const arr = new Int32Array([-2147483648, 2147483647]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof Int32Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone Float32Array', () => {
      const arr = new Float32Array([1.5, -2.5, 3.14]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof Float32Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone Float64Array', () => {
      const arr = new Float64Array([1.5, -2.5, Math.PI]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof Float64Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone BigInt64Array', () => {
      const arr = new BigInt64Array([0n, -9223372036854775808n, 9223372036854775807n]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof BigInt64Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone BigUint64Array', () => {
      const arr = new BigUint64Array([0n, 18446744073709551615n]);
      const cloned = structuredClone(arr);
      assert.ok(cloned instanceof BigUint64Array);
      assert.deepStrictEqual([...cloned], [...arr]);
    });

    it('should clone DataView', () => {
      const buffer = new ArrayBuffer(8);
      const view = new DataView(buffer, 2, 4);
      view.setInt32(0, 12345);
      const cloned = structuredClone(view);
      assert.ok(cloned instanceof DataView);
      assert.strictEqual(cloned.byteOffset, 2);
      assert.strictEqual(cloned.byteLength, 4);
      assert.strictEqual(cloned.getInt32(0), 12345);
      assert.notStrictEqual(cloned.buffer, view.buffer);
    });
  });

  describe('Transfer', () => {
    it('should transfer ArrayBuffer', () => {
      const buffer = new ArrayBuffer(8);
      const view = new Uint8Array(buffer);
      view[0] = 42;
      const cloned = structuredClone(buffer, { transfer: [buffer] });
      assert.ok(cloned instanceof ArrayBuffer);
      assert.strictEqual(cloned.byteLength, 8);
      const clonedView = new Uint8Array(cloned);
      assert.strictEqual(clonedView[0], 42);
      // Original buffer should be detached
      assert.strictEqual(buffer.byteLength, 0);
    });

    it('should transfer multiple ArrayBuffers', () => {
      const buffer1 = new ArrayBuffer(4);
      const buffer2 = new ArrayBuffer(8);
      new Uint8Array(buffer1)[0] = 1;
      new Uint8Array(buffer2)[0] = 2;
      const obj = { a: buffer1, b: buffer2 };
      const cloned = structuredClone(obj, { transfer: [buffer1, buffer2] });
      assert.strictEqual(new Uint8Array(cloned.a)[0], 1);
      assert.strictEqual(new Uint8Array(cloned.b)[0], 2);
      assert.strictEqual(buffer1.byteLength, 0);
      assert.strictEqual(buffer2.byteLength, 0);
    });

    it('should throw when transferring already detached buffer', () => {
      const buffer = new ArrayBuffer(8);
      structuredClone(buffer, { transfer: [buffer] });
      assert.throws(() => {
        structuredClone(buffer, { transfer: [buffer] });
      });
    });

    it('should accept empty transfer array', () => {
      const obj = { a: 1 };
      const cloned = structuredClone(obj, { transfer: [] });
      assert.deepStrictEqual(cloned, obj);
    });

    it('should accept undefined options', () => {
      const obj = { a: 1 };
      const cloned = structuredClone(obj, undefined);
      assert.deepStrictEqual(cloned, obj);
    });

    it('should accept null options', () => {
      const obj = { a: 1 };
      const cloned = structuredClone(obj, null);
      assert.deepStrictEqual(cloned, obj);
    });
  });

  describe('Error Cases', () => {
    it('should throw for function', () => {
      assert.throws(() => {
        structuredClone(() => {});
      });
    });

    it('should throw for Symbol', () => {
      assert.throws(() => {
        structuredClone(Symbol('test'));
      });
    });

    it('should throw for WeakMap', () => {
      assert.throws(() => {
        structuredClone(new WeakMap());
      });
    });

    it('should throw for WeakSet', () => {
      assert.throws(() => {
        structuredClone(new WeakSet());
      });
    });

    it('should throw for object with function property', () => {
      assert.throws(() => {
        structuredClone({ fn: () => {} });
      });
    });
  });

  describe('Circular References', () => {
    it('should handle circular object references', () => {
      const obj = { a: 1 };
      obj.self = obj;
      const cloned = structuredClone(obj);
      assert.strictEqual(cloned.a, 1);
      assert.strictEqual(cloned.self, cloned);
      assert.notStrictEqual(cloned, obj);
    });

    it('should handle circular array references', () => {
      const arr = [1, 2, 3];
      arr.push(arr);
      const cloned = structuredClone(arr);
      assert.strictEqual(cloned[0], 1);
      assert.strictEqual(cloned[3], cloned);
      assert.notStrictEqual(cloned, arr);
    });

    it('should handle shared references', () => {
      const shared = { value: 42 };
      const obj = { a: shared, b: shared };
      const cloned = structuredClone(obj);
      assert.strictEqual(cloned.a.value, 42);
      assert.strictEqual(cloned.a, cloned.b);
      assert.notStrictEqual(cloned.a, shared);
    });
  });

  describe('Error objects', () => {
    it('should clone Error', () => {
      const err = new Error('test message');
      const cloned = structuredClone(err);
      assert.ok(cloned instanceof Error);
      assert.strictEqual(cloned.message, 'test message');
      assert.notStrictEqual(cloned, err);
    });

    it('should clone TypeError', () => {
      const err = new TypeError('type error');
      const cloned = structuredClone(err);
      assert.ok(cloned instanceof TypeError);
      assert.strictEqual(cloned.message, 'type error');
    });

    it('should clone RangeError', () => {
      const err = new RangeError('range error');
      const cloned = structuredClone(err);
      assert.ok(cloned instanceof RangeError);
      assert.strictEqual(cloned.message, 'range error');
    });

    it('should clone SyntaxError', () => {
      const err = new SyntaxError('syntax error');
      const cloned = structuredClone(err);
      assert.ok(cloned instanceof SyntaxError);
      assert.strictEqual(cloned.message, 'syntax error');
    });

    it('should clone ReferenceError', () => {
      const err = new ReferenceError('reference error');
      const cloned = structuredClone(err);
      assert.ok(cloned instanceof ReferenceError);
      assert.strictEqual(cloned.message, 'reference error');
    });

    it('should clone EvalError', () => {
      const err = new EvalError('eval error');
      const cloned = structuredClone(err);
      assert.ok(cloned instanceof EvalError);
      assert.strictEqual(cloned.message, 'eval error');
    });

    it('should clone URIError', () => {
      const err = new URIError('uri error');
      const cloned = structuredClone(err);
      assert.ok(cloned instanceof URIError);
      assert.strictEqual(cloned.message, 'uri error');
    });
  });
});
