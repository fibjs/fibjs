'use strict';

/**
 * WHATWG Streams Standard - WritableStream Implementation
 * Pure JavaScript implementation for fibjs
 */

const {
    WRITABLE_STATE,
    kState,
    kStoredError,
    kController,
    kStream,
    kClosedPromise,
    kQueue,
    kQueueTotalSize,
    kHighWaterMark,
    kSizeAlgorithm,
    kStarted,
    kWriter,
    kWriteRequests,
    kInFlightWriteRequest,
    kCloseRequest,
    kInFlightCloseRequest,
    kPendingAbortRequest,
    kBackpressure,
    kReadyPromise,
    kWriteAlgorithm,
    kCloseAlgorithm,
    kAbortAlgorithm,
    kIsWritableStream,
    kIsWritableStreamDefaultWriter,
    kIsWritableStreamDefaultController
} = require('./symbols');

const {
    isObject,
    isWritableStream,
    isWritableStreamDefaultWriter,
    isWritableStreamDefaultController,
    isWritableStreamLocked,
    createPromiseCapability,
    validateAndNormalizeHighWaterMark,
    makeSizeAlgorithmFromSizeFunction,
    enqueueValueWithSize,
    dequeueValue,
    resetQueue
} = require('./helpers');

const STATE = WRITABLE_STATE;

// =============================================================================
// WritableStreamDefaultController
// =============================================================================

class WritableStreamDefaultController {
    constructor() {
        throw new TypeError('Illegal constructor');
    }

    get signal() {
        if (!isWritableStreamDefaultController(this)) {
            throw new TypeError('Invalid receiver');
        }
        return this._abortController?.signal;
    }

    error(e = undefined) {
        if (!isWritableStreamDefaultController(this)) {
            throw new TypeError('Invalid receiver');
        }
        const state = this[kStream][kState];
        if (state !== STATE.WRITABLE) {
            return;
        }
        writableStreamDefaultControllerError(this, e);
    }
}

Object.defineProperty(WritableStreamDefaultController.prototype, Symbol.toStringTag, {
    value: 'WritableStreamDefaultController',
    configurable: true
});

// Internal constructor
function createWritableStreamDefaultController() {
    const controller = Object.create(WritableStreamDefaultController.prototype);
    controller[kIsWritableStreamDefaultController] = true;
    return controller;
}

// =============================================================================
// Controller Algorithms
// =============================================================================

function writableStreamDefaultControllerGetDesiredSize(controller) {
    return controller[kHighWaterMark] - controller[kQueueTotalSize];
}

function writableStreamDefaultControllerError(controller, error) {
    const stream = controller[kStream];
    const state = stream[kState];

    if (state !== STATE.WRITABLE) {
        return;
    }

    writableStreamDefaultControllerClearAlgorithms(controller);
    writableStreamStartErroring(stream, error);
}

function writableStreamDefaultControllerClearAlgorithms(controller) {
    controller[kWriteAlgorithm] = undefined;
    controller[kCloseAlgorithm] = undefined;
    controller[kAbortAlgorithm] = undefined;
    controller[kSizeAlgorithm] = undefined;
}

function writableStreamDefaultControllerProcessClose(controller) {
    const stream = controller[kStream];

    writableStreamMarkCloseRequestInFlight(stream);
    
    resetQueue(controller);

    const sinkClosePromise = controller[kCloseAlgorithm]();
    writableStreamDefaultControllerClearAlgorithms(controller);

    Promise.resolve(sinkClosePromise).then(
        () => {
            writableStreamFinishInFlightClose(stream);
        },
        (reason) => {
            writableStreamFinishInFlightCloseWithError(stream, reason);
        }
    );
}

function writableStreamDefaultControllerProcessWrite(controller, chunk) {
    const stream = controller[kStream];

    writableStreamMarkFirstWriteRequestInFlight(stream);

    const sinkWritePromise = controller[kWriteAlgorithm](chunk, controller);

    Promise.resolve(sinkWritePromise).then(
        () => {
            writableStreamFinishInFlightWrite(stream);

            const state = stream[kState];
            if (state === STATE.ERRORING) {
                return;
            }

            const writeRequests = stream[kWriteRequests];
            if (writeRequests.length > 0) {
                writableStreamUpdateBackpressure(
                    stream,
                    writableStreamDefaultControllerGetBackpressure(controller)
                );
            }

            writableStreamDefaultControllerAdvanceQueueIfNeeded(controller);
        },
        (reason) => {
            if (stream[kState] === STATE.WRITABLE) {
                writableStreamDefaultControllerClearAlgorithms(controller);
            }
            writableStreamFinishInFlightWriteWithError(stream, reason);
        }
    );
}

function writableStreamDefaultControllerGetBackpressure(controller) {
    const desiredSize = writableStreamDefaultControllerGetDesiredSize(controller);
    return desiredSize <= 0;
}

function writableStreamDefaultControllerWrite(controller, chunk, chunkSize) {
    try {
        enqueueValueWithSize(controller, chunk, chunkSize);
    } catch (e) {
        writableStreamDefaultControllerErrorIfNeeded(controller, e);
        return;
    }

    const stream = controller[kStream];
    if (!isWritableStreamLocked(stream) || stream[kWriteRequests].length === 0) {
        const backpressure = writableStreamDefaultControllerGetBackpressure(controller);
        writableStreamUpdateBackpressure(stream, backpressure);
    }

    writableStreamDefaultControllerAdvanceQueueIfNeeded(controller);
}

function writableStreamDefaultControllerAdvanceQueueIfNeeded(controller) {
    const stream = controller[kStream];

    if (!controller[kStarted]) {
        return;
    }

    if (stream[kInFlightWriteRequest] !== undefined) {
        return;
    }

    const state = stream[kState];
    if (state === STATE.ERRORING) {
        writableStreamFinishErroring(stream);
        return;
    }

    if (controller[kQueue].length === 0) {
        return;
    }

    const value = dequeueValue(controller);
    if (value === 'close') {
        writableStreamDefaultControllerProcessClose(controller);
    } else {
        writableStreamDefaultControllerProcessWrite(controller, value);
    }
}

function writableStreamDefaultControllerErrorIfNeeded(controller, error) {
    if (controller[kStream][kState] === STATE.WRITABLE) {
        writableStreamDefaultControllerError(controller, error);
    }
}

function writableStreamDefaultControllerGetChunkSize(controller, chunk) {
    try {
        return controller[kSizeAlgorithm](chunk);
    } catch (e) {
        writableStreamDefaultControllerErrorIfNeeded(controller, e);
        return 1;
    }
}

function writableStreamDefaultControllerClose(controller) {
    enqueueValueWithSize(controller, 'close', 0);
    writableStreamDefaultControllerAdvanceQueueIfNeeded(controller);
}

// =============================================================================
// WritableStreamDefaultWriter
// =============================================================================

class WritableStreamDefaultWriter {
    constructor(stream) {
        if (!isWritableStream(stream)) {
            throw new TypeError('WritableStreamDefaultWriter requires a WritableStream');
        }
        if (isWritableStreamLocked(stream)) {
            throw new TypeError('This stream has already been locked for exclusive writing');
        }

        this[kIsWritableStreamDefaultWriter] = true;
        this[kStream] = stream;
        stream[kWriter] = this;

        const state = stream[kState];

        if (state === STATE.WRITABLE) {
            if (!writableStreamCloseQueuedOrInFlight(stream) && stream[kBackpressure]) {
                this[kReadyPromise] = createPromiseCapability();
            } else {
                this[kReadyPromise] = {
                    promise: Promise.resolve(undefined),
                    resolve: undefined,
                    reject: undefined
                };
            }
            this[kClosedPromise] = createPromiseCapability();
        } else if (state === STATE.ERRORING) {
            this[kReadyPromise] = {
                promise: Promise.reject(stream[kStoredError]),
                resolve: undefined,
                reject: undefined
            };
            this[kReadyPromise].promise.catch(() => {});
            this[kClosedPromise] = createPromiseCapability();
        } else if (state === STATE.CLOSED) {
            this[kReadyPromise] = {
                promise: Promise.resolve(undefined),
                resolve: undefined,
                reject: undefined
            };
            this[kClosedPromise] = {
                promise: Promise.resolve(undefined),
                resolve: undefined,
                reject: undefined
            };
        } else {
            // ERRORED
            const storedError = stream[kStoredError];
            this[kReadyPromise] = {
                promise: Promise.reject(storedError),
                resolve: undefined,
                reject: undefined
            };
            this[kReadyPromise].promise.catch(() => {});
            this[kClosedPromise] = {
                promise: Promise.reject(storedError),
                resolve: undefined,
                reject: undefined
            };
            this[kClosedPromise].promise.catch(() => {});
        }
    }

    get closed() {
        if (!isWritableStreamDefaultWriter(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        return this[kClosedPromise].promise;
    }

    get desiredSize() {
        if (!isWritableStreamDefaultWriter(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (this[kStream] === undefined) {
            throw new TypeError('Writer has no stream');
        }
        return writableStreamDefaultWriterGetDesiredSize(this);
    }

    get ready() {
        if (!isWritableStreamDefaultWriter(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        return this[kReadyPromise].promise;
    }

    abort(reason = undefined) {
        if (!isWritableStreamDefaultWriter(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        if (this[kStream] === undefined) {
            return Promise.reject(new TypeError('Writer has no stream'));
        }
        return writableStreamDefaultWriterAbort(this, reason);
    }

    close() {
        if (!isWritableStreamDefaultWriter(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        const stream = this[kStream];
        if (stream === undefined) {
            return Promise.reject(new TypeError('Writer has no stream'));
        }
        if (writableStreamCloseQueuedOrInFlight(stream)) {
            return Promise.reject(new TypeError('Cannot close an already closing stream'));
        }
        return writableStreamDefaultWriterClose(this);
    }

    releaseLock() {
        if (!isWritableStreamDefaultWriter(this)) {
            throw new TypeError('Invalid receiver');
        }
        const stream = this[kStream];
        if (stream === undefined) {
            return;
        }
        writableStreamDefaultWriterRelease(this);
    }

    write(chunk = undefined) {
        if (!isWritableStreamDefaultWriter(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        if (this[kStream] === undefined) {
            return Promise.reject(new TypeError('Writer has no stream'));
        }
        return writableStreamDefaultWriterWrite(this, chunk);
    }
}

Object.defineProperty(WritableStreamDefaultWriter.prototype, Symbol.toStringTag, {
    value: 'WritableStreamDefaultWriter',
    configurable: true
});
// =============================================================================
// Writer Algorithms
// =============================================================================

function writableStreamDefaultWriterGetDesiredSize(writer) {
    const stream = writer[kStream];
    const state = stream[kState];

    if (state === STATE.ERRORED || state === STATE.ERRORING) {
        return null;
    }
    if (state === STATE.CLOSED) {
        return 0;
    }
    return writableStreamDefaultControllerGetDesiredSize(stream[kController]);
}

function writableStreamDefaultWriterAbort(writer, reason) {
    const stream = writer[kStream];
    return writableStreamAbort(stream, reason);
}

function writableStreamDefaultWriterClose(writer) {
    const stream = writer[kStream];
    return writableStreamClose(stream);
}

function writableStreamDefaultWriterRelease(writer) {
    const stream = writer[kStream];
    const releasedError = new TypeError('Writer was released');

    writableStreamDefaultWriterEnsureReadyPromiseRejected(writer, releasedError);
    writableStreamDefaultWriterEnsureClosedPromiseRejected(writer, releasedError);

    stream[kWriter] = undefined;
    writer[kStream] = undefined;
}

function writableStreamDefaultWriterWrite(writer, chunk) {
    const stream = writer[kStream];
    const controller = stream[kController];
    const chunkSize = writableStreamDefaultControllerGetChunkSize(controller, chunk);

    if (stream !== writer[kStream]) {
        return Promise.reject(new TypeError('Writer has been released'));
    }

    const state = stream[kState];
    if (state === STATE.ERRORED) {
        return Promise.reject(stream[kStoredError]);
    }
    if (writableStreamCloseQueuedOrInFlight(stream) || state === STATE.CLOSED) {
        return Promise.reject(new TypeError('Cannot write to a closed stream'));
    }
    if (state === STATE.ERRORING) {
        return Promise.reject(stream[kStoredError]);
    }

    const promise = writableStreamAddWriteRequest(stream);
    writableStreamDefaultControllerWrite(controller, chunk, chunkSize);
    return promise;
}

function writableStreamDefaultWriterEnsureReadyPromiseRejected(writer, error) {
    if (writer[kReadyPromise].reject) {
        writer[kReadyPromise].reject(error);
    } else {
        writer[kReadyPromise] = {
            promise: Promise.reject(error),
            resolve: undefined,
            reject: undefined
        };
    }
    writer[kReadyPromise].promise.catch(() => {});
}

function writableStreamDefaultWriterEnsureClosedPromiseRejected(writer, error) {
    if (writer[kClosedPromise].reject) {
        writer[kClosedPromise].reject(error);
    } else {
        writer[kClosedPromise] = {
            promise: Promise.reject(error),
            resolve: undefined,
            reject: undefined
        };
    }
    writer[kClosedPromise].promise.catch(() => {});
}

// =============================================================================
// WritableStream Operations
// =============================================================================

function writableStreamAbort(stream, reason) {
    if (stream[kState] === STATE.CLOSED || stream[kState] === STATE.ERRORED) {
        return Promise.resolve(undefined);
    }

    const controller = stream[kController];
    if (controller._abortController) {
        controller._abortController.abort(reason);
    }

    const state = stream[kState];

    if (state === STATE.CLOSED || state === STATE.ERRORED) {
        return Promise.resolve(undefined);
    }

    if (stream[kPendingAbortRequest] !== undefined) {
        return stream[kPendingAbortRequest].promise;
    }

    let wasAlreadyErroring = false;
    if (state === STATE.ERRORING) {
        wasAlreadyErroring = true;
        reason = undefined;
    }

    const abortRequest = createPromiseCapability();
    stream[kPendingAbortRequest] = {
        promise: abortRequest.promise,
        resolve: abortRequest.resolve,
        reject: abortRequest.reject,
        reason: reason,
        wasAlreadyErroring: wasAlreadyErroring
    };

    if (!wasAlreadyErroring) {
        writableStreamStartErroring(stream, reason);
    }

    return abortRequest.promise;
}

function writableStreamClose(stream) {
    const state = stream[kState];
    if (state === STATE.CLOSED || state === STATE.ERRORED) {
        return Promise.reject(new TypeError('Cannot close stream in state: ' + state));
    }
    if (writableStreamCloseQueuedOrInFlight(stream)) {
        return Promise.reject(new TypeError('Close is already in progress'));
    }

    const promise = createPromiseCapability();
    stream[kCloseRequest] = promise;

    const writer = stream[kWriter];
    if (writer !== undefined && stream[kBackpressure] && state === STATE.WRITABLE) {
        writer[kReadyPromise].resolve(undefined);
    }

    writableStreamDefaultControllerClose(stream[kController]);

    return promise.promise;
}

function writableStreamAddWriteRequest(stream) {
    const promise = createPromiseCapability();
    stream[kWriteRequests].push(promise);
    return promise.promise;
}

function writableStreamCloseQueuedOrInFlight(stream) {
    return stream[kCloseRequest] !== undefined || stream[kInFlightCloseRequest] !== undefined;
}

function writableStreamStartErroring(stream, reason) {
    const controller = stream[kController];

    stream[kState] = STATE.ERRORING;
    stream[kStoredError] = reason;

    const writer = stream[kWriter];
    if (writer !== undefined) {
        writableStreamDefaultWriterEnsureReadyPromiseRejected(writer, reason);
    }

    if (!writableStreamHasOperationMarkedInFlight(stream) && controller[kStarted]) {
        writableStreamFinishErroring(stream);
    }
}

function writableStreamFinishErroring(stream) {
    stream[kState] = STATE.ERRORED;

    writableStreamDefaultControllerClearAlgorithms(stream[kController]);

    const storedError = stream[kStoredError];
    for (const writeRequest of stream[kWriteRequests]) {
        writeRequest.reject(storedError);
    }
    stream[kWriteRequests] = [];

    const abortRequest = stream[kPendingAbortRequest];
    if (abortRequest === undefined) {
        writableStreamRejectCloseAndClosedPromiseIfNeeded(stream);
        return;
    }

    stream[kPendingAbortRequest] = undefined;

    if (abortRequest.wasAlreadyErroring) {
        abortRequest.reject(storedError);
        writableStreamRejectCloseAndClosedPromiseIfNeeded(stream);
        return;
    }

    const abortPromise = stream[kController][kAbortAlgorithm](abortRequest.reason);
    Promise.resolve(abortPromise).then(
        () => {
            abortRequest.resolve(undefined);
            writableStreamRejectCloseAndClosedPromiseIfNeeded(stream);
        },
        (reason) => {
            abortRequest.reject(reason);
            writableStreamRejectCloseAndClosedPromiseIfNeeded(stream);
        }
    );
}

function writableStreamHasOperationMarkedInFlight(stream) {
    return stream[kInFlightWriteRequest] !== undefined || stream[kInFlightCloseRequest] !== undefined;
}

function writableStreamMarkCloseRequestInFlight(stream) {
    stream[kInFlightCloseRequest] = stream[kCloseRequest];
    stream[kCloseRequest] = undefined;
}

function writableStreamMarkFirstWriteRequestInFlight(stream) {
    stream[kInFlightWriteRequest] = stream[kWriteRequests].shift();
}

function writableStreamFinishInFlightWrite(stream) {
    stream[kInFlightWriteRequest].resolve(undefined);
    stream[kInFlightWriteRequest] = undefined;
}

function writableStreamFinishInFlightWriteWithError(stream, error) {
    stream[kInFlightWriteRequest].reject(error);
    stream[kInFlightWriteRequest] = undefined;

    writableStreamDealWithRejection(stream, error);
}

function writableStreamFinishInFlightClose(stream) {
    stream[kInFlightCloseRequest].resolve(undefined);
    stream[kInFlightCloseRequest] = undefined;

    const state = stream[kState];
    if (state === STATE.ERRORING) {
        stream[kStoredError] = undefined;
        if (stream[kPendingAbortRequest] !== undefined) {
            stream[kPendingAbortRequest].resolve(undefined);
            stream[kPendingAbortRequest] = undefined;
        }
    }

    stream[kState] = STATE.CLOSED;

    const writer = stream[kWriter];
    if (writer !== undefined) {
        writer[kClosedPromise].resolve(undefined);
    }
}

function writableStreamFinishInFlightCloseWithError(stream, error) {
    stream[kInFlightCloseRequest].reject(error);
    stream[kInFlightCloseRequest] = undefined;

    if (stream[kPendingAbortRequest] !== undefined) {
        stream[kPendingAbortRequest].reject(error);
        stream[kPendingAbortRequest] = undefined;
    }

    writableStreamDealWithRejection(stream, error);
}

function writableStreamDealWithRejection(stream, error) {
    const state = stream[kState];
    if (state === STATE.WRITABLE) {
        writableStreamStartErroring(stream, error);
        return;
    }
    writableStreamFinishErroring(stream);
}

function writableStreamRejectCloseAndClosedPromiseIfNeeded(stream) {
    const storedError = stream[kStoredError];

    if (stream[kCloseRequest] !== undefined) {
        stream[kCloseRequest].reject(storedError);
        stream[kCloseRequest] = undefined;
    }

    const writer = stream[kWriter];
    if (writer !== undefined) {
        writer[kClosedPromise].reject(storedError);
        writer[kClosedPromise].promise.catch(() => {});
    }
}

function writableStreamUpdateBackpressure(stream, backpressure) {
    const writer = stream[kWriter];
    if (writer !== undefined && backpressure !== stream[kBackpressure]) {
        if (backpressure) {
            writer[kReadyPromise] = createPromiseCapability();
        } else {
            writer[kReadyPromise].resolve(undefined);
        }
    }
    stream[kBackpressure] = backpressure;
}

// =============================================================================
// WritableStream Class
// =============================================================================

class WritableStream {
    constructor(underlyingSink = {}, strategy = {}) {
        if (underlyingSink !== undefined && !isObject(underlyingSink)) {
            throw new TypeError('underlyingSink must be an object');
        }
        if (strategy !== undefined && strategy !== null && !isObject(strategy)) {
            throw new TypeError('strategy must be an object');
        }

        this[kIsWritableStream] = true;
        this[kState] = STATE.WRITABLE;
        this[kStoredError] = undefined;
        this[kWriter] = undefined;
        this[kController] = undefined;
        this[kWriteRequests] = [];
        this[kInFlightWriteRequest] = undefined;
        this[kCloseRequest] = undefined;
        this[kInFlightCloseRequest] = undefined;
        this[kPendingAbortRequest] = undefined;
        this[kBackpressure] = false;

        const sizeAlgorithm = makeSizeAlgorithmFromSizeFunction(strategy?.size);
        const highWaterMark = strategy?.highWaterMark;
        const hwm = highWaterMark === undefined ? 1 : validateAndNormalizeHighWaterMark(highWaterMark);

        setUpWritableStreamDefaultControllerFromUnderlyingSink(
            this, underlyingSink, hwm, sizeAlgorithm
        );
    }

    get locked() {
        if (!isWritableStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        return isWritableStreamLocked(this);
    }

    abort(reason = undefined) {
        if (!isWritableStream(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        if (isWritableStreamLocked(this)) {
            return Promise.reject(new TypeError('Cannot abort a locked stream'));
        }
        return writableStreamAbort(this, reason);
    }

    close() {
        if (!isWritableStream(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        if (isWritableStreamLocked(this)) {
            return Promise.reject(new TypeError('Cannot close a locked stream'));
        }
        if (writableStreamCloseQueuedOrInFlight(this)) {
            return Promise.reject(new TypeError('Cannot close an already closing stream'));
        }
        return writableStreamClose(this);
    }

    getWriter() {
        if (!isWritableStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        return new WritableStreamDefaultWriter(this);
    }
}

Object.defineProperty(WritableStream.prototype, Symbol.toStringTag, {
    value: 'WritableStream',
    configurable: true
});

// =============================================================================
// Controller Setup
// =============================================================================

function setUpWritableStreamDefaultControllerFromUnderlyingSink(
    stream, underlyingSink, highWaterMark, sizeAlgorithm
) {
    const controller = createWritableStreamDefaultController();

    const startAlgorithm = underlyingSink.start
        ? () => underlyingSink.start(controller)
        : () => undefined;

    const writeAlgorithm = underlyingSink.write
        ? (chunk) => underlyingSink.write(chunk, controller)
        : () => Promise.resolve();

    const closeAlgorithm = underlyingSink.close
        ? () => underlyingSink.close()
        : () => Promise.resolve();

    const abortAlgorithm = underlyingSink.abort
        ? (reason) => underlyingSink.abort(reason)
        : () => Promise.resolve();

    setUpWritableStreamDefaultController(
        stream, controller, startAlgorithm, writeAlgorithm, closeAlgorithm,
        abortAlgorithm, highWaterMark, sizeAlgorithm
    );
}

function setUpWritableStreamDefaultController(
    stream, controller, startAlgorithm, writeAlgorithm, closeAlgorithm,
    abortAlgorithm, highWaterMark, sizeAlgorithm
) {
    controller[kStream] = stream;
    controller[kQueue] = [];
    controller[kQueueTotalSize] = 0;
    controller[kStarted] = false;
    controller[kHighWaterMark] = highWaterMark;
    controller[kSizeAlgorithm] = sizeAlgorithm;
    controller[kWriteAlgorithm] = writeAlgorithm;
    controller[kCloseAlgorithm] = closeAlgorithm;
    controller[kAbortAlgorithm] = abortAlgorithm;

    stream[kController] = controller;

    const backpressure = writableStreamDefaultControllerGetBackpressure(controller);
    writableStreamUpdateBackpressure(stream, backpressure);

    const startResult = startAlgorithm();
    Promise.resolve(startResult).then(
        () => {
            controller[kStarted] = true;
            writableStreamDefaultControllerAdvanceQueueIfNeeded(controller);
        },
        (r) => {
            controller[kStarted] = true;
            writableStreamDealWithRejection(stream, r);
        }
    );
}

// =============================================================================
// Internal Factory Function (for TransformStream)
// =============================================================================

function createInternalWritableStream(startAlgorithm, writeAlgorithm, closeAlgorithm, abortAlgorithm, highWaterMark, sizeAlgorithm) {
    const stream = Object.create(WritableStream.prototype);
    stream[kIsWritableStream] = true;
    stream[kState] = STATE.WRITABLE;
    stream[kStoredError] = undefined;
    stream[kWriter] = undefined;
    stream[kController] = undefined;
    stream[kWriteRequests] = [];
    stream[kInFlightWriteRequest] = undefined;
    stream[kCloseRequest] = undefined;
    stream[kInFlightCloseRequest] = undefined;
    stream[kPendingAbortRequest] = undefined;
    stream[kBackpressure] = false;

    const controller = createWritableStreamDefaultController();
    setUpWritableStreamDefaultController(
        stream, controller, startAlgorithm, writeAlgorithm, closeAlgorithm,
        abortAlgorithm, highWaterMark, sizeAlgorithm
    );

    return stream;
}

// =============================================================================
// Exports
// =============================================================================

module.exports = {
    // Classes
    WritableStream,
    WritableStreamDefaultWriter,
    WritableStreamDefaultController,
    
    // Internal exports for TransformStream
    kState,
    kController,
    kStoredError,
    STATE,
    writableStreamDefaultControllerErrorIfNeeded,
    createInternalWritableStream
};