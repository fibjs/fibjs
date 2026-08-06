'use strict';

// Minimal `inspector` module for fibjs (Node.js API compatibility).
//
// fibjs does not implement the V8 inspector protocol, so this module exists
// mainly so that code doing a top-level `require('inspector')` (playwright,
// debug tooling, etc.) loads successfully. The API surface matches Node's
// inspector module; methods that would require a real debugger either no-op
// or report that the protocol is unavailable.

class Session {
    constructor() {
        this._listeners = {};
        this._connected = false;
    }

    connect() {
        this._connected = true;
    }

    disconnect() {
        this._connected = false;
    }

    post(method, params, callback) {
        if (typeof params === 'function') {
            callback = params;
            params = undefined;
        }

        const err = new Error('inspector.Session.post: the V8 inspector protocol is not available in fibjs');

        if (typeof callback === 'function') {
            process.nextTick(() => callback(err));
            return;
        }

        throw err;
    }

    on(event, cb) {
        if (!this._listeners[event])
            this._listeners[event] = [];
        this._listeners[event].push(cb);
        return this;
    }

    once(event, cb) {
        const self = this;
        const wrap = function (...args) {
            self.off(event, wrap);
            cb.apply(this, args);
        };
        return this.on(event, wrap);
    }

    off(event, cb) {
        const list = this._listeners[event];
        if (!list)
            return this;
        const idx = list.indexOf(cb);
        if (idx >= 0)
            list.splice(idx, 1);
        return this;
    }

    removeListener(event, cb) {
        return this.off(event, cb);
    }

    emit(event, ...args) {
        const list = this._listeners[event];
        if (!list)
            return false;
        for (const cb of list.slice())
            cb.apply(this, args);
        return true;
    }
}

let url = undefined;

exports.Session = Session;
exports.url = function () {
    return url;
};
exports.open = function () {};
exports.close = function () {};
exports.waitForDebugger = function () {};
exports.console = undefined;
