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

describe("profiler", () => {
    after(() => {
        unlink(__dirname + "/test.heapsnapshot" + vmid);
        unlink(__dirname + "/test1.heapsnapshot" + vmid);
        unlink(__dirname + "/test2.heapsnapshot" + vmid);
    });

    it("take snapshot & dispose", () => {
        var ss = profiler.takeSnapshot();
        ss.dispose();
    });

    it("crash when double take", () => {
        profiler.takeSnapshot();
        profiler.takeSnapshot();
    });

    it("disable change on proxy", () => {
        var ss = profiler.takeSnapshot();
        assert.throws(() => {
            ss.nodes[0] = 100;
        });

        assert.throws(() => {
            ss.nodes[0].childs[0] = 100;
        });
        ss.dispose();
    });

    it("serialize", () => {
        profiler.saveSnapshot(__dirname + "/test.heapsnapshot" + vmid);

        var ss = profiler.loadSnapshot(__dirname + "/test.heapsnapshot" + vmid);
        ss.save(__dirname + "/test1.heapsnapshot" + vmid);

        var ss = profiler.loadSnapshot(__dirname + "/test1.heapsnapshot" + vmid);
        ss.save(__dirname + "/test2.heapsnapshot" + vmid);

        assert.equal(fs.readTextFile(__dirname + "/test1.heapsnapshot" + vmid),
            fs.readTextFile(__dirname + "/test2.heapsnapshot" + vmid));
    });

    it("disable change", () => {
        var ss = profiler.loadSnapshot(__dirname + "/test.heapsnapshot" + vmid);
        assert.throws(() => {
            ss.nodes[0] = 100;
        });

        assert.throws(() => {
            ss.nodes[0].childs[0] = 100;
        });
        ss.dispose();
    });

    it("diff", () => {
        var ss = profiler.loadSnapshot(__dirname + "/test.heapsnapshot" + vmid);
        var ss1 = profiler.loadSnapshot(__dirname + "/test.heapsnapshot" + vmid);

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