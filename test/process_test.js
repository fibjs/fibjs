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
                env: {
                    QEMU_LD_PREFIX: process.env.QEMU_LD_PREFIX,
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
            });
        });

        it("throw on invalid pid", () => {
            assert.throws(() => {
                process.kill(-99999, 0);
            });
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
            });
        });

        it("kill a child process with integer signal", () => {
            var cp = child_process.spawn(cmd, ['-e', 'coroutine.sleep(100000)']);
            var pid = cp.pid;

            process.kill(pid, 0);
            process.kill(pid, 15); // SIGTERM
            cp.join();

            assert.throws(() => {
                process.kill(pid, 0);
            });
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
});

