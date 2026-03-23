var { describe, it, xit, before, after } = require('node:test');
var assert = require('assert');

var test_util = require('./test_util');

const child_process = require('child_process');
var coroutine = require("coroutine");
var path = require('path');
var json = require('json');
var net = require('net');
var http = require('http');
var io = require('io');
var os = require('os');

const isWin32 = process.platform === "win32";
const isIOS = process.platform === "ios";

var envKeys = require('./process/const.env_keys.js');

describe("child_process", () => {
    var cmd;

    before(() => {
        cmd = process.execPath;
    });

    after(test_util.cleanup);

    it("stdout", () => {
        var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.js')]);
        var stdout = new io.BufferedStream(bs.stdout);

        assert.equal(stdout.readLine(), "exec testing....");

        var t0 = new Date().getTime();

        stdout.readLine();
        assert.equal(stdout.readLine(), "console.print....");
        assert.closeTo(new Date().getTime() - t0, 1000, 500);

        stdout.readLine();
        assert.equal(stdout.readLine(), "console.print....");
        assert.closeTo(new Date().getTime() - t0, 2000, 500);

        var stderr = new io.BufferedStream(bs.stderr);
        assert.deepEqual(stderr.readLines(), [
            "warn exec testing....",
            "error exec testing...."
        ]);
    });

    describe("ChildProcess::std[xx].fd", () => {
        it("ChildProcess::stdin.fd", () => {
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.js')]);
            var stdin = new io.BufferedStream(bs.stdin);

            assert.isDefined(stdin.fd);
            assert.ok(stdin.fd > 2 || stdin.fd < 0);
        });

        it("ChildProcess::stdout.fd", () => {
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.js')]);
            var stdout = new io.BufferedStream(bs.stdout);

            assert.isDefined(stdout.fd);
            assert.ok(stdout.fd > 2 || stdout.fd < 0);
        });

        it("ChildProcess::stderr.fd", () => {
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.js')]);
            var stderr = new io.BufferedStream(bs.stderr);

            assert.isDefined(stderr.fd);
            assert.ok(stderr.fd > 2 || stderr.fd < 0);
        });
    });

    describe("stdout/stderr", () => {
        it("SubProcess::stderr/stdout exist", () => {
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.js')]);

            assert.exist(bs.stdout);
            assert.exist(bs.stderr);
        });

        it("stdout output", () => {
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.stdout.js')]);
            var stdout = new io.BufferedStream(bs.stdout);

            assert.equal(stdout.readLine(), "exec testing....");

            var t0 = new Date().getTime();

            stdout.readLine();
            var offsets = []
            offsets[0] = new Date().getTime() - t0;
            assert.closeTo(offsets[0], 1000, 500);

            stdout.readLine();
            offsets[1] = new Date().getTime() - t0;
            assert.closeTo(offsets[1], 2000, 1000);
        });

        it("stderr output", () => {
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.stderr.js')]);
            var stderr = new io.BufferedStream(bs.stderr);

            assert.equal(stderr.readLine(), "exec testing....");

            var t0 = new Date().getTime();

            stderr.readLine();
            var offsets = []
            offsets[0] = new Date().getTime() - t0;
            assert.closeTo(offsets[0], 1000, 500);

            stderr.readLine();
            offsets[1] = new Date().getTime() - t0;
            assert.closeTo(offsets[1], 2000, 1000);
        });

        it("console stdout output", () => {
            var status = child_process.run(cmd, [path.join(__dirname, 'process', 'exec.stdout.js')]);
            assert.equal(status, 0);
        });

        it("sequential stdin write", () => {
            // Tests: parent writes 10 chunks with delay between each, child reads all synchronously
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.stdin_seq_write.js')]);
            var str = "";
            coroutine.sleep(100);
            for (var i = 0; i < 10; i++) {
                coroutine.sleep(10);
                var id = test_util.makeid(10);
                str += id;
                bs.stdin.write(id);
            }
            var str1 = bs.stdout.read(100).toString();
            assert.equal(str, str1);
        });

        it("bulk write with parallel read", () => {
            // Tests: parent writes all data at once, child reads via 10 parallel read(10) calls
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.stdin_parallel_read.js')]);
            var str = "";
            for (var i = 0; i < 10; i++) {
                str += test_util.makeid(10);
            }
            coroutine.sleep(100);
            bs.stdin.write(str);
            var str1 = bs.stdout.read(100).toString();
            assert.equal(str, str1);
        });

        it("rapid stdin write", () => {
            // Tests: parent writes 10 chunks without sleep, child reads all synchronously
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.stdin_seq_write.js')]);
            var str = "";
            coroutine.sleep(100);
            for (var i = 0; i < 10; i++) {
                var id = test_util.makeid(10);
                str += id;
                bs.stdin.write(id);
            }
            var str1 = bs.stdout.read(100).toString();
            assert.equal(str, str1);
        })
    });

    if (!isWin32)
        describe("pty", () => {
            it("basic pty functionality", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.stdout.js')], {
                    stdio: 'pty'
                });
                var stdout = new io.BufferedStream(bs.stdout);

                // Function to strip ANSI escape sequences for ConPTY compatibility
                function stripAnsi(str) {
                    return str.replace(/\x1b\[[0-9;?]*[a-zA-Z]/g, '');
                }

                if (isWin32) {
                    var line = stdout.readLine();
                    var cleanLine = stripAnsi(line);
                    assert.equal(cleanLine, "exec testing....");
                } else
                    assert.equal(stdout.readLine(), "exec testing....\r");

                var t0 = new Date().getTime();

                stdout.readLine();
                var offsets = []
                offsets[0] = new Date().getTime() - t0;
                assert.closeTo(offsets[0], 1000, 500);

                stdout.readLine();
                offsets[1] = new Date().getTime() - t0;
                assert.closeTo(offsets[1], 2000, 1000);
            });

            it("pty with custom initial size", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'pty_simple_test.js')], {
                    stdio: 'pty',
                    cols: 100,
                    rows: 30
                });

                // Check that the process has the specified dimensions
                assert.equal(bs.cols, 100);
                assert.equal(bs.rows, 30);

                var stdout = new io.BufferedStream(bs.stdout);

                // Function to strip ANSI escape sequences for ConPTY compatibility
                function stripAnsi(str) {
                    return str.replace(/\x1b\[[0-9;?]*[a-zA-Z]/g, '');
                }

                if (isWin32) {
                    var line = stdout.readLine();
                    var cleanLine = stripAnsi(line);
                    assert.equal(cleanLine, "PTY_TEST_OUTPUT");
                } else {
                    assert.equal(stdout.readLine(), "PTY_TEST_OUTPUT\r");
                }

                bs.join();
                assert.equal(bs.exitCode, 42);
            });

            it("pty default size", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'pty_simple_test.js')], {
                    stdio: 'pty'
                });

                // Default size should be 80x24
                assert.equal(bs.cols, 80);
                assert.equal(bs.rows, 24);

                bs.join();
            });

            it("pty resize functionality", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'pty_resize_test.js')], {
                    stdio: 'pty',
                    cols: 80,
                    rows: 24
                });
                var stdout = new io.BufferedStream(bs.stdout);

                // Check initial size
                assert.equal(bs.cols, 80);
                assert.equal(bs.rows, 24);

                // Read initial size output
                var initialLine = stdout.readLine();
                if (isWin32) {
                    // Strip ANSI sequences for Windows ConPTY
                    initialLine = initialLine.replace(/\x1b\[[0-9;?]*[a-zA-Z]/g, '');
                } else {
                    // Remove carriage return for Unix PTY
                    initialLine = initialLine.replace(/\r$/, '');
                }
                assert.equal(initialLine, "RESIZE:80x24");

                // Resize the PTY
                bs.resize(120, 40);

                // Check that resize was successful
                assert.equal(bs.cols, 120);
                assert.equal(bs.rows, 40);

                // Read resize notification (may take a moment to appear)
                var resizeLine;
                var attempts = 0;
                while (attempts < 10) {
                    try {
                        resizeLine = stdout.readLine();
                        if (isWin32) {
                            resizeLine = resizeLine.replace(/\x1b\[[0-9;?]*[a-zA-Z]/g, '');
                        } else {
                            resizeLine = resizeLine.replace(/\r$/, '');
                        }
                        if (resizeLine.includes("RESIZE:120x40")) {
                            break;
                        }
                    } catch (e) {
                        coroutine.sleep(100);
                        attempts++;
                    }
                }

                // Should have received resize notification
                assert.ok(resizeLine && resizeLine.includes("RESIZE:120x40"), `Expected resize notification, got: ${resizeLine}`);

                // Test another resize
                bs.resize(60, 20);
                assert.equal(bs.cols, 60);
                assert.equal(bs.rows, 20);

                // Clean up
                bs.kill();
                bs.join();
            });

            it("pty resize with invalid parameters", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'pty_simple_test.js')], {
                    stdio: 'pty',
                    cols: 80,
                    rows: 24
                });

                // Test invalid resize parameters
                assert.throws(() => {
                    bs.resize(0, 24);
                });

                assert.throws(() => {
                    bs.resize(80, 0);
                });

                assert.throws(() => {
                    bs.resize(-10, 24);
                });

                assert.throws(() => {
                    bs.resize(80, -5);
                });

                // Valid resize should work
                assert.doesNotThrow(() => {
                    bs.resize(80, 24);
                });

                bs.join();
            });

            it("pty resize on non-pty process should fail", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'pty_simple_test.js')], {
                    stdio: 'pipe'
                });

                // Resize should fail on non-PTY process
                assert.throws(() => {
                    bs.resize(80, 24);
                });

                bs.join();
            });

            it("cols and rows getters", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'pty_simple_test.js')], {
                    stdio: 'pty',
                    cols: 90,
                    rows: 35
                });

                // Check getter properties
                assert.equal(bs.cols, 90);
                assert.equal(bs.rows, 35);

                // Resize and check again
                bs.resize(110, 50);
                assert.equal(bs.cols, 110);
                assert.equal(bs.rows, 50);

                bs.join();
            });

            it("cols and rows getters on non-pty process", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'pty_simple_test.js')], {
                    stdio: 'pipe'
                });

                // cols and rows should throw error for non-PTY processes
                assert.throws(() => {
                    var cols = bs.cols;
                }, /cols property only available in PTY mode/);

                assert.throws(() => {
                    var rows = bs.rows;
                }, /rows property only available in PTY mode/);

                bs.join();
            });

            it("console.log and stdout.write ordering", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.stdout_console_order.js')], {
                    stdio: 'pty'
                });
                var stdout = new io.BufferedStream(bs.stdout);

                // Strip ANSI escape sequences and trailing \r from pty output
                function cleanLine(str) {
                    return str.replace(/\x1b\[[0-9;?]*[a-zA-Z]/g, '').replace(/\r$/, '');
                }

                var lines = [];
                for (var i = 0; i < 40; i++) {
                    var line = stdout.readLine();
                    if (line !== null)
                        lines.push(cleanLine(line));
                }

                // Verify console.log always appears before its paired stdout.write
                for (var i = 0; i < 20; i++) {
                    assert.equal(lines[i * 2], 'console-' + i, 'console-' + i + ' order wrong');
                    assert.equal(lines[i * 2 + 1], 'stdout-' + i, 'stdout-' + i + ' order wrong');
                }

                bs.join();
            });
        });

    it("stdin/stdout", () => {
        var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec1.js')]);
        var stdout = new io.BufferedStream(bs.stdout);

        bs.stdin.write("hello, exec1" + os.EOL);
        assert.equal(stdout.readLine(), "hello, exec1");
    });

    it("fork", () => {
        var bs = child_process.fork(path.join(__dirname, 'process', 'exec1.js'), {
            stdio: "pipe"
        });
        var stdout = new io.BufferedStream(bs.stdout);

        bs.stdin.write("hello, exec1" + os.EOL);
        assert.equal(stdout.readLine(), "hello, exec1");
    });

    const isAndroid = process.platform === 'android';
    
    if (!process.env.QEMU_LD_PREFIX && !isAndroid) {
        it("exec", () => {
            var ret = child_process.exec("export a = 100");
            assert.equal(ret.stdout, null);

            var ret = child_process.exec("echo hello");
            assert.equal(ret.stdout, "hello" + os.EOL);

            if (isWin32) {
                var ret = child_process.exec(`echo "hello world"`);
                assert.equal(ret.stdout, `"hello world"\r\n`);

                var ret = child_process.exec(`echo "hello "world""`);
                assert.equal(ret.stdout, `"hello "world""\r\n`);
            } else {
                var ret = child_process.exec(`echo "hello world"`);
                assert.equal(ret.stdout, `hello world\n`);

                var ret = child_process.exec(`echo "hello "world""`);
                assert.equal(ret.stdout, `hello world\n`);
            }
        });

        it("sh", () => {
            const $ = child_process.sh;

            var ret = $`echo hello`;
            assert.equal(ret, "hello");

            var word = "world";
            var ret = $`echo ${word}`;
            assert.equal(ret, "world");

            var hello = "hello";
            var ret = $`echo ${hello} ${word}`;
            assert.equal(ret, "hello world");

            var o = { a: 1, b: 2 };
            var ret = $`echo ${o}`;
            assert.equal(ret, '[object Object]');

            var a = [1, 2, 3];
            var ret = $`echo ${a}`;
            assert.equal(ret, '1 2 3');

            assert.throws(() => {
                $`echo1 100`;
            });
        });

        it("execSync", () => {
            // Test successful execSync
            var ret = child_process.execSync("echo hello");
            assert.equal(ret, "hello" + os.EOL);

            // Test execSync with encoding
            var ret = child_process.execSync("echo world", { encoding: 'utf8' });
            assert.equal(ret, "world" + os.EOL);

            // Test execSync error case
            assert.throws(() => {
                child_process.execSync("exit 1");
            }, (error) => {
                assert.equal(error.status, 1);
                assert.equal(error.signal, null);
                assert.ok(error.hasOwnProperty('stdout'));
                assert.ok(error.hasOwnProperty('stderr'));
                assert.ok(error.hasOwnProperty('output'));
                assert.equal(error.output.length, 3);
                assert.equal(error.output[0], null);
                return true;
            });

            // Test execSync with command not found
            assert.throws(() => {
                child_process.execSync("nonexistent_command_12345");
            }, (error) => {
                if (isWin32) {
                    assert.equal(error.status, 1);
                } else {
                    assert.equal(error.status, 127);
                }
                assert.equal(error.signal, null);
                assert.ok(error.stderr.includes("command not found") ||
                    error.stderr.includes("not found") ||
                    error.stderr.includes("not recognized"));
                return true;
            });

            if (isWin32) {
                var ret = child_process.execSync(`echo "hello world"`);
                assert.equal(ret, `"hello world"\r\n`);
            } else {
                var ret = child_process.execSync(`echo "hello world"`);
                assert.equal(ret, `hello world\n`);
            }
        });

        it("execFileSync", () => {
            // Test successful execFileSync
            var ret = child_process.execFileSync(cmd, [
                path.join(__dirname, "process", "exec_sync_success.js")
            ]);
            assert.equal(ret, "execSync success output" + os.EOL);

            // Test execFileSync with encoding
            var ret = child_process.execFileSync(cmd, [
                path.join(__dirname, "process", "exec_sync_success.js")
            ], { encoding: 'utf8' });
            assert.equal(ret, "execSync success output" + os.EOL);

            // Test execFileSync with arguments
            var ret = child_process.execFileSync(cmd, [
                path.join(__dirname, "process", "exec_file_sync.js"),
                "arg1", "arg2"
            ]);
            assert.ok(ret.includes('["arg1","arg2"]'));

            // Test execFileSync error case
            assert.throws(() => {
                child_process.execFileSync(cmd, [
                    path.join(__dirname, "process", "exec_sync_error.js")
                ]);
            }, (error) => {
                assert.equal(error.status, 1);
                assert.equal(error.signal, null);
                assert.ok(error.stdout.includes("execSync stdout before error"));
                assert.ok(error.stderr.includes("execSync stderr error message"));
                assert.ok(error.hasOwnProperty('output'));
                assert.equal(error.output.length, 3);
                assert.equal(error.output[0], null);
                return true;
            });

            // Test execFileSync with different exit code
            assert.throws(() => {
                child_process.execFileSync(cmd, [
                    path.join(__dirname, "process", "exec_file_sync_error.js")
                ]);
            }, (error) => {
                assert.equal(error.status, 42);
                assert.equal(error.signal, null);
                assert.ok(error.stdout.includes("execFileSync stdout"));
                assert.ok(error.stderr.includes("execFileSync stderr"));
                return true;
            });

            // Test execFileSync with stdio inherit
            var ret = child_process.execFileSync(cmd, [
                path.join(__dirname, "process", "exec_sync_success.js")
            ], {
                stdio: "inherit"
            });
            assert.equal(ret, null);

            // Test execFileSync with env option
            assert.throws(() => {
                child_process.execFileSync(cmd, [
                    path.join(__dirname, "process", "exec4.js")
                ], {
                    env: {
                        QEMU_LD_PREFIX: process.env.QEMU_LD_PREFIX,
                        test_env_var: "test_value"
                    }
                });
            }, (error) => {
                // iOS returns different exit status
                if (!isIOS) {
                    assert.equal(error.status, 4);
                    var env = json.decode(error.stdout);
                    assert.equal(env.test_env_var, "test_value");
                }
                return true;
            });
        });
    }

    if (!isWin32 && !process.env.QEMU_LD_PREFIX && !isAndroid) {
        describe("timeout", () => {
            it("execFile with timeout", () => {
                var t = Date.now();
                var result = child_process.execFile("sleep", ["10"], { timeout: 500 });
                var elapsed = Date.now() - t;
                assert.equal(result.exitCode, -15);
                assert.ok(elapsed < 2000);
            });

            it("execFile without timeout completes normally", () => {
                var result = child_process.execFile("echo", ["hello"], { timeout: 0 });
                assert.equal(result.exitCode, 0);
                assert.equal(result.stdout.trim(), "hello");
            });

            it("exec with timeout", () => {
                var t = Date.now();
                var result = child_process.exec("sleep 10", { timeout: 500 });
                var elapsed = Date.now() - t;
                assert.equal(result.exitCode, -15);
                assert.ok(elapsed < 2000);
            });

            it("spawnSync with timeout", () => {
                var t = Date.now();
                var result = child_process.spawnSync("sleep", ["10"], { timeout: 500 });
                var elapsed = Date.now() - t;
                assert.equal(result.status, -15);
                assert.ok(elapsed < 2000);
            });

            it("execSync with timeout throws", () => {
                assert.throws(() => {
                    child_process.execSync("sleep 10", { timeout: 500 });
                });
            });

            it("execFileSync with timeout throws", () => {
                assert.throws(() => {
                    child_process.execFileSync("sleep", ["10"], { timeout: 500 });
                });
            });

            it("custom killSignal", () => {
                var result = child_process.execFile("sleep", ["10"], {
                    timeout: 500,
                    killSignal: "SIGKILL"
                });
                assert.equal(result.exitCode, -9);
            });

            it("process completes before timeout", () => {
                var result = child_process.execFile("echo", ["done"], { timeout: 5000 });
                assert.equal(result.exitCode, 0);
                assert.equal(result.stdout.trim(), "done");
            });

            it("run with timeout", () => {
                var t = Date.now();
                var exitCode = child_process.run("sleep", ["10"], { timeout: 500 });
                var elapsed = Date.now() - t;
                assert.equal(exitCode, -15);
                assert.ok(elapsed < 2000);
            });

            it("run with custom killSignal", () => {
                var exitCode = child_process.run("sleep", ["10"], {
                    timeout: 500,
                    killSignal: "SIGKILL"
                });
                assert.equal(exitCode, -9);
            });

            it("run completes before timeout", () => {
                var exitCode = child_process.run("echo", ["done"], { timeout: 5000 });
                assert.equal(exitCode, 0);
            });

            it("spawn with timeout", () => {
                var t = Date.now();
                var cp = child_process.spawn("sleep", ["10"], { timeout: 500 });
                cp.join();
                var elapsed = Date.now() - t;
                assert.equal(cp.exitCode, -15);
                assert.ok(elapsed < 2000);
            });

            it("spawn with custom killSignal", () => {
                var cp = child_process.spawn("sleep", ["10"], {
                    timeout: 500,
                    killSignal: "SIGKILL"
                });
                cp.join();
                assert.equal(cp.exitCode, -9);
            });

            it("spawn completes before timeout", () => {
                var cp = child_process.spawn("echo", ["done"], { timeout: 5000 });
                cp.join();
                assert.equal(cp.exitCode, 0);
            });
        });
    }

    xit("stdin/stdout stream", () => {
        var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.chargeable.js')]);
        var stdout = new io.BufferedStream(bs.stdout);
        var outputs = []

        process.nextTick(() => {
            var oline = null

            while (true) {
                oline = stdout.readLine()
                if (oline === 'exit') {
                    break
                }

                if (oline) {
                    outputs.push(oline)
                }
            }
        });

        process.nextTick(() => {
            bs.stdin.write('line1' + os.EOL)
            bs.stdin.write('line2' + os.EOL)
            bs.stdin.write('.exit' + os.EOL)
        })

        bs.join()

        assert.deepEqual(
            outputs,
            [
                `> your input is: line1`,
                `> your input is: line2`,
            ]
        )
    });

    it("run", () => {
        assert.equal(child_process.run(cmd, [path.join(__dirname, 'process', 'exec.js')]), 100);
    });

    it("exitCode", () => {
        assert.equal(child_process.run(cmd, [path.join(__dirname, 'process', 'exec13.js')]), 100);
        assert.equal(child_process.run(cmd, [path.join(__dirname, 'process', 'exec14.js')]), 101);
    });

    it("run throw error", () => {
        assert.throws(() => {
            child_process.run("not_exists_exec_file");
        });
    });

    it("multi run", () => {
        coroutine.parallel([1, 2, 3, 4, 5, 6], (n) => {
            assert.equal(child_process.run(cmd, [path.join(__dirname, 'process', 'exec6.js'), n]), n);
        });
    });

    it("FIX: crash when bad stdio array", () => {
        child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.js')], {
            stdio: ['pipe', ,]
        });
    });

    describe('process holding', () => {
        it("multi fiber", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec7.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "100");
            p.join();
            assert.equal(p.exitCode, 7);
        });

        it("pendding callback", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec8.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "200");
            p.join();
            assert.equal(p.exitCode, 8);
        });

        it("setTimeout", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec9.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "300");
            p.join();
            assert.equal(p.exitCode, 9);
        });

        it("setTimeout unref", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec9.1.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "301");
            p.join();
            assert.equal(p.exitCode, 0);
        });

        it("setTimeout ref", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec9.2.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "302");
            p.join();
            assert.equal(p.exitCode, 9);
        });

        it("setInterval", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec10.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "400");
            p.join();
            assert.equal(p.exitCode, 10);
        });

        it("setImmediate", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec11.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "500");
            p.join();
            assert.equal(p.exitCode, 11);
        });

        it("websocket connect", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec18.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            p.join();
            assert.equal(p.exitCode, 81);
        });

        it("websocket disconnect", () => {
            var httpd = new http.Server(8899, {
                "/ws": WebSocket.upgrade((s) => {
                    s.onmessage = function (msg) {
                        s.send(msg);
                    };
                })
            });
            test_util.push(httpd.socket);
            httpd.start();

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec19.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "1900");
            p.join();
            assert.equal(p.exitCode, 19);
        });

        it("worker", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec20.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "2000");
            p.join();
            assert.equal(p.exitCode, 20);
        });

        it("bugfix: multi fiber async", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec12.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "600");
            p.join();
            assert.equal(p.exitCode, 12);
        });

        it("tcp server", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec21.js')]);
            assert.isNull(p.exitCode);
            var stdout = new io.BufferedStream(p.stdout);

            for (var i = 0; i < 100; i++) {
                coroutine.sleep(500);
                try {
                    net.connect('tcp://127.0.0.1:28080');
                    break;
                } catch (e) { }
            }

            assert.equal(stdout.readLine(), "700");
            p.join();
            assert.equal(p.exitCode, 21);
        });
    });

    it("spawn", () => {
        var t1 = new Date().getTime();
        child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.js')], {
            stdio: 'inherit'
        });
        assert.lessThan(new Date().getTime() - t1, 100);
    });

    it("kill", () => {
        var t1 = new Date().getTime();
        var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.js')], {
            stdio: 'inherit'
        });
        assert.isFalse(p.killed);
        coroutine.sleep(500);
        p.kill(15);
        assert.isTrue(p.killed);
        p.join();
        assert.lessThan(new Date().getTime() - t1, 2000);
    });

    (isIOS ? xit : it)("usage", () => {
        var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec22.js')]);
        var o = JSON.parse(p.stdout.read().toString());
        var o1 = p.usage();

        try {
            assert.closeTo(o.user, o1.user, 200000);
            assert.closeTo(o.system, o1.system, 200000);

            if (o.rss > 0)
                assert.closeTo(o.rss, o1.rss, o.rss / 2);
        } finally {
            p.kill(15);
            p.join();
        }
    });

    it("argv", () => {
        assert.deepEqual(json.decode(child_process.execFile(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "arg1",
            "arg2"
        ]).stdout), [
            cmd, path.join(__dirname, "process", "exec2.js"), "arg1", "arg2"
        ]);
    });

    it("spawnSync", () => {
        var result = child_process.spawnSync(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "arg1",
            "arg2"
        ]);

        assert.notEqual(result.pid, 0);

        assert.deepEqual(JSON.parse(result.stdout), [
            cmd, path.join(__dirname, "process", "exec2.js"), "arg1", "arg2"
        ]);

        assert.isNull(result.output[0]);
        assert.deepEqual(result.stdout, result.output[1]);
        assert.deepEqual(result.stderr, result.output[2]);
        assert.equal(result.status, 2);
        assert.equal(result.error, undefined);
    });

    it("spawnSync encoding", () => {
        var result = child_process.spawnSync(cmd, [path.join(__dirname, "process", "exec28.js")], {
            encoding: 'utf8'
        });

        assert.equal(result.stdout, result.output[1]);
        assert.equal(result.stderr, result.output[2]);

        if (isWin32) {
            assert.equal(result.stdout, "stdout output.\r\n");
            assert.equal(result.stderr, "stderr output.\r\n");
        } else {
            assert.equal(result.stdout, "stdout output.\n");
            assert.equal(result.stderr, "stderr output.\n");
        }
    });

    it("spawnSync env option", () => {
        var result = child_process.spawnSync(cmd, [path.join(__dirname, "process", "exec28.js")], {
            env: process.env
        });

        assert.equal(result.error, undefined);
    });

    it("inherit in spawnSync", () => {
        var result = child_process.spawnSync(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "arg1",
            "arg2"
        ], {
            stdio: "inherit"
        });

        assert.notEqual(result.pid, 0);
        assert.equal(result.stdout, null);
        assert.equal(result.stdout, null);
        assert.equal(result.stderr, result.output[1]);
        assert.equal(result.status, 2);
        assert.equal(result.error, undefined);
    });

    it("error when spawnSync", () => {
        var result = child_process.spawnSync("not_exists_exec_file");

        assert.equal(result.pid, 0);
        assert.equal(result.stdout, null);
        assert.equal(result.stdout, null);
        assert.equal(result.stderr, result.output[1]);
        assert.equal(result.status, 0);
        assert.notEqual(result.error, undefined);
    });

    it("argv 1", () => {
        assert.deepEqual(json.decode(child_process.execFile(cmd, [
            "--use_strict",
            "--test1",
            path.join(__dirname, "process", "exec2.js"),
            "arg1",
            "arg2"
        ]).stdout), [
            cmd, path.join(__dirname, "process", "exec2.js"), "arg1", "arg2"
        ]);
    });

    it("argv utf8", () => {
        assert.deepEqual(json.decode(child_process.execFile(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "参数1",
            "参数2"
        ]).stdout), [
            cmd, path.join(__dirname, "process", "exec2.js"), "参数1", "参数2"
        ]);
    });

    it("inherit in execFile", () => {
        assert.equal(child_process.execFile(cmd, [
            path.join(__dirname, "process", "exec2.js"),
            "参数1",
            "参数2"
        ], {
            stdio: "inherit"
        }).stdout, null);
    });

    it("execArgv", () => {
        assert.deepEqual(json.decode(child_process.execFile(cmd, [
            "--use_strict",
            "--test",
            path.join(__dirname, "process", "exec3.js"),
            "arg1",
            "arg2"
        ]).stdout), [
            "--use_strict",
            "--test",
        ]);
    });

    it("env", () => {
        process.env.abc = 123;

        assert.equal(json.decode(child_process.execFile(cmd, [
            path.join(__dirname, "process", "exec4.js")
        ]).stdout).abc, "123");
    });

    (isIOS ? xit : it)("env1", () => {
        var env = json.decode(child_process.execFile(cmd, [
            path.join(__dirname, "process", "exec4.js")
        ], {
            env: {
                QEMU_LD_PREFIX: process.env.QEMU_LD_PREFIX,
                abcd: "234"
            }
        }).stdout);

        assert.isUndefined(env.abc);
        assert.equal(env.abcd, "234");
    });

    if (process.platform != "win32") {
        xit("PATH env", () => {
            assert.equal(child_process.run("ls", [path.join(__dirname, "process")]), 0)
            assert.ok(child_process.execFile("ls", ["-a", path.join(__dirname, "process")]).stdout);
        });

        it("umask()", () => {
            const mask = '0664';
            const unmask = process.umask();
            // assert.equal(0o777 & ~unmask, 0o755);

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
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec15.js')]);
            var stdout = new io.BufferedStream(bs.stdout);
            assert.deepEqual(stdout.readLines(), [
                "beforeExit 101",
                "other beforeExit 101",
                "new work 101",
                "beforeExit 101",
                "other beforeExit 101"
            ]);

            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec16.js')]);
            var stdout = new io.BufferedStream(bs.stdout);
            assert.deepEqual(stdout.readLines(), []);
        });

        it("exit", () => {
            var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec17.js')]);
            var stdout = new io.BufferedStream(bs.stdout);
            assert.deepEqual(stdout.readLines(), [
                "exit 101",
                "other exit 101"
            ]);
        });

        it("spawn", () => {
            var spawnEventTriggered = false;
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_spawn_event.js')]);

            p.on('spawn', () => {
                spawnEventTriggered = true;
            });

            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "spawn event test process started");

            p.join();
            assert.equal(p.exitCode, 0);
            assert.equal(spawnEventTriggered, true);
        });

        it("spawn event timing", () => {
            var spawnEventTriggered = false;
            var spawnEventTime = 0;
            var processStartTime = new Date().getTime();

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_spawn_timing.js')]);

            p.on('spawn', () => {
                spawnEventTriggered = true;
                spawnEventTime = new Date().getTime();
            });

            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "process started");

            // spawn event should have been triggered by now
            assert.equal(spawnEventTriggered, true);

            // spawn event should be triggered quickly after process creation
            assert.lessThan(spawnEventTime - processStartTime, 1000);

            assert.equal(stdout.readLine(), "process ending");
            p.join();
            assert.equal(p.exitCode, 42);
        });

        it("spawn event with fork", () => {
            var spawnEventTriggered = false;
            var p = child_process.fork(path.join(__dirname, 'process', 'exec_spawn_event.js'), {
                stdio: "pipe"
            });

            p.on('spawn', () => {
                spawnEventTriggered = true;
            });

            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "spawn event test process started");

            p.join();
            assert.equal(p.exitCode, 0);
            assert.equal(spawnEventTriggered, true);
        });

        it("spawn event with multiple listeners", () => {
            var spawnCount = 0;
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_spawn_event.js')]);

            p.on('spawn', () => {
                spawnCount++;
            });

            p.on('spawn', () => {
                spawnCount++;
            });

            var stdout = new io.BufferedStream(p.stdout);
            assert.equal(stdout.readLine(), "spawn event test process started");

            p.join();
            assert.equal(p.exitCode, 0);
            assert.equal(spawnCount, 2); // Both listeners should be called
        });

        it("spawn event should not trigger on failed spawn", () => {
            var spawnEventTriggered = false;

            try {
                var p = child_process.spawn("non_existent_command");

                p.on('spawn', () => {
                    spawnEventTriggered = true;
                });

                p.join();
            } catch (e) {
                // Expected to fail
            }

            // spawn event should not be triggered for failed process creation
            assert.equal(spawnEventTriggered, false);
        });

        it("close event with pipe stdio", () => {
            var closeEventTriggered = false;
            var closeCode = null;
            var closeSignal = null;

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_event.js')], {
                stdio: 'pipe'
            });

            p.on('close', (code, signal) => {
                closeEventTriggered = true;
                closeCode = code;
                closeSignal = signal;
            });

            p.join();
            p.stdout.close(); // Close stdout to trigger close event
            coroutine.sleep(100); // Allow time for close event to be processed

            // close event should be triggered
            assert.equal(closeEventTriggered, true);
            assert.equal(closeCode, 42);
            assert.equal(closeSignal, null);
        });

        it("close event with inherit stdio", () => {
            var closeEventTriggered = false;
            var closeCode = null;

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_immediate.js')], {
                stdio: 'inherit'
            });

            p.on('close', (code, signal) => {
                closeEventTriggered = true;
                closeCode = code;
            });

            p.join();
            coroutine.sleep(100); // Allow time for close event to be processed

            // close event should be triggered
            assert.equal(closeEventTriggered, true);
            assert.equal(closeCode, 123);
        });

        it("close event with ipc stdio", () => {
            var closeEventTriggered = false;
            var closeCode = null;
            var messageReceived = false;

            var p = child_process.fork(path.join(__dirname, 'process', 'exec_close_ipc.js'), {
                stdio: 'pipe'
            });

            p.on('close', (code, signal) => {
                closeEventTriggered = true;
                closeCode = code;
            });

            p.on('message', (msg) => {
                if (msg === 'ready') {
                    messageReceived = true;
                    // Send exit message after receiving ready
                    setTimeout(() => {
                        p.send('exit');
                    }, 10);
                }
            });

            p.join();
            p.stdout.close(); // Close stdout to trigger close event
            coroutine.sleep(100); // Allow time for close event to be processed

            assert.equal(messageReceived, true);

            // close event should be triggered
            assert.equal(closeEventTriggered, true);
            assert.equal(closeCode, 0);
        });

        it("close event timing", () => {
            var closeEventTime = 0;
            var processEndTime = 0;

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_immediate.js')]);

            p.on('close', () => {
                closeEventTime = new Date().getTime();
            });

            p.join();
            p.stdout.close(); // Close stdout to trigger close event
            coroutine.sleep(100); // Allow time for close event to be processed
            processEndTime = new Date().getTime();

            // close event should be triggered before or at the same time as join() returns
            assert.notGreaterThan(closeEventTime, processEndTime);
        });

        it("close event with multiple listeners", () => {
            var closeCount = 0;
            var totalCode = 0;

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_immediate.js')]);

            p.on('close', (code) => {
                closeCount++;
                totalCode += code;
            });

            p.on('close', (code) => {
                closeCount++;
                totalCode += code;
            });

            p.join();
            p.stdout.close(); // Close stdout to trigger close event
            coroutine.sleep(100); // Allow time for close event to be processed

            // Both listeners should be called
            assert.equal(closeCount, 2);
            assert.equal(totalCode, 246); // 123 * 2
        });

        it("close event without reading stdout", () => {
            // This test verifies that the close event fires even when stdout.on('data') is not listened
            var closeEventTriggered = false;
            var closeCode = null;

            var p = child_process.spawn('echo', ['hello']);

            // Only listen for close event, NOT stdout.on('data')
            p.on('close', (code, signal) => {
                closeEventTriggered = true;
                closeCode = code;
            });

            coroutine.sleep(500); // Allow time for process to complete and close event to fire

            // close event should be triggered even without reading stdout
            assert.equal(closeEventTriggered, true);
            assert.equal(closeCode, 0);
        });


        it("exit event fires without close listener", () => {
            var exitTriggered = false;

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_immediate.js')], {
                stdio: 'pipe'
            });

            p.on('exit', (code) => {
                exitTriggered = true;
            });

            // Don't listen to close event

            p.join();
            coroutine.sleep(100);

            // exit event should fire even without close listener
            assert.equal(exitTriggered, true);
        });

        it("stdout readable after join", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec2.js'), 'arg1', 'arg2'], {
                stdio: 'pipe'
            });

            p.join();

            // stdout should still be accessible after join
            assert.notEqual(p.stdout, null);

            // Should be able to read data
            var data = p.stdout.read();
            assert.notEqual(data, null);

            var result = JSON.parse(data.toString());
            assert.equal(result.length, 4);
        });

        it("exit and close event order", () => {
            var events = [];

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_immediate.js')], {
                stdio: 'pipe'
            });

            p.on('exit', (code) => {
                events.push('exit');
            });

            p.on('close', (code) => {
                events.push('close');
            });

            // Wait for both events, check every 100ms, up to 30 times (3 seconds)
            for (var i = 0; i < 30 && events.length < 2; i++) {
                coroutine.sleep(100);
            }

            // exit should come before close
            assert.equal(events[0], 'exit');
            assert.equal(events[1], 'close');
        });

        it("close stderr does not trigger close event without close listener", () => {
            var exitTriggered = false;

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_immediate.js')], {
                stdio: 'pipe'
            });

            p.on('exit', () => {
                exitTriggered = true;
            });

            p.join();
            p.stderr.close(); // Close stderr instead of stdout
            coroutine.sleep(100);

            // exit event should trigger, but close event was not listened
            assert.equal(exitTriggered, true);
        });

        it("multiple stdout.close calls are safe", () => {
            var closeCount = 0;

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_immediate.js')], {
                stdio: 'pipe'
            });

            p.on('close', () => {
                closeCount++;
            });

            p.join();
            p.stdout.close();
            p.stdout.close(); // Second close should be safe
            p.stdout.close(); // Third close should be safe
            coroutine.sleep(100);

            // close event should only fire once
            assert.equal(closeCount, 1);
        });

        it("on data mode triggers close event automatically", () => {
            var dataReceived = false;
            var closeTriggered = false;
            var stdoutClosed = false;

            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_close_immediate.js')], {
                stdio: 'pipe'
            });

            p.stdout.on('data', (data) => {
                dataReceived = true;
            });

            p.stdout.on('close', () => {
                stdoutClosed = true;
            });

            p.on('close', (code) => {
                closeTriggered = true;
            });

            p.join();
            coroutine.sleep(100);

            // When using on('data'), close events should fire automatically
            assert.equal(stdoutClosed, true);
            assert.equal(closeTriggered, true);
        });

        if (process.platform != "win32")
            it("SIGINT", () => {
                var bs = child_process.spawn(cmd, [path.join(__dirname, 'process', 'signal1.js')]);
                var stdout = new io.BufferedStream(bs.stdout);

                setImmediate(() => {
                    coroutine.sleep(1000);
                    bs.kill('SIGINT');
                    coroutine.sleep(1000);
                    bs.kill('SIGINT');
                });

                assert.deepEqual(stdout.readLines(), [
                    "SIGINT received",
                    "SIGINT received"
                ]);
            });
    });

    describe("ipc", () => {
        it("init variable", () => {
            var cp = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec25.js')], {
                "stdio": ['inherit', 'inherit', 'inherit']
            });
            cp.join();
            assert.equal(cp.exitCode, 1);
        });

        it("replace stdio", () => {
            var cp = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec25.js')], {
                "stdio": ['ipc', 'inherit', 'inherit']
            });
            assert.equal(cp.stdin, null);

            var cp = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec25.js')], {
                "stdio": ['inherit', 'ipc', 'inherit']
            });
            assert.equal(cp.stdout, null);

            var cp = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec25.js')], {
                "stdio": ['inherit', 'inherit', 'ipc']
            });
            assert.equal(cp.stderr, null);
        });

        it("can have only one IPC pipe", () => {
            assert.throws(() => {
                var n = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec1.js')], {
                    "stdio": ['ipc', 'ipc', 'inherit']
                });
            });
        });

        it("hold process on message", () => {
            var p = child_process.fork(path.join(__dirname, 'process', 'exec23.js'));
            setTimeout(() => {
                p.send(1);
            }, 1);
            p.join();
            assert.equal(p.exitCode, 12);
        });

        it("send message", () => {
            var k;
            var p = child_process.fork(path.join(__dirname, 'process', 'exec24.js'));
            p.on("message", m => {
                if (m == 100)
                    k = true;
            });

            p.send(100);

            for (var i = 0; i < 10000 && !k; i++)
                coroutine.sleep(1);

            assert.equal(k, true);
        });

        it("disconnect", () => {
            var cp = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec25.1.js')], {
                "stdio": ['ipc', 'inherit', 'inherit']
            });

            assert.equal(cp.connected, true);

            cp.send(100);

            cp.join();

            for (var i = 0; i < 100 && cp.connected; i++)
                coroutine.sleep(1);

            assert.equal(cp.connected, false);
            assert.equal(cp.exitCode, 1);
        });

        it("grandson process", () => {
            var k;
            var cp = child_process.fork(path.join(__dirname, 'process', 'exec27.js'));
            cp.join();

            assert.equal(cp.exitCode, 1);
        });
    });

    // leave here to tuning manually
    xit("print child process's env items", () => {
        var retcode = child_process.run(cmd, [path.join(__dirname, 'process', 'exec.print_kvs.js')]);
        assert.equal(retcode, 0)
    });

    (isIOS ? describe.skip : describe)("signal option", () => {
        it("spawn with already aborted signal", () => {
            var controller = new AbortController();
            controller.abort();

            var result = child_process.spawn(cmd, [
                path.join(__dirname, "process", "exec_signal_test.js")
            ], {
                signal: controller.signal
            });

            assert.notEqual(result.pid, 0);
            result.join();
            assert.strictEqual(result.killed, true);
        });

        it("spawn with signal aborted during execution", () => {
            var controller = new AbortController();

            // Abort the signal after a short delay
            setTimeout(() => {
                controller.abort();
            }, 100);

            var result = child_process.spawn(cmd, [
                path.join(__dirname, "process", "exec_long_running.js")
            ], {
                signal: controller.signal
            });

            assert.notEqual(result.pid, 0);
            assert.strictEqual(result.exitCode, null);
            result.join();
            assert.strictEqual(result.killed, true);
        });

        it("spawn with signal never aborted", () => {
            var controller = new AbortController();

            var result = child_process.spawn(cmd, [
                path.join(__dirname, "process", "exec2.js"),
                "arg1",
                "arg2"
            ], {
                signal: controller.signal
            });

            // Normal execution should work fine
            assert.notEqual(result.pid, 0);
            assert.strictEqual(result.exitCode, null);
            result.join();
            assert.strictEqual(result.killed, false);
            assert.deepEqual(JSON.parse(result.stdout.read().toString()), [
                cmd, path.join(__dirname, "process", "exec2.js"), "arg1", "arg2"
            ]);
        });

        it("spawn signal abort sends SIGTERM to child process", () => {
            var controller = new AbortController();

            // Start a long-running process and abort it
            setTimeout(() => {
                controller.abort();
            }, 100);

            var result = child_process.spawn(cmd, [
                path.join(__dirname, "process", "exec_long_running.js")
            ], {
                signal: controller.signal,
                encoding: 'utf8'
            });

            assert.notEqual(result.pid, 0);
            assert.strictEqual(result.exitCode, null);
            result.join();
            assert.strictEqual(result.killed, true);
        });

        it("spawn with signal option encoding", () => {
            var controller = new AbortController();
            controller.abort();

            var result = child_process.spawn(cmd, [
                path.join(__dirname, "process", "exec_signal_test.js")
            ], {
                signal: controller.signal,
                encoding: 'utf8'
            });

            assert.notEqual(result.pid, 0);
            result.join();
            assert.strictEqual(result.killed, true);
        });
    });

    (isIOS ? xit : it)("unref", () => {
        var t1 = new Date().getTime();
        // Start the main script that will spawn child process and call unref
        var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec.unref_main.js')], {
            stdio: 'pipe'
        });

        var stdout = new io.BufferedStream(p.stdout);
        var output = stdout.readLines();

        // Find "main process exit" message
        var mainExitIndex = -1;
        for (var i = 0; i < output.length; i++) {
            if (output[i] === "main process exit") {
                mainExitIndex = i;
                break;
            }
        }

        // Verify that "main process exit" message exists
        assert.notEqual(mainExitIndex, -1, "main process exit message should be found");

        // Verify that "main process exit" is not the first output
        assert.greaterThan(mainExitIndex, 0, "main process exit should not be the first output");

        // Verify that "main process exit" is not the last output
        assert.lessThan(mainExitIndex, output.length - 1, "main process exit should not be the last output");

        var hasChildOutput = false;
        for (var i = 0; i < output.length; i++) {
            if (output[i].includes("sub process running")) {
                hasChildOutput = true;
                break;
            }
        }
        assert.isTrue(hasChildOutput, "should have child process output");
    });
});

