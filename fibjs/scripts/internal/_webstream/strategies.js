'use strict';

/**
 * WHATWG Streams Standard - Queuing Strategies
 * CountQueuingStrategy and ByteLengthQueuingStrategy implementations
 */

// =============================================================================
// CountQueuingStrategy
// =============================================================================

class CountQueuingStrategy {
    constructor(init) {
        if (init === undefined || init === null || typeof init !== 'object') {
            throw new TypeError('init must be an object');
        }
        const highWaterMark = init.highWaterMark;
        if (highWaterMark === undefined) {
            throw new TypeError('highWaterMark is required');
        }
        this.highWaterMark = Number(highWaterMark);
    }

    get size() {
        return () => 1;
    }
}

Object.defineProperty(CountQueuingStrategy.prototype, Symbol.toStringTag, {
    value: 'CountQueuingStrategy',
    configurable: true
});

// =============================================================================
// ByteLengthQueuingStrategy
// =============================================================================

class ByteLengthQueuingStrategy {
    constructor(init) {
        if (init === undefined || init === null || typeof init !== 'object') {
            throw new TypeError('init must be an object');
        }
        const highWaterMark = init.highWaterMark;
        if (highWaterMark === undefined) {
            throw new TypeError('highWaterMark is required');
        }
        this.highWaterMark = Number(highWaterMark);
    }

    get size() {
        return (chunk) => chunk.byteLength;
    }
}

Object.defineProperty(ByteLengthQueuingStrategy.prototype, Symbol.toStringTag, {
    value: 'ByteLengthQueuingStrategy',
    configurable: true
});

// =============================================================================
// Exports
// =============================================================================

module.exports = {
    CountQueuingStrategy,
    ByteLengthQueuingStrategy
};
