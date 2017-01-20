var coroutine = require('coroutine')

setTimeout(() => {
    coroutine.start(() => {
        console.log(600);
    });
}, 10);
