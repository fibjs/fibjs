'use strict';

// Minimal internal/util shim for readline module compatibility

const primordials = require('internal/primordials');
const {
  ObjectFreeze,
  ObjectCreate,
} = primordials;

const kEmptyObject = ObjectFreeze(ObjectCreate(null));

function promisify(original) {
  if (typeof original !== 'function') {
    throw new TypeError('The "original" argument must be of type Function');
  }

  function fn(...args) {
    return new Promise((resolve, reject) => {
      original.call(this, ...args, (err, ...values) => {
        if (err) {
          reject(err);
        } else if (values.length === 1) {
          resolve(values[0]);
        } else {
          resolve(values);
        }
      });
    });
  }

  Object.setPrototypeOf(fn, Object.getPrototypeOf(original));
  Object.defineProperty(fn, 'name', { value: original.name });

  return fn;
}

module.exports = {
  kEmptyObject,
  promisify,
};
