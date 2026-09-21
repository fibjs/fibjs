var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var fs = require('fs');
var io = require('io');
var path = require('path');
var process = require('process');
var child_process = require('child_process');
var zip = require('zip');
var util = require('util');
var coroutine = require('coroutine');

var execPath = process.execPath;
const $ = child_process.sh;

describe("selfzip", () => {
    var path_cnt = 0;
    var paths = [];
    var inject;

    before(() => {
        try {
            fs.unlink(path.join(__dirname, '_helpers', "fib-inject", "addon", "fib-inject.node"));
        } catch (e) { }
        child_process.execFile(execPath, [path.join(__dirname, '_helpers', "fib-inject", "install.js")]);
        inject = require('./_helpers/fib-inject').inject;
    });

    after(() => {
        paths.forEach(p => {
            try {
                fs.unlink(p);
            } catch (e) { }
        });
    });

    function get_path(uni) {
        var p = path.join(__dirname, `${uni ? "测试" : "test"}_${path_cnt++}_${path.basename(execPath)}`);
        paths.push(p);
        return p;
    }

    function package(testPath, data) {
        fs.copyFile(execPath, testPath);
        inject(testPath, 'APP', data, {
            sentinelFuse: "FIBJS_FUSE_fe21d3488eb4cdf267e5ea624f2006ce"
        });
        if (process.platform === 'darwin')
            child_process.execFile('codesign', ['-s', '-', testPath]);

        if (process.platform !== 'win32')
            fs.chmod(testPath, 511);
    }

    function package_legacy(testPath, data) {
        const sentinelFuse = "FIBJS_FUSE_fe21d3488eb4cdf267e5ea624f2006ce";

        fs.copyFile(execPath, testPath);
        var buffer = fs.readFile(testPath);
        const firstSentinel = buffer.indexOf(sentinelFuse);

        const hasResourceIndex = firstSentinel + sentinelFuse.length + 1;
        buffer[hasResourceIndex] = "2".charCodeAt(0);

        fs.writeFile(testPath, buffer);

        if (process.platform === 'darwin')
            child_process.execFile('codesign', ['-s', '-', testPath]);

        if (process.platform !== 'win32')
            fs.chmod(testPath, 511);

        fs.appendFile(testPath, data);
    }

    function test_selfzip(testPath, script, argv, compile, legacy) {
        var ms = new io.MemoryStream();
        var zf = zip.open(ms, 'w');
        if (compile)
            zf.write(util.compile('test.js', script, 1), 'index.jsc');
        else
            zf.write(new Buffer(script), 'index.js');
        zf.close();

        ms.rewind();

        if (legacy)
            package_legacy(testPath, ms.readAll());
        else
            package(testPath, ms.readAll());

        return child_process.run(testPath, argv);
    }

    function test_suite(name, compile, legacy) {
        if (!legacy) {
            // The packed modes rebuild the executable with the fib-inject addon
            // (LIEF) instead of appending the payload.  fib-inject 2.1.4 vendors
            // LIEF 0.15.1, which cannot shift the relocations of every
            // architecture, so the moved ELF comes out broken:
            //   * mips64 / loong64: nothing is patched - upstream LIEF has no
            //     MIPS64/LoongArch support either, so this needs a LIEF patch;
            //   * riscv64: injection is disabled in fib-inject itself (upstream
            //     LIEF does have ARCH::RISCV since 0.17.0);
            //   * ppc64: LIEF 0.15.1 has no ARCH::PPC64 case (upstream has it
            //     since 0.17.0), but even with that patch applied the packed
            //     binary still crashes at startup - the GOT fix-up gives up on
            //     ppc64 (DT_PLTGOT points at the NOBITS .plt), so this needs
            //     more work in fib-inject first;
            //   * musl/arm64: the released addon cannot inject into that binary
            //     at all ("Resource was not injected correctly").
            // Drop an architecture from this list once fib-inject ships a
            // release with the matching support (bump the version in
            // _helpers/fib-inject/package.json as well).
            if (process.arch === 'mips64' || process.arch === 'loong64'
                || process.arch === 'riscv64' || process.arch === 'ppc64')
                return;

            if (process.versions.musl && process.arch === 'arm64')
                return;
        }

        describe(name, () => {
            it("zip", () => {
                assert.equal(test_selfzip(get_path(false), 'process.exit(65);', [], compile, legacy), 65);
                assert.equal(test_selfzip(get_path(true), 'process.exit(65);', [], compile, legacy), 65);
            });

            it("argv", () => {
                assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv[1] === process.argv[0] + "$");', [], compile, legacy), 1);
                assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv[1] === process.argv[0] + "$");', [], compile, legacy), 1);
            });

            it("argv size", () => {
                assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv.length);', [1, 2, 3, 4, 5], compile, legacy), 7);
                assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv.length);', [1, 2, 3, 4, 5], compile, legacy), 7);
            });

            it("custom argv", () => {
                assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv[2]);', [94], compile, legacy), 94);
                assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv[2]);', [94], compile, legacy), 94);
            });
        });
    }

    test_suite("js", false);
    test_suite("jsc", true);
    test_suite("js-legacy", false, true);
    test_suite("jsc-legacy", true, true);
});

