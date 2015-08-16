var test = require("test");
test.setup();

var profiler = require("profiler");
var fs = require("fs");
var os = require("os");

function unlink(pathname) {
	try {
		fs.unlink(pathname);
	} catch (e) {}
}

describe("profiler", function() {
	after(function() {
		unlink("test.heapsnapshot");
		unlink("test1.heapsnapshot");
	});

	it("take snapshot & dispose", function() {
		var ss = profiler.takeSnapshot();
		ss.dispose();
	});

	it("serialize", function() {
		profiler.saveSnapshot("test.heapsnapshot");

		var ss = profiler.loadSnapshot("test.heapsnapshot");
		ss.save("test1.heapsnapshot");

		assert.equal(fs.readFile("test.heapsnapshot"),
			fs.readFile("test1.heapsnapshot"));
	});

	it("crash when double take", function() {
		profiler.takeSnapshot();
		profiler.takeSnapshot();
	});

	it("diff", function() {
		var ss = profiler.loadSnapshot("test.heapsnapshot");
		var ss1 = profiler.loadSnapshot("test.heapsnapshot");

		assert.deepEqual(ss.diff(ss1).change, {
			"size_bytes": 0,
			"size": "0 bytes",
			"freed_nodes": 0,
			"allocated_nodes": 0,
			"details": []
		});
	});
});

//test.run(console.DEBUG);