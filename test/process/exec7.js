var coroutine = require('coroutine');

coroutine.start(function() {
    coroutine.sleep(100);
    console.log(100);
});
