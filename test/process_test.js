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
            const str = Array.apply(null, { length: LEN }).fill(undefined).map((_, idx) => {
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

            var strs = Array.apply(null, { length: COUNT_LEN }).fill(undefined).map((_, idx) => {
                return `${idx + 1}`
            })

            assert.deepEqual(stdout.readLines(), strs);
        });
    });

    xdescribe("ppid", () => {
        it("basic", () => {
            assert.property(process, 'ppid');
            assert.isNumber(process.ppid);
        });

        it("`ppid` in subprocess equals to its parent process's `pid`", () => {
            var sp = process.open(cmd, [path.join(__dirname, 'process', 'exec.ppid.js')]);
            var [_ppid, _pid] = sp.stdout.readLines();

            assert.strictEqual(_pid, `${sp.pid}`);
            assert.strictEqual(_ppid, `${process.pid}`);
        });
    });

    xit("stdout", () => {
        var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.js')]);

        assert.equal(bs.stdout.readLine(), "exec testing....");

        var t0 = new Date().getTime();

        bs.stdout.readLine();
        assert.equal(bs.stdout.readLine(), "console.print....");
        assert.closeTo(new Date().getTime() - t0, 1000, 500);

        bs.stdout.readLine();
        assert.equal(bs.stdout.readLine(), "console.print....");
        assert.closeTo(new Date().getTime() - t0, 2000, 500);
    });

    xdescribe("stdout/stderr", () => {
        it("SubProcess::stderr/stdout exist", () => {
            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.js')]);

            assert.exist(bs.stdout);
            assert.exist(bs.stderr);
        });

        it("stdout output", () => {
            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.stdout.js')]);

            assert.equal(bs.stdout.readLine(), "exec testing....");

            var t0 = new Date().getTime();

            bs.stdout.readLine();
            var offsets = []
            offsets[0] = new Date().getTime() - t0;
            assert.closeTo(offsets[0], 1000, 500);

            bs.stdout.readLine();
            offsets[1] = new Date().getTime() - t0;
            assert.closeTo(offsets[1], 2000, 1000);
        });

        it("stderr output", () => {
            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.stderr.js')]);

            assert.equal(bs.stderr.readLine(), "exec testing....");

            var t0 = new Date().getTime();

            bs.stderr.readLine();
            var offsets = []
            offsets[0] = new Date().getTime() - t0;
            assert.closeTo(offsets[0], 1000, 500);

            bs.stderr.readLine();
            offsets[1] = new Date().getTime() - t0;
            assert.closeTo(offsets[1], 2000, 1000);
        });
    });

    xit("stdin/stdout", () => {
        var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec1.js')]);

        bs.stdin.writeLine("hello, exec1");
        assert.equal(bs.stdout.readLine(), "hello, exec1");
    });

    xit("stdin/stdout stream", () => {
        var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.chargeable.js')]);
        var outputs = []

        process.nextTick(() => {
            var oline = null

            while (true) {
                oline = bs.stdout.readLine()
                if (oline === 'exit') {
                    break
                }

                if (oline) {
                    outputs.push(oline)
                }
            }
        });

        process.nextTick(() => {
            bs.stdin.writeLine('line1')
            bs.stdin.writeLine('line2')
            bs.stdin.writeLine('.exit')
        })

        bs.wait()

        assert.deepEqual(
            outputs,
            [
                `> your input is: line1`,
                `> your input is: line2`,
            ]
        )
    });

    xit("run", () => {
        assert.equal(process.run(cmd, [path.join(__dirname, 'process', 'exec.js')]), 100);
    });

    xit("exitCode", () => {
        assert.equal(process.run(cmd, [path.join(__dirname, 'process', 'exec13.js')]), 100);
        assert.equal(process.run(cmd, [path.join(__dirname, 'process', 'exec14.js')]), 101);
    });

    if (require("os").type() != "Linux")
        xit("run throw error", () => {
            assert.throws(() => {
                process.run("not_exists_exec_file");
            });
        });

    xit("multi run", () => {
        coroutine.parallel([1, 2, 3, 4, 5, 6], (n) => {
            assert.equal(process.run(cmd, [path.join(__dirname, 'process', 'exec6.js'), n]), n);
        });
    });

    xdescribe('process holding', () => {
        it("multi fiber", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec7.js')]);
            assert.equal(p.stdout.readLine(), "100");
            assert.equal(p.wait(), 7);
        });

        it("pendding callback", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec8.js')]);
            assert.equal(p.stdout.readLine(), "200");
            assert.equal(p.wait(), 8);
        });

        it("setTimeout", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec9.js')]);
            assert.equal(p.stdout.readLine(), "300");
            assert.equal(p.wait(), 9);
        });

        it("setTimeout unref", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec9.1.js')]);
            assert.equal(p.stdout.readLine(), "301");
            assert.equal(p.wait(), 0);
        });

        it("setTimeout ref", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec9.2.js')]);
            assert.equal(p.stdout.readLine(), "302");
            assert.equal(p.wait(), 9);
        });

        it("setInterval", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec10.js')]);
            assert.equal(p.stdout.readLine(), "400");
            assert.equal(p.wait(), 10);
        });

        it("setImmediate", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec11.js')]);
            assert.equal(p.stdout.readLine(), "500");
            assert.equal(p.wait(), 11);
        });

        it("websocket connect", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec18.js')]);
            assert.equal(p.wait(), 81);
        });

        it("websocket disconnect", () => {
            var httpd = new http.Server(8899, {
                "/ws": ws.upgrade((s) => {
                    s.onmessage = function (msg) {
                        s.send(msg);
                    };
                })
            });
            test_util.push(httpd.socket);
            httpd.start();

            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec19.js')]);
            assert.equal(p.stdout.readLine(), "1900");
            assert.equal(p.wait(), 19);
        });

        it("worker", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec20.js')]);
            assert.equal(p.stdout.readLine(), "2000");
            assert.equal(p.wait(), 20);
        });

        it("bugfix: multi fiber async", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec12.js')]);
            assert.equal(p.stdout.readLine(), "600");
            assert.equal(p.wait(), 12);
        });

        it("tcp server", () => {
            var p = process.open(cmd, [path.join(__dirname, 'process', 'exec21.js')]);

            for (var i = 0; i < 100; i++) {
                coroutine.sleep(10);
                try {
                    net.connect('tcp://127.0.0.1:28080');
                    break;
                } catch (e) { }
            }

            assert.equal(p.stdout.readLine(), "700");
            assert.equal(p.wait(), 21);
        });

    });

    xit("start", () => {
        var t1 = new Date().getTime();
        process.start(cmd, [path.join(__dirname, 'process', 'exec.js')]);
        assert.lessThan(new Date().getTime() - t1, 100);
    });

    xit("kill", () => {
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
    //==================================================
    xit("argv", () => {
        assert.deepEqual(json.decode(process.open(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "arg1",
            "arg2"
        ]).stdout.readLine()), [
            cmd, path.join(__dirname, "process", "exec2.js"), "arg1", "arg2"
        ]);
    });

    xit("argv 1", () => {
        assert.deepEqual(json.decode(process.open(cmd, [
            "--use_strict",
            "--test1",
            path.join(__dirname, "process", "exec2.js"),
            "arg1",
            "arg2"
        ]).stdout.readLine()), [
            cmd, path.join(__dirname, "process", "exec2.js"), "arg1", "arg2"
        ]);
    });

    xit("argv utf8", () => {
        assert.deepEqual(json.decode(process.open(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "参数1",
            "参数2"
        ]).stdout.readLine()), [
            cmd, path.join(__dirname, "process", "exec2.js"), "参数1", "参数2"
        ]);
    });

    xit("execArgv", () => {
        assert.deepEqual(json.decode(process.open(cmd, [
            "--use_strict",
            "--test",
            path.join(__dirname, "process", "exec3.js"),
            "arg1",
            "arg2"
        ]).stdout.readLine()), [
            "--use_strict",
            "--test",
        ]);
    });

    xit("env", () => {
        process.env.abc = 123;
        assert.equal(json.decode(process.open(cmd, [
            path.join(__dirname, "process", "exec4.js")
        ]).stdout.readLine()).abc, "123");
    });

    xit("env1", () => {
        var env = json.decode(process.open(cmd, [
            path.join(__dirname, "process", "exec4.js")
        ], {
            env: {
                abcd: "234"
            }
        }).stdout.readLine());

        assert.isUndefined(env.abc);
        assert.equal(env.abcd, "234");
    });

    xit("timeout", () => {
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
        xit("PATH env", () => {
            assert.equal(process.run("ls", [path.join(__dirname, "process")]), 0)
            assert.ok(process.open("ls", ["-a", path.join(__dirname, "process")]).stdout.readLine());
        });

        xit("umask()", () => {
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

    it('binding', () => {
        assert.equal(process.binding('process'), process);
    })

    xdescribe("Event", () => {
        it("beforeExit", () => {
            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec15.js')]);
            assert.deepEqual(bs.stdout.readLines(), [
                "beforeExit 101",
                "other beforeExit 101",
                "new work 101",
                "beforeExit 101",
                "other beforeExit 101"
            ]);

            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec16.js')]);
            assert.deepEqual(bs.stdout.readLines(), []);
        });

        it("exit", () => {
            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec17.js')]);
            assert.deepEqual(bs.stdout.readLines(), [
                "exit 101",
                "other exit 101"
            ]);
        });
    });

    xdescribe("SubProcess Spec", () => {
        it("default kvs", () => {
            var retcode = process.run(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')]);
            assert.equal(retcode, 0)
        });

        if (process.platform === 'win32') {
            const win_keys = [
                'SYSTEMROOT',
                'SystemRoot',
                'TEMP',
                'TMP',
                // 'CommonProgramFiles',
                'CommonProgramFiles(x86)',
                'CommonProgramW6432',
                // 'ProgramFiles',
                'ProgramFiles(x86)',
                'ProgramW6432',
            ];

            it(`default kvs: win32`, () => {
                var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')]);
                bs.wait()

                assert.deepEqual(
                    bs.stdout.readLines(),
                    win_keys.map(key => `process.env['${key}']=${process.env[key] || ''}`)
                );
            });

            it(`specify kvs: win32`, () => {
                const envs = {};
                /**
                 * @notice win32's program lifecycle depends on some reserve environment, never override them
                 */
                win_keys.forEach(key => envs[`nothing_${key}`] = `nothing_${key}`);

                Object.keys(envs).forEach(x => {
                    assert.notEqual(x, process.env[x])
                });

                var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')], {
                    env: envs
                });
                bs.wait()

                assert.deepEqual(
                    bs.stdout.readLines(),
                    win_keys.map(key => `process.env['${key}']=${process.env[key] || ''}`)
                )
            });

            win_keys.forEach(win_key => {
                it(`override required reserve env vars - ${win_key}`, () => {
                    var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')], {
                        env: {
                            [win_key]: undefined
                        }
                    });
                    bs.wait();

                    assert.deepEqual(
                        bs.stdout.readLines(),
                        win_keys.map(key => {
                            if (key !== win_key)
                                return `process.env['${key}']=${process.env[key] || ''}`
                            else
                                return `process.env['${key}']=`
                        })
                    );

                    var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')], {
                        env: {
                            [win_key]: 'foo'
                        }
                    });
                    bs.wait();

                    assert.deepEqual(
                        bs.stdout.readLines(),
                        win_keys.map(key => {
                            if (key !== win_key)
                                return `process.env['${key}']=${process.env[key] || ''}`
                            else
                                return `process.env['${key}']=foo`
                        })
                    );
                });
            });

            win_keys.forEach(win_key => {
                if (win_key.toUpperCase() === 'SYSTEMROOT') return;

                it(`cancel reserve env var - ${win_key}`, () => {
                    var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.win32_envs.js')], {
                        env: {
                            [win_key]: process.env[win_key]
                        }
                    });
                    bs.wait();

                    assert.deepEqual(bs.stdout.readLines(), []);
                });
            });
        }

        if (process.platform === 'darwin') {
            it(`default kvs: darwin`, () => {
                var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')]);
                bs.wait()

                assert.deepEqual(
                    bs.stdout.readLines(),
                    [
                        `process.env.HOME=${process.env.HOME || ''}`,
                        `process.env.TMPDIR=${process.env.TMPDIR || ''}`,
                    ]
                )
            });

            it(`specify kvs: darwin`, () => {
                const envs = {
                    HOME: 'noHome',
                    TMPDIR: 'noTMPDIR'
                };

                var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')], {
                    env: envs
                });
                bs.wait()

                assert.deepEqual(
                    bs.stdout.readLines(),
                    [
                        `process.env.HOME=${envs.HOME}`,
                        `process.env.TMPDIR=${envs.TMPDIR}`,
                    ]
                )

                Object.keys(envs).forEach(x => assert.notEqual(x, process.env[x]))
            });
        }

        if (process.platform === 'linux') {
            it(`default kvs: linux`, () => {
                var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')]);
                bs.wait()

                assert.deepEqual(
                    bs.stdout.readLines(),
                    [
                        `process.env.HOME=${process.env.HOME || ''}`,
                        `process.env.TMPDIR=${process.env.TMPDIR || ''}`,
                    ]
                )
            });

            it(`specify kvs: linux`, () => {
                const envs = {
                    HOME: 'noHome',
                    TMPDIR: 'noTMPDIR'
                };

                var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.env_kvs.js')], {
                    env: envs
                });
                bs.wait()

                assert.deepEqual(
                    bs.stdout.readLines(),
                    [
                        `process.env.HOME=${envs.HOME}`,
                        `process.env.TMPDIR=${envs.TMPDIR}`,
                    ]
                )

                Object.keys(envs).forEach(x => assert.notEqual(x, process.env[x]))
            });
        }

        /**
         * in win32, socket/nslookup require system win32 api, some ENV variable is required
         * - SYSTEMROOT
         * if case below cannot executed normally, check your implementation about environment variables in 
         * sub process
         */
        it("dns.resolve", () => {
            var bs = process.open(cmd, [path.join(__dirname, 'process', 'exec.dns.js')]);
            bs.wait()

            assert.deepEqual(
                bs.stdout.readLines(),
                [
                    `resolve domain success!`,
                    `lookup domain success!`,
                ]
            )
        });
    });
});

require.main === module && test.run(console.DEBUG);