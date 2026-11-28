// The fib-orm bridge shape: a CommonJS module that re-exports an ES module
// namespace. import() must still expose `default` (the exports object) while
// keeping the named bindings.
module.exports = require('./cjs_reexport_named.mjs');
