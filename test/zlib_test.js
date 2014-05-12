var test = require("test");
test.setup();

var zlib = require('zlib');
var io = require('io');

var M = 102400;
var b = new Buffer();
b.resize(M);

for (i = 0; i < M; i++)
	b[i] = Math.random() * 70 + 33;

describe("zlib", function() {
	it("inflate", function() {
		assert.equal(zlib.inflate(zlib.deflate(b)).toString(), b.toString());
	});

	it("gzip", function() {
		assert.equal(zlib.gunzip(zlib.gzip(b)).toString(), b.toString());
	});

	it("deflateTo (from Buffer)", function() {
		var stm = new io.MemoryStream();
		zlib.deflateTo(b, stm);
		stm.rewind();
		assert.equal(zlib.inflate(stm.read()).toString(), b.toString());
	});

	it("inflateTo (from Buffer)", function() {
		var stm = new io.MemoryStream();
		zlib.inflateTo(zlib.deflate(b), stm);
		stm.rewind();
		assert.equal(stm.read().toString(), b.toString());
	});

	it("gzipTo (from Buffer)", function() {
		var stm = new io.MemoryStream();
		zlib.gzipTo(b, stm);
		stm.rewind();
		assert.equal(zlib.gunzip(stm.read()).toString(), b.toString());
	});

	it("gunzipTo (from Buffer)", function() {
		var stm = new io.MemoryStream();
		zlib.gunzipTo(zlib.gzip(b), stm);
		stm.rewind();
		assert.equal(stm.read().toString(), b.toString());
	});

	it("deflateTo (from Stream)", function() {
		var stm = new io.MemoryStream();
		stm.write(b);

		stm.rewind()
		var stm1 = new io.MemoryStream();
		zlib.deflateTo(stm, stm1);
		stm1.rewind();
		assert.equal(zlib.inflate(stm1.read()).toString(), b.toString());
	});

	it("inflateTo (from Stream)", function() {
		var stm = new io.MemoryStream();
		stm.write(zlib.deflate(b));
		stm.rewind()
		var stm1 = new io.MemoryStream();
		zlib.inflateTo(stm, stm1);
		stm1.rewind();
		assert.equal(stm1.read().toString(), b.toString());
	});

	it("gzipTo (from Stream)", function() {
		var stm = new io.MemoryStream();
		stm.write(b);

		stm.rewind()
		var stm1 = new io.MemoryStream();
		zlib.gzipTo(stm, stm1);
		stm1.rewind();
		assert.equal(zlib.gunzip(stm1.read()).toString(), b.toString());
	});

	it("gunzipTo (from Stream)", function() {
		var stm = new io.MemoryStream();
		stm.write(zlib.gzip(b));
		stm.rewind()
		var stm1 = new io.MemoryStream();
		zlib.gunzipTo(stm, stm1);
		stm1.rewind();
		assert.equal(stm1.read().toString(), b.toString());
	});
});

//test.run();