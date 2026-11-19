var { describe, it, after } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');

// Temp directories a suite could not remove. Reported once by the after() hook
// below instead of from the teardown.
var leftovers = [];

// JIT-less builds (iOS/iPadOS compile V8 with V8_JITLESS) execute JavaScript
// only through the interpreter and do not expose the WebAssembly API at all,
// so the wasm fixtures cannot be instantiated there.
var hasWasm = typeof WebAssembly !== 'undefined';

// Windows releases host handles asynchronously, so a directory the host just
// closed can still look busy to a recursive remove: the file delete fails and
// the rmdir then reports ENOTEMPTY. Retry with a GC and a short sleep, like the
// fs suite does.
//
// A directory that stays busy means an assertion threw before fd_close, so the
// host still holds a descriptor. Throwing here would replace that assertion - a
// throw inside finally wins over the error already in flight - and turn a clear
// failure into an EPERM on a temp path. Record it and let the suite report it.
function rmSyncRetry(target) {
    for (var attempt = 0; attempt < 10; attempt++) {
        try {
            fs.rmSync(target, { recursive: true, force: true });
            return;
        } catch (err) {
            var code = err && err.code;
            if (code !== 'EBUSY' && code !== 'EPERM' && code !== 'EACCES'
                && code !== 'ENOTEMPTY') {
                leftovers.push(target + ': ' + err.message);
                return;
            }

            if (typeof gc == 'function')
                gc();
            coroutine.sleep(Math.min(20 * (attempt + 1), 200));
        }
    }

    try {
        fs.rmSync(target, { recursive: true, force: true });
    } catch (err) {
        leftovers.push(target + ': ' + err.message);
    }
}

// uvwasi returns ENOSYS from fd_fdstat_set_flags on Windows: there is no fcntl(),
// so a guest cannot switch a descriptor to append mode, and fd_fdstat_get keeps
// reporting the flags the descriptor was opened with.
var isWindows = process.platform === 'win32';

// wasm32 fixture compiled with clang --target=wasm32 -nostdlib (Homebrew LLVM
// 22). It imports fd_write, fd_read and poll_oneoff, exports memory, _start and
// three getters. Kept inline so the test has no build-time toolchain
// dependency.
var FIXTURE = Buffer.from(
    'AGFzbQEAAAABFARgBH9/f38Bf2AAAX9gAABgAAF+AmkDFndhc2lfc25hcHNob3RfcHJldmlldzEIZmRf' +
    'd3JpdGUAABZ3YXNpX3NuYXBzaG90X3ByZXZpZXcxB2ZkX3JlYWQAABZ3YXNpX3NuYXBzaG90X3ByZXZp' +
    'ZXcxC3BvbGxfb25lb2ZmAAADBQQCAwEBBQMBAAIGCAF/AUGAgAQLB0YFBm1lbW9yeQIABl9zdGFydAAD' +
    'DWxhc3RfdXNlcmRhdGEABA9iYWRfd3JpdGVfZXJybm8ABQ5iYWRfcmVhZF9lcnJubwAGDAEBCt0BBLwB' +
    'AQF/IwBB0ABrIgAkACAAQQs2AkwgAEGAgAQ2AkggAEEANgJEQQEgAEHIAGpBASAAQcQAahAAGkGQgARB' +
    '4wAgAEHIAGpBASAAQcQAahAANgIAQZSABEHjACAAQcgAakEBIABBxABqEAE2AgAgAEEAOwE4IABCADcD' +
    'MCAAQoCcnDk3AyggAEEBNgIgIABBADoAGCAAQsIANwMQIABBADYCDCAAQRBqQZiABEEBIABBDGoQAhog' +
    'AEHQAGokAAsJAEGYgAQpAwALCQBBkIAEKAIACwkAQZSABCgCAAsLEwEAQYCABAsLaGVsbG8gd2FzaQoA' +
    'Lwlwcm9kdWNlcnMBDHByb2Nlc3NlZC1ieQEOSG9tZWJyZXcgY2xhbmcGMjIuMS40AJQBD3RhcmdldF9m' +
    'ZWF0dXJlcwgrD211dGFibGUtZ2xvYmFscysTbm9udHJhcHBpbmctZnB0b2ludCsLYnVsay1tZW1vcnkr' +
    'CHNpZ24tZXh0Kw9yZWZlcmVuY2UtdHlwZXMrCm11bHRpdmFsdWUrD2J1bGstbWVtb3J5LW9wdCsWY2Fs' +
    'bC1pbmRpcmVjdC1vdmVybG9uZw==',
    'base64');

function instantiate() {
    var { WASI } = require('wasi');
    var wasi = new WASI({ version: 'preview1' });
    var module = new WebAssembly.Module(FIXTURE);
    var instance = new WebAssembly.Instance(module, wasi.getImportObject());
    return { wasi: wasi, instance: instance };
}

// wasm32 fixture for the preopen/path_open/fd-table flow: args/env sizes,
// fd_prestat_get/dir_name, fd_fdstat_get/fd_filestat_get, path_open/read/close
// and the NOTCAPABLE check.
var FS_FIXTURE = Buffer.from(
    'AGFzbQEAAAABLwdgAAF/YAJ/fwF/YAR/f39/AX9gA39/fwF/YAl/f39/f35+f38Bf2ABfwF/YAAAAsgD' +
    'DBZ3YXNpX3NuYXBzaG90X3ByZXZpZXcxDmFyZ3Nfc2l6ZXNfZ2V0AAEWd2FzaV9zbmFwc2hvdF9wcmV2' +
    'aWV3MQhhcmdzX2dldAABFndhc2lfc25hcHNob3RfcHJldmlldzERZW52aXJvbl9zaXplc19nZXQAARZ3' +
    'YXNpX3NuYXBzaG90X3ByZXZpZXcxC2Vudmlyb25fZ2V0AAEWd2FzaV9zbmFwc2hvdF9wcmV2aWV3MQ5m' +
    'ZF9wcmVzdGF0X2dldAABFndhc2lfc25hcHNob3RfcHJldmlldzETZmRfcHJlc3RhdF9kaXJfbmFtZQAD' +
    'Fndhc2lfc25hcHNob3RfcHJldmlldzENZmRfZmRzdGF0X2dldAABFndhc2lfc25hcHNob3RfcHJldmll' +
    'dzEPZmRfZmlsZXN0YXRfZ2V0AAEWd2FzaV9zbmFwc2hvdF9wcmV2aWV3MQlwYXRoX29wZW4ABBZ3YXNp' +
    'X3NuYXBzaG90X3ByZXZpZXcxB2ZkX3JlYWQAAhZ3YXNpX3NuYXBzaG90X3ByZXZpZXcxCGZkX2Nsb3Nl' +
    'AAUWd2FzaV9zbmFwc2hvdF9wcmV2aWV3MQhmZF93cml0ZQACAxIRBgAAAAAAAAAAAAAAAAAAAAAFAwEA' +
    'Agf4ARIGbWVtb3J5AgAGX3N0YXJ0AAwJYXJnX2NvdW50AA0MYXJnX2J1Zl9zaXplAA4JZW52X2NvdW50' +
    'AA8MZW52X2J1Zl9zaXplABAIbmFtZV9sZW4AEQ9wcmVfZmRzdGF0X3R5cGUAEhBmaWxlX2Zkc3RhdF90' +
    'eXBlABMKb3Blbl9lcnJubwAUCnJlYWRfZXJybm8AFQtjbG9zZV9lcnJubwAWDGVzY2FwZV9lcnJubwAX' +
    'BW5yZWFkABgNcHJlc3RhdF9lcnJubwAZDXByZW5hbWVfZXJybm8AGgxmZHN0YXRfZXJybm8AGw5maWxl' +
    'c3RhdF9lcnJubwAcDAECCuwDEckCAQF/AkBBsIAEQbSABBAADQBBsIAEKAIAQQhLDQBBwIAEQeCABBAB' +
    'GgsCQEHgggRB5IIEEAINAEHgggQoAgBBCEsNAEHwggRBkIMEEAMaC0GYhQRBA0GQhQQQBDYCAEGchQRB' +
    'lIUEKAAANgIAQeCFBEEDQaCFBEHAABAFNgIAQYiGBEEDQfCFBBAGNgIAQdCGBEEDQZCGBBAHNgIAQdiG' +
    'BEEDQQBBgIAEQQlBAEICQgBBAEHUhgQQCCIANgIAAkAgAA0AQdSGBCgCAEHghgQQBhpB/IYEQdSGBCgC' +
    'AEGUgARBAUH4hgQQCTYCAEGAhwRB1IYEKAIAEAo2AgBB/IYEKAIADQBB+IYEKAIAIgBFDQBBoIAEIAA2' +
    'AgBBAUGcgARBAUGEhwQQCxoLQYiHBEEDQQBBioAEQQlBAEICQgBBAEHUhgQQCDYCAAsJAEGwgAQoAgAL' +
    'CQBBtIAEKAIACwkAQeCCBCgCAAsJAEHkggQoAgALCQBBnIUEKAIACwkAQfCFBC0AAAsJAEHghgQtAAAL' +
    'CQBB2IYEKAIACwkAQfyGBCgCAAsJAEGAhwQoAgALCQBBiIcEKAIACwkAQfiGBCgCAAsJAEGYhQQoAgAL' +
    'CQBB4IUEKAIACwkAQYiGBCgCAAsJAEHQhgQoAgALCy0CAEGAgAQLE2hlbGxvLnR4dAAuLi9lc2NhcGUA' +
    'QZSABAsLkAMBAEAAAACQAwEALwlwcm9kdWNlcnMBDHByb2Nlc3NlZC1ieQEOSG9tZWJyZXcgY2xhbmcG' +
    'MjIuMS40AJQBD3RhcmdldF9mZWF0dXJlcwgrD211dGFibGUtZ2xvYmFscysTbm9udHJhcHBpbmctZnB0' +
    'b2ludCsLYnVsay1tZW1vcnkrCHNpZ24tZXh0Kw9yZWZlcmVuY2UtdHlwZXMrCm11bHRpdmFsdWUrD2J1' +
    'bGstbWVtb3J5LW9wdCsWY2FsbC1pbmRpcmVjdC1vdmVybG9uZw==',
    'base64');

// wasm32 fixture for seek/tell/pread, readdir, directory and path mutations
// (create/remove/rename/link/symlink/readlink/unlink) and ftruncate.
var OPS_FIXTURE = fs.readFileSync(path.join(__dirname, 'wasi_ops.wasm'));

// Minimal module: one exported memory and nothing else. Used to call the
// bound syscalls directly from JS with the same guest layout as Node's tests.
var MEMORY_FIXTURE = Buffer.from('0061736d010000000503010002070a01066d656d6f72790200', 'hex');

// Command-style module: exported memory and an empty _start.
var START_FIXTURE = Buffer.from(
    'AGFzbQEAAAABBAFgAAADAgEABQMBAAEHEwIGbWVtb3J5AgAGX3N0YXJ0AAAKBAECAAsALwlw' +
    'cm9kdWNlcnMBDHByb2Nlc3NlZC1ieQEOSG9tZWJyZXcgY2xhbmcGMjIuMS40AJQBD3Rhcmdl' +
    'dF9mZWF0dXJlcwgrD211dGFibGUtZ2xvYmFscysTbm9udHJhcHBpbmctZnB0b2ludCsLYnVs' +
    'ay1tZW1vcnkrCHNpZ24tZXh0Kw9yZWZlcmVuY2UtdHlwZXMrCm11bHRpdmFsdWUrD2J1bGst' +
    'bWVtb3J5LW9wdCsWY2FsbC1pbmRpcmVjdC1vdmVybG9uZw==',
    'base64');

// Both exports, to prove start()/initialize() reject the wrong shape.
var BOTH_FIXTURE = Buffer.from(
    'AGFzbQEAAAABBAFgAAADAgEABQMBAAEHIQMGbWVtb3J5AgAGX3N0YXJ0AAALX2luaXRpYWxp' +
    'emUAAAoEAQIACwAvCXByb2R1Y2VycwEMcHJvY2Vzc2VkLWJ5AQ5Ib21lYnJldyBjbGFuZwYy' +
    'Mi4xLjQAlAEPdGFyZ2V0X2ZlYXR1cmVzCCsPbXV0YWJsZS1nbG9iYWxzKxNub250cmFwcGlu' +
    'Zy1mcHRvaW50KwtidWxrLW1lbW9yeSsIc2lnbi1leHQrD3JlZmVyZW5jZS10eXBlcysKbXVs' +
    'dGl2YWx1ZSsPYnVsay1tZW1vcnktb3B0KxZjYWxsLWluZGlyZWN0LW92ZXJsb25n',
    'base64');

// Command-style module that calls proc_exit(7) immediately.
var EXIT_FIXTURE = Buffer.from(
    'AGFzbQEAAAABCAJgAX8AYAAAAiQBFndhc2lfc25hcHNob3RfcHJldmlldzEJcHJvY19leGl0' +
    'AAADAgEBBQMBAAEHEwIGbWVtb3J5AgAGX3N0YXJ0AAEKCAEGAEEHEAALAC8JcHJvZHVjZXJz' +
    'AQxwcm9jZXNzZWQtYnkBDkhvbWVicmV3IGNsYW5nBjIyLjEuNACUAQ90YXJnZXRfZmVhdHVy' +
    'ZXMIKw9tdXRhYmxlLWdsb2JhbHMrE25vbnRyYXBwaW5nLWZwdG9pbnQrC2J1bGstbWVtb3J5' +
    'KwhzaWduLWV4dCsPcmVmZXJlbmNlLXR5cGVzKwptdWx0aXZhbHVlKw9idWxrLW1lbW9yeS1v' +
    'cHQrFmNhbGwtaW5kaXJlY3Qtb3Zlcmxvbmc=',
    'base64');


// uvwasi__RIGHTS_REGULAR_FILE_BASE; the maximum a regular file can grant.
var FILE_RIGHTS = 0x08E001FF;
// Bits 0..27: everything a preopened directory can hand out to a new file.
var PREOPEN_INHERITING = 0x0FFFFFFF;

describe('wasi', { skip: !hasWasm }, () => {
    // A directory the suites could not remove is a failure of its own: the
    // teardown cannot report it without hiding the assertion that left it.
    after(() => {
        assert.deepStrictEqual(leftovers, [], 'temp directories left behind');
    });

    it('exports the WASI constructor and a self-bound import object', () => {
        var { WASI } = require('wasi');

        assert.strictEqual(typeof WASI, 'function');

        var wasi = new WASI({ version: 'preview1' });
        assert.ok(wasi instanceof WASI);

        // wasm imports are plain function calls; the constructor binds each
        // syscall to the host and stores it as an own property, so the import
        // object is the host itself (unlike Node, where it is a wrapper).
        assert.strictEqual(wasi.wasiImport, wasi);
        assert.strictEqual(wasi.wasiImport, wasi.wasiImport);
        assert.strictEqual(typeof wasi.wasiImport.fd_write, 'function');
        assert.strictEqual(typeof wasi.wasiImport.fd_read, 'function');
        assert.strictEqual(typeof wasi.wasiImport.poll_oneoff, 'function');
        assert.ok(Object.prototype.hasOwnProperty.call(wasi, 'fd_write'));

        var imports = wasi.getImportObject();
        assert.deepStrictEqual(Object.keys(imports), ['wasi_snapshot_preview1']);
        assert.strictEqual(imports.wasi_snapshot_preview1, wasi);
    });

    it('rejects an unsupported version', () => {
        var { WASI } = require('wasi');

        assert.throws(function() {
            new WASI({ version: 'preview2' });
        }, /unsupported version/);
    });

    it('runs fd_write/fd_read/poll_oneoff from wasm without blocking other fibers', () => {
        var { wasi, instance } = instantiate();

        var started = Date.now();
        var timerAt = -1;
        setTimeout(function() {
            timerAt = Date.now() - started;
        }, 30);

        var code = wasi.start(instance);
        var elapsed = Date.now() - started;

        // _start writes "hello wasi\n", checks the BADF paths and then waits
        // 120ms in poll_oneoff.
        assert.strictEqual(code, 0);
        assert.ok(elapsed >= 100, `poll_oneoff returned too early: ${elapsed}ms`);
        assert.ok(elapsed < 3000, `poll_oneoff used the wrong time unit: ${elapsed}ms`);

        // The timer ran while the wasm import was suspended: the fiber, not
        // the isolate, was waiting. A loaded runner can deliver the callback
        // only after start() returns, so drain a few loop turns before failing;
        // a timer that never runs at all still fails here, and the socket poll
        // case below proves the callback has to fire inside the wait.
        for (var i = 0; i < 200 && timerAt < 0; i++)
            coroutine.sleep(5);

        assert.ok(timerAt >= 0,
            `the timer did not run after poll_oneoff (${timerAt}ms)`);

        // Guest memory was written after the fiber resumed.
        assert.strictEqual(instance.exports.last_userdata(), 66n);
        assert.strictEqual(instance.exports.bad_write_errno(), 8);
        assert.strictEqual(instance.exports.bad_read_errno(), 8);
    });

    it('opens and reads a host file through a preopened directory', () => {
        var dir = fs.mkdtempSync(path.join(require('os').tmpdir(), 'fibjs-wasi-'));
        fs.writeFileSync(path.join(dir, 'hello.txt'), 'from-host\n');

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({
                version: 'preview1',
                args: ['alpha', 'beta'],
                env: { FOO: 'bar' },
                preopens: { '/': dir }
            });

            var module = new WebAssembly.Module(FS_FIXTURE);
            var instance = new WebAssembly.Instance(module, wasi.getImportObject());
            var e = instance.exports;

            assert.strictEqual(wasi.start(instance), 0);

            // args: alpha + beta; env: FOO=bar
            assert.strictEqual(e.arg_count(), 2);
            assert.strictEqual(e.arg_buf_size(), 11);
            assert.strictEqual(e.env_count(), 1);
            assert.strictEqual(e.env_buf_size(), 8);

            // preopen fd 3: guest name "/", directory filetype
            assert.strictEqual(e.prestat_errno(), 0);
            assert.strictEqual(e.name_len(), 1);
            assert.strictEqual(e.prename_errno(), 0);
            assert.strictEqual(e.pre_fdstat_type(), 3);
            assert.strictEqual(e.fdstat_errno(), 0);
            assert.strictEqual(e.filestat_errno(), 0);

            // path_open + fd_read + fd_close; the file is a regular file
            assert.strictEqual(e.open_errno(), 0);
            assert.strictEqual(e.file_fdstat_type(), 4);
            assert.strictEqual(e.read_errno(), 0);
            assert.strictEqual(e.close_errno(), 0);
            assert.strictEqual(e.nread(), 10);

            // "../escape" must stay inside the preopen
            assert.strictEqual(e.escape_errno(), 76);

            // The synchronous syscalls share the bound instance memory.
            assert.strictEqual(wasi.random_get(1024, 16), 0);
            var randomBytes = new Uint8Array(instance.exports.memory.buffer, 1024, 16);
            assert.ok(randomBytes.some(function(b) { return b !== 0; }));

            assert.strictEqual(wasi.clock_time_get(1, 1, 2048), 0);
            var now = new DataView(instance.exports.memory.buffer).getBigUint64(2048, true);
            assert.ok(now > 0n);

            assert.strictEqual(wasi.clock_res_get(0, 2112), 0);
            assert.strictEqual(wasi.sched_yield(), 0);
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('seeks, reads directories and mutates paths inside the preopen', () => {
        var dir = fs.mkdtempSync(path.join(require('os').tmpdir(), 'fibjs-wasi-'));
        fs.writeFileSync(path.join(dir, 'hello.txt'), 'from-host\n');

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1', preopens: { '/': dir } });
            var module = new WebAssembly.Module(OPS_FIXTURE);
            var instance = new WebAssembly.Instance(module, wasi.getImportObject());
            var e = instance.exports;

            assert.strictEqual(wasi.start(instance), 0);

            // readdir saw hello.txt
            assert.ok(e.dir_used() > 0);
            assert.strictEqual(e.has_hello(), 1);

            // tell after a 10-byte read, seek back to 0, pread byte 5 ('h')
            assert.strictEqual(e.tell_errno(), 0);
            assert.strictEqual(e.tell_pos(), 10n);
            assert.strictEqual(e.seek_errno(), 0);
            assert.strictEqual(e.seek_pos(), 0n);
            assert.strictEqual(e.pread_errno(), 0);
            assert.strictEqual(e.pread_byte(), 104);

            // directory and file mutations
            assert.strictEqual(e.mkdir_errno(), 0);
            assert.strictEqual(e.rmdir_errno(), 0);
            assert.strictEqual(e.create_errno(), 0);
            assert.strictEqual(e.pwrite_errno(), 0);
            assert.strictEqual(e.rename_errno(), 0);
            assert.strictEqual(e.symlink_errno(), 0);
            assert.strictEqual(e.readlink_errno(), 0);
            assert.strictEqual(e.link_used(), 5);
            assert.strictEqual(e.link_errno(), 0);
            assert.strictEqual(e.truncate_errno(), 0);
            assert.strictEqual(e.stat_errno(), 0);
            assert.strictEqual(e.size_after(), 1n);
            assert.strictEqual(e.unlink_errno(), 0);

            // only the host-created file is left
            assert.deepStrictEqual(fs.readdirSync(dir), ['hello.txt']);
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('syncs, retimes, renumbers and polls descriptors', () => {
        var dir = fs.mkdtempSync(path.join(require('os').tmpdir(), 'fibjs-wasi-'));
        var file = path.join(dir, 'file.txt');
        fs.writeFileSync(file, 'abc');

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1', preopens: { '/': dir } });
            var module = new WebAssembly.Module(FS_FIXTURE);
            var instance = new WebAssembly.Instance(module, wasi.getImportObject());
            var e = instance.exports;
            var memory = e.memory;

            assert.strictEqual(wasi.start(instance), 0);

            function view() {
                return new DataView(memory.buffer);
            }

            function put(addr, text) {
                new Uint8Array(memory.buffer, addr, text.length).set(Buffer.from(text));
            }

            // open with the full regular-file rights; the preopen grants them
            put(200, 'file.txt');
            assert.strictEqual(wasi.path_open(3, 0, 200, 8, 0, FILE_RIGHTS, 0, 0, 300), 0);
            var fd = view().getUint32(300, true);
            assert.ok(fd > 3);
            assert.strictEqual(wasi.fd_fdstat_get(fd, 400), 0);
            assert.strictEqual(Number(view().getBigUint64(408, true)), FILE_RIGHTS);
            assert.strictEqual(Number(view().getBigUint64(416, true)), 0);

            // sync/no-op advisory calls
            assert.strictEqual(wasi.fd_datasync(fd), 0);
            assert.strictEqual(wasi.fd_sync(fd), 0);
            assert.strictEqual(wasi.fd_advise(fd, 0, 0, 3), 0);
            assert.strictEqual(wasi.fd_advise(fd, 0, 0, 9), 28);
            assert.strictEqual(wasi.fd_advise(99, 0, 0, 3), 8);
            assert.strictEqual(wasi.fd_allocate(fd, 0, 0), 0);
            // directories never carry FD_ALLOCATE
            assert.strictEqual(wasi.fd_allocate(3, 0, 0), 76);
            assert.strictEqual(wasi.fd_sync(99), 8);

            // fd flags are stored and reported by fd_fdstat_get
            if (isWindows) {
                // uvwasi refuses the call before it even looks at the
                // descriptor, so an unknown descriptor reports ENOSYS too
                assert.strictEqual(wasi.fd_fdstat_set_flags(fd, 1), 52);
                assert.strictEqual(wasi.fd_fdstat_set_flags(99, 1), 52);
                assert.strictEqual(wasi.fd_fdstat_get(fd, 400), 0);
                assert.strictEqual(view().getUint16(402, true), 0);
            } else {
                assert.strictEqual(wasi.fd_fdstat_set_flags(fd, 1), 0);
                assert.strictEqual(wasi.fd_fdstat_get(fd, 400), 0);
                assert.strictEqual(view().getUint16(402, true), 1);
                assert.strictEqual(wasi.fd_fdstat_set_flags(99, 1), 8);
            }

            // ATIM_NOW | MTIM_NOW
            assert.strictEqual(wasi.fd_filestat_set_times(fd, 0, 0, 10), 0);
            assert.strictEqual(wasi.fd_filestat_get(fd, 448), 0);
            var mtime = Number(view().getBigUint64(496, true));
            assert.ok(Math.abs(mtime - Date.now() * 1e6) < 5e9, `mtime ${mtime}`);
            assert.strictEqual(wasi.fd_filestat_set_times(3, 0, 0, 10), 31);
            assert.strictEqual(wasi.fd_filestat_set_times(99, 0, 0, 10), 8);

            // SET_ATIM | SET_MTIM on a path, following the entry
            assert.strictEqual(wasi.path_filestat_set_times(3, 0, 200, 8, 1e9, 2e9, 5), 0);
            var host = fs.statSync(file);
            assert.strictEqual(Math.round(host.mtimeMs), 2000);
            assert.strictEqual(Math.round(host.atimeMs), 1000);
            assert.strictEqual(wasi.path_filestat_set_times(99, 0, 200, 8, 0, 0, 0), 8);

            // descriptor readiness is reported without waiting
            new Uint8Array(memory.buffer, 520, 48).fill(0);
            view().setBigUint64(520, 123n, true);
            view().setUint8(528, 1);
            view().setUint32(536, fd, true);
            assert.strictEqual(wasi.poll_oneoff(520, 600, 1, 700), 0);
            assert.strictEqual(view().getUint32(700, true), 1);
            assert.strictEqual(view().getBigUint64(600, true), 123n);
            assert.strictEqual(view().getUint8(610), 1);
            assert.strictEqual(view().getUint16(608, true), 0);

            // an unknown descriptor is reported as BADF in the event
            view().setUint32(536, 99, true);
            assert.strictEqual(wasi.poll_oneoff(520, 600, 1, 700), 0);
            assert.strictEqual(view().getUint16(608, true), 8);

            // rights may only shrink; FD_READDIR is not part of a file's set
            assert.strictEqual(wasi.fd_fdstat_set_rights(fd, 1 << 14, 0), 76);
            assert.strictEqual(wasi.fd_fdstat_set_rights(fd, 2, 0), 0);
            assert.strictEqual(wasi.fd_fdstat_set_rights(99, 0, 0), 8);
            assert.strictEqual(wasi.fd_sync(fd), 76);

            // renumber moves the descriptor; the old number becomes free
            assert.strictEqual(wasi.fd_renumber(fd, 10), 0);
            assert.strictEqual(wasi.fd_close(fd), 8);
            assert.strictEqual(wasi.fd_fdstat_get(10, 480), 0);
            assert.strictEqual(wasi.fd_close(10), 0);

            // uvwasi checks the socket rights before the descriptor type, so a
            // non-socket descriptor reports NOTCAPABLE
            assert.strictEqual(wasi.sock_accept(3, 0, 720), 76);
            assert.strictEqual(wasi.sock_send(3, 0, 0, 0, 724), 76);
            assert.strictEqual(wasi.sock_recv(3, 0, 0, 0, 728, 732), 76);
            assert.strictEqual(wasi.sock_shutdown(3, 0), 76);

            // and the flag checks come before any descriptor lookup
            assert.strictEqual(wasi.sock_accept(99, 0x100, 720), 58);
            assert.strictEqual(wasi.sock_send(99, 0, 0, 1, 724), 28);
            assert.strictEqual(wasi.sock_recv(99, 0, 0, 1, 728, 732), 58);
            assert.strictEqual(wasi.sock_shutdown(99, 9), 58);
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('enforces descriptor and directory rights like uvwasi', () => {
        var dir = fs.mkdtempSync(path.join(require('os').tmpdir(), 'fibjs-wasi-'));
        fs.writeFileSync(path.join(dir, 'file.txt'), 'abc');
        fs.mkdirSync(path.join(dir, 'sub'));

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1', preopens: { '/': dir } });
            var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
                wasi.getImportObject());
            var memory = instance.exports.memory;
            wasi.initialize(instance);

            function view() {
                return new DataView(memory.buffer);
            }

            function put(addr, text) {
                new Uint8Array(memory.buffer, addr, text.length).set(Buffer.from(text));
            }

            put(200, 'file.txt');
            assert.strictEqual(wasi.path_open(3, 0, 200, 8, 0, 2, 0, 0, 300), 0);
            var fd = view().getUint32(300, true);

            // iovec { ptr: 1200, len: 8 }
            view().setUint32(1000, 1200, true);
            view().setUint32(1004, 8, true);

            // with FD_READ only, every other right is refused
            assert.strictEqual(wasi.fd_write(fd, 1000, 1, 760), 76);
            assert.strictEqual(wasi.fd_seek(fd, 0, 0, 760), 76);
            assert.strictEqual(wasi.fd_tell(fd, 760), 76);
            assert.strictEqual(wasi.fd_pread(fd, 1000, 1, 0, 760), 76);
            assert.strictEqual(wasi.fd_pwrite(fd, 1000, 1, 0, 760), 76);
            assert.strictEqual(wasi.fd_advise(fd, 0, 0, 0), 76);
            assert.strictEqual(wasi.fd_allocate(fd, 0, 0), 76);
            assert.strictEqual(wasi.fd_sync(fd), 76);
            assert.strictEqual(wasi.fd_datasync(fd), 76);
            // Windows reports NOSYS before it looks at the descriptor or the
            // rights, so the missing right is not reported there
            assert.strictEqual(wasi.fd_fdstat_set_flags(fd, 0), isWindows ? 52 : 76);
            assert.strictEqual(wasi.fd_filestat_get(fd, 448), 76);
            assert.strictEqual(wasi.fd_filestat_set_times(fd, 0, 0, 10), 76);
            assert.strictEqual(wasi.fd_filestat_set_size(fd, 0), 76);
            assert.strictEqual(wasi.fd_fdstat_set_rights(fd, 4, 0), 76);
            assert.strictEqual(wasi.fd_readdir(fd, 1300, 64, 0, 760), 76);

            // the granted right still works; fdstat itself needs no right
            assert.strictEqual(wasi.fd_read(fd, 1000, 1, 760), 0);
            assert.strictEqual(view().getUint32(760, true), 3);
            assert.strictEqual(wasi.fd_fdstat_get(fd, 400), 0);
            assert.strictEqual(Number(view().getBigUint64(408, true)), 2);
            assert.strictEqual(Number(view().getBigUint64(416, true)), 0);

            // shrink to nothing, then even read is refused
            assert.strictEqual(wasi.fd_fdstat_set_rights(fd, 0, 0), 0);
            assert.strictEqual(wasi.fd_read(fd, 1000, 1, 760), 76);
            assert.strictEqual(wasi.fd_close(fd), 0);

            // directories carry the path rights but not FD_READ/FD_WRITE
            assert.strictEqual(wasi.fd_write(3, 1000, 1, 760), 76);
            assert.strictEqual(wasi.fd_read(3, 1000, 1, 760), 76);
            // FD_ADVISE is part of the directory set, but fadvise on a directory is EBADF
            assert.strictEqual(wasi.fd_advise(3, 0, 0, 0), 8);
            assert.strictEqual(wasi.fd_filestat_set_size(3, 0), 76);
            assert.strictEqual(wasi.fd_filestat_get(3, 448), 0);
            assert.strictEqual(view().getUint8(464), 3); // directory

            // path_open: the request must fit the directory inheriting rights
            put(220, 'file.txt');
            assert.strictEqual(wasi.path_open(3, 0, 220, 8, 0, 1 << 29, 0, 0, 304), 76);
            assert.strictEqual(wasi.path_open(3, 0, 220, 8, 0, PREOPEN_INHERITING, 0, 0, 304), 0);
            var full = view().getUint32(304, true);
            assert.strictEqual(wasi.fd_fdstat_get(full, 400), 0);
            assert.strictEqual(Number(view().getBigUint64(408, true)), FILE_RIGHTS);
            assert.strictEqual(Number(view().getBigUint64(416, true)), 0);
            assert.strictEqual(wasi.fd_close(full), 0);

            // a plain file descriptor cannot act as a path root
            put(200, 'file.txt');
            assert.strictEqual(wasi.path_open(3, 0, 200, 8, 0, 2, 0, 0, 300), 0);
            var file_fd = view().getUint32(300, true);
            put(240, 'sub');
            assert.strictEqual(wasi.path_open(file_fd, 0, 240, 3, 0, 0, 0, 0, 308), 76);
            assert.strictEqual(wasi.fd_close(file_fd), 0);

            // opening a directory without O_DIRECTORY still reports a directory
            put(260, 'sub');
            assert.strictEqual(wasi.path_open(3, 0, 260, 3, 2, 1 << 14, 0, 0, 312), 0);
            var dfd = view().getUint32(312, true);
            assert.strictEqual(wasi.fd_fdstat_get(dfd, 400), 0);
            assert.strictEqual(view().getUint8(400), 3);
            assert.strictEqual(Number(view().getBigUint64(408, true)), 1 << 14);
            assert.strictEqual(wasi.fd_readdir(dfd, 1300, 512, 0, 760), 0);
            assert.strictEqual(wasi.fd_close(dfd), 0);
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('matches Node errno values for pointers, buffers and flags', () => {
        var dir = fs.mkdtempSync(path.join(require('os').tmpdir(), 'fibjs-wasi-'));
        fs.writeFileSync(path.join(dir, 'a.txt'), 'abc');
        fs.symlinkSync('a.txt', path.join(dir, 'link'));

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1', preopens: { '/': dir } });
            var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
                wasi.getImportObject());
            var memory = instance.exports.memory;
            wasi.initialize(instance);

            function view() {
                return new DataView(memory.buffer);
            }

            function put(addr, text) {
                new Uint8Array(memory.buffer, addr, text.length).set(Buffer.from(text));
            }

            // pointer range failures are EOVERFLOW (61), like uvwasi
            assert.strictEqual(wasi.args_get(0x7ffffff0, 0), 61);
            assert.strictEqual(wasi.environ_get(0x7ffffff0, 0), 61);
            assert.strictEqual(wasi.path_create_directory(3, 0x7ffffff0, 4), 61);
            assert.strictEqual(wasi.fd_read(0, 0x7ffffff0, 1, 760), 61);

            // readlink/prestat report ENOBUFS (42) when the buffer is too small
            put(200, 'link');
            assert.strictEqual(wasi.path_readlink(3, 200, 4, 1300, 2, 760), 42);
            assert.strictEqual(wasi.fd_prestat_dir_name(3, 1300, 0), 42);

            // fstflags are validated before the rights check
            put(210, 'a.txt');
            assert.strictEqual(wasi.path_open(3, 0, 210, 5, 0, 2 | 4, 0, 0, 300), 0);
            var fd = view().getUint32(300, true);
            assert.strictEqual(wasi.fd_filestat_set_times(fd, 0, 0, 16), 28);
            assert.strictEqual(wasi.path_filestat_set_times(3, 0, 210, 5, 0, 0, 16), 28);
            assert.strictEqual(wasi.fd_advise(fd, 0, 0, 9), 28);

            // bad arguments are still rejected after the rights pass
            assert.strictEqual(wasi.fd_seek(fd, 0, 3, 760), 28);
            assert.strictEqual(wasi.fd_seek(fd, -1, 0, 760), 28);
            assert.strictEqual(wasi.poll_oneoff(1700, 1800, 0, 760), 28);
            assert.strictEqual(wasi.fd_close(fd), 0);

            // path errors mirror uvwasi's host mapping
            put(220, 'missing');
            assert.strictEqual(wasi.path_open(3, 0, 220, 7, 0, 2, 0, 0, 304), 44);
            assert.strictEqual(wasi.path_open(3, 0, 220, 7, 1 | 4, 2, 0, 0, 304), 0);
            assert.strictEqual(wasi.fd_close(view().getUint32(304, true)), 0);
            put(230, 'a.txt');
            assert.strictEqual(wasi.path_open(3, 0, 230, 5, 2, 2, 0, 0, 304), 54);
            assert.strictEqual(wasi.path_readlink(3, 230, 5, 1300, 8, 760), 28);
            assert.strictEqual(wasi.path_open(3, 0, 230, 5, 0, 2, 0, 0, 304), 0);
            assert.strictEqual(wasi.fd_close(view().getUint32(304, true)), 0);
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('reads options like Node and rejects invalid ones', () => {
        var { WASI } = require('wasi');

        var wasi = new WASI({
            version: 'preview1',
            args: ['a', 5],
            env: { A: '1', B: undefined, C: 7 }
        });
        var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
            wasi.getImportObject());
        var view = new DataView(instance.exports.memory.buffer);
        wasi.initialize(instance);

        // 'a' and '5' -> 4 bytes; undefined env values are dropped
        assert.strictEqual(wasi.args_sizes_get(700, 704), 0);
        assert.strictEqual(view.getUint32(700, true), 2);
        assert.strictEqual(view.getUint32(704, true), 4);
        assert.strictEqual(wasi.environ_sizes_get(700, 704), 0);
        assert.strictEqual(view.getUint32(700, true), 2);
        assert.strictEqual(view.getUint32(704, true), 8);

        var legacy = new WASI({ version: 'unstable' });
        assert.deepStrictEqual(Object.keys(legacy.getImportObject()), ['wasi_unstable']);
        assert.strictEqual(legacy.wasiImport, legacy.getImportObject().wasi_unstable);

        assert.throws(function() { new WASI({ version: 'wasi_unstable' }); }, /unsupported version/);
        assert.throws(function() { new WASI({ version: 'preview1', args: 5 }); }, /args/);
        assert.throws(function() { new WASI({ version: 'preview1', env: 5 }); }, /env/);
        assert.throws(function() { new WASI({ version: 'preview1', preopens: 5 }); }, /preopens/);
        assert.throws(function() {
            new WASI({ version: 'preview1', returnOnExit: 1 });
        }, /returnOnExit/);
        assert.throws(function() {
            new WASI({ version: 'preview1', sockets: 5 });
        }, /sockets/);
        assert.throws(function() {
            new WASI({ version: 'preview1', sockets: [{ address: '127.0.0.1' }] });
        }, /port/);
        assert.throws(function() {
            new WASI({ version: 'preview1', sockets: [{ address: '127.0.0.1', port: 70000 }] });
        }, /65535/);

        // Node requires an explicit version and rejects negative stdio fds
        assert.throws(function() { new WASI({}); }, /version/);
        assert.throws(function() { new WASI({}); }, { number: 20002 });
        assert.throws(function() {
            new WASI({ version: 'preview1', sockets: [{ address: '127.0.0.1' }] });
        }, /port/);
        assert.throws(function() {
            new WASI({ version: 'preview1', sockets: [{ address: '127.0.0.1' }] });
        }, { number: 20002 });
        assert.throws(function() {
            new WASI({ version: 'preview1', stdin: -1 });
        }, /out of range/);
        assert.throws(function() {
            new WASI({ version: 'preview1', preopens: { '/': '__/not/real/path' } });
        }, /preopen/);
    });

    it('mirrors uvwasi argument checks and the not-started error', () => {
        var { WASI } = require('wasi');

        // calling a syscall before the memory is bound is a programming error
        var wasi = new WASI({ version: 'preview1' });
        assert.throws(function() { wasi.fd_write(1, 0, 0, 0); }, /has not been called/);
        assert.throws(function() { wasi.sched_yield(); }, /has not been called/);
        assert.throws(function() { wasi.clock_time_get(0, 1, 0); }, /has not been called/);

        var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
            wasi.getImportObject());
        var memory = instance.exports.memory;
        wasi.initialize(instance);

        function view() {
            return new DataView(memory.buffer);
        }

        // prestat distinguishes a missing fd from a descriptor that is not a
        // preopen, exactly like uvwasi
        assert.strictEqual(wasi.fd_prestat_get(0, 700), 28);
        assert.strictEqual(wasi.fd_prestat_get(99, 700), 8);
        assert.strictEqual(wasi.fd_prestat_dir_name(0, 200, 8), 8);

        // proc_raise maps WASI signal numbers; an unknown one is NOSYS and
        // must not touch the process
        assert.strictEqual(typeof wasi.proc_raise, 'function');
        assert.strictEqual(wasi.proc_raise(200), 52);

        // a short read must not overwrite the unread tail of the iovec
        var dir = fs.mkdtempSync(path.join(require('os').tmpdir(), 'fibjs-wasi-'));
        fs.writeFileSync(path.join(dir, 'a.txt'), 'hello');
        try {
            var reader = new WASI({ version: 'preview1', preopens: { '/': dir } });
            var rinst = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
                reader.getImportObject());
            var rview = new DataView(rinst.exports.memory.buffer);
            var rbuf = new Uint8Array(rinst.exports.memory.buffer);

            function put(addr, text) {
                rbuf.set(Buffer.from(text), addr);
            }
            function iovec(slot, addr, len) {
                rview.setUint32(slot, addr, true);
                rview.setUint32(slot + 4, len, true);
            }

            reader.initialize(rinst);
            put(400, 'a.txt');
            assert.strictEqual(reader.path_open(3, 0, 400, 5, 0, 0x08E001FF, 0, 0, 300),
                0);
            var fd = rview.getUint32(300, true);

            rbuf.fill(0x5a, 200, 216); // sentinel pattern
            iovec(100, 200, 16);
            assert.strictEqual(reader.fd_read(fd, 100, 1, 304), 0);
            assert.strictEqual(rview.getUint32(304, true), 5);
            assert.strictEqual(Buffer.from(rbuf.slice(200, 205)).toString(), 'hello');
            // the eleven bytes past the data keep the sentinel
            assert.strictEqual(Buffer.from(rbuf.slice(205, 216)).toString(), 'Z'.repeat(11));

            assert.strictEqual(reader.fd_close(fd), 0);
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('honours the returnOnExit option', () => {
        var { WASI } = require('wasi');

        // default (true): start() reports the guest exit code
        var wasi = new WASI({ version: 'preview1' });
        var instance = new WebAssembly.Instance(new WebAssembly.Module(EXIT_FIXTURE),
            wasi.getImportObject());
        assert.strictEqual(wasi.start(instance), 7);

        // false: proc_exit terminates the host process
        var cp = require('child_process');
        var script = [
            "var { WASI } = require('wasi');",
            "var b = Buffer.from('" + EXIT_FIXTURE.toString('base64') + "', 'base64');",
            "var w = new WASI({ version: 'preview1', returnOnExit: false });",
            "var i = new WebAssembly.Instance(new WebAssembly.Module(b), w.getImportObject());",
            "w.start(i);",
            "console.log('proc_exit did not terminate');"
        ].join('\n');

        var child = cp.spawnSync(process.execPath, ['-e', script], { encoding: 'utf8' });
        assert.strictEqual(child.status, 7);
        assert.ok(!/did not terminate/.test(String(child.stdout)));
    });

    it('validates command/reactor bindings and supports a memory override', () => {
        var { WASI } = require('wasi');
        var MemoryModule = new WebAssembly.Module(MEMORY_FIXTURE);
        var StartModule = new WebAssembly.Module(START_FIXTURE);
        var BothModule = new WebAssembly.Module(BOTH_FIXTURE);

        // command model requires _start
        var w1 = new WASI({ version: 'preview1' });
        var i1 = new WebAssembly.Instance(MemoryModule, w1.getImportObject());
        assert.throws(function() { w1.start(i1); }, /_start/);

        // _start runs once; the second call reports the bound state
        var w2 = new WASI({ version: 'preview1' });
        var i2 = new WebAssembly.Instance(StartModule, w2.getImportObject());
        assert.strictEqual(w2.start(i2), 0);
        assert.throws(function() { w2.start(i2); }, /already started/);

        // reactor model rejects a _start export
        var w3 = new WASI({ version: 'preview1' });
        var i3 = new WebAssembly.Instance(StartModule, w3.getImportObject());
        assert.throws(function() { w3.initialize(i3); }, /_start/);

        // command model rejects a _initialize export
        var w4 = new WASI({ version: 'preview1' });
        var i4 = new WebAssembly.Instance(BothModule, w4.getImportObject());
        assert.throws(function() { w4.start(i4); }, /_initialize/);

        // initialize without either export is valid
        var w5 = new WASI({ version: 'preview1' });
        var i5 = new WebAssembly.Instance(MemoryModule, w5.getImportObject());
        w5.initialize(i5);

        // finalizeBindings accepts a memory override and binds only once
        var w6 = new WASI({ version: 'preview1' });
        var alt = new WebAssembly.Memory({ initial: 1 });
        w6.finalizeBindings(i5, { memory: alt });
        assert.strictEqual(w6.random_get(0, 8), 0);
        assert.ok(new Uint8Array(alt.buffer, 0, 8).some(function(b) { return b !== 0; }));
        assert.throws(function() {
            w6.finalizeBindings(i5, { memory: alt });
        }, /already started/);
    });

    it('tracks positions across read, write, seek and allocate', () => {
        var dir = fs.mkdtempSync(path.join(require('os').tmpdir(), 'fibjs-wasi-'));
        var file = path.join(dir, 'a.txt');
        fs.writeFileSync(file, 'hello');

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1', preopens: { '/': dir } });
            var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
                wasi.getImportObject());
            var memory = instance.exports.memory;
            wasi.initialize(instance);

            function view() {
                return new DataView(memory.buffer);
            }

            function put(addr, text) {
                new Uint8Array(memory.buffer, addr, text.length).set(Buffer.from(text));
            }

            function text(addr, len) {
                return Buffer.from(new Uint8Array(memory.buffer, addr, len)).toString();
            }

            put(200, 'a.txt');
            assert.strictEqual(wasi.path_open(3, 0, 200, 5, 0, FILE_RIGHTS, 0, 0, 300), 0);
            var fd = view().getUint32(300, true);

            // read advances the shared position
            view().setUint32(1000, 1200, true);
            view().setUint32(1004, 8, true);
            assert.strictEqual(wasi.fd_read(fd, 1000, 1, 760), 0);
            assert.strictEqual(view().getUint32(760, true), 5);
            assert.strictEqual(text(1200, 5), 'hello');
            assert.strictEqual(wasi.fd_tell(fd, 760), 0);
            assert.strictEqual(Number(view().getBigUint64(760, true)), 5);

            // seek sets it, read moves it again
            assert.strictEqual(wasi.fd_seek(fd, 0, 0, 760), 0);
            view().setUint32(1004, 2, true);
            assert.strictEqual(wasi.fd_read(fd, 1000, 1, 760), 0);
            assert.strictEqual(text(1200, 2), 'he');
            assert.strictEqual(wasi.fd_tell(fd, 760), 0);
            assert.strictEqual(Number(view().getBigUint64(760, true)), 2);

            // pwrite does not move it and can extend the file
            new Uint8Array(memory.buffer, 1300, 1).set(Buffer.from('X'));
            view().setUint32(1010, 1300, true);
            view().setUint32(1014, 1, true);
            assert.strictEqual(wasi.fd_pwrite(fd, 1010, 1, 6, 760), 0);
            assert.strictEqual(view().getUint32(760, true), 1);
            assert.strictEqual(wasi.fd_tell(fd, 760), 0);
            assert.strictEqual(Number(view().getBigUint64(760, true)), 2);
            assert.strictEqual(fs.statSync(file).size, 7);

            // set_size truncates, allocate grows the host file
            assert.strictEqual(wasi.fd_filestat_set_size(fd, 2), 0);
            assert.strictEqual(fs.statSync(file).size, 2);
            assert.strictEqual(wasi.fd_allocate(fd, 0, 8), 0);
            assert.strictEqual(fs.statSync(file).size, 8);

            // append ignores the tracked offset and moves the host position;
            // Windows cannot enable append-on-write, so there the call reports
            // ENOSYS and the write lands at the tracked position instead
            new Uint8Array(memory.buffer, 1300, 1).set(Buffer.from('Z'));
            if (isWindows) {
                assert.strictEqual(wasi.fd_fdstat_set_flags(fd, 1), 52);
                assert.strictEqual(wasi.fd_write(fd, 1010, 1, 760), 0);
                assert.strictEqual(view().getUint32(760, true), 1);
                assert.strictEqual(fs.statSync(file).size, 8);
                assert.strictEqual(wasi.fd_tell(fd, 760), 0);
                assert.strictEqual(Number(view().getBigUint64(760, true)), 3);
            } else {
                assert.strictEqual(wasi.fd_fdstat_set_flags(fd, 1), 0);
                assert.strictEqual(wasi.fd_write(fd, 1010, 1, 760), 0);
                assert.strictEqual(view().getUint32(760, true), 1);
                assert.strictEqual(fs.statSync(file).size, 9);
                assert.strictEqual(wasi.fd_tell(fd, 760), 0);
                assert.strictEqual(Number(view().getBigUint64(760, true)), 9);

                // seek from the end sees the appended byte
                assert.strictEqual(wasi.fd_seek(fd, -1, 2, 760), 0);
                assert.strictEqual(wasi.fd_read(fd, 1000, 1, 760), 0);
                assert.strictEqual(text(1200, 1), 'Z');
            }
            assert.strictEqual(wasi.fd_close(fd), 0);
        } finally {
            rmSyncRetry(dir);
        }
    });

    it('accepts, receives and sends on preopened sockets', () => {
        var { WASI } = require('wasi');
        var net = require('net');

        // Find a port the host can bind; the constructor throws otherwise.
        var wasi = null;
        var port = 0;
        for (var i = 0; i < 32 && !wasi; i++) {
            var candidate = 20000 + Math.floor(Math.random() * 20000);
            try {
                wasi = new WASI({
                    version: 'preview1',
                    sockets: [{ address: '127.0.0.1', port: candidate }]
                });
                port = candidate;
            } catch (e) {
                assert.ok(/cannot listen/.test(String(e.message)), String(e.message));
            }
        }
        assert.ok(wasi, 'could not bind a test port');

        var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
            wasi.getImportObject());
        var memory = instance.exports.memory;
        wasi.initialize(instance);

        function view() {
            return new DataView(memory.buffer);
        }

        function put(addr, text) {
            new Uint8Array(memory.buffer, addr, text.length).set(Buffer.from(text));
        }

        function text(addr, len) {
            return Buffer.from(new Uint8Array(memory.buffer, addr, len)).toString();
        }

        var client = net.connect({ host: '127.0.0.1', port: port });

        try {
            // no directory preopens, so the listener is guest fd 3
            assert.strictEqual(wasi.sock_accept(3, 0, 300), 0);
            var conn = view().getUint32(300, true);
            assert.ok(conn > 3);

            assert.strictEqual(wasi.fd_fdstat_get(conn, 400), 0);
            assert.strictEqual(view().getUint8(400), 6); // socket stream

            // nothing is pending now, so a non-blocking accept reports EAGAIN
            assert.strictEqual(wasi.sock_accept(3, 4, 304), 6);

            // host -> guest
            client.send(Buffer.from('ping'));
            view().setUint32(1000, 1200, true);
            view().setUint32(1004, 16, true);
            assert.strictEqual(wasi.sock_recv(conn, 1000, 1, 0, 760, 764), 0);
            var nread = view().getUint32(760, true);
            assert.ok(nread >= 4, `nread ${nread}`);
            assert.strictEqual(text(1200, 4), 'ping');

            // guest -> host
            new Uint8Array(memory.buffer, 1300, 4).set(Buffer.from('pong'));
            view().setUint32(1010, 1300, true);
            view().setUint32(1014, 4, true);
            assert.strictEqual(wasi.sock_send(conn, 1010, 1, 0, 760), 0);
            assert.strictEqual(view().getUint32(760, true), 4);
            assert.strictEqual(client.recv(4).toString(), 'pong');

            // flag validation mirrors uvwasi
            assert.strictEqual(wasi.sock_recv(conn, 1000, 1, 1, 760, 764), 58);
            assert.strictEqual(wasi.sock_send(conn, 1010, 1, 1, 760), 28);
            // uvwasi implements only SHUT_WR; SHUT_RD reports NOTSUP
            assert.strictEqual(wasi.sock_shutdown(conn, 1), 58);
            assert.strictEqual(wasi.sock_shutdown(conn, 0), 0);
            assert.strictEqual(wasi.sock_shutdown(conn, 2), 0);

            // a listener is a socket but not a connection
            assert.strictEqual(wasi.sock_recv(3, 1000, 1, 0, 760, 764), 57);
            assert.strictEqual(wasi.sock_send(3, 1010, 1, 0, 760), 57);

            assert.strictEqual(wasi.fd_close(conn), 0);
        } finally {
            try {
                client.close();
            } catch (e) {
                // already closed
            }
        }
    });

    // Windows waits on the socket handle through uv_poll_init_socket, so both
    // platforms block here; only the handle the host watches differs.
    it('blocks in poll_oneoff until a socket becomes readable', () => {
        var { WASI } = require('wasi');
        var net = require('net');

        var wasi = null;
        var port = 0;
        for (var i = 0; i < 32 && !wasi; i++) {
            var candidate = 20000 + Math.floor(Math.random() * 20000);
            try {
                wasi = new WASI({
                    version: 'preview1',
                    sockets: [{ address: '127.0.0.1', port: candidate }]
                });
                port = candidate;
            } catch (e) {
                // retry another port
            }
        }
        assert.ok(wasi, 'could not bind a test port');

        var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
            wasi.getImportObject());
        var memory = instance.exports.memory;
        wasi.initialize(instance);

        function view() {
            return new DataView(memory.buffer);
        }

        var client = net.connect({ host: '127.0.0.1', port: port });

        try {
            assert.strictEqual(wasi.sock_accept(3, 0, 300), 0);
            var conn = view().getUint32(300, true);

            // a read subscription is not ready until the host sends data
            new Uint8Array(memory.buffer, 520, 48).fill(0);
            view().setBigUint64(520, 99n, true);
            view().setUint8(528, 1);
            view().setUint32(536, conn, true);

            var started = Date.now();
            setTimeout(function() {
                client.send(Buffer.from('wake'));
            }, 40);

            assert.strictEqual(wasi.poll_oneoff(520, 600, 1, 700), 0);
            var elapsed = Date.now() - started;
            assert.ok(elapsed >= 30, `poll returned before the data arrived (${elapsed}ms)`);
            assert.ok(elapsed < 2000, `poll did not wake up (${elapsed}ms)`);
            assert.strictEqual(view().getUint32(700, true), 1);
            assert.strictEqual(view().getBigUint64(600, true), 99n);
            assert.strictEqual(view().getUint8(610), 1);
            assert.strictEqual(view().getUint16(608, true), 0);

            // the data is still queued for sock_recv
            view().setUint32(1000, 1200, true);
            view().setUint32(1004, 16, true);
            assert.strictEqual(wasi.sock_recv(conn, 1000, 1, 0, 760, 764), 0);
            assert.strictEqual(Buffer.from(new Uint8Array(memory.buffer, 1200,
                view().getUint32(760, true))).toString().slice(0, 4), 'wake');

            // clock + not-ready descriptor: the timer wins and one clock event
            // is reported
            new Uint8Array(memory.buffer, 1700, 48 * 2).fill(0);
            view().setBigUint64(1700, 1n, true);
            view().setUint8(1708, 0);
            view().setBigUint64(1724, 30000000n, true); // 30ms
            view().setBigUint64(1748, 2n, true);
            view().setUint8(1756, 1);
            view().setUint32(1764, conn, true);

            var clock_start = Date.now();
            assert.strictEqual(wasi.poll_oneoff(1700, 1800, 2, 700), 0);
            var clock_elapsed = Date.now() - clock_start;
            assert.ok(clock_elapsed >= 20, `clock returned too early (${clock_elapsed}ms)`);
            assert.strictEqual(view().getUint32(700, true), 1);
            assert.strictEqual(view().getUint8(1810), 0); // clock event

            assert.strictEqual(wasi.fd_close(conn), 0);
            assert.strictEqual(wasi.fd_close(3), 0);
        } finally {
            try {
                client.close();
            } catch (e) {
                // already closed
            }
        }
    });

    it('keeps other syscalls running while a fiber blocks in sock_accept', () => {
        var { WASI } = require('wasi');
        var coroutine = require('coroutine');

        // the host is FIBER_FREE, so a fiber parked inside a blocking syscall
        // must not hold the instance lock against the other fibers
        var wasi = new WASI({ version: 'preview1', sockets: [{ address: '127.0.0.1', port: 0 }] });
        var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
            wasi.getImportObject());
        var memory = instance.exports.memory;
        wasi.initialize(instance);

        function view() {
            return new DataView(memory.buffer);
        }

        var accept_ret = null;
        var fiber = coroutine.start(function() {
            accept_ret = wasi.sock_accept(3, 0, 300);
        });

        coroutine.sleep(100);

        var started = Date.now();
        assert.strictEqual(wasi.fd_fdstat_get(3, 400), 0);
        assert.strictEqual(view().getUint8(400), 6); // still a socket stream
        assert.strictEqual(wasi.fd_close(3), 0);
        var elapsed = Date.now() - started;
        assert.ok(elapsed < 1000, `syscalls serialized behind the blocked fiber (${elapsed}ms)`);

        fiber.join();

        // the close wakes the parked accept with BADF
        assert.strictEqual(accept_ret, 8);
    });

    it('settles each accept waiter exactly once', () => {
        var { WASI } = require('wasi');
        var net = require('net');
        var coroutine = require('coroutine');

        var wasi = null;
        var port = 0;
        for (var i = 0; i < 32 && !wasi; i++) {
            var candidate = 20000 + Math.floor(Math.random() * 20000);
            try {
                wasi = new WASI({
                    version: 'preview1',
                    sockets: [{ address: '127.0.0.1', port: candidate }]
                });
                port = candidate;
            } catch (e) {
                // retry another port
            }
        }
        assert.ok(wasi, 'could not bind a test port');

        var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
            wasi.getImportObject());
        var memory = instance.exports.memory;
        wasi.initialize(instance);

        function view() {
            return new DataView(memory.buffer);
        }

        // two fibers wait in sock_accept on the same listener
        var results = [null, null];
        var out_addrs = [300, 304];
        var fibers = out_addrs.map(function(addr, index) {
            var fiber = coroutine.start(function() {
                results[index] = wasi.sock_accept(3, 0, addr);
            });
            coroutine.sleep(50);
            return fiber;
        });

        var client = net.connect({ host: '127.0.0.1', port: port });
        try {
            coroutine.sleep(100);

            // one connection settles exactly one waiter
            var accepted = results.filter(function(v) { return v === 0; }).length;
            assert.strictEqual(accepted, 1, `accept results ${JSON.stringify(results)}`);

            var conn = results[0] === 0
                ? view().getUint32(out_addrs[0], true)
                : view().getUint32(out_addrs[1], true);
            assert.ok(conn > 3);

            // the still-parked waiter is woken by the close with BADF
            assert.strictEqual(wasi.fd_close(3), 0);
            fibers.forEach(function(fiber) { fiber.join(); });
            assert.strictEqual(results.filter(function(v) { return v === 8; }).length, 1,
                `accept results ${JSON.stringify(results)}`);

            assert.strictEqual(wasi.fd_close(conn), 0);
        } finally {
            try {
                client.close();
            } catch (e) {
                // already closed
            }
        }
    });

    it('maps the stdin/stdout/stderr options to host descriptors', () => {
        var dir = fs.mkdtempSync(path.join(require('os').tmpdir(), 'fibjs-wasi-'));
        var input = path.join(dir, 'in.txt');
        var output = path.join(dir, 'out.txt');
        fs.writeFileSync(input, 'stdin-data');

        var handle_in = fs.openSync(input, 'r');
        var handle_out = fs.openSync(output, 'w');

        try {
            var { WASI } = require('wasi');
            var wasi = new WASI({ version: 'preview1', stdin: handle_in.fd, stdout: handle_out.fd });
            var instance = new WebAssembly.Instance(new WebAssembly.Module(MEMORY_FIXTURE),
                wasi.getImportObject());
            var memory = instance.exports.memory;
            wasi.initialize(instance);

            function view() {
                return new DataView(memory.buffer);
            }

            // guest fd 0 is the configured stdin descriptor
            view().setUint32(1000, 1200, true);
            view().setUint32(1004, 10, true);
            assert.strictEqual(wasi.fd_read(0, 1000, 1, 760), 0);
            assert.strictEqual(view().getUint32(760, true), 10);
            assert.strictEqual(Buffer.from(new Uint8Array(memory.buffer, 1200, 10)).toString(),
                'stdin-data');
            assert.strictEqual(wasi.fd_fdstat_get(0, 400), 0);
            assert.strictEqual(view().getUint8(400), 4); // regular file

            // guest fd 1 writes through the configured stdout descriptor
            new Uint8Array(memory.buffer, 1300, 1).set(Buffer.from('x'));
            view().setUint32(1010, 1300, true);
            view().setUint32(1014, 1, true);
            assert.strictEqual(wasi.fd_write(1, 1010, 1, 760), 0);
            assert.strictEqual(view().getUint32(760, true), 1);
        } finally {
            handle_in.closeSync();
            handle_out.closeSync();
            try {
                assert.strictEqual(fs.readFileSync(output, 'utf8'), 'x');
            } finally {
                rmSyncRetry(dir);
            }
        }
    });
});
