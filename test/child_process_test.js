var { describe, it, xit, before, after } = require('node:test');
var assert = require('assert');

var test_util = require('./test_util');

const child_process = require('child_process');
var coroutine = require("coroutine");
var fs = require('fs');
var path = require('path');
var json = require('json');
var net = require('net');
var http = require('http');
var io = require('io');
var os = require('os');

const isWin32 = process.platform === "win32";
const isIOS = process.platform === "ios";
const supportsPosixStdioFd = !isWin32 && !isIOS;

var envKeys = require('./process/const.env_keys.js');

function getFdCount() {
    if (isWin32 || isIOS)
        return null;

    try {
        return fs.readdir('/dev/fd').length;
    } catch (e) {
        return null;
    }
}

describe("child_process", () => {
    var cmd;

    before(() => {
        cmd = process.execPath;
    });

    after(test_util.cleanup);

    it("fibjs --version output via pipe (not TTY)", () => {
        // When stdout is a pipe (not TTY), printf/puts use full buffering.
        // If --version or --help calls _exit() without fflush(stdout),
        // the output buffer is lost and the command produces no output.
        var ret = child_process.execFileSync(cmd, ["--version"]);
        var version = ret.toString().trim();
        assert.ok(version.length > 0, "fibjs --version should output version via pipe, got: " + JSON.stringify(version));
        assert.ok(version.startsWith("v"), "fibjs --version output should start with 'v', got: " + version);
    });

    it("fibjs -v output via pipe (not TTY)", () => {
        var ret = child_process.execFileSync(cmd, ["-v"]);
        var version = ret.toString().trim();
        assert.ok(version.length > 0, "fibjs -v should output version via pipe, got: " + JSON.stringify(version));
        assert.ok(version.startsWith("v"), "fibjs -v output should start with 'v', got: " + version);
    });

    it("fibjs --help output via pipe (not TTY)", () => {
        var ret = child_process.execFileSync(cmd, ["--help"]);
        var help = ret.toString().trim();
        assert.ok(help.length > 0, "fibjs --help should output help text via pipe, got: " + JSON.stringify(help));
        assert.ok(help.indexOf("Usage") >= 0, "fibjs --help should contain 'Usage'");
    });

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
                    assert.equal(stdout.readLine(), "PTY_TEST_OUTPUT");
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

    describe("fork silent option", () => {
        var silentChild = path.join(__dirname, 'process', 'exec_silent.js');
        var silentHarness = path.join(__dirname, 'process', 'exec_silent_harness.js');

        // inherit 模式的用例不需要子进程打印，加静默标记避免污染套件日志
        function quietEnv() {
            var env = {};
            Object.keys(process.env).forEach(k => env[k] = process.env[k]);
            env.CP_SILENT_QUIET = '1';
            return env;
        }

        it("silent:true pipes child stdio", () => {
            var p = child_process.fork(silentChild, { silent: true });

            // silent:true 时 stdin/stdout/stderr 都是管道
            assert.exist(p.stdin);
            assert.exist(p.stdout);
            assert.exist(p.stderr);

            var stdout = new io.BufferedStream(p.stdout);
            var stderr = new io.BufferedStream(p.stderr);

            assert.deepEqual(stdout.readLines(), [
                "silent-out-1",
                "silent-out-2"
            ]);
            assert.deepEqual(stderr.readLines(), [
                "silent-err-1",
                "silent-err-2"
            ]);

            p.join();
            assert.equal(p.exitCode, 0);
        });

        it("silent:true child output is not written to parent stdio", () => {
            // 通过 harness 监控父进程侧的输出：harness 以 silent:true fork 子进程，
            // 若 silent 失效，子进程的输出会原样泄漏到 harness 的 stdout/stderr，
            // 外层 spawnSync 捕获后即可发现。harness 自身只上报 base64 编码的
            // 管道内容（HARNESS-OUT/HARNESS-ERR），因此任何非 HARNESS- 行都
            // 只能是泄漏的子进程输出。
            var r = child_process.spawnSync(cmd, [silentHarness]);
            assert.equal(r.status, 0, "harness 应正常退出（silent 失效时退出码为 2）");

            // fibjs 同步 API 的空输出为 null（node 为 ''），统一按空串处理
            var out = r.stdout == null ? '' : r.stdout.toString('utf8');
            var err = r.stderr == null ? '' : r.stderr.toString('utf8');

            // stderr 必须为空：子进程的任何 stderr 泄漏都会出现在这里
            assert.equal(err, '', "子进程 stderr 泄漏到了父进程 stderr: " + JSON.stringify(err));

            // stdout 只允许 HARNESS-OUT:/HARNESS-ERR: 两行报告
            var lines = out.split('\n').filter(l => l.length > 0);
            var leaked = lines.filter(l => !/^HARNESS-(OUT|ERR):[A-Za-z0-9+/=]*$/.test(l));
            assert.deepEqual(leaked, [], "子进程输出泄漏到了父进程 stdout: " + JSON.stringify(leaked));

            // 管道内容应完整到达（base64 解码后与预期一致）
            var payload = { OUT: '', ERR: '' };
            lines.forEach(l => {
                var m = /^HARNESS-(OUT|ERR):(.*)$/.exec(l);
                if (m)
                    payload[m[1]] += m[2];
            });
            var outText = Buffer.from(payload.OUT, 'base64').toString('utf8');
            var errText = Buffer.from(payload.ERR, 'base64').toString('utf8');
            assert.deepEqual(outText.split('\n').filter(l => l.length > 0), ["silent-out-1", "silent-out-2"]);
            assert.deepEqual(errText.split('\n').filter(l => l.length > 0), ["silent-err-1", "silent-err-2"]);
        });

        it("default fork inherits stdio", () => {
            var p = child_process.fork(silentChild, { env: quietEnv() });

            assert.equal(p.stdin, null);
            assert.equal(p.stdout, null);
            assert.equal(p.stderr, null);

            p.join();
            assert.equal(p.exitCode, 0);
        });

        it("silent:false inherits stdio", () => {
            var p = child_process.fork(silentChild, { silent: false, env: quietEnv() });

            assert.equal(p.stdin, null);
            assert.equal(p.stdout, null);
            assert.equal(p.stderr, null);

            p.join();
            assert.equal(p.exitCode, 0);
        });

        it("explicit stdio overrides silent", () => {
            var p = child_process.fork(silentChild, {
                silent: true,
                stdio: 'inherit',
                env: quietEnv()
            });

            // Node 语义：提供 stdio 时 silent 不生效
            assert.equal(p.stdin, null);
            assert.equal(p.stdout, null);
            assert.equal(p.stderr, null);

            p.join();
            assert.equal(p.exitCode, 0);
        });

        it("silent:true keeps ipc channel working", () => {
            var p = child_process.fork(silentChild, { silent: true });
            var got = false;

            p.on('message', m => {
                if (m === 'ready')
                    got = true;
            });

            for (var i = 0; i < 2000 && !got; i++)
                coroutine.sleep(1);

            assert.equal(got, true);

            p.join();
            assert.equal(p.exitCode, 0);
        });
    });

    const isAndroid = process.platform === 'android';
    
    if (!process.env.QEMU_LD_PREFIX && !isAndroid) {
        it("exec with input", () => {
            var ret = child_process.exec("cat", { input: "hello from exec\n" });
            assert.equal(ret.stdout, "hello from exec\n");

            var ret = child_process.exec("cat | tr a-z A-Z", { input: "lowercase\n" });
            assert.equal(ret.stdout, "LOWERCASE\n");
        });

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

        it("execSync with input", () => {
            // string input
            var ret = child_process.execSync("cat", { input: "hello from execSync\n" });
            assert.ok(Buffer.isBuffer(ret));
            assert.equal(ret.toString(), "hello from execSync\n");

            // Buffer input
            var ret = child_process.execSync("cat", { input: Buffer.from("buffer execSync\n") });
            assert.ok(Buffer.isBuffer(ret));
            assert.equal(ret.toString(), "buffer execSync\n");

            // input to shell command via stdin
            var ret = child_process.execSync("cat | tr a-z A-Z", { input: "lowercase\n" });
            assert.ok(Buffer.isBuffer(ret));
            assert.equal(ret.toString(), "LOWERCASE\n");
        });

        it("execSync", () => {
            // Test successful execSync
            var ret = child_process.execSync("echo hello");
            assert.ok(Buffer.isBuffer(ret));
            assert.equal(ret.toString(), "hello" + os.EOL);

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
                assert.ok(Buffer.isBuffer(error.stderr));
                var stderrStr = error.stderr.toString();
                assert.ok(stderrStr.includes("command not found") ||
                    stderrStr.includes("not found") ||
                    stderrStr.includes("not recognized"));
                return true;
            });

            if (isWin32) {
                var ret = child_process.execSync(`echo "hello world"`);
                assert.ok(Buffer.isBuffer(ret));
                assert.equal(ret.toString(), `"hello world"\r\n`);
            } else {
                var ret = child_process.execSync(`echo "hello world"`);
                assert.ok(Buffer.isBuffer(ret));
                assert.equal(ret.toString(), `hello world\n`);
            }
        });

        it("execFileSync with input", () => {
            // string input
            var ret = child_process.execFileSync("cat", [], { input: "hello from execFileSync\n" });
            assert.ok(Buffer.isBuffer(ret));
            assert.equal(ret.toString(), "hello from execFileSync\n");

            // Buffer input
            var ret = child_process.execFileSync("cat", [], { input: Buffer.from("buffer execFileSync\n") });
            assert.ok(Buffer.isBuffer(ret));
            assert.equal(ret.toString(), "buffer execFileSync\n");

            // empty input
            var ret = child_process.execFileSync("cat", [], { input: "" });
            assert.equal(ret, null);

            // input without args array
            var ret = child_process.execFileSync("cat", { input: "no args array\n" });
            assert.ok(Buffer.isBuffer(ret));
            assert.equal(ret.toString(), "no args array\n");
        });

        it("execFileSync", () => {
            // Test successful execFileSync
            var ret = child_process.execFileSync(cmd, [
                path.join(__dirname, "process", "exec_sync_success.js")
            ]);
            assert.ok(Buffer.isBuffer(ret));
            assert.equal(ret.toString(), "execSync success output\n");

            // Test execFileSync with encoding
            var ret = child_process.execFileSync(cmd, [
                path.join(__dirname, "process", "exec_sync_success.js")
            ], { encoding: 'utf8' });
            assert.equal(ret, "execSync success output\n");

            // Test execFileSync with arguments
            var ret = child_process.execFileSync(cmd, [
                path.join(__dirname, "process", "exec_file_sync.js"),
                "arg1", "arg2"
            ]);
            assert.ok(Buffer.isBuffer(ret));
            assert.ok(ret.toString().includes('["arg1","arg2"]'));

            // Test execFileSync error case
            assert.throws(() => {
                child_process.execFileSync(cmd, [
                    path.join(__dirname, "process", "exec_sync_error.js")
                ]);
            }, (error) => {
                assert.equal(error.status, 1);
                assert.equal(error.signal, null);
                assert.ok(Buffer.isBuffer(error.stdout));
                assert.ok(Buffer.isBuffer(error.stderr));
                assert.ok(error.stdout.toString().includes("execSync stdout before error"));
                assert.ok(error.stderr.toString().includes("execSync stderr error message"));
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
                assert.ok(Buffer.isBuffer(error.stdout));
                assert.ok(Buffer.isBuffer(error.stderr));
                assert.ok(error.stdout.toString().includes("execFileSync stdout"));
                assert.ok(error.stderr.toString().includes("execFileSync stderr"));
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
                    assert.ok(Buffer.isBuffer(error.stdout));
                    var env = json.decode(error.stdout.toString());
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
                assert.ok(elapsed < 10000);
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
                assert.ok(elapsed < 10000);
            });

            it("spawnSync with timeout", () => {
                var t = Date.now();
                var result = child_process.spawnSync("sleep", ["10"], { timeout: 500 });
                var elapsed = Date.now() - t;
                assert.equal(result.signal, "SIGTERM");
                assert.equal(result.status, 0);
                assert.ok(elapsed < 10000);
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
                assert.ok(elapsed < 10000);
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
                assert.ok(elapsed < 10000);
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

        assert.equal(result.stdout, "stdout output.\n");
        assert.equal(result.stderr, "stderr output.\n");
    });

    it("spawnSync env option", () => {
        var result = child_process.spawnSync(cmd, [path.join(__dirname, "process", "exec28.js")], {
            env: process.env
        });

        assert.equal(result.error, undefined);
    });

    (supportsPosixStdioFd ? it : xit)("spawnSync numeric stdio fds", () => {
        var tempDir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-spawnsync-fd-'));
        var stdoutPath = path.join(tempDir, 'stdout.log');
        var stderrPath = path.join(tempDir, 'stderr.log');
        var stdoutHandle = fs.open(stdoutPath, 'w');
        var stderrHandle = fs.open(stderrPath, 'w');

        try {
            var result = child_process.spawnSync(cmd, [path.join(__dirname, "process", "exec28.js")], {
                stdio: ['ignore', stdoutHandle.fd, stderrHandle.fd]
            });

            assert.equal(result.error, undefined);
            assert.isNull(result.stdout);
            assert.isNull(result.stderr);
            assert.equal(fs.readTextFile(stdoutPath), 'stdout output.\n');
            assert.equal(fs.readTextFile(stderrPath), 'stderr output.\n');
        } finally {
            fs.close(stdoutHandle);
            fs.close(stderrHandle);
            fs.unlink(stdoutPath);
            fs.unlink(stderrPath);
            fs.rmdir(tempDir);
        }
    });

    (supportsPosixStdioFd ? it : xit)("spawnSync numeric stdio fds do not leak", () => {
        var baseFdCount = getFdCount();
        if (baseFdCount === null)
            return;

        var tempDir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-spawnsync-fd-leak-'));

        try {
            for (var i = 0; i < 25; i++) {
                var stdoutPath = path.join(tempDir, 'stdout-' + i + '.log');
                var stderrPath = path.join(tempDir, 'stderr-' + i + '.log');
                var stdoutHandle = fs.open(stdoutPath, 'w');
                var stderrHandle = fs.open(stderrPath, 'w');

                try {
                    var result = child_process.spawnSync(cmd, [path.join(__dirname, "process", "exec28.js")], {
                        stdio: ['ignore', stdoutHandle.fd, stderrHandle.fd]
                    });

                    assert.equal(result.error, undefined);
                } finally {
                    fs.close(stdoutHandle);
                    fs.close(stderrHandle);
                    fs.unlink(stdoutPath);
                    fs.unlink(stderrPath);
                }
            }

            var finalFdCount = getFdCount();
            assert.ok(finalFdCount - baseFdCount <= 4,
                'spawnSync leaked file descriptors: base=' + baseFdCount + ', final=' + finalFdCount);
        } finally {
            fs.rmdir(tempDir);
        }
    });

    (supportsPosixStdioFd ? it : xit)("execSync does not leak stdio pipes", () => {
        var baseFdCount = getFdCount();
        if (baseFdCount === null)
            return;

        for (var i = 0; i < 25; i++)
            child_process.execSync("true");

        var finalFdCount = getFdCount();
        assert.ok(finalFdCount - baseFdCount <= 4,
            'execSync leaked file descriptors: base=' + baseFdCount + ', final=' + finalFdCount);
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

    it("spawnSync with string input", () => {
        var result = child_process.spawnSync("cat", [], {
            input: "hello from input\n",
            encoding: "utf8"
        });

        assert.equal(result.status, 0);
        assert.equal(result.signal, null);
        assert.equal(result.stdout, "hello from input\n");
    });

    it("spawnSync with Buffer input", () => {
        var result = child_process.spawnSync("cat", [], {
            input: Buffer.from("buffer input\n"),
            encoding: "utf8"
        });

        assert.equal(result.status, 0);
        assert.equal(result.stdout, "buffer input\n");
    });

    it("spawnSync with empty input", () => {
        var result = child_process.spawnSync("cat", [], {
            input: "",
            encoding: "utf8"
        });

        assert.equal(result.status, 0);
        assert.equal(result.stdout, "");
    });

    it("spawnSync input to shell via stdin", () => {
        var result = child_process.spawnSync("sh", [], {
            input: "echo hello\nexit 42\n",
            encoding: "utf8"
        });

        assert.equal(result.stdout, "hello\n");
        assert.equal(result.status, 42);
        assert.equal(result.signal, null);
    });

    it("spawnSync signal field is null on normal exit", () => {
        var result = child_process.spawnSync("true", [], {
            encoding: "utf8"
        });

        assert.equal(result.status, 0);
        assert.equal(result.signal, null);
    });

    it("spawnSync signal field on timeout kill", () => {
        var result = child_process.spawnSync("sleep", ["10"], {
            timeout: 500
        });

        assert.equal(result.signal, "SIGTERM");
        assert.equal(result.status, 0);
    });

    it("spawnSync empty stdout/stderr with encoding returns empty string", () => {
        var result = child_process.spawnSync("true", [], {
            encoding: "utf8"
        });

        assert.equal(result.status, 0);
        assert.strictEqual(result.stdout, "");
        assert.strictEqual(result.stderr, "");
    });

    it("spawnSync empty stdout/stderr without encoding returns Buffer", () => {
        var result = child_process.spawnSync(cmd, [
            path.join(__dirname, "process", "exec28.js")
        ]);

        assert.ok(Buffer.isBuffer(result.stdout));
        assert.ok(Buffer.isBuffer(result.stderr));
    });

    it("execFile with string input", () => {
        var ret = child_process.execFile("cat", [], { input: "hello from execFile\n" });
        assert.equal(ret.stdout, "hello from execFile\n");
    });

    it("execFile with Buffer input", () => {
        var ret = child_process.execFile("cat", [], { input: Buffer.from("buffer execFile\n") });
        assert.equal(ret.stdout, "buffer execFile\n");
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

    it("argv relative path resolved to absolute", () => {
        var relPath = path.relative(process.cwd(), path.join(__dirname, "process", "exec2.js"));
        var absPath = path.join(__dirname, "process", "exec2.js");
        assert.deepEqual(json.decode(child_process.execFile(cmd, [
            relPath,
            "arg1",
            "arg2"
        ]).stdout), [
            cmd, absPath, "arg1", "arg2"
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
            "--use_strict"
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
            coroutine.sleep(100); // Allow time for close event to be processed

            // Both listeners should be called
            assert.equal(closeCount, 2);
            assert.equal(totalCode, 246); // 123 * 2
        });

        it("close event without reading stdout", () => {
            // This test verifies that the close event fires even when stdout.on('data') is not listened
            var closeEventTriggered = false;
            var closeCode = null;

            var p = process.platform === 'win32'
                ? child_process.spawn('cmd', ['/c', 'echo', 'hello'])
                : child_process.spawn('echo', ['hello']);

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

    // Node-compatible exit/close event args when the child is killed by a signal.
    // Node baseline: both events emit (null, signalName). fibjs used to emit
    // (0, signalName) on 'exit' (code 0 masked the crash) and (-11, signalName)
    // on 'close' (negative signal value as code).
    (process.platform == "win32" || isIOS ? describe.skip : describe)("signal death", () => {
        function spawnKillMe() {
            return child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_kill_me.js')], {
                stdio: 'ignore'
            });
        }

        function captureEvents(p) {
            var events = [];
            p.on('exit', (code, signal) => events.push(['exit', code, signal]));
            p.on('close', (code, signal) => events.push(['close', code, signal]));
            return events;
        }

        function waitBothEvents(events) {
            for (var i = 0; i < 100 && events.length < 2; i++)
                coroutine.sleep(10);
            return events;
        }

        it("normal exit 0 reports (0, null) on exit and close", () => {
            var p = child_process.spawn(cmd, [path.join(__dirname, 'process', 'exec_spawn_event.js')], {
                stdio: 'ignore'
            });
            var events = captureEvents(p);
            p.join();
            assert.deepEqual(waitBothEvents(events), [
                ['exit', 0, null],
                ['close', 0, null]
            ]);
        });

        it("custom exit code 124 reports (124, null) on exit and close", () => {
            var p = child_process.spawn(cmd, ['-e', 'process.exit(124)'], {
                stdio: 'ignore'
            });
            var events = captureEvents(p);
            p.join();
            assert.deepEqual(waitBothEvents(events), [
                ['exit', 124, null],
                ['close', 124, null]
            ]);
        });

        it("killed by SIGKILL reports (null, SIGKILL) on exit and close", () => {
            var p = spawnKillMe();
            var events = captureEvents(p);
            coroutine.sleep(200); // Ensure the child is running
            p.kill('SIGKILL');
            p.join();
            assert.deepEqual(waitBothEvents(events), [
                ['exit', null, 'SIGKILL'],
                ['close', null, 'SIGKILL']
            ]);
        });

        it("killed by SIGSEGV reports (null, SIGSEGV) on exit and close", () => {
            var p = spawnKillMe();
            var events = captureEvents(p);
            coroutine.sleep(200); // Ensure the child is running
            p.kill('SIGSEGV');
            p.join();
            assert.deepEqual(waitBothEvents(events), [
                ['exit', null, 'SIGSEGV'],
                ['close', null, 'SIGSEGV']
            ]);
        });

        it("join returns negative signal code for signal death", () => {
            var p = spawnKillMe();
            coroutine.sleep(200); // Ensure the child is running
            p.kill('SIGKILL');
            assert.equal(p.join(), -9);
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

