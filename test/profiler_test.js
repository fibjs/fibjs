var test = require("test");
test.setup();

var profiler = require("profiler");
var fs = require("fs");
var os = require("os");

describe("profiler", function() {
	it("take snapshot & dispose", function() {
		var ss = profiler.takeSnapshot();
		ss.dispose();
	});

	it("serialize", function() {
		var ss = profiler.takeSnapshot();
		assert.equal(ss.serialize, "");

		ss = profiler.takeSnapshot(true);
		assert.notEqual(ss.serialize, "");

		ss.write("test.heapsnapshot");
		assert.equal(ss.serialize, fs.readFile("test.heapsnapshot"));
		fs.unlink("test.heapsnapshot");
	});
});

//test.run(console.DEBUG);