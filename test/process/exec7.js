var coroutine = require('coroutine');

process.exitCode = 7;

coroutine.start(() => {
    coroutine.sleep(100);
    console.log(100);
});