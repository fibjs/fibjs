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

const {
  Buffer
} = require('buffer');

// Note: export Stream before Readable/Writable/Duplex/...
// to avoid a cross-reference(require) issues
const Stream = module.exports = require('internal/streams/legacy');

Stream.Readable = require('_stream_readable');
Stream.Writable = require('_stream_writable');
Stream.Duplex = require('_stream_duplex');
Stream.Transform = require('_stream_transform');
Stream.PassThrough = require('_stream_passthrough');

// The async iteration surface of a readable: `for await (const chunk of r)`,
// `r.iterator({ destroyOnReturn })`, and the operator helpers (`map`, `filter`,
// `toArray`, `reduce`, ...) Node 17+ installs on the prototype. They live in
// internal/streams/{async_iterator,operators}.js; this is where they are wired,
// so that requiring `stream` (or `stream/promises`, which pulls the same
// Readable) is enough to have them - exactly like Node's lib/stream.js.
Stream.Readable.prototype[Symbol.asyncIterator] = function () {
    return require('internal/streams/async_iterator')(this);
};

Object.assign(Stream.Readable.prototype, require('internal/streams/operators'));

// Callback-based pipeline
Stream.pipeline = function pipeline() {
    var streams = Array.prototype.slice.call(arguments);
    var callback = streams.pop();
    if (typeof callback !== 'function') {
        streams.push(callback);
        callback = null;
    }
    var p = require('stream/promises').pipeline.apply(null, streams);
    if (callback) {
        p.then(function () { callback(null); }, callback);
        return streams[0];
    }
    return p;
};

// Callback-based finished
Stream.finished = function finished(stream, opts, callback) {
    if (typeof opts === 'function') { callback = opts; opts = {}; }
    var p = require('stream/promises').finished(stream, opts);
    if (callback) {
        p.then(function () { callback(null); }, callback);
        return function () {};
    }
    return p;
};

// The stream predicates Node exposes on the module object. They are read by the
// stream utility packages (end-of-stream, is-stream, @fastify/*, ...) as a first
// probe, and without them those packages fall back to checking for a `pipe`
// method, which any object can have.
//
// A C++ stream (`fs.createReadStream`, `process.stdout`, a socket) carries the
// `readable`/`writable` booleans but no `_readableState.destroyed`, while a JS
// compatibility-layer stream carries both. So the booleans come first and the
// state bits refine them, which makes one test correct for either shape.
//
// null means "not a stream of that kind" - Node uses it to tell "this is not
// one" apart from "this one is finished", and only isReadable/isWritable/
// isDestroyed answer it; isDisturbed and isErrored always return a boolean.
// A Readable has no `writable` flag at all, so isWritable answers null for it
// rather than false - the same distinction Node makes.
function isReadableStream(stream) {
    return stream !== null && typeof stream === 'object' &&
        typeof stream.readable === 'boolean';
}

function isWritableStream(stream) {
    return stream !== null && typeof stream === 'object' &&
        typeof stream.writable === 'boolean';
}

Stream.isReadable = function isReadable(stream) {
    if (!isReadableStream(stream)) return null;
    if (stream.readable !== true) return false;
    if (stream._readableState &&
        (stream._readableState.destroyed || stream._readableState.ended)) return false;
    return true;
};

Stream.isWritable = function isWritable(stream) {
    if (!isWritableStream(stream)) return null;
    if (stream.writable !== true) return false;
    if (stream._writableState &&
        (stream._writableState.destroyed || stream._writableState.ended)) return false;
    return true;
};

Stream.isDestroyed = function isDestroyed(stream) {
    if (!isReadableStream(stream) && !isWritableStream(stream)) return null;
    // A native stream has no `destroyed` field at all, but `destroy()` clears
    // its `readable`/`writable` flags, so the state is still observable.
    if (typeof stream.destroyed === 'boolean') return stream.destroyed;
    return stream.readable !== true && stream.writable !== true;
};

Stream.isDisturbed = function isDisturbed(stream) {
    if (stream == null) return false;
    var state = stream._readableState;
    return !!(state && state.dataEmitted);
};

Stream.isErrored = function isErrored(stream) {
    if (stream == null) return false;
    var state = stream._readableState || stream._writableState;
    if (!state) return false;
    // `errored` is the state field of the JS layer; a native stream reports the
    // same thing through the readable/writable flags after it is destroyed.
    return !!(state.errored || state.error);
};

// The defaults are the runtime's own: 16384 bytes and 16 objects, which is what
// a new stream is created with. Reporting Node's 65536 would make the function
// disagree with the streams it describes. setDefaultHighWaterMark only affects
// streams created afterwards, so a stream that already exists keeps its value.
var hwm = require('internal/streams/hwm');

Stream.getDefaultHighWaterMark = function getDefaultHighWaterMark(objectMode) {
    return hwm.get(objectMode);
};

Stream.setDefaultHighWaterMark = function setDefaultHighWaterMark(objectMode, value) {
    if (typeof value !== 'number' || !(value >= 0))
        throw new TypeError('The "hwm" argument must be a non-negative number');

    hwm.set(objectMode, value);
};

Stream.promises = require('stream/promises');

// Node's `stream.addAbortSignal(signal, stream)`: aborting the signal destroys
// the stream - at once when it is already aborted. The returned stream is the
// one given, so the call chains. Also installed as the method
// `stream.addAbortSignal(signal)` (Node has both).
//
// fibjs streams are the compatibility classes here, which carry Node's
// `destroy()`; the signal is the standard AbortSignal, so `addEventListener`
// is what subscribes. `destroy(reason)` is the Node behaviour of reporting the
// abort reason to the 'error' listener.
function addAbortSignal(signal, stream) {
    if (signal == null || typeof signal.addEventListener !== 'function')
        throw new TypeError('The "signal" argument must be an AbortSignal.');

    if (stream == null || typeof stream.destroy !== 'function')
        throw new TypeError('The "stream" argument must be a stream with a destroy method.');

    const onAbort = () => {
        stream.destroy(signal.reason);
    };

    if (signal.aborted) {
        // defer, so the caller gets the return value before the destruction
        // (and so a synchronous destroy cannot unwind the caller's stack)
        process.nextTick(onAbort);
    } else {
        signal.addEventListener('abort', onAbort, { once: true });
    }

    return stream;
}

Stream.addAbortSignal = addAbortSignal;

[Stream.Readable, Stream.Writable, Stream.Duplex].forEach(function (ctor) {
    if (ctor && ctor.prototype && typeof ctor.prototype.addAbortSignal !== 'function') {
        ctor.prototype.addAbortSignal = function (signal) {
            return addAbortSignal(signal, this);
        };
    }
});

// Backwards-compat with node 0.4.x
Stream.Stream = Stream;

// Internal utilities
try {
  Stream._isUint8Array = require('internal/util/types').isUint8Array;
} catch (e) {
  // Throws for code outside of Node.js core.

  try {
    Stream._isUint8Array = process.binding('util').isUint8Array;
  } catch (e) {
    // This throws for Node < 4.2.0 because there’s no util binding and
    // returns undefined for Node < 7.4.0.
  }
}

if (!Stream._isUint8Array) {
  Stream._isUint8Array = function _isUint8Array(obj) {
    return Object.prototype.toString.call(obj) === '[object Uint8Array]' || Buffer.isBuffer(obj);
  };
}

const version = process.version.substr(1).split('.');
if (version[0] === 0 && version[1] < 12) {
  Stream._uint8ArrayToBuffer = Buffer;
} else {
  try {
    const internalBuffer = require('internal/buffer');
    Stream._uint8ArrayToBuffer = function _uint8ArrayToBuffer(chunk) {
      return new internalBuffer.FastBuffer(chunk.buffer,
        chunk.byteOffset,
        chunk.byteLength);
    };
  } catch (e) {}

  if (!Stream._uint8ArrayToBuffer) {
    Stream._uint8ArrayToBuffer = function _uint8ArrayToBuffer(chunk) {
      return Buffer.prototype.slice.call(chunk);
    };
  }
}