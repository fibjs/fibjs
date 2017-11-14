var coroutine = require('coroutine');

process.exitCode = 8;
coroutine.sleep(100, () => {
    console.log(200);
});