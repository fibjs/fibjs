'use strict';

const {
  ArrayPrototypeJoin,
  ArrayPrototypePop,
  ArrayPrototypeSlice,
  Error,
  ErrorCaptureStackTrace,
  ObjectAssign,
  ObjectDefineProperty,
  ObjectGetPrototypeOf,
  ObjectPrototypeHasOwnProperty,
  String,
  StringPrototypeRepeat,
  StringPrototypeSlice,
  StringPrototypeSplit,
} = require('internal/primordials.js');

const { isNativeError, inspect, colors } = require('util');
const { myersDiff, printMyersDiff, printSimpleMyersDiff } = require('./myers_diff');

const kReadableOperator = {
  deepStrictEqual: 'Expected values to be strictly deep-equal:',
  strictEqual: 'Expected values to be strictly equal:',
  strictEqualObject: 'Expected "actual" to be reference-equal to "expected":',
  deepEqual: 'Expected values to be loosely deep-equal:',
  notDeepStrictEqual: 'Expected "actual" not to be strictly deep-equal to:',
  notStrictEqual: 'Expected "actual" to be strictly unequal to:',
  notStrictEqualObject:
    'Expected "actual" not to be reference-equal to "expected":',
  notDeepEqual: 'Expected "actual" not to be loosely deep-equal to:',
  notIdentical: 'Values have same structure but are not reference-equal:',
  notDeepEqualUnequal: 'Expected values not to be loosely deep-equal:',
};

const kReadableMessage = {
  fail: 'Failed',
  ok: 'Expected the expression to be truthy',
  notOk: 'Expected the expression to be falsy',
  isTrue: 'Expected the expression to strictly equal true',
  isNotTrue: 'Expected the expression not to strictly equal true',
  isFalse: 'Expected the expression to strictly equal false',
  isNotFalse: 'Expected the expression not to strictly equal false',
  exist: 'Expected the value to exist (non-null and non-undefined)',
  notExist: 'Expected the value to not exist (null or undefined)',
  isNull: 'Expected the value to be null',
  isNotNull: 'Expected the value not to be null',
  isUndefined: 'Expected the value to be undefined',
  isDefined: 'Expected the value not to be undefined',
  isFunction: 'Expected the value to be a function',
  isNotFunction: 'Expected the value not to be a function',
  isObject: 'Expected the value to be an object',
  isNotObject: 'Expected the value not to be an object',
  isArray: 'Expected the value to be an array',
  isNotArray: 'Expected the value not to be an array',
  isString: 'Expected the value to be a string',
  isNotString: 'Expected the value not to be a string',
  isNumber: 'Expected the value to be a number',
  isNotNumber: 'Expected the value not to be a number',
  isBoolean: 'Expected the value to be a boolean',
  isNotBoolean: 'Expected the value not to be a boolean',
  throws: 'Missing expected exception',
  doesNotThrow: 'Got unwanted exception',
  rejects: 'Missing expected rejection',
}

const kOperatorDescription = {
  match: 'to match',
  doesNotMatch: 'not to match',
  closeTo: 'to be close to',
  notCloseTo: 'not to be close to',
  lessThan: 'to be below',
  notLessThan: 'to be at least',
  greaterThan: 'to be above',
  notGreaterThan: 'to be at most',
  property: 'to have property',
  notProperty: 'not to have property',
  deepProperty: 'to have deep property',
  notDeepProperty: 'not to have deep property',
  propertyVal: 'to have property with value',
  propertyNotVal: 'not to have property with value',
  deepPropertyVal: 'to have deep property with value',
  deepPropertyNotVal: 'not to have deep property with value',
};

const kMaxShortStringLength = 12;
const kMaxLongStringLength = 512;

function copyError(source) {
  const target = ObjectAssign(
    { __proto__: ObjectGetPrototypeOf(source) },
    source,
  );
  ObjectDefineProperty(target, 'message', {
    __proto__: null,
    value: source.message,
  });
  if (ObjectPrototypeHasOwnProperty(source, 'cause')) {
    let { cause } = source;

    if (isNativeError(cause)) {
      cause = copyError(cause);
    }

    ObjectDefineProperty(target, 'cause', { __proto__: null, value: cause });
  }
  return target;
}

function inspectValue(val) {
  // The util.inspect default values could be changed. This makes sure the
  // error messages contain the necessary information nevertheless.
  return inspect(val, {
    compact: false,
    depth: 1000,
    maxArrayLength: Infinity,
    // Assert compares only enumerable properties (with a few exceptions).
    showHidden: false,
    // Assert does not detect proxies currently.
    showProxy: false,
    sorted: true,
    // Inspect getters as we also check them when comparing entries.
    getters: true,
    colors: false
  });
}

function getErrorMessage(operator, message) {
  return message || kReadableOperator[operator];
}

function getOpteratorDescription(operator) {
  return kOperatorDescription[operator] || operator;
}

function checkOperator(actual, expected, operator) {
  // In case both values are objects or functions explicitly mark them as not
  // reference equal for the `strictEqual` operator.
  if (
    operator === 'strictEqual' &&
    ((typeof actual === 'object' &&
      actual !== null &&
      typeof expected === 'object' &&
      expected !== null) ||
      (typeof actual === 'function' && typeof expected === 'function'))
  ) {
    operator = 'strictEqualObject';
  }

  return operator;
}

function getColoredMyersDiff(actual, expected) {
  const header = `${colors.green}actual${colors.clear} ${colors.red}expected${colors.clear}`;
  const skipped = false;

  const diff = myersDiff(StringPrototypeSplit(actual, ''), StringPrototypeSplit(expected, ''));
  let message = printSimpleMyersDiff(diff);

  if (skipped) {
    message += '...';
  }

  return { message, header, skipped };
}

function getStackedDiff(actual, expected) {
  const isStringComparison = typeof actual === 'string' && typeof expected === 'string';

  let message = `\n${colors.green}+${colors.clear} ${actual}\n${colors.red}- ${colors.clear}${expected}`;
  const stringsLen = actual.length + expected.length;
  const maxTerminalLength = process.stderr.isTTY ? process.stderr.columns : 80;
  const showIndicator = isStringComparison && (stringsLen <= maxTerminalLength);

  if (showIndicator) {
    let indicatorIdx = -1;

    for (let i = 0; i < actual.length; i++) {
      if (actual[i] !== expected[i]) {
        // Skip the indicator for the first 2 characters because the diff is immediately apparent
        // It is 3 instead of 2 to account for the quotes
        if (i >= 3) {
          indicatorIdx = i;
        }
        break;
      }
    }

    if (indicatorIdx !== -1) {
      message += `\n${StringPrototypeRepeat(' ', indicatorIdx + 2)}^`;
    }
  }

  return { message };
}

function getSimpleDiff(originalActual, actual, originalExpected, expected) {
  let stringsLen = actual.length + expected.length;
  // Accounting for the quotes wrapping strings
  if (typeof originalActual === 'string') {
    stringsLen -= 2;
  }
  if (typeof originalExpected === 'string') {
    stringsLen -= 2;
  }
  if (stringsLen <= kMaxShortStringLength && (originalActual !== 0 || originalExpected !== 0)) {
    return { message: `${actual} !== ${expected}`, header: '' };
  }

  const isStringComparison = typeof originalActual === 'string' && typeof originalExpected === 'string';
  // colored myers diff
  if (isStringComparison && colors.hasColors) {
    return getColoredMyersDiff(actual, expected);
  }

  return getStackedDiff(actual, expected);
}

function isSimpleDiff(actual, inspectedActual, expected, inspectedExpected) {
  if (inspectedActual.length > 1 || inspectedExpected.length > 1) {
    return false;
  }

  return typeof actual !== 'object' || actual === null || typeof expected !== 'object' || expected === null;
}

function filterMatchingProperties(actual, expected) {
  if (!expected || typeof expected !== 'object' || !actual || typeof actual !== 'object') {
    return { actual, expected };
  }

  const filtered = {};
  const actualFiltered = {};

  for (const key in expected) {
    if (key in actual) {
      filtered[key] = expected[key];
      actualFiltered[key] = actual[key];
    }
  }

  return { actual: actualFiltered, expected: filtered };
}

function createErrDiff(actual, expected, operator, customMessage, needsFilter = false) {
  operator = checkOperator(actual, expected, operator);

  // Only filter properties for throws/rejects
  if (needsFilter) {
    const { actual: filteredActual, expected: filteredExpected } = filterMatchingProperties(actual, expected);
    actual = filteredActual;
    expected = filteredExpected;
  }

  let skipped = false;
  let message = '';
  const inspectedActual = inspectValue(actual);
  const inspectedExpected = inspectValue(expected);
  const inspectedSplitActual = StringPrototypeSplit(inspectedActual, '\n');
  const inspectedSplitExpected = StringPrototypeSplit(inspectedExpected, '\n');
  const showSimpleDiff = isSimpleDiff(actual, inspectedSplitActual, expected, inspectedSplitExpected);
  let header = `${colors.green}+ actual${colors.clear} ${colors.red}- expected${colors.clear}`;

  if (showSimpleDiff) {
    const simpleDiff = getSimpleDiff(actual, inspectedSplitActual[0], expected, inspectedSplitExpected[0]);
    message = simpleDiff.message;
    if (typeof simpleDiff.header !== 'undefined') {
      header = simpleDiff.header;
    }
    if (simpleDiff.skipped) {
      skipped = true;
    }
  } else if (inspectedActual === inspectedExpected) {
    // Handles the case where the objects are structurally the same but different references
    operator = 'notIdentical';
    if (inspectedSplitActual.length > 50) {
      message = `${ArrayPrototypeJoin(ArrayPrototypeSlice(inspectedSplitActual, 0, 50), '\n')}\n...}`;
      skipped = true;
    } else {
      message = ArrayPrototypeJoin(inspectedSplitActual, '\n');
    }
    header = '';
  } else {
    const checkCommaDisparity = actual != null && typeof actual === 'object';
    const diff = myersDiff(inspectedSplitActual, inspectedSplitExpected, checkCommaDisparity);

    const myersDiffMessage = printMyersDiff(diff);
    message = myersDiffMessage.message;

    if (myersDiffMessage.skipped) {
      skipped = true;
    }
  }

  const headerMessage = customMessage ?
    `${customMessage}\n${header}` :
    `${getErrorMessage(operator)}\n${header}`;
  const skippedMessage = skipped ? '\n... Skipped lines' : '';

  return `${headerMessage}${skippedMessage}\n${message}\n`;
}

function addEllipsis(string) {
  const lines = StringPrototypeSplit(string, '\n', 11);
  if (lines.length > 10) {
    lines.length = 10;
    return `${ArrayPrototypeJoin(lines, '\n')}\n...`;
  } else if (string.length > kMaxLongStringLength) {
    return `${StringPrototypeSlice(string, kMaxLongStringLength)}...`;
  }
  return string;
}

function getClassName(fn) {
  if (!fn || typeof fn !== 'function') return '';
  return fn.name || '(anonymous class)';
}

class AssertionError extends Error {
  constructor(options) {
    const {
      message,
      operator,
      stackStartFn,
      details,
      // Compatibility with older versions.
      stackStartFunction,
    } = options;
    let {
      actual,
      property,
      expected,
    } = options;

    const limit = Error.stackTraceLimit;

    if (operator === 'throws' || operator === 'rejects') {
      if (expected instanceof RegExp) {
        super(message ? message : `The input did not match the regular expression ${expected}. Input:${colors.clear}\n\n${actual}\n\n`);
      } else if (typeof expected === 'function') {
        if (expected.prototype && expected.prototype.constructor === expected) {
          // This is likely a class
          const expectedClassName = getClassName(expected);
          const actualClassName = actual && actual.constructor ? getClassName(actual.constructor) : 'Unknown';
          super(message ? message : `The error is expected to be an instance of "${expectedClassName}". Received "${actualClassName}"`);
        } else {
          super(message ? message : 'The validation function is expected to return "true". Received false');
        }
      } else if (expected !== undefined) {
        super(createErrDiff(actual, expected, 'deepStrictEqual', message, true));
      } else {
        super(kReadableMessage[operator]);
      }
    } else if (message != null) {
      if (operator === 'deepStrictEqual' || operator === 'strictEqual') {
        super(createErrDiff(actual, expected, operator, message));
      } else {
        super(String(message));
      }
    } else {
      const knownMessage = kReadableMessage[operator];
      if (knownMessage) {
        super(knownMessage);
      } else {
        // Prevent the error stack from being visible by duplicating the error
        // in a very close way to the original in case both sides are actually
        // instances of Error.
        if (typeof actual === 'object' && actual !== null &&
          typeof expected === 'object' && expected !== null &&
          'stack' in actual && actual instanceof Error &&
          'stack' in expected && expected instanceof Error) {
          actual = copyError(actual);
          expected = copyError(expected);
        }

        if (operator === 'deepStrictEqual' || operator === 'strictEqual') {
          super(createErrDiff(actual, expected, operator, message));
        } else if (operator === 'notDeepStrictEqual' ||
          operator === 'notStrictEqual') {
          // In case the objects are equal but the operator requires unequal, show
          // the first object and say A equals B
          let base = kReadableOperator[operator];
          const res = StringPrototypeSplit(inspectValue(actual), '\n');

          // In case "actual" is an object or a function, it should not be
          // reference equal.
          if (operator === 'notStrictEqual' &&
            ((typeof actual === 'object' && actual !== null) ||
              typeof actual === 'function')) {
            base = kReadableOperator.notStrictEqualObject;
          }

          // Only remove lines in case it makes sense to collapse those.
          // TODO: Accept env to always show the full error.
          if (res.length > 50) {
            res[46] = `${colors.blue}...${colors.clear}`;
            while (res.length > 47) {
              ArrayPrototypePop(res);
            }
          }

          // Only print a single input.
          if (res.length === 1) {
            super(`${base}${res[0].length > 5 ? '\n\n' : ' '}${res[0]}`);
          } else {
            super(`${base}\n\n${ArrayPrototypeJoin(res, '\n')}\n`);
          }
        } else {
          let res = inspectValue(actual);
          let other = inspectValue(expected);
          const knownOperator = kReadableOperator[operator];
          if (operator === 'notDeepEqual' && res === other) {
            res = `${knownOperator}\n\n${res}`;
            if (res.length > 1024) {
              res = `${StringPrototypeSlice(res, 0, 1021)}...`;
            }
            super(res);
          } else {
            if (res.length > kMaxLongStringLength) {
              res = `${StringPrototypeSlice(res, 0, 509)}...`;
            }
            if (other.length > kMaxLongStringLength) {
              other = `${StringPrototypeSlice(other, 0, 509)}...`;
            }
            if (operator === 'deepEqual') {
              res = `${knownOperator}\n\n${res}\n\nshould loosely deep-equal\n\n`;
            } else {
              const newOp = kReadableOperator[`${operator}Unequal`];
              if (newOp) {
                res = `${newOp}\n\n${res}\n\nshould not loosely deep-equal\n\n`;
              } else {
                other = ` ${getOpteratorDescription(operator)} ${other}`;
              }
            }
            super(`Expected ${res}${other}`);
          }
        }
      }
    }

    this.generatedMessage = !message;
    ObjectDefineProperty(this, 'name', {
      __proto__: null,
      value: 'AssertionError [ERR_ASSERTION]',
      enumerable: false,
      writable: true,
      configurable: true,
    });
    this.code = 'ERR_ASSERTION';
    if (details) {
      for (let i = 0; i < details.length; i++) {
        this['message ' + i] = details[i].message;
        this['actual ' + i] = details[i].actual;
        if (details[i].expected !== undefined)
          this['expected ' + i] = details[i].expected;
        this['operator ' + i] = details[i].operator;
        this['stack trace ' + i] = details[i].stack;
      }
    } else {
      this.actual = actual;
      if (property !== undefined)
        this.property = property;
      if (expected !== undefined)
        this.expected = expected;
      this.operator = operator;
    }
    // ErrorCaptureStackTrace(this, stackStartFn || stackStartFunction);
    // Create error message including the error code in the name.
    this.stack; // eslint-disable-line no-unused-expressions
    // Reset the name.
    this.name = 'AssertionError';
  }

  toString() {
    return `${this.name} [${this.code}]: ${this.message}`;
  }
}

module.exports = AssertionError;
