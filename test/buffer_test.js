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
        var arr = new Uint8Array(2);
        arr[0] = 50;
        arr[1] = 40;

        var buf = new Buffer(arr);

        assert.equal(buf.length, 2);
        assert.equal(buf.hex(), "3228");

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

    it('new Buffer(ArrayBufferView)', () => {
        var arr = new DataView(new ArrayBuffer(2));
        arr.setInt8(0, 0x10);
        arr.setInt8(1, 0x20);

        var buf = new Buffer(arr);

        assert.equal(buf.length, 2);
        assert.equal(buf.hex(), "1020");

        var arr = new DataView(new ArrayBuffer(4), 2);
        arr.setInt8(0, 0x10);
        arr.setInt8(1, 0x20);

        var buf = new Buffer(arr);

        assert.equal(buf.length, 2);
        assert.equal(buf.hex(), "1020");
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

        const expected = [0xff, 0xff, 0xbe, 0xff, 0xef, 0xbf, 0xfb, 0xef, 0xff];
        assert.deepEqual(Buffer.from('//++/++/++//', 'base64'),
            Buffer.from(expected));
        assert.deepEqual(Buffer.from('__--_--_--__', 'base64'),
            Buffer.from(expected));

        ['ucs2', 'ucs-2', 'utf16le', 'utf-16le'].forEach((encoding) => {
            {
                // Test for proper UTF16LE encoding, length should be 8
                const f = Buffer.from('über', encoding);
                assert.deepEqual(f, Buffer.from([252, 0, 98, 0, 101, 0, 114, 0]));
            }

            {
                // Length should be 12
                const f = Buffer.from('привет', encoding);
                assert.deepEqual(
                    f, Buffer.from([63, 4, 64, 4, 56, 4, 50, 4, 53, 4, 66, 4])
                );
                assert.strictEqual(f.toString(encoding), 'привет');
            }
            // todo fix the half char write case
            //             {
            //                 const f = Buffer.from([0, 0, 0, 0, 0]);
            //                 assert.strictEqual(f.length, 5);
            //                 const size = f.write('あいうえお', encoding);
            //                 assert.strictEqual(size, 4);
            //                 assert.deepEqual(f, Buffer.from([0x42, 0x30, 0x44, 0x30, 0x00]));
            //             }
        });
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

    it('Buffer.from(Buffer, offset, len)', () => {
        var buf = Buffer.from(new Buffer("abcd"), 1, 2);
        assert.equal(buf.length, 2);
        assert.equal(buf.toString(), "bc");

        var buf = Buffer.from(new Buffer("abcd"), 1);
        assert.equal(buf.length, 3);
        assert.equal(buf.toString(), "bcd");

        var buf = Buffer.from(new Buffer("abcd"), 10);
        assert.equal(buf.length, 0);
    });

    it('Buffer.byteLength(String)', () => {
        var str1 = "\u00bd + \u00bc = \u00be";
        var str2 = "0xffffff";
        var str3 = "YnVmZmVy";
        assert.equal(Buffer.byteLength(str1), 12);
        assert.equal(Buffer.byteLength(str1, "utf8"), 12);
        assert.equal(Buffer.byteLength(str2, "hex"), 4);
        assert.equal(Buffer.byteLength(str3, "base64"), 6);
    });

    it('Buffer.byteLength(ArrayBuffer)', () => {
        var buf = new ArrayBuffer(8);
        assert.equal(Buffer.byteLength(buf), 8);
        assert.equal(Buffer.byteLength(buf, ""), 8);
        assert.equal(Buffer.byteLength(buf, "utf8"), 8);
    });

    it('Buffer.byteLength(TypedArray)', () => {
        var buf = new ArrayBuffer(8);
        var int32Array = new Int32Array(buf);
        assert.equal(Buffer.byteLength(int32Array), 8);
        assert.equal(Buffer.byteLength(int32Array, ""), 8);
        assert.equal(Buffer.byteLength(int32Array, "utf8"), 8);
    });

    it('Buffer.byteLength(DateView)', () => {
        var buf = new ArrayBuffer(8);
        var dataView = new DataView(buf);
        assert.equal(Buffer.byteLength(dataView), 8);
        assert.equal(Buffer.byteLength(dataView, ""), 8);
        assert.equal(Buffer.byteLength(dataView, "utf8"), 8);
    });

    it('Buffer.byteLength(Buffer)', () => {
        var buf1 = new Buffer("abcd");
        var buf2 = new Buffer([98, 117, 102, 102, 101, 114]);
        assert.equal(Buffer.byteLength(buf1), 4);
        assert.equal(Buffer.byteLength(buf2), 6);
        assert.equal(Buffer.byteLength(buf1, ""), 4);
        assert.equal(Buffer.byteLength(buf1, "utf-8"), 4);
        assert.equal(Buffer.byteLength(buf2, ""), 6);
        assert.equal(Buffer.byteLength(buf2, "utf-8"), 6);
    });

    it('Buffer.byteLength(other)', () => {
        assert.equal(Buffer.byteLength({}), 15);
        assert.equal(Buffer.byteLength(function() { }), 15);
        assert.equal(Buffer.byteLength(() => { }), 9);
        assert.equal(Buffer.byteLength([]), 0);
    });

    it('Buffer.alloc(Integer)', () => {
        var buf1 = Buffer.alloc(10, 2);
        var buf2 = Buffer.alloc(2, 0xf);
        assert.equal(buf1.toString(), new Array(11).join("\u0002"));
        assert.equal(buf2.toString(), "\u000f\u000f");
    });

    it('Buffer.alloc(String)', () => {
        var buf1 = Buffer.alloc(10, "h");
        var buf2 = Buffer.alloc(11, 'aGVsbG8gd29ybGQ=', 'base64');
        var buf3 = Buffer.alloc(16, 'aGVsbG8gd29ybGQ=', 'base64');
        assert.equal(buf1.toString(), new Array(11).join("h"));
        assert.equal(buf2.toString(), "hello world");
        assert.equal(buf3.toString(), "hello worldhello");
    });

    it('Buffer.alloc(Buffer)', () => {
        var buf1 = Buffer.alloc(10, new Buffer("h"));
        var buf2 = Buffer.alloc(3, new Buffer("hello"));
        var buf3 = Buffer.alloc(6, new Buffer([0x31, 0x32, 0x33, 0x34]));
        var buf4 = Buffer.alloc(22, new Buffer('aGVsbG8gd29ybGQ=', 'base64'));
        assert.equal(buf1.toString(), new Array(11).join("h"));
        assert.equal(buf2.toString(), "hel");
        assert.equal(buf3.toString(), "123412");
        assert.equal(buf4.toString(), "hello worldhello world");
    });

    it('Buffer.alloc(other)', () => {
        var buf = Buffer.alloc(10);
        assert.equal(buf.toString(), new Array(11).join("\u0000"));
    });

    it('Buffer.allocUnsafe(Integer)', () => {
        var buf1 = Buffer.allocUnsafe(10);
        assert.equal(buf1.length, 10);
    });

    it('Buffer.allocUnsafeSlow(Integer)', () => {
        var buf1 = Buffer.allocUnsafeSlow(10);
        assert.equal(buf1.length, 10);
    });

    it('Buffer.isEncoding', () => {
        assert.equal(Buffer.isEncoding('utf8'), true);
        assert.equal(Buffer.isEncoding('utf-8'), true);
        assert.equal(Buffer.isEncoding('gbk'), true);
        assert.equal(Buffer.isEncoding('gb2312'), true);
        assert.equal(Buffer.isEncoding('hex'), true);
        assert.equal(Buffer.isEncoding('base64'), true);
        assert.equal(Buffer.isEncoding('jis'), false);
        assert.equal(Buffer.isEncoding('aaabbbccc'), false);
        assert.equal(Buffer.isEncoding('binary'), true);
        assert.equal(Buffer.isEncoding('latin1'), true);
        assert.equal(Buffer.isEncoding('big5'), true);
    });

    it('keys', () => {
        var buf1 = new Buffer("buffer");
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
        var buf1 = new Buffer("buffer");
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

    it('entries', () => {
        var buf1 = new Buffer("buffer");
        var buf2 = new Buffer([98, 117, 102, 102, 101, 114]);
        var correctResult = [
            [0, 98],
            [1, 117],
            [2, 102],
            [3, 102],
            [4, 101],
            [5, 114]
        ];
        var entries1 = [];
        var entries2 = [];
        for (let value of buf1.entries()) {
            entries1.push(value);
        }
        assert.deepEqual(entries1, correctResult);

        for (let value of buf2.entries()) {
            entries2.push(value);
        }
        assert.deepEqual(entries2, correctResult);
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
        assert.equal(buf.toString("hex", 2), "3334");
        assert.equal(buf.toString("base64", 2), "MzQ=");

        buf = new Buffer(5)
        buf.append("abcd");
        assert.equal(buf.toString("utf8", 5), "abcd");

        var buf1 = new Buffer('this is a tést');
        assert.equal(buf1.toString(), 'this is a tést');
        assert.equal(buf1.toString('ascii'), 'this is a tC)st');

        assert.equal(buf1.toString('ucs2'), '桴獩椠⁳⁡썴玩');

        assert.strictEqual(Buffer.from([0x41]).toString('utf8', -1), 'A');
        assert.strictEqual(Buffer.from([0x41]).toString('utf8', 1), '');
        assert.strictEqual(Buffer.from([0x41]).toString('utf8', 2), '');

        const b = Buffer.allocUnsafe(1024);
        const utf8String = '¡hέlló wôrld!';
        const offset = 100;

        b.write(utf8String, 0, Buffer.byteLength(utf8String), 'utf8');
        let utf8Slice = b.toString('utf8', 0, Buffer.byteLength(utf8String));
        assert.strictEqual(utf8String, utf8Slice);

        assert.strictEqual(Buffer.byteLength(utf8String),
            b.write(utf8String, offset, 'utf8'));
        utf8Slice = b.toString('utf8', offset,
            offset + Buffer.byteLength(utf8String));
        assert.strictEqual(utf8String, utf8Slice);
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

        assert.equal(buf.write("31323334", 0, 20, "hex"), 4);
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

        const utf8String = '¡hέlló wôrld!';
        const offset = 100;
        const b = Buffer.allocUnsafe(1024);
        const sliceA = b.slice(offset, offset + Buffer.byteLength(utf8String));
        const sliceB = b.slice(offset, offset + Buffer.byteLength(utf8String));
        for (let i = 0; i < Buffer.byteLength(utf8String); i++) {
            assert.strictEqual(sliceA[i], sliceB[i]);
        }
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

        assert.equal(buf.writeUInt16BE(9026, 0), 2);
        assert.equal(buf.readUInt16BE(), 9026);

        assert.equal(buf.writeUInt16LE(16931, 0), 2);
        assert.equal(buf.readUInt16BE(), 9026);

        buf[1] = 0x00;
        assert.equal(buf.writeUInt16BE(16896, 1, true), 2);
        assert.equal(buf[1], 0x42);

        buf[1] = 0x00;
        assert.equal(buf.writeUInt16LE(66, 1, true), 2);
        assert.equal(buf[1], 0x42);

        assert.equal(buf.length, 2);

        assert.throws(() => {
            buf.writeUInt16BE(0, 1);
        });

        assert.throws(() => {
            buf.writeUInt16LE(0, 1);
        });

        var buf = new Buffer(4);

        assert.equal(buf.writeInt32BE(0x12345678, 0), 4);
        assert.deepEqual(buf.toArray(), [
            0x12,
            0x34,
            0x56,
            0x78
        ]);

        assert.equal(buf.writeInt32LE(0x12345678, 0), 4);
        assert.deepEqual(buf.toArray(), [
            0x78,
            0x56,
            0x34,
            0x12
        ]);

        var buf = new Buffer(6);

        assert.equal(buf.writeIntBE(0x12345678abcd, 0), 6);
        assert.deepEqual(buf.toArray(), [
            0x12,
            0x34,
            0x56,
            0x78,
            0xab,
            0xcd
        ]);

        assert.equal(buf.writeIntLE(0x12345678abcd, 0), 6);
        assert.deepEqual(buf.toArray(), [
            0xcd,
            0xab,
            0x78,
            0x56,
            0x34,
            0x12
        ]);

        var buf = new Buffer(8);

        assert.equal(buf.writeInt64BE(0x12345678abcd, 0), 8);
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

        assert.equal(buf.writeInt64LE(0x12345678abcd, 0), 8);
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

        assert.equal(buf.writeInt64BE(new Int64(0x12345678abcd), 0), 8);
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

        assert.equal(buf.writeInt64LE(new Int64(0x12345678abcd), 0), 8);
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
        assert.equal(buf.writeFloatLE(1, 0), 4);
        assert.equal(buf.hex(), "0000803f");

        var buf = new Buffer(8);
        assert.equal(buf.writeDoubleLE(0.3333333333333333, 0), 8);
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
        assert.doesNotThrow(() => {
            buf.resize("a12")
        });

        assert.doesNotThrow(() => {
            buf.resize("12a")
        });

        assert.doesNotThrow(() => {
            buf.resize("12.a")
        });
    });

    describe('indexOf', () => {
        const b = Buffer.from('abcdef');
        const buf_a = Buffer.from('a');
        const buf_bc = Buffer.from('bc');
        const buf_f = Buffer.from('f');
        const buf_z = Buffer.from('z');
        const buf_empty = Buffer.from('');
        const s = 'abcdef';
        // Search in string containing many non-ASCII chars.
        const allCodePoints = [];
        /**
         * 65536 todo , 
         * 62542 x64 limit
         * 60394 darwin i386
         */
        for (let i = 0; i < 60394; i++) allCodePoints[i] = i;
        const allCharsString = String.fromCharCode.apply(String, allCodePoints);

        it('indexOf', () => {
            var buf = new Buffer([0x31, 0x32, 0x33, 0x34, 0x00]);
            assert.equal(buf.indexOf(0x33), 2);
            assert.equal(buf.indexOf(0x00), 4);

            buf = new Buffer("cacdbfcde");

            assert.equal(buf.indexOf("cd"), 2);
            assert.equal(buf.indexOf(new Buffer("de")), 7);

            buf = new Buffer('123456');
            assert.equal(buf.indexOf(0x33), 2);

            buf = new Buffer([0x31, 0x32, 0x33, 0x34, 0x05, 0x36]);
            assert.equal(buf.indexOf(0x38, 3), -1);

            buf = new Buffer([0x31, 0x32, 0x33, 0x34, 0x05, 0x00, 0x36, 0x37]);
            assert.equal(buf.indexOf(new Buffer([0x00, 0x36])), 5);

        });

        it('string', () => {
            assert.strictEqual(b.indexOf('a'), 0);
            assert.strictEqual(b.indexOf('a', 1), -1);
            assert.strictEqual(b.indexOf('a', -1), -1);
            assert.strictEqual(b.indexOf('a', -4), -1);
            assert.strictEqual(b.indexOf('a', -b.length), 0);
            assert.strictEqual(b.indexOf('a', NaN), 0);
            assert.strictEqual(b.indexOf('a', -Infinity), 0);
            assert.strictEqual(b.indexOf('a', Infinity), -1);
            assert.strictEqual(b.indexOf('bc'), 1);
            assert.strictEqual(b.indexOf('bc', 2), -1);
            assert.strictEqual(b.indexOf('bc', -1), -1);
            assert.strictEqual(b.indexOf('bc', -3), -1);
            assert.strictEqual(b.indexOf('bc', -5), 1);
            assert.strictEqual(b.indexOf('bc', NaN), 1);
            assert.strictEqual(b.indexOf('bc', -Infinity), 1);
            assert.strictEqual(b.indexOf('bc', Infinity), -1);
            assert.strictEqual(b.indexOf('f'), b.length - 1);
            assert.strictEqual(b.indexOf('z'), -1);
            assert.strictEqual(b.indexOf(''), 0);
            assert.strictEqual(b.indexOf('', 1), 1);
            assert.strictEqual(b.indexOf('', b.length + 1), b.length);
            assert.strictEqual(b.indexOf('', Infinity), b.length);
        });

        it('Buffer', () => {
            assert.strictEqual(b.indexOf(buf_a), 0);
            assert.strictEqual(b.indexOf(buf_a, 1), -1);
            assert.strictEqual(b.indexOf(buf_a, -1), -1);
            assert.strictEqual(b.indexOf(buf_a, -4), -1);
            assert.strictEqual(b.indexOf(buf_a, -b.length), 0);
            assert.strictEqual(b.indexOf(buf_a, NaN), 0);
            assert.strictEqual(b.indexOf(buf_a, -Infinity), 0);
            assert.strictEqual(b.indexOf(buf_a, Infinity), -1);
            assert.strictEqual(b.indexOf(buf_bc), 1);
            assert.strictEqual(b.indexOf(buf_bc, 2), -1);
            assert.strictEqual(b.indexOf(buf_bc, -1), -1);
            assert.strictEqual(b.indexOf(buf_bc, -3), -1);
            assert.strictEqual(b.indexOf(buf_bc, -5), 1);
            assert.strictEqual(b.indexOf(buf_bc, NaN), 1);
            assert.strictEqual(b.indexOf(buf_bc, -Infinity), 1);
            assert.strictEqual(b.indexOf(buf_bc, Infinity), -1);
            assert.strictEqual(b.indexOf(buf_f), b.length - 1);
            assert.strictEqual(b.indexOf(buf_z), -1);
            assert.strictEqual(b.indexOf(buf_empty), 0);
            assert.strictEqual(b.indexOf(buf_empty, 1), 1);
            assert.strictEqual(b.indexOf(buf_empty, b.length + 1), b.length);
            assert.strictEqual(b.indexOf(buf_empty, Infinity), b.length);
        });

        it('Number', () => {
            assert.strictEqual(b.indexOf(0x61), 0);
            assert.strictEqual(b.indexOf(0x61, 1), -1);
            assert.strictEqual(b.indexOf(0x61, -1), -1);
            assert.strictEqual(b.indexOf(0x61, -4), -1);
            assert.strictEqual(b.indexOf(0x61, -b.length), 0);
            assert.strictEqual(b.indexOf(0x61, NaN), 0);
            assert.strictEqual(b.indexOf(0x61, -Infinity), 0);
            assert.strictEqual(b.indexOf(0x61, Infinity), -1);
            assert.strictEqual(b.indexOf(0x0), -1);
        });

        it('Offset', () => {
            assert.strictEqual(b.indexOf('d', 2), 3);
            assert.strictEqual(b.indexOf('f', 5), 5);
            assert.strictEqual(b.indexOf('f', -1), 5);
            assert.strictEqual(b.indexOf('f', 6), -1);

            assert.strictEqual(b.indexOf(Buffer.from('d'), 2), 3);
            assert.strictEqual(b.indexOf(Buffer.from('f'), 5), 5);
            assert.strictEqual(b.indexOf(Buffer.from('f'), -1), 5);
            assert.strictEqual(b.indexOf(Buffer.from('f'), 6), -1);
        });

        it('ucs2', () => {
            assert.strictEqual(Buffer.from('ff').indexOf(Buffer.from('f'), 1, 'ucs2'), -1);

            {
                // test usc2 encoding
                const twoByteString = Buffer.from('\u039a\u0391\u03a3\u03a3\u0395', 'ucs2');

                assert.strictEqual(8, twoByteString.indexOf('\u0395', 4, 'ucs2'));
                assert.strictEqual(6, twoByteString.indexOf('\u03a3', -4, 'ucs2'));
                assert.strictEqual(4, twoByteString.indexOf('\u03a3', -6, 'ucs2'));
                assert.strictEqual(4, twoByteString.indexOf(
                    Buffer.from('\u03a3', 'ucs2'), -6, 'ucs2'));
                assert.strictEqual(-1, twoByteString.indexOf('\u03a3', -2, 'ucs2'));
            }

            const mixedByteStringUcs2 =
                Buffer.from('\u039a\u0391abc\u03a3\u03a3\u0395', 'ucs2');
            assert.strictEqual(6, mixedByteStringUcs2.indexOf('bc', 0, 'ucs2'));
            assert.strictEqual(10, mixedByteStringUcs2.indexOf('\u03a3', 0, 'ucs2'));
            assert.strictEqual(-1, mixedByteStringUcs2.indexOf('\u0396', 0, 'ucs2'));

            assert.strictEqual(
                6, mixedByteStringUcs2.indexOf(Buffer.from('bc', 'ucs2'), 0, 'ucs2'));
            assert.strictEqual(
                10, mixedByteStringUcs2.indexOf(Buffer.from('\u03a3', 'ucs2'), 0, 'ucs2'));
            assert.strictEqual(
                -1, mixedByteStringUcs2.indexOf(Buffer.from('\u0396', 'ucs2'), 0, 'ucs2'));

            {
                const twoByteString = Buffer.from('\u039a\u0391\u03a3\u03a3\u0395', 'ucs2');

                // Test single char pattern
                assert.strictEqual(0, twoByteString.indexOf('\u039a', 0, 'ucs2'));
                assert.strictEqual(2, twoByteString.indexOf('\u0391', 0, 'ucs2'),
                    'Alpha');
                assert.strictEqual(4, twoByteString.indexOf('\u03a3', 0, 'ucs2'),
                    'First Sigma');
                assert.strictEqual(6, twoByteString.indexOf('\u03a3', 6, 'ucs2'),
                    'Second Sigma');
                assert.strictEqual(8, twoByteString.indexOf('\u0395', 0, 'ucs2'),
                    'Epsilon');
                assert.strictEqual(-1, twoByteString.indexOf('\u0392', 0, 'ucs2'),
                    'Not beta');

                // Test multi-char pattern
                assert.strictEqual(
                    0, twoByteString.indexOf('\u039a\u0391', 0, 'ucs2'), 'Lambda Alpha');
                assert.strictEqual(
                    2, twoByteString.indexOf('\u0391\u03a3', 0, 'ucs2'), 'Alpha Sigma');
                assert.strictEqual(
                    4, twoByteString.indexOf('\u03a3\u03a3', 0, 'ucs2'), 'Sigma Sigma');
                assert.strictEqual(
                    6, twoByteString.indexOf('\u03a3\u0395', 0, 'ucs2'), 'Sigma Epsilon');
            }


            const allCharsBufferUcs2 = Buffer.from(allCharsString, 'ucs2');

            // Search for string long enough to trigger complex search with ASCII pattern
            // and UC16 subject.
            assert.strictEqual(-1, allCharsBufferUcs2.indexOf('notfound'));

            // Needle is longer than haystack, but only because it's encoded as UTF-16
            assert.strictEqual(Buffer.from('aaaa').indexOf('a'.repeat(4), 'ucs2'), -1);

            assert.strictEqual(Buffer.from('aaaa').indexOf('a'.repeat(4), 'utf8'), 0);
            assert.strictEqual(Buffer.from('aaaa').indexOf('你好', 'ucs2'), -1);

            // Haystack has odd length, but the needle is UCS2.
            assert.strictEqual(Buffer.from('aaaaa').indexOf('b', 'ucs2'), -1);

            {
                // Find substrings in Usc2.
                const lengths = [2, 4, 16];  // Single char, simple and complex.
                const indices = [0x5, 0x65, 0x105, 0x205, 0x285];// 0x2005, 0x2085, 0xfff0 todo
                for (let lengthIndex = 0; lengthIndex < lengths.length; lengthIndex++) {
                    for (let i = 0; i < indices.length; i++) {
                        const index = indices[i] * 2;
                        const length = lengths[lengthIndex];

                        const patternBufferUcs2 =
                            allCharsBufferUcs2.slice(index, index + length);
                        assert.strictEqual(
                            index, allCharsBufferUcs2.indexOf(patternBufferUcs2, 0, 'ucs2'));

                        const patternStringUcs2 = patternBufferUcs2.toString('ucs2');
                        assert.strictEqual(
                            index, allCharsBufferUcs2.indexOf(patternStringUcs2, 0, 'ucs2'));
                    }
                }
            }
        });

        it('test invalid and uppercase encoding', () => {
            assert.strictEqual(b.indexOf('b', 'utf8'), 1);
            assert.strictEqual(b.indexOf('b', 'UTF8'), 1);
            assert.strictEqual(b.indexOf('62', 'HEX'), 1);
            assert.throws(() => b.indexOf('bad', 'enc'), /Unknown encoding: enc/);
        });

        it('hex', () => {
            assert.strictEqual(
                Buffer.from(b.toString('hex'), 'hex')
                    .indexOf('64', 0, 'hex'),
                3
            );
            assert.strictEqual(Buffer.from(b.toString('hex'), 'hex').indexOf(Buffer.from('64', 'hex'), 0, 'hex'), 3);
            // test optional offset with passed encoding
            assert.strictEqual(Buffer.from('aaaa0').indexOf('30', 'hex'), 4);
            assert.strictEqual(Buffer.from('aaaa00a').indexOf('3030', 'hex'), 4);
        });

        it('base64', () => {
            // test base64 encoding
            assert.strictEqual(
                Buffer.from(b.toString('base64'), 'base64')
                    .indexOf('ZA==', 0, 'base64'),
                3
            );
            assert.strictEqual(
                Buffer.from(b.toString('base64'), 'base64')
                    .indexOf(Buffer.from('ZA==', 'base64'), 0, 'base64'),
                3
            );
        });

        it('ascii', () => {
            assert.strictEqual(
                Buffer.from(b.toString('ascii'), 'ascii')
                    .indexOf('d', 0, 'ascii'),
                3
            );
            assert.strictEqual(
                Buffer.from(b.toString('ascii'), 'ascii')
                    .indexOf(Buffer.from('d', 'ascii'), 0, 'ascii'),
                3
            );

            // Search for a non-ASCII string in a pure ASCII string.
            const asciiString = Buffer.from(
                'arglebargleglopglyfarglebargleglopglyfarglebargleglopglyf');
            assert.strictEqual(-1, asciiString.indexOf('\x2061'));
            assert.strictEqual(3, asciiString.indexOf('leb', 0));
        });

        it('latin1', () => {
            assert.strictEqual(
                Buffer.from(b.toString('latin1'), 'latin1')
                    .indexOf('d', 0, 'latin1'),
                3
            );
            assert.strictEqual(
                Buffer.from(b.toString('latin1'), 'latin1')
                    .indexOf(Buffer.from('d', 'latin1'), 0, 'latin1'),
                3
            );
            assert.strictEqual(
                Buffer.from('aa\u00e8aa', 'latin1')
                    .indexOf('\u00e8', 'latin1'),
                2
            );
            assert.strictEqual(
                Buffer.from('\u00e8', 'latin1')
                    .indexOf('\u00e8', 'latin1'),
                0
            );
            assert.strictEqual(
                Buffer.from('\u00e8', 'latin1')
                    .indexOf(Buffer.from('\u00e8', 'latin1'), 'latin1'),
                0
            );
        });

        it('binary', () => {
            assert.strictEqual(
                Buffer.from(b.toString('binary'), 'binary')
                    .indexOf('d', 0, 'binary'),
                3
            );
            assert.strictEqual(
                Buffer.from(b.toString('binary'), 'binary')
                    .indexOf(Buffer.from('d', 'binary'), 0, 'binary'),
                3
            );
            assert.strictEqual(
                Buffer.from('aa\u00e8aa', 'binary')
                    .indexOf('\u00e8', 'binary'),
                2
            );
            assert.strictEqual(
                Buffer.from('\u00e8', 'binary')
                    .indexOf('\u00e8', 'binary'),
                0
            );
            assert.strictEqual(
                Buffer.from('\u00e8', 'binary')
                    .indexOf(Buffer.from('\u00e8', 'binary'), 'binary'),
                0
            );
        });

        it('utf8', () => {
            const mixedByteStringUtf8 = Buffer.from('\u039a\u0391abc\u03a3\u03a3\u0395');
            assert.strictEqual(5, mixedByteStringUtf8.indexOf('bc'));
            assert.strictEqual(5, mixedByteStringUtf8.indexOf('bc', 5));
            assert.strictEqual(5, mixedByteStringUtf8.indexOf('bc', -8));
            assert.strictEqual(7, mixedByteStringUtf8.indexOf('\u03a3'));
            assert.strictEqual(-1, mixedByteStringUtf8.indexOf('\u0396'));


            // Test complex string indexOf algorithms. Only trigger for long strings.
            // Long string that isn't a simple repeat of a shorter string.
            let longString = 'A';
            for (let i = 66; i < 76; i++) {  // from 'B' to 'K'
                longString = longString + String.fromCharCode(i) + longString;
            }

            const longBufferString = Buffer.from(longString);

            // pattern of 15 chars, repeated every 16 chars in long
            let pattern = 'ABACABADABACABA';
            for (let i = 0; i < longBufferString.length - pattern.length; i += 7) {
                const index = longBufferString.indexOf(pattern, i);
                assert.strictEqual((i + 15) & ~0xf, index,
                    `Long ABACABA...-string at index ${i}`);
            }
            assert.strictEqual(510, longBufferString.indexOf('AJABACA'),
                'Long AJABACA, First J');
            assert.strictEqual(
                1534, longBufferString.indexOf('AJABACA', 511), 'Long AJABACA, Second J');

            pattern = 'JABACABADABACABA';
            assert.strictEqual(
                511, longBufferString.indexOf(pattern), 'Long JABACABA..., First J');
            assert.strictEqual(
                1535, longBufferString.indexOf(pattern, 512), 'Long JABACABA..., Second J');

            const allCharsBufferUtf8 = Buffer.from(allCharsString);
            assert.strictEqual(-1, allCharsBufferUtf8.indexOf('notfound'));

            {
                // Find substrings in Utf8.
                const lengths = [1, 3, 15];  // Single char, simple and complex.
                const indices = [0x5, 0x60, 0x400, 0x680, 0x7ee, 0xFF02, 0x16610];// 0x2f77b todo
                for (let lengthIndex = 0; lengthIndex < lengths.length; lengthIndex++) {
                    for (let i = 0; i < indices.length; i++) {
                        const index = indices[i];
                        let length = lengths[lengthIndex];

                        if (index + length > 0x7F) {
                            length = 2 * length;
                        }

                        if (index + length > 0x7FF) {
                            length = 3 * length;
                        }

                        if (index + length > 0xFFFF) {
                            length = 4 * length;
                        }

                        const patternBufferUtf8 = allCharsBufferUtf8.slice(index, index + length);
                        assert.strictEqual(index, allCharsBufferUtf8.indexOf(patternBufferUtf8));

                        const patternStringUtf8 = patternBufferUtf8.toString();
                        assert.strictEqual(index, allCharsBufferUtf8.indexOf(patternStringUtf8));
                    }
                }
            }
        });

        it('coerce', () => {
            // Test weird offset arguments.
            // The following offsets coerce to NaN or 0, searching the whole Buffer
            assert.strictEqual(b.indexOf('b', undefined), 1);
            assert.strictEqual(b.indexOf('b', {}), 1);
            assert.strictEqual(b.indexOf('b', 0), 1);
            assert.strictEqual(b.indexOf('b', null), 1);
            assert.strictEqual(b.indexOf('b', []), 1);

            // The following offset coerces to 2, in other words +[2] === 2
            assert.strictEqual(b.indexOf('b', [2]), -1);

            // Behavior should match String.indexOf()
            assert.strictEqual(
                b.indexOf('b', undefined),
                s.indexOf('b', undefined));
            assert.strictEqual(
                b.indexOf('b', {}),
                s.indexOf('b', {}));
            assert.strictEqual(
                b.indexOf('b', 0),
                s.indexOf('b', 0));
            assert.strictEqual(
                b.indexOf('b', null),
                s.indexOf('b', null));
            assert.strictEqual(
                b.indexOf('b', []),
                s.indexOf('b', []));
            assert.strictEqual(
                b.indexOf('b', [2]),
                s.indexOf('b', [2]));
        });

        it('uint8', () => {
            // test truncation of Number arguments to uint8
            {
                const buf = Buffer.from('this is a test');
                assert.strictEqual(buf.indexOf(0x6973), 3);
                assert.strictEqual(buf.indexOf(0x697320), 4);
                assert.strictEqual(buf.indexOf(0x69732069), 2);
                // assert.strictEqual(buf.indexOf(0x697374657374), 0);// todo
                assert.strictEqual(buf.indexOf(0x69737374), 0);
                assert.strictEqual(buf.indexOf(0x69737465), 11);
                assert.strictEqual(buf.indexOf(0x69737465), 11);
                assert.strictEqual(buf.indexOf(-140), 0);
                assert.strictEqual(buf.indexOf(-152), 1);
                assert.strictEqual(buf.indexOf(0xff), -1);
                assert.strictEqual(buf.indexOf(0xffff), -1);
            }

            // Test that Uint8Array arguments are okay.
            {
                const needle = new Uint8Array([0x66, 0x6f, 0x6f]);
                const haystack = Buffer.from('a foo b foo');
                assert.strictEqual(haystack.indexOf(needle), 2);
                // assert.strictEqual(haystack.lastIndexOf(needle), haystack.length - 3);
            }
        });
    });

    var fixtures = [{
            "a": "ffff00",
            "expected": "00ffff"
        },
        {
            "a": "ffff",
            "expected": "ffff"
        },
        {
            "a": "0000",
            "expected": "0000"
        },
        {
            "a": "0000ff",
            "expected": "ff0000"
        },
        {
            "a": "000000",
            "expected": "000000"
        },
        {
            "a": "ffffff",
            "expected": "ffffff"
        },
        {
            "a": "00ffff00ff",
            "expected": "ff00ffff00"
        },
        {
            "a": "0000ff00ffff00ff",
            "expected": "ff00ffff00ff0000"
        }
    ];

    it('reverse', () => {
        fixtures.forEach((f) => {
            var a = new Buffer(f.a, 'hex');
            assert.equal(a.reverse().toString('hex'), f.expected);
            assert.equal(a.toString('hex'), f.a);
        })
    });

    it('indexed setter', () => {
        const b = new Buffer([1, 2]);
        b[0] = -1;
        assert.deepEqual(new Buffer([255, 2]), b);
        b[0] = -255;
        assert.deepEqual(new Buffer([1, 2]), b);
        b[0] = -256;
        assert.deepEqual(new Buffer([0, 2]), b);
        b[0] = -257;
        assert.deepEqual(new Buffer([255, 2]), b);

        b[0] = 255;
        assert.deepEqual(new Buffer([255, 2]), b);
        b[0] = 256;
        assert.deepEqual(new Buffer([0, 2]), b);
        b[0] = 257;
        assert.deepEqual(new Buffer([1, 2]), b);

        b[3] = -1;
        assert.deepEqual(new Buffer([1, 2]), b);
    });

    it('indexed getter', () => {
        const b = new Buffer([1, 2]);
        assert.isUndefined(b[3]);
        assert.isUndefined(b[-1]);
        assert.equal(b[0], 1);
        assert.equal(b[1], 2);
    });
});

repl && test.run(console.DEBUG);