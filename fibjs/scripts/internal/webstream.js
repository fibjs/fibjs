'use strict';

/**
 * WHATWG Streams Standard - Main Entry Point
 * Pure JavaScript implementation for fibjs
 */

const {
    ReadableStream,
    ReadableStreamDefaultReader,
    ReadableStreamDefaultController,
    ReadableStreamBYOBReader,
    ReadableByteStreamController,
    ReadableStreamBYOBRequest
} = require('./_webstream/ReadableStream');

const {
    WritableStream,
    WritableStreamDefaultWriter,
    WritableStreamDefaultController
} = require('./_webstream/WritableStream');

const {
    TransformStream,
    TransformStreamDefaultController
} = require('./_webstream/TransformStream');

const {
    CountQueuingStrategy,
    ByteLengthQueuingStrategy
} = require('./_webstream/strategies');

// =============================================================================
// Export to Global
// =============================================================================

globalThis.ReadableStream = ReadableStream;
globalThis.ReadableStreamDefaultReader = ReadableStreamDefaultReader;
globalThis.ReadableStreamDefaultController = ReadableStreamDefaultController;
globalThis.ReadableStreamBYOBReader = ReadableStreamBYOBReader;
globalThis.ReadableByteStreamController = ReadableByteStreamController;
globalThis.ReadableStreamBYOBRequest = ReadableStreamBYOBRequest;

globalThis.WritableStream = WritableStream;
globalThis.WritableStreamDefaultWriter = WritableStreamDefaultWriter;
globalThis.WritableStreamDefaultController = WritableStreamDefaultController;

globalThis.TransformStream = TransformStream;
globalThis.TransformStreamDefaultController = TransformStreamDefaultController;

globalThis.CountQueuingStrategy = CountQueuingStrategy;
globalThis.ByteLengthQueuingStrategy = ByteLengthQueuingStrategy;

// =============================================================================
// Module Exports
// =============================================================================

module.exports = {
    // ReadableStream
    ReadableStream,
    ReadableStreamDefaultReader,
    ReadableStreamDefaultController,
    ReadableStreamBYOBReader,
    ReadableByteStreamController,
    ReadableStreamBYOBRequest,
    
    // WritableStream
    WritableStream,
    WritableStreamDefaultWriter,
    WritableStreamDefaultController,
    
    // TransformStream
    TransformStream,
    TransformStreamDefaultController,
    
    // Queuing Strategies
    CountQueuingStrategy,
    ByteLengthQueuingStrategy
};
