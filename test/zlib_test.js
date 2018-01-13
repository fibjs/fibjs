var test = require("test");
test.setup();

var zlib = require('zlib');
var io = require('io');
var fs = require('fs');
var path = require('path');

var M = 102400;
var b = new Buffer();
b.resize(M);

var i;

for (i = 0; i < M; i++)
    b[i] = Math.random() * 70 + 33;

describe("zlib", () => {
    it("inflate", () => {
        assert.deepEqual(zlib.inflate(zlib.deflate(b)), b);
    });

    it("gzip", () => {
        assert.deepEqual(zlib.gunzip(zlib.gzip(b)), b);
    });

    it("deflateTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.deflateTo(b, stm);
        stm.rewind();
        assert.deepEqual(zlib.inflate(stm.readAll()), b);
    });

    it("inflateTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.inflateTo(zlib.deflate(b), stm);
        stm.rewind();
        assert.deepEqual(stm.readAll(), b);
    });

    it("gzipTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.gzipTo(b, stm);
        stm.rewind();
        assert.deepEqual(zlib.gunzip(stm.readAll()), b);
    });

    it("gunzipTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.gunzipTo(zlib.gzip(b), stm);
        stm.rewind();
        assert.deepEqual(stm.readAll(), b);
    });

    it("deflateTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(b);
        stm.rewind();

        var stm1 = new io.MemoryStream();
        zlib.deflateTo(stm, stm1);

        stm1.rewind();
        assert.deepEqual(zlib.inflate(stm1.readAll()), b);
    });

    it("inflateTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(zlib.deflate(b));
        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.inflateTo(stm, stm1);
        stm1.rewind();
        assert.deepEqual(stm1.readAll(), b);
    });

    it("gzipTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(b);

        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.gzipTo(stm, stm1);
        stm1.rewind();
        assert.deepEqual(zlib.gunzip(stm1.readAll()), b);
    });

    it("gunzipTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(zlib.gzip(b));
        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.gunzipTo(stm, stm1);
        stm1.rewind();
        assert.deepEqual(stm1.readAll(), b);
    });

    it("gunzip (from file)", () => {
        var f1 = fs.openFile(path.join(__dirname, 'zlib_files', 'gzip'));
        var f2 = fs.openFile(path.join(__dirname, 'zlib_files', 'original.js'));
        assert.deepEqual(zlib.gunzip(f1.readAll()), f2.readAll());
    });

    it("inflate (from file)", () => {
        var f1 = fs.openFile(path.join(__dirname, 'zlib_files', 'deflate'));
        var f2 = fs.openFile(path.join(__dirname, 'zlib_files', 'original.js'));
        assert.deepEqual(zlib.inflateRaw(f1.readAll()), f2.readAll());
    });
});

require.main === module && test.run(console.DEBUG);