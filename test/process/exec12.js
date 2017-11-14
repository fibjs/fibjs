var coroutine = require('coroutine')

process.exitCode = 12;
setTimeout(() => {
    coroutine.start(() => {
        console.log(600);
    });
}, 10);