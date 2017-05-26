var test = require("test");
test.setup();

var zlib = require('zlib');
var io = require('io');
var fs = require('fs');

var M = 102400;
var b = new Buffer();
b.resize(M);

var i;

for (i = 0; i < M; i++)
    b[i] = Math.random() * 70 + 33;

describe("zlib", () => {
    it("inflate", () => {
        assert.equal(zlib.inflate(zlib.deflate(b)).toString(), b.toString());
    });

    it("gzip", () => {
        assert.equal(zlib.gunzip(zlib.gzip(b)).toString(), b.toString());
    });

    it("deflateTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.deflateTo(b, stm);
        stm.rewind();
        assert.equal(zlib.inflate(stm.read()).toString(), b.toString());
    });

    it("inflateTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.inflateTo(zlib.deflate(b), stm);
        stm.rewind();
        assert.equal(stm.read().toString(), b.toString());
    });

    it("gzipTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.gzipTo(b, stm);
        stm.rewind();
        assert.equal(zlib.gunzip(stm.read()).toString(), b.toString());
    });

    it("gunzipTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.gunzipTo(zlib.gzip(b), stm);
        stm.rewind();
        assert.equal(stm.read().toString(), b.toString());
    });

    it("deflateTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(b);

        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.deflateTo(stm, stm1);
        stm1.rewind();
        assert.equal(zlib.inflate(stm1.read()).toString(), b.toString());
    });

    it("inflateTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(zlib.deflate(b));
        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.inflateTo(stm, stm1);
        stm1.rewind();
        assert.equal(stm1.read().toString(), b.toString());
    });

    it("gzipTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(b);

        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.gzipTo(stm, stm1);
        stm1.rewind();
        assert.equal(zlib.gunzip(stm1.read()).toString(), b.toString());
    });

    it("gunzipTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(zlib.gzip(b));
        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.gunzipTo(stm, stm1);
        stm1.rewind();
        assert.equal(stm1.read().toString(), b.toString());
    });

    it("gunzip (from file)", () => {
        var f1 = fs.openTextStream(__dirname + '/zlib_files/gzip');
        var f2 = fs.openTextStream(__dirname + '/zlib_files/original.js');
        assert.equal(zlib.gunzip(f1.read()).toString(), f2.read().toString());
    });

    it("inflate (from file)", () => {
        var f1 = fs.openTextStream(__dirname + '/zlib_files/deflate');
        var f2 = fs.openTextStream(__dirname + '/zlib_files/original.js');
        assert.equal(zlib.inflateRaw(f1.read()).toString(), f2.read().toString());
    });
});

//test.run();