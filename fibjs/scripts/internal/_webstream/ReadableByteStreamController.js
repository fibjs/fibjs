'use strict';

/**
 * WHATWG Streams Standard - ReadableByteStreamController
 * Pure JavaScript implementation for fibjs
 */

const {
    READABLE_STATE,
    kState,
    kController,
    kStream,
    kQueue,
    kQueueTotalSize,
    kHighWaterMark,
    kStarted,
    kReader,
    kReadRequests,
    kPullAlgorithm,
    kCancelAlgorithm,
    kCloseRequested,
    kPulling,
    kPullAgain,
    kIsReadableByteStreamController,
    kIsReadableStreamBYOBRequest,
    kPendingPullIntos,
    kAutoAllocateChunkSize,
    kByobRequest,
    kView
} = require('./symbols');

const {
    isReadableByteStreamController,
    isReadableStreamBYOBRequest,
    createIterResultObject,
    resetQueue,
    STATE,
    isArrayBufferView,
    isDataView,
    readableStreamHasDefaultReader,
    readableStreamHasBYOBReader,
    readableStreamGetNumReadRequests,
    readableStreamGetNumReadIntoRequests,
    readableStreamClose,
    readableStreamError,
    readableStreamFulfillReadRequest,
    readableStreamFulfillReadIntoRequest,
    readableStreamAddReadIntoRequest
} = require('./helpers');

// =============================================================================
// ReadableStreamBYOBRequest Class
// =============================================================================

class ReadableStreamBYOBRequest {
    constructor() {
        throw new TypeError('Illegal constructor');
    }

    get view() {
        if (!isReadableStreamBYOBRequest(this)) {
            throw new TypeError('Invalid receiver');
        }
        return this[kView];
    }

    respond(bytesWritten) {
        if (!isReadableStreamBYOBRequest(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (this[kController] === undefined) {
            throw new TypeError('This BYOB request has been invalidated');
        }
        if (this[kView].buffer.byteLength === 0) {
            throw new TypeError('View buffer is detached');
        }
        readableByteStreamControllerRespond(this[kController], bytesWritten);
    }

    respondWithNewView(view) {
        if (!isReadableStreamBYOBRequest(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (this[kController] === undefined) {
            throw new TypeError('This BYOB request has been invalidated');
        }
        if (!isArrayBufferView(view)) {
            throw new TypeError('view must be an ArrayBufferView');
        }
        if (view.buffer.byteLength === 0) {
            throw new TypeError('View buffer is detached');
        }
        readableByteStreamControllerRespondWithNewView(this[kController], view);
    }
}

Object.defineProperty(ReadableStreamBYOBRequest.prototype, Symbol.toStringTag, {
    value: 'ReadableStreamBYOBRequest',
    configurable: true
});

function createReadableStreamBYOBRequest(controller, view) {
    const request = Object.create(ReadableStreamBYOBRequest.prototype);
    request[kIsReadableStreamBYOBRequest] = true;
    request[kController] = controller;
    request[kView] = view;
    return request;
}

// =============================================================================
// ReadableByteStreamController Class
// =============================================================================

class ReadableByteStreamController {
    constructor() {
        throw new TypeError('Illegal constructor');
    }

    get byobRequest() {
        if (!isReadableByteStreamController(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (this[kByobRequest] === null && this[kPendingPullIntos].length > 0) {
            const firstDescriptor = this[kPendingPullIntos][0];
            const view = new Uint8Array(
                firstDescriptor.buffer,
                firstDescriptor.byteOffset + firstDescriptor.bytesFilled,
                firstDescriptor.byteLength - firstDescriptor.bytesFilled
            );
            const byobRequest = createReadableStreamBYOBRequest(this, view);
            this[kByobRequest] = byobRequest;
        }
        return this[kByobRequest];
    }

    get desiredSize() {
        if (!isReadableByteStreamController(this)) {
            throw new TypeError('Invalid receiver');
        }
        return readableByteStreamControllerGetDesiredSize(this);
    }

    close() {
        if (!isReadableByteStreamController(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (this[kCloseRequested]) {
            throw new TypeError('Controller is already closed');
        }
        if (this[kStream][kState] !== STATE.READABLE) {
            throw new TypeError('ReadableStream is not readable');
        }
        readableByteStreamControllerClose(this);
    }

    enqueue(chunk) {
        if (!isReadableByteStreamController(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (!isArrayBufferView(chunk)) {
            throw new TypeError('chunk must be an ArrayBufferView');
        }
        if (chunk.byteLength === 0) {
            throw new TypeError('chunk must have non-zero byteLength');
        }
        if (chunk.buffer.byteLength === 0) {
            throw new TypeError('chunk buffer must have non-zero byteLength');
        }
        if (this[kCloseRequested]) {
            throw new TypeError('Controller is already closed');
        }
        if (this[kStream][kState] !== STATE.READABLE) {
            throw new TypeError('ReadableStream is not readable');
        }
        readableByteStreamControllerEnqueue(this, chunk);
    }

    error(e = undefined) {
        if (!isReadableByteStreamController(this)) {
            throw new TypeError('Invalid receiver');
        }
        readableByteStreamControllerError(this, e);
    }
}

Object.defineProperty(ReadableByteStreamController.prototype, Symbol.toStringTag, {
    value: 'ReadableByteStreamController',
    configurable: true
});

// Internal constructor
function createReadableByteStreamController() {
    const controller = Object.create(ReadableByteStreamController.prototype);
    controller[kIsReadableByteStreamController] = true;
    return controller;
}

// =============================================================================
// ByteStreamController Operations
// =============================================================================

function readableByteStreamControllerGetDesiredSize(controller) {
    const stream = controller[kStream];
    const state = stream[kState];

    if (state === STATE.ERRORED) {
        return null;
    }
    if (state === STATE.CLOSED) {
        return 0;
    }
    return controller[kHighWaterMark] - controller[kQueueTotalSize];
}

function readableByteStreamControllerClose(controller) {
    const stream = controller[kStream];

    if (controller[kCloseRequested] || stream[kState] !== STATE.READABLE) {
        return;
    }

    if (controller[kQueueTotalSize] > 0) {
        controller[kCloseRequested] = true;
        return;
    }

    if (controller[kPendingPullIntos].length > 0) {
        const firstPendingPullInto = controller[kPendingPullIntos][0];
        if (firstPendingPullInto.bytesFilled % firstPendingPullInto.elementSize !== 0) {
            const error = new TypeError('Insufficient bytes to fill elements');
            readableByteStreamControllerError(controller, error);
            throw error;
        }
    }

    readableByteStreamControllerClearAlgorithms(controller);
    readableStreamClose(stream);
}

function readableByteStreamControllerEnqueue(controller, chunk) {
    const stream = controller[kStream];

    if (controller[kCloseRequested] || stream[kState] !== STATE.READABLE) {
        return;
    }

    const buffer = chunk.buffer;
    const byteOffset = chunk.byteOffset;
    const byteLength = chunk.byteLength;
    
    // Transfer the buffer
    const transferredBuffer = buffer.slice(0);

    if (controller[kPendingPullIntos].length > 0) {
        const firstPendingPullInto = controller[kPendingPullIntos][0];
        readableByteStreamControllerInvalidateBYOBRequest(controller);
        firstPendingPullInto.buffer = firstPendingPullInto.buffer.slice(0);
        if (firstPendingPullInto.type === 'none') {
            readableByteStreamControllerEnqueueDetachedPullIntoToQueue(controller, firstPendingPullInto);
        }
    }

    if (readableStreamHasDefaultReader(stream)) {
        readableByteStreamControllerProcessReadRequestsUsingQueue(controller);
        if (readableStreamGetNumReadRequests(stream) === 0) {
            readableByteStreamControllerEnqueueChunkToQueue(controller, transferredBuffer, byteOffset, byteLength);
        } else {
            if (controller[kPendingPullIntos].length > 0) {
                readableByteStreamControllerShiftPendingPullInto(controller);
            }
            const transferredView = new Uint8Array(transferredBuffer, byteOffset, byteLength);
            readableStreamFulfillReadRequest(stream, transferredView, false);
        }
    } else if (readableStreamHasBYOBReader(stream)) {
        readableByteStreamControllerEnqueueChunkToQueue(controller, transferredBuffer, byteOffset, byteLength);
        readableByteStreamControllerProcessPullIntoDescriptorsUsingQueue(controller);
    } else {
        readableByteStreamControllerEnqueueChunkToQueue(controller, transferredBuffer, byteOffset, byteLength);
    }

    readableByteStreamControllerCallPullIfNeeded(controller);
}

function readableByteStreamControllerEnqueueChunkToQueue(controller, buffer, byteOffset, byteLength) {
    controller[kQueue].push({ buffer, byteOffset, byteLength });
    controller[kQueueTotalSize] += byteLength;
}

function readableByteStreamControllerEnqueueDetachedPullIntoToQueue(controller, pullIntoDescriptor) {
    if (pullIntoDescriptor.bytesFilled > 0) {
        readableByteStreamControllerEnqueueClonedChunkToQueue(
            controller,
            pullIntoDescriptor.buffer,
            pullIntoDescriptor.byteOffset,
            pullIntoDescriptor.bytesFilled
        );
    }
    readableByteStreamControllerShiftPendingPullInto(controller);
}

function readableByteStreamControllerEnqueueClonedChunkToQueue(controller, buffer, byteOffset, byteLength) {
    const clonedBuffer = buffer.slice(byteOffset, byteOffset + byteLength);
    readableByteStreamControllerEnqueueChunkToQueue(controller, clonedBuffer, 0, byteLength);
}

function readableByteStreamControllerError(controller, e) {
    const stream = controller[kStream];
    if (stream[kState] !== STATE.READABLE) {
        return;
    }

    readableByteStreamControllerClearPendingPullIntos(controller);
    resetQueue(controller);
    readableByteStreamControllerClearAlgorithms(controller);
    readableStreamError(stream, e);
}

function readableByteStreamControllerClearAlgorithms(controller) {
    controller[kPullAlgorithm] = undefined;
    controller[kCancelAlgorithm] = undefined;
}

function readableByteStreamControllerClearPendingPullIntos(controller) {
    readableByteStreamControllerInvalidateBYOBRequest(controller);
    controller[kPendingPullIntos] = [];
}

function readableByteStreamControllerInvalidateBYOBRequest(controller) {
    if (controller[kByobRequest] === null) {
        return;
    }
    controller[kByobRequest][kController] = undefined;
    controller[kByobRequest][kView] = null;
    controller[kByobRequest] = null;
}

function readableByteStreamControllerShiftPendingPullInto(controller) {
    return controller[kPendingPullIntos].shift();
}

function readableByteStreamControllerCallPullIfNeeded(controller) {
    if (!readableByteStreamControllerShouldCallPull(controller)) {
        return;
    }

    if (controller[kPulling]) {
        controller[kPullAgain] = true;
        return;
    }

    controller[kPulling] = true;

    Promise.resolve(controller[kPullAlgorithm](controller)).then(
        () => {
            controller[kPulling] = false;
            if (controller[kPullAgain]) {
                controller[kPullAgain] = false;
                readableByteStreamControllerCallPullIfNeeded(controller);
            }
        },
        (e) => {
            readableByteStreamControllerError(controller, e);
        }
    );
}

function readableByteStreamControllerShouldCallPull(controller) {
    const stream = controller[kStream];

    if (stream[kState] !== STATE.READABLE) {
        return false;
    }
    if (controller[kCloseRequested]) {
        return false;
    }
    if (!controller[kStarted]) {
        return false;
    }

    if (readableStreamHasDefaultReader(stream) && readableStreamGetNumReadRequests(stream) > 0) {
        return true;
    }
    if (readableStreamHasBYOBReader(stream) && readableStreamGetNumReadIntoRequests(stream) > 0) {
        return true;
    }

    const desiredSize = readableByteStreamControllerGetDesiredSize(controller);
    return desiredSize > 0;
}

function readableByteStreamControllerPullInto(controller, view, min, readIntoRequest) {
    const stream = controller[kStream];

    let elementSize = 1;
    let ctor = DataView;
    if (view.constructor !== DataView) {
        elementSize = view.BYTES_PER_ELEMENT;
        ctor = view.constructor;
    }

    const minimumFill = min * elementSize;
    
    const byteOffset = view.byteOffset;
    const byteLength = view.byteLength;

    // Transfer the buffer
    const buffer = view.buffer.slice(0);
    const bufferByteLength = buffer.byteLength;

    const pullIntoDescriptor = {
        buffer,
        bufferByteLength,
        byteOffset,
        byteLength,
        bytesFilled: 0,
        minimumFill,
        elementSize,
        ctor,
        type: 'byob'
    };

    if (controller[kPendingPullIntos].length > 0) {
        controller[kPendingPullIntos].push(pullIntoDescriptor);
        readableStreamAddReadIntoRequest(stream, readIntoRequest);
        return;
    }

    if (stream[kState] === STATE.CLOSED) {
        const emptyView = new ctor(pullIntoDescriptor.buffer, pullIntoDescriptor.byteOffset, 0);
        readIntoRequest.resolve(createIterResultObject(emptyView, true));
        return;
    }

    if (controller[kQueueTotalSize] > 0) {
        if (readableByteStreamControllerFillPullIntoDescriptorFromQueue(controller, pullIntoDescriptor)) {
            const filledView = readableByteStreamControllerConvertPullIntoDescriptor(pullIntoDescriptor);
            readableByteStreamControllerHandleQueueDrain(controller);
            readIntoRequest.resolve(createIterResultObject(filledView, false));
            return;
        }
        if (controller[kCloseRequested]) {
            const error = new TypeError('Insufficient bytes to fill');
            readableByteStreamControllerError(controller, error);
            readIntoRequest.reject(error);
            return;
        }
    }

    controller[kPendingPullIntos].push(pullIntoDescriptor);
    readableStreamAddReadIntoRequest(stream, readIntoRequest);
    readableByteStreamControllerCallPullIfNeeded(controller);
}

function readableByteStreamControllerFillPullIntoDescriptorFromQueue(controller, pullIntoDescriptor) {
    const maxBytesToCopy = Math.min(
        controller[kQueueTotalSize],
        pullIntoDescriptor.byteLength - pullIntoDescriptor.bytesFilled
    );
    const maxBytesFilled = pullIntoDescriptor.bytesFilled + maxBytesToCopy;
    const maxAlignedBytes = maxBytesFilled - (maxBytesFilled % pullIntoDescriptor.elementSize);

    let totalBytesToCopyRemaining = maxBytesToCopy;
    let ready = false;

    if (maxAlignedBytes >= pullIntoDescriptor.minimumFill) {
        totalBytesToCopyRemaining = maxAlignedBytes - pullIntoDescriptor.bytesFilled;
        ready = true;
    }

    const queue = controller[kQueue];

    while (totalBytesToCopyRemaining > 0) {
        const headOfQueue = queue[0];
        const bytesToCopy = Math.min(totalBytesToCopyRemaining, headOfQueue.byteLength);
        
        const destStart = pullIntoDescriptor.byteOffset + pullIntoDescriptor.bytesFilled;
        
        // Copy bytes
        const srcView = new Uint8Array(headOfQueue.buffer, headOfQueue.byteOffset, bytesToCopy);
        const destView = new Uint8Array(pullIntoDescriptor.buffer, destStart, bytesToCopy);
        destView.set(srcView);

        if (headOfQueue.byteLength === bytesToCopy) {
            queue.shift();
        } else {
            headOfQueue.byteOffset += bytesToCopy;
            headOfQueue.byteLength -= bytesToCopy;
        }

        controller[kQueueTotalSize] -= bytesToCopy;
        pullIntoDescriptor.bytesFilled += bytesToCopy;
        totalBytesToCopyRemaining -= bytesToCopy;
    }

    return ready;
}

function readableByteStreamControllerConvertPullIntoDescriptor(pullIntoDescriptor) {
    const bytesFilled = pullIntoDescriptor.bytesFilled;
    const elementSize = pullIntoDescriptor.elementSize;

    return new pullIntoDescriptor.ctor(
        pullIntoDescriptor.buffer,
        pullIntoDescriptor.byteOffset,
        bytesFilled / elementSize
    );
}

function readableByteStreamControllerHandleQueueDrain(controller) {
    if (controller[kQueueTotalSize] === 0 && controller[kCloseRequested]) {
        readableByteStreamControllerClearAlgorithms(controller);
        readableStreamClose(controller[kStream]);
        return;
    }
    readableByteStreamControllerCallPullIfNeeded(controller);
}

function readableByteStreamControllerProcessReadRequestsUsingQueue(controller) {
    const stream = controller[kStream];
    const reader = stream[kReader];

    while (reader[kReadRequests].length > 0) {
        if (controller[kQueueTotalSize] === 0) {
            return;
        }
        const readRequest = reader[kReadRequests].shift();
        readableByteStreamControllerFillReadRequestFromQueue(controller, readRequest);
    }
}

function readableByteStreamControllerFillReadRequestFromQueue(controller, readRequest) {
    const entry = controller[kQueue].shift();
    controller[kQueueTotalSize] -= entry.byteLength;
    readableByteStreamControllerHandleQueueDrain(controller);
    const view = new Uint8Array(entry.buffer, entry.byteOffset, entry.byteLength);
    readRequest.resolve(createIterResultObject(view, false));
}

function readableByteStreamControllerProcessPullIntoDescriptorsUsingQueue(controller) {
    while (controller[kPendingPullIntos].length > 0) {
        if (controller[kQueueTotalSize] === 0) {
            return;
        }
        const pullIntoDescriptor = controller[kPendingPullIntos][0];
        if (readableByteStreamControllerFillPullIntoDescriptorFromQueue(controller, pullIntoDescriptor)) {
            readableByteStreamControllerShiftPendingPullInto(controller);
            readableByteStreamControllerCommitPullIntoDescriptor(controller[kStream], pullIntoDescriptor);
        }
    }
}

function readableByteStreamControllerCommitPullIntoDescriptor(stream, pullIntoDescriptor) {
    let done = false;
    if (stream[kState] === STATE.CLOSED) {
        done = true;
    }

    const filledView = readableByteStreamControllerConvertPullIntoDescriptor(pullIntoDescriptor);

    if (pullIntoDescriptor.type === 'default') {
        readableStreamFulfillReadRequest(stream, filledView, done);
    } else {
        readableStreamFulfillReadIntoRequest(stream, filledView, done);
    }
}

function readableByteStreamControllerRespond(controller, bytesWritten) {
    const firstDescriptor = controller[kPendingPullIntos][0];
    const state = controller[kStream][kState];

    if (state === STATE.CLOSED) {
        if (bytesWritten !== 0) {
            throw new TypeError('bytesWritten must be 0 for closed stream');
        }
    } else {
        if (bytesWritten === 0) {
            throw new TypeError('bytesWritten must be greater than 0');
        }
        if (firstDescriptor.bytesFilled + bytesWritten > firstDescriptor.byteLength) {
            throw new RangeError('bytesWritten out of range');
        }
    }

    firstDescriptor.buffer = firstDescriptor.buffer.slice(0);
    readableByteStreamControllerRespondInternal(controller, bytesWritten);
}

function readableByteStreamControllerRespondWithNewView(controller, view) {
    const firstDescriptor = controller[kPendingPullIntos][0];
    const state = controller[kStream][kState];

    if (state === STATE.CLOSED) {
        if (view.byteLength !== 0) {
            throw new TypeError('view must be empty for closed stream');
        }
    } else {
        if (view.byteLength === 0) {
            throw new TypeError('view must not be empty');
        }
    }

    if (firstDescriptor.byteOffset + firstDescriptor.bytesFilled !== view.byteOffset) {
        throw new RangeError('Invalid view byteOffset');
    }
    if (firstDescriptor.bufferByteLength !== view.buffer.byteLength) {
        throw new RangeError('Invalid view buffer length');
    }
    if (firstDescriptor.bytesFilled + view.byteLength > firstDescriptor.byteLength) {
        throw new RangeError('Invalid view byteLength');
    }

    firstDescriptor.buffer = view.buffer.slice(0);
    readableByteStreamControllerRespondInternal(controller, view.byteLength);
}

function readableByteStreamControllerRespondInternal(controller, bytesWritten) {
    const firstDescriptor = controller[kPendingPullIntos][0];
    readableByteStreamControllerInvalidateBYOBRequest(controller);

    const state = controller[kStream][kState];
    if (state === STATE.CLOSED) {
        readableByteStreamControllerRespondInClosedState(controller, firstDescriptor);
    } else {
        readableByteStreamControllerRespondInReadableState(controller, bytesWritten, firstDescriptor);
    }
    readableByteStreamControllerCallPullIfNeeded(controller);
}

function readableByteStreamControllerRespondInClosedState(controller, firstDescriptor) {
    if (firstDescriptor.type === 'none') {
        readableByteStreamControllerShiftPendingPullInto(controller);
    }

    const stream = controller[kStream];
    if (readableStreamHasBYOBReader(stream)) {
        while (readableStreamGetNumReadIntoRequests(stream) > 0) {
            const pullIntoDescriptor = readableByteStreamControllerShiftPendingPullInto(controller);
            readableByteStreamControllerCommitPullIntoDescriptor(stream, pullIntoDescriptor);
        }
    }
}

function readableByteStreamControllerRespondInReadableState(controller, bytesWritten, pullIntoDescriptor) {
    pullIntoDescriptor.bytesFilled += bytesWritten;

    if (pullIntoDescriptor.type === 'none') {
        readableByteStreamControllerEnqueueDetachedPullIntoToQueue(controller, pullIntoDescriptor);
        readableByteStreamControllerProcessPullIntoDescriptorsUsingQueue(controller);
        return;
    }

    if (pullIntoDescriptor.bytesFilled < pullIntoDescriptor.minimumFill) {
        return;
    }

    readableByteStreamControllerShiftPendingPullInto(controller);

    const remainderSize = pullIntoDescriptor.bytesFilled % pullIntoDescriptor.elementSize;
    if (remainderSize > 0) {
        const end = pullIntoDescriptor.byteOffset + pullIntoDescriptor.bytesFilled;
        const start = end - remainderSize;
        const remainder = pullIntoDescriptor.buffer.slice(start, end);
        readableByteStreamControllerEnqueueChunkToQueue(controller, remainder, 0, remainder.byteLength);
    }

    pullIntoDescriptor.bytesFilled -= remainderSize;
    readableByteStreamControllerCommitPullIntoDescriptor(controller[kStream], pullIntoDescriptor);
    readableByteStreamControllerProcessPullIntoDescriptorsUsingQueue(controller);
}

// =============================================================================
// ByteStreamController Setup
// =============================================================================

function setUpReadableByteStreamControllerFromUnderlyingSource(
    stream, underlyingSource, highWaterMark
) {
    const controller = createReadableByteStreamController();

    const startAlgorithm = underlyingSource.start
        ? () => underlyingSource.start(controller)
        : () => undefined;

    const pullAlgorithm = underlyingSource.pull
        ? () => underlyingSource.pull(controller)
        : () => Promise.resolve();

    const cancelAlgorithm = underlyingSource.cancel
        ? (reason) => underlyingSource.cancel(reason)
        : () => Promise.resolve();

    const autoAllocateChunkSize = underlyingSource.autoAllocateChunkSize;
    if (autoAllocateChunkSize === 0) {
        throw new TypeError('autoAllocateChunkSize must be greater than 0');
    }

    setUpReadableByteStreamController(
        stream, controller, startAlgorithm, pullAlgorithm, cancelAlgorithm,
        highWaterMark, autoAllocateChunkSize
    );
}

function setUpReadableByteStreamController(
    stream, controller, startAlgorithm, pullAlgorithm, cancelAlgorithm,
    highWaterMark, autoAllocateChunkSize
) {
    controller[kStream] = stream;
    controller[kPullAlgorithm] = pullAlgorithm;
    controller[kCancelAlgorithm] = cancelAlgorithm;
    controller[kHighWaterMark] = highWaterMark;
    controller[kAutoAllocateChunkSize] = autoAllocateChunkSize;
    controller[kQueue] = [];
    controller[kQueueTotalSize] = 0;
    controller[kCloseRequested] = false;
    controller[kStarted] = false;
    controller[kPulling] = false;
    controller[kPullAgain] = false;
    controller[kPendingPullIntos] = [];
    controller[kByobRequest] = null;

    stream[kController] = controller;

    // Run start algorithm
    const startResult = startAlgorithm();
    Promise.resolve(startResult).then(
        () => {
            controller[kStarted] = true;
            readableByteStreamControllerCallPullIfNeeded(controller);
        },
        (r) => {
            readableByteStreamControllerError(controller, r);
        }
    );
}

// =============================================================================
// Exports
// =============================================================================

module.exports = {
    // Classes
    ReadableByteStreamController,
    ReadableStreamBYOBRequest,
    
    // Internal constructors
    createReadableByteStreamController,
    createReadableStreamBYOBRequest,
    
    // Controller algorithms
    readableByteStreamControllerGetDesiredSize,
    readableByteStreamControllerClose,
    readableByteStreamControllerEnqueue,
    readableByteStreamControllerError,
    readableByteStreamControllerClearAlgorithms,
    readableByteStreamControllerCallPullIfNeeded,
    readableByteStreamControllerPullInto,
    readableByteStreamControllerRespond,
    readableByteStreamControllerRespondWithNewView,
    
    // Setup functions
    setUpReadableByteStreamControllerFromUnderlyingSource,
    setUpReadableByteStreamController
};
