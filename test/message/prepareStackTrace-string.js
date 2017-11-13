'use strict';

function prepareStackTrace(error, stack) {
  return 'custom Error String!';
}
Error.prepareStackTrace = prepareStackTrace;

throw new Error();
