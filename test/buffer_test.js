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

	it("readNumber", function() {
		var buf = new Buffer([0x23, 0x42]);

		assert.equal(buf.readUInt8(0), 35);

		assert.equal(buf.readUInt16BE(0), 9026);
		assert.equal(buf.readUInt16LE(0), 16931);

		assert.equal(buf.readUInt16BE(1, true), 16896);
		assert.equal(buf.readUInt16LE(1, true), 66);

		assert.throws(function() {
			buf.readUInt16BE(1);
		});

		assert.throws(function() {
			buf.readUInt16LE(1);
		});

		var buf = new Buffer([0xb3, 0x42]);

		assert.equal(buf.readInt16BE(0), -19646);
		assert.equal(buf.readInt16LE(0), 17075);

		var buf = new Buffer(4);

		buf[0] = 0x00;
		buf[1] = 0x00;
		buf[2] = 0x80;
		buf[3] = 0x3f;

		assert.equal(buf.readFloatLE(0), 1);

		var buf = new Buffer(8);

		buf[0] = 0x55;
		buf[1] = 0x55;
		buf[2] = 0x55;
		buf[3] = 0x55;
		buf[4] = 0x55;
		buf[5] = 0x55;
		buf[6] = 0xd5;
		buf[7] = 0x3f;

		assert.equal(buf.readDoubleLE(0), 0.3333333333333333);
	});

	it("writeNumber", function() {
		var buf = new Buffer(2);

		buf.writeUInt16BE(9026, 0);
		assert.equal(buf.readUInt16BE(0), 9026);

		buf.writeUInt16LE(16931, 0);
		assert.equal(buf.readUInt16BE(0), 9026);

		buf[1] = 0x00;
		buf.writeUInt16BE(16896, 1, true);
		assert.equal(buf[1], 0x42);

		buf[1] = 0x00;
		buf.writeUInt16LE(66, 1, true);
		assert.equal(buf[1], 0x42);

		assert.equal(buf.length, 2);

		assert.throws(function() {
			buf.writeUInt16BE(0, 1);
		});

		assert.throws(function() {
			buf.writeUInt16LE(0, 1);
		});

		var buf = new Buffer(4);
		buf.writeFloatLE(1, 0);
		assert.equal(buf.hex(), "0000803f");

		var buf = new Buffer(8);
		buf.writeDoubleLE(0.3333333333333333, 0);
		assert.equal(buf.hex(), "555555555555d53f");
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