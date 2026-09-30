'use strict';

// Minimal internal/errors shim for readline module compatibility

const DOMException = require('internal/domexception');

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
    ERR_OUT_OF_RANGE: (name, range, actual) => {
      return `The value of "${name}" is out of range. It must be ${range}. Received ${actual}`;
    },
    ERR_UNKNOWN_ENCODING: (encoding) => {
      return `Unknown encoding: ${encoding}`;
    },
    ERR_BUFFER_OUT_OF_BOUNDS: (name) => {
      return name ? `"${name}" is outside of buffer bounds` : 'Attempt to access memory outside buffer bounds';
    },
    ERR_INVALID_STATE: (message = 'Invalid state') => {
      return message;
    },
    ERR_INVALID_THIS: (expected) => {
      return `Value of "this" must be of type ${expected}`;
    },
    ERR_ILLEGAL_CONSTRUCTOR: (message = 'Illegal constructor') => {
      return message;
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
    return createErrorWithCode(Base, key, getMessage(key, args));
  };
}

function createErrorWithCode(Base, key, message) {
  const error = new Base(message);
  error.code = key;
  // Node.js keeps err.name as the class name and renders the code only when the
  // error is stringified: "TypeError [ERR_UNKNOWN_ENCODING]: ...".
  ObjectDefineProperty(error, 'name', {
    value: Base.name,
    writable: true,
    enumerable: false,
    configurable: true,
  });
  ObjectDefineProperty(error, 'toString', {
    value: function () {
      return `${this.name} [${this.code}]: ${this.message}`;
    },
    writable: true,
    enumerable: false,
    configurable: true,
  });
  // The stack was captured when the error was constructed, before the name was
  // rewritten; refresh its head so it reads like Node's.
  if (typeof error.stack === 'string') {
    const lines = error.stack.split('\n');
    lines[0] = `${Base.name} [${key}]: ${error.message}`;
    ObjectDefineProperty(error, 'stack', {
      value: lines.join('\n'),
      writable: true,
      enumerable: false,
      configurable: true,
    });
  }
  return error;
}

function typeErrorWithCode(key, message) {
  return createErrorWithCode(TypeError, key, message);
}

function rangeErrorWithCode(key, message) {
  return createErrorWithCode(RangeError, key, message);
}

const codes = {
  ERR_INVALID_ARG_VALUE: makeNodeErrorWithCode(TypeError, 'ERR_INVALID_ARG_VALUE'),
  ERR_INVALID_ARG_TYPE: makeNodeErrorWithCode(TypeError, 'ERR_INVALID_ARG_TYPE'),
  ERR_OUT_OF_RANGE: makeNodeErrorWithCode(RangeError, 'ERR_OUT_OF_RANGE'),
  ERR_UNKNOWN_ENCODING: makeNodeErrorWithCode(TypeError, 'ERR_UNKNOWN_ENCODING'),
  ERR_BUFFER_OUT_OF_BOUNDS: makeNodeErrorWithCode(RangeError, 'ERR_BUFFER_OUT_OF_BOUNDS'),
  ERR_INVALID_STATE: makeNodeErrorWithCode(TypeError, 'ERR_INVALID_STATE'),
  ERR_INVALID_THIS: makeNodeErrorWithCode(TypeError, 'ERR_INVALID_THIS'),
  ERR_ILLEGAL_CONSTRUCTOR: makeNodeErrorWithCode(TypeError, 'ERR_ILLEGAL_CONSTRUCTOR'),
  ERR_USE_AFTER_CLOSE: makeNodeErrorWithCode(Error, 'ERR_USE_AFTER_CLOSE'),
  ERR_INVALID_CURSOR_POS: makeNodeErrorWithCode(Error, 'ERR_INVALID_CURSOR_POS'),
};

function invalidArgType(name, expected, actual) {
  return codes.ERR_INVALID_ARG_TYPE(name, expected, actual);
}

function invalidArgValue(name, actual, reason = 'is invalid') {
  return codes.ERR_INVALID_ARG_VALUE(name, actual, reason);
}

function outOfRange(name, range, actual) {
  return codes.ERR_OUT_OF_RANGE(name, range, actual);
}

function unknownEncoding(encoding) {
  return codes.ERR_UNKNOWN_ENCODING(encoding);
}

function bufferOutOfBounds(name) {
  return codes.ERR_BUFFER_OUT_OF_BOUNDS(name);
}

function invalidBufferSize(message) {
  return rangeErrorWithCode('ERR_INVALID_BUFFER_SIZE', message);
}

function invalidState(message = 'Invalid state') {
  return codes.ERR_INVALID_STATE(message);
}

function invalidThis(expected) {
  return codes.ERR_INVALID_THIS(expected);
}

function illegalConstructor(message = 'Illegal constructor') {
  return codes.ERR_ILLEGAL_CONSTRUCTOR(message);
}

function getErrorType(name) {
  switch (name) {
    case 'AbortError':
      return globalThis.AbortError || AbortError;
    case 'TimeoutError':
      return globalThis.TimeoutError || TimeoutError;
    case 'DOMException':
      return globalThis.DOMException;
    case 'Error':
      return globalThis.Error || Error;
    case 'TypeError':
      return globalThis.TypeError || TypeError;
    case 'RangeError':
      return globalThis.RangeError || RangeError;
    case 'SyntaxError':
      return globalThis.SyntaxError || SyntaxError;
    case 'ReferenceError':
      return globalThis.ReferenceError || ReferenceError;
    case 'URIError':
      return globalThis.URIError || URIError;
    case 'EvalError':
      return globalThis.EvalError || EvalError;
    default:
      return globalThis[name] || globalThis.Error || Error;
  }
}

function createError(name, message, options) {
  const ErrorCtor = getErrorType(name);

  if (name === 'DOMException')
    return new ErrorCtor(message, options?.name || 'DOMException');

  if (name === 'AbortError' || name === 'TimeoutError')
    return new ErrorCtor(message, options);

  return new ErrorCtor(message);
}

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

class TimeoutError extends Error {
  constructor(message = 'The operation timed out', options = undefined) {
    if (options !== undefined && typeof options !== 'object') {
      throw codes.ERR_INVALID_ARG_TYPE('options', 'Object', options);
    }
    super(message, options);
    this.code = 'TIMEOUT_ERR';
    this.name = 'TimeoutError';
  }
}

ObjectSetPrototypeOf(TimeoutError.prototype, Error.prototype);

const errorTypes = Object.freeze({
  Error,
  TypeError,
  RangeError,
  SyntaxError,
  ReferenceError,
  URIError,
  EvalError,
  DOMException,
  AbortError,
  TimeoutError,
});

module.exports = {
  AbortError,
  TimeoutError,
  errorTypes,
  getErrorType,
  createError,
  invalidArgType,
  invalidArgValue,
  outOfRange,
  unknownEncoding,
  bufferOutOfBounds,
  invalidBufferSize,
  createErrorWithCode,
  typeErrorWithCode,
  rangeErrorWithCode,
  invalidState,
  invalidThis,
  illegalConstructor,
  codes,
};
