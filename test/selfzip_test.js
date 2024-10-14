var test = require("test");
test.setup();

var fs = require('fs');
var io = require('io');
var path = require('path');
var process = require('process');
var child_process = require('child_process');
var zip = require('zip');
var util = require('util');
var coroutine = require('coroutine');
var postject = require('./_helpers/postject');

var execPath = process.execPath;
const $ = child_process.sh;

var inject = util.sync(postject.inject);

describe("selfzip", () => {
    var path_cnt = 0;
    var paths = [];

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

    describe("js", () => {
        function test_selfzip(testPath, script, argv) {
            var ms = new io.MemoryStream();
            var zf = zip.open(ms, 'w');
            zf.write(new Buffer(script), 'index.js');
            zf.close();

            ms.rewind();

            fs.copyFile(execPath, testPath);
            inject(testPath, 'APP', ms.readAll(), {
                sentinelFuse: "FIBJS_FUSE_fe21d3488eb4cdf267e5ea624f2006ce",
                overwrite: true
            });
            if (process.platform === 'darwin')
                $`codesign -s - ${testPath}`;

            if (process.platform !== 'win32')
                fs.chmod(testPath, 511);

            return child_process.run(testPath, argv);
        }

        it("zip", () => {
            assert.equal(test_selfzip(get_path(false), 'process.exit(65);', []), 65);
            assert.equal(test_selfzip(get_path(true), 'process.exit(65);', []), 65);
        });

        it("argv", () => {
            assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv[1] === process.argv[0] + "$");', []), 1);
            assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv[1] === process.argv[0] + "$");', []), 1);
        });

        it("argv size", () => {
            assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv.length);', [1, 2, 3, 4, 5]), 7);
            assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv.length);', [1, 2, 3, 4, 5]), 7);
        });

        it("custom argv", () => {
            assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv[2]);', [94]), 94);
            assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv[2]);', [94]), 94);
        });
    });

    describe("jsc", () => {
        function test_selfzip(testPath, script, argv) {
            var ms = new io.MemoryStream();
            var zf = zip.open(ms, 'w');
            zf.write(util.compile('test.js', script, 1), 'index.jsc');
            zf.close();

            ms.rewind();

            fs.copyFile(execPath, testPath);
            inject(testPath, 'APP', ms.readAll(), {
                sentinelFuse: "FIBJS_FUSE_fe21d3488eb4cdf267e5ea624f2006ce",
                overwrite: true
            });
            if (process.platform === 'darwin')
                $`codesign -s - ${testPath}`;

            if (process.platform !== 'win32')
                fs.chmod(testPath, 511);

            return child_process.run(testPath, argv);
        }

        it("zip", () => {
            assert.equal(test_selfzip(get_path(false), 'process.exit(65);', []), 65);
            assert.equal(test_selfzip(get_path(true), 'process.exit(65);', []), 65);
        });

        it("argv", () => {
            assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv[1] === process.argv[0] + "$");', []), 1);
            assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv[1] === process.argv[0] + "$");', []), 1);
        });

        it("argv size", () => {
            assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv.length);', [1, 2, 3, 4, 5]), 7);
            assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv.length);', [1, 2, 3, 4, 5]), 7);
        });

        it("custom argv", () => {
            assert.equal(test_selfzip(get_path(false), 'process.exit(process.argv[2]);', [94]), 94);
            assert.equal(test_selfzip(get_path(true), 'process.exit(process.argv[2]);', [94]), 94);
        });
    });
});

require.main === module && test.run(console.DEBUG);