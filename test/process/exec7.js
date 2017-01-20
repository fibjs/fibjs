var coroutine = require('coroutine');

coroutine.start(() => {
    coroutine.sleep(100);
    console.log(100);
});
