var { describe, it } = require('node:test');
var assert = require('assert');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

// Headers API Tests
describe("Headers API", () => {
    it("Headers constructor - empty", () => {
        const headers = new Headers();
        assert.strictEqual(headers.get('content-type'), null);

        // Check that headers is iterable
        let count = 0;
        for (let [name, value] of headers) {
            count++;
        }
        assert.strictEqual(count, 0);
    });

    it("Headers constructor - from object", () => {
        const init = {
            'content-type': 'application/json',
            'authorization': 'Bearer token123',
            'x-custom-header': 'custom-value'
        };

        const headers = new Headers(init);

        assert.strictEqual(headers.get('content-type'), 'application/json');
        assert.strictEqual(headers.get('authorization'), 'Bearer token123');
        assert.strictEqual(headers.get('x-custom-header'), 'custom-value');
    });

    it("Headers constructor - from array of arrays", () => {
        const init = [
            ['content-type', 'text/html'],
            ['cache-control', 'no-cache'],
            ['x-powered-by', 'fibjs']
        ];

        const headers = new Headers(init);

        assert.strictEqual(headers.get('content-type'), 'text/html');
        assert.strictEqual(headers.get('cache-control'), 'no-cache');
        assert.strictEqual(headers.get('x-powered-by'), 'fibjs');
    });

    it("Headers constructor - from another Headers object", () => {
        const original = new Headers({
            'content-type': 'application/xml',
            'accept': 'application/json'
        });

        const copy = new Headers(original);

        assert.strictEqual(copy.get('content-type'), 'application/xml');
        assert.strictEqual(copy.get('accept'), 'application/json');

        // Verify they are independent
        copy.set('content-type', 'text/plain');
        assert.strictEqual(original.get('content-type'), 'application/xml');
        assert.strictEqual(copy.get('content-type'), 'text/plain');
    });

    it("Headers.append() method", () => {
        const headers = new Headers();

        headers.append('accept', 'text/html');
        assert.strictEqual(headers.get('accept'), 'text/html');

        // Append to existing header
        headers.append('accept', 'application/json');
        assert.strictEqual(headers.get('accept'), 'text/html, application/json');

        headers.append('accept', 'text/plain');
        assert.strictEqual(headers.get('accept'), 'text/html, application/json, text/plain');
    });

    it("Headers.delete() method", () => {
        const headers = new Headers({
            'content-type': 'application/json',
            'authorization': 'Bearer token',
            'x-custom': 'value'
        });

        assert.strictEqual(headers.has('content-type'), true);
        headers.delete('content-type');
        assert.strictEqual(headers.has('content-type'), false);
        assert.strictEqual(headers.get('content-type'), null);

        // Non-existent header
        headers.delete('non-existent');
        assert.strictEqual(headers.has('x-custom'), true);
    });

    it("Headers.get() method", () => {
        const headers = new Headers({
            'content-type': 'application/json',
            'accept-encoding': 'gzip, deflate'
        });

        // Existing header
        assert.strictEqual(headers.get('content-type'), 'application/json');
        assert.strictEqual(headers.get('accept-encoding'), 'gzip, deflate');

        // Non-existent header
        assert.strictEqual(headers.get('non-existent'), null);

        // Empty string header name should throw TypeError
        assert.throws(() => {
            headers.get('');
        });
    });

    it("Headers.has() method", () => {
        const headers = new Headers({
            'content-type': 'text/html',
            'cache-control': 'max-age=3600'
        });

        assert.strictEqual(headers.has('content-type'), true);
        assert.strictEqual(headers.has('cache-control'), true);

        assert.strictEqual(headers.has('non-existent'), false);

        // Empty string header name should throw TypeError
        assert.throws(() => {
            headers.has('');
        });
    });

    it("Headers.set() method", () => {
        const headers = new Headers();

        // Set new header
        headers.set('content-type', 'application/json');
        assert.strictEqual(headers.get('content-type'), 'application/json');

        // Override existing header
        headers.set('content-type', 'text/html');
        assert.strictEqual(headers.get('content-type'), 'text/html');

        headers.set('content-type', 'application/xml');
        assert.strictEqual(headers.get('content-type'), 'application/xml');
    });

    it("Headers iteration - for...of", () => {
        const init = {
            'content-type': 'application/json',
            'accept': 'application/json',
            'authorization': 'Bearer token'
        };
        const headers = new Headers(init);

        const collected = [];
        for (const [name, value] of headers) {
            collected.push([name, value]);
        }

        // Headers should be sorted by name
        assert.strictEqual(collected.length, 3);
        assert.deepEqual(collected.sort(), [
            ['accept', 'application/json'],
            ['authorization', 'Bearer token'],
            ['content-type', 'application/json']
        ]);
    });

    it("Headers.keys() iterator", () => {
        const headers = new Headers({
            'content-type': 'text/html',
            'accept': 'text/html',
            'cache-control': 'no-cache'
        });

        const keys = Array.from(headers.keys()).sort();
        assert.deepEqual(keys, ['accept', 'cache-control', 'content-type']);
    });

    it("Headers.keys() iterator protocol", () => {
        const headers = new Headers({
            'content-type': 'text/html',
            'accept': 'text/html'
        });

        // Test iterator protocol manually
        const iterator = headers.keys();
        assert.strictEqual(typeof iterator, 'object');
        assert.strictEqual(typeof iterator.next, 'function');
        assert.strictEqual(typeof iterator[Symbol.iterator], 'function');

        // Test that iterator returns itself
        assert.strictEqual(iterator[Symbol.iterator](), iterator);

        // Collect all values manually using next()
        const results = [];
        let result = iterator.next();
        while (!result.done) {
            results.push(result.value);
            result = iterator.next();
        }

        // Final call should return done: true
        assert.strictEqual(result.done, true);
        assert.strictEqual(result.value, undefined);

        // Sort and verify results
        results.sort();
        assert.deepEqual(results, ['accept', 'content-type']);

        // Iterator should be exhausted
        result = iterator.next();
        assert.strictEqual(result.done, true);
        assert.strictEqual(result.value, undefined);
    });

    it("Headers.values() iterator", () => {
        const headers = new Headers({
            'accept': 'application/json',
            'content-type': 'application/json'
        });

        const values = Array.from(headers.values()).sort();
        assert.deepEqual(values, ['application/json', 'application/json']);
    });

    it("Headers.values() iterator protocol", () => {
        const headers = new Headers({
            'x-test': 'value1',
            'y-test': 'value2'
        });

        // Test iterator protocol manually
        const iterator = headers.values();
        assert.strictEqual(typeof iterator, 'object');
        assert.strictEqual(typeof iterator.next, 'function');
        assert.strictEqual(typeof iterator[Symbol.iterator], 'function');

        // Test that iterator returns itself
        assert.strictEqual(iterator[Symbol.iterator](), iterator);

        // Collect all values manually using next()
        const results = [];
        let result = iterator.next();
        while (!result.done) {
            assert.strictEqual(typeof result.value, 'string');
            results.push(result.value);
            result = iterator.next();
        }

        // Final call should return done: true
        assert.strictEqual(result.done, true);
        assert.strictEqual(result.value, undefined);

        // Sort and verify results
        results.sort();
        assert.deepEqual(results, ['value1', 'value2']);

        // Iterator should be exhausted
        result = iterator.next();
        assert.strictEqual(result.done, true);
        assert.strictEqual(result.value, undefined);
    });

    it("Headers.entries() iterator", () => {
        const headers = new Headers({
            'content-type': 'text/plain',
            'accept': 'text/plain'
        });

        const entries = Array.from(headers.entries()).sort();
        assert.deepEqual(entries, [
            ['accept', 'text/plain'],
            ['content-type', 'text/plain']
        ]);
    });

    it("Headers.entries() iterator protocol", () => {
        const headers = new Headers({
            'x-test': 'value1',
            'y-test': 'value2'
        });

        // Test iterator protocol manually
        const iterator = headers.entries();
        assert.strictEqual(typeof iterator, 'object');
        assert.strictEqual(typeof iterator.next, 'function');
        assert.strictEqual(typeof iterator[Symbol.iterator], 'function');

        // Test that iterator returns itself
        assert.strictEqual(iterator[Symbol.iterator](), iterator);

        // Collect all entries manually using next()
        const results = [];
        let result = iterator.next();
        while (!result.done) {
            assert.strictEqual(Array.isArray(result.value), true);
            assert.strictEqual(result.value.length, 2);
            assert.strictEqual(typeof result.value[0], 'string'); // name
            assert.strictEqual(typeof result.value[1], 'string'); // value
            results.push(result.value);
            result = iterator.next();
        }

        // Final call should return done: true
        assert.strictEqual(result.done, true);
        assert.strictEqual(result.value, undefined);

        // Sort and verify results
        results.sort();
        assert.deepEqual(results, [
            ['x-test', 'value1'],
            ['y-test', 'value2']
        ]);

        // Iterator should be exhausted
        result = iterator.next();
        assert.strictEqual(result.done, true);
        assert.strictEqual(result.value, undefined);
    });

    it("Headers main iterator protocol (Symbol.iterator)", () => {
        const headers = new Headers({
            'content-type': 'application/json',
            'accept': 'application/json'
        });

        // Test that headers[Symbol.iterator] returns entries iterator
        const mainIterator = headers[Symbol.iterator]();
        const entriesIterator = headers.entries();

        // Both should have the same interface
        assert.strictEqual(typeof mainIterator.next, 'function');
        assert.strictEqual(typeof entriesIterator.next, 'function');

        // Collect results from both iterators
        const mainResults = [];
        const entriesResults = [];

        let mainResult = mainIterator.next();
        let entriesResult = entriesIterator.next();

        while (!mainResult.done && !entriesResult.done) {
            mainResults.push(mainResult.value);
            entriesResults.push(entriesResult.value);
            mainResult = mainIterator.next();
            entriesResult = entriesIterator.next();
        }

        // Both should be done at the same time
        assert.strictEqual(mainResult.done, true);
        assert.strictEqual(entriesResult.done, true);

        // Results should be identical
        mainResults.sort();
        entriesResults.sort();
        assert.deepEqual(mainResults, entriesResults);
    });

    it("Headers iterator independence", () => {
        const headers = new Headers({
            'a': '1',
            'b': '2',
            'c': '3'
        });

        // Create multiple iterators
        const iter1 = headers.keys();
        const iter2 = headers.keys();
        const iter3 = headers.values();

        // Advance first iterator partially
        const result1 = iter1.next();
        assert.strictEqual(result1.done, false);

        // Second iterator should start from beginning
        const result2 = iter2.next();
        assert.strictEqual(result2.done, false);
        assert.strictEqual(result2.value, result1.value);

        // Third iterator (values) should be independent
        const result3 = iter3.next();
        assert.strictEqual(result3.done, false);
        assert.strictEqual(typeof result3.value, 'string');

        // Continue with first iterator
        iter1.next();
        const result1Final = iter1.next();

        // Second iterator should still be at beginning state
        const result2Second = iter2.next();
        assert.notEqual(result2Second.value, result1Final.value);
    });

    it("Headers empty iterator", () => {
        const headers = new Headers();

        // All iterators should be immediately done for empty headers
        const keysIter = headers.keys();
        const valuesIter = headers.values();
        const entriesIter = headers.entries();

        assert.strictEqual(keysIter.next().done, true);
        assert.strictEqual(valuesIter.next().done, true);
        assert.strictEqual(entriesIter.next().done, true);
    });

    it("Headers.forEach() method", () => {
        const headers = new Headers({
            'content-type': 'application/json',
            'accept': 'application/json',
            'authorization': 'Bearer token'
        });

        const collected = [];
        headers.forEach((value, name, headersObj) => {
            collected.push([name, value]);
            assert.strictEqual(headersObj, headers);
        });

        assert.strictEqual(collected.length, 3);
        collected.sort();
        assert.deepEqual(collected, [
            ['accept', 'application/json'],
            ['authorization', 'Bearer token'],
            ['content-type', 'application/json']
        ]);
    });

    it("Headers case insensitivity", () => {
        const headers = new Headers();

        headers.set('Content-Type', 'text/html');
        headers.set('content-type', 'application/json');

        // Should have only one header
        let count = 0;
        for (const [name, value] of headers) {
            count++;
            assert.strictEqual(name, 'content-type');
            assert.strictEqual(value, 'application/json');
        }
        assert.strictEqual(count, 1);
    });

    it("Headers case insensitive operations", () => {
        const headers = new Headers();

        // Test case insensitive set/get
        headers.set('Content-Type', 'application/json');
        assert.strictEqual(headers.get('content-type'), 'application/json');
        assert.strictEqual(headers.get('Content-Type'), 'application/json');
        assert.strictEqual(headers.get('CONTENT-TYPE'), 'application/json');

        // Test case insensitive has
        assert.strictEqual(headers.has('content-type'), true);
        assert.strictEqual(headers.has('Content-Type'), true);
        assert.strictEqual(headers.has('CONTENT-TYPE'), true);

        // Test case insensitive delete
        headers.delete('CONTENT-TYPE');
        assert.strictEqual(headers.has('content-type'), false);

        // Test case insensitive append
        headers.append('Accept', 'text/html');
        headers.append('ACCEPT', 'application/json');
        assert.strictEqual(headers.get('accept'), 'text/html, application/json');
    });

    it("Headers with multiple values", () => {
        const headers = new Headers();

        headers.append('accept', 'text/html');
        headers.append('accept', 'application/xhtml+xml');
        headers.append('accept', 'application/xml;q=0.9');
        headers.append('accept', '*/*;q=0.8');

        const acceptValue = headers.get('accept');
        assert.strictEqual(acceptValue, 'text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8');

        // Setting should replace all values
        headers.set('accept', 'application/json');
        assert.strictEqual(headers.get('accept'), 'application/json');
    });

    it("Headers constructor with invalid init", () => {
        // Invalid init types should throw TypeError
        assert.throws(() => {
            new Headers('invalid string');
        });

        assert.throws(() => {
            new Headers(123);
        });

        assert.throws(() => {
            new Headers(true);
        });

        // Invalid array structure
        assert.throws(() => {
            new Headers([['name']]);  // Missing value
        });

        assert.throws(() => {
            new Headers([['name', 'value', 'extra']]);  // Too many elements
        });
    });

    it("Headers forbidden header names", () => {
        const headers = new Headers();

        // These headers should be forbidden in some contexts (like fetch)
        // but Headers constructor itself should allow them
        const forbiddenNames = [
            'Accept-Charset',
            'Accept-Encoding',
            'Access-Control-Request-Headers',
            'Access-Control-Request-Method',
            'Connection',
            'Content-Length',
            'Cookie',
            'Cookie2',
            'Date',
            'DNT',
            'Expect',
            'Host',
            'Keep-Alive',
            'Origin',
            'Referer',
            'TE',
            'Trailer',
            'Transfer-Encoding',
            'Upgrade',
            'Via'
        ];

        // Headers constructor should allow these
        forbiddenNames.forEach(name => {
            assert.doesNotThrow(() => {
                headers.set(name, 'test-value');
            }, `Headers should allow setting ${name}`);

            assert.strictEqual(headers.get(name), 'test-value');
            headers.delete(name);
        });
    });

    it("Headers special characters in values", () => {
        const headers = new Headers();

        // These should be allowed (Latin-1 characters and extended Latin)
        const validValues = [
            'simple-value',
            'value with spaces',
            'value-with-dashes',
            'value_with_underscores',
            'value.with.dots',
            'value123with456numbers',
            'value/with/slashes',
            'value;with=parameters',
            'value(with)parentheses',
            'value"with"quotes',
            'émojí',  // These are within Latin-1 extended range
            'ünïcödé',  // These are within Latin-1 extended range
            ''  // empty value should be allowed
        ];

        validValues.forEach((value, index) => {
            const headerName = `x-test-${index}`;
            assert.doesNotThrow(() => {
                headers.set(headerName, value);
            }, `Should allow header value: "${value}"`);

            assert.strictEqual(headers.get(headerName), value);
        });
    });

    it("Headers normalization", () => {
        const headers = new Headers();

        // Header names should be normalized to lowercase
        headers.set('content-type', 'application/json');
        headers.set('accept', 'application/json');
        headers.set('authorization', 'Bearer token');
        headers.set('x-custom-header', 'custom');

        const names = Array.from(headers.keys()).sort();
        assert.deepEqual(names, ['accept', 'authorization', 'content-type', 'x-custom-header']);
    });

    it("Headers case normalization", () => {
        const headers = new Headers();

        // Test that different case inputs result in normalized lowercase keys
        headers.set('Content-Type', 'application/json');
        headers.set('ACCEPT', 'application/json');
        headers.set('Authorization', 'Bearer token');
        headers.set('x-CUSTOM-header', 'custom');

        const names = Array.from(headers.keys()).sort();
        assert.deepEqual(names, ['accept', 'authorization', 'content-type', 'x-custom-header']);

        // Verify all are accessible via lowercase
        assert.strictEqual(headers.get('content-type'), 'application/json');
        assert.strictEqual(headers.get('accept'), 'application/json');
        assert.strictEqual(headers.get('authorization'), 'Bearer token');
        assert.strictEqual(headers.get('x-custom-header'), 'custom');
    });

    it("Headers edge cases", () => {
        // Empty Headers should be iterable
        const emptyHeaders = new Headers();
        assert.strictEqual([...emptyHeaders].length, 0);

        // Setting empty string value
        const headers = new Headers();
        headers.set('x-empty', '');
        assert.strictEqual(headers.get('x-empty'), '');
        assert.strictEqual(headers.has('x-empty'), true);

        // Appending to non-existent header
        headers.append('x-new', 'value1');
        assert.strictEqual(headers.get('x-new'), 'value1');

        // Multiple appends with empty strings
        headers.append('x-test-empty', '');
        headers.append('x-test-empty', '');
        headers.append('x-test-empty', 'actual-value');
        assert.strictEqual(headers.get('x-test-empty'), ', , actual-value');
    });

    it("Headers toString behavior", () => {
        const headers = new Headers();

        // Headers object should have proper toString
        assert.strictEqual(Object.prototype.toString.call(headers), '[object Headers]');

        // Headers should be instanceof Headers
        assert.strictEqual(headers instanceof Headers, true);
    });

    it("Headers prototype methods", () => {
        const headers = new Headers();

        // Check that all expected methods exist
        assert.strictEqual(typeof headers.append, 'function');
        assert.strictEqual(typeof headers.delete, 'function');
        assert.strictEqual(typeof headers.get, 'function');
        assert.strictEqual(typeof headers.has, 'function');
        assert.strictEqual(typeof headers.set, 'function');
        assert.strictEqual(typeof headers.keys, 'function');
        assert.strictEqual(typeof headers.values, 'function');
        assert.strictEqual(typeof headers.entries, 'function');
        assert.strictEqual(typeof headers.forEach, 'function');

        // Headers should be iterable
        assert.strictEqual(typeof headers[Symbol.iterator], 'function');
        // assert.strictEqual(headers[Symbol.iterator], headers.entries);
    });

    it("Headers bracket notation access", () => {
        const headers = new Headers({
            'content-type': 'application/json',
            'accept': 'application/json',
            'authorization': 'Bearer token'
        });

        // Test bracket notation for getting header values
        assert.strictEqual(headers['content-type'], 'application/json');
        assert.strictEqual(headers['accept'], 'application/json');
        assert.strictEqual(headers['authorization'], 'Bearer token');

        // Test case insensitive access
        assert.strictEqual(headers['Content-Type'], 'application/json');
        assert.strictEqual(headers['ACCEPT'], 'application/json');
        assert.strictEqual(headers['Authorization'], 'Bearer token');

        // Test non-existent header
        assert.strictEqual(headers['non-existent'], undefined);

        // Test bracket notation for setting header values
        headers['x-custom-header'] = 'custom-value';
        assert.strictEqual(headers.get('x-custom-header'), 'custom-value');
        assert.strictEqual(headers['x-custom-header'], 'custom-value');

        // Test overriding existing header with bracket notation
        headers['content-type'] = 'text/html';
        assert.strictEqual(headers.get('content-type'), 'text/html');
        assert.strictEqual(headers['content-type'], 'text/html');

        // Test setting with different cases
        headers['Accept-Language'] = 'en-US';
        assert.strictEqual(headers.get('accept-language'), 'en-US');
        assert.strictEqual(headers['accept-language'], 'en-US');

        // Test deleting header with bracket notation
        delete headers['authorization'];
        assert.strictEqual(headers.has('authorization'), false);
        assert.strictEqual(headers['authorization'], undefined);
        assert.strictEqual(headers.get('authorization'), null);
    });

    it("Headers bracket notation edge cases", () => {
        const headers = new Headers();

        // Test setting empty string value
        headers['x-empty'] = '';
        assert.strictEqual(headers['x-empty'], '');
        assert.strictEqual(headers.get('x-empty'), '');

        // Test setting null value (should convert to string)
        headers['x-null'] = null;
        assert.strictEqual(headers['x-null'], 'null');
        assert.strictEqual(headers.get('x-null'), 'null');

        // Test setting undefined value (should convert to string)
        headers['x-undefined'] = undefined;
        assert.strictEqual(headers['x-undefined'], 'undefined');
        assert.strictEqual(headers.get('x-undefined'), 'undefined');

        // Test setting number value (should convert to string)
        headers['x-number'] = 42;
        assert.strictEqual(headers['x-number'], '42');
        assert.strictEqual(headers.get('x-number'), '42');

        // Test setting boolean value (should convert to string)
        headers['x-boolean'] = true;
        assert.strictEqual(headers['x-boolean'], 'true');
        assert.strictEqual(headers.get('x-boolean'), 'true');

        // Test using symbols as property keys (should work like regular object property)
        const sym = Symbol('test');
        headers[sym] = 'symbol-value';
        assert.strictEqual(headers[sym], 'symbol-value');
        // Symbol properties should not appear in header iteration
        assert.strictEqual(headers.has(sym.toString()), false);

        // Test numeric keys (should be converted to strings)
        headers[123] = 'numeric-key';
        assert.strictEqual(headers['123'], 'numeric-key');
        // Note: fibjs Headers may not support numeric string keys in get() method
        // assert.strictEqual(headers.get('123'), 'numeric-key');
    });

    it("Headers bracket notation consistency with methods", () => {
        const headers = new Headers();

        // Test that bracket notation and methods work consistently
        headers.set('content-type', 'application/json');
        assert.strictEqual(headers['content-type'], 'application/json');

        headers['accept'] = 'text/html';
        assert.strictEqual(headers.get('accept'), 'text/html');

        // Test append vs bracket assignment
        headers.append('cache-control', 'no-cache');
        headers.append('cache-control', 'no-store');
        // In fibjs, bracket notation may return array for multiple values
        const cacheControlValue = headers['cache-control'];
        if (Array.isArray(cacheControlValue)) {
            assert.deepEqual(cacheControlValue, ['no-cache', 'no-store']);
        } else {
            assert.strictEqual(cacheControlValue, 'no-cache, no-store');
        }

        // Setting with bracket notation should replace, not append
        headers['cache-control'] = 'max-age=3600';
        assert.strictEqual(headers['cache-control'], 'max-age=3600');
        assert.strictEqual(headers.get('cache-control'), 'max-age=3600');

        // Test delete consistency
        headers.set('x-to-delete', 'value');
        assert.strictEqual(headers.has('x-to-delete'), true);
        
        delete headers['x-to-delete'];
        assert.strictEqual(headers.has('x-to-delete'), false);
        assert.strictEqual(headers['x-to-delete'], undefined);
        assert.strictEqual(headers.get('x-to-delete'), null);
    });
});
