'use strict';

// Minimal internal/validators shim for readline module compatibility

const primordials = require('internal/primordials');
const {
  ArrayIsArray,
  NumberIsInteger,
  NumberIsNaN,
} = primordials;

const {
  invalidArgType,
  invalidArgValue,
  outOfRange,
} = require('internal/errors');

function hideStackFrames(fn) {
  return fn;
}

const validateAbortSignal = hideStackFrames((signal, name) => {
  if (signal !== undefined &&
      (signal === null ||
       typeof signal !== 'object' ||
       !('aborted' in signal))) {
    throw invalidArgType(name, 'an instance of AbortSignal', signal);
  }
});

const validateArray = hideStackFrames((value, name, minLength = 0) => {
  if (!ArrayIsArray(value)) {
    throw invalidArgType(name, 'an instance of Array', value);
  }
  if (value.length < minLength) {
    throw invalidArgValue(name, value.length, `must have a length of at least ${minLength}`);
  }
});

const validateString = hideStackFrames((value, name) => {
  if (typeof value !== 'string') {
    throw invalidArgType(name, 'string', value);
  }
});

const validateNumber = hideStackFrames((value, name, min = undefined, max) => {
  if (typeof value !== 'number') {
    throw invalidArgType(name, 'number', value);
  }

  if ((min != null && value < min) || (max != null && value > max) ||
      ((min != null || max != null) && NumberIsNaN(value))) {
    throw invalidArgValue(name, value,
      `must be ${min != null ? `>= ${min}` : ''}${min != null && max != null ? ' && ' : ''}${max != null ? `<= ${max}` : ''}`);
  }
});

const validateBoolean = hideStackFrames((value, name) => {
  if (typeof value !== 'boolean') {
    throw invalidArgType(name, 'boolean', value);
  }
});

const validateInteger = hideStackFrames(
  (value, name, min = Number.MIN_SAFE_INTEGER, max = Number.MAX_SAFE_INTEGER) => {
    if (typeof value !== 'number') {
      throw invalidArgType(name, 'number', value);
    }
    if (!NumberIsInteger(value)) {
      throw outOfRange(name, 'an integer', value);
    }
    if (value < min || value > max) {
      throw outOfRange(name, `>= ${min} && <= ${max}`, value);
    }
  }
);

const validateUint32 = hideStackFrames((value, name, positive = false) => {
  if (typeof value !== 'number') {
    throw invalidArgType(name, 'number', value);
  }
  if (!NumberIsInteger(value)) {
    throw outOfRange(name, 'an integer', value);
  }
  const min = positive ? 1 : 0;
  const max = 4294967295; // 2 ** 32 - 1
  if (value < min || value > max) {
    throw outOfRange(name, `>= ${min} && <= ${max}`, value);
  }
});

const validateFunction = hideStackFrames((value, name) => {
  if (typeof value !== 'function') {
    throw invalidArgType(name, 'function', value);
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
