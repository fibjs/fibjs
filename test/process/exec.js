console.log('exec testing....');

var coroutine = require('coroutine');

process.on("SIGTERM", () => {
	console.log("killed");
	process.exit(0);
});

for (var i = 0; i < 2; i++) {
	coroutine.sleep(1000);
	console.log(new Date());
	console.print('console.print....');
	console.log();
}

process.exit(100);
console.log('not output');