// Fiber throws keep the fibjs "report but do not exit" semantics (path A in
// the D1 analysis). They must NOT be turned into a process exit by the
// unhandledRejection handling.
const coroutine = require('coroutine');

coroutine.start(() => {
    throw new Error('fiber boom');
});

setTimeout(() => {}, 50);
