
console.log('os testing....');

var assert = require('assert');
var process = require('process');

var cmd = process.execPath + ' exec_test';

var bs = process.exec(cmd);
var s;

while ((s = bs.readLine()) != null)
	console.log('>>>', s);

assert.equal(process.system(cmd), 100);
console.dir(process.memoryUsage());
console.dir(process.argv);