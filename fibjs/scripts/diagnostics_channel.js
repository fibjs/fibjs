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

module.exports = {
    channel,
    hasSubscribers,
    subscribe,
    unsubscribe,
    Channel
};
