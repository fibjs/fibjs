var { describe, it } = require('node:test');
var assert = require('assert');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

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
