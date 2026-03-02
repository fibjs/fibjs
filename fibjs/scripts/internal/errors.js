'use strict';

// Minimal internal/errors shim for readline module compatibility

const primordials = require('internal/primordials');
const {
  ObjectDefineProperty,
  ObjectSetPrototypeOf,
} = primordials;

function getMessage(key, args) {
  const messages = {
    ERR_INVALID_ARG_VALUE: (name, value, reason = 'is invalid') => {
      let inspected;
      try {
        inspected = JSON.stringify(value);
      } catch {
        inspected = String(value);
      }
      if (inspected.length > 128) {
        inspected = `${inspected.slice(0, 128)}...`;
      }
      return `The argument '${name}' ${reason}. Received ${inspected}`;
    },
    ERR_INVALID_ARG_TYPE: (name, expected, actual) => {
      return `The "${name}" argument must be of type ${expected}. Received type ${typeof actual}`;
    },
    ERR_USE_AFTER_CLOSE: () => {
      return 'This socket has been ended by the other party';
    },
    ERR_INVALID_CURSOR_POS: () => {
      return 'Cannot set cursor row without setting its column';
    },
  };

  const fn = messages[key];
  if (fn) return fn(...args);
  return `${key}`;
}

function makeNodeErrorWithCode(Base, key) {
  return function NodeError(...args) {
    const error = new Base(getMessage(key, args));
    error.code = key;
    ObjectDefineProperty(error, 'name', {
      value: `${Base.name} [${key}]`,
      writable: true,
      enumerable: false,
      configurable: true,
    });
    return error;
  };
}

const codes = {
  ERR_INVALID_ARG_VALUE: makeNodeErrorWithCode(TypeError, 'ERR_INVALID_ARG_VALUE'),
  ERR_INVALID_ARG_TYPE: makeNodeErrorWithCode(TypeError, 'ERR_INVALID_ARG_TYPE'),
  ERR_USE_AFTER_CLOSE: makeNodeErrorWithCode(Error, 'ERR_USE_AFTER_CLOSE'),
  ERR_INVALID_CURSOR_POS: makeNodeErrorWithCode(Error, 'ERR_INVALID_CURSOR_POS'),
};

class AbortError extends Error {
  constructor(message = 'The operation was aborted', options = undefined) {
    if (options !== undefined && typeof options !== 'object') {
      throw codes.ERR_INVALID_ARG_TYPE('options', 'Object', options);
    }
    super(message, options);
    this.code = 'ABORT_ERR';
    this.name = 'AbortError';
  }
}

ObjectSetPrototypeOf(AbortError.prototype, Error.prototype);

module.exports = {
  AbortError,
  codes,
};
