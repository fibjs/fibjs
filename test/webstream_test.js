const { describe, it, beforeEach } = require('node:test');
const assert = require('node:assert');

const {
  ReadableStream,
  ReadableStreamDefaultReader,
  ReadableStreamDefaultController,
  ReadableStreamBYOBReader,
  ReadableByteStreamController,
  ReadableStreamBYOBRequest,
  WritableStream,
  TransformStream,
  CountQueuingStrategy,
  ByteLengthQueuingStrategy
} = globalThis;

// Helper: delay for async tests
const delay = (ms = 0) => new Promise(resolve => setTimeout(resolve, ms));

// =============================================================================
// Test Suite: UnderlyingSource Callbacks
// =============================================================================
describe('UnderlyingSource Callbacks', () => {

  it('should call start with ReadableStreamDefaultController', async () => {
    let startCalled = false;
    let receivedController = null;

    new ReadableStream({
      start(controller) {
        startCalled = true;
        receivedController = controller;
      }
    });

    assert.strictEqual(startCalled, true);
    assert.ok(receivedController instanceof ReadableStreamDefaultController);
  });

  it('should call start with ReadableByteStreamController for bytes type', async () => {
    let startCalled = false;
    let receivedController = null;

    new ReadableStream({
      type: 'bytes',
      start(controller) {
        startCalled = true;
        receivedController = controller;
      }
    });

    assert.strictEqual(startCalled, true);
    assert.ok(receivedController instanceof ReadableByteStreamController);
  });

  it('should call pull when reader reads', async () => {
    let pullCalled = false;

    const stream = new ReadableStream({
      pull(controller) {
        pullCalled = true;
        controller.close();
      }
    });

    const reader = stream.getReader();
    await reader.read();

    assert.strictEqual(pullCalled, true);
  });

  it('should call cancel when stream is cancelled', async () => {
    let cancelCalled = false;
    let cancelReason = null;

    const stream = new ReadableStream({
      cancel(reason) {
        cancelCalled = true;
        cancelReason = reason;
      }
    });

    await stream.cancel('test reason');

    assert.strictEqual(cancelCalled, true);
    assert.strictEqual(cancelReason, 'test reason');
  });

  it('should support async start', async () => {
    let startCompleted = false;

    const stream = new ReadableStream({
      async start(controller) {
        await delay(10);
        controller.enqueue('data');
        controller.close();
        startCompleted = true;
      }
    });

    const reader = stream.getReader();
    const result = await reader.read();

    assert.strictEqual(startCompleted, true);
    assert.strictEqual(result.value, 'data');
  });

});

// =============================================================================
// Test Suite: ReadableStream Constructor
// =============================================================================
describe('ReadableStream Constructor', () => {

  it('should create a ReadableStream with default options', () => {
    const stream = new ReadableStream();
    assert.strictEqual(typeof stream.locked, 'boolean');
    assert.strictEqual(stream.locked, false);
  });

  it('should have required methods', () => {
    const stream = new ReadableStream();
    assert.strictEqual(typeof stream.cancel, 'function');
    assert.strictEqual(typeof stream.getReader, 'function');
    assert.strictEqual(typeof stream.pipeThrough, 'function');
    assert.strictEqual(typeof stream.pipeTo, 'function');
    assert.strictEqual(typeof stream.tee, 'function');
  });

  it('should accept empty object as source', () => {
    const stream = new ReadableStream({});
    assert.ok(stream instanceof ReadableStream);
  });

  it('should accept empty object as strategy', () => {
    const stream = new ReadableStream({}, {});
    assert.ok(stream instanceof ReadableStream);
  });

  it('should accept null as strategy', () => {
    const stream = new ReadableStream({}, null);
    assert.ok(stream instanceof ReadableStream);
  });

  it('should throw for invalid source types', () => {
    ['a', false, 1, null].forEach((source) => {
      assert.throws(() => new ReadableStream(source), {
        code: 'ERR_INVALID_ARG_TYPE',
      });
    });
  });

  it('should throw for invalid strategy types', () => {
    ['a', false, 1].forEach((strategy) => {
      assert.throws(() => new ReadableStream({}, strategy), {
        code: 'ERR_INVALID_ARG_TYPE',
      });
    });
  });

  it('should throw for invalid size function', () => {
    ['a', {}, false].forEach((size) => {
      assert.throws(() => new ReadableStream({}, { size }), {
        code: 'ERR_INVALID_ARG_TYPE',
      });
    });
  });

  it('should throw for invalid highWaterMark', () => {
    ['a', {}].forEach((highWaterMark) => {
      assert.throws(() => new ReadableStream({}, { highWaterMark }), {
        code: 'ERR_INVALID_ARG_VALUE',
      });
    });

    [-1, NaN].forEach((highWaterMark) => {
      assert.throws(() => new ReadableStream({}, { highWaterMark }), {
        code: 'ERR_INVALID_ARG_VALUE',
      });
    });
  });

});

// =============================================================================
// Test Suite: ReadableStreamDefaultReader
// =============================================================================
describe('ReadableStreamDefaultReader', () => {

  it('should lock the stream when reader is obtained', () => {
    const stream = new ReadableStream();
    assert.strictEqual(stream.locked, false);

    stream.getReader();
    assert.strictEqual(stream.locked, true);
  });

  it('should unlock when releaseLock is called', () => {
    const stream = new ReadableStream();
    const reader = stream.getReader();

    assert.strictEqual(stream.locked, true);
    reader.releaseLock();
    assert.strictEqual(stream.locked, false);
  });

  it('should throw when getting reader on locked stream', () => {
    const stream = new ReadableStream();
    stream.getReader();

    assert.throws(() => stream.getReader(), {
      code: 'ERR_INVALID_STATE',
    });
  });

  it('should read chunks in order', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue('chunk1');
        controller.enqueue('chunk2');
        controller.close();
      }
    });

    const reader = stream.getReader();

    const r1 = await reader.read();
    assert.deepStrictEqual(r1, { value: 'chunk1', done: false });

    const r2 = await reader.read();
    assert.deepStrictEqual(r2, { value: 'chunk2', done: false });

    const r3 = await reader.read();
    assert.deepStrictEqual(r3, { value: undefined, done: true });
  });

  it('should have closed promise that resolves on close', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.close();
      }
    });

    const reader = stream.getReader();
    await reader.closed; // Should resolve without error
  });

  it('should reject read after releaseLock', async () => {
    const stream = new ReadableStream();
    const reader = stream.getReader();
    reader.releaseLock();

    await assert.rejects(reader.read(), {
      code: 'ERR_INVALID_STATE',
    });
  });

  it('should support cancel through reader', async () => {
    let cancelCalled = false;

    const stream = new ReadableStream({
      cancel() {
        cancelCalled = true;
      }
    });

    const reader = stream.getReader();
    await reader.cancel();

    assert.strictEqual(cancelCalled, true);
  });

});

// =============================================================================
// Test Suite: ReadableStreamDefaultController
// =============================================================================
describe('ReadableStreamDefaultController', () => {

  it('should not be constructable directly', () => {
    assert.throws(() => new ReadableStreamDefaultController(), {
      code: 'ERR_ILLEGAL_CONSTRUCTOR',
    });
  });

  it('should have correct desiredSize initially', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, { highWaterMark: 10 });

    assert.strictEqual(controller.desiredSize, 10);
  });

  it('should decrease desiredSize when enqueue', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, { highWaterMark: 3 });

    assert.strictEqual(controller.desiredSize, 3);
    controller.enqueue('a');
    assert.strictEqual(controller.desiredSize, 2);
    controller.enqueue('b');
    assert.strictEqual(controller.desiredSize, 1);
    controller.enqueue('c');
    assert.strictEqual(controller.desiredSize, 0);
  });

  it('should have desiredSize 0 after close', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, { highWaterMark: 10 });

    controller.close();
    assert.strictEqual(controller.desiredSize, 0);
  });

  it('should have desiredSize null after error', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, { highWaterMark: 10 });

    controller.error(new Error('test'));
    assert.strictEqual(controller.desiredSize, null);
  });

  it('should throw when enqueue after close', () => {
    let controller;
    new ReadableStream({
      start(c) {
        controller = c;
        c.close();
      }
    });

    assert.throws(() => controller.enqueue('data'), {
      code: 'ERR_INVALID_STATE',
    });
  });

  it('should throw when close twice', () => {
    let controller;
    new ReadableStream({
      start(c) {
        controller = c;
        c.close();
      }
    });

    assert.throws(() => controller.close(), {
      code: 'ERR_INVALID_STATE',
    });
  });

});

// =============================================================================
// Test Suite: ReadableStream.tee()
// =============================================================================
describe('ReadableStream.tee()', () => {

  it('should return two ReadableStreams', () => {
    const stream = new ReadableStream();
    const [s1, s2] = stream.tee();

    assert.ok(s1 instanceof ReadableStream);
    assert.ok(s2 instanceof ReadableStream);
  });

  it('should lock the original stream', () => {
    const stream = new ReadableStream();
    assert.strictEqual(stream.locked, false);

    stream.tee();
    assert.strictEqual(stream.locked, true);
  });

  it('should provide same data to both branches', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue('a');
        controller.enqueue('b');
        controller.close();
      }
    });

    const [s1, s2] = stream.tee();

    async function readAll(stream) {
      const reader = stream.getReader();
      const chunks = [];
      let result;
      while (!(result = await reader.read()).done) {
        chunks.push(result.value);
      }
      return chunks;
    }

    const [chunks1, chunks2] = await Promise.all([
      readAll(s1),
      readAll(s2),
    ]);

    assert.deepStrictEqual(chunks1, ['a', 'b']);
    assert.deepStrictEqual(chunks2, ['a', 'b']);
  });

  it('should allow cancelling one branch without affecting the other', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue('a');
        controller.enqueue('b');
        controller.close();
      }
    });

    const [s1, s2] = stream.tee();

    // Cancel s2 immediately
    s2.cancel();

    // s1 should still work
    const reader = s1.getReader();
    const r1 = await reader.read();
    assert.deepStrictEqual(r1, { value: 'a', done: false });

    const r2 = await reader.read();
    assert.deepStrictEqual(r2, { value: 'b', done: false });
  });

  it('should propagate errors to both branches', async () => {
    let controller;
    const stream = new ReadableStream({
      start(c) { controller = c; }
    });

    const [s1, s2] = stream.tee();
    const reader1 = s1.getReader();
    const reader2 = s2.getReader();

    const error = new Error('test error');
    controller.error(error);

    await assert.rejects(reader1.read(), error);
    await assert.rejects(reader2.read(), error);
  });

});

// =============================================================================
// Test Suite: Async Iterator
// =============================================================================
describe('Async Iterator (for await...of)', () => {

  it('should be iterable with for-await-of', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue('a');
        controller.enqueue('b');
        controller.enqueue('c');
        controller.close();
      }
    });

    const chunks = [];
    for await (const chunk of stream) {
      chunks.push(chunk);
    }

    assert.deepStrictEqual(chunks, ['a', 'b', 'c']);
  });

  it('should support values() method', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue(1);
        controller.enqueue(2);
        controller.close();
      }
    });

    const chunks = [];
    for await (const chunk of stream.values()) {
      chunks.push(chunk);
    }

    assert.deepStrictEqual(chunks, [1, 2]);
  });

  it('should unlock stream after iteration completes', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue('data');
        controller.close();
      }
    });

    for await (const _ of stream) {
      // consume
    }

    assert.strictEqual(stream.locked, false);
  });

  it('should support preventCancel option', async () => {
    let cancelCalled = false;
    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue('a');
        controller.enqueue('b');
      },
      cancel() {
        cancelCalled = true;
      }
    });

    // Break early with preventCancel: true
    for await (const _ of stream.values({ preventCancel: true })) {
      break;
    }

    assert.strictEqual(cancelCalled, false);
  });

  it('should cancel stream on early break by default', async () => {
    let cancelCalled = false;
    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue('a');
        controller.enqueue('b');
      },
      cancel() {
        cancelCalled = true;
      }
    });

    for await (const _ of stream.values({ preventCancel: false })) {
      break;
    }

    assert.strictEqual(cancelCalled, true);
  });

});

// =============================================================================
// Test Suite: ReadableStream.from()
// =============================================================================
describe('ReadableStream.from()', () => {

  it('should create stream from array', async () => {
    const stream = ReadableStream.from(['a', 'b', 'c']);

    const chunks = [];
    for await (const chunk of stream) {
      chunks.push(chunk);
    }

    assert.deepStrictEqual(chunks, ['a', 'b', 'c']);
  });

  it('should create stream from async generator', async () => {
    async function* gen() {
      yield 1;
      yield 2;
      yield 3;
    }

    const stream = ReadableStream.from(gen());

    const chunks = [];
    for await (const chunk of stream) {
      chunks.push(chunk);
    }

    assert.deepStrictEqual(chunks, [1, 2, 3]);
  });

  it('should create stream from sync generator', async () => {
    function* gen() {
      yield 'x';
      yield 'y';
    }

    const stream = ReadableStream.from(gen());

    const chunks = [];
    for await (const chunk of stream) {
      chunks.push(chunk);
    }

    assert.deepStrictEqual(chunks, ['x', 'y']);
  });

});

// =============================================================================
// Test Suite: Error Handling
// =============================================================================
describe('Error Handling', () => {

  it('should transition to errored state on controller.error()', async () => {
    const error = new Error('test error');
    let controller;

    const stream = new ReadableStream({
      start(c) { controller = c; }
    });

    controller.error(error);

    const reader = stream.getReader();
    await assert.rejects(reader.read(), error);
  });

  it('should reject closed promise on error', async () => {
    const error = new Error('boom');

    const stream = new ReadableStream({
      start(controller) {
        controller.error(error);
      }
    });

    const reader = stream.getReader();
    await assert.rejects(reader.closed, error);
  });

  it('should handle error thrown in async start()', async () => {
    const stream = new ReadableStream({
      async start() {
        throw new Error('start error');
      }
    });

    // Async start error puts stream in errored state
    await delay(0);
    const reader = stream.getReader();
    await assert.rejects(reader.read(), { message: 'start error' });
  });

  it('should handle error thrown in pull()', async () => {
    const stream = new ReadableStream({
      pull() {
        throw new Error('pull error');
      }
    });

    const reader = stream.getReader();
    await assert.rejects(reader.read(), { message: 'pull error' });
  });

  it('should handle rejected promise in cancel()', async () => {
    const stream = new ReadableStream({
      cancel() {
        return Promise.reject(new Error('cancel error'));
      }
    });

    await assert.rejects(stream.cancel(), { message: 'cancel error' });
  });

});

// =============================================================================
// Test Suite: Backpressure (highWaterMark)
// =============================================================================
describe('Backpressure', () => {

  it('should respect highWaterMark with default strategy', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, { highWaterMark: 3 });

    assert.strictEqual(controller.desiredSize, 3);

    controller.enqueue('a');
    assert.strictEqual(controller.desiredSize, 2);

    controller.enqueue('b');
    controller.enqueue('c');
    assert.strictEqual(controller.desiredSize, 0);

    controller.enqueue('d');
    assert.strictEqual(controller.desiredSize, -1);
  });

  it('should use size function from strategy', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, {
      highWaterMark: 10,
      size(chunk) { return chunk.length; }
    });

    assert.strictEqual(controller.desiredSize, 10);

    controller.enqueue('hello'); // size = 5
    assert.strictEqual(controller.desiredSize, 5);

    controller.enqueue('world'); // size = 5
    assert.strictEqual(controller.desiredSize, 0);
  });

  it('should work with CountQueuingStrategy', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, new CountQueuingStrategy({ highWaterMark: 2 }));

    assert.strictEqual(controller.desiredSize, 2);
    controller.enqueue('any');
    assert.strictEqual(controller.desiredSize, 1);
  });

  it('should work with ByteLengthQueuingStrategy', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, new ByteLengthQueuingStrategy({ highWaterMark: 1024 }));

    assert.strictEqual(controller.desiredSize, 1024);

    controller.enqueue(new Uint8Array(100));
    assert.strictEqual(controller.desiredSize, 924);
  });

  it('should increase desiredSize after read', async () => {
    let controller;
    const stream = new ReadableStream({
      start(c) { controller = c; }
    }, { highWaterMark: 2 });

    controller.enqueue('a');
    controller.enqueue('b');
    assert.strictEqual(controller.desiredSize, 0);

    const reader = stream.getReader();
    await reader.read();
    assert.strictEqual(controller.desiredSize, 1);

    await reader.read();
    assert.strictEqual(controller.desiredSize, 2);
  });

});

// =============================================================================
// Test Suite: ReadableByteStream (BYOB)
// =============================================================================
describe('ReadableByteStream (type: bytes)', () => {

  it('should create byte stream with type: bytes', () => {
    const stream = new ReadableStream({
      type: 'bytes'
    });

    assert.ok(stream instanceof ReadableStream);
  });

  it('should throw when size is specified for bytes type', () => {
    assert.throws(() => {
      new ReadableStream({ type: 'bytes' }, { size: () => 1 });
    }, {
      code: 'ERR_INVALID_ARG_VALUE',
    });
  });

  it('should get BYOB reader with mode: byob', () => {
    const stream = new ReadableStream({ type: 'bytes' });
    const reader = stream.getReader({ mode: 'byob' });

    assert.ok(reader instanceof ReadableStreamBYOBReader);
  });

  it('should throw for BYOB reader on non-byte stream', () => {
    const stream = new ReadableStream();

    assert.throws(() => stream.getReader({ mode: 'byob' }), {
      code: 'ERR_INVALID_ARG_VALUE',
    });
  });

  it('should read into provided buffer', async () => {
    const stream = new ReadableStream({
      type: 'bytes',
      pull(controller) {
        const view = new Uint8Array([1, 2, 3, 4]);
        controller.enqueue(view);
        controller.close();
      }
    });

    const reader = stream.getReader({ mode: 'byob' });
    const buffer = new Uint8Array(10);

    const { value, done } = await reader.read(buffer);

    assert.strictEqual(done, false);
    assert.ok(value instanceof Uint8Array);
    assert.deepStrictEqual([...value], [1, 2, 3, 4]);
  });

  it('should support byobRequest', async () => {
    let byobRequestReceived = false;

    const stream = new ReadableStream({
      type: 'bytes',
      pull(controller) {
        if (controller.byobRequest) {
          byobRequestReceived = true;
          const view = controller.byobRequest.view;
          view[0] = 42;
          controller.byobRequest.respond(1);
        }
      }
    });

    const reader = stream.getReader({ mode: 'byob' });
    const buffer = new Uint8Array(1);
    await reader.read(buffer);

    assert.strictEqual(byobRequestReceived, true);
  });

});

// =============================================================================
// Test Suite: pipeTo
// =============================================================================
describe('pipeTo()', () => {

  it('should pipe data from readable to writable', async () => {
    const chunks = [];

    const readable = new ReadableStream({
      start(controller) {
        controller.enqueue('a');
        controller.enqueue('b');
        controller.close();
      }
    });

    const writable = new WritableStream({
      write(chunk) {
        chunks.push(chunk);
      }
    });

    await readable.pipeTo(writable);

    assert.deepStrictEqual(chunks, ['a', 'b']);
  });

  it('should close writable by default', async () => {
    let writableClosed = false;

    const readable = new ReadableStream({
      start(controller) {
        controller.close();
      }
    });

    const writable = new WritableStream({
      close() {
        writableClosed = true;
      }
    });

    await readable.pipeTo(writable);

    assert.strictEqual(writableClosed, true);
  });

  it('should respect preventClose option', async () => {
    let writableClosed = false;

    const readable = new ReadableStream({
      start(controller) {
        controller.close();
      }
    });

    const writable = new WritableStream({
      close() {
        writableClosed = true;
      }
    });

    await readable.pipeTo(writable, { preventClose: true });

    assert.strictEqual(writableClosed, false);
  });

  it('should throw when readable is locked', async () => {
    const readable = new ReadableStream();
    readable.getReader(); // Lock it

    const writable = new WritableStream();

    await assert.rejects(readable.pipeTo(writable), {
      code: 'ERR_INVALID_STATE',
    });
  });

  it('should throw when writable is locked', async () => {
    const readable = new ReadableStream();
    const writable = new WritableStream();
    writable.getWriter(); // Lock it

    await assert.rejects(readable.pipeTo(writable), {
      code: 'ERR_INVALID_STATE',
    });
  });

});

// =============================================================================
// Test Suite: pipeThrough
// =============================================================================
describe('pipeThrough()', () => {

  it('should pipe through transform stream', async () => {
    const readable = new ReadableStream({
      start(controller) {
        controller.enqueue('hello');
        controller.enqueue('world');
        controller.close();
      }
    });

    const transform = new TransformStream({
      transform(chunk, controller) {
        controller.enqueue(chunk.toUpperCase());
      }
    });

    const result = readable.pipeThrough(transform);

    assert.ok(result instanceof ReadableStream);

    const chunks = [];
    for await (const chunk of result) {
      chunks.push(chunk);
    }

    assert.deepStrictEqual(chunks, ['HELLO', 'WORLD']);
  });

  it('should lock the readable stream', () => {
    const readable = new ReadableStream();
    const transform = new TransformStream();

    readable.pipeThrough(transform);

    assert.strictEqual(readable.locked, true);
  });

  it('should throw when readable is locked', () => {
    const readable = new ReadableStream();
    readable.getReader(); // Lock it

    const transform = new TransformStream();

    assert.throws(() => readable.pipeThrough(transform), {
      code: 'ERR_INVALID_STATE',
    });
  });

});

// =============================================================================
// Test Suite: Symbol.toStringTag
// =============================================================================
describe('Symbol.toStringTag', () => {

  it('should have correct toStringTag for all classes', () => {
    const testCases = [
      [ReadableStream, 'ReadableStream'],
      [ReadableStreamDefaultReader, 'ReadableStreamDefaultReader'],
      [ReadableStreamDefaultController, 'ReadableStreamDefaultController'],
      [ReadableByteStreamController, 'ReadableByteStreamController'],
      [ReadableStreamBYOBReader, 'ReadableStreamBYOBReader'],
      [ReadableStreamBYOBRequest, 'ReadableStreamBYOBRequest'],
      [ByteLengthQueuingStrategy, 'ByteLengthQueuingStrategy'],
      [CountQueuingStrategy, 'CountQueuingStrategy'],
    ];

    for (const [cls, name] of testCases) {
      assert.strictEqual(cls.prototype[Symbol.toStringTag], name);

      const descriptor = Object.getOwnPropertyDescriptor(
        cls.prototype,
        Symbol.toStringTag
      );
      assert.strictEqual(descriptor.configurable, true);
      assert.strictEqual(descriptor.enumerable, false);
      assert.strictEqual(descriptor.writable, false);
      assert.strictEqual(descriptor.value, name);
    }
  });

});

// =============================================================================
// Test Suite: Brand Checking (ERR_INVALID_THIS)
// =============================================================================
describe('Brand Checking', () => {

  it('should throw ERR_INVALID_THIS for ReadableStream methods', async () => {
    assert.throws(() => Reflect.get(ReadableStream.prototype, 'locked', {}), {
      code: 'ERR_INVALID_THIS',
    });

    await assert.rejects(ReadableStream.prototype.cancel.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => ReadableStream.prototype.getReader.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => ReadableStream.prototype.tee.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => ReadableStream.prototype.values.call({}), {
      code: 'ERR_INVALID_THIS',
    });
  });

  it('should throw ERR_INVALID_THIS for Reader methods', async () => {
    await assert.rejects(ReadableStreamDefaultReader.prototype.read.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    await assert.rejects(ReadableStreamDefaultReader.prototype.cancel.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => ReadableStreamDefaultReader.prototype.releaseLock.call({}), {
      code: 'ERR_INVALID_THIS',
    });
  });

  it('should throw ERR_INVALID_THIS for BYOB Reader methods', async () => {
    await assert.rejects(ReadableStreamBYOBReader.prototype.read.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => ReadableStreamBYOBReader.prototype.releaseLock.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    await assert.rejects(ReadableStreamBYOBReader.prototype.cancel.call({}), {
      code: 'ERR_INVALID_THIS',
    });
  });

  it('should throw ERR_INVALID_THIS for ByteStreamController methods', () => {
    assert.throws(() => {
      Reflect.get(ReadableByteStreamController.prototype, 'byobRequest', {});
    }, {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => {
      Reflect.get(ReadableByteStreamController.prototype, 'desiredSize', {});
    }, {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => ReadableByteStreamController.prototype.close.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => ReadableByteStreamController.prototype.enqueue.call({}), {
      code: 'ERR_INVALID_THIS',
    });

    assert.throws(() => ReadableByteStreamController.prototype.error.call({}), {
      code: 'ERR_INVALID_THIS',
    });
  });

  it('should throw ERR_ILLEGAL_CONSTRUCTOR for direct construction', () => {
    assert.throws(() => new ReadableStreamBYOBRequest(), {
      code: 'ERR_ILLEGAL_CONSTRUCTOR',
    });

    assert.throws(() => new ReadableStreamDefaultController(), {
      code: 'ERR_ILLEGAL_CONSTRUCTOR',
    });

    assert.throws(() => new ReadableByteStreamController(), {
      code: 'ERR_ILLEGAL_CONSTRUCTOR',
    });
  });

});

// =============================================================================
// Test Suite: ReadableStream.from() edge cases
// =============================================================================
describe('ReadableStream.from() edge cases', () => {

  it('should throw for non-iterable', () => {
    assert.throws(() => ReadableStream.from({}), {
      code: 'ERR_ARG_NOT_ITERABLE',
    });
  });

  it('should throw for null', () => {
    assert.throws(() => ReadableStream.from(null));
  });

  it('should throw for undefined', () => {
    assert.throws(() => ReadableStream.from(undefined));
  });

});

// =============================================================================
// Test Suite: getReader mode validation
// =============================================================================
describe('getReader() mode validation', () => {

  it('should throw for invalid mode values', () => {
    const stream = new ReadableStream();

    ['', null, 'asdf'].forEach((mode) => {
      assert.throws(() => stream.getReader({ mode }), {
        code: 'ERR_INVALID_ARG_VALUE',
      });
    });
  });

  it('should throw for invalid options type', () => {
    const stream = new ReadableStream();

    [1, 'asdf'].forEach((options) => {
      assert.throws(() => stream.getReader(options), {
        code: 'ERR_INVALID_ARG_TYPE',
      });
    });
  });

});

// =============================================================================
// Test Suite: Pull behavior
// =============================================================================
describe('Pull behavior', () => {

  it('should call pull when queue is empty and reader reads', async () => {
    let pullCount = 0;

    const stream = new ReadableStream({
      pull(controller) {
        pullCount++;
        if (pullCount <= 3) {
          controller.enqueue(pullCount);
        } else {
          controller.close();
        }
      }
    });

    const reader = stream.getReader();

    // First pull is triggered automatically
    await delay(0);
    assert.strictEqual(pullCount, 1);

    const r1 = await reader.read();
    assert.deepStrictEqual(r1, { value: 1, done: false });
  });

  it('should not call pull when queue has enough items', async () => {
    let pullCount = 0;

    const stream = new ReadableStream({
      start(controller) {
        controller.enqueue('a');
        controller.enqueue('b');
        controller.close();
      },
      pull() {
        pullCount++;
      }
    });

    await delay(0);
    // pull should not be called since we already have items and closed
    assert.strictEqual(pullCount, 0);
  });

  it('should call pull based on highWaterMark', async () => {
    let pullCount = 0;

    const stream = new ReadableStream({
      pull(controller) {
        pullCount++;
        controller.enqueue(pullCount);
      }
    }, { highWaterMark: 4 });

    await delay(10);
    // Should fill up to highWaterMark
    assert.strictEqual(pullCount, 4);
  });

  it('should wait for pull promise to resolve before calling again', async () => {
    let pullCount = 0;
    let resolvePull;

    const stream = new ReadableStream({
      async pull(controller) {
        pullCount++;
        if (pullCount === 1) {
          await new Promise(r => resolvePull = r);
        }
        controller.enqueue(pullCount);
      }
    }, { highWaterMark: 3 }); // higher highWaterMark to trigger multiple pulls

    const reader = stream.getReader();

    // Read to trigger pull
    reader.read();
    await delay(10);
    assert.strictEqual(pullCount, 1);

    // Resolve first pull and wait
    resolvePull();
    await delay(20);
    // After resolve, pull should be called again due to highWaterMark
    assert.ok(pullCount >= 2, `Expected pullCount >= 2, got ${pullCount}`);

    reader.releaseLock();
  });

});

// =============================================================================
// Test Suite: Cancel behavior
// =============================================================================
describe('Cancel behavior', () => {

  it('should pass cancel reason to cancel callback', async () => {
    let receivedReason;

    const stream = new ReadableStream({
      cancel(reason) {
        receivedReason = reason;
      }
    });

    const reason = { custom: 'reason' };
    await stream.cancel(reason);

    assert.strictEqual(receivedReason, reason);
  });

  it('should resolve cancel even after stream is closed', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.close();
      }
    });

    // Should resolve without error
    await stream.cancel();
  });

  it('should resolve cancel multiple times', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.close();
      }
    });

    const [r1, r2] = await Promise.all([
      stream.cancel(),
      stream.cancel(),
    ]);

    assert.strictEqual(r1, undefined);
    assert.strictEqual(r2, undefined);
  });

  it('should reject cancel when stream is errored', async () => {
    const error = new Error('test error');

    const stream = new ReadableStream({
      start(controller) {
        controller.error(error);
      }
    });

    await assert.rejects(stream.cancel(), error);
  });

  it('should call cancel when reader cancels', async () => {
    let cancelCalled = false;

    const stream = new ReadableStream({
      cancel() {
        cancelCalled = true;
      }
    });

    const reader = stream.getReader();
    await reader.cancel();

    assert.strictEqual(cancelCalled, true);
  });

});

// =============================================================================
// Test Suite: Reader closed promise
// =============================================================================
describe('Reader closed promise', () => {

  it('should resolve when stream closes', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.close();
      }
    });

    const reader = stream.getReader();
    await reader.closed;
  });

  it('should reject when stream errors', async () => {
    const error = new Error('boom');
    const stream = new ReadableStream({
      start(controller) {
        controller.error(error);
      }
    });

    const reader = stream.getReader();
    await assert.rejects(reader.closed, error);
  });

  it('should reject after releaseLock', async () => {
    const stream = new ReadableStream({
      start(controller) {
        controller.close();
      }
    });

    const reader = stream.getReader();
    // Wait for closed promise to resolve
    await reader.closed;

    reader.releaseLock();

    // After releaseLock on a closed stream, reader.closed is rejected
    await assert.rejects(reader.closed, {
      name: 'TypeError'
    });
  });

  it('should get new rejected promise after releaseLock on readable stream', async () => {
    const stream = new ReadableStream();

    const reader = stream.getReader();
    reader.releaseLock();

    // After releaseLock, accessing reader.closed should reject
    await assert.rejects(reader.closed, {
      name: 'TypeError',
    });
  });

});

// =============================================================================
// Test Suite: Queuing Strategy details
// =============================================================================
describe('Queuing Strategy details', () => {

  it('should use ByteLengthQueuingStrategy correctly', () => {
    const strategy = new ByteLengthQueuingStrategy({ highWaterMark: 1024 });

    assert.strictEqual(strategy.highWaterMark, 1024);
    assert.strictEqual(strategy.size(new ArrayBuffer(100)), 100);
    assert.strictEqual(strategy.size(new Uint8Array(50)), 50);
  });

  it('should use CountQueuingStrategy correctly', () => {
    const strategy = new CountQueuingStrategy({ highWaterMark: 5 });

    assert.strictEqual(strategy.highWaterMark, 5);
    assert.strictEqual(strategy.size('anything'), 1);
    assert.strictEqual(strategy.size({ large: 'object' }), 1);
  });

  it('should allow extracted size function to work', () => {
    const strategy = new ByteLengthQueuingStrategy({ highWaterMark: 1 });
    const { size } = strategy;

    // size should work even when extracted
    assert.strictEqual(size(new ArrayBuffer(10)), 10);
  });

  it('should handle negative desiredSize', () => {
    let controller;
    new ReadableStream({
      start(c) { controller = c; }
    }, { highWaterMark: 1 });

    controller.enqueue('a');
    assert.strictEqual(controller.desiredSize, 0);

    controller.enqueue('b');
    assert.strictEqual(controller.desiredSize, -1);

    controller.enqueue('c');
    assert.strictEqual(controller.desiredSize, -2);
  });

});

// =============================================================================
// Test Suite: BYOB Reader edge cases
// =============================================================================
describe('BYOB Reader edge cases', () => {

  it('should reject read with zero-length view', async () => {
    const stream = new ReadableStream({
      type: 'bytes',
      start(controller) {
        controller.enqueue(new Uint8Array([1, 2, 3]));
      }
    });

    const reader = stream.getReader({ mode: 'byob' });
    const view = new Uint8Array(0);

    await assert.rejects(reader.read(view), {
      code: 'ERR_INVALID_STATE',
    });
  });

  it('should reject read with detached buffer view', async () => {
    const stream = new ReadableStream({
      type: 'bytes',
      start(controller) {
        controller.enqueue(new Uint8Array([1, 2, 3]));
      }
    });

    const reader = stream.getReader({ mode: 'byob' });
    const buffer = new ArrayBuffer(10);
    const view = new Uint8Array(buffer);

    // Detach the buffer by transferring it
    structuredClone(buffer, { transfer: [buffer] });

    await assert.rejects(reader.read(view), {
      code: 'ERR_INVALID_STATE',
    });
  });

  it('should support read with options.min', async () => {
    const stream = new ReadableStream({
      type: 'bytes',
      pull(controller) {
        controller.enqueue(new Uint8Array([1, 2, 3, 4, 5]));
      }
    });

    const reader = stream.getReader({ mode: 'byob' });
    const buffer = new Uint8Array(10);

    const result = await reader.read(buffer, { min: 3 });

    assert.strictEqual(result.done, false);
    assert.ok(result.value.length >= 3);
  });

  it('should throw for invalid min option', async () => {
    const stream = new ReadableStream({ type: 'bytes' });
    const reader = stream.getReader({ mode: 'byob' });

    await assert.rejects(reader.read(new Uint8Array(10), { min: 0 }), {
      code: 'ERR_INVALID_ARG_VALUE',
    });

    await assert.rejects(reader.read(new Uint8Array(10), { min: -1 }), {
      code: 'ERR_INVALID_ARG_VALUE',
    });

    await assert.rejects(reader.read(new Uint8Array(10), { min: 1.5 }), {
      code: 'ERR_INVALID_ARG_VALUE',
    });
  });

  it('should throw when min > view.length', async () => {
    const stream = new ReadableStream({ type: 'bytes' });
    const reader = stream.getReader({ mode: 'byob' });

    await assert.rejects(reader.read(new Uint8Array(5), { min: 10 }), {
      code: 'ERR_OUT_OF_RANGE',
    });
  });

  it('should handle releaseLock after reading', async () => {
    const stream = new ReadableStream({ type: 'bytes' });
    const reader = stream.getReader({ mode: 'byob' });

    reader.releaseLock();
    reader.releaseLock(); // Should not throw

    await assert.rejects(reader.read(new Uint8Array(10)), {
      code: 'ERR_INVALID_STATE',
    });
  });

});

// =============================================================================
// Test Suite: ByteStreamController enqueue validation
// =============================================================================
describe('ByteStreamController enqueue validation', () => {

  it('should throw for non-ArrayBufferView enqueue', () => {
    let controller;
    new ReadableStream({
      type: 'bytes',
      start(c) { controller = c; }
    });

    assert.throws(() => controller.enqueue(1), {
      code: 'ERR_INVALID_ARG_TYPE',
    });

    assert.throws(() => controller.enqueue('string'), {
      code: 'ERR_INVALID_ARG_TYPE',
    });
  });

  it('should throw enqueue after close', () => {
    let controller;
    new ReadableStream({
      type: 'bytes',
      start(c) { controller = c; }
    });

    controller.close();

    assert.throws(() => controller.enqueue(new Uint8Array(10)), {
      code: 'ERR_INVALID_STATE',
    });
  });

  it('should throw close after close', () => {
    let controller;
    new ReadableStream({
      type: 'bytes',
      start(c) { controller = c; }
    });

    controller.close();

    assert.throws(() => controller.close(), {
      code: 'ERR_INVALID_STATE',
    });
  });

  it('should throw for zero-length chunk', () => {
    let controller;
    new ReadableStream({
      type: 'bytes',
      start(c) { controller = c; }
    });

    assert.throws(() => controller.enqueue(new Uint8Array(0)), {
      code: 'ERR_INVALID_STATE',
    });
  });

});

// Run test
console.log('Running ReadableStream tests...');
