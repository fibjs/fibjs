'use strict';

/**
 * WHATWG Streams Standard - Private Symbols
 * Shared symbols for internal slots across all stream types
 */

// =============================================================================
// State Constants
// =============================================================================

const READABLE_STATE = {
    READABLE: 'readable',
    CLOSED: 'closed',
    ERRORED: 'errored'
};

const WRITABLE_STATE = {
    WRITABLE: 'writable',
    CLOSED: 'closed',
    ERRORING: 'erroring',
    ERRORED: 'errored'
};

// =============================================================================
// Common Symbols
// =============================================================================

const kState = Symbol('[[state]]');
const kStoredError = Symbol('[[storedError]]');
const kController = Symbol('[[controller]]');
const kStream = Symbol('[[stream]]');
const kClosedPromise = Symbol('[[closedPromise]]');

// Queue-related symbols
const kQueue = Symbol('[[queue]]');
const kQueueTotalSize = Symbol('[[queueTotalSize]]');
const kHighWaterMark = Symbol('[[highWaterMark]]');
const kSizeAlgorithm = Symbol('[[sizeAlgorithm]]');
const kStarted = Symbol('[[started]]');

// =============================================================================
// ReadableStream Symbols
// =============================================================================

const kReader = Symbol('[[reader]]');
const kDisturbed = Symbol('[[disturbed]]');
const kReadRequests = Symbol('[[readRequests]]');
const kPullAlgorithm = Symbol('[[pullAlgorithm]]');
const kCancelAlgorithm = Symbol('[[cancelAlgorithm]]');
const kCloseRequested = Symbol('[[closeRequested]]');
const kPulling = Symbol('[[pulling]]');
const kPullAgain = Symbol('[[pullAgain]]');

// Brand checking symbols - Readable
const kIsReadableStream = Symbol('[[IsReadableStream]]');
const kIsReadableStreamDefaultReader = Symbol('[[IsReadableStreamDefaultReader]]');
const kIsReadableStreamDefaultController = Symbol('[[IsReadableStreamDefaultController]]');
const kIsReadableStreamBYOBReader = Symbol('[[IsReadableStreamBYOBReader]]');
const kIsReadableByteStreamController = Symbol('[[IsReadableByteStreamController]]');
const kIsReadableStreamBYOBRequest = Symbol('[[IsReadableStreamBYOBRequest]]');

// ReadableByteStreamController-specific symbols
const kReadIntoRequests = Symbol('[[readIntoRequests]]');
const kPendingPullIntos = Symbol('[[pendingPullIntos]]');
const kAutoAllocateChunkSize = Symbol('[[autoAllocateChunkSize]]');
const kByobRequest = Symbol('[[byobRequest]]');
const kView = Symbol('[[view]]');

// =============================================================================
// WritableStream Symbols
// =============================================================================

const kWriter = Symbol('[[writer]]');
const kWriteRequests = Symbol('[[writeRequests]]');
const kInFlightWriteRequest = Symbol('[[inFlightWriteRequest]]');
const kCloseRequest = Symbol('[[closeRequest]]');
const kInFlightCloseRequest = Symbol('[[inFlightCloseRequest]]');
const kPendingAbortRequest = Symbol('[[pendingAbortRequest]]');
const kBackpressure = Symbol('[[backpressure]]');
const kReadyPromise = Symbol('[[readyPromise]]');
const kWriteAlgorithm = Symbol('[[writeAlgorithm]]');
const kCloseAlgorithm = Symbol('[[closeAlgorithm]]');
const kAbortAlgorithm = Symbol('[[abortAlgorithm]]');

// Brand checking symbols - Writable
const kIsWritableStream = Symbol('[[IsWritableStream]]');
const kIsWritableStreamDefaultWriter = Symbol('[[IsWritableStreamDefaultWriter]]');
const kIsWritableStreamDefaultController = Symbol('[[IsWritableStreamDefaultController]]');

// =============================================================================
// TransformStream Symbols
// =============================================================================

const kIsTransformStream = Symbol('[[IsTransformStream]]');
const kBackpressureChangePromise = Symbol('[[BackpressureChangePromise]]');
const kReadable = Symbol('[[Readable]]');
const kWritable = Symbol('[[Writable]]');
const kFlushAlgorithm = Symbol('[[FlushAlgorithm]]');
const kTransformAlgorithm = Symbol('[[TransformAlgorithm]]');
const kFinishPromise = Symbol('[[FinishPromise]]');

// =============================================================================
// Exports
// =============================================================================

module.exports = {
    // State constants
    READABLE_STATE,
    WRITABLE_STATE,
    
    // Common symbols
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
    
    // ReadableStream symbols
    kReader,
    kDisturbed,
    kReadRequests,
    kPullAlgorithm,
    kCancelAlgorithm,
    kCloseRequested,
    kPulling,
    kPullAgain,
    kIsReadableStream,
    kIsReadableStreamDefaultReader,
    kIsReadableStreamDefaultController,
    kIsReadableStreamBYOBReader,
    kIsReadableByteStreamController,
    kIsReadableStreamBYOBRequest,
    kReadIntoRequests,
    kPendingPullIntos,
    kAutoAllocateChunkSize,
    kByobRequest,
    kView,
    
    // WritableStream symbols
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
    kIsWritableStreamDefaultController,
    
    // TransformStream symbols
    kIsTransformStream,
    kBackpressureChangePromise,
    kReadable,
    kWritable,
    kFlushAlgorithm,
    kTransformAlgorithm,
    kFinishPromise
};
