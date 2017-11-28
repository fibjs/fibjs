var coroutine = require('coroutine');
var path = require('path');

process.exitCode = 20;
new coroutine.Worker(path.join(__dirname, 'exec20_worker.js'));