var fs = require('fs');
var coroutine = require('coroutine');

var vmid = coroutine.vmid;

fs.writeFile("module/check1" + vmid + ".js", '');

function t() {
	require('module/check1' + vmid);
}
coroutine.start(t).join();
fs.unlink('module/check1' + vmid + '.js');