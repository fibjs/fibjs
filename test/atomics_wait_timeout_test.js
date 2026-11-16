var { describe, it } = require('node:test');
var assert = require('assert');

// fibjs used to pass V8's microsecond TimeDelta straight to exlib's
// millisecond wait API (vender/v8/patch/src/base/platform/condition-variable.cc),
// so every Atomics.wait timeout was 1000x longer: 50ms became ~50s.
//
// The upper bound is deliberately far above a real 50ms wait (slow and
// emulated CI can jitter) but far below the broken 1000x value, so this locks
// the unit conversion without pinning scheduler precision.
describe('Atomics.wait timeout', () => {
    it('honours the millisecond timeout', () => {
        const timeout = 50;
        const ta = new Int32Array(new SharedArrayBuffer(4));

        const started = Date.now();
        const result = Atomics.wait(ta, 0, 0, timeout);
        const elapsed = Date.now() - started;

        assert.strictEqual(result, 'timed-out');
        assert.ok(elapsed >= timeout - 10,
            `woke up too early: ${elapsed}ms for ${timeout}ms`);
        assert.ok(elapsed < 3000,
            `timeout scaled by ~1000x: ${elapsed}ms for ${timeout}ms`);
    });

    it('returns immediately when the value does not match', () => {
        const ta = new Int32Array(new SharedArrayBuffer(4));
        ta[0] = 1;

        const started = Date.now();
        const result = Atomics.wait(ta, 0, 0, 10000);
        const elapsed = Date.now() - started;

        assert.strictEqual(result, 'not-equal');
        assert.ok(elapsed < 1000, `not-equal path blocked: ${elapsed}ms`);
    });
});
