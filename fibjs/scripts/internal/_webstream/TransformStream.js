'use strict';

/**
 * WHATWG Streams Standard - TransformStream Implementation
 * Pure JavaScript implementation for fibjs
 */

const {
    WRITABLE_STATE,
    kState,
    kStoredError,
    kController,
    kIsTransformStream,
    kBackpressure,
    kBackpressureChangePromise,
    kReadable,
    kWritable,
    kStream,
    kFlushAlgorithm,
    kTransformAlgorithm,
    kCancelAlgorithm,
    kFinishPromise
} = require('./symbols');

const {
    isObject,
    isTransformStream,
    createPromiseCapability
} = require('./helpers');

// Import internal interfaces from ReadableStream and WritableStream
const {
    kState: kReadableState,
    kController: kReadableController,
    kStoredError: kReadableStoredError,
    readableStreamDefaultControllerClose,
    readableStreamDefaultControllerEnqueue,
    readableStreamDefaultControllerError,
    readableStreamDefaultControllerCanCloseOrEnqueue,
    readableStreamDefaultControllerHasBackpressure,
    readableStreamDefaultControllerGetDesiredSize,
    createInternalReadableStream
} = require('./ReadableStream');

const {
    kState: kWritableState,
    kController: kWritableController,
    kStoredError: kWritableStoredError,
    STATE: WRITABLE_STATE_CONST,
    writableStreamDefaultControllerErrorIfNeeded,
    createInternalWritableStream
} = require('./WritableStream');

// =============================================================================
// TransformStreamDefaultController
// =============================================================================

class TransformStreamDefaultController {
    constructor() {
        throw new TypeError('Illegal constructor');
    }

    get desiredSize() {
        const stream = this[kStream];
        const readableController = stream[kReadable][kReadableController];
        return readableStreamDefaultControllerGetDesiredSize(readableController);
    }

    enqueue(chunk = undefined) {
        transformStreamDefaultControllerEnqueue(this, chunk);
    }

    error(reason = undefined) {
        transformStreamDefaultControllerError(this, reason);
    }

    terminate() {
        transformStreamDefaultControllerTerminate(this);
    }
}

Object.defineProperty(TransformStreamDefaultController.prototype, Symbol.toStringTag, {
    value: 'TransformStreamDefaultController',
    configurable: true
});

function createTransformStreamDefaultController() {
    return Object.create(TransformStreamDefaultController.prototype);
}

// =============================================================================
// TransformStreamDefaultController Algorithms
// =============================================================================

function transformStreamDefaultControllerEnqueue(controller, chunk) {
    const stream = controller[kStream];
    const readableController = stream[kReadable][kReadableController];

    if (!readableStreamDefaultControllerCanCloseOrEnqueue(readableController)) {
        throw new TypeError('Readable side is not in a state that permits enqueue');
    }

    try {
        readableStreamDefaultControllerEnqueue(readableController, chunk);
    } catch (e) {
        transformStreamErrorWritableAndUnblockWrite(stream, e);
        throw stream[kReadable][kReadableStoredError];
    }

    const backpressure = readableStreamDefaultControllerHasBackpressure(readableController);
    if (backpressure !== stream[kBackpressure]) {
        transformStreamSetBackpressure(stream, backpressure);
    }
}

function transformStreamDefaultControllerError(controller, e) {
    transformStreamError(controller[kStream], e);
}

function transformStreamDefaultControllerTerminate(controller) {
    const stream = controller[kStream];
    const readableController = stream[kReadable][kReadableController];

    readableStreamDefaultControllerClose(readableController);
    transformStreamErrorWritableAndUnblockWrite(stream, new TypeError('TransformStream terminated'));
}

function transformStreamDefaultControllerPerformTransform(controller, chunk) {
    const transformPromise = controller[kTransformAlgorithm](chunk);
    return transformPromise.catch((r) => {
        transformStreamError(controller[kStream], r);
        throw r;
    });
}

// =============================================================================
// TransformStream Algorithms
// =============================================================================

function transformStreamError(stream, e) {
    const readableController = stream[kReadable][kReadableController];

    if (readableStreamDefaultControllerCanCloseOrEnqueue(readableController)) {
        readableStreamDefaultControllerError(readableController, e);
    }

    transformStreamErrorWritableAndUnblockWrite(stream, e);
}

function transformStreamErrorWritableAndUnblockWrite(stream, e) {
    const controller = stream[kController];

    controller[kTransformAlgorithm] = () => Promise.reject(e);
    controller[kFlushAlgorithm] = () => Promise.reject(e);

    transformStreamUnblockWrite(stream);

    // Error the writable side
    const writableController = stream[kWritable][kWritableController];
    if (writableController) {
        writableStreamDefaultControllerErrorIfNeeded(writableController, e);
    }
}

function transformStreamUnblockWrite(stream) {
    if (stream[kBackpressure]) {
        transformStreamSetBackpressure(stream, false);
    }
}

function transformStreamDefaultControllerClearAlgorithms(controller) {
    controller[kTransformAlgorithm] = undefined;
    controller[kFlushAlgorithm] = undefined;
    controller[kCancelAlgorithm] = undefined;
}

function transformStreamSetBackpressure(stream, backpressure) {
    if (stream[kBackpressureChangePromise] !== undefined) {
        stream[kBackpressureChangePromise].resolve(undefined);
    }
    stream[kBackpressureChangePromise] = createPromiseCapability();
    stream[kBackpressure] = backpressure;
}
// =============================================================================
// TransformStream Sink/Source Algorithms
// =============================================================================

function transformStreamDefaultSinkWriteAlgorithm(stream, chunk) {
    const controller = stream[kController];

    if (stream[kBackpressure]) {
        const backpressureChangePromise = stream[kBackpressureChangePromise];
        return backpressureChangePromise.promise.then(() => {
            const writable = stream[kWritable];
            const state = writable[kWritableState];
            if (state === WRITABLE_STATE_CONST.ERRORING) {
                throw writable[kWritableStoredError];
            }
            return transformStreamDefaultControllerPerformTransform(controller, chunk);
        });
    }

    return transformStreamDefaultControllerPerformTransform(controller, chunk);
}

function transformStreamDefaultSinkCloseAlgorithm(stream) {
    const controller = stream[kController];
    const readableController = stream[kReadable][kReadableController];

    // Use finishPromise to prevent duplicate execution
    if (controller[kFinishPromise] !== undefined) {
        return controller[kFinishPromise];
    }

    const { promise, resolve, reject } = createPromiseCapability();
    controller[kFinishPromise] = promise;

    const flushPromise = controller[kFlushAlgorithm]();
    transformStreamDefaultControllerClearAlgorithms(controller);

    Promise.resolve(flushPromise).then(
        () => {
            if (stream[kReadable][kReadableState] === 'errored') {
                reject(stream[kReadable][kReadableStoredError]);
            } else {
                if (readableStreamDefaultControllerCanCloseOrEnqueue(readableController)) {
                    readableStreamDefaultControllerClose(readableController);
                }
                resolve();
            }
        },
        (r) => {
            readableStreamDefaultControllerError(readableController, r);
            reject(r);
        }
    );

    return promise;
}

function transformStreamDefaultSinkAbortAlgorithm(stream, reason) {
    const controller = stream[kController];

    // Use finishPromise to prevent duplicate execution
    if (controller[kFinishPromise] !== undefined) {
        return controller[kFinishPromise];
    }

    const { promise, resolve, reject } = createPromiseCapability();
    controller[kFinishPromise] = promise;

    const cancelPromise = controller[kCancelAlgorithm] ? controller[kCancelAlgorithm](reason) : Promise.resolve();
    transformStreamDefaultControllerClearAlgorithms(controller);

    Promise.resolve(cancelPromise).then(
        () => {
            if (stream[kReadable][kReadableState] === 'errored') {
                reject(stream[kReadable][kReadableStoredError]);
            } else {
                readableStreamDefaultControllerError(stream[kReadable][kReadableController], reason);
                resolve();
            }
        },
        (error) => {
            readableStreamDefaultControllerError(stream[kReadable][kReadableController], error);
            reject(error);
        }
    );

    return promise;
}

function transformStreamDefaultSourcePullAlgorithm(stream) {
    transformStreamSetBackpressure(stream, false);
    return stream[kBackpressureChangePromise].promise;
}

function transformStreamDefaultSourceCancelAlgorithm(stream, reason) {
    const controller = stream[kController];

    // Use finishPromise to prevent duplicate execution
    if (controller[kFinishPromise] !== undefined) {
        return controller[kFinishPromise];
    }

    const { promise, resolve, reject } = createPromiseCapability();
    controller[kFinishPromise] = promise;

    const cancelPromise = controller[kCancelAlgorithm] ? controller[kCancelAlgorithm](reason) : Promise.resolve();
    transformStreamDefaultControllerClearAlgorithms(controller);

    Promise.resolve(cancelPromise).then(
        () => {
            if (stream[kWritable][kWritableState] === 'errored') {
                reject(stream[kWritable][kWritableStoredError]);
            } else {
                writableStreamDefaultControllerErrorIfNeeded(stream[kWritable][kWritableController], reason);
                transformStreamUnblockWrite(stream);
                resolve();
            }
        },
        (error) => {
            writableStreamDefaultControllerErrorIfNeeded(stream[kWritable][kWritableController], error);
            transformStreamUnblockWrite(stream);
            reject(error);
        }
    );

    return promise;
}

// =============================================================================
// TransformStream Class
// =============================================================================

class TransformStream {
    constructor(transformer = {}, writableStrategy = {}, readableStrategy = {}) {
        if (transformer !== undefined && !isObject(transformer)) {
            throw new TypeError('transformer must be an object');
        }

        this[kIsTransformStream] = true;

        const writableHighWaterMark = writableStrategy?.highWaterMark ?? 1;
        const writableSizeAlgorithm = writableStrategy?.size ?? (() => 1);
        const readableHighWaterMark = readableStrategy?.highWaterMark ?? 0;
        const readableSizeAlgorithm = readableStrategy?.size ?? (() => 1);

        // Create controller
        const controller = createTransformStreamDefaultController();
        this[kController] = controller;
        controller[kStream] = this;

        // Setup backpressure
        this[kBackpressureChangePromise] = createPromiseCapability();
        this[kBackpressure] = true;

        // Create start promise for synchronizing readable and writable
        const startPromise = createPromiseCapability();

        // Create writable side
        const transformStream = this;
        this[kWritable] = createInternalWritableStream(
            () => startPromise.promise,
            (chunk) => transformStreamDefaultSinkWriteAlgorithm(transformStream, chunk),
            () => transformStreamDefaultSinkCloseAlgorithm(transformStream),
            (reason) => transformStreamDefaultSinkAbortAlgorithm(transformStream, reason),
            writableHighWaterMark,
            writableSizeAlgorithm
        );

        // Create readable side
        this[kReadable] = createInternalReadableStream(
            () => startPromise.promise,
            () => transformStreamDefaultSourcePullAlgorithm(transformStream),
            (reason) => transformStreamDefaultSourceCancelAlgorithm(transformStream, reason),
            readableHighWaterMark,
            readableSizeAlgorithm
        );

        // Setup transform algorithm
        const transformAlgorithm = transformer.transform
            ? (chunk) => {
                try {
                    const result = transformer.transform(chunk, controller);
                    return Promise.resolve(result);
                } catch (e) {
                    return Promise.reject(e);
                }
            }
            : (chunk) => {
                try {
                    controller.enqueue(chunk);
                    return Promise.resolve();
                } catch (e) {
                    return Promise.reject(e);
                }
            };

        const flushAlgorithm = transformer.flush
            ? () => {
                try {
                    const result = transformer.flush(controller);
                    return Promise.resolve(result);
                } catch (e) {
                    return Promise.reject(e);
                }
            }
            : () => Promise.resolve();

        const cancelAlgorithm = transformer.cancel
            ? (reason) => {
                try {
                    const result = transformer.cancel(reason);
                    return Promise.resolve(result);
                } catch (e) {
                    return Promise.reject(e);
                }
            }
            : () => Promise.resolve();

        controller[kTransformAlgorithm] = transformAlgorithm;
        controller[kFlushAlgorithm] = flushAlgorithm;
        controller[kCancelAlgorithm] = cancelAlgorithm;
        controller[kFinishPromise] = undefined;

        // Handle start
        const startResult = transformer.start ? transformer.start(controller) : undefined;
        Promise.resolve(startResult).then(
            () => startPromise.resolve(),
            (r) => startPromise.reject(r)
        );
    }

    get readable() {
        if (!isTransformStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        return this[kReadable];
    }

    get writable() {
        if (!isTransformStream(this)) {
            throw new TypeError('Invalid receiver');
        }
        return this[kWritable];
    }
}

Object.defineProperty(TransformStream.prototype, Symbol.toStringTag, {
    value: 'TransformStream',
    configurable: true
});

// =============================================================================
// Exports
// =============================================================================

module.exports = {
    TransformStream,
    TransformStreamDefaultController
};