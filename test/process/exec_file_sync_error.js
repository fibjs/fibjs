// Test file for execFileSync error case
var process = require('process');

console.log("execFileSync stdout");
console.error("execFileSync stderr");
process.exitCode = 42;
