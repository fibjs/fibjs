var process = require('process');
var json = require('json');

console.log(json.encode(process.execArgv));
process.exitCode = 3;