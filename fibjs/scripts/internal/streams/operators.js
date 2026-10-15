// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

'use strict';

// Node's readable operator helpers (lib/internal/streams/operators.js), the
// methods `readable.map()`, `filter()`, `toArray()`, `reduce()`, ... that work
// off the async iterator. They are installed on Readable.prototype by
// fibjs/scripts/stream.js.
//
// The mapping/filtering operators return a new Readable built on an async
// generator, which is Node's shape too: `readable.map(fn)` is lazy, applies
// backpressure through the generator, and yields objects (objectMode).
//
// Not modelled from Node: the `concurrency` option of the operator options
// (always 1, which is Node's default) and `asIndexedPairs`/`compose`.

const Readable = require('_stream_readable');
const createReadableStreamAsyncIterator = require('internal/streams/async_iterator');

function invalidArgType(name, expected, actual) {
  const err = new TypeError('The "' + name + '" argument must be of type ' + expected +
    '. Received ' + (actual === null ? 'null' : typeof actual));
  err.code = 'ERR_INVALID_ARG_TYPE';
  return err;
}

function validateFunction(fn, name) {
  if (typeof fn !== 'function')
    throw invalidArgType(name, 'function', fn);
}

function validateAbortSignal(signal, name) {
  if (signal === undefined)
    return;

  if (signal === null || typeof signal !== 'object' ||
    typeof signal.aborted !== 'boolean' || typeof signal.addEventListener !== 'function')
    throw invalidArgType(name, 'AbortSignal', signal);
}

function validateInteger(n, name, min, max) {
  if (typeof n !== 'number' || !Number.isInteger(n)) {
    const err = new TypeError('The "' + name + '" argument must be of type number. Received ' +
      (n === null ? 'null' : typeof n));
    err.code = 'ERR_INVALID_ARG_TYPE';
    throw err;
  }

  if (n < min || n > max) {
    const err = new RangeError('The value of "' + name + '" is out of range. It must be >= ' +
      min + ' && <= ' + max + '. Received ' + n);
    err.code = 'ERR_OUT_OF_RANGE';
    throw err;
  }
}

function abortError() {
  const err = new Error('The operation was aborted');
  err.name = 'AbortError';
  err.code = 'ABORT_ERR';
  return err;
}

function throwIfAborted(signal) {
  if (signal && signal.aborted)
    throw abortError();
}

function createIterable(iter) {
  return Readable.from(iter, { objectMode: true });
}

// ---------------------------------------------------------------------------
// async generators behind the operators
// ---------------------------------------------------------------------------

async function* streamMap(stream, fn, signal) {
  for await (const chunk of stream) {
    throwIfAborted(signal);
    yield await fn(chunk, { signal: signal });
  }
}

async function* streamFilter(stream, fn, signal) {
  for await (const chunk of stream) {
    throwIfAborted(signal);
    if (await fn(chunk, { signal: signal }))
      yield chunk;
  }
}

async function* streamFlatMap(stream, fn, signal) {
  for await (const chunk of stream) {
    throwIfAborted(signal);
    const mapped = await fn(chunk, { signal: signal });

    if (mapped == null)
      continue;

    for await (const value of mapped)
      yield value;
  }
}

async function* streamDrop(stream, n, signal) {
  let dropped = 0;

  for await (const chunk of stream) {
    throwIfAborted(signal);

    if (dropped++ < n)
      continue;

    yield chunk;
  }
}

async function* streamTake(stream, n, signal) {
  if (n === 0)
    return;

  let taken = 0;

  for await (const chunk of stream) {
    throwIfAborted(signal);
    yield chunk;

    if (++taken >= n)
      return;
  }
}

// ---------------------------------------------------------------------------
// the operator methods
// ---------------------------------------------------------------------------

function map(fn, options) {
  validateFunction(fn, 'fn');
  validateAbortSignal(options && options.signal, 'options.signal');

  return createIterable(streamMap(this, fn, options && options.signal));
}

function filter(fn, options) {
  validateFunction(fn, 'fn');
  validateAbortSignal(options && options.signal, 'options.signal');

  return createIterable(streamFilter(this, fn, options && options.signal));
}

function flatMap(fn, options) {
  validateFunction(fn, 'fn');
  validateAbortSignal(options && options.signal, 'options.signal');

  return createIterable(streamFlatMap(this, fn, options && options.signal));
}

function drop(n, options) {
  validateInteger(n, 'n', 0, Number.MAX_SAFE_INTEGER);
  validateAbortSignal(options && options.signal, 'options.signal');

  return createIterable(streamDrop(this, n, options && options.signal));
}

function take(n, options) {
  validateInteger(n, 'n', 0, 0xFFFFFFFF);
  validateAbortSignal(options && options.signal, 'options.signal');

  return createIterable(streamTake(this, n, options && options.signal));
}

async function toArray(options) {
  validateAbortSignal(options && options.signal, 'options.signal');

  const signal = options && options.signal;
  const result = [];

  for await (const chunk of this) {
    throwIfAborted(signal);
    result.push(chunk);
  }

  return result;
}

async function forEach(fn, options) {
  validateFunction(fn, 'fn');
  validateAbortSignal(options && options.signal, 'options.signal');

  const signal = options && options.signal;

  for await (const chunk of this) {
    throwIfAborted(signal);
    await fn(chunk, { signal: signal });
  }
}

async function some(fn, options) {
  validateFunction(fn, 'fn');
  validateAbortSignal(options && options.signal, 'options.signal');

  const signal = options && options.signal;

  for await (const chunk of this) {
    throwIfAborted(signal);
    if (await fn(chunk, { signal: signal }))
      return true;
  }

  return false;
}

async function every(fn, options) {
  validateFunction(fn, 'fn');
  validateAbortSignal(options && options.signal, 'options.signal');

  const signal = options && options.signal;

  for await (const chunk of this) {
    throwIfAborted(signal);
    if (!await fn(chunk, { signal: signal }))
      return false;
  }

  return true;
}

async function find(fn, options) {
  validateFunction(fn, 'fn');
  validateAbortSignal(options && options.signal, 'options.signal');

  const signal = options && options.signal;

  for await (const chunk of this) {
    throwIfAborted(signal);
    if (await fn(chunk, { signal: signal }))
      return chunk;
  }

  return undefined;
}

async function reduce(fn, initial) {
  validateFunction(fn, 'fn');

  let accumulator = initial;
  let started = arguments.length > 1;

  for await (const chunk of this) {
    if (!started) {
      accumulator = chunk;
      started = true;
      continue;
    }

    accumulator = await fn(accumulator, chunk);
  }

  if (!started) {
    const err = new TypeError('Reduce of an empty stream requires an initial value');
    err.code = 'ERR_MISSING_ARGS';
    throw err;
  }

  return accumulator;
}

function iterator(options) {
  return createReadableStreamAsyncIterator(this, options);
}

module.exports = {
  map,
  filter,
  flatMap,
  drop,
  take,
  toArray,
  forEach,
  some,
  every,
  find,
  reduce,
  iterator
};
