'use strict';

/**
 * WHATWG Streams Standard - ReadableStreamDefaultController
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
    kSizeAlgorithm,
    kStarted,
    kReader,
    kReadRequests,
    kPullAlgorithm,
    kCancelAlgorithm,
    kCloseRequested,
    kPulling,
    kPullAgain,
    kIsReadableStreamDefaultController
} = require('./symbols');

const {
    isReadableStreamDefaultController,
    isReadableStreamLocked,
    createIterResultObject,
    createPromiseCapability,
    enqueueValueWithSize,
    dequeueValue,
    resetQueue,
    STATE,
    readableStreamClose,
    readableStreamError,
    readableStreamFulfillReadRequest
} = require('./helpers');

// =============================================================================
// ReadableStreamDefaultController Class
// =============================================================================

class ReadableStreamDefaultController {
    constructor() {
        throw new TypeError('Illegal constructor');
    }

    get desiredSize() {
        if (!isReadableStreamDefaultController(this)) {
            throw new TypeError('Invalid receiver');
        }
        return readableStreamDefaultControllerGetDesiredSize(this);
    }

    close() {
        if (!isReadableStreamDefaultController(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (!readableStreamDefaultControllerCanCloseOrEnqueue(this)) {
            throw new TypeError('Cannot close stream');
        }
        readableStreamDefaultControllerClose(this);
    }

    enqueue(chunk = undefined) {
        if (!isReadableStreamDefaultController(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (!readableStreamDefaultControllerCanCloseOrEnqueue(this)) {
            throw new TypeError('Cannot enqueue');
        }
        readableStreamDefaultControllerEnqueue(this, chunk);
    }

    error(e = undefined) {
        if (!isReadableStreamDefaultController(this)) {
            throw new TypeError('Invalid receiver');
        }
        readableStreamDefaultControllerError(this, e);
    }
}

Object.defineProperty(ReadableStreamDefaultController.prototype, Symbol.toStringTag, {
    value: 'ReadableStreamDefaultController',
    configurable: true
});

// =============================================================================
// Internal Constructor
// =============================================================================

function createReadableStreamDefaultController() {
    const controller = Object.create(ReadableStreamDefaultController.prototype);
    controller[kIsReadableStreamDefaultController] = true;
    return controller;
}

// =============================================================================
// Controller Algorithms
// =============================================================================

function readableStreamDefaultControllerGetDesiredSize(controller) {
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

function readableStreamDefaultControllerCanCloseOrEnqueue(controller) {
    const state = controller[kStream][kState];
    return !controller[kCloseRequested] && state === STATE.READABLE;
}

function readableStreamDefaultControllerClose(controller) {
    if (!readableStreamDefaultControllerCanCloseOrEnqueue(controller)) {
        return;
    }

    const stream = controller[kStream];
    controller[kCloseRequested] = true;

    if (controller[kQueue].length === 0) {
        readableStreamDefaultControllerClearAlgorithms(controller);
        readableStreamClose(stream);
    }
}

function readableStreamDefaultControllerEnqueue(controller, chunk) {
    const stream = controller[kStream];

    if (!readableStreamDefaultControllerCanCloseOrEnqueue(controller)) {
        return;
    }

    // If there's a reader with pending read requests, fulfill directly
    const reader = stream[kReader];
    if (reader !== undefined && reader[kReadRequests].length > 0) {
        readableStreamFulfillReadRequest(stream, chunk, false);
    } else {
        // Enqueue to internal queue
        let chunkSize;
        try {
            chunkSize = controller[kSizeAlgorithm](chunk);
        } catch (e) {
            readableStreamDefaultControllerError(controller, e);
            throw e;
        }

        try {
            enqueueValueWithSize(controller, chunk, chunkSize);
        } catch (e) {
            readableStreamDefaultControllerError(controller, e);
            throw e;
        }
    }

    readableStreamDefaultControllerCallPullIfNeeded(controller);
}

function readableStreamDefaultControllerError(controller, e) {
    const stream = controller[kStream];
    if (stream[kState] !== STATE.READABLE) {
        return;
    }

    resetQueue(controller);
    readableStreamDefaultControllerClearAlgorithms(controller);
    readableStreamError(stream, e);
}

function readableStreamDefaultControllerClearAlgorithms(controller) {
    controller[kPullAlgorithm] = undefined;
    controller[kCancelAlgorithm] = undefined;
    controller[kSizeAlgorithm] = undefined;
}

function readableStreamDefaultControllerCallPullIfNeeded(controller) {
    const shouldPull = readableStreamDefaultControllerShouldCallPull(controller);
    if (!shouldPull) {
        return;
    }

    if (controller[kPulling]) {
        controller[kPullAgain] = true;
        return;
    }

    controller[kPulling] = true;

    let pullPromise;
    try {
        pullPromise = controller[kPullAlgorithm](controller);
    } catch (e) {
        readableStreamDefaultControllerError(controller, e);
        return;
    }
    
    Promise.resolve(pullPromise).then(
        () => {
            controller[kPulling] = false;
            if (controller[kPullAgain]) {
                controller[kPullAgain] = false;
                readableStreamDefaultControllerCallPullIfNeeded(controller);
            }
        },
        (e) => {
            readableStreamDefaultControllerError(controller, e);
        }
    );
}

function readableStreamDefaultControllerShouldCallPull(controller) {
    const stream = controller[kStream];

    if (!readableStreamDefaultControllerCanCloseOrEnqueue(controller)) {
        return false;
    }

    if (!controller[kStarted]) {
        return false;
    }

    // If there's a reader with pending read requests
    if (isReadableStreamLocked(stream)) {
        const reader = stream[kReader];
        if (reader[kReadRequests].length > 0) {
            return true;
        }
    }

    const desiredSize = readableStreamDefaultControllerGetDesiredSize(controller);
    return desiredSize > 0;
}

function readableStreamDefaultControllerHasBackpressure(controller) {
    return !readableStreamDefaultControllerShouldCallPull(controller);
}

function readableStreamDefaultControllerPull(controller) {
    const stream = controller[kStream];

    if (controller[kQueue].length > 0) {
        const chunk = dequeueValue(controller);

        if (controller[kCloseRequested] && controller[kQueue].length === 0) {
            readableStreamDefaultControllerClearAlgorithms(controller);
            readableStreamClose(stream);
        } else {
            readableStreamDefaultControllerCallPullIfNeeded(controller);
        }

        return Promise.resolve(createIterResultObject(chunk, false));
    }

    // No chunks in queue, add read request
    const reader = stream[kReader];
    const readRequest = createPromiseCapability();
    reader[kReadRequests].push(readRequest);
    readableStreamDefaultControllerCallPullIfNeeded(controller);
    return readRequest.promise;
}

// =============================================================================
// Controller Setup
// =============================================================================

function setUpReadableStreamDefaultControllerFromUnderlyingSource(
    stream, underlyingSource, highWaterMark, sizeAlgorithm
) {
    const controller = createReadableStreamDefaultController();

    // Extract callbacks
    const startAlgorithm = underlyingSource.start
        ? () => underlyingSource.start(controller)
        : () => undefined;

    const pullAlgorithm = underlyingSource.pull
        ? () => underlyingSource.pull(controller)
        : () => Promise.resolve();

    const cancelAlgorithm = underlyingSource.cancel
        ? (reason) => underlyingSource.cancel(reason)
        : () => Promise.resolve();

    setUpReadableStreamDefaultController(
        stream, controller, startAlgorithm, pullAlgorithm, cancelAlgorithm,
        highWaterMark, sizeAlgorithm
    );
}

function setUpReadableStreamDefaultController(
    stream, controller, startAlgorithm, pullAlgorithm, cancelAlgorithm,
    highWaterMark, sizeAlgorithm
) {
    controller[kStream] = stream;
    controller[kQueue] = [];
    controller[kQueueTotalSize] = 0;
    controller[kStarted] = false;
    controller[kCloseRequested] = false;
    controller[kPulling] = false;
    controller[kPullAgain] = false;
    controller[kHighWaterMark] = highWaterMark;
    controller[kSizeAlgorithm] = sizeAlgorithm;
    controller[kPullAlgorithm] = pullAlgorithm;
    controller[kCancelAlgorithm] = cancelAlgorithm;

    stream[kController] = controller;

    // Run start algorithm
    const startResult = startAlgorithm();
    Promise.resolve(startResult).then(
        () => {
            controller[kStarted] = true;
            readableStreamDefaultControllerCallPullIfNeeded(controller);
        },
        (r) => {
            readableStreamDefaultControllerError(controller, r);
        }
    );
}

// =============================================================================
// Exports
// =============================================================================

module.exports = {
    // Class
    ReadableStreamDefaultController,
    
    // Internal constructor
    createReadableStreamDefaultController,
    
    // Controller algorithms
    readableStreamDefaultControllerGetDesiredSize,
    readableStreamDefaultControllerCanCloseOrEnqueue,
    readableStreamDefaultControllerClose,
    readableStreamDefaultControllerEnqueue,
    readableStreamDefaultControllerError,
    readableStreamDefaultControllerClearAlgorithms,
    readableStreamDefaultControllerCallPullIfNeeded,
    readableStreamDefaultControllerShouldCallPull,
    readableStreamDefaultControllerHasBackpressure,
    readableStreamDefaultControllerPull,
    
    // Setup functions
    setUpReadableStreamDefaultControllerFromUnderlyingSource,
    setUpReadableStreamDefaultController
};
