'use strict';

// Sandbox global initialization - loaded once at startup.
// Registers all Web API globals that fibjs JS/C++ layers don't provide natively.

require('./domexception');
require('./webstream');

// process.hrtime.bigint() - Node.js compat: returns high-res time as BigInt nanoseconds
if (typeof process !== 'undefined' && typeof process.hrtime === 'function' && typeof process.hrtime.bigint !== 'function') {
    process.hrtime.bigint = function () {
        var t = process.hrtime();
        return BigInt(t[0]) * 1000000000n + BigInt(t[1]);
    };
}
