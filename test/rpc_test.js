var test = require("test");
test.setup();

var rpc = require('rpc');
var os = require('os');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

describe("rpc", () => {
    var ss = [];

    after(() => {
        ss.forEach((s) => {
            s.close();
        });
    });

    it("Task", () => {
        assert.throws(() => {
            var task = rpc.open("./not_exists.js");
        });

        assert.throws(() => {
            var task = rpc.open("../not_exists.js");
        });

        assert.throws(() => {
            var task = rpc.open("http://127.0.0.1/not_exists.js");
        });

        var task = rpc.open("/not_exists.js");
        var task1 = task.func;
        var task2 = task['1234'];

        assert.throws(() => {
            task();
        });

        var task = rpc.open("module/c4.js");
        assert.equal(task.foo(), 1);
        assert.equal(task.arg_count(100, 200), 2);
        assert.deepEqual(task.arg_obj({
            a: 100,
            b: 200
        }), {
            a: 100,
            b: 200
        });

        assert.deepEqual(task.arg_obj(new Buffer("1234567")), new Buffer("1234567"));

        var n = 0;
        coroutine.start(() => {
            n = 1;
        });

        task.foo();
        assert.equal(n, 1);
    });

    xit("Garbage Collection", () => {
        GC();

        var no1 = os.memoryUsage().nativeObjects.objects;

        rpc.json({});

        GC();
        assert.equal(no1, os.memoryUsage().nativeObjects.objects);
    });
});

// test.run(console.DEBUG);
