'use strict';

// Sandbox global initialization - loaded once at startup.
// Registers all Web API globals that fibjs JS/C++ layers don't provide natively.

require('./domexception');
require('./webstream');

// HttpResponse.prototype.pipe - Node.js stream.Readable compat for node-fetch etc.
// Reads body data from C++ Stream and pushes into a JS Writable/Duplex dest.
(function () {
    try { var proto = require('http').Response.prototype; } catch (_) { return; }
    if (typeof proto.pipe === 'function') return;

    proto.pipe = function (dest, options) {
        var src = this;

        src.on('data', function (chunk) {
            dest.write(chunk);
        });

        src.on('end', function () {
            if (typeof dest.end === 'function' && (!options || options.end !== false))
                dest.end();
        });

        src.on('error', function (e) {
            if (dest.emit) dest.emit('error', e);
        });

        return dest;
    };
})();

// process.hrtime.bigint() - Node.js compat: returns high-res time as BigInt nanoseconds
if (typeof process !== 'undefined' && typeof process.hrtime === 'function' && typeof process.hrtime.bigint !== 'function') {
    process.hrtime.bigint = function () {
        var t = process.hrtime();
        return BigInt(t[0]) * 1000000000n + BigInt(t[1]);
    };
}
