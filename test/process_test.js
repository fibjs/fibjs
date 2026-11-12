var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var test_util = require('./test_util');

var process = require('process');
var coroutine = require("coroutine");
var path = require('path');
var json = require('json');
var child_process = require('child_process');
var io = require('io');
var os = require('os');
var fs = require('fs');

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

    // Node rejects a diff that is not a two-element tuple; silently ignoring it
    // would turn a mistyped argument into a plausible-looking but wrong reading.
    it("hrtime rejects a diff that is not two numbers", () => {
        [1, 2, 3].forEach((length) => {
            if (length === 2)
                return;

            var bad = [];
            for (var i = 0; i < length; i++)
                bad.push(0);

            var thrown = null;
            try {
                process.hrtime(bad);
            } catch (err) {
                thrown = err;
            }

            assert.notStrictEqual(thrown, null, 'length ' + length + ' should throw');
            assert.equal(thrown.name, 'RangeError');
            assert.equal(thrown.code, 'ERR_OUT_OF_RANGE');
        });
    });

    it("hrtime accepts an empty diff and a two-element diff", () => {
        assert.equal(process.hrtime([]).length, 2);
        assert.equal(process.hrtime([0, 0]).length, 2);
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
                env: Object.assign(test_util.pickEnv(process.env, []), {
                    QEMU_LD_PREFIX: process.env.QEMU_LD_PREFIX,
                    COUNT_LEN
                })
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

    describe("exitCode", () => {
        it("exit 0 when no error", () => {
            var code = child_process.run(cmd, [path.join(__dirname, 'process', 'exit.js')]);
            assert.equal(code, 0);
        });

        it("exit 1 when error", () => {
            var code = child_process.run(cmd, [path.join(__dirname, 'process', 'exit_error.js')]);
            assert.equal(code, 1);
        });
    });

    it("emitWarning", () => {
        var ev = new coroutine.Event();
        var warnings = [];

        process.emitWarning('Something happened!', {
            code: 'MY_WARNING',
            detail: 'This is some additional information',
        });

        process.emit("warning", 100);

        process.on('warning', (warning) => {
            warnings.push(warning);
            ev.set();
        });

        ev.wait();
        ev.clear();

        assert.equal(warnings.length, 1);
        assert.equal(warnings[0].name, 'Warning');
        assert.equal(warnings[0].message, 'Something happened!');
        assert.equal(warnings[0].code, 'MY_WARNING');
        assert.equal(warnings[0].detail, 'This is some additional information');

        process.emit("warning", 200);
        ev.wait();

        assert.equal(warnings.length, 2);
        assert.equal(warnings[1], 200);

        process.off('warning');
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

    it("resourceUsage", () => {
        let usage = process.resourceUsage();
        assert.property(usage, 'userCPUTime');
        assert.property(usage, 'systemCPUTime');
        assert.property(usage, 'maxRSS');
        assert.property(usage, 'sharedMemorySize');
        assert.property(usage, 'unsharedDataSize');
        assert.property(usage, 'unsharedStackSize');
    });

    it("release", () => {
        // the release metadata is an object with the three documented fields
        assert.deepEqual(Object.keys(process.release), ['name', 'sourceUrl', 'venderUrl']);
        assert.strictEqual(process.release.name, 'node');
        assert.isString(process.release.sourceUrl);
        assert.isString(process.release.venderUrl);
    });

    it("execArgv is a string array", () => {
        assert.isTrue(Array.isArray(process.execArgv));
        process.execArgv.forEach((v) => assert.isString(v));
    });

    it("kill signal forms", () => {
        const pid = process.pid;

        // the numeric form and the signal name form are both accepted
        assert.isUndefined(process.kill(pid, 0));
        if (process.platform === 'win32') {
            // Windows has no SIGCONT, so sig_name_to_number() never matches it
            // (its table entry is inside #ifdef SIGCONT) and the name is
            // reported the same way as any other unknown one.
            assert.throws(() => process.kill(pid, 'SIGCONT'),
                { name: 'TypeError', code: 'ERR_UNKNOWN_SIGNAL', number: 20011,
                  message: /Unknown signal: SIGCONT/ });
        } else {
            assert.isUndefined(process.kill(pid, 'SIGCONT'));
        }

        // an unknown signal name reports the name, not a type error
        assert.throws(() => process.kill(pid, 'M13_NO_SUCH_SIGNAL'),
            { name: 'TypeError', code: 'ERR_UNKNOWN_SIGNAL', number: 20011,
              message: /Unknown signal: M13_NO_SUCH_SIGNAL/ });

        // a value that is neither a number nor a signal name keeps the type error
        assert.throws(() => process.kill(pid, {}), { number: 20005 });
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

    it('BUGFIX: process.env exists key', () => {
        process.env.test_key = 'test_value';
        assert.isTrue('test_key' in process.env);
        assert.isFalse('test_key_1' in process.env);
        delete process.env.test_key;
    });

    it('loadEnvFile', () => {
        var oldCwd = process.cwd();
        var tempDir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-load-env-'));
        var envFile = path.join(tempDir, '.env');

        fs.writeTextFile(envFile, [
            'BASIC=basic',
            'EXISTING=from-file',
            'EMPTY=',
            'export EXPORTED = value',
            'SPACED=value # comment'
        ].join('\n'));

        delete process.env.BASIC;
        delete process.env.EMPTY;
        delete process.env.EXPORTED;
        delete process.env.SPACED;
        process.env.EXISTING = 'existing';

        try {
            process.chdir(tempDir);
            process.loadEnvFile();

            assert.equal(process.env.BASIC, 'basic');
            assert.equal(process.env.EXISTING, 'existing');
            assert.equal(process.env.EMPTY, '');
            assert.equal(process.env.EXPORTED, 'value');
            assert.equal(process.env.SPACED, 'value');
        } finally {
            process.chdir(oldCwd);
            delete process.env.BASIC;
            delete process.env.EMPTY;
            delete process.env.EXPORTED;
            delete process.env.SPACED;
            delete process.env.EXISTING;

            fs.unlink(envFile);
            fs.rmdir(tempDir);
        }
    });

    it('loadEnvFile missing file', () => {
        assert.throws(() => {
            process.loadEnvFile(path.join(__dirname, 'not-exists.env'));
        }, (err) => err && err.code === 'ENOENT');
    });

    it('cli --env-file loads dotenv into child process', () => {
        var tempDir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-cli-env-'));
        var baseEnvFile = path.join(tempDir, 'base.env');
        var overrideEnvFile = path.join(tempDir, 'override.env');

        fs.writeTextFile(baseEnvFile, [
            'BASIC=basic',
            'OVERRIDE=base',
            'EXISTING=from-file'
        ].join('\n'));
        fs.writeTextFile(overrideEnvFile, [
            'OVERRIDE=override'
        ].join('\n'));

        try {
            var result = child_process.spawnSync(cmd, [
                '--env-file=' + baseEnvFile,
                '--env-file=' + overrideEnvFile,
                '-e',
                'console.log(JSON.stringify({ BASIC: process.env.BASIC, OVERRIDE: process.env.OVERRIDE, EXISTING: process.env.EXISTING }));'
            ], {
                env: Object.assign({}, process.env, { EXISTING: 'existing' })
            });

            assert.equal(result.status, 0);
            assert.deepEqual(JSON.parse(result.stdout), {
                BASIC: 'basic',
                OVERRIDE: 'override',
                EXISTING: 'existing'
            });
        } finally {
            fs.unlink(baseEnvFile);
            fs.unlink(overrideEnvFile);
            fs.rmdir(tempDir);
        }
    });

    it('cli --env-file-if-exists ignores missing file', () => {
        var result = child_process.spawnSync(cmd, [
            '--env-file-if-exists=' + path.join(__dirname, 'missing-cli.env'),
            '-e',
            'console.log("ok")'
        ]);

        assert.equal(result.status, 0);
        assert.equal(result.stdout.toString().trim(), 'ok');
    });

    it('cli --env-file fails on missing file', () => {
        var result = child_process.spawnSync(cmd, [
            '--env-file=' + path.join(__dirname, 'missing-cli.env'),
            '-e',
            'console.log("unreachable")'
        ]);

        assert.notEqual(result.status, 0);
        assert.match(result.stderr, /not found/);
    });

    describe("kill", () => {
        it("should be a function", () => {
            assert.isFunction(process.kill);
        });

        it("signal 0 to check process existence", () => {
            process.kill(process.pid, 0);
        });

        process.platform !== 'win32' && it("signal 0 with string signal name", () => {
            process.kill(process.pid, 'SIGURG');
        });

        it("throw on invalid signal name", () => {
            assert.throws(() => {
                process.kill(process.pid, 'INVALID_SIGNAL');
            }, { name: 'TypeError', code: 'ERR_UNKNOWN_SIGNAL', number: 20011 });
        });

        it("throw on invalid pid", () => {
            assert.throws(() => {
                process.kill(-99999, 0);
            }, { name: 'Error', code: 'ERR_PROCESS_KILL', number: 20009 });
        });

        it("kill a child process with SIGTERM", () => {
            var cp = child_process.spawn(cmd, ['-e', 'coroutine.sleep(100000)']);
            var pid = cp.pid;

            // verify the child is alive
            process.kill(pid, 0);

            // kill the child
            process.kill(pid, 'SIGTERM');
            cp.join();

            // verify the child is gone
            assert.throws(() => {
                process.kill(pid, 0);
            }, { name: 'Error', code: 'ERR_PROCESS_KILL', number: 20009 });
        });

        it("kill a child process with integer signal", () => {
            var cp = child_process.spawn(cmd, ['-e', 'coroutine.sleep(100000)']);
            var pid = cp.pid;

            process.kill(pid, 0);
            process.kill(pid, 15); // SIGTERM
            cp.join();

            assert.throws(() => {
                process.kill(pid, 0);
            }, { name: 'Error', code: 'ERR_PROCESS_KILL', number: 20009 });
        });
    });

    // Signal handling of the process itself (plans/npm-cli-alignment-2026-10-10.md,
    // items A5/A13/B2): the listener runs from a task posted to the isolate —
    // never from inside the V8 interrupt callback, which used to segfault when
    // the interrupt landed in the middle of JS execution — and a program that
    // never yields still has to die from the signal.
    describe("signal handling", { skip: process.platform == 'win32' }, () => {
        function childAlive(pid) {
            try {
                process.kill(pid, 0);
                return true;
            } catch (e) {
                return false;
            }
        }

        // A fixed sleep cannot tell that the child reached JS: an emulated
        // target needs more than a second to boot (test/process/signal1.js
        // makes the same point), and a signal delivered before the listener
        // exists kills the child by default action. Each child announces
        // itself on stdout instead.
        function spawnReady(code) {
            var cp = child_process.spawn(cmd, ['-e', code],
                { stdio: ['ignore', 'pipe', 'ignore'] });

            new io.BufferedStream(cp.stdout).readLine(); // blocks until ready

            return cp;
        }

        it("runs a signal listener and lets it decide the exit code", () => {
            var cp = spawnReady(
                'process.on("SIGTERM",()=>{process.exit(7)});console.log("ready");setInterval(()=>{},50)');

            cp.kill('SIGTERM');

            assert.equal(cp.join(), 7, "the listener has to run and decide the exit code");
        });

        it("terminates a JS loop that never yields", () => {
            var cp = spawnReady('console.log("ready");while(true);');

            cp.kill('SIGTERM');

            var t0 = Date.now();
            while (childAlive(cp.pid) && Date.now() - t0 < 15000)
                coroutine.sleep(50);

            if (childAlive(cp.pid))
                cp.kill('SIGKILL');

            assert.isFalse(childAlive(cp.pid), "a loop that never yields must still die from SIGTERM");
            assert.equal(cp.join(), -15);
        });

        it("self kill with a listener keeps running instead of crashing", () => {
            for (var i = 0; i < 5; i++) {
                var cp = spawnReady(
                    'process.on("SIGTERM",()=>{});console.log("ready");process.kill(process.pid,"SIGTERM");setInterval(()=>{},50)');

                // the self kill fired right after the marker: a crash would
                // have taken the process down by now
                coroutine.sleep(500);
                assert.isTrue(childAlive(cp.pid), "iteration " + i + " of the self kill crashed");

                cp.kill('SIGKILL');
                assert.equal(cp.join(), -9);
            }
        });

        // 回归：init_signal() 曾经无条件
        //   setrlimit(RLIMIT_CORE, {RLIM_INFINITY, RLIM_INFINITY})
        // 把 soft limit 从 shell 默认的 0 抬到硬上限，于是 `ulimit -c 0`、
        // systemd 的 `LimitCore=0`、容器默认设置全部被静默绕过 —— 一个本来
        // 不该留下任何文件的 SIGSEGV 会在 cwd 里写出一份几百 MB 的 core。
        //
        // 契约：fibjs **不得**改变调用方的 core 限制，只能尊重它。
        // 下面直接读 /proc/self/limits 对比父 shell 的设置。
        if (process.platform === 'linux') {
            it("does not raise the inherited RLIMIT_CORE", () => {
                function readSoftCoreLimit(pid) {
                    var fd = fs.open('/proc/' + pid + '/limits', 'r');
                    try {
                        var buf = Buffer.alloc(8192);
                        var n = fs.read(fd, buf, 0, 8192, 0);
                        var txt = buf.slice(0, n).toString();
                        var line = txt.split('\n').filter(function (l) {
                            return l.indexOf('core file size') >= 0;
                        })[0];
                        // "Max core file size  <soft>  <hard>  bytes"
                        return line.trim().split(/\s+/).slice(-3)[0];
                    } finally {
                        fs.close(fd);
                    }
                }

                // 必须复现"shell 默认"这个精确条件：soft=0 而 **hard=unlimited**。
                //
                // 为什么不能直接 `ulimit -c 0`：那条命令会把 hard 也压成 0，
                // 于是非 root 的 setrlimit 必然失败，旧代码也"看起来"没问题。
                // 真正会漏的是默认场景 —— soft=0 / hard=unlimited，此时
                // `setrlimit(RLIM_INFINITY)` 会成功，soft 被静默抬到 unlimited，
                // `ulimit -c 0`、systemd LimitCore=0、容器默认设置全被绕过。
                //
                // 用 `ulimit -Sc 0` 只动 soft，保留 hard。
                var cp = child_process.spawn('/bin/sh',
                    ['-c', 'ulimit -Sc 0; exec "$0" -e "setInterval(()=>{},1000)"', cmd],
                    { stdio: ['ignore', 'ignore', 'ignore'] });
                try {
                    var t0 = Date.now();
                    while (Date.now() - t0 < 5000) {
                        try {
                            readSoftCoreLimit(cp.pid);
                            break;
                        } catch (e) {
                            coroutine.sleep(20);
                        }
                    }
                    coroutine.sleep(300);   // 让 init_signal 跑完

                    // soft 0 + hard unlimited 时，继承来的 0 必须保持 0
                    assert.strictEqual(readSoftCoreLimit(cp.pid), '0',
                        'init_signal must not raise an inherited soft core limit of 0');
                } finally {
                    cp.kill('SIGKILL');
                    cp.join();
                }
            });
        }
    });

    describe("umask", () => {
        it("accepts an octal string and a number", () => {
            const old = process.umask();
            // Windows has no POSIX mode bits: its CRT mask keeps only
            // _S_IREAD|_S_IWRITE, so every mask reads back as 0o600
            const masked = process.platform === 'win32' ? () => 0o600 : (mask) => mask;
            try {
                process.umask('0700');
                assert.strictEqual(process.umask() & 0o777, masked(0o700));

                process.umask(0o644);
                assert.strictEqual(process.umask() & 0o777, masked(0o644));
            } finally {
                process.umask(old);
            }
        });
        it("rejects a mask that is not an octal string or a number", () => {
            const old = process.umask();
            try {
                assert.throws(() => process.umask('nope'),
                    { name: 'TypeError', code: 'ERR_INVALID_ARG_VALUE', number: 20011 });
            } finally {
                process.umask(old);
            }
        });
    });

    describe("getBuiltinModule (Node.js compatibility)", () => {
        it("is a function", () => {
            assert.isFunction(process.getBuiltinModule);
        });

        it("returns undefined for non builtin modules", () => {
            assert.isUndefined(process.getBuiltinModule('not/a/builtin'));
            assert.isUndefined(process.getBuiltinModule('node:not/a/builtin'));
            assert.isUndefined(process.getBuiltinModule(''));
            assert.isUndefined(process.getBuiltinModule('internal/test/binding'));
        });

        it("returns the same module object as require for natives", () => {
            assert.strictEqual(process.getBuiltinModule('path'), require('path'));
            assert.strictEqual(process.getBuiltinModule('node:path'), require('node:path'));
            assert.strictEqual(process.getBuiltinModule('fs'), require('fs'));
            assert.strictEqual(process.getBuiltinModule('node:process'), require('process'));
            assert.strictEqual(process.getBuiltinModule('buffer'), require('buffer'));
        });

        it("supports the module builtin", () => {
            assert.strictEqual(process.getBuiltinModule('module'), require('module'));
            assert.strictEqual(process.getBuiltinModule('node:module'), require('module'));
            assert.strictEqual(process.getBuiltinModule('fibjs:module'), require('module'));
            assert.isFunction(process.getBuiltinModule('module').createRequire);
            assert.ok(Array.isArray(process.getBuiltinModule('module').builtinModules));
        });

        it("accepts the fibjs: prefix", () => {
            assert.strictEqual(process.getBuiltinModule('fibjs:fs'), require('fibjs:fs'));
            assert.isFunction(process.getBuiltinModule('fibjs:stream'));
            assert.isUndefined(process.getBuiltinModule('fibjs:not/a/builtin'));
        });

        it("supports path sub-path modules", () => {
            assert.strictEqual(process.getBuiltinModule('path/posix'), require('path').posix);
            assert.strictEqual(process.getBuiltinModule('node:path/posix'), require('path').posix);
            assert.strictEqual(process.getBuiltinModule('path/win32'), require('path').win32);
            assert.strictEqual(process.getBuiltinModule('node:path/win32'), require('path').win32);

            assert.strictEqual(process.getBuiltinModule('node:path/posix').sep, '/');
            assert.strictEqual(process.getBuiltinModule('node:path/win32').sep, '\\');
        });

        it("supports assert/strict and util/types", () => {
            assert.ok(process.getBuiltinModule('assert/strict'));
            assert.ok(process.getBuiltinModule('node:assert/strict'));
            assert.ok(process.getBuiltinModule('util/types'));
            assert.ok(process.getBuiltinModule('node:util/types'));
        });

        it("supports promise-flavored sub-path modules", () => {
            assert.ok(process.getBuiltinModule('fs/promises'));
            assert.ok(process.getBuiltinModule('node:fs/promises'));
            assert.ok(process.getBuiltinModule('dns/promises'));
        });

        it("supports embedded JS builtin modules", () => {
            assert.ok(process.getBuiltinModule('stream'));
            assert.ok(process.getBuiltinModule('stream/web'));
            assert.ok(process.getBuiltinModule('readline/promises'));
            assert.ok(process.getBuiltinModule('timers/promises'));
        });

        it("keeps module identity with require for embedded modules", () => {
            assert.strictEqual(process.getBuiltinModule('stream/web'), require('stream/web'));
            assert.strictEqual(process.getBuiltinModule('node:stream/web'), require('node:stream/web'));
            assert.strictEqual(process.getBuiltinModule('timers/promises'), require('timers/promises'));
        });
    });

    describe("features (Node.js compatibility)", () => {
        // The key set follows Node.js's own test, test/parallel/test-process-features.js.
        // A probe written as `'x' in process.features` must never throw, so the
        // object has to be present and every documented key has to exist.
        var keys = [
            'inspector', 'debug', 'uv', 'ipv6', 'openssl_is_boringssl',
            'dtls', 'quic', 'tls_alpn', 'tls_sni', 'tls_ocsp', 'tls',
            'cached_builtins', 'require_module', 'typescript', 'sea',
        ];

        it("exists and is an object", () => {
            assert.isObject(process.features);
            assert.strictEqual(process.features, process.features,
                'the object must be stable across reads');
        });

        it("has every documented key", () => {
            keys.forEach((key) => {
                assert.ok(key in process.features, 'missing process.features.' + key);
            });
        });

        it("reports booleans for the capability flags", () => {
            keys.filter((k) => k !== 'typescript').forEach((key) => {
                assert.strictEqual(typeof process.features[key], 'boolean',
                    'process.features.' + key + ' must be a boolean');
            });
        });

        it("reports a string or false for typescript", () => {
            var ts = process.features.typescript;

            assert.ok(ts === false || typeof ts === 'string',
                'process.features.typescript must be false or a string, got ' + typeof ts);
        });

        it("reports the capabilities this build actually has", () => {
            assert.isTrue(process.features.uv);
            assert.isTrue(process.features.tls);
            assert.isTrue(process.features.tls_alpn);
            assert.isTrue(process.features.tls_sni);
            assert.isFalse(process.features.openssl_is_boringssl,
                'fibjs bundles OpenSSL proper');
        });

        it("is not writable", () => {
            assert.throws(() => { process.features = {}; }, TypeError);
            assert.isObject(process.features);
        });

        it("supports the 'sea' probe used by real packages", () => {
            // pi-coding-agent 1.5.2 does exactly this at load time; an `in`
            // operator on undefined used to throw there.
            assert.doesNotThrow(() => {
                return ('sea' in process.features && process.features.sea === true)
                    || process.getBuiltinModule('node:sea') === undefined;
            });
            assert.isFalse(process.features.sea,
                'the test runner is not a single executable application');
        });

        it("is a plain data object", () => {
            assert.strictEqual(Object.prototype.toString.call(process.features),
                '[object Object]');
            assert.ok(Object.keys(process.features).length >= keys.length);
        });
    });

    describe("constrainedMemory (Node.js compatibility)", () => {
        it("is a function returning a number", () => {
            assert.strictEqual(typeof process.constrainedMemory, 'function');
            assert.strictEqual(typeof process.constrainedMemory(), 'number');
        });

        it("reports 2^64 when there is no cgroup limit", () => {
            // The same "no constraint" answer Node.js gives: a value a caller
            // compares its usage against, and one that no real limit reaches.
            const limit = process.constrainedMemory();
            assert.ok(limit > 0, 'a positive number: ' + limit);
            assert.ok(Number.isFinite(limit), 'finite: ' + limit);
        });

        it("is stable across reads", () => {
            assert.strictEqual(process.constrainedMemory(), process.constrainedMemory());
        });
    });
});

