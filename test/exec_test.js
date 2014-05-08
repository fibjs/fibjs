console.log('exec testing....');

var coroutine = require('coroutine');
var process = require('process');

for (i = 0; i < 2; i++) {
	console.log(new Date());
	console.print('console.print....');
	console.log();
	coroutine.sleep(100);
}

process.exit(100);
console.log('not output');