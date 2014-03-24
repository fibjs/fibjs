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
		while ((s = bs.readLine()) != null)
			console.log('>>>', s);
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