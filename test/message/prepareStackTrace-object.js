'use strict';

function prepareStackTrace(error, stack) {
  return new Error("custom Error Object!");
}
Error.prepareStackTrace = prepareStackTrace;

throw new Error();
