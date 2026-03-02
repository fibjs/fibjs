'use strict';

// Minimal internal/streams/utils shim for readline module compatibility

function isWritable(stream) {
  if (stream == null) return false;
  if (typeof stream.writable === 'boolean') return stream.writable;
  if (typeof stream.write === 'function') return true;
  return false;
}

module.exports = {
  isWritable,
};
