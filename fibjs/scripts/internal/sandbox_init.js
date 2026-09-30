'use strict';

// Sandbox global initialization - loaded once at startup.
// Registers all Web API globals that fibjs JS/C++ layers don't provide natively.

const DOMException = require('./domexception');
const {
    AbortError,
    TimeoutError,
    errorTypes,
    getErrorType,
    createError,
} = require('./errors');
require('./webstream');

function defineGlobalIfMissing(name, value) {
    if (!(name in globalThis)) {
        Object.defineProperty(globalThis, name, {
            value,
            writable: true,
            enumerable: false,
            configurable: true,
        });
    }
}

// Centralize predefined error-type registration here so JS and C++ layers have
// a single startup path for global error constructors.
defineGlobalIfMissing('DOMException', DOMException);
defineGlobalIfMissing('AbortError', AbortError);
defineGlobalIfMissing('TimeoutError', TimeoutError);

// process.hrtime.bigint() - Node.js compat: returns high-res time as BigInt nanoseconds
if (typeof process !== 'undefined' && typeof process.hrtime === 'function' && typeof process.hrtime.bigint !== 'function') {
    process.hrtime.bigint = function () {
        var t = process.hrtime();
        return BigInt(t[0]) * 1000000000n + BigInt(t[1]);
    };
}

// Export JS helper methods for C++ native objects to call.
// The returned object is saved by SandBox::installBuffer() into Isolate,
// and individual methods are retrieved by C++ code (e.g. AsyncStream::pipe).
module.exports = {
    pipe: require('./pipe'),
    errorTypes,
    getErrorType,
    createError,
};
