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
		var buf = new Buffer(new Buffer("abcd"));
		assert.equal(buf.length, 4);
		assert.equal(buf.toString(), "abcd");
		var buf = new Buffer({});
	});

	it('isBuffer', function() {
		var buf = new Buffer("abcd");
		var str = "abcd"
		assert.equal(Buffer.isBuffer(buf), true);
		assert.equal(Buffer.isBuffer(str), false);
	});

	it('concat', function() {
		var buf1 = new Buffer("abcd");
		var buf2 = new Buffer("efg");
		var bufArray = [buf1];
		var bufRes = Buffer.concat(bufArray);
		assert.equal(bufRes.toString(), "abcd")

		bufArray = [buf1, buf2];
		bufRes = Buffer.concat(bufArray);
		assert.equal(bufRes.toString(), "abcdefg")

		bufRes = Buffer.concat(bufArray, 6);
		assert.equal(bufRes.toString(), "abcdef")

		buf1 = new Buffer([0x31, 0x32, 0x33, 0x34]);
		buf2 = new Buffer([0x35, 0x36, 0x37, 0x38]);
		bufArray = [buf1, buf2];
		bufRes = Buffer.concat(bufArray);
		assert.equal(bufRes.length, 8);
		assert.equal(bufRes.toString(), "12345678");
		bufRes = Buffer.concat(bufArray, 7);
		assert.equal(bufRes.length, 7);
		assert.equal(bufRes.toString(), "1234567");

		buf1 = new Buffer([1, 2, 3, 4]);
		buf2 = new Buffer([5, 6, 7, 8]);
		bufArray = [buf1, buf2];
		bufRes = Buffer.concat(bufArray);
		for (var i = 0; i < 8; i++) {
			assert.equal(bufRes[i], i + 1);
		}
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
		assert.equal(buf.toString("utf8", 1), "234");
		assert.equal(buf.toString("utf8", 1, 3), "23");
		assert.equal(buf.toString("hex", 2), "323334");
		assert.equal(buf.toString("base64", 2), "IzNA==");

		buf = new Buffer(5)
		buf.append("abcd");
		assert.equal(buf.toString("utf8", 5), "abcd");
	});

	it('append', function() {
		var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
		assert.equal(buf.toString(), "1234");

		buf.append("abcd");
		assert.equal(buf.toString(), "1234abcd");

		buf.append([0x31, 0x32, 0x33, 0x34]);
		assert.equal(buf.toString(), "1234abcd1234");

		buf.append("3132", "hex");
		assert.equal(buf.toString(), "1234abcd123412");

		buf.append("MTIzNA==", "base64");
		assert.equal(buf.toString(), "1234abcd1234121234");
	});

	it('write', function() {
		var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
		assert.equal(buf.toString(), "1234");

		buf = new Buffer(10);
		assert.equal(buf.write("abcd", 0, 4), 4);
		assert.equal(buf.toString('utf8', 0, 4), "abcd");
		assert.equal(buf.toString('utf8', 0, 3), "abc");

		buf = new Buffer(10);
		assert.equal(buf.write("MTIzNA==", 0, 4, "base64"), 4);
		assert.equal(buf.toString("utf8", 0, 4), "1234");

		assert.equal(buf.write("31323334", 0, 20, "hex"), 8);
		assert.equal(buf.toString("utf8", 0, 4), "1234");

		assert.equal(buf.write("abcde", 1, 4), 4);
		assert.equal(buf.toString('utf8', 1, 4), "abc");

		buf = new Buffer(3);
		assert.equal(buf.write("abcd", 0, 4), 3);
		assert.equal(buf.toString('utf8', 0, 3), "abc");
	});

	it('fill', function() {
		var buf = new Buffer(5);
		buf.fill(10);
		for (var i = 0; i < 5; i++)
			assert.equal(buf[i], 10);

		buf = new Buffer(10);
		buf.fill("abc");
		assert.equal(buf.toString(), "abcabcabca");

		buf.fill("abcabcabcabc");
		assert.equal(buf.toString(), "abcabcabca");
		assert.throws(function() {
			buf.fill("abcabcabcabc", 1, 11);
		})
		assert.throws(function() {
			buf.fill("abcabcabcabc", 6, 5);
		})
	});

	it('equals & compare', function() {
		var buf = new Buffer("abcd");
		assert.equal(buf.equals(new Buffer("abcd")), true);
		assert.equal(buf.equals(new Buffer("abc")), false);
		assert.equal(buf.compare(new Buffer("abcd")), 0);
		assert.greaterThan(buf.compare(new Buffer("abc")), 0);
		assert.lessThan(buf.compare(new Buffer("abcde")), 0);

		buf = new Buffer([1, 0, 1]);
		assert.equal(buf.equals(new Buffer([1, 0, 1])), true);
		assert.equal(buf.equals(new Buffer([1, 0, 2])), false);
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

//test.run(console.DEBUG);