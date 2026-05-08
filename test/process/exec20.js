var path = require('path');
var { Worker } = require('worker_threads');

process.exitCode = 20;
new Worker(path.join(__dirname, 'exec20_worker.js'));