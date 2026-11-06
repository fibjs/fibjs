// Plain CommonJS package: import() must still expose the exports object as the
// `default` binding, and the namespace must be the same object require() returns
// (builtins behave that way, so a package must not diverge).
module.exports = {
    hello: 'world',
    count: 42
};
