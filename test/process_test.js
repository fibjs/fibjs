var test = require("test");
test.setup();

var process = require('process');
var coroutine = require("coroutine");
var json = require('json');

var cmd;
var s;

describe('process', function() {
    it("execPath", function() {
        cmd = process.execPath;
    });

    it("stdout", function() {
        var bs = process.open(cmd, ['process/exec.js']);

        assert.equal(bs.readLine(), "exec testing....");

        var t0 = new Date().getTime();

        bs.readLine();
        assert.equal(bs.readLine(), "console.print....");
        assert.closeTo(new Date().getTime() - t0, 1000, 500);

        bs.readLine();
        assert.equal(bs.readLine(), "console.print....");
        assert.closeTo(new Date().getTime() - t0, 2000, 500);
    });

    it("stdin/stdout", function() {
        var bs = process.open(cmd, ['process/exec1.js']);

        bs.writeLine("hello, exec1");
        assert.equal(bs.readLine(), "hello, exec1");
    });

    it("run", function() {
        assert.equal(process.run(cmd, ['process/exec.js']), 100);
    });

    it("multi run", function() {
        coroutine.parallel([1, 2, 3, 4, 5, 6], function(n) {
            assert.equal(process.run(cmd, ['process/exec6.js', n]), n);
        });
    });

    it("multi fiber", function() {
        assert.equal(process.run(cmd, ['process/exec7.js']), 100);
    });

    it("start", function() {
        var t1 = new Date().getTime();
        process.start(cmd, ['process/exec.js']);
        assert.lessThan(new Date().getTime() - t1, 100);
    });

    it("memoryUsage", function() {
        console.dir(process.memoryUsage());
    });

    it("argv", function() {
        assert.deepEqual(json.decode(process.open(cmd, [
            "process/exec2.js",
            "arg1",
            "arg2"
        ]).readLine()), [
            cmd, "process/exec2.js", "arg1", "arg2"
        ]);
    });

    it("argv 1", function() {
        assert.deepEqual(json.decode(process.open(cmd, [
            "process/exec2.js",
            "arg1",
            "arg2",
            "--use_strict"
        ]).readLine()), [
            cmd, "process/exec2.js", "arg1", "arg2"
        ]);
    });

    it("argv utf8", function() {
        assert.deepEqual(json.decode(process.open(cmd, [
            "process/exec2.js",
            "参数1",
            "参数2"
        ]).readLine()), [
            cmd, "process/exec2.js", "参数1", "参数2"
        ]);
    });

    it("execArgv", function() {
        assert.deepEqual(json.decode(process.open(cmd, [
            "process/exec3.js",
            "arg1",
            "arg2",
            "--use_strict"
        ]).readLine()), [
            "--use_strict"
        ]);
    });

    it("env", function() {
        process.env.abc = 123;
        assert.equal(json.decode(process.open(cmd, [
            "process/exec4.js"
        ]).readLine()).abc, "123");
    });

    it("env1", function() {
        var env = json.decode(process.open(cmd, [
            "process/exec4.js"
        ], {
            env: {
                abcd: "234"
            }
        }).readLine());

        assert.isUndefined(env.abc);
        assert.equal(env.abcd, "234");
    });

    it("timeout", function() {
        var d = new Date();
        process.run(cmd, [
            "process/exec5.js"
        ], {
            timeout: 1000
        });

        assert.lessThan(new Date() - d, 2000);
    });

    it("version", () => {
        assert.ok(process.version);
    });
});

// test.run(console.DEBUG);
