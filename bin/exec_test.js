console.log('exec testing....');

var coroutine = require('coroutine');

for(i = 0; i < 10; i ++)
{
	console.log(new Date());
	console.out.writeLine('console.out.writeLine()');
	coroutine.sleep(1000);
}
