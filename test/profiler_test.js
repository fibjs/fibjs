var test = require("test");
test.setup();

var profiler = require("profiler");
var fs = require("fs");
var os = require("os");

var vmid = require('coroutine').vmid;

function unlink(pathname) {
	try {
		fs.unlink(pathname);
	} catch (e) {}
}

describe("profiler", function() {
	after(function() {
		unlink("test.heapsnapshot" + vmid);
		unlink("test1.heapsnapshot" + vmid);
		unlink("test2.heapsnapshot" + vmid);
	});

	it("take snapshot & dispose", function() {
		var ss = profiler.takeSnapshot();
		ss.dispose();
	});

	it("crash when double take", function() {
		profiler.takeSnapshot();
		profiler.takeSnapshot();
	});

	it("disable change on proxy", function() {
		var ss = profiler.takeSnapshot();
		assert.throws(function() {
			ss.nodes[0] = 100;
		});

		assert.throws(function() {
			ss.nodes[0].childs[0] = 100;
		});
		ss.dispose();
	});

	it("serialize", function() {
		profiler.saveSnapshot("test.heapsnapshot" + vmid);

		var ss = profiler.loadSnapshot("test.heapsnapshot" + vmid);
		ss.save("test1.heapsnapshot" + vmid);

		var ss = profiler.loadSnapshot("test1.heapsnapshot" + vmid);
		ss.save("test2.heapsnapshot" + vmid);

		assert.equal(fs.readTextFile("test1.heapsnapshot" + vmid),
			fs.readTextFile("test2.heapsnapshot" + vmid));
	});

	it("disable change", function() {
		var ss = profiler.loadSnapshot("test.heapsnapshot" + vmid);
		assert.throws(function() {
			ss.nodes[0] = 100;
		});

		assert.throws(function() {
			ss.nodes[0].childs[0] = 100;
		});
		ss.dispose();
	});

	it("diff", function() {
		var ss = profiler.loadSnapshot("test.heapsnapshot" + vmid);
		var ss1 = profiler.loadSnapshot("test.heapsnapshot" + vmid);

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