var coroutine = require('coroutine');

console.log('process started');
coroutine.sleep(100);
console.log('process ending');
process.exit(42);
