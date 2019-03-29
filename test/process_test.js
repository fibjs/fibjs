var test = require("test");
test.setup();

var process = require('process');
var coroutine = require("coroutine");
var path = require('path');
var json = require('json');
var ws = require('ws');
var http = require('http');

var cmd;
var s;

describe('process', () => {
    before(() => {
        cmd = process.execPath;
    });

    var ss = [];

    after(() => {
        ss.forEach((s) => {
            s.close();
        });
    });

    it("hrtime", () => {
        var start = process.hrtime();
        assert.isArray(start);

        var now = Date.now();
        while (Date.now() - now < 2000);

        var diff = process.hrtime(start);

        assert.notLessThan(diff[0], 1);
        assert.notGreaterThan(diff[0], 2);
    });

    it("pid", () => {
        assert.property(process, 'pid');
        assert.isNumber(process.pid);
        assert.ok(process.pid);
    });

    it("stdout", () => {
        var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.js')]);

        assert.equal(bs.readLine(), "exec testing....");

        var t0 = new Date().getTime();

        bs.readLine();
        assert.equal(bs.readLine(), "console.print....");
        assert.closeTo(new Date().getTime() - t0, 1000, 500);

        bs.readLine();
        assert.equal(bs.readLine(), "console.print....");
        assert.closeTo(new Date().getTime() - t0, 2000, 500);
    });

    it("stdin/stdout", () => {
        var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec1.js')]);

        bs.writeLine("hello, exec1");
        assert.equal(bs.readLine(), "hello, exec1");
    });

    it("run", () => {
        assert.equal(process.run(cmd, [path.join(__dirname, 'process', 'exec.js')]), 100);
    });

    it("exitCode", () => {
        assert.equal(process.run(cmd, [path.join(__dirname, 'process', 'exec13.js')]), 100);
        assert.equal(process.run(cmd, [path.join(__dirname, 'process', 'exec14.js')]), 101);
    });

    it("run throw error", () => {
        assert.throws(() => {
            process.run("not_exists_exec_file");
        });
    });

    it("multi run", () => {
        coroutine.parallel([1, 2, 3, 4, 5, 6], (n) => {
            assert.equal(process.run(cmd, [path.join(__dirname, 'process', 'exec6.js'), n]), n);
        });
    });

    describe('process holding', () => {
        it("multi fiber", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec7.js')]);
            assert.equal(p.readLine(), "100");
            assert.equal(p.wait(), 7);
        });

        it("pendding callback", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec8.js')]);
            assert.equal(p.readLine(), "200");
            assert.equal(p.wait(), 8);
        });

        it("setTimeout", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec9.js')]);
            assert.equal(p.readLine(), "300");
            assert.equal(p.wait(), 9);
        });

        it("setTimeout unref", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec9.1.js')]);
            assert.equal(p.readLine(), "301");
            assert.equal(p.wait(), 0);
        });

        it("setTimeout ref", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec9.2.js')]);
            assert.equal(p.readLine(), "302");
            assert.equal(p.wait(), 9);
        });

        it("setInterval", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec10.js')]);
            assert.equal(p.readLine(), "400");
            assert.equal(p.wait(), 10);
        });

        it("setImmediate", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec11.js')]);
            assert.equal(p.readLine(), "500");
            assert.equal(p.wait(), 11);
        });

        it("websocket connect", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec18.js')]);
            assert.equal(p.readLine(), "1800");
            assert.equal(p.wait(), 18);
        });

        it("websocket disconnect", () => {
            var httpd = new http.Server(8899, {
                "/ws": ws.upgrade((s) => {
                    s.onmessage = function (msg) {
                        s.send(msg);
                    };
                })
            });
            ss.push(httpd.socket);
            httpd.run(() => {});

            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec19.js')]);
            assert.equal(p.readLine(), "1900");
            assert.equal(p.wait(), 19);
        });

        it("worker", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec20.js')]);
            assert.equal(p.readLine(), "2000");
            assert.equal(p.wait(), 20);
        });

        it("bugfix: multi fiber async", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec12.js')]);
            assert.equal(p.readLine(), "600");
            assert.equal(p.wait(), 12);
        });
    });

    it("start", () => {
        var t1 = new Date().getTime();
        process.start(cmd, [path.join(__dirname, 'process', 'exec.js')]);
        assert.lessThan(new Date().getTime() - t1, 100);
    });

    it("kill", () => {
        var t1 = new Date().getTime();
        var p = process.start(cmd, [path.join(__dirname, 'process', 'exec.js')]);
        coroutine.sleep(500);
        p.kill(15);
        p.wait();
        assert.lessThan(new Date().getTime() - t1, 1000);
    });

    it("memoryUsage", () => {
        console.dir(process.memoryUsage());
    });

    it("argv", () => {
        assert.deepEqual(json.decode(process.open(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "arg1",
            "arg2"
        ]).readLine()), [
            cmd, path.join(__dirname, "process", "exec2.js"), "arg1", "arg2"
        ]);
    });

    it("argv 1", () => {
        assert.deepEqual(json.decode(process.open(cmd, [
            "--use_strict",
            "--test1",
            path.join(__dirname, "process", "exec2.js"),
            "arg1",
            "arg2"
        ]).readLine()), [
            cmd, path.join(__dirname, "process", "exec2.js"), "arg1", "arg2"
        ]);
    });

    it("argv utf8", () => {
        assert.deepEqual(json.decode(process.open(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "参数1",
            "参数2"
        ]).readLine()), [
            cmd, path.join(__dirname, "process", "exec2.js"), "参数1", "参数2"
        ]);
    });

    it("execArgv", () => {
        assert.deepEqual(json.decode(process.open(cmd, [
            "--use_strict",
            "--test",
            path.join(__dirname, "process", "exec3.js"),
            "arg1",
            "arg2"
        ]).readLine()), [
            "--use_strict",
            "--test",
        ]);
    });

    it("env", () => {
        process.env.abc = 123;
        assert.equal(json.decode(process.open(cmd, [
            path.join(__dirname, "process", "exec4.js")
        ]).readLine()).abc, "123");
    });

    it("env1", () => {
        var env = json.decode(process.open(cmd, [
            path.join(__dirname, "process", "exec4.js")
        ], {
            env: {
                abcd: "234"
            }
        }).readLine());

        assert.isUndefined(env.abc);
        assert.equal(env.abcd, "234");
    });

    it("timeout", () => {
        var d = new Date();
        process.run(cmd, [
            path.join(__dirname, "process", "exec5.js")
        ], {
            timeout: 1000
        });

        assert.lessThan(new Date() - d, 2000);
    });

    it("version", () => {
        assert.ok(process.version);
    });

    if (process.platform != "win32") {
        it("PATH env", () => {
            assert.equal(process.run("ls", [path.join(__dirname, "process")]), 0)
            assert.ok(process.open("ls", ["-a", path.join(__dirname, "process")]).stdout.readLine());
        });

        it("umask()", () => {
            const mask = '0664';
            assert.equal(process.umask(), 0);

            const old = process.umask(mask);
            assert.equal(parseInt(mask, 8), process.umask(old));

            // confirm reading the umask does not modify it.
            // 1. If the test fails, this call will succeed, but the mask will be set to 0
            assert.equal(old, process.umask());
            // 2. If the test fails, process.umask() will return 0
            assert.equal(old, process.umask());
        });
    }

    describe("Event", () => {
        it("beforeExit", () => {
            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec15.js')]);
            assert.deepEqual(bs.readLines(), [
                "beforeExit 101",
                "other beforeExit 101",
                "new work 101",
                "beforeExit 101",
                "other beforeExit 101"
            ]);

            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec16.js')]);
            assert.deepEqual(bs.readLines(), []);
        });

        it("exit", () => {
            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec17.js')]);
            assert.deepEqual(bs.readLines(), [
                "exit 101",
                "other exit 101"
            ]);
        });
    });
});

require.main === module && test.run(console.DEBUG);