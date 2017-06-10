var test = require("test");
test.setup();

var fs = require('fs');
var io = require('io');
var path = require('path');
var process = require('process');
var zip = require('zip');

var execPath = process.execPath;
var testPath = path.join(__dirname, path.basename(execPath));

describe("selfzip", () => {
    after(() => {
        try {
            fs.unlink(testPath);
        } catch (e) {}
    });

    function test_selfzip(script, argv) {
        var ms = new io.MemoryStream();
        var zf = zip.open(ms, 'w');
        zf.write(new Buffer(script), 'index.js');
        zf.close();

        ms.rewind();

        fs.copy(execPath, testPath);
        fs.appendFile(testPath, ms.readAll());

        if (process.platform !== 'win32')
            fs.chmod(testPath, 511);

        var r = process.run(testPath, argv);

        try {
            fs.unlink(testPath);
        } catch (e) {}

        return r;
    }

    it("zip", () => {
        assert.equal(test_selfzip('process.exit(65);', []), 65);
    });

    it("argv", () => {
        assert.equal(test_selfzip('process.exit(process.argv[1] === process.argv[0] + "$");', []), 1);
    });

    it("argv size", () => {
        assert.equal(test_selfzip('process.exit(process.argv.length);', [1, 2, 3, 4, 5]), 7);
    });

    it("custom argv", () => {
        assert.equal(test_selfzip('process.exit(process.argv[2]);', [94]), 94);
    });
});

// test.run(console.DEBUG);