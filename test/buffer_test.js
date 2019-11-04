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

        var arr = new Uint8Array([0x10, 0x20, 0x30]);
        var arr1 = new Uint8Array(arr.buffer, 1, 2);
        var buf = new Buffer(arr1);
        assert.equal(buf.length, 2);
        assert.equal(buf.hex(), "2030");
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
            bufRes = Buffer.concat([() => {}, {}, undefined, '']);
        });
    });

    it('concat error', () => {
        var buf2 = Buffer.concat([]);
        assert.equal(buf2.length, 0);
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
        assert.equal(Buffer.byteLength(function () {}), 14);
        assert.equal(Buffer.byteLength(() => {}), 8);
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
        assert.equal(Buffer.compare(buf, "abcd"), 0);
        assert.greaterThan(buf.compare(new Buffer("abc")), 0);
        assert.greaterThan(Buffer.compare(buf, "abc"), 0);
        assert.lessThan(buf.compare(new Buffer("abcde")), 0);
        assert.lessThan(Buffer.compare(buf, "abcde"), 0);

        buf = new Buffer([1, 0, 1]);
        assert.equal(buf.equals(new Buffer([1, 0, 1])), true);
        assert.equal(Buffer.compare(buf, new Buffer([1, 0, 1])), 0);
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
            0x10
        ]);

        assert.equal(buf.readIntBE(0, 6).toString(16), "123456789abc");
        assert.equal(buf.readIntLE(0, 6).toString(16), "-436587a9cbee");

        assert.equal(buf.readIntBE(1, 5).toString(16), "3456789abc");
        assert.equal(buf.readIntLE(1, 5).toString(16), "-436587a9cc");

        assert.equal(buf.readUIntBE(0, 6).toString(16), "123456789abc");
        assert.equal(buf.readUIntLE(0, 6).toString(16), "bc9a78563412");

        assert.equal(buf.readIntBE(1, 5).toString(16), "3456789abc");
        assert.equal(buf.readIntLE(1, 5).toString(16), "-436587a9cc");

        assert.equal(buf.readInt64BE().toString(16), "123456789abcde10");
        assert.equal(buf.readInt64LE().toString(16), "10debc9a78563412");

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

        assert.equal(buf.writeUInt16BE(9026n, 0), 2);
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

        assert.equal(buf.writeInt32LE("0x12345679", 0), 4);
        assert.deepEqual(buf.toArray(), [
            0x79,
            0x56,
            0x34,
            0x12
        ]);

        var buf = new Buffer(6);

        assert.equal(buf.writeIntBE(0x12345678abcd, 0, 6), 6);
        assert.deepEqual(buf.toArray(), [
            0x12,
            0x34,
            0x56,
            0x78,
            0xab,
            0xcd
        ]);

        assert.equal(buf.writeIntLE(0x12345678abcd, 0, 6), 6);
        assert.deepEqual(buf.toArray(), [
            0xcd,
            0xab,
            0x78,
            0x56,
            0x34,
            0x12
        ]);

        assert.equal(buf.writeIntBE(-0x12345678abcd, 0, 6), 6);
        assert.deepEqual(buf.toArray(), [
            0xed,
            0xcb,
            0xa9,
            0x87,
            0x54,
            0x33
        ]);

        assert.equal(buf.writeIntLE(-0x12345678abcd, 0, 6), 6);
        assert.deepEqual(buf.toArray(), [
            0x33,
            0x54,
            0x87,
            0xa9,
            0xcb,
            0xed
        ]);

        var buf = new Buffer(8);

        assert.equal(buf.writeInt64BE(BigInt('0x3112345678abcdef'), 0), 8);
        assert.deepEqual(buf.toArray(), [
            0x31,
            0x12,
            0x34,
            0x56,
            0x78,
            0xab,
            0xcd,
            0xef
        ]);

        assert.equal(buf.writeInt64LE(BigInt('0x3112345678abcdef'), 0), 8);
        assert.deepEqual(buf.toArray(), [
            0xef,
            0xcd,
            0xab,
            0x78,
            0x56,
            0x34,
            0x12,
            0x31
        ]);

        buf.writeInt64BE(BigInt('0x7fffffffffffffff'), 0);
        assert.deepEqual(buf.toArray(), [
            0x7f,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff
        ]);

        buf.writeInt64BE(9007199254740992, 0);
        assert.deepEqual(buf.toArray(), [
            0,
            32,
            0,
            0,
            0,
            0,
            0,
            0
        ]);

        buf.writeInt64BE(9007199254740999n, 0);
        assert.deepEqual(buf.toArray(), [
            0,
            32,
            0,
            0,
            0,
            0,
            0,
            7
        ]);

        assert.throws(() => {
            buf.writeInt64LE(BigInt('0x8000000000000000'), 0);
        });

        var buf = new Buffer(4);
        assert.equal(buf.writeFloatLE(1, 0), 4);
        assert.equal(buf.hex(), "0000803f");

        var buf = new Buffer(8);
        assert.equal(buf.writeDoubleLE(0.3333333333333333, 0), 8);
        assert.equal(buf.hex(), "555555555555d53f");
    });

    it("readInt/writeInt", () => {
        let buf = Buffer.allocUnsafe(3);

        buf.writeUIntLE(0x123456, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0x56, 0x34, 0x12]);
        assert.equal(buf.readUIntLE(0, 3), 0x123456);

        buf.fill(0xFF);
        buf.writeUIntBE(0x123456, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0x12, 0x34, 0x56]);
        assert.equal(buf.readUIntBE(0, 3), 0x123456);

        buf.fill(0xFF);
        buf.writeIntLE(0x123456, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0x56, 0x34, 0x12]);
        assert.equal(buf.readIntLE(0, 3), 0x123456);

        buf.fill(0xFF);
        buf.writeIntBE(0x123456, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0x12, 0x34, 0x56]);
        assert.equal(buf.readIntBE(0, 3), 0x123456);

        buf.fill(0xFF);
        buf.writeIntLE(-0x123456, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0xaa, 0xcb, 0xed]);
        assert.equal(buf.readIntLE(0, 3), -0x123456);

        buf.fill(0xFF);
        buf.writeIntBE(-0x123456, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0xed, 0xcb, 0xaa]);
        assert.equal(buf.readIntBE(0, 3), -0x123456);

        buf.fill(0xFF);
        buf.writeIntLE(-0x123400, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0x00, 0xcc, 0xed]);
        assert.equal(buf.readIntLE(0, 3), -0x123400);

        buf.fill(0xFF);
        buf.writeIntBE(-0x123400, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0xed, 0xcc, 0x00]);
        assert.equal(buf.readIntBE(0, 3), -0x123400);

        buf.fill(0xFF);
        buf.writeIntLE(-0x120000, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0x00, 0x00, 0xee]);
        assert.equal(buf.readIntLE(0, 3), -0x120000);

        buf.fill(0xFF);
        buf.writeIntBE(-0x120000, 0, 3);
        assert.deepEqual(buf.toJSON().data, [0xee, 0x00, 0x00]);
        assert.equal(buf.readIntBE(0, 3), -0x120000);

        buf = Buffer.allocUnsafe(5);
        buf.writeUIntLE(0x1234567890, 0, 5);
        assert.deepEqual(buf.toJSON().data, [0x90, 0x78, 0x56, 0x34, 0x12]);
        assert.equal(buf.readUIntLE(0, 5), 0x1234567890);

        buf.fill(0xFF);
        buf.writeUIntBE(0x1234567890, 0, 5);
        assert.deepEqual(buf.toJSON().data, [0x12, 0x34, 0x56, 0x78, 0x90]);
        assert.equal(buf.readUIntBE(0, 5), 0x1234567890);

        buf.fill(0xFF);
        buf.writeIntLE(0x1234567890, 0, 5);
        assert.deepEqual(buf.toJSON().data, [0x90, 0x78, 0x56, 0x34, 0x12]);
        assert.equal(buf.readIntLE(0, 5), 0x1234567890);

        buf.fill(0xFF);
        buf.writeIntBE(0x1234567890, 0, 5);
        assert.deepEqual(buf.toJSON().data, [0x12, 0x34, 0x56, 0x78, 0x90]);
        assert.equal(buf.readIntBE(0, 5), 0x1234567890);

        buf.fill(0xFF);
        buf.writeIntLE(-0x1234567890, 0, 5);
        assert.deepEqual(buf.toJSON().data, [0x70, 0x87, 0xa9, 0xcb, 0xed]);
        assert.equal(buf.readIntLE(0, 5), -0x1234567890);

        buf.fill(0xFF);
        buf.writeIntBE(-0x1234567890, 0, 5);
        assert.deepEqual(buf.toJSON().data, [0xed, 0xcb, 0xa9, 0x87, 0x70]);
        assert.equal(buf.readIntBE(0, 5), -0x1234567890);

        buf.fill(0xFF);
        buf.writeIntLE(-0x0012000000, 0, 5);
        assert.deepEqual(buf.toJSON().data, [0x00, 0x00, 0x00, 0xee, 0xff]);
        assert.equal(buf.readIntLE(0, 5), -0x0012000000);

        buf.fill(0xFF);
        buf.writeIntBE(-0x0012000000, 0, 5);
        assert.deepEqual(buf.toJSON().data, [0xff, 0xee, 0x00, 0x00, 0x00]);
        assert.equal(buf.readIntBE(0, 5), -0x0012000000);
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

    it('join', () => {
        var a = new Buffer([192, 168, 0, 1]);
        assert.equal(a.join('.'), '192.168.0.1');
        assert.equal(a.join(), '192,168,0,1');
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

require.main === module && test.run(console.DEBUG);