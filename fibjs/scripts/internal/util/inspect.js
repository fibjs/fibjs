'use strict';

// Minimal internal/util/inspect shim for readline module compatibility
// Delegates to fibjs native util module

const util = require('util');

// Wrap getStringWidth to accept optional second parameter (stripVTControlCharacters flag)
// Node.js signature: getStringWidth(str, removeControlChars)
// fibjs signature: getStringWidth(str) - already handles VT control chars internally
function getStringWidth(str, removeControlChars) {
  if (removeControlChars === false) {
    // Don't strip VT control characters - strip them first, then measure
    // Actually Node.js: false means DON'T strip before measuring
    // fibjs always strips internally, so we need to not strip
    // But fibjs getStringWidth already handles escape sequences
  }
  return util.getStringWidth(str);
}

module.exports = {
  inspect: util.inspect,
  getStringWidth,
  stripVTControlCharacters: util.stripVTControlCharacters,
};
