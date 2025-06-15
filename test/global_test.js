var test = require("test");
test.setup();

describe("global test", () => {
    it("AbortController basic functionality", () => {
        var ev_;
        var controller = new AbortController();

        const signal = controller.signal;
        assert.isFalse(signal.aborted);
        signal.addEventListener('abort', (ev) => {
            ev_ = ev;
        });

        controller.abort("Abort!");

        assert.isTrue(signal.aborted);
        assert.equal(ev_.type, "abort");
        assert.equal(ev_.reason, "Abort!");
    });

    it("AbortController without reason", () => {
        var controller = new AbortController();
        const signal = controller.signal;
        
        assert.isFalse(signal.aborted);
        controller.abort();
        assert.isTrue(signal.aborted);
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
        assert.equal(events[0].reason, "Multiple listeners test");
        assert.equal(events[1].reason, "Multiple listeners test");
    });

    it("AbortController already aborted signal", () => {
        var controller = new AbortController();
        const signal = controller.signal;
        var eventCount = 0;

        // Abort first
        controller.abort("First abort");
        assert.isTrue(signal.aborted);

        // Add listener after abort - should not be called
        signal.addEventListener('abort', () => {
            eventCount++;
        });

        // Abort again - should have no effect
        controller.abort("Second abort");
        
        assert.isTrue(signal.aborted);
        assert.equal(eventCount, 0); // Listener added after abort should not be called
    });

    it("AbortController signal properties", () => {
        var controller = new AbortController();
        const signal = controller.signal;

        // Check initial state
        assert.isFalse(signal.aborted);
        assert.isUndefined(signal.reason);

        // Abort with custom reason
        const customReason = new Error("Custom abort reason");
        controller.abort(customReason);

        assert.isTrue(signal.aborted);
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
            assert.isTrue(isAborted);
            assert.isTrue(signal.aborted);
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
});

