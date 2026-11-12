'use strict';

// The default highWaterMark of a newly created stream, in one place because
// three modules need it: Readable and Writable read it when they are
// constructed, and stream.getDefaultHighWaterMark/setDefaultHighWaterMark read
// and write it. Keeping it here instead of in stream.js avoids a cycle, since
// _stream_readable already requires stream.
//
// The values are the runtime's own defaults - 16384 bytes and 16 objects - not
// Node's 65536, because getDefaultHighWaterMark has to describe the streams
// this runtime actually creates.

exports.bytes = 16 * 1024;
exports.objectMode = 16;

exports.get = function get(objectMode) {
    return objectMode ? exports.objectMode : exports.bytes;
};

exports.set = function set(objectMode, hwm) {
    if (objectMode)
        exports.objectMode = hwm;
    else
        exports.bytes = hwm;
};
