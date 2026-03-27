'use strict';

// Unified pipe implementation for C++ native streams and messages.
// Called from C++ via Isolate::m_pipe_fn with (src, dest, options).

module.exports = function (src, dest, options) {
    var cleanedUp = false;

    function ondata(chunk) {
        if (dest.writable !== false) {
            if (false === dest.write(chunk) && typeof src.pause === 'function')
                src.pause();
        }
    }

    function ondrain() {
        if (typeof src.resume === 'function')
            src.resume();
    }

    var didOnEnd = false;
    function onend() {
        if (didOnEnd) return;
        didOnEnd = true;
        cleanup();
        if (typeof dest.end === 'function' && (!options || options.end !== false))
            dest.end();
    }

    function onerror(err) {
        cleanup();
        if (dest.emit) dest.emit('error', err);
    }

    function onclose() {
        if (!didOnEnd) {
            didOnEnd = true;
            cleanup();
            if (typeof dest.destroy === 'function') dest.destroy();
        }
    }

    function cleanup() {
        if (cleanedUp) return;
        cleanedUp = true;
        src.removeListener('data', ondata);
        src.removeListener('end', onend);
        src.removeListener('close', onclose);
        src.removeListener('error', onerror);
        dest.removeListener('drain', ondrain);
        dest.removeListener('close', destClose);
    }

    function destClose() { cleanup(); }

    src.on('data', ondata);
    src.on('end', onend);
    src.on('close', onclose);
    src.on('error', onerror);
    dest.on('drain', ondrain);
    dest.on('close', destClose);
    if (dest.emit) dest.emit('pipe', src);

    return dest;
};
