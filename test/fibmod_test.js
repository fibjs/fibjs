var fs = require('fs');
var coroutine = require('coroutine');

fs.writeFile("module/check1.js", '');

function t() {
	require('module/check1');
}
coroutine.start(t).join();
fs.unlink('module/check1.js');