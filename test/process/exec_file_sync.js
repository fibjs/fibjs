// Test file for execFileSync with arguments
var process = require('process');
var json = require('json');

console.log("execFileSync with args: " + json.encode(process.argv.slice(2)));
process.exitCode = 0;
