'use strict';

// timers/promises - Promise-based timer functions (Node.js compatibility)
// Usage: import { setTimeout, setImmediate } from 'node:timers/promises';

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
                return reject(options.signal.reason || new Error('AbortError'));
            }
            options.signal.addEventListener('abort', () => {
                reject(options.signal.reason || new Error('AbortError'));
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
                return reject(options.signal.reason || new Error('AbortError'));
            }
            options.signal.addEventListener('abort', () => {
                reject(options.signal.reason || new Error('AbortError'));
            }, { once: true });
        }
        global.setImmediate(() => resolve(value));
    });
}

module.exports = { setTimeout, setImmediate };
