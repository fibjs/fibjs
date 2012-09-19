console.log('exec testing....');

var coroutine = require('coroutine');
var os = require('os');

for(i = 0; i < 2; i ++)
{
	console.log(new Date());
	console.stdout.writeLine('console.out.writeLine....');
	coroutine.sleep(100);
}

os.exit(100);
console.log('not output');
