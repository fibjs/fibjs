'use strict';

// Minimal internal/validators shim for readline module compatibility

const primordials = require('internal/primordials');
const {
  ArrayIsArray,
  NumberIsInteger,
  NumberIsNaN,
} = primordials;

function hideStackFrames(fn) {
  return fn;
}

const validateAbortSignal = hideStackFrames((signal, name) => {
  if (signal !== undefined &&
      (signal === null ||
       typeof signal !== 'object' ||
       !('aborted' in signal))) {
    throw new TypeError(
      `The "${name}" argument must be an instance of AbortSignal. Received ${signal === null ? 'null' : typeof signal}`
    );
  }
});

const validateArray = hideStackFrames((value, name, minLength = 0) => {
  if (!ArrayIsArray(value)) {
    throw new TypeError(
      `The "${name}" argument must be an instance of Array. Received type ${typeof value}`
    );
  }
  if (value.length < minLength) {
    throw new TypeError(
      `The "${name}" argument must have a length of at least ${minLength}. Received ${value.length}`
    );
  }
});

const validateString = hideStackFrames((value, name) => {
  if (typeof value !== 'string') {
    throw new TypeError(
      `The "${name}" argument must be of type string. Received type ${typeof value}`
    );
  }
});

const validateNumber = hideStackFrames((value, name, min = undefined, max) => {
  if (typeof value !== 'number') {
    throw new TypeError(
      `The "${name}" argument must be of type number. Received type ${typeof value}`
    );
  }

  if ((min != null && value < min) || (max != null && value > max) ||
      ((min != null || max != null) && NumberIsNaN(value))) {
    throw new RangeError(
      `The value of "${name}" is out of range. It must be ${min != null ? `>= ${min}` : ''}${min != null && max != null ? ' && ' : ''}${max != null ? `<= ${max}` : ''}. Received ${value}`
    );
  }
});

const validateBoolean = hideStackFrames((value, name) => {
  if (typeof value !== 'boolean') {
    throw new TypeError(
      `The "${name}" argument must be of type boolean. Received type ${typeof value}`
    );
  }
});

const validateInteger = hideStackFrames(
  (value, name, min = Number.MIN_SAFE_INTEGER, max = Number.MAX_SAFE_INTEGER) => {
    if (typeof value !== 'number') {
      throw new TypeError(
        `The "${name}" argument must be of type number. Received type ${typeof value}`
      );
    }
    if (!NumberIsInteger(value)) {
      throw new RangeError(
        `The "${name}" argument must be an integer. Received ${value}`
      );
    }
    if (value < min || value > max) {
      throw new RangeError(
        `The value of "${name}" is out of range. It must be >= ${min} && <= ${max}. Received ${value}`
      );
    }
  }
);

const validateUint32 = hideStackFrames((value, name, positive = false) => {
  if (typeof value !== 'number') {
    throw new TypeError(
      `The "${name}" argument must be of type number. Received type ${typeof value}`
    );
  }
  if (!NumberIsInteger(value)) {
    throw new RangeError(
      `The "${name}" argument must be an integer. Received ${value}`
    );
  }
  const min = positive ? 1 : 0;
  const max = 4294967295; // 2 ** 32 - 1
  if (value < min || value > max) {
    throw new RangeError(
      `The value of "${name}" is out of range. It must be >= ${min} && <= ${max}. Received ${value}`
    );
  }
});

const validateFunction = hideStackFrames((value, name) => {
  if (typeof value !== 'function') {
    throw new TypeError(
      `The "${name}" argument must be of type function. Received type ${typeof value}`
    );
  }
});

module.exports = {
  hideStackFrames,
  validateAbortSignal,
  validateArray,
  validateBoolean,
  validateFunction,
  validateInteger,
  validateNumber,
  validateString,
  validateUint32,
};
