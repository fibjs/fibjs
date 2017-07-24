var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');

var vmid = coroutine.vmid;

fs.writeFile(path.join(__dirname, "module", "check1" + vmid + ".js"), '');

function t() {
	require('./module/check1' + vmid);
}
coroutine.start(t).join();
fs.unlink(path.join(__dirname, 'module', 'check1' + vmid + '.js'));