var { describe, it } = require('node:test');
var assert = require('assert');
var vm = require('vm');

describe('error type registry', () => {
    it('registers predefined constructors on globalThis via sandbox init', () => {
        const init = require('internal/sandbox_init');
        const internalErrors = require('internal/errors');
        const names = [
            'Error',
            'TypeError',
            'RangeError',
            'SyntaxError',
            'ReferenceError',
            'URIError',
            'EvalError',
            'DOMException',
            'AbortError',
            'TimeoutError',
        ];

        assert.equal(typeof globalThis.DOMException, 'function');
        assert.equal(typeof globalThis.AbortError, 'function');
        assert.equal(typeof globalThis.TimeoutError, 'function');

        assert.isObject(init.errorTypes);
        assert.ok(Object.isFrozen(init.errorTypes));
        assert.strictEqual(init.errorTypes, internalErrors.errorTypes);

        names.forEach((name) => {
            assert.equal(typeof init.errorTypes[name], 'function');
            assert.strictEqual(init.errorTypes[name], globalThis[name]);
        });

        assert.strictEqual(init.errorTypes.DOMException, globalThis.DOMException);
        assert.strictEqual(init.errorTypes.AbortError, globalThis.AbortError);
        assert.strictEqual(init.errorTypes.TimeoutError, globalThis.TimeoutError);
        assert.strictEqual(globalThis.AbortError, internalErrors.AbortError);
        assert.strictEqual(globalThis.TimeoutError, internalErrors.TimeoutError);
    });

    it('exposes stable registry helpers', () => {
        const init = require('internal/sandbox_init');
        const internalErrors = require('internal/errors');

        assert.strictEqual(init.getErrorType, internalErrors.getErrorType);
        assert.strictEqual(init.createError, internalErrors.createError);

        assert.strictEqual(init.getErrorType('AbortError'), AbortError);
        assert.strictEqual(init.getErrorType('TimeoutError'), TimeoutError);
        assert.strictEqual(init.getErrorType('DoesNotExist'), Error);

        const abortErr = init.createError('AbortError', 'aborted');
        assert.ok(abortErr instanceof AbortError);
        assert.equal(abortErr.message, 'aborted');

        const timeoutErr = init.createError('TimeoutError', 'timed out');
        assert.ok(timeoutErr instanceof TimeoutError);
        assert.equal(timeoutErr.message, 'timed out');

        const domErr = init.createError('DOMException', 'bad state');
        assert.ok(domErr instanceof DOMException);
        assert.equal(domErr.name, 'DOMException');
        assert.equal(domErr.message, 'bad state');

        const fallbackErr = init.createError('UnknownErrorType', 'fallback');
        assert.ok(fallbackErr instanceof Error);
        assert.equal(fallbackErr.message, 'fallback');
    });

    it('registers predefined constructors per sandbox global', () => {
        const script = [
            "const init = require('internal/sandbox_init');",
            'module.exports = {',
            '  dom: typeof DOMException,',
            '  abort: typeof AbortError,',
            '  timeout: typeof TimeoutError,',
            '  sameAbort: init.errorTypes.AbortError === AbortError,',
            '  sameTimeout: init.errorTypes.TimeoutError === TimeoutError,',
            '  sameDom: init.errorTypes.DOMException === DOMException,',
            '};',
        ].join('\n');

        const sbox1 = new vm.SandBox({}, {});
        sbox1.addBuiltinModules();
        const r1 = sbox1.addScript('error-types-1.js', script);

        const sbox2 = new vm.SandBox({}, {});
        sbox2.addBuiltinModules();
        const r2 = sbox2.addScript('error-types-2.js', script);

        [r1, r2].forEach((r) => {
            assert.deepEqual(r, {
                dom: 'function',
                abort: 'function',
                timeout: 'function',
                sameAbort: true,
                sameTimeout: true,
                sameDom: true,
            });
        });
    });
});