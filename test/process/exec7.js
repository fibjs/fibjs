var coroutine = require('coroutine');

coroutine.start(function() {
    coroutine.sleep(100);
    process.exit(100);
});
