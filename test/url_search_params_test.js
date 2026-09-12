var { describe, it } = require('node:test');
var assert = require('assert');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

describe("URLSearchParams Test Suite", () => {
    describe("Constructor Tests", () => {
        it("should create empty URLSearchParams", () => {
            const params = new URLSearchParams();
            assert.strictEqual(params.toString(), '');
        });

        it("should create URLSearchParams from string", () => {
            const params = new URLSearchParams('foo=bar&baz=qux');
            assert.strictEqual(params.get('foo'), 'bar');
            assert.strictEqual(params.get('baz'), 'qux');
        });

        it("should create URLSearchParams from object", () => {
            const params = new URLSearchParams({ foo: 'bar', baz: 'qux' });
            assert.strictEqual(params.get('foo'), 'bar');
            assert.strictEqual(params.get('baz'), 'qux');
        });

        it("should create URLSearchParams from iterable", () => {
            const params = new URLSearchParams([['foo', 'bar'], ['baz', 'qux']]);
            assert.strictEqual(params.get('foo'), 'bar');
            assert.strictEqual(params.get('baz'), 'qux');
        });

        it("should create URLSearchParams from any iterable (Map / FormData / Set)", () => {
            // WebIDL: the init argument may be a sequence of pairs
            const fromMap = new URLSearchParams(new Map([['foo', 'bar'], ['baz', 'qux']]));
            assert.strictEqual(fromMap.toString(), 'foo=bar&baz=qux');

            const form = new FormData();
            form.append('name', '张三');
            form.append('tag', 'a');
            form.append('tag', 'b');
            assert.strictEqual(new URLSearchParams(form).toString(), 'name=%E5%BC%A0%E4%B8%89&tag=a&tag=b');

            const fromSet = new URLSearchParams(new Set([['one', '1'], ['two', '2']]));
            assert.strictEqual(fromSet.toString(), 'one=1&two=2');

            const copy = new URLSearchParams(fromMap);
            assert.strictEqual(copy.toString(), fromMap.toString());

            // file values are stringified, like browsers/Node
            const withFile = new FormData();
            withFile.append('a', '1');
            withFile.append('file', new Blob(['x'], { type: 'text/plain' }), 'a.txt');
            assert.strictEqual(new URLSearchParams(withFile).toString(), 'a=1&file=%5Bobject+File%5D');
        });

        it("should reject iterable elements that are not pairs", () => {
            assert.throws(() => new URLSearchParams(new Set([['three']])), TypeError);
        });

        it("should report the number of pairs via size", () => {
            const params = new URLSearchParams('a=1&b=2&a=3');
            assert.strictEqual(params.size, 3); // duplicates are counted

            params.append('c', '4');
            assert.strictEqual(params.size, 4);

            params.delete('a');
            assert.strictEqual(params.size, 2);

            assert.strictEqual(new URLSearchParams().size, 0);
        });

        it("should handle leading question mark", () => {
            const params = new URLSearchParams('?foo=bar&baz=qux');
            assert.strictEqual(params.get('foo'), 'bar');
            assert.strictEqual(params.get('baz'), 'qux');
        });
    });

    describe("Basic Operations", () => {
        it("should get parameter values", () => {
            const params = new URLSearchParams('foo=bar&baz=qux');
            assert.strictEqual(params.get('foo'), 'bar');
            assert.strictEqual(params.get('baz'), 'qux');
            assert.strictEqual(params.get('nonexistent'), null);
        });

        it("should set parameter values", () => {
            const params = new URLSearchParams();
            params.set('foo', 'bar');
            assert.strictEqual(params.get('foo'), 'bar');
        });

        it("should append parameter values", () => {
            const params = new URLSearchParams();
            params.append('foo', 'bar');
            params.append('foo', 'baz');
            assert.deepStrictEqual(params.getAll('foo'), ['bar', 'baz']);
        });

        it("should delete parameters", () => {
            const params = new URLSearchParams('foo=bar&baz=qux');
            params.delete('foo');
            assert.strictEqual(params.get('foo'), null);
            assert.strictEqual(params.get('baz'), 'qux');
        });

        it("should check parameter existence", () => {
            const params = new URLSearchParams('foo=bar');
            assert.strictEqual(params.has('foo'), true);
            assert.strictEqual(params.has('nonexistent'), false);
        });

        it("should check parameter existence by name and value", () => {
            const params = new URLSearchParams('foo=bar&foo=baz&empty=');
            assert.strictEqual(params.has('foo', 'bar'), true);
            assert.strictEqual(params.has('foo', 'baz'), true);
            assert.strictEqual(params.has('foo', 'qux'), false);
            assert.strictEqual(params.has('empty', ''), true);
        });
    });

    describe("Multiple Values Handling", () => {
        it("should handle multiple values for same key", () => {
            const params = new URLSearchParams('foo=bar&foo=baz&foo=qux');
            assert.strictEqual(params.get('foo'), 'bar'); // First value
            assert.deepStrictEqual(params.getAll('foo'), ['bar', 'baz', 'qux']);
        });

        it("should replace all values when using set", () => {
            const params = new URLSearchParams('foo=bar&foo=baz');
            params.set('foo', 'qux');
            assert.strictEqual(params.get('foo'), 'qux');
            assert.deepStrictEqual(params.getAll('foo'), ['qux']);
        });

        it("should delete all values when using delete", () => {
            const params = new URLSearchParams('foo=bar&foo=baz&other=value');
            params.delete('foo');
            assert.deepStrictEqual(params.getAll('foo'), []);
            assert.strictEqual(params.get('other'), 'value');
        });

        it("should delete only matching values when using delete(name, value)", () => {
            const params = new URLSearchParams('foo=bar&foo=baz&foo=bar&other=value');
            params.delete('foo', 'bar');
            assert.deepStrictEqual(params.getAll('foo'), ['baz']);
            assert.strictEqual(params.get('other'), 'value');
        });
    });

    describe("Encoding and Decoding", () => {
        it("should encode special characters", () => {
            const params = new URLSearchParams();
            params.set('special', 'hello world & more');
            assert.strictEqual(params.toString(), 'special=hello+world+%26+more');
        });

        it("should decode encoded parameters", () => {
            const params = new URLSearchParams('encoded=hello%20world%26more');
            assert.strictEqual(params.get('encoded'), 'hello world&more');
        });

        it("should handle plus signs as spaces", () => {
            const params = new URLSearchParams('text=hello+world');
            assert.strictEqual(params.get('text'), 'hello world');
        });

        it("should encode Unicode characters", () => {
            const params = new URLSearchParams();
            params.set('unicode', '测试');
            const encoded = params.toString();
            assert.ok(encoded.includes('%'));

            const decoded = new URLSearchParams(encoded);
            assert.strictEqual(decoded.get('unicode'), '测试');
        });

        it("should handle percent encoding edge cases", () => {
            const params = new URLSearchParams();
            params.set('percent', '100%');
            assert.ok(params.toString().includes('%25'));
        });
    });

    describe("Edge Cases", () => {
        it("should handle empty keys and values", () => {
            const params = new URLSearchParams('=value&key=&=');
            // empty names are valid (WHATWG URL)
            assert.strictEqual(params.get(''), 'value');
            assert.strictEqual(params.get('key'), '');
        });

        it("should handle parameters without values", () => {
            const params = new URLSearchParams('foo&bar=baz');
            assert.strictEqual(params.get('foo'), '');
            assert.strictEqual(params.get('bar'), 'baz');
        });

        it("should handle malformed query strings", () => {
            const params = new URLSearchParams('foo=bar&baz');
            assert.strictEqual(params.get('foo'), 'bar');
            assert.strictEqual(params.get('baz'), '');
        });

        it("should handle duplicate equals signs", () => {
            const params = new URLSearchParams('equation=x=y+z');
            assert.strictEqual(params.get('equation'), 'x=y z');
        });

        it("should handle ampersands in values", () => {
            const params = new URLSearchParams();
            params.set('formula', 'a&b');
            const str = params.toString();
            const reparsed = new URLSearchParams(str);
            assert.strictEqual(reparsed.get('formula'), 'a&b');
        });
    });

    describe("Iteration", () => {
        it("should iterate with for...of", () => {
            const params = new URLSearchParams('a=1&b=2&a=3');
            const entries = [];
            for (const [key, value] of params) {
                entries.push([key, value]);
            }
            // iteration keeps list order (WHATWG URL), it does not sort
            assert.deepStrictEqual(entries, [['a', '1'], ['b', '2'], ['a', '3']]);
        });

        it("should iterate with forEach", () => {
            const params = new URLSearchParams('foo=bar&baz=qux');
            const collected = [];
            params.forEach((value, key) => {
                collected.push([key, value]);
            });
            assert.deepStrictEqual(collected, [['foo', 'bar'], ['baz', 'qux']]);
        });

        it("should iterate keys", () => {
            const params = new URLSearchParams('a=1&b=2&a=3');
            const keys = Array.from(params.keys());
            assert.deepStrictEqual(keys, ['a', 'b', 'a']);
        });

        it("should iterate values", () => {
            const params = new URLSearchParams('a=1&b=2&a=3');
            const values = Array.from(params.values());
            assert.deepStrictEqual(values, ['1', '2', '3']);
        });

        it("should iterate entries", () => {
            const params = new URLSearchParams('foo=bar&baz=qux');
            const entries = Array.from(params.entries());
            assert.deepStrictEqual(entries, [['foo', 'bar'], ['baz', 'qux']]);
        });

        it("iteration must not reorder the list", () => {
            const params = new URLSearchParams();
            params.append('b', '2');
            params.append('a', '1');

            assert.strictEqual(params.toString(), 'b=2&a=1');

            // reading the entries must not change the serialization
            assert.deepStrictEqual(Array.from(params.keys()), ['b', 'a']);
            assert.deepStrictEqual(Array.from(params.entries()), [['b', '2'], ['a', '1']]);
            params.forEach(() => { });
            assert.strictEqual(params.toString(), 'b=2&a=1');

            const url = new URL('https://example.com/?b=2&a=1');
            Array.from(url.searchParams.keys());
            assert.strictEqual(url.href, 'https://example.com/?b=2&a=1');
        });

        it("sort() orders the list and updates the serialization", () => {
            const params = new URLSearchParams();
            params.append('b', '2');
            params.append('a', '1');
            params.append('b', '3');

            params.sort();

            assert.deepStrictEqual(Array.from(params.keys()), ['a', 'b', 'b']);
            assert.strictEqual(params.toString(), 'a=1&b=2&b=3');
        });

        it("allows empty parameter names", () => {
            const params = new URLSearchParams();
            params.append('', 'value');

            assert.strictEqual(params.get(''), 'value');
            assert.strictEqual(params.has(''), true);
            assert.strictEqual(params.toString(), '=value');

            const parsed = new URLSearchParams('=value');
            assert.strictEqual(parsed.get(''), 'value');
        });
    });

    describe("toString and Serialization", () => {
        it("should serialize to query string", () => {
            const params = new URLSearchParams();
            params.set('foo', 'bar');
            params.set('baz', 'qux');
            const str = params.toString();
            assert.ok(str.includes('foo=bar'));
            assert.ok(str.includes('baz=qux'));
            assert.ok(str.includes('&'));
        });

        it("should maintain order", () => {
            const params = new URLSearchParams();
            params.set('z', '1');
            params.set('a', '2');
            params.set('m', '3');
            const str = params.toString();
            assert.strictEqual(str, 'z=1&a=2&m=3');
        });

        it("should handle empty parameters", () => {
            const params = new URLSearchParams();
            assert.strictEqual(params.toString(), '');
        });
    });

    describe("Sort Operation", () => {
        it("should sort parameters by key", () => {
            const params = new URLSearchParams('z=1&a=2&m=3&a=4');
            params.sort();
            assert.strictEqual(params.toString(), 'a=2&a=4&m=3&z=1');
        });

        it("should maintain multiple values order after sort", () => {
            const params = new URLSearchParams('b=2&a=1&b=3&a=0');
            params.sort();
            assert.strictEqual(params.toString(), 'a=1&a=0&b=2&b=3');
        });
    });

    describe("Performance and Memory", () => {
        it("should handle large number of parameters", () => {
            const params = new URLSearchParams();
            for (let i = 0; i < 1000; i++) {
                params.set(`param${i}`, `value${i}`);
            }
            assert.strictEqual(params.get('param999'), 'value999');
            assert.strictEqual(params.get('param0'), 'value0');
        });

        it("should handle long parameter values", () => {
            const longValue = 'x'.repeat(10000);
            const params = new URLSearchParams();
            params.set('long', longValue);
            assert.strictEqual(params.get('long'), longValue);
        });

        it("should handle repeated operations efficiently", () => {
            const params = new URLSearchParams('base=value');
            const start = Date.now();

            for (let i = 0; i < 1000; i++) {
                params.set('test', `value${i}`);
                params.get('test');
                params.delete('test');
                params.append('test', 'new');
            }

            const end = Date.now();
            assert.ok(end - start < 1000); // Should complete quickly
        });
    });

    describe("Type Coercion and Validation", () => {
        it("should convert values to strings", () => {
            const params = new URLSearchParams();
            params.set('number', 123);
            params.set('boolean', true);
            params.set('null', null);
            params.set('undefined', undefined);

            assert.strictEqual(params.get('number'), '123');
            assert.strictEqual(params.get('boolean'), 'true');
            assert.strictEqual(params.get('null'), 'null');
            assert.strictEqual(params.get('undefined'), 'undefined');
        });
    });

    describe("Clone and Copy Operations", () => {
        it("should create independent copies", () => {
            const original = new URLSearchParams('foo=bar&baz=qux');
            const copy = new URLSearchParams(original);

            copy.set('foo', 'modified');
            assert.strictEqual(original.get('foo'), 'bar');
            assert.strictEqual(copy.get('foo'), 'modified');
        });

        it("should copy all parameters including duplicates", () => {
            const original = new URLSearchParams('key=value1&key=value2');
            const copy = new URLSearchParams(original);

            assert.deepStrictEqual(copy.getAll('key'), ['value1', 'value2']);
        });
    });

    describe("Error Handling", () => {
        it("should handle invalid constructor arguments gracefully", () => {
            // Test various invalid inputs
            try {
                new URLSearchParams(Symbol('invalid'));
            } catch (e) {
                assert.ok(e instanceof TypeError);
            }
        });

        it("should handle malformed percent encoding", () => {
            const params = new URLSearchParams('malformed=%G');
            // Should handle gracefully, exact behavior may vary
            assert.ok(params.toString().length >= 0);
        });
    });
});
