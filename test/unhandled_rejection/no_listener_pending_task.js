// Unhandled rejection with no listener while a 5s timer keeps the loop alive:
// fibjs must terminate with code 1 immediately instead of hanging until the
// timer fires (the pre-fix "process hangs" behavior).
setTimeout(() => {}, 5000);
Promise.reject(new Error('boom'));
