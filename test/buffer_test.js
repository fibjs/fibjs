var test = require("test");
test.setup();

describe('Buffer', function() {
	it('new Buffer(Integr)', function() {
		var buf = new Buffer(100);
		assert.equal(buf.length, 100);
	});

	it('new Buffer(String)', function() {
		var buf = new Buffer("abcd");
		assert.equal(buf.length, 4);
		assert.equal(buf.toString(), "abcd");

		var buf = new Buffer("100");
		assert.equal(buf.length, 3);
		assert.equal(buf.toString(), "100");
	});

	it('new Buffer(Array)', function() {
		var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
		assert.equal(buf.length, 4);
		assert.equal(buf.toString(), "1234");
	});

	it('new Buffer(Buffer)', function() {
		var b = new Buffer("1234");
		var buf = new Buffer(b);
		assert.equal(buf.length, 4);
		assert.equal(buf.toString(), "1234");
	});

	it('toJSON', function() {
		var buf = new Buffer([1, 2, 3, 4]);
		assert.deepEqual(buf.toJSON(), [1, 2, 3, 4]);
	});

	it('toString', function() {
		var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
		assert.equal(buf.toString("utf8"), "1234");
		assert.equal(buf.toString("hex"), "31323334");
		assert.equal(buf.toString("base64"), "MTIzNA==");
	});

	it('write', function() {
		var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
		assert.equal(buf.toString(), "1234");

		buf.write("abcd");
		assert.equal(buf.toString(), "1234abcd");

		buf.write([0x31, 0x32, 0x33, 0x34]);
		assert.equal(buf.toString(), "1234abcd1234");

		buf.write("3132", "hex");
		assert.equal(buf.toString(), "1234abcd123412");

		buf.write("MTIzNA==", "base64");
		assert.equal(buf.toString(), "1234abcd1234121234");
	});

	it('charset', function() {
		assert.equal(new Buffer("哈哈哈").toString(), "哈哈哈");
		assert.deepEqual(new Buffer("哈哈哈哈", "gbk").toJSON(), [
			185, 254, 185, 254, 185, 254, 185, 254
		]);
		assert.equal(new Buffer("哈哈哈", "gbk").toString("gbk"), "哈哈哈");
	});

	it('resize', function() {
		var buf = new Buffer();
		buf.resize(100);
		assert.equal(buf.length, 100);

		assert.doesNotThrow(function() {
			buf.resize("12")
		});
		assert.throws(function() {
			buf.resize("a12")
		});

		assert.throws(function() {
			buf.resize("12a")
		});

		assert.throws(function() {
			buf.resize("12.a")
		});
	});
});

//test.run(console.DEBUG);