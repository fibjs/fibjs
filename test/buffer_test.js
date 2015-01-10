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

	it("concat", function(){
		assert.equal(Buffer.concat([]).length, 0);

		var a = [new Buffer('abcd')];
		var b = Buffer.concat(a);
		assert.equal(b.toString(), "abcd");
		assert.equal(a[0], b);
		assert.equal(b.length, 4);
		assert.throws(function() {
			Buffer.concat("a");
		});

		var c = [new Buffer('abc'), new Buffer('def')];
		c = Buffer.concat(c);
		assert.equal(c.toString(), "abcdef");
		assert.equal(c.length, 6);

		var d = [new Buffer('abc'), new Buffer('def')];
		d = Buffer.concat(d, 5);
		assert.equal(d.toString(), "abcde");
		assert.equal(d.length, 5);

		var e = [new Buffer('a'), new Buffer('b'), new Buffer('c'), new Buffer('d')];
		e = Buffer.concat(e, 3);
		assert.equal(e.toString(), "abc");
		assert.equal(e.length, 3);

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

	it('copy', function() {

		var buf1 = new Buffer([0x31, 0x32, 0x33]);
		var arr = [0x34, 0x35, 0x36];

		var buf2 = new Buffer(arr);
		var sz = buf1.copy(buf2);
		assert.equal(sz, 3);
		assert.equal(buf2.toString(), '123');

		buf2 = new Buffer(arr);
		sz = buf1.copy(buf2, 1);
		assert.equal(sz, 2);
		assert.equal(buf2.toString(), '412');

		buf2 = new Buffer(arr);
		sz = buf1.copy(buf2, 1, 1);
		assert.equal(sz, 2);
		assert.equal(buf2.toString(), '423');

		buf2 = new Buffer(arr);
		sz = buf1.copy(buf2, 1, 1, 2);
		assert.equal(sz, 1);
		assert.equal(buf2.toString(), '426');

		buf2 = new Buffer(arr);
		sz = buf1.copy(buf2, 1, 1, 1);
		assert.equal(sz, 0);
		assert.equal(buf2.toString(), '456');

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

		var buf = new Buffer([
			0x12,
			0x34,
			0x56,
			0x78
		]);

		assert.equal(buf.readInt32BE(0), 0x12345678);
		assert.equal(buf.readInt32LE(0), 0x78563412);

		var buf = new Buffer([
			0x12,
			0x34,
			0x56,
			0x78,
			0x9a,
			0xbc,
			0xde,
			0xf0
		]);

		assert.equal(buf.readInt64BE(0).toString(16), "0x123456789abcdef0");
		assert.equal(buf.readInt64LE(0).toString(16), "0xf0debc9a78563412");

		var buf = new Buffer([
			0x00,
			0x00,
			0x80,
			0x3f
		]);

		assert.equal(buf.readFloatLE(0), 1);

		var buf = new Buffer([
			0x55,
			0x55,
			0x55,
			0x55,
			0x55,
			0x55,
			0xd5,
			0x3f
		]);

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

		buf.writeInt32BE(0x12345678, 0);
		assert.deepEqual(buf.toJSON(), [
			0x12,
			0x34,
			0x56,
			0x78
		]);

		buf.writeInt32LE(0x12345678, 0);
		assert.deepEqual(buf.toJSON(), [
			0x78,
			0x56,
			0x34,
			0x12
		]);

		var buf = new Buffer(8);

		buf.writeInt64BE(0x12345678abcd, 0);
		assert.deepEqual(buf.toJSON(), [
			0x0,
			0x0,
			0x12,
			0x34,
			0x56,
			0x78,
			0xab,
			0xcd
		]);

		buf.writeInt64LE(0x12345678abcd, 0);
		assert.deepEqual(buf.toJSON(), [
			0xcd,
			0xab,
			0x78,
			0x56,
			0x34,
			0x12,
			0x0,
			0x0
		]);

		buf.writeInt64BE(new Int64(0x12345678abcd), 0);
		assert.deepEqual(buf.toJSON(), [
			0x0,
			0x0,
			0x12,
			0x34,
			0x56,
			0x78,
			0xab,
			0xcd
		]);

		buf.writeInt64LE(new Int64(0x12345678abcd), 0);
		assert.deepEqual(buf.toJSON(), [
			0xcd,
			0xab,
			0x78,
			0x56,
			0x34,
			0x12,
			0x0,
			0x0
		]);

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

test.run(console.DEBUG);