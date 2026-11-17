var { describe, it } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var os = require('os');
var path = require('path');
var vm = require('vm');
var coroutine = require('coroutine');

// wasm32 fixtures built with Homebrew LLVM:
//
//   clang --target=wasm32 -nostdlib -O2 -Wl,--no-entry -Wl,--export-memory \
//         -o wasm_module.wasm wasm_module.c
//   clang --target=wasm32 -nostdlib -O2 -Wl,--no-entry -Wl,--export-memory \
//         -o wasm_wasi_module.wasm wasm_wasi_module.c
//
// wasm_module.c imports add() from "./helper.js" and exports memory + run():
//
//   __attribute__((import_module("./helper.js"), import_name("add")))
//   extern int add(int, int);
//   __attribute__((export_name("run"))) int run(int x) { return add(x, 1) * 2; }
//
// wasm_wasi_module.c imports fd_write() from the wasi namespace and exports
// memory + _start(), which writes "wasm module\n" to fd 1:
//
//   __attribute__((import_module("wasi_snapshot_preview1"),
//                  import_name("fd_write")))
//   extern int fd_write(int fd, const void* iovs, int iovs_len, int* nwritten);
//   __attribute__((export_name("_start"))) void _start(void) { ... }

// JIT-less builds (iOS/iPadOS compile V8 with V8_JITLESS) do not expose the
// WebAssembly API, so none of the fixtures can be loaded there.
var hasWasm = typeof WebAssembly !== 'undefined';

var MODULE_FIXTURE = path.join(__dirname, 'wasm_module.wasm');
var WASI_FIXTURE = path.join(__dirname, 'wasm_wasi_module.wasm');

function tempDir() {
    return fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-wasm-'));
}

function copyFixture(src, dir, name) {
    var dest = path.join(dir, name);
    fs.writeFileSync(dest, fs.readFileSync(src));
    return dest;
}

// Windows releases host handles asynchronously, so a directory the host just
// closed can still look busy to a recursive remove: the file delete fails and
// the rmdir then reports ENOTEMPTY. Retry with a GC and a short sleep, like the
// fs suite does.
function rmSyncRetry(target) {
    for (var attempt = 0; attempt < 10; attempt++) {
        try {
            fs.rmSync(target, { recursive: true, force: true });
            return;
        } catch (err) {
            var code = err && err.code;
            if (code !== 'EBUSY' && code !== 'EPERM' && code !== 'EACCES'
                && code !== 'ENOTEMPTY')
                throw err;

            if (typeof gc == 'function')
                gc();
            coroutine.sleep(Math.min(20 * (attempt + 1), 200));
        }
    }

    // The handles never drained: let the removal report the real error.
    fs.rmSync(target, { recursive: true, force: true });
}

// A temp directory holding one copy of the module fixture plus the `./helper.js`
// namespace it imports.
function linkedModule(name) {
    var dir = tempDir();
    var file = copyFixture(MODULE_FIXTURE, dir, name);
    fs.writeFileSync(path.join(dir, 'helper.js'),
        'exports.add = function (a, b) { return a + b; };\n');
    return { dir: dir, file: file };
}

describe('wasm module', { skip: !hasWasm }, () => {
    it('loads a wasm file and resolves its import namespace as a module', () => {
        var fixture = linkedModule('mod.wasm');
        try {
            var exports = require(fixture.file);

            assert.deepStrictEqual(Object.keys(exports).sort(), ['memory', 'run']);
            assert.strictEqual(typeof exports.run, 'function');
            assert.strictEqual(exports.run(3), 8);
            assert.strictEqual(exports.run(-1), 0);
        } finally {
            rmSyncRetry(fixture.dir);
        }
    });

    it('caches the instantiated module', () => {
        var fixture = linkedModule('cache.wasm');
        try {
            var first = require(fixture.file);
            var second = require(fixture.file);

            assert.strictEqual(first, second);
            assert.strictEqual(first.run(1), 4);
        } finally {
            rmSyncRetry(fixture.dir);
        }
    });

    it('links the instance to the exports under a well-known symbol', () => {
        var fixture = linkedModule('link.wasm');
        try {
            var exports = require(fixture.file);
            var instance = exports[Symbol.for('fibjs.wasm.instance')];

            assert.strictEqual(typeof instance, 'object');
            assert.strictEqual(instance.exports.run, exports.run);
            assert.strictEqual(instance.exports.memory, exports.memory);
            // the symbol key stays out of the module surface
            assert.deepStrictEqual(Object.keys(exports).sort(), ['memory', 'run']);
            assert.strictEqual(JSON.stringify(exports), '{"memory":{}}');
        } finally {
            rmSyncRetry(fixture.dir);
        }
    });

    it('reports an unresolvable namespace as a LinkError', () => {
        var dir = tempDir();
        try {
            copyFixture(MODULE_FIXTURE, dir, 'orphan.wasm');
            var box = new vm.SandBox({});

            assert.throws(() => box.require('./orphan.wasm', dir), (e) => {
                assert.strictEqual(e.name, 'LinkError');
                assert.ok(/Cannot find wasm import namespace '\.\/helper\.js'/
                    .test(e.message), e.message);
                assert.ok(e.message.indexOf('orphan.wasm') > 0, e.message);
                return true;
            });
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('prefixes compile errors with the module path', () => {
        var dir = tempDir();
        try {
            var file = path.join(dir, 'broken.wasm');
            fs.writeFileSync(file, Buffer.from('not a wasm binary'));
            var box = new vm.SandBox({});
            var real = fs.realpathSync(file);

            assert.throws(() => box.require('./broken.wasm', dir), (e) => {
                assert.strictEqual(e.name, 'CompileError');
                assert.ok(e.message.indexOf(real) === 0, e.message);
                return true;
            });
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('exposes the exports through an esm import', async () => {
        var fixture = linkedModule('esm.wasm');
        try {
            var ns = await import(fixture.file);

            assert.strictEqual(typeof ns.run, 'function');
            assert.strictEqual(ns.run(2), 6);
            assert.strictEqual(ns.default.run, ns.run);
        } finally {
            rmSyncRetry(fixture.dir);
        }
    });
});

describe('wasm module with wasi', { skip: !hasWasm }, () => {
    it('starts a registered host on the value returned by require', () => {
        var dir = tempDir();
        var output = path.join(dir, 'out.txt');
        var handle = fs.openSync(output, 'w');

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1', args: ['app'], stdout: handle.fd });
            var file = copyFixture(WASI_FIXTURE, dir, 'app.wasm');

            wasi.registerAsImport();
            try {
                var exports = require(file);
                wasi.start(exports);
            } finally {
                wasi.unregisterAsImport();
            }

            handle.closeSync();
            assert.strictEqual(fs.readFileSync(output, 'utf8'), 'wasm module\n');
        } finally {
            try {
                handle.closeSync();
            } catch (e) {
                // already closed
            }
            rmSyncRetry(dir);
        }
    });

    it('falls back to require() once the host is unregistered', () => {
        var dir = tempDir();
        try {
            copyFixture(WASI_FIXTURE, dir, 'orphan.wasm');
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1' });

            wasi.registerAsImport();
            wasi.unregisterAsImport();

            var box = new vm.SandBox({});
            assert.throws(() => box.require('./orphan.wasm', dir), (e) => {
                assert.strictEqual(e.name, 'LinkError');
                assert.ok(/wasi_snapshot_preview1/.test(e.message), e.message);
                return true;
            });
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('still accepts a plain WebAssembly.Instance', () => {
        var dir = tempDir();
        var output = path.join(dir, 'out.txt');
        var handle = fs.openSync(output, 'w');

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1', stdout: handle.fd });
            var instance = new WebAssembly.Instance(
                new WebAssembly.Module(fs.readFileSync(WASI_FIXTURE)),
                wasi.getImportObject());

            wasi.start(instance);

            handle.closeSync();
            assert.strictEqual(fs.readFileSync(output, 'utf8'), 'wasm module\n');
        } finally {
            try {
                handle.closeSync();
            } catch (e) {
                // already closed
            }
            rmSyncRetry(dir);
        }
    });

    it('keeps rejecting objects that are not an instance or wasm exports', () => {
        var { WASI } = require('wasi');
        var wasi = new WASI({ version: 'preview1' });

        assert.throws(() => wasi.start({}), /The "instance\.exports" property must be of type object/);
    });
});
