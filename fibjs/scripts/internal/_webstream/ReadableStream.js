'use strict';

/**
 * WHATWG Streams Standard - ReadableStream Main Class
 * Pure JavaScript implementation for fibjs
 */

const {
    READABLE_STATE,
    kState,
    kStoredError,
    kController,
    kStream,
    kReader,
    kDisturbed,
    kCancelAlgorithm,
    kIsReadableStream
} = require('./symbols');

const {
    isObject,
    isReadableStream,
    isReadableStreamLocked,
    isReadableByteStreamController,
    createPromiseCapability,
    validateAndNormalizeHighWaterMark,
    makeSizeAlgorithmFromSizeFunction,
    STATE,
    readableStreamClose
} = require('./helpers');

const {
    ReadableStreamDefaultController,
    createReadableStreamDefaultController,
    readableStreamDefaultControllerClose,
    readableStreamDefaultControllerEnqueue,
    readableStreamDefaultControllerError,
    readableStreamDefaultControllerCanCloseOrEnqueue,
    readableStreamDefaultControllerHasBackpressure,
    readableStreamDefaultControllerGetDesiredSize,
    readableStreamDefaultControllerClearAlgorithms,
    setUpReadableStreamDefaultControllerFromUnderlyingSource,
    setUpReadableStreamDefaultController
} = require('./ReadableStreamDefaultController');

const {
    ReadableByteStreamController,
    ReadableStreamBYOBRequest,
    setUpReadableByteStreamControllerFromUnderlyingSource
} = require('./ReadableByteStreamController');

const {
    ReadableStreamDefaultReader,
    ReadableStreamBYOBReader
} = require('./ReadableStreamReaders');

// =============================================================================
// ReadableStream Cancel Operation
// =============================================================================

function readableStreamCancel(stream, reason) {
    stream[kDisturbed] = true;

    const state = stream[kState];
    if (state === STATE.CLOSED) {
        return Promise.resolve(undefined);
    }
    if (state === STATE.ERRORED) {
        return Promise.reject(stream[kStoredError]);
    }

    readableStreamClose(stream);

    const controller = stream[kController];
    const cancelResult = controller[kCancelAlgorithm](reason);
    readableStreamDefaultControllerClearAlgorithms(controller);
    return Promise.resolve(cancelResult).then(() => undefined);
}

// =============================================================================
// ReadableStream Class
// =============================================================================

class ReadableStream {
    constructor(underlyingSource = {}, strategy = {}) {
        // Validate underlyingSource
        if (underlyingSource !== undefined && !isObject(underlyingSource)) {
            throw new TypeError('underlyingSource must be an object');
        }

        // Validate strategy
        if (strategy !== undefined && strategy !== null && !isObject(strategy)) {
            throw new TypeError('strategy must be an object');
        }

        this[kIsReadableStream] = true;
        this[kState] = STATE.READABLE;
        this[kReader] = undefined;
        this[kStoredError] = undefined;
        this[kDisturbed] = false;
        this[kController] = undefined;

        // Get the type
        const type = underlyingSource.type;
        if (type === 'bytes') {
            // Byte stream
            if (strategy?.size !== undefined) {
                throw new RangeError('size strategy is not allowed for byte streams');
            }
            const highWaterMark = strategy?.highWaterMark;
            const hwm = highWaterMark === undefined ? 0 : validateAndNormalizeHighWaterMark(highWaterMark);
            setUpReadableByteStreamControllerFromUnderlyingSource(
                this, underlyingSource, hwm
            );
        } else if (type !== undefined) {
            throw new RangeError('Invalid type');
        } else {
            // Default stream
            const sizeAlgorithm = makeSizeAlgorithmFromSizeFunction(strategy?.size);
            const highWaterMark = strategy?.highWaterMark;
            const hwm = highWaterMark === undefined ? 1 : validateAndNormalizeHighWaterMark(highWaterMark);
            setUpReadableStreamDefaultControllerFromUnderlyingSource(
                this, underlyingSource, hwm, sizeAlgorithm
            );
        }
    }

    get locked() {
        if (!isReadableStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        return isReadableStreamLocked(this);
    }

    cancel(reason = undefined) {
        if (!isReadableStream(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        if (isReadableStreamLocked(this)) {
            return Promise.reject(new TypeError('Cannot cancel a locked stream'));
        }
        return readableStreamCancel(this, reason);
    }

    getReader(options = {}) {
        if (!isReadableStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        
        // Validate options object type
        if (options !== undefined && options !== null && typeof options !== 'object') {
            const err = new TypeError('options must be an object');
            err.code = 'ERR_INVALID_ARG_TYPE';
            throw err;
        }

        const mode = options?.mode;
        if (mode === undefined) {
            return new ReadableStreamDefaultReader(this);
        }
        if (mode === 'byob') {
            return new ReadableStreamBYOBReader(this);
        }
        throw new TypeError('Invalid mode');
    }

    pipeThrough(transform, options = {}) {
        if (!isReadableStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        if (!isObject(transform)) {
            throw new TypeError('transform must be an object');
        }
        const readable = transform.readable;
        const writable = transform.writable;
        if (readable === undefined) {
            throw new TypeError('transform.readable is required');
        }
        if (writable === undefined) {
            throw new TypeError('transform.writable is required');
        }
        if (isReadableStreamLocked(this)) {
            const err = new TypeError('ReadableStream is locked');
            err.code = 'ERR_INVALID_STATE';
            throw err;
        }
        if (writable.locked) {
            const err = new TypeError('WritableStream is locked');
            err.code = 'ERR_INVALID_STATE';
            throw err;
        }

        // Start piping (ignore promise result as per spec)
        this.pipeTo(writable, options).catch(() => {});

        return readable;
    }

    pipeTo(destination, options = {}) {
        if (!isReadableStream(this)) {
            return Promise.reject(new TypeError('Invalid receiver'));
        }
        if (!isObject(destination) || typeof destination.getWriter !== 'function') {
            return Promise.reject(new TypeError('destination must be a WritableStream'));
        }
        if (isReadableStreamLocked(this)) {
            const err = new TypeError('ReadableStream is locked');
            err.code = 'ERR_INVALID_STATE';
            return Promise.reject(err);
        }
        if (destination.locked) {
            const err = new TypeError('WritableStream is locked');
            err.code = 'ERR_INVALID_STATE';
            return Promise.reject(err);
        }

        const preventClose = Boolean(options.preventClose);
        const preventAbort = Boolean(options.preventAbort);
        const preventCancel = Boolean(options.preventCancel);
        const signal = options.signal;

        if (signal !== undefined && !isObject(signal)) {
            return Promise.reject(new TypeError('signal must be an AbortSignal'));
        }

        return readableStreamPipeTo(
            this, destination, preventClose, preventAbort, preventCancel, signal
        );
    }

    tee() {
        if (!isReadableStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        return readableStreamTee(this, false);
    }

    values(options = {}) {
        if (!isReadableStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        return readableStreamAsyncIterator(this, options);
    }

    [Symbol.asyncIterator](options) {
        return this.values(options);
    }

    static from(asyncIterable) {
        return readableStreamFromIterable(asyncIterable);
    }
}

Object.defineProperty(ReadableStream.prototype, Symbol.toStringTag, {
    value: 'ReadableStream',
    configurable: true
});

// =============================================================================
// PipeTo Implementation
// =============================================================================

function readableStreamPipeTo(source, dest, preventClose, preventAbort, preventCancel, signal) {
    const reader = source.getReader();
    const writer = dest.getWriter();

    let shuttingDown = false;
    let currentWrite = Promise.resolve();
    const promise = createPromiseCapability();

    // Handle abort signal
    if (signal !== undefined) {
        if (signal.aborted) {
            return Promise.reject(signal.reason || new DOMException('Aborted', 'AbortError'));
        }

        const abortAlgorithm = () => {
            const error = signal.reason || new DOMException('Aborted', 'AbortError');
            const actions = [];

            if (!preventAbort) {
                actions.push(() => {
                    if (dest.locked) {
                        return writer.abort(error).catch(() => {});
                    }
                    return dest.abort(error).catch(() => {});
                });
            }
            if (!preventCancel) {
                actions.push(() => reader.cancel(error).catch(() => {}));
            }

            shutdownWithAction(() => Promise.all(actions.map(a => a())), true, error);
        };

        if (signal.addEventListener) {
            signal.addEventListener('abort', abortAlgorithm);
        }
    }

    // Check source/dest initial errors
    checkInitialState();

    function checkInitialState() {
        pipeLoop();
    }

    function pipeLoop() {
        if (shuttingDown) return;

        return currentWrite.then(() => {
            if (shuttingDown) return;
            return pipeStep();
        });
    }

    function pipeStep() {
        if (shuttingDown) return;

        return reader.read().then(({ value, done }) => {
            if (shuttingDown) return;

            if (done) {
                if (!preventClose) {
                    shutdownWithAction(() => writer.close());
                } else {
                    shutdown();
                }
                return;
            }

            currentWrite = writer.write(value).catch(() => {});
            pipeLoop();
        }, (error) => {
            if (!preventAbort) {
                shutdownWithAction(() => writer.abort(error).catch(() => {}), true, error);
            } else {
                shutdown(true, error);
            }
        });
    }

    function shutdownWithAction(action, isError = false, error = undefined) {
        if (shuttingDown) return;
        shuttingDown = true;

        currentWrite.then(() => {
            return action();
        }).then(() => {
            finalize(isError, error);
        }, (newError) => {
            finalize(true, newError);
        });
    }

    function shutdown(isError = false, error = undefined) {
        if (shuttingDown) return;
        shuttingDown = true;

        currentWrite.then(() => {
            finalize(isError, error);
        });
    }

    function finalize(isError, error) {
        writer.releaseLock();
        reader.releaseLock();

        if (isError) {
            promise.reject(error);
        } else {
            promise.resolve(undefined);
        }
    }

    return promise.promise;
}

// =============================================================================
// Tee Implementation
// =============================================================================

function readableStreamTee(stream, cloneForBranch2) {
    const reader = stream.getReader();

    let reading = false;
    let readAgain = false;
    let canceled1 = false;
    let canceled2 = false;
    let reason1;
    let reason2;
    let branch1;
    let branch2;

    const cancelPromise = createPromiseCapability();

    function pullAlgorithm() {
        if (reading) {
            readAgain = true;
            return Promise.resolve();
        }

        reading = true;

        reader.read().then(({ value, done }) => {
            reading = false;

            if (done) {
                if (!canceled1) {
                    readableStreamDefaultControllerClose(branch1[kController]);
                }
                if (!canceled2) {
                    readableStreamDefaultControllerClose(branch2[kController]);
                }
                if (!canceled1 || !canceled2) {
                    cancelPromise.resolve(undefined);
                }
                return;
            }

            const value1 = value;
            const value2 = value;

            if (!canceled1) {
                readableStreamDefaultControllerEnqueue(branch1[kController], value1);
            }
            if (!canceled2) {
                readableStreamDefaultControllerEnqueue(branch2[kController], value2);
            }

            if (readAgain) {
                readAgain = false;
                pullAlgorithm();
            }
        }, () => {
            // reader.closed handles propagating the stored error to both branches.
            // This branch exists to consume the in-flight read() rejection so it
            // does not surface as an unhandled promise rejection.
            reading = false;
            readAgain = false;
        });

        return Promise.resolve();
    }

    function cancel1Algorithm(reason) {
        canceled1 = true;
        reason1 = reason;
        if (canceled2) {
            const compositeReason = [reason1, reason2];
            const cancelResult = readableStreamCancel(stream, compositeReason);
            cancelPromise.resolve(cancelResult);
        }
        return cancelPromise.promise;
    }

    function cancel2Algorithm(reason) {
        canceled2 = true;
        reason2 = reason;
        if (canceled1) {
            const compositeReason = [reason1, reason2];
            const cancelResult = readableStreamCancel(stream, compositeReason);
            cancelPromise.resolve(cancelResult);
        }
        return cancelPromise.promise;
    }

    branch1 = new ReadableStream({
        start() {},
        pull: pullAlgorithm,
        cancel: cancel1Algorithm
    });

    branch2 = new ReadableStream({
        start() {},
        pull: pullAlgorithm,
        cancel: cancel2Algorithm
    });

    // Handle errors from the original reader
    reader.closed.catch((r) => {
        readableStreamDefaultControllerError(branch1[kController], r);
        readableStreamDefaultControllerError(branch2[kController], r);
        if (!canceled1 || !canceled2) {
            cancelPromise.resolve(undefined);
        }
    });

    return [branch1, branch2];
}

// =============================================================================
// Async Iterator Implementation
// =============================================================================

function readableStreamAsyncIterator(stream, options) {
    const reader = stream.getReader();
    const preventCancel = Boolean(options?.preventCancel);

    const iterator = {
        async next() {
            try {
                const result = await reader.read();
                if (result.done) {
                    reader.releaseLock();
                }
                return result;
            } catch (e) {
                reader.releaseLock();
                throw e;
            }
        },

        async return(value) {
            if (!preventCancel) {
                const cancelResult = reader.cancel(value);
                reader.releaseLock();
                await cancelResult;
            } else {
                reader.releaseLock();
            }
            return { value, done: true };
        },

        [Symbol.asyncIterator]() {
            return this;
        }
    };

    return iterator;
}

// =============================================================================
// ReadableStream.from() Implementation
// =============================================================================

function readableStreamFromIterable(asyncIterable) {
    let iterator;
    let nextMethod;

    // Get iterator
    if (asyncIterable[Symbol.asyncIterator]) {
        iterator = asyncIterable[Symbol.asyncIterator]();
    } else if (asyncIterable[Symbol.iterator]) {
        iterator = asyncIterable[Symbol.iterator]();
    } else {
        throw new TypeError('Value is not iterable');
    }

    nextMethod = iterator.next;

    return new ReadableStream({
        async pull(controller) {
            let result;
            try {
                result = await nextMethod.call(iterator);
            } catch (e) {
                controller.error(e);
                return;
            }

            if (result.done) {
                controller.close();
            } else {
                controller.enqueue(result.value);
            }
        },

        async cancel(reason) {
            if (iterator.return) {
                try {
                    await iterator.return(reason);
                } catch (e) {
                    // Ignore errors from return
                }
            }
        }
    });
}

// =============================================================================
// Internal Factory Function (for TransformStream)
// =============================================================================

function createInternalReadableStream(startAlgorithm, pullAlgorithm, cancelAlgorithm, highWaterMark, sizeAlgorithm) {
    const stream = Object.create(ReadableStream.prototype);
    stream[kIsReadableStream] = true;
    stream[kState] = STATE.READABLE;
    stream[kStoredError] = undefined;
    stream[kReader] = undefined;
    stream[kController] = undefined;
    stream[kDisturbed] = false;
    
    const controller = createReadableStreamDefaultController();
    setUpReadableStreamDefaultController(
        stream, controller, startAlgorithm, pullAlgorithm, cancelAlgorithm,
        highWaterMark, sizeAlgorithm
    );
    
    return stream;
}

// =============================================================================
// Exports
// =============================================================================

Object.assign(exports, {
    // Classes
    ReadableStream,
    ReadableStreamDefaultReader,
    ReadableStreamDefaultController,
    ReadableStreamBYOBReader,
    ReadableByteStreamController,
    ReadableStreamBYOBRequest,
    
    // Internal exports for TransformStream
    kState,
    kController,
    kStoredError,
    readableStreamDefaultControllerClose,
    readableStreamDefaultControllerEnqueue,
    readableStreamDefaultControllerError,
    readableStreamDefaultControllerCanCloseOrEnqueue,
    readableStreamDefaultControllerHasBackpressure,
    readableStreamDefaultControllerGetDesiredSize,
    createInternalReadableStream,
    readableStreamCancel,
    
    // Type checking
    isReadableStream,
    isReadableStreamLocked,
    isReadableByteStreamController
});



