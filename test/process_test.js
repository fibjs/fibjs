var test = require("test");
test.setup();

var process = require('process');

var cmd;
var bs;
var s;

describe('fiber', function() {
	it("execPath", function() {
		cmd = process.execPath + ' exec_test.js';
	});

	it("exec", function() {
		bs = process.exec(cmd);
	});

	it("stdout", function() {
		var sa = bs.readLines();

		assert.equal(sa.length, 5);
		assert.equal(sa[0], "exec testing....");
		assert.equal(sa[2], "console.print....");
		assert.equal(sa[4], "console.print....");
	});

	it("system", function() {
		assert.equal(process.system(cmd), 100);
	});

	it("memoryUsage", function() {
		console.dir(process.memoryUsage());
	});

	it("argv", function() {
		console.dir(process.argv);
	});
});

//test.run();