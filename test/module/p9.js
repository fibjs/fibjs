const coroutine = require('coroutine');

coroutine.sleep(100);

throw new Error("error in p8.js");

exports.test = 1000;