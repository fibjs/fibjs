var { describe, it } = require('node:test');
var assert = require('assert');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

describe("global test", () => {
    describe("AbortController API", () => {
        it("AbortController basic functionality", () => {
            var ev_;
            var controller = new AbortController();

            const signal = controller.signal;
            assert.strictEqual(signal.aborted, false);
            signal.addEventListener('abort', (ev) => {
                ev_ = ev;
            });

            controller.abort("Abort!");

            assert.strictEqual(signal.aborted, true);
            assert.equal(ev_.type, "abort");

            assert.equal(signal.reason, "Abort!");
        });

        it("AbortController without reason", () => {
            var controller = new AbortController();
            const signal = controller.signal;

            assert.strictEqual(signal.aborted, false);
            controller.abort();
            assert.strictEqual(signal.aborted, true);
        });

        it("AbortController multiple listeners", () => {
            var controller = new AbortController();
            const signal = controller.signal;
            var callCount = 0;
            var events = [];

            // Add multiple listeners
            signal.addEventListener('abort', (ev) => {
                callCount++;
                events.push({ type: ev.type, reason: ev.reason, listener: 1 });
            });

            signal.addEventListener('abort', (ev) => {
                callCount++;
                events.push({ type: ev.type, reason: ev.reason, listener: 2 });
            });

            controller.abort("Multiple listeners test");

            assert.equal(callCount, 2);
            assert.equal(events.length, 2);

            assert.equal(signal.reason, "Multiple listeners test");
        });

        it("AbortController already aborted signal", () => {
            var controller = new AbortController();
            const signal = controller.signal;
            var eventCount = 0;

            // Abort first
            controller.abort("First abort");
            assert.strictEqual(signal.aborted, true);

            // Add listener after abort - should not be called
            signal.addEventListener('abort', () => {
                eventCount++;
            });

            // Abort again - should have no effect
            controller.abort("Second abort");

            assert.strictEqual(signal.aborted, true);
            assert.equal(eventCount, 0); // Listener added after abort should not be called
        });

        it("AbortController signal properties", () => {
            var controller = new AbortController();
            const signal = controller.signal;

            // Check initial state
            assert.strictEqual(signal.aborted, false);
            assert.strictEqual(signal.reason, undefined);

            // Abort with custom reason
            const customReason = new Error("Custom abort reason");
            controller.abort(customReason);

            assert.strictEqual(signal.aborted, true);
            assert.equal(signal.reason, customReason);
        });

        it("AbortController throwIfAborted", () => {
            var controller = new AbortController();
            const signal = controller.signal;

            // Should not throw when not aborted
            assert.doesNotThrow(() => {
                signal.throwIfAborted();
            });

            // Abort with custom reason
            const customError = new Error("Operation aborted");
            controller.abort(customError);

            // Should throw when aborted
            assert.throws(() => {
                signal.throwIfAborted();
            }, customError);
        });

        it("AbortController listener removal", () => {
            var controller = new AbortController();
            const signal = controller.signal;
            var callCount = 0;

            const listener = () => {
                callCount++;
            };

            // Add and remove listener
            signal.addEventListener('abort', listener);
            signal.removeEventListener('abort', listener);

            controller.abort();

            assert.equal(callCount, 0); // Listener should not be called after removal
        });

        it("AbortController with timeout simulation", () => {
            var controller = new AbortController();
            const signal = controller.signal;
            var isAborted = false;

            signal.addEventListener('abort', () => {
                isAborted = true;
            });

            // Simulate async operation that gets aborted
            setTimeout(() => {
                controller.abort("Timeout");
            }, 1);

            // Wait a bit and check
            setTimeout(() => {
                assert.strictEqual(isAborted, true);
                assert.strictEqual(signal.aborted, true);
            }, 10);
        });

        it("AbortController signal reuse", () => {
            var controller = new AbortController();
            const signal1 = controller.signal;
            const signal2 = controller.signal;

            // Should return the same signal instance
            assert.equal(signal1, signal2);

            var eventCount = 0;
            signal1.addEventListener('abort', () => eventCount++);
            signal2.addEventListener('abort', () => eventCount++);

            controller.abort();

            // Both listeners should be called since they're on the same signal
            assert.equal(eventCount, 2);
        });

        it("AbortController.abort static method", () => {
            // Test static abort method if available
            if (typeof AbortSignal !== 'undefined' && AbortSignal.abort) {
                const signal = AbortSignal.abort("Static abort reason");

                assert.strictEqual(signal.aborted, true);
                assert.equal(signal.reason, "Static abort reason");

                // Listeners added to already aborted signal should not be called
                var callCount = 0;
                signal.addEventListener('abort', () => {
                    callCount++;
                });

                assert.equal(callCount, 0);
            }
        });

        it("AbortController.timeout static method", () => {
            // Test static timeout method if available
            if (typeof AbortSignal !== 'undefined' && AbortSignal.timeout) {
                const signal = AbortSignal.timeout(50);
                var isAborted = false;
                var abortReason;

                assert.strictEqual(signal.aborted, false);

                signal.addEventListener('abort', (ev) => {
                    isAborted = true;
                    abortReason = signal.reason;
                });

                // Wait for timeout
                setTimeout(() => {
                    assert.strictEqual(isAborted, true);
                    assert.strictEqual(signal.aborted, true);
                    assert.ok(abortReason instanceof Error || typeof abortReason === 'string');
                }, 100);
            }
        });

        it("AbortController signal immutability", () => {
            var controller = new AbortController();
            const signal = controller.signal;

            // Try to modify signal properties
            const originalAborted = signal.aborted;

            if (isFibjs) {
                // In fibjs, setting read-only properties should throw TypeError
                assert.throws(() => {
                    signal.aborted = true;
                }, TypeError, "Property is read-only.");
            } else {
                // In Node.js, setting read-only properties is silently ignored
                signal.aborted = true;
                assert.strictEqual(signal.aborted, originalAborted); // Should remain unchanged
            }

            assert.strictEqual(signal.aborted, originalAborted);

            // Only controller.abort() should change the state
            controller.abort();
            assert.strictEqual(signal.aborted, true);
        });

        it("AbortController event listener 'once' option", () => {
            var controller = new AbortController();
            const signal = controller.signal;
            var callCount = 0;

            // Add listener with 'once' option
            signal.addEventListener('abort', () => {
                callCount++;
            }, { once: true });

            signal.addEventListener('abort', () => {
                callCount++;
            }, { once: true });

            controller.abort("First abort");
            assert.equal(callCount, 2);

            // Create new controller to test if listeners are truly removed
            controller = new AbortController();
            const signal2 = controller.signal;
            callCount = 0;

            signal2.addEventListener('abort', () => {
                callCount++;
            }, { once: true });

            controller.abort("Second abort");
            assert.equal(callCount, 1);
        });

        it("AbortController nested abort operations", () => {
            var controller1 = new AbortController();
            var controller2 = new AbortController();
            const signal1 = controller1.signal;
            const signal2 = controller2.signal;

            var abort1Count = 0;
            var abort2Count = 0;

            signal1.addEventListener('abort', () => {
                abort1Count++;
                // Abort second controller when first is aborted
                controller2.abort("Nested abort");
            });

            signal2.addEventListener('abort', () => {
                abort2Count++;
            });

            controller1.abort("Primary abort");

            assert.equal(abort1Count, 1);
            assert.equal(abort2Count, 1);
            assert.strictEqual(signal1.aborted, true);
            assert.strictEqual(signal2.aborted, true);
            assert.equal(signal1.reason, "Primary abort");
            assert.equal(signal2.reason, "Nested abort");
        });

        it("AbortController with different reason types", () => {
            // Test cases for different reason types - fibjs should support all these
            var tests = [
                { reason: "string reason", expected: "string reason", description: "string" },
                { reason: 42, expected: 42, description: "number" },
                { reason: true, expected: true, description: "boolean true" },
                { reason: false, expected: false, description: "boolean false" },
                // In fibjs, null is treated as "no parameter passed", so it uses the default "AbortError"
                { reason: null, expected: isFibjs ? "AbortError" : null, description: "null" },
                { reason: { custom: "object" }, expected: { custom: "object" }, description: "object" },
                { reason: [1, 2, 3], expected: [1, 2, 3], description: "array" },
                { reason: 0, expected: 0, description: "zero" },
                { reason: "", expected: "", description: "empty string" },
                { reason: new Error("custom error"), expected: new Error("custom error"), description: "Error object" }
            ];

            tests.forEach((test, index) => {
                var controller = new AbortController();
                const signal = controller.signal;
                var eventReason;

                signal.addEventListener('abort', (ev) => {
                    eventReason = signal.reason;
                });

                controller.abort(test.reason);

                assert.strictEqual(signal.aborted, true);

                // fibjs should preserve the original type and value of reason
                if (test.reason instanceof Error) {
                    // For Error objects, check that it's still an Error with the same message
                    assert.ok(signal.reason instanceof Error, `signal.reason should be Error for ${test.description}`);
                    assert.equal(signal.reason.message, test.expected.message, `Error message should match for ${test.description}`);
                } else {
                    // For other types, check exact equality
                    assert.deepEqual(signal.reason, test.expected, `signal.reason should preserve type and value for ${test.description}`);
                    assert.deepEqual(eventReason, test.expected, `event.reason should preserve type and value for ${test.description}`);
                }
            });
        });

        it("AbortController signal addEventListener error handling", () => {
            var controller = new AbortController();
            const signal = controller.signal;
            var normalListenerCalled = false;
            var errorListenerCalled = false;

            // Add a listener that would normally throw an error (but we'll catch it)
            signal.addEventListener('abort', () => {
                try {
                    errorListenerCalled = true;
                    throw new Error("Listener error");
                } catch (e) {
                    // Catch the error to prevent uncaught exception
                }
            });

            // Add a normal listener that should still be called
            signal.addEventListener('abort', () => {
                normalListenerCalled = true;
            });

            controller.abort("Test abort");

            assert.strictEqual(signal.aborted, true);
            assert.strictEqual(errorListenerCalled, true);
            // Normal listener should be called even if another listener has errors
            assert.strictEqual(normalListenerCalled, true);
        });

        it("AbortController multiple abort calls with different reasons", () => {
            var controller = new AbortController();
            const signal = controller.signal;
            var abortCallCount = 0;
            var reasons = [];

            signal.addEventListener('abort', (ev) => {
                abortCallCount++;
                reasons.push(signal.reason);
            });

            // First abort
            controller.abort("First reason");
            assert.equal(abortCallCount, 1);
            assert.equal(reasons[0], "First reason");
            assert.equal(signal.reason, "First reason");

            // Second abort (should be ignored)
            controller.abort("Second reason");
            assert.equal(abortCallCount, 1); // Should not increase
            assert.equal(signal.reason, "First reason"); // Should remain the same
        });

        it("AbortController signal passive listener option", () => {
            var controller = new AbortController();
            const signal = controller.signal;
            var passiveListenerCalled = false;
            var normalListenerCalled = false;

            // Add passive listener
            signal.addEventListener('abort', () => {
                passiveListenerCalled = true;
            }, { passive: true });

            // Add normal listener
            signal.addEventListener('abort', () => {
                normalListenerCalled = true;
            });

            controller.abort("Passive test");

            assert.strictEqual(passiveListenerCalled, true);
            assert.strictEqual(normalListenerCalled, true);
            assert.strictEqual(signal.aborted, true);
        });

        it("AbortController constructor parameter validation", () => {
            // Constructor should not accept parameters and should ignore extra ones in Node.js
            // but throw errors in fibjs due to strict parameter checking
            var controller1 = new AbortController();
            assert.ok(controller1.signal);

            if (isFibjs) {
                // In fibjs, extra parameters cause an error due to strict checking
                assert.throws(() => {
                    new AbortController("invalid param");
                }, /Invalid number of parameters/);

                assert.throws(() => {
                    new AbortController({});
                }, /Invalid number of parameters/);
            } else {
                // In Node.js, extra parameters are ignored
                var controller2 = new AbortController("invalid param");
                var controller3 = new AbortController({});

                assert.ok(controller2.signal);
                assert.ok(controller3.signal);

                // All should be separate instances
                assert.notEqual(controller1.signal, controller2.signal);
                assert.notEqual(controller1.signal, controller3.signal);
                assert.notEqual(controller2.signal, controller3.signal);
            }
        });

        it("AbortController reason with complex objects", () => {
            // Test more complex reason types that should be preserved
            var complexReasons = [
                {
                    reason: { error: "network", code: 500, details: { retry: true, timeout: 30 } },
                    description: "nested object"
                },
                {
                    reason: new TypeError("Type validation failed"),
                    description: "TypeError"
                },
                {
                    reason: [{ id: 1, name: "item1" }, { id: 2, name: "item2" }],
                    description: "array of objects"
                },
                {
                    reason: new Date("2024-01-01"),
                    description: "Date object"
                },
                {
                    reason: function () { return "custom function"; },
                    description: "function"
                }
            ];

            complexReasons.forEach((test) => {
                var controller = new AbortController();
                const signal = controller.signal;
                var eventReason;

                signal.addEventListener('abort', (ev) => {
                    eventReason = signal.reason;
                });

                controller.abort(test.reason);

                assert.strictEqual(signal.aborted, true);

                if (test.reason instanceof Error) {
                    assert.ok(signal.reason instanceof Error, `signal.reason should preserve Error type for ${test.description}`);
                    assert.equal(signal.reason.message, test.reason.message);
                } else if (test.reason instanceof Date) {
                    assert.ok(signal.reason instanceof Date, `signal.reason should preserve Date type for ${test.description}`);
                    assert.equal(signal.reason.getTime(), test.reason.getTime());
                } else if (typeof test.reason === 'function') {
                    assert.equal(typeof signal.reason, 'function', `signal.reason should preserve function type for ${test.description}`);
                    assert.equal(signal.reason.toString(), test.reason.toString());
                } else {
                    assert.deepEqual(signal.reason, test.reason, `signal.reason should preserve complex object for ${test.description}`);
                    assert.deepEqual(eventReason, test.reason, `event.reason should preserve complex object for ${test.description}`);
                }
            });
        });

        it("AbortController reason with undefined and special values", () => {
            // Test edge cases with undefined and special values
            var edgeCases = [
                { reason: undefined, description: "undefined" },
                { reason: NaN, description: "NaN" },
                { reason: Infinity, description: "Infinity" },
                { reason: -Infinity, description: "-Infinity" }
            ];

            edgeCases.forEach((test) => {
                var controller = new AbortController();
                const signal = controller.signal;
                var eventReason;

                signal.addEventListener('abort', (ev) => {
                    eventReason = signal.reason;
                });

                controller.abort(test.reason);

                assert.strictEqual(signal.aborted, true);

                if (test.reason === undefined) {
                    // When no reason is provided or undefined, should default to AbortError
                    assert.ok(signal.reason, `signal.reason should have a default value for ${test.description}`);
                } else if (Number.isNaN(test.reason)) {
                    assert.ok(Number.isNaN(signal.reason), `signal.reason should preserve NaN for ${test.description}`);
                } else {
                    assert.strictEqual(signal.reason, test.reason, `signal.reason should preserve ${test.description}`);
                    assert.strictEqual(eventReason, test.reason, `event.reason should preserve ${test.description}`);
                }
            });
        });
    });

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
            if (isFibjs)
                assert.deepEqual(names, ['ACCEPT', 'Authorization', 'Content-Type', 'x-CUSTOM-header']);
            else
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
            assert.strictEqual(Object.prototype.toString.call(headers),
                isFibjs ? '[object HttpHeaders]' : '[object Headers]');

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
    });
});

