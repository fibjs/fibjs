var coroutine = require('coroutine')

setTimeout(function() {
    coroutine.start(function() {
        console.log(600);
    });
}, 10);
