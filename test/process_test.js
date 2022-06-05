var test = require("test");
test.setup();

var test_util = require('./test_util');

var process = require('process');
var coroutine = require("coroutine");
var path = require('path');
var json = require('json');
var ws = require('ws');
var net = require('net');
var http = require('http');

var cmd;
var s;

describe('process', () => {
    before(() => {
        cmd = process.execPath;
    });

    after(test_util.cleanup);

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

    describe("process.std[xx].fd", () => {
        it("process.stdin.fd", () => {
            assert.isDefined(process.stdin.fd);
            assert.equal(process.stdin.fd, 0);
        });

        it("process.stdout.fd", () => {
            assert.isDefined(process.stdout.fd);
            assert.equal(process.stdout.fd, 1);
        });

        it("process.stderr.fd", () => {
            assert.isDefined(process.stderr.fd);
            assert.equal(process.stderr.fd, 2);
        });
    });

    /**
     * @why issues: #620, #622
     */
    describe("access process.std[xx]", () => {
        const child_process = require('child_process');
        const io = require('io');
        const os = require('os');

        process.env.CI && it("test process.stdout in this proc", () => {
            // access it
            process.stdout;

            const LEN = 100
            console.log(`you could see ${LEN} lines to ouput 1~${LEN}`)
            const str = Array.apply(null, {
                length: LEN
            }).fill(undefined).map((_, idx) => {
                return `${idx + 1}`
            })

            console.log(str)
        });

        it("test process.stdin in child process", () => {
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.blocking_stdin.js')]);
            var stdin = new io.BufferedStream(bs.stdin);
            var stdout = new io.BufferedStream(bs.stdout);

            stdin.write("hello, blocking_std" + os.EOL);
            assert.equal(stdout.readLine(), "hello, blocking_std");
        });

        it("test process.stdout in child process", () => {
            const COUNT_LEN = 500;
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.blocking_stdout.js')], {
                env: {
                    COUNT_LEN
                }
            });
            var stdout = new io.BufferedStream(bs.stdout);

            var strs = Array.apply(null, {
                length: COUNT_LEN
            }).fill(undefined).map((_, idx) => {
                return `${idx + 1}`
            })

            assert.deepEqual(stdout.readLines(), strs);
        });
    });

    it("cpuUsage", () => {
        let thisUsage;
        let lastUsage = process.cpuUsage();
        for (let i = 0; i < 10; i++) {
            thisUsage = process.cpuUsage();
            assert.notLessThan(thisUsage.user, lastUsage.user);
            assert.notLessThan(thisUsage.system, lastUsage.system);
            lastUsage = thisUsage;
        }

        let startUsage;
        let diffUsage;
        for (let i = 0; i < 10; i++) {
            startUsage = process.cpuUsage();
            diffUsage = process.cpuUsage(startUsage);
            assert.notLessThan(diffUsage.user, 0);
            assert.notLessThan(diffUsage.system, 0);
        }
    });

    it("memoryUsage", () => {
        console.dir(process.memoryUsage());
    });

    it("version", () => {
        assert.ok(process.version);
    });

    it('binding', () => {
        assert.equal(process.binding('process'), process);
    })

    it("globalThis", () => {
        assert.equal(globalThis, global);
        assert.equal(globalThis.global, global);
        assert.equal(globalThis, global.globalThis);
    });
});

require.main === module && test.run(console.DEBUG);