'use strict';

/**
 * WHATWG Streams Standard - ReadableStream Readers
 * Pure JavaScript implementation for fibjs
 */

const {
    READABLE_STATE,
    kState,
    kStoredError,
    kController,
    kStream,
    kClosedPromise,
    kReader,
    kDisturbed,
    kReadRequests,
    kIsReadableStreamDefaultReader,
    kIsReadableStreamBYOBReader,
    kReadIntoRequests
} = require('./symbols');

const {
    isReadableStream,
    isReadableStreamDefaultReader,
    isReadableStreamBYOBReader,
    isReadableStreamLocked,
    isReadableByteStreamController,
    createPromiseCapability,
    createIterResultObject,
    STATE,
    isArrayBufferView,
    isDataView
} = require('./helpers');

const {
    readableStreamDefaultControllerPull
} = require('./ReadableStreamDefaultController');

const {
    readableByteStreamControllerPullInto
} = require('./ReadableByteStreamController');

const {
    invalidArgType,
    invalidArgValue,
    invalidState,
    invalidThis,
    outOfRange,
} = require('internal/errors');

const ReadableStreamModule = require('./ReadableStream');

// =============================================================================
// ReadableStreamDefaultReader Class
// =============================================================================

class ReadableStreamDefaultReader {
    constructor(stream) {
        if (!isReadableStream(stream)) {
            throw new TypeError('ReadableStreamDefaultReader requires a ReadableStream');
        }
        if (isReadableStreamLocked(stream)) {
            throw invalidState('This stream has already been locked for exclusive reading');
        }

        this[kIsReadableStreamDefaultReader] = true;
        readableStreamReaderGenericInitialize(this, stream);
        this[kReadRequests] = [];
    }

    get closed() {
        if (!isReadableStreamDefaultReader(this)) {
            return Promise.reject(invalidThis('ReadableStreamDefaultReader'));
        }
        return this[kClosedPromise].promise;
    }

    read() {
        if (!isReadableStreamDefaultReader(this)) {
            return Promise.reject(invalidThis('ReadableStreamDefaultReader'));
        }
        if (this[kStream] === undefined) {
            return Promise.reject(new TypeError('Reader has no stream'));
        }
        return readableStreamDefaultReaderRead(this);
    }

    releaseLock() {
        if (!isReadableStreamDefaultReader(this)) {
            throw invalidThis('ReadableStreamDefaultReader');
        }
        if (this[kStream] === undefined) {
            return;
        }
        readableStreamReaderGenericRelease(this);
    }

    cancel(reason = undefined) {
        if (!isReadableStreamDefaultReader(this)) {
            return Promise.reject(invalidThis('ReadableStreamDefaultReader'));
        }
        if (this[kStream] === undefined) {
            return Promise.reject(new TypeError('Reader has no stream'));
        }
        return readableStreamReaderGenericCancel(this, reason);
    }
}

Object.defineProperty(ReadableStreamDefaultReader.prototype, Symbol.toStringTag, {
    value: 'ReadableStreamDefaultReader',
    configurable: true
});

// =============================================================================
// ReadableStreamBYOBReader Class
// =============================================================================

class ReadableStreamBYOBReader {
    constructor(stream) {
        if (!isReadableStream(stream)) {
            throw new TypeError('ReadableStreamBYOBReader requires a ReadableStream');
        }
        if (!isReadableByteStreamController(stream[kController])) {
            throw new TypeError('ReadableStreamBYOBReader requires a byte stream');
        }
        if (isReadableStreamLocked(stream)) {
            throw invalidState('This stream has already been locked for exclusive reading');
        }

        this[kIsReadableStreamBYOBReader] = true;
        readableStreamReaderGenericInitialize(this, stream);
        this[kReadIntoRequests] = [];
    }

    get closed() {
        if (!isReadableStreamBYOBReader(this)) {
            return Promise.reject(invalidThis('ReadableStreamBYOBReader'));
        }
        return this[kClosedPromise].promise;
    }

    read(view, options = {}) {
        if (!isReadableStreamBYOBReader(this)) {
            return Promise.reject(invalidThis('ReadableStreamBYOBReader'));
        }
        if (!isArrayBufferView(view)) {
            return Promise.reject(invalidArgType('view', 'ArrayBufferView', view));
        }
        if (view.byteLength === 0) {
            return Promise.reject(invalidState('view must have non-zero byteLength'));
        }
        if (view.buffer.byteLength === 0) {
            return Promise.reject(invalidState('view buffer must have non-zero byteLength'));
        }
        
        const min = options?.min ?? 1;
        if (typeof min !== 'number') {
            return Promise.reject(invalidArgValue('options.min', min, 'must be a number'));
        }
        if (!Number.isInteger(min)) {
            return Promise.reject(invalidArgValue('options.min', min, 'must be an integer'));
        }
        if (min <= 0) {
            return Promise.reject(invalidArgValue('options.min', min, 'must be greater than 0'));
        }
        if (!isDataView(view)) {
            if (min > view.length) {
                return Promise.reject(outOfRange('options.min', '<= view.length', min));
            }
        } else {
            if (min > view.byteLength) {
                return Promise.reject(outOfRange('options.min', '<= view.byteLength', min));
            }
        }
        
        if (this[kStream] === undefined) {
            return Promise.reject(new TypeError('Reader has no stream'));
        }
        
        const readIntoRequest = createPromiseCapability();
        readableStreamBYOBReaderRead(this, view, min, readIntoRequest);
        return readIntoRequest.promise;
    }

    releaseLock() {
        if (!isReadableStreamBYOBReader(this)) {
            throw invalidThis('ReadableStreamBYOBReader');
        }
        if (this[kStream] === undefined) {
            return;
        }
        readableStreamBYOBReaderRelease(this);
    }

    cancel(reason = undefined) {
        if (!isReadableStreamBYOBReader(this)) {
            return Promise.reject(invalidThis('ReadableStreamBYOBReader'));
        }
        if (this[kStream] === undefined) {
            return Promise.reject(new TypeError('Reader has no stream'));
        }
        return readableStreamReaderGenericCancel(this, reason);
    }
}

Object.defineProperty(ReadableStreamBYOBReader.prototype, Symbol.toStringTag, {
    value: 'ReadableStreamBYOBReader',
    configurable: true
});

// =============================================================================
// Reader Algorithms
// =============================================================================

function readableStreamReaderGenericInitialize(reader, stream) {
    reader[kStream] = stream;
    stream[kReader] = reader;

    const state = stream[kState];
    if (state === STATE.READABLE) {
        reader[kClosedPromise] = createPromiseCapability();
    } else if (state === STATE.CLOSED) {
        reader[kClosedPromise] = {
            promise: Promise.resolve(undefined),
            resolve: undefined,
            reject: undefined
        };
    } else {
        // ERRORED
        reader[kClosedPromise] = {
            promise: Promise.reject(stream[kStoredError]),
            resolve: undefined,
            reject: undefined
        };
        // Prevent unhandled rejection
        reader[kClosedPromise].promise.catch(() => {});
    }
}

function readableStreamReaderGenericRelease(reader) {
    const stream = reader[kStream];

    if (stream[kState] === STATE.READABLE) {
        reader[kClosedPromise].reject(
            new TypeError('Reader was released')
        );
    } else {
        reader[kClosedPromise] = {
            promise: Promise.reject(new TypeError('Reader was released')),
            resolve: undefined,
            reject: undefined
        };
    }
    reader[kClosedPromise].promise.catch(() => {});

    stream[kReader] = undefined;
    reader[kStream] = undefined;
}

function readableStreamReaderGenericCancel(reader, reason) {
    const stream = reader[kStream];
    return ReadableStreamModule.readableStreamCancel(stream, reason);
}

function readableStreamDefaultReaderRead(reader) {
    const stream = reader[kStream];
    stream[kDisturbed] = true;

    const state = stream[kState];
    if (state === STATE.CLOSED) {
        return Promise.resolve(createIterResultObject(undefined, true));
    }
    if (state === STATE.ERRORED) {
        return Promise.reject(stream[kStoredError]);
    }

    return readableStreamDefaultControllerPull(stream[kController]);
}

function readableStreamBYOBReaderRead(reader, view, min, readIntoRequest) {
    const stream = reader[kStream];
    stream[kDisturbed] = true;

    if (stream[kState] === STATE.ERRORED) {
        readIntoRequest.reject(stream[kStoredError]);
        return;
    }

    readableByteStreamControllerPullInto(stream[kController], view, min, readIntoRequest);
}

function readableStreamBYOBReaderRelease(reader) {
    readableStreamReaderGenericRelease(reader);
    
    // Error all pending read into requests
    const error = new TypeError('Reader was released');
    for (const readIntoRequest of reader[kReadIntoRequests]) {
        readIntoRequest.reject(error);
    }
    reader[kReadIntoRequests] = [];
}

// =============================================================================
// Exports
// =============================================================================

module.exports = {
    // Classes
    ReadableStreamDefaultReader,
    ReadableStreamBYOBReader,
    
    // Reader algorithms
    readableStreamReaderGenericInitialize,
    readableStreamReaderGenericRelease,
    readableStreamReaderGenericCancel,
    readableStreamDefaultReaderRead,
    readableStreamBYOBReaderRead,
    readableStreamBYOBReaderRelease
};
