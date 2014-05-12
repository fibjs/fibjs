var fs = require('fs');

fs.writeFile("module/check1.js", '');

function t() {
	require('module/check1');
}

t.start().join();
fs.unlink('module/check1.js');