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

// CustomEvent: the DOM event carrying an application payload.  `Event` already
// accepts the EventInit dictionary (bubbles/cancelable/composed), so extending it
// only has to keep `detail`, which is the one field CustomEvent adds.
if (typeof CustomEvent === 'undefined' && typeof Event === 'function') {
    class CustomEvent extends Event {
        constructor(type, eventInitDict) {
            super(type, eventInitDict);
            var init = eventInitDict || {};
            Object.defineProperty(this, 'detail', {
                value: init.detail,
                writable: false,
                enumerable: true,
                configurable: true,
            });
        }
    }
    defineGlobalIfMissing('CustomEvent', CustomEvent);
}

// process.hrtime.bigint() - Node.js compat: returns high-res time as BigInt nanoseconds
if (typeof process !== 'undefined' && typeof process.hrtime === 'function' && typeof process.hrtime.bigint !== 'function') {
    process.hrtime.bigint = function () {
        var t = process.hrtime();
        return BigInt(t[0]) * 1000000000n + BigInt(t[1]);
    };
}

// events module Node.js surface: the two registry symbols, getEventListeners and
// the static broadcast forms of setMaxListeners/getMaxListeners.
//
// The events module object *is* the EventEmitter class in fibjs, and its class
// method table (events.cpp) is what every emitter shares, so the broadcast forms
// cannot be installed there: `setMaxListeners(n, ...targets)` would have to take
// a different argument list than the instance form `setMaxListeners(n)`, which
// the table does not support.  They are added on the module object instead, so
// `events.setMaxListeners` shadows the class entry while every emitter instance
// keeps the chainable single-argument method.
(function installEventsCompat() {
    var events;
    try {
        events = require('events');
    } catch (e) {
        return;
    }

    // Symbol.for is the registry form Node uses, so the same symbol is reachable
    // from another realm and `Symbol.keyFor(events.errorMonitor)` is
    // 'nodejs.errorMonitor'.
    function defineStatic(name, value) {
        Object.defineProperty(events, name, {
            value: value,
            writable: true,
            enumerable: true,
            configurable: true,
        });
    }

    defineStatic('errorMonitor', Symbol.for('nodejs.errorMonitor'));
    defineStatic('captureRejectionSymbol', Symbol.for('nodejs.rejection'));

    // Node delegates to emitter.listeners(type); a missing type is an empty
    // array, and a non-emitter is a TypeError.
    defineStatic('getEventListeners', function getEventListeners(target, type) {
        if (target && typeof target.listeners === 'function')
            return target.listeners(type);

        var err = new TypeError('The "target" argument must be an instance of EventEmitter.');
        err.code = 'ERR_INVALID_ARG_TYPE';
        throw err;
    });

    // The static broadcast forms.  A target list of emitters forwards the value
    // to each one; without targets the process-wide default changes, which is
    // what `events.setMaxListeners(n)` means in Node.
    var instanceSetMaxListeners = events.prototype.setMaxListeners;
    var instanceGetMaxListeners = events.prototype.getMaxListeners;

    defineStatic('setMaxListeners', function setMaxListeners(n) {
        if (arguments.length <= 1)
            return events.defaultMaxListeners = n;

        for (var i = 1; i < arguments.length; i++)
            instanceSetMaxListeners.call(arguments[i], n);
    });

    defineStatic('getMaxListeners', function getMaxListeners(target) {
        if (arguments.length === 0)
            return events.defaultMaxListeners;

        return instanceGetMaxListeners.call(target);
    });

    // dispatchEvent makes the EventEmitter/EventTarget alias usable the way the
    // Web API is: the listener receives the Event object itself, and the return
    // value is "not canceled".  DOMEvent#target/currentTarget are read-only in
    // this release and stay null, so a handler that reads ev.target still gets
    // null -- that limitation is documented rather than faked here.
    if (typeof events.prototype.dispatchEvent !== 'function') {
        Object.defineProperty(events.prototype, 'dispatchEvent', {
            value: function dispatchEvent(event) {
                if (!event || typeof event.type !== 'string') {
                    var err = new TypeError('The "event" argument must be an object with a string "type".');
                    err.code = 'ERR_INVALID_ARG_TYPE';
                    throw err;
                }
                this.emit(event.type, event);
                return !event.defaultPrevented;
            },
            writable: true,
            enumerable: false,
            configurable: true,
        });
    }
})();

// Export JS helper methods for C++ native objects to call.
// The returned object is saved by SandBox::installBuffer() into Isolate,
// and individual methods are retrieved by C++ code (e.g. AsyncStream::pipe).
module.exports = {
    pipe: require('./pipe'),
    errorTypes,
    getErrorType,
    createError,
};
