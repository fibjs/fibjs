// A frozen export object cannot take the `default` binding in place; import()
// wraps it and must hand back the same wrapper on every call.
module.exports = Object.freeze({ frozen: true });
