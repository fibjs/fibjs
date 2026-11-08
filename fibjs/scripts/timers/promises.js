'use strict';

// timers/promises - Promise-based timer functions (Node.js compatibility)
// Usage: import { setTimeout, setImmediate } from 'node:timers/promises';

const {
    createError,
} = require('internal/errors');

/**
 * Returns a Promise that resolves after the given delay in milliseconds.
 * @param {number} delay - Milliseconds to wait. Default: 1.
 * @param {*} value - The value to resolve with. Default: undefined.
 * @param {object} options - Optional: { signal: AbortSignal }
 */
function setTimeout(delay = 1, value, options) {
    return new Promise((resolve, reject) => {
        if (options && options.signal) {
            if (options.signal.aborted) {
                return reject(options.signal.reason || createError('AbortError'));
            }
            options.signal.addEventListener('abort', () => {
                reject(options.signal.reason || createError('AbortError'));
            }, { once: true });
        }
        global.setTimeout(() => resolve(value), delay);
    });
}

/**
 * Returns a Promise that resolves on the next iteration of the event loop.
 * @param {*} value - The value to resolve with. Default: undefined.
 * @param {object} options - Optional: { signal: AbortSignal }
 */
function setImmediate(value, options) {
    return new Promise((resolve, reject) => {
        if (options && options.signal) {
            if (options.signal.aborted) {
                return reject(options.signal.reason || createError('AbortError'));
            }
            options.signal.addEventListener('abort', () => {
                reject(options.signal.reason || createError('AbortError'));
            }, { once: true });
        }
        global.setImmediate(() => resolve(value));
    });
}

/**
 * Returns an async iterator that yields `{}` every `delay` milliseconds, the
 * way `require('node:timers/promises').setInterval` does. The `value` argument
 * is accepted for signature compatibility but is not yielded: Node yields an
 * empty object, so consumers read the occurrence count from their own counter.
 *
 * The timer is cleared in `finally`, which covers `break` in the consumer
 * (the iterator is closed) as well as an aborted signal, so a loop that stops
 * early never leaves a repeating timer holding the process open.
 *
 * @param {number} delay - Milliseconds between iterations. Default: 1.
 * @param {*} value - Accepted for compatibility, ignored.
 * @param {object} options - Optional: { signal: AbortSignal }
 */
async function* setInterval(delay = 1, value, options) {
    const signal = options && options.signal;

    if (signal && signal.aborted)
        return;

    let resolveNext = null;

    const timer = global.setInterval(() => {
        const resolve = resolveNext;
        resolveNext = null;
        if (resolve)
            resolve();
    }, delay);

    // An abort wakes the loop and ends the iteration without waiting for the
    // next tick, so a slow interval still reacts to cancellation immediately.
    const onAbort = () => {
        const resolve = resolveNext;
        resolveNext = null;
        if (resolve)
            resolve();
    };
    if (signal)
        signal.addEventListener('abort', onAbort, { once: true });

    try {
        while (true) {
            await new Promise((resolve) => { resolveNext = resolve; });

            if (signal && signal.aborted)
                return;

            yield {};
        }
    } finally {
        global.clearInterval(timer);
        if (signal)
            signal.removeEventListener('abort', onAbort);
    }
}

module.exports = { setTimeout, setImmediate, setInterval };

