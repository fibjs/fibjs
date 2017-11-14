var process = require('process');
var json = require('json');

console.log(json.encode(process.argv));
process.exitCode = 2;