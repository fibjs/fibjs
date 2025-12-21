'use strict';

/**
 * WHATWG Streams Standard - Helper Functions
 * Common utility functions used across all stream types
 */

const {
    READABLE_STATE,
    kState,
    kStoredError,
    kClosedPromise,
    kQueue,
    kQueueTotalSize,
    kReader,
    kReadRequests,
    kReadIntoRequests,
    kIsReadableStream,
    kIsReadableStreamDefaultReader,
    kIsReadableStreamDefaultController,
    kIsReadableStreamBYOBReader,
    kIsReadableByteStreamController,
    kIsReadableStreamBYOBRequest,
    kIsWritableStream,
    kIsWritableStreamDefaultWriter,
    kIsWritableStreamDefaultController,
    kIsTransformStream,
    kWriter
} = require('./symbols');

const STATE = READABLE_STATE;

// =============================================================================
// Type Checking Helpers
// =============================================================================

function isObject(x) {
    return (typeof x === 'object' && x !== null) || typeof x === 'function';
}

function isReadableStream(x) {
    return isObject(x) && x[kIsReadableStream] === true;
}

function isReadableStreamDefaultReader(x) {
    return isObject(x) && x[kIsReadableStreamDefaultReader] === true;
}

function isReadableStreamDefaultController(x) {
    return isObject(x) && x[kIsReadableStreamDefaultController] === true;
}

function isReadableStreamBYOBReader(x) {
    return isObject(x) && x[kIsReadableStreamBYOBReader] === true;
}

function isReadableByteStreamController(x) {
    return isObject(x) && x[kIsReadableByteStreamController] === true;
}

function isReadableStreamBYOBRequest(x) {
    return isObject(x) && x[kIsReadableStreamBYOBRequest] === true;
}

function isWritableStream(x) {
    return isObject(x) && x[kIsWritableStream] === true;
}

function isWritableStreamDefaultWriter(x) {
    return isObject(x) && x[kIsWritableStreamDefaultWriter] === true;
}

function isWritableStreamDefaultController(x) {
    return isObject(x) && x[kIsWritableStreamDefaultController] === true;
}

function isTransformStream(x) {
    return isObject(x) && x[kIsTransformStream] === true;
}

function isReadableStreamLocked(stream) {
    return stream[kReader] !== undefined;
}

function isWritableStreamLocked(stream) {
    return stream[kWriter] !== undefined;
}

// =============================================================================
// ArrayBuffer Helpers
// =============================================================================

function isArrayBufferView(value) {
    return ArrayBuffer.isView(value);
}

function isDataView(value) {
    return value instanceof DataView;
}

function cloneAsUint8Array(view) {
    const buffer = view.buffer.slice(view.byteOffset, view.byteOffset + view.byteLength);
    return new Uint8Array(buffer);
}

// =============================================================================
// Promise Helpers
// =============================================================================

function createPromiseCapability() {
    const capability = {};
    capability.promise = new Promise((resolve, reject) => {
        capability.resolve = resolve;
        capability.reject = reject;
    });
    return capability;
}

function createIterResultObject(value, done) {
    return { value, done };
}

// =============================================================================
// Validation Helpers
// =============================================================================

function validateAndNormalizeHighWaterMark(highWaterMark) {
    const hwm = Number(highWaterMark);
    if (Number.isNaN(hwm) || hwm < 0) {
        throw new RangeError('Invalid highWaterMark');
    }
    return hwm;
}

function makeSizeAlgorithmFromSizeFunction(size) {
    if (size === undefined) {
        return () => 1;
    }
    if (typeof size !== 'function') {
        throw new TypeError('size must be a function');
    }
    return (chunk) => size(chunk);
}

// =============================================================================
// Queue Operations
// =============================================================================

function enqueueValueWithSize(container, value, size) {
    if (!Number.isFinite(size) || size < 0) {
        throw new RangeError('Invalid size');
    }
    container[kQueue].push({ value, size });
    container[kQueueTotalSize] += size;
}

function dequeueValue(container) {
    const entry = container[kQueue].shift();
    container[kQueueTotalSize] -= entry.size;
    return entry.value;
}

function peekQueueValue(container) {
    return container[kQueue][0]?.value;
}

function resetQueue(container) {
    container[kQueue] = [];
    container[kQueueTotalSize] = 0;
}

// =============================================================================
// Stream Reader Helpers
// =============================================================================

function readableStreamHasDefaultReader(stream) {
    const reader = stream[kReader];
    if (reader === undefined) return false;
    return isReadableStreamDefaultReader(reader);
}

function readableStreamHasBYOBReader(stream) {
    const reader = stream[kReader];
    if (reader === undefined) return false;
    return isReadableStreamBYOBReader(reader);
}

function readableStreamGetNumReadRequests(stream) {
    return stream[kReader][kReadRequests].length;
}

function readableStreamGetNumReadIntoRequests(stream) {
    return stream[kReader][kReadIntoRequests].length;
}

// =============================================================================
// ReadableStream Operations
// =============================================================================

function readableStreamClose(stream) {
    stream[kState] = STATE.CLOSED;

    const reader = stream[kReader];
    if (reader === undefined) {
        return;
    }

    // Resolve the closed promise
    reader[kClosedPromise].resolve(undefined);

    // Fulfill all pending read requests with done=true
    if (isReadableStreamDefaultReader(reader)) {
        for (const readRequest of reader[kReadRequests]) {
            readRequest.resolve(createIterResultObject(undefined, true));
        }
        reader[kReadRequests] = [];
    } else if (isReadableStreamBYOBReader(reader)) {
        for (const readIntoRequest of reader[kReadIntoRequests]) {
            readIntoRequest.resolve(createIterResultObject(undefined, true));
        }
        reader[kReadIntoRequests] = [];
    }
}

function readableStreamError(stream, e) {
    stream[kState] = STATE.ERRORED;
    stream[kStoredError] = e;

    const reader = stream[kReader];
    if (reader === undefined) {
        return;
    }

    // Reject the closed promise
    reader[kClosedPromise].reject(e);
    reader[kClosedPromise].promise.catch(() => {});

    // Reject all pending read requests
    if (isReadableStreamDefaultReader(reader)) {
        for (const readRequest of reader[kReadRequests]) {
            readRequest.reject(e);
        }
        reader[kReadRequests] = [];
    } else if (isReadableStreamBYOBReader(reader)) {
        for (const readIntoRequest of reader[kReadIntoRequests]) {
            readIntoRequest.reject(e);
        }
        reader[kReadIntoRequests] = [];
    }
}

function readableStreamFulfillReadRequest(stream, chunk, done) {
    const reader = stream[kReader];
    const readRequest = reader[kReadRequests].shift();
    readRequest.resolve(createIterResultObject(chunk, done));
}

function readableStreamFulfillReadIntoRequest(stream, chunk, done) {
    const reader = stream[kReader];
    const readIntoRequest = reader[kReadIntoRequests].shift();
    readIntoRequest.resolve(createIterResultObject(chunk, done));
}

function readableStreamAddReadIntoRequest(stream, readIntoRequest) {
    stream[kReader][kReadIntoRequests].push(readIntoRequest);
}

// =============================================================================
// Exports
// =============================================================================

module.exports = {
    // Constants
    STATE,
    
    // Type checking
    isObject,
    isReadableStream,
    isReadableStreamDefaultReader,
    isReadableStreamDefaultController,
    isReadableStreamBYOBReader,
    isReadableByteStreamController,
    isReadableStreamBYOBRequest,
    isWritableStream,
    isWritableStreamDefaultWriter,
    isWritableStreamDefaultController,
    isTransformStream,
    isReadableStreamLocked,
    isWritableStreamLocked,
    
    // ArrayBuffer helpers
    isArrayBufferView,
    isDataView,
    cloneAsUint8Array,
    
    // Promise helpers
    createPromiseCapability,
    createIterResultObject,
    
    // Validation helpers
    validateAndNormalizeHighWaterMark,
    makeSizeAlgorithmFromSizeFunction,
    
    // Queue operations
    enqueueValueWithSize,
    dequeueValue,
    peekQueueValue,
    resetQueue,
    
    // Stream reader helpers
    readableStreamHasDefaultReader,
    readableStreamHasBYOBReader,
    readableStreamGetNumReadRequests,
    readableStreamGetNumReadIntoRequests,
    
    // Stream operations
    readableStreamClose,
    readableStreamError,
    readableStreamFulfillReadRequest,
    readableStreamFulfillReadIntoRequest,
    readableStreamAddReadIntoRequest
};
