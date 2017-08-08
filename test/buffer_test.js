var test = require("test");
test.setup();

describe('Buffer', () => {
    it("buffer module", () => {
        assert.equal(require('buffer'), Buffer);
        assert.equal(require('buffer').Buffer, Buffer);
    });

    it('new Buffer(Integr)', () => {
        var buf = new Buffer(100);
        assert.equal(buf.length, 100);
    });

    it('new Buffer(String)', () => {
        var buf = new Buffer("abcd");
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "abcd");

        var buf = new Buffer("100");
        assert.equal(buf.length, 3);
        assert.equal(buf.toString(), "100");
    });

    it('new Buffer(Array)', () => {
        var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "1234");
    });

    it('new Buffer(Array) with undefined encoding', () => {
        var buf = new Buffer([0x31, 0x32, 0x33, 0x34], undefined);
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "1234");
    });

    it('new Buffer(TypedArray)', () => {
        var arr = new Uint16Array(2);
        arr[0] = 5000;
        arr[1] = 4000;

        var buf = new Buffer(arr);

        assert.equal(buf.length, 2);
        assert.equal(buf.hex(), "88a0");
    });

    it('new Buffer(ArrayBuffer)', () => {
        var arr = new Uint16Array(2);
        arr[0] = 5000;
        arr[1] = 4000;

        var buf = new Buffer(arr.buffer);

        assert.equal(buf.length, 4);
        assert.equal(buf.hex(), "8813a00f");
    });

    it('new Buffer(Buffer)', () => {
        var buf = new Buffer(new Buffer("abcd"));
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "abcd");
        var buf = new Buffer({});
    });

    it('isBuffer', () => {
        var buf = new Buffer("abcd");
        var str = "abcd"
        assert.equal(Buffer.isBuffer(buf), true);
        assert.equal(Buffer.isBuffer(str), false);
    });

    it('concat', () => {
        var buf1 = new Buffer("abcd");
        var buf2 = new Buffer("efg");
        var buf3 = new Buffer();
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
        buf3 = new Buffer([135, 136]);
        bufArray = [buf1, buf2, buf3];
        bufRes = Buffer.concat(bufArray);
        for (var i = 0; i < 8; i++) {
            assert.equal(bufRes[i], i + 1);
        }
        assert.equal(bufRes[8], 135);
        assert.equal(bufRes[9], 136);
        buf1 = new Buffer('');
        bufArray = [buf1];
        assert.doesNotThrow(() => {
            bufRes = Buffer.concat([() => { }, {}, undefined, '']);
        });
    });

    it('Buffer.from(String)', () => {
        var buf = Buffer.from("abcd");
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "abcd");

        var buf = new Buffer("100");
        assert.equal(buf.length, 3);
        assert.equal(buf.toString(), "100");
    });

    it('Buffer.from(Array)', () => {
        var buf = Buffer.from([0x31, 0x32, 0x33, 0x34]);
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "1234");
    });

    it('Buffer.from(TypedArray)', () => {
        var arr = new Uint16Array(2);
        arr[0] = 5000;
        arr[1] = 4000;

        var buf = Buffer.from(arr);

        assert.equal(buf.length, 2);
        assert.equal(buf.hex(), "88a0");
    });

    it('Buffer.from(ArrayBuffer)', () => {
        var arr = new Uint16Array(2);
        arr[0] = 5000;
        arr[1] = 4000;

        var buf = Buffer.from(arr.buffer);

        assert.equal(buf.length, 4);
        assert.equal(buf.hex(), "8813a00f");
    });

    it('Buffer.from(Buffer)', () => {
        var buf = Buffer.from(new Buffer("abcd"));
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "abcd");
    });

    it('Buffer.byteLength(String)', () => {
        var str1 = '\u00bd + \u00bc = \u00be';
        var str2 = '0xffffff';
        var str3 = 'YnVmZmVy';
        assert.equal(Buffer.byteLength(str1), 12);
        assert.equal(Buffer.byteLength(str1, 'utf8'), 12);
        assert.equal(Buffer.byteLength(str2, 'hex'), 4);
        assert.equal(Buffer.byteLength(str3, 'base64'), 6);
    });

    it('Buffer.byteLength(ArrayBuffer)', () => {
        var buf = new ArrayBuffer(8);
        assert.equal(Buffer.byteLength(buf), 8);
    });

    it('Buffer.byteLength(TypedArray)', () => {
        var buf = new ArrayBuffer(8);
        var int32Array = new Int32Array(buf);
        assert.equal(Buffer.byteLength(int32Array), 8);
    });

    it('Buffer.byteLength(Buffer)', () => {
        var buf1 = new Buffer("abcd");
        var buf2 = new Buffer([98, 117, 102, 102, 101, 114]);
        assert.equal(Buffer.byteLength(buf1), 4);
        assert.equal(Buffer.byteLength(buf2), 6);
    });

    it('keys', () => {
        var buf1 = new Buffer('buffer');
        var buf2 = new Buffer([98, 117, 102, 102, 101, 114]);
        var correctResult = [0, 1, 2, 3, 4, 5];
        var keys1 = [];
        var keys2 = [];
        for (let key of buf1.keys()) {
            keys1.push(key);
        }
        assert.deepEqual(keys1, correctResult);

        for (let key of buf2.keys()) {
            keys2.push(key);
        }
        assert.deepEqual(keys2, correctResult);
    });

    it('values', () => {
        var buf1 = new Buffer('buffer');
        var buf2 = new Buffer([98, 117, 102, 102, 101, 114]);
        var correctResult = [98, 117, 102, 102, 101, 114];
        var values1 = [];
        var values2 = [];
        for (let value of buf1.values()) {
            values1.push(value);
        }
        assert.deepEqual(values1, correctResult);

        for (let value of buf2.values()) {
            values2.push(value);
        }
        assert.deepEqual(values2, correctResult);
    });

    it('toArray', () => {
        var buf = new Buffer([1, 2, 3, 4]);
        assert.deepEqual(buf.toArray(), [1, 2, 3, 4]);
    });

    it('toJSON', () => {
        var buf = new Buffer([1, 2, 3, 4]);
        assert.deepEqual(buf.toJSON(), {
            type: 'Buffer',
            data: [1, 2, 3, 4]
        });
    });

    it('toString', () => {
        var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
        assert.equal(buf.toString("utf8"), "1234");
        assert.equal(buf.toString(undefined), "1234");
        assert.equal(buf.toString("hex"), "31323334");
        assert.equal(buf.toString("base64"), "MTIzNA==");
        assert.equal(buf.toString("utf8", 1), "234");
        assert.equal(buf.toString("utf8", 1, 3), "23");
        assert.equal(buf.toString("hex", 2), "323334");
        assert.equal(buf.toString("base64", 2), "IzNA==");

        buf = new Buffer(5)
        buf.append("abcd");
        assert.equal(buf.toString("utf8", 5), "abcd");

        var buf1 = new Buffer('this is a tést');
        assert.equal(buf1.toString(), 'this is a tést');
        assert.equal(buf1.toString('ascii'), 'this is a tC)st');

        assert.equal(buf1.toString('ucs2'), '桴獩椠⁳⁡썴玩');
    });

    it('append', () => {
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

    it('write', () => {
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

        buf = new Buffer(3);
        assert.equal(buf.write("abcd", 0, "utf8"), 3);
        assert.equal(buf.toString('utf8', 0, 3), "abc");

        buf = new Buffer(3);
        assert.equal(buf.write("abcd", "utf8"), 3);
        assert.equal(buf.toString('utf8', 0, 3), "abc");
    });

    it('fill', () => {
        var buf = new Buffer(5);
        buf.fill(10);
        for (var i = 0; i < 5; i++)
            assert.equal(buf[i], 10);

        buf = new Buffer(10);
        buf.fill("abc");
        assert.equal(buf.toString(), "abcabcabca");

        buf.fill("abcabcabcabc");
        assert.equal(buf.toString(), "abcabcabca");

        assert.throws(() => {
            buf.fill("abcabcabcabc", 1, 12);
        })
        assert.throws(() => {
            buf.fill("abcabcabcabc", 6, 5);
        })

        buf = new Buffer(10);
        var buf1 = buf.fill(new Buffer([0, 1, 2]));
        for (var i = 0; i < 3; i++) {
            assert.equal(buf[i], i);
            assert.equal(buf[i + 3], i);
            assert.equal(buf[i + 6], i);
        }
        assert.equal(buf[9], 0);
        assert.throws(() => {
            buf[10];
        })
        assert.equal(buf, buf1);
    });

    it('slice', () => {
        var buf = new Buffer(5);
        buf.fill(10);
        var sli = buf.slice(1, 4);
        for (var i = 0; i < 3; i++)
            assert.equal(sli[i], 10);

        buf = new Buffer(10);
        buf.write("abcdefghih");
        assert.equal(buf.slice(0, 3), "abc");
        assert.equal(buf.slice(6, 5), "");
        assert.equal(buf.slice(0, 11), "abcdefghih");
        assert.equal(buf.slice(8), "ih");

        var buf = new Buffer('buffer'); //TODO slice 反向的支持
        assert.equal(buf.slice(-6, -1), 'buffe');
    });

    it('equals & compare', () => {
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

    it('copy', () => {

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

    it("readNumber", () => {
        var buf = new Buffer([0x23, 0x42]);

        assert.equal(buf.readUInt8(), 35);

        assert.equal(buf.readUInt16BE(), 9026);
        assert.equal(buf.readUInt16LE(), 16931);

        assert.equal(buf.readUInt16BE(1, true), 16896);
        assert.equal(buf.readUInt16LE(1, true), 66);

        assert.throws(() => {
            buf.readUInt16BE(1);
        });

        assert.throws(() => {
            buf.readUInt16LE(1);
        });

        var buf = new Buffer([0xb3, 0x42]);

        assert.equal(buf.readInt16BE(), -19646);
        assert.equal(buf.readInt16LE(), 17075);

        var buf = new Buffer([
            0x12,
            0x34,
            0x56,
            0x78
        ]);

        assert.equal(buf.readInt32BE(), 0x12345678);
        assert.equal(buf.readInt32LE(), 0x78563412);

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

        assert.equal(buf.readIntBE(), 0x123456789abc);
        assert.equal(buf.readIntLE(), -0x3c9a78563412);

        assert.equal(buf.readUIntBE(), 0x123456789abc);
        assert.equal(buf.readUIntLE(), 0xbc9a78563412);

        assert.equal(buf.readInt64BE().toString(16), "0x123456789abcdef0");
        assert.equal(buf.readInt64LE().toString(16), "0xf0debc9a78563412");

        var buf = new Buffer([
            0x00,
            0x00,
            0x80,
            0x3f
        ]);

        assert.equal(buf.readFloatLE(), 1);

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

        assert.equal(buf.readDoubleLE(), 0.3333333333333333);
    });

    it("writeNumber", () => {
        var buf = new Buffer(2);

        buf.writeUInt16BE(9026, 0);
        assert.equal(buf.readUInt16BE(), 9026);

        buf.writeUInt16LE(16931, 0);
        assert.equal(buf.readUInt16BE(), 9026);

        buf[1] = 0x00;
        buf.writeUInt16BE(16896, 1, true);
        assert.equal(buf[1], 0x42);

        buf[1] = 0x00;
        buf.writeUInt16LE(66, 1, true);
        assert.equal(buf[1], 0x42);

        assert.equal(buf.length, 2);

        assert.throws(() => {
            buf.writeUInt16BE(0, 1);
        });

        assert.throws(() => {
            buf.writeUInt16LE(0, 1);
        });

        var buf = new Buffer(4);

        buf.writeInt32BE(0x12345678, 0);
        assert.deepEqual(buf.toArray(), [
            0x12,
            0x34,
            0x56,
            0x78
        ]);

        buf.writeInt32LE(0x12345678, 0);
        assert.deepEqual(buf.toArray(), [
            0x78,
            0x56,
            0x34,
            0x12
        ]);

        var buf = new Buffer(6);

        buf.writeIntBE(0x12345678abcd, 0);
        assert.deepEqual(buf.toArray(), [
            0x12,
            0x34,
            0x56,
            0x78,
            0xab,
            0xcd
        ]);

        buf.writeIntLE(0x12345678abcd, 0);
        assert.deepEqual(buf.toArray(), [
            0xcd,
            0xab,
            0x78,
            0x56,
            0x34,
            0x12
        ]);

        var buf = new Buffer(8);

        buf.writeInt64BE(0x12345678abcd, 0);
        assert.deepEqual(buf.toArray(), [
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
        assert.deepEqual(buf.toArray(), [
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
        assert.deepEqual(buf.toArray(), [
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
        assert.deepEqual(buf.toArray(), [
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

    it('charset', () => {
        assert.equal(new Buffer("哈哈哈").toString(), "哈哈哈");
        assert.deepEqual(new Buffer("哈哈哈哈", "gbk").toArray(), [
            185, 254, 185, 254, 185, 254, 185, 254
        ]);
        assert.equal(new Buffer("哈哈哈", "gbk").toString("gbk"), "哈哈哈");
    });

    it('resize', () => {
        var buf = new Buffer();
        buf.resize(100);
        assert.equal(buf.length, 100);

        assert.doesNotThrow(() => {
            buf.resize("12")
        });
        assert.throws(() => {
            buf.resize("a12")
        });

        assert.throws(() => {
            buf.resize("12a")
        });

        assert.throws(() => {
            buf.resize("12.a")
        });
    });

    it('indexOf', () => {
        var buf = new Buffer([0x31, 0x32, 0x33, 0x34, 0x00]);
        assert.equal(buf.indexOf(0x33), 2);
        assert.equal(buf.indexOf(0x00), 4);

        buf = new Buffer("cacdbfcde");

        assert.equal(buf.indexOf("cd"), 2);
        assert.equal(buf.indexOf(new Buffer("de")), 7);

        assert.throws(() => {
            buf.indexOf("cd", 10);
        });

        assert.throws(() => {
            buf.indexOf(new Buffer("de"), 10);
        });

        buf = new Buffer('123456');
        assert.equal(buf.indexOf(0x33), 2);

        buf = new Buffer([0x31, 0x32, 0x33, 0x34, 0x05, 0x36]);
        assert.equal(buf.indexOf(0x38, 3), -1);

        buf = new Buffer([0x31, 0x32, 0x33, 0x34, 0x05, 0x00, 0x36, 0x37]);
        assert.equal(buf.indexOf(new Buffer([0x00, 0x36])), 5);

    });
});

repl && test.run(console.DEBUG);