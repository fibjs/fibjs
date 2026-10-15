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

// Node's Readable async iterator (lib/internal/streams/async_iterator.js), the
// piece that makes `for await (const chunk of readable)` work. Ported from the
// reference implementation so the observable semantics stay Node's:
//
//   - chunks already buffered are handed out before anything waits
//   - the iterator subscribes to 'readable' and pulls with read()
//   - a pending next() is woken by the next chunk, by the end of the stream,
//     by an error, or by the stream being torn down
//   - `return()` destroys the stream (`destroyOnReturn`, Node 16.3+); with
//     `iterator({ destroyOnReturn: false })` it leaves the stream alone
//
// One deliberate difference from current Node (v22+): a stream that is
// destroyed mid-iteration *without* an error ends the loop instead of
// rejecting with ERR_STREAM_PREMATURE_CLOSE. That is the behaviour of the
// reference implementation ported here (`readable-stream@3`, Node <= 20), and
// it is the one the rest of this compatibility layer can support: the reader
// loop of a socket that was destroyed should end, not throw. An error passed
// to destroy() still rejects, because it arrives as an 'error' event.

const kLastResolve = Symbol('lastResolve');
const kLastReject = Symbol('lastReject');
const kError = Symbol('error');
const kEnded = Symbol('ended');
const kLastPromise = Symbol('lastPromise');
const kHandlePromise = Symbol('handlePromise');
const kStream = Symbol('stream');
const kDestroyOnReturn = Symbol('destroyOnReturn');

function createIterResult(value, done) {
  return { value: value, done: done };
}

function readAndResolve(iter) {
  const resolve = iter[kLastResolve];

  if (resolve !== null) {
    const data = iter[kStream].read();
    // we defer if data is null
    // we can be expecting either 'end' or
    // 'error'
    if (data !== null) {
      iter[kLastPromise] = null;
      iter[kLastResolve] = null;
      iter[kLastReject] = null;
      resolve(createIterResult(data, false));
    }
  }
}

function onReadable(iter) {
  // we wait for the next tick, because it might
  // emit an error with process.nextTick
  process.nextTick(readAndResolve, iter);
}

function wrapForNext(lastPromise, iter) {
  return function (resolve, reject) {
    lastPromise.then(function () {
      if (iter[kEnded]) {
        resolve(createIterResult(undefined, true));
        return;
      }

      iter[kHandlePromise](resolve, reject);
    }, reject);
  };
}

// A trimmed end-of-stream for the readable side: the iterator needs to know
// when the stream ended ('end'), when it failed ('error') and when it went
// away without ending ('close', which the destroy path emits).
function onFinished(stream, callback) {
  function cleanup() {
    stream.removeListener('end', onEnd);
    stream.removeListener('error', onError);
    stream.removeListener('close', onClose);
  }

  function onEnd() {
    cleanup();
    callback(null);
  }

  function onError(err) {
    cleanup();
    callback(err);
  }

  function onClose() {
    cleanup();
    const err = new Error('Premature close');
    err.code = 'ERR_STREAM_PREMATURE_CLOSE';
    callback(err);
  }

  stream.on('end', onEnd);
  stream.on('error', onError);
  stream.on('close', onClose);
}

const AsyncIteratorPrototype = Object.getPrototypeOf(Object.getPrototypeOf(async function* () { }).prototype);

const ReadableStreamAsyncIteratorPrototype = Object.setPrototypeOf({
  get stream() {
    return this[kStream];
  },

  next: function next() {
    const iterator = this;

    // if we have detected an error in the meanwhile
    // reject straight away
    const error = iterator[kError];

    if (error !== null) {
      return Promise.reject(error);
    }

    if (iterator[kEnded]) {
      return Promise.resolve(createIterResult(undefined, true));
    }

    if (iterator[kStream].destroyed) {
      // We need to defer via nextTick because if .destroy(err) is
      // called, the error will be emitted via nextTick, and
      // we cannot guarantee that there is no error lingering around
      // waiting to be emitted.
      return new Promise(function (resolve, reject) {
        process.nextTick(function () {
          if (iterator[kError]) {
            reject(iterator[kError]);
          } else {
            resolve(createIterResult(undefined, true));
          }
        });
      });
    }

    // if we have multiple next() calls
    // we will wait for the previous Promise to finish
    // this logic is optimized to support for await loops,
    // where next() is only called once at a time
    const lastPromise = iterator[kLastPromise];
    let promise;

    if (lastPromise) {
      promise = new Promise(wrapForNext(lastPromise, iterator));
    } else {
      // fast path needed to support multiple this.push()
      // without triggering the next() queue
      const data = iterator[kStream].read();

      if (data !== null) {
        return Promise.resolve(createIterResult(data, false));
      }

      promise = new Promise(iterator[kHandlePromise]);
    }

    iterator[kLastPromise] = promise;
    return promise;
  },

  return: function () {
    const iterator = this;

    if (!iterator[kDestroyOnReturn]) {
      return Promise.resolve(createIterResult(undefined, true));
    }

    // destroy(err, cb) is a private API
    // we can guarantee we have that here, because we control the
    // Readable class this is attached to
    return new Promise(function (resolve, reject) {
      iterator[kStream].destroy(null, function (err) {
        if (err) {
          reject(err);
          return;
        }

        resolve(createIterResult(undefined, true));
      });
    });
  },

  [Symbol.asyncIterator]: function () {
    return this;
  }
}, AsyncIteratorPrototype);

/**
 * Creates the async iterator of a Readable.
 *
 * @param {object} stream the readable (or duplex) stream to iterate
 * @param {object} [options] `{ destroyOnReturn = true }`
 * @returns {AsyncIterator} the iterator, itself async iterable
 */
function createReadableStreamAsyncIterator(stream, options) {
  const iterator = Object.create(ReadableStreamAsyncIteratorPrototype);

  iterator[kStream] = stream;
  iterator[kDestroyOnReturn] = !options || options.destroyOnReturn !== false;
  iterator[kLastPromise] = null;
  iterator[kLastResolve] = null;
  iterator[kLastReject] = null;
  iterator[kError] = null;
  iterator[kEnded] = !!(stream._readableState && stream._readableState.endEmitted);
  iterator[kHandlePromise] = function (resolve, reject) {
    const data = iterator[kStream].read();

    if (data) {
      iterator[kLastPromise] = null;
      iterator[kLastResolve] = null;
      iterator[kLastReject] = null;
      resolve(createIterResult(data, false));
    } else {
      iterator[kLastResolve] = resolve;
      iterator[kLastReject] = reject;
    }
  };

  onFinished(stream, function (err) {
    if (err && err.code !== 'ERR_STREAM_PREMATURE_CLOSE') {
      const reject = iterator[kLastReject];

      // reject if we are waiting for data in the Promise
      // returned by next() and store the error
      if (reject !== null) {
        iterator[kLastPromise] = null;
        iterator[kLastResolve] = null;
        iterator[kLastReject] = null;
        reject(err);
      }

      iterator[kError] = err;
      return;
    }

    const resolve = iterator[kLastResolve];

    if (resolve !== null) {
      iterator[kLastPromise] = null;
      iterator[kLastResolve] = null;
      iterator[kLastReject] = null;
      resolve(createIterResult(undefined, true));
    }

    iterator[kEnded] = true;
  });

  stream.on('readable', onReadable.bind(null, iterator));

  return iterator;
}

module.exports = createReadableStreamAsyncIterator;
