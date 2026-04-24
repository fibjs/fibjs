'use strict';

// Minimal diagnostics_channel implementation for fibjs.
// Provides the Channel API needed by undici and other modules.

const channels = new Map();

class Channel {
    constructor(name) {
        this._name = name;
        this._subscribers = [];
    }

    get name() {
        return this._name;
    }

    get hasSubscribers() {
        return this._subscribers.length > 0;
    }

    subscribe(handler) {
        this._subscribers.push(handler);
    }

    unsubscribe(handler) {
        const idx = this._subscribers.indexOf(handler);
        if (idx >= 0) {
            this._subscribers.splice(idx, 1);
            return true;
        }
        return false;
    }

    publish(message) {
        for (let i = 0; i < this._subscribers.length; i++) {
            this._subscribers[i](message, this._name);
        }
    }
}

function channel(name) {
    let ch = channels.get(name);
    if (!ch) {
        ch = new Channel(name);
        channels.set(name, ch);
    }
    return ch;
}

function subscribe(name, handler) {
    channel(name).subscribe(handler);
}

function unsubscribe(name, handler) {
    return channel(name).unsubscribe(handler);
}

function hasSubscribers(name) {
    const ch = channels.get(name);
    return ch ? ch.hasSubscribers : false;
}

class TracingChannel {
    constructor(nameOrChannels) {
        if (typeof nameOrChannels === 'string' || typeof nameOrChannels === 'symbol') {
            const prefix = 'tracing:' + String(nameOrChannels) + ':';
            this.start = channel(prefix + 'start');
            this.end = channel(prefix + 'end');
            this.asyncStart = channel(prefix + 'asyncStart');
            this.asyncEnd = channel(prefix + 'asyncEnd');
            this.error = channel(prefix + 'error');
        } else if (nameOrChannels && typeof nameOrChannels === 'object') {
            this.start = nameOrChannels.start || channel('tracing:anonymous:start');
            this.end = nameOrChannels.end || channel('tracing:anonymous:end');
            this.asyncStart = nameOrChannels.asyncStart || channel('tracing:anonymous:asyncStart');
            this.asyncEnd = nameOrChannels.asyncEnd || channel('tracing:anonymous:asyncEnd');
            this.error = nameOrChannels.error || channel('tracing:anonymous:error');
        } else {
            this.start = channel('tracing:anonymous:start');
            this.end = channel('tracing:anonymous:end');
            this.asyncStart = channel('tracing:anonymous:asyncStart');
            this.asyncEnd = channel('tracing:anonymous:asyncEnd');
            this.error = channel('tracing:anonymous:error');
        }
    }

    get hasSubscribers() {
        return this.start.hasSubscribers
            || this.end.hasSubscribers
            || this.asyncStart.hasSubscribers
            || this.asyncEnd.hasSubscribers
            || this.error.hasSubscribers;
    }

    subscribe(subscribers) {
        if (!subscribers || typeof subscribers !== 'object')
            return;
        if (typeof subscribers.start === 'function')
            this.start.subscribe(subscribers.start);
        if (typeof subscribers.end === 'function')
            this.end.subscribe(subscribers.end);
        if (typeof subscribers.asyncStart === 'function')
            this.asyncStart.subscribe(subscribers.asyncStart);
        if (typeof subscribers.asyncEnd === 'function')
            this.asyncEnd.subscribe(subscribers.asyncEnd);
        if (typeof subscribers.error === 'function')
            this.error.subscribe(subscribers.error);
    }

    unsubscribe(subscribers) {
        if (!subscribers || typeof subscribers !== 'object')
            return false;

        let ok = true;
        if (typeof subscribers.start === 'function')
            ok = this.start.unsubscribe(subscribers.start) && ok;
        if (typeof subscribers.end === 'function')
            ok = this.end.unsubscribe(subscribers.end) && ok;
        if (typeof subscribers.asyncStart === 'function')
            ok = this.asyncStart.unsubscribe(subscribers.asyncStart) && ok;
        if (typeof subscribers.asyncEnd === 'function')
            ok = this.asyncEnd.unsubscribe(subscribers.asyncEnd) && ok;
        if (typeof subscribers.error === 'function')
            ok = this.error.unsubscribe(subscribers.error) && ok;

        return ok;
    }

    traceSync(fn, context, thisArg) {
        const args = Array.prototype.slice.call(arguments, 3);
        const ctx = context && typeof context === 'object' ? context : {};

        this.start.publish(ctx);
        try {
            const result = fn.apply(thisArg, args);
            ctx.result = result;
            this.end.publish(ctx);
            return result;
        } catch (err) {
            ctx.error = err;
            this.error.publish(ctx);
            this.end.publish(ctx);
            throw err;
        }
    }

    tracePromise(fn, context, thisArg) {
        const args = Array.prototype.slice.call(arguments, 3);
        const ctx = context && typeof context === 'object' ? context : {};
        let p;

        this.start.publish(ctx);
        try {
            p = Promise.resolve(fn.apply(thisArg, args));
        } catch (err) {
            ctx.error = err;
            this.error.publish(ctx);
            this.end.publish(ctx);
            return Promise.reject(err);
        }

        this.end.publish(ctx);
        this.asyncStart.publish(ctx);

        return p.then((result) => {
            ctx.result = result;
            this.asyncEnd.publish(ctx);
            return result;
        }, (err) => {
            ctx.error = err;
            this.error.publish(ctx);
            this.asyncEnd.publish(ctx);
            throw err;
        });
    }

    traceCallback(fn, position, context, thisArg) {
        const args = Array.prototype.slice.call(arguments, 4);
        const callbackIndex = typeof position === 'number' ? position : args.length - 1;
        const ctx = context && typeof context === 'object' ? context : {};
        const originalCallback = args[callbackIndex];

        this.start.publish(ctx);

        if (typeof originalCallback === 'function') {
            const self = this;
            args[callbackIndex] = function() {
                self.asyncStart.publish(ctx);
                try {
                    const result = originalCallback.apply(this, arguments);
                    self.asyncEnd.publish(ctx);
                    return result;
                } catch (err) {
                    ctx.error = err;
                    self.error.publish(ctx);
                    self.asyncEnd.publish(ctx);
                    throw err;
                }
            };
        }

        try {
            const ret = fn.apply(thisArg, args);
            this.end.publish(ctx);
            return ret;
        } catch (err) {
            ctx.error = err;
            this.error.publish(ctx);
            this.end.publish(ctx);
            throw err;
        }
    }
}

function tracingChannel(nameOrChannels) {
    return new TracingChannel(nameOrChannels);
}

module.exports = {
    channel,
    hasSubscribers,
    subscribe,
    unsubscribe,
    tracingChannel,
    Channel,
    TracingChannel
};
