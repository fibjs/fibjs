const { describe, it } = require("node:test");
const assert = require("assert");

var os = require("os");
var vm = require("vm");

var is_big_endian = os.endianness() === 'BE';

describe('Buffer', () => {
    it('instanceof', () => {
        var buf = Buffer.from("abcd");
        assert.equal(buf instanceof Buffer, true);
        assert.equal(buf instanceof Uint8Array, true);
    });

    it('Buffer.from(String)', () => {
        var buf = Buffer.from("abcd");
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "abcd");

        var buf = Buffer.from("100");
        assert.equal(buf.length, 3);
        assert.equal(buf.toString(), "100");
    });

    it('Buffer.from(Array)', () => {
        var buf = Buffer.from([0x31, 0x32, 0x33, 0x34]);
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "1234");
    });

    it('Buffer.from(Array) with undefined encoding', () => {
        var buf = Buffer.from([0x31, 0x32, 0x33, 0x34]);
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "1234");
    });

    it('Buffer.from(Uint8Array)', () => {
        var arr = new Uint8Array(2);
        arr[0] = 50;
        arr[1] = 40;

        var buf = Buffer.from(arr);

        assert.equal(buf.length, 2);
        assert.equal(buf.toString('hex'), "3228");

        var arr = new Uint8Array([0x10, 0x20, 0x30]);
        var arr1 = new Uint8Array(arr.buffer, 1, 2);
        var buf = Buffer.from(arr1);
        assert.equal(buf.length, 2);
        assert.equal(buf.toString('hex'), "2030");
    });

    it('Buffer.from(ArrayBuffer)', () => {
        var arr = new Uint16Array(2);
        arr[0] = 5000;
        arr[1] = 4000;

        var buf = Buffer.from(arr.buffer);

        assert.equal(buf.length, 4);
        assert.equal(buf.toString('hex'), is_big_endian ? "13880fa0" : "8813a00f");
    });

    it('Buffer.from(Buffer)', () => {
        var buf = Buffer.from(Buffer.from("abcd"));
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "abcd");
    });

    it('Buffer.from(date)', () => {
        var data = Buffer.from(new Date("2016-03-09T07:58:57.303Z").toISOString());
        assert.equal(new Date(data.toString()).toISOString(), "2016-03-09T07:58:57.303Z");
    });

    it('isBuffer', () => {
        var buf = Buffer.from("abcd");
        var str = "abcd"
        assert.equal(Buffer.isBuffer(buf), true);
        assert.equal(Buffer.isBuffer(str), false);
    });

    it('concat', () => {
        var buf1 = Buffer.from("abcd");
        var buf2 = Buffer.from("efg");
        var buf3;
        var bufArray = [buf1];
        var bufRes = Buffer.concat(bufArray);
        assert.equal(bufRes.toString(), "abcd")

        bufArray = [buf1, buf2];
        bufRes = Buffer.concat(bufArray);
        assert.equal(bufRes.toString(), "abcdefg")

        bufRes = Buffer.concat(bufArray, 6);
        assert.equal(bufRes.toString(), "abcdef")

        buf1 = Buffer.from([0x31, 0x32, 0x33, 0x34]);
        buf2 = Buffer.from([0x35, 0x36, 0x37, 0x38]);
        bufArray = [buf1, buf2];
        bufRes = Buffer.concat(bufArray);
        assert.equal(bufRes.length, 8);
        assert.equal(bufRes.toString(), "12345678");
        bufRes = Buffer.concat(bufArray, 7);
        assert.equal(bufRes.length, 7);
        assert.equal(bufRes.toString(), "1234567");

        buf1 = Buffer.from([1, 2, 3, 4]);
        buf2 = Buffer.from([5, 6, 7, 8]);
        buf3 = Buffer.from([135, 136]);
        bufArray = [buf1, buf2, buf3];
        bufRes = Buffer.concat(bufArray);
        for (var i = 0; i < 8; i++) {
            assert.equal(bufRes[i], i + 1);
        }
        assert.equal(bufRes[8], 135);
        assert.equal(bufRes[9], 136);
        buf1 = Buffer.from('');
        bufArray = [buf1];
    });

    it('concat error', () => {
        var buf2 = Buffer.concat([]);
        assert.equal(buf2.length, 0);
    });

    it('concat type error', () => {
        assert.throws(() => {
            Buffer.concat(["aaa"]);
        });
    });

    it('Buffer.from(String)', () => {
        var buf = Buffer.from("abcd");
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "abcd");

        var buf = Buffer.from("100");
        assert.equal(buf.length, 3);
        assert.equal(buf.toString(), "100");

        const expected = [0xff, 0xff, 0xbe, 0xff, 0xef, 0xbf, 0xfb, 0xef, 0xff];
        assert.deepEqual(Buffer.from('//++/++/++//', 'base64'),
            Buffer.from(expected));
        assert.deepEqual(Buffer.from('__--_--_--__', 'base64'),
            Buffer.from(expected));

        assert.equal(Buffer.from("6am+77yG556/", "base64").toString(), "驾＆瞿");
        assert.equal(Buffer.from("6am-77yG556_", "base64url").toString(), "驾＆瞿");
    });

    it('Buffer.from(String, utf16le)', () => {
        // Test for proper UTF16LE encoding, length should be 8
        const f = Buffer.from('über', 'utf16le');
        assert.deepEqual(f, Buffer.from([252, 0, 98, 0, 101, 0, 114, 0]));
        assert.strictEqual(f.toString('utf16le'), 'über');

        // Length should be 12
        const f2 = Buffer.from('привет', 'utf16le');
        assert.deepEqual(f2, Buffer.from([63, 4, 64, 4, 56, 4, 50, 4, 53, 4, 66, 4]));
        assert.strictEqual(f2.toString('utf16le'), 'привет');
    });

    it('Buffer.from(Array)', () => {
        var buf = Buffer.from([0x31, 0x32, 0x33, 0x34]);
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "1234");
    });

    it('Buffer.from(ArrayBuffer)', () => {
        var arr = new Uint16Array(2);
        arr[0] = 5000;
        arr[1] = 4000;

        var buf = Buffer.from(arr.buffer);

        assert.equal(buf.length, 4);
        assert.equal(buf.toString('hex'), is_big_endian ? "13880fa0" : "8813a00f");
    });

    it('Buffer.from(Uint8Array, offset)', () => {
        var sourceArray = new Uint8Array([1, 2, 3, 4, 5, 6, 7, 8, 9]);
        var arr = new Uint8Array(sourceArray.buffer, 2, 4);

        assert.equal(Buffer.from(arr).toString('hex'), "03040506");
    });

    it('Buffer.from(Buffer)', () => {
        var buf = Buffer.from(Buffer.from("abcd"));
        assert.equal(buf.length, 4);
        assert.equal(buf.toString(), "abcd");
    });

    it('Buffer.from encoding', () => {
        var txts = ['abc',
            '\u0222aa',
            'a\u0234b\u0235c\u0236'];

        var results = {
            utf8: ['616263', 'c8a26161', '61c8b462c8b563c8b6'],
            ucs2: ['610062006300', '220261006100', '610034026200350263003602'],
            binary: ['616263', '226161', '613462356336'],
            latin1: ['616263', '226161', '613462356336']
        };

        ['utf8', 'ucs2', 'binary', 'latin1'].forEach(codec => {
            var rs = results[codec];
            txts.forEach((txt, i) => {
                assert.equal(Buffer.from(txt, codec).toString('hex'), rs[i]);
            })
        });
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
    });

    it('Buffer.byteLength(Uint8Array)', () => {
        var buf = new ArrayBuffer(8);
        var int32Array = new Uint8Array(buf);
        assert.equal(Buffer.byteLength(int32Array), 8);
    });

    it('Buffer.byteLength(Buffer)', () => {
        var buf1 = Buffer.from("abcd");
        var buf2 = Buffer.from([98, 117, 102, 102, 101, 114]);
        assert.equal(Buffer.byteLength(buf1), 4);
        assert.equal(Buffer.byteLength(buf2), 6);
    });

    // xit('Buffer.byteLength(other)', () => {
    //     assert.equal(Buffer.byteLength({}), 15);
    //     assert.equal(Buffer.byteLength(function () { }), 15);
    //     assert.equal(Buffer.byteLength(() => { }), 9);
    //     assert.equal(Buffer.byteLength([]), 0);
    // });

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
        var buf1 = Buffer.alloc(10, Buffer.from("h"));
        var buf2 = Buffer.alloc(3, Buffer.from("hello"));
        var buf3 = Buffer.alloc(6, Buffer.from([0x31, 0x32, 0x33, 0x34]));
        var buf4 = Buffer.alloc(22, Buffer.from('aGVsbG8gd29ybGQ=', 'base64'));
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
        assert.equal(Buffer.isEncoding('hex'), true);
        assert.equal(Buffer.isEncoding('base64'), true);
        assert.equal(Buffer.isEncoding('base64url'), true);
        assert.equal(Buffer.isEncoding('binary'), true);
        assert.equal(Buffer.isEncoding('latin1'), true);
    });

    it('@iterator', () => {
        var buf1 = Buffer.from("buffer");
        var buf2 = Buffer.from([98, 117, 102, 102, 101, 114]);
        var correctResult = [98, 117, 102, 102, 101, 114];
        var values1 = [];
        var values2 = [];
        for (let value of buf1[Symbol.iterator]()) {
            values1.push(value);
        }
        assert.deepEqual(values1, correctResult);

        for (let value of buf2[Symbol.iterator]()) {
            values2.push(value);
        }
        assert.deepEqual(values2, correctResult);

        var it = buf1[Symbol.iterator]();
        assert.equal(it, it[Symbol.iterator]());
    });

    it('keys', () => {
        var buf1 = Buffer.from("buffer");
        var buf2 = Buffer.from([98, 117, 102, 102, 101, 114]);
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

        var it = buf1.keys();
        assert.equal(it, it[Symbol.iterator]());
    });

    it('values', () => {
        var buf1 = Buffer.from("buffer");
        var buf2 = Buffer.from([98, 117, 102, 102, 101, 114]);
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

        var it = buf1.values();
        assert.equal(it, it[Symbol.iterator]());
    });

    it('entries', () => {
        var buf1 = Buffer.from("buffer");
        var buf2 = Buffer.from([98, 117, 102, 102, 101, 114]);
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

        var it = buf1.entries();
        assert.equal(it, it[Symbol.iterator]());
    });

    it('toString', () => {
        var buf = Buffer.from([0x31, 0x32, 0x33, 0x34]);
        assert.equal(buf.toString("utf8"), "1234");
        assert.equal(buf.toString(undefined), "1234");
        assert.equal(buf.toString("hex"), "31323334");
        assert.equal(buf.toString("base64"), "MTIzNA==");
        assert.equal(buf.toString("utf8", 1), "234");
        assert.equal(buf.toString("utf8", 1, 3), "23");
        assert.equal(buf.toString("hex", 2), "3334");
        assert.equal(buf.toString("base64", 2), "MzQ=");
        assert.equal(buf.toString("base64url"), "MTIzNA");

        buf = Buffer.concat([Buffer.alloc(5), Buffer.from("abcd")]);
        assert.equal(buf.toString("utf8", 5), "abcd");

        var buf1 = Buffer.from('this is a tést');
        assert.equal(buf1.toString(), 'this is a tést');
        assert.equal(buf1.toString('ascii'), 'this is a tC)st');

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

    it('write', () => {
        var buf = Buffer.from([0x31, 0x32, 0x33, 0x34]);
        assert.equal(buf.toString(), "1234");

        buf = Buffer.alloc(10);
        assert.equal(buf.write("abcd", 0, 4), 4);
        assert.equal(buf.toString('utf8', 0, 4), "abcd");
        assert.equal(buf.toString('utf8', 0, 3), "abc");

        buf = Buffer.alloc(10);
        assert.equal(buf.write("MTIzNA==", 0, 4, "base64"), 4);
        assert.equal(buf.toString("utf8", 0, 4), "1234");

        assert.equal(buf.write("31323334", 0, 4, "hex"), 4);
        assert.equal(buf.toString("utf8", 0, 4), "1234");

        assert.equal(buf.write("abcde", 1, 4), 4);
        assert.equal(buf.toString('utf8', 1, 4), "abc");

        buf = Buffer.alloc(3);
        assert.equal(buf.write("abcd", 0, 3), 3);
        assert.equal(buf.toString('utf8', 0, 3), "abc");

        buf = Buffer.alloc(3);
        assert.equal(buf.write("abcd", 0, "utf8"), 3);
        assert.equal(buf.toString('utf8', 0, 3), "abc");

        buf = Buffer.alloc(3);
        assert.equal(buf.write("abcd", "utf8"), 3);
        assert.equal(buf.toString('utf8', 0, 3), "abc");
    });

    it('fill', () => {
        var buf = Buffer.alloc(5);
        buf.fill(10);
        for (var i = 0; i < 5; i++)
            assert.equal(buf[i], 10);

        buf = Buffer.alloc(10);
        buf.fill("abc");
        assert.equal(buf.toString(), "abcabcabca");

        buf.fill("abcabcabcabc");
        assert.equal(buf.toString(), "abcabcabca");

        buf = Buffer.alloc(10);
        var buf1 = buf.fill(Buffer.from([0, 1, 2]));
        for (var i = 0; i < 3; i++) {
            assert.equal(buf[i], i);
            assert.equal(buf[i + 3], i);
            assert.equal(buf[i + 6], i);
        }
        assert.equal(buf[9], 0);
        assert.equal(buf, buf1);

        buf = Buffer.alloc(20);
        buf.fill("abc");
        assert.equal(buf.toString(), 'abcabcabcabcabcabcab');
    });

    it('slice', () => {
        var buf = Buffer.alloc(5);
        buf.fill(10);
        var sli = buf.slice(1, 4);
        for (var i = 0; i < 3; i++)
            assert.equal(sli[i], 10);

        buf = Buffer.alloc(10);
        buf.write("abcdefghih");
        assert.equal(buf.slice(0, 3), "abc");
        assert.equal(buf.slice(6, 5), "");
        assert.equal(buf.slice(0, 11), "abcdefghih");
        assert.equal(buf.slice(8), "ih");
        assert.equal(buf.slice(-20, 2), "ab");

        var buf = Buffer.from('buffer'); //TODO slice 反向的支持
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
        var buf = Buffer.from("abcd");
        assert.equal(buf.equals(Buffer.from("abcd")), true);
        assert.equal(buf.equals(Buffer.from("abc")), false);
        assert.equal(buf.compare(Buffer.from("abcd")), 0);
        assert.equal(Buffer.compare(buf, Buffer.from("abcd")), 0);
        assert.ok(buf.compare(Buffer.from("abc")) > 0);
        assert.ok(Buffer.compare(buf, Buffer.from("abc")) > 0);
        assert.ok(buf.compare(Buffer.from("abcde")) < 0);
        assert.ok(Buffer.compare(buf, Buffer.from("abcde")) < 0);

        buf = Buffer.from([1, 0, 1]);
        assert.equal(buf.equals(Buffer.from([1, 0, 1])), true);
        assert.equal(Buffer.compare(buf, Buffer.from([1, 0, 1])), 0);
        assert.equal(buf.equals(Buffer.from([1, 0, 2])), false);
    });

    it('copy', () => {
        var buf1 = Buffer.from([0x31, 0x32, 0x33]);
        var arr = [0x34, 0x35, 0x36];

        var buf2 = Buffer.from(arr);
        var sz = buf1.copy(buf2);
        assert.equal(sz, 3);
        assert.equal(buf2.toString(), '123');

        buf2 = Buffer.from(arr);
        sz = buf1.copy(buf2, 1);
        assert.equal(sz, 2);
        assert.equal(buf2.toString(), '412');

        buf2 = Buffer.from(arr);
        sz = buf1.copy(buf2, 1, 1);
        assert.equal(sz, 2);
        assert.equal(buf2.toString(), '423');

        buf2 = Buffer.from(arr);
        sz = buf1.copy(buf2, 1, 1, 2);
        assert.equal(sz, 1);
        assert.equal(buf2.toString(), '426');

        buf2 = Buffer.from(arr);
        sz = buf1.copy(buf2, 1, 1, 1);
        assert.equal(sz, 0);
        assert.equal(buf2.toString(), '456');

    });

    it('set', () => {
        var buf = Buffer.from([0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);

        buf.set([1, 2, 3], 3);
        assert.deepEqual(buf, Buffer.from([0, 0, 0, 1, 2, 3, 0, 0, 0, 0]));

        assert.throws(() => {
            buf.set([1, 2, 3], -1);
        });

        assert.throws(() => {
            buf.set([1, 2, 3], 9);
        });
    });

    it("readNumber", () => {
        var buf = Buffer.from([0x23, 0x42]);

        assert.equal(buf.readUInt8(), 35);

        assert.equal(buf.readUInt16BE(), 9026);
        assert.equal(buf.readUInt16LE(), 16931);

        assert.throws(() => {
            buf.readUInt16BE(1);
        });

        assert.throws(() => {
            buf.readUInt16LE(1);
        });

        var buf = Buffer.from([0xb3, 0x42]);

        assert.equal(buf.readInt16BE(), -19646);
        assert.equal(buf.readInt16LE(), 17075);

        var buf = Buffer.from([
            0x12,
            0x34,
            0x56,
            0x78
        ]);

        assert.equal(buf.readInt32BE(), 0x12345678);
        assert.equal(buf.readInt32LE(), 0x78563412);

        var buf = Buffer.from([
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

        assert.equal(buf.readBigInt64BE().toString(16), "123456789abcde10");
        assert.equal(buf.readBigInt64LE().toString(16), "10debc9a78563412");

        var buf = Buffer.from([
            0x00,
            0x00,
            0x80,
            0x3f
        ]);

        assert.equal(buf.readFloatLE(), 1);

        var buf = Buffer.from([
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
        var buf = Buffer.alloc(2);

        assert.equal(buf.writeUInt16BE(9026, 0), 2);
        assert.equal(buf.readUInt16BE(), 9026);

        assert.equal(buf.writeUInt16LE(16931, 0), 2);
        assert.equal(buf.readUInt16BE(), 9026);

        assert.equal(buf.writeUInt16BE(9026, 0), 2);
        assert.equal(buf.readUInt16BE(), 9026);

        assert.equal(buf.length, 2);

        assert.throws(() => {
            buf.writeUInt16BE(0, 1);
        });

        assert.throws(() => {
            buf.writeUInt16LE(0, 1);
        });

        var buf = Buffer.alloc(4);

        assert.equal(buf.writeInt32BE(0x12345678, 0), 4);
        assert.deepEqual(Array.from(buf), [
            0x12,
            0x34,
            0x56,
            0x78
        ]);

        assert.equal(buf.writeInt32LE(0x12345678, 0), 4);
        assert.deepEqual(Array.from(buf), [
            0x78,
            0x56,
            0x34,
            0x12
        ]);

        assert.equal(buf.writeInt32LE("0x12345679", 0), 4);
        assert.deepEqual(Array.from(buf), [
            0x79,
            0x56,
            0x34,
            0x12
        ]);

        var buf = Buffer.alloc(6);

        assert.equal(buf.writeIntBE(0x12345678abcd, 0, 6), 6);
        assert.deepEqual(Array.from(buf), [
            0x12,
            0x34,
            0x56,
            0x78,
            0xab,
            0xcd
        ]);

        assert.equal(buf.writeIntLE(0x12345678abcd, 0, 6), 6);
        assert.deepEqual(Array.from(buf), [
            0xcd,
            0xab,
            0x78,
            0x56,
            0x34,
            0x12
        ]);

        assert.equal(buf.writeIntBE(-0x12345678abcd, 0, 6), 6);
        assert.deepEqual(Array.from(buf), [
            0xed,
            0xcb,
            0xa9,
            0x87,
            0x54,
            0x33
        ]);

        assert.equal(buf.writeIntLE(-0x12345678abcd, 0, 6), 6);
        assert.deepEqual(Array.from(buf), [
            0x33,
            0x54,
            0x87,
            0xa9,
            0xcb,
            0xed
        ]);

        var buf = Buffer.alloc(8);

        assert.equal(buf.writeBigInt64BE(BigInt('0x3112345678abcdef'), 0), 8);
        assert.deepEqual(Array.from(buf), [
            0x31,
            0x12,
            0x34,
            0x56,
            0x78,
            0xab,
            0xcd,
            0xef
        ]);

        assert.equal(buf.writeBigInt64LE(BigInt('0x3112345678abcdef'), 0), 8);
        assert.deepEqual(Array.from(buf), [
            0xef,
            0xcd,
            0xab,
            0x78,
            0x56,
            0x34,
            0x12,
            0x31
        ]);

        buf.writeBigInt64BE(BigInt('0x7fffffffffffffff'), 0);
        assert.deepEqual(Array.from(buf), [
            0x7f,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff
        ]);

        buf.writeBigInt64BE(9007199254740992n, 0);
        assert.deepEqual(Array.from(buf), [
            0,
            32,
            0,
            0,
            0,
            0,
            0,
            0
        ]);

        buf.writeBigInt64BE(9007199254740999n, 0);
        assert.deepEqual(Array.from(buf), [
            0,
            32,
            0,
            0,
            0,
            0,
            0,
            7
        ]);

        // Node.js doesn't throw for this value, it wraps around
        // assert.throws(() => {
        //     buf.writeBigInt64LE(BigInt('0x8000000000000000'), 0);
        // });

        var buf = Buffer.alloc(4);
        assert.equal(buf.writeFloatLE(1, 0), 4);
        assert.equal(buf.toString('hex'), "0000803f");

        var buf = Buffer.alloc(8);
        assert.equal(buf.writeDoubleLE(0.3333333333333333, 0), 8);
        assert.equal(buf.toString('hex'), "555555555555d53f");
    });

    it("readInt/writeInt", () => {
        let buf = Buffer.allocUnsafe(3);

        buf.writeUIntLE(0x123456, 0, 3);
        assert.deepEqual(Array.from(buf), [0x56, 0x34, 0x12]);
        assert.equal(buf.readUIntLE(0, 3), 0x123456);

        buf.fill(0xFF);
        buf.writeUIntBE(0x123456, 0, 3);
        assert.deepEqual(Array.from(buf), [0x12, 0x34, 0x56]);
        assert.equal(buf.readUIntBE(0, 3), 0x123456);

        buf.fill(0xFF);
        buf.writeIntLE(0x123456, 0, 3);
        assert.deepEqual(Array.from(buf), [0x56, 0x34, 0x12]);
        assert.equal(buf.readIntLE(0, 3), 0x123456);

        buf.fill(0xFF);
        buf.writeIntBE(0x123456, 0, 3);
        assert.deepEqual(Array.from(buf), [0x12, 0x34, 0x56]);
        assert.equal(buf.readIntBE(0, 3), 0x123456);

        buf.fill(0xFF);
        buf.writeIntLE(-0x123456, 0, 3);
        assert.deepEqual(Array.from(buf), [0xaa, 0xcb, 0xed]);
        assert.equal(buf.readIntLE(0, 3), -0x123456);

        buf.fill(0xFF);
        buf.writeIntBE(-0x123456, 0, 3);
        assert.deepEqual(Array.from(buf), [0xed, 0xcb, 0xaa]);
        assert.equal(buf.readIntBE(0, 3), -0x123456);

        buf.fill(0xFF);
        buf.writeIntLE(-0x123400, 0, 3);
        assert.deepEqual(Array.from(buf), [0x00, 0xcc, 0xed]);
        assert.equal(buf.readIntLE(0, 3), -0x123400);

        buf.fill(0xFF);
        buf.writeIntBE(-0x123400, 0, 3);
        assert.deepEqual(Array.from(buf), [0xed, 0xcc, 0x00]);
        assert.equal(buf.readIntBE(0, 3), -0x123400);

        buf.fill(0xFF);
        buf.writeIntLE(-0x120000, 0, 3);
        assert.deepEqual(Array.from(buf), [0x00, 0x00, 0xee]);
        assert.equal(buf.readIntLE(0, 3), -0x120000);

        buf.fill(0xFF);
        buf.writeIntBE(-0x120000, 0, 3);
        assert.deepEqual(Array.from(buf), [0xee, 0x00, 0x00]);
        assert.equal(buf.readIntBE(0, 3), -0x120000);

        buf = Buffer.allocUnsafe(5);
        buf.writeUIntLE(0x1234567890, 0, 5);
        assert.deepEqual(Array.from(buf), [0x90, 0x78, 0x56, 0x34, 0x12]);
        assert.equal(buf.readUIntLE(0, 5), 0x1234567890);

        buf.fill(0xFF);
        buf.writeUIntBE(0x1234567890, 0, 5);
        assert.deepEqual(Array.from(buf), [0x12, 0x34, 0x56, 0x78, 0x90]);
        assert.equal(buf.readUIntBE(0, 5), 0x1234567890);

        buf.fill(0xFF);
        buf.writeIntLE(0x1234567890, 0, 5);
        assert.deepEqual(Array.from(buf), [0x90, 0x78, 0x56, 0x34, 0x12]);
        assert.equal(buf.readIntLE(0, 5), 0x1234567890);

        buf.fill(0xFF);
        buf.writeIntBE(0x1234567890, 0, 5);
        assert.deepEqual(Array.from(buf), [0x12, 0x34, 0x56, 0x78, 0x90]);
        assert.equal(buf.readIntBE(0, 5), 0x1234567890);

        buf.fill(0xFF);
        buf.writeIntLE(-0x1234567890, 0, 5);
        assert.deepEqual(Array.from(buf), [0x70, 0x87, 0xa9, 0xcb, 0xed]);
        assert.equal(buf.readIntLE(0, 5), -0x1234567890);

        buf.fill(0xFF);
        buf.writeIntBE(-0x1234567890, 0, 5);
        assert.deepEqual(Array.from(buf), [0xed, 0xcb, 0xa9, 0x87, 0x70]);
        assert.equal(buf.readIntBE(0, 5), -0x1234567890);

        buf.fill(0xFF);
        buf.writeIntLE(-0x0012000000, 0, 5);
        assert.deepEqual(Array.from(buf), [0x00, 0x00, 0x00, 0xee, 0xff]);
        assert.equal(buf.readIntLE(0, 5), -0x0012000000);

        buf.fill(0xFF);
        buf.writeIntBE(-0x0012000000, 0, 5);
        assert.deepEqual(Array.from(buf), [0xff, 0xee, 0x00, 0x00, 0x00]);
        assert.equal(buf.readIntBE(0, 5), -0x0012000000);
    });

    it('forEach', () => {
        var buf = Buffer.from([1, 2, 3, 4, 5]);
        var arr = [];

        buf.forEach(function (v, i, a) {
            assert.equal(this, arr);
            assert.equal(a, buf);
            assert.equal(v, buf[i]);
            arr.push(v);
        }, arr);

        assert.deepEqual(arr, [1, 2, 3, 4, 5]);

        assert.throws(() => {
            buf.forEach(function (v, i, a) {
                arr.push(v);
                if (i == 3)
                    throw new Error('test');
            });
        });

        assert.deepEqual(arr, [1, 2, 3, 4, 5, 1, 2, 3, 4]);
    });

    it('indexOf', () => {
        var buf = Buffer.from([0x31, 0x32, 0x33, 0x34, 0x00]);
        assert.equal(buf.indexOf(0x33), 2);
        assert.equal(buf.indexOf(0x00), 4);

        buf = Buffer.from("cacdbfcde");

        assert.equal(buf.indexOf("cd"), 2);
        assert.equal(buf.indexOf(Buffer.from("de")), 7);

        buf = Buffer.from('123456');
        assert.equal(buf.indexOf(0x33), 2);

        buf = Buffer.from([0x31, 0x32, 0x33, 0x34, 0x05, 0x36]);
        assert.equal(buf.indexOf(0x38, 3), -1);

        buf = Buffer.from([0x31, 0x32, 0x33, 0x34, 0x05, 0x00, 0x36, 0x37]);
        assert.equal(buf.indexOf(Buffer.from([0x00, 0x36])), 5);

    });

    it('indexOf - comprehensive tests', () => {
        // Test not found cases
        var buf = Buffer.from('hello world');
        assert.equal(buf.indexOf('x'), -1);
        assert.equal(buf.indexOf(Buffer.from('xyz')), -1);
        assert.equal(buf.indexOf(99), -1); // 'c' ascii

        // Test empty patterns
        assert.equal(buf.indexOf(''), 0);
        assert.equal(buf.indexOf(Buffer.alloc(0)), 0);

        // Test pattern at start and end
        buf = Buffer.from('abcdef');
        assert.equal(buf.indexOf('abc'), 0);
        assert.equal(buf.indexOf('def'), 3);
        assert.equal(buf.indexOf('a'), 0);
        assert.equal(buf.indexOf('f'), 5);

        // Test overlapping matches (should return first)
        buf = Buffer.from('aaaa');
        assert.equal(buf.indexOf('aa'), 0);
        buf = Buffer.from('abababab');
        assert.equal(buf.indexOf('abab'), 0);

        // Test with offsets
        buf = Buffer.from('hello hello hello');
        assert.equal(buf.indexOf('hello'), 0);
        assert.equal(buf.indexOf('hello', 1), 6);
        assert.equal(buf.indexOf('hello', 7), 12);
        assert.equal(buf.indexOf('hello', 13), -1);

        // Test negative offsets (Node.js treats as buf.length + offset, clamped to 0)
        assert.equal(buf.indexOf('hello', -5), 12);  // 17 + (-5) = 12
        assert.equal(buf.indexOf('hello', -100), 0); // Math.max(0, 17 + (-100)) = 0

        // Test large offsets
        assert.equal(buf.indexOf('hello', 1000), -1);
        assert.equal(buf.indexOf('hello', buf.length), -1);

        // Test with numbers (byte values)
        buf = Buffer.from([1, 2, 3, 4, 5, 2, 7]);
        assert.equal(buf.indexOf(2), 1);
        assert.equal(buf.indexOf(2, 2), 5);
        assert.equal(buf.indexOf(256), -1); // Should wrap to 0
        assert.equal(buf.indexOf(-1), -1); // Should wrap to 255

        // Test multi-byte patterns
        buf = Buffer.from('abcdefabcdef');
        assert.equal(buf.indexOf('cde'), 2);
        assert.equal(buf.indexOf('cde', 3), 8);
        assert.equal(buf.indexOf(Buffer.from('abc')), 0);
        assert.equal(buf.indexOf(Buffer.from('abc'), 1), 6);

        // Test with different encodings
        buf = Buffer.from('café', 'utf8');
        assert.equal(buf.indexOf('é'), 3); // é is 2 bytes in UTF-8
        assert.equal(buf.indexOf('café'), 0);

        // Test binary data
        buf = Buffer.from([0x00, 0x01, 0x02, 0x00, 0x01, 0x02]);
        assert.equal(buf.indexOf(0x00), 0);
        assert.equal(buf.indexOf(0x00, 1), 3);
        assert.equal(buf.indexOf(Buffer.from([0x01, 0x02])), 1);
        assert.equal(buf.indexOf(Buffer.from([0x01, 0x02]), 2), 4);

        // Test edge case: pattern longer than buffer
        buf = Buffer.from('ab');
        assert.equal(buf.indexOf('abc'), -1);
        assert.equal(buf.indexOf(Buffer.from('abcd')), -1);

        // Test same length pattern
        assert.equal(buf.indexOf('ab'), 0);
        assert.equal(buf.indexOf('xy'), -1);
    });

    it('lastIndexOf', () => {
        var buf = Buffer.from([0x31, 0x32, 0x33, 0x34, 0x00]);
        assert.equal(buf.lastIndexOf(0x33), 2);
        assert.equal(buf.lastIndexOf(0x00), 4);

        buf = Buffer.from("cacdbfcde");
        assert.equal(buf.lastIndexOf("cd"), 6);
        assert.equal(buf.lastIndexOf(Buffer.from("de")), 7);

        buf = Buffer.from('123456');
        assert.equal(buf.lastIndexOf(0x33), 2);

        buf = Buffer.from([0x31, 0x32, 0x33, 0x34, 0x05, 0x36]);
        assert.equal(buf.lastIndexOf(0x38, 3), -1);

        buf = Buffer.from([0x31, 0x32, 0x33, 0x34, 0x05, 0x00, 0x36, 0x37]);
        assert.equal(buf.lastIndexOf(Buffer.from([0x00, 0x36])), 5);
    });

    it('lastIndexOf - comprehensive tests', () => {
        // Test not found cases
        var buf = Buffer.from('hello world');
        assert.equal(buf.lastIndexOf('x'), -1);
        assert.equal(buf.lastIndexOf(Buffer.from('xyz')), -1);
        assert.equal(buf.lastIndexOf(99), -1); // 'c' ascii

        // Test empty patterns
        assert.equal(buf.lastIndexOf(''), buf.length);
        assert.equal(buf.lastIndexOf(Buffer.alloc(0)), buf.length);

        // Test pattern at start and end
        buf = Buffer.from('abcdef');
        assert.equal(buf.lastIndexOf('abc'), 0);
        assert.equal(buf.lastIndexOf('def'), 3);
        assert.equal(buf.lastIndexOf('a'), 0);
        assert.equal(buf.lastIndexOf('f'), 5);

        // Test overlapping matches (should return last)
        buf = Buffer.from('aaaa');
        assert.equal(buf.lastIndexOf('aa'), 2);
        buf = Buffer.from('abababab');
        assert.equal(buf.lastIndexOf('abab'), 4);

        // Test multiple occurrences
        buf = Buffer.from('hello hello hello');
        assert.equal(buf.lastIndexOf('hello'), 12);
        assert.equal(buf.lastIndexOf('hello', 11), 6);
        assert.equal(buf.lastIndexOf('hello', 5), 0);
        assert.equal(buf.lastIndexOf('hello', -1), 12); // -1 means from end

        // Test with offsets
        buf = Buffer.from('abcabcabc');
        assert.equal(buf.lastIndexOf('abc'), 6);
        assert.equal(buf.lastIndexOf('abc', 5), 3);
        assert.equal(buf.lastIndexOf('abc', 2), 0);
        assert.equal(buf.lastIndexOf('abc', 8), 6);

        // Test negative offsets (Node.js behavior: -1 means from end)
        buf = Buffer.from('hello hello hello');
        assert.equal(buf.lastIndexOf('hello', -1), 12);
        assert.equal(buf.lastIndexOf('hello', -100), -1); // Very negative should find nothing

        // Test large offsets
        assert.equal(buf.lastIndexOf('hello', 1000), 12);

        // Test with numbers (byte values)
        buf = Buffer.from([1, 2, 3, 4, 5, 2, 7, 2]);
        assert.equal(buf.lastIndexOf(2), 7);
        assert.equal(buf.lastIndexOf(2, 6), 5);
        assert.equal(buf.lastIndexOf(2, 4), 1);
        assert.equal(buf.lastIndexOf(256), -1); // Should wrap to 0
        assert.equal(buf.lastIndexOf(-1), -1); // Should wrap to 255

        // Test multi-byte patterns
        buf = Buffer.from('abcdefabcdef');
        assert.equal(buf.lastIndexOf('cde'), 8);
        assert.equal(buf.lastIndexOf('cde', 7), 2);
        assert.equal(buf.lastIndexOf(Buffer.from('abc')), 6);
        assert.equal(buf.lastIndexOf(Buffer.from('abc'), 5), 0);

        // Test with different encodings
        buf = Buffer.from('café café', 'utf8');
        assert.equal(buf.lastIndexOf('é'), 9); // Last é position (é is at byte 9-10)
        assert.equal(buf.lastIndexOf('café'), 6); // Last café position

        // Test binary data
        buf = Buffer.from([0x00, 0x01, 0x02, 0x00, 0x01, 0x02, 0x00]);
        assert.equal(buf.lastIndexOf(0x00), 6);
        assert.equal(buf.lastIndexOf(0x00, 5), 3);
        assert.equal(buf.lastIndexOf(0x00, 2), 0);
        assert.equal(buf.lastIndexOf(Buffer.from([0x01, 0x02])), 4);
        assert.equal(buf.lastIndexOf(Buffer.from([0x01, 0x02]), 3), 1);

        // Test edge case: pattern longer than buffer
        buf = Buffer.from('ab');
        assert.equal(buf.lastIndexOf('abc'), -1);
        assert.equal(buf.lastIndexOf(Buffer.from('abcd')), -1);

        // Test same length pattern
        assert.equal(buf.lastIndexOf('ab'), 0);
        assert.equal(buf.lastIndexOf('xy'), -1);

        // Test pattern at exact boundaries
        buf = Buffer.from('abcdef');
        assert.equal(buf.lastIndexOf('a', 0), 0);
        assert.equal(buf.lastIndexOf('f', 5), 5);
        assert.equal(buf.lastIndexOf('f', 4), -1);

        // Test with special characters and Unicode
        buf = Buffer.from('αβγαβγ', 'utf8');
        assert.equal(buf.lastIndexOf('α'), 6); // Last α position in UTF-8 bytes
        assert.equal(buf.lastIndexOf('γ'), 10); // Last γ position in UTF-8 bytes
    });

    it('indexOf/lastIndexOf - edge cases and compatibility', () => {
        // Test with empty buffer
        var emptyBuf = Buffer.alloc(0);
        assert.equal(emptyBuf.indexOf(''), 0);
        assert.equal(emptyBuf.indexOf('a'), -1);
        assert.equal(emptyBuf.indexOf(Buffer.from('a')), -1);
        assert.equal(emptyBuf.lastIndexOf(''), 0);
        assert.equal(emptyBuf.lastIndexOf('a'), -1);

        // Test type coercion for numbers
        var buf = Buffer.from([0, 1, 2, 255, 256, 257]);
        assert.equal(buf.indexOf(256), buf.indexOf(0)); // 256 % 256 = 0
        assert.equal(buf.indexOf(257), buf.indexOf(1)); // 257 % 256 = 1
        assert.equal(buf.indexOf(-1), buf.indexOf(255)); // -1 % 256 = 255
        assert.equal(buf.lastIndexOf(256), buf.lastIndexOf(0));
        assert.equal(buf.lastIndexOf(257), buf.lastIndexOf(1));
        assert.equal(buf.lastIndexOf(-1), buf.lastIndexOf(255));

        // Test with very large numbers
        assert.equal(buf.indexOf(0x100), buf.indexOf(0));
        assert.equal(buf.indexOf(0x101), buf.indexOf(1));

        // Test string vs Buffer vs number consistency
        buf = Buffer.from('hello');
        var hChar = 'h'.charCodeAt(0); // 104
        assert.equal(buf.indexOf('h'), 0);
        assert.equal(buf.indexOf(hChar), 0);
        assert.equal(buf.indexOf(Buffer.from('h')), 0);
        assert.equal(buf.lastIndexOf('h'), 0);
        assert.equal(buf.lastIndexOf(hChar), 0);
        assert.equal(buf.lastIndexOf(Buffer.from('h')), 0);

        // Test case sensitivity
        buf = Buffer.from('Hello');
        assert.equal(buf.indexOf('h'), -1);
        assert.equal(buf.indexOf('H'), 0);
        assert.equal(buf.lastIndexOf('h'), -1);
        assert.equal(buf.lastIndexOf('H'), 0);

        // Test with null bytes
        buf = Buffer.from([0x48, 0x00, 0x65, 0x00, 0x6c, 0x6c, 0x6f]); // H\0e\0llo
        assert.equal(buf.indexOf(0x00), 1);
        assert.equal(buf.indexOf(0x00, 2), 3);
        assert.equal(buf.lastIndexOf(0x00), 3);
        assert.equal(buf.lastIndexOf(0x00, 2), 1);

        // Test pattern that appears multiple times with different offsets
        buf = Buffer.from('ababcabab');
        assert.equal(buf.indexOf('ab'), 0);
        assert.equal(buf.indexOf('ab', 1), 2);
        assert.equal(buf.indexOf('ab', 3), 5);
        assert.equal(buf.indexOf('ab', 6), 7);
        assert.equal(buf.lastIndexOf('ab'), 7);
        assert.equal(buf.lastIndexOf('ab', 6), 5);
        assert.equal(buf.lastIndexOf('ab', 4), 2);
        assert.equal(buf.lastIndexOf('ab', 1), 0);

        // Test boundary conditions for offsets
        buf = Buffer.from('abcdef');
        assert.equal(buf.indexOf('a', buf.length), -1);
        assert.equal(buf.indexOf('a', buf.length + 1), -1);
        assert.equal(buf.lastIndexOf('f', 0), -1);
        assert.equal(buf.lastIndexOf('a', 0), 0);

        // Test fractional offsets (should be truncated)
        assert.equal(buf.indexOf('b', 0.9), 1);
        assert.equal(buf.indexOf('c', 1.9), 2);
        assert.equal(buf.lastIndexOf('e', 4.9), 4);

        // Test offset edge cases
        assert.equal(buf.indexOf('a', 0), 0);
        assert.equal(buf.lastIndexOf('a', buf.length - 1), 0); // 'a' is at position 0 in 'abcdef'
    });

    it('indexOf/lastIndexOf - encoding tests', () => {
        // Test UTF-8 multibyte characters
        var buf = Buffer.from('🚀🌟🚀', 'utf8');
        var rocketBytes = Buffer.from('🚀', 'utf8');
        var starBytes = Buffer.from('🌟', 'utf8');

        assert.equal(buf.indexOf(rocketBytes), 0);
        assert.equal(buf.lastIndexOf(rocketBytes), 8); // Second 🚀
        assert.equal(buf.indexOf(starBytes), 4);
        assert.equal(buf.lastIndexOf(starBytes), 4);

        // Test ASCII vs UTF-8
        buf = Buffer.from('café', 'utf8'); // é is 2 bytes: 0xc3, 0xa9
        assert.equal(buf.length, 5); // c, a, f, 0xc3, 0xa9
        assert.equal(buf.indexOf('é'), 3); // Should find at byte position 3
        assert.equal(buf.indexOf(0xc3), 3); // First byte of é
        assert.equal(buf.indexOf(0xa9), 4); // Second byte of é

        // Test Latin-1 encoding
        buf = Buffer.from('café', 'latin1'); // é is 1 byte: 0xe9
        assert.equal(buf.length, 4);
        assert.equal(buf.indexOf('é'), -1); // String search for 'é' fails in latin1 buffer
        assert.equal(buf.indexOf(0xe9), 3);

        // Test hex strings
        buf = Buffer.from('48656c6c6f', 'hex'); // "Hello"
        assert.equal(buf.indexOf('Hello'), 0);
        assert.equal(buf.indexOf(0x48), 0); // 'H'
        assert.equal(buf.indexOf(0x6f), 4); // 'o'
    });

    it('indexOf/lastIndexOf - performance edge cases', () => {
        // Test with large buffer and small pattern
        var large = Buffer.alloc(10000, 'a');
        large[5000] = 98; // 'b'
        large[7500] = 98; // 'b'

        assert.equal(large.indexOf('b'), 5000);
        assert.equal(large.lastIndexOf('b'), 7500);
        assert.equal(large.indexOf('b', 5001), 7500);
        assert.equal(large.lastIndexOf('b', 7499), 5000);

        // Test with repeating pattern
        var pattern = Buffer.from('abcd');
        var repeated = Buffer.concat(Array(1000).fill(pattern));
        assert.equal(repeated.indexOf('abcd'), 0);
        assert.equal(repeated.lastIndexOf('abcd'), repeated.length - 4);
        assert.equal(repeated.indexOf('bcd'), 1);
        assert.equal(repeated.lastIndexOf('bcd'), repeated.length - 3);
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
            var a = Buffer.from(f.a, 'hex');
            assert.equal(a.reverse().toString('hex'), f.expected);
        })
    });

    it('join', () => {
        var a = Buffer.from([192, 168, 0, 1]);
        assert.equal(a.join('.'), '192.168.0.1');
        assert.equal(a.join(), '192,168,0,1');
    });

    it('indexed setter', () => {
        const b = Buffer.from([1, 2]);
        b[0] = -1;
        assert.deepEqual(Buffer.from([255, 2]), b);
        b[0] = -255;
        assert.deepEqual(Buffer.from([1, 2]), b);
        b[0] = -256;
        assert.deepEqual(Buffer.from([0, 2]), b);
        b[0] = -257;
        assert.deepEqual(Buffer.from([255, 2]), b);

        b[0] = 255;
        assert.deepEqual(Buffer.from([255, 2]), b);
        b[0] = 256;
        assert.deepEqual(Buffer.from([0, 2]), b);
        b[0] = 257;
        assert.deepEqual(Buffer.from([1, 2]), b);

        b[3] = -1;
        assert.deepEqual(Buffer.from([1, 2]), b);
    });

    it('indexed getter', () => {
        const b = Buffer.from([1, 2]);
        assert.equal(b[3], undefined);
        assert.equal(b[-1], undefined);
        assert.equal(b[0], 1);
        assert.equal(b[1], 2);
    });

    // fibjs specific features
    if (process.versions.fibjs) {
        describe('fibjs specific features', () => {
            it("buffer module", () => {
                var buf = require("buffer");
                assert.equal(typeof buf.Buffer, "function");
                assert.equal(buf.Buffer, Buffer);
            });

            it('sandbox', () => {
                var sbox = new vm.SandBox({
                    Buffer: Buffer
                });
                var buf = sbox.require("buffer", __dirname);
                assert.equal(typeof buf.Buffer, "function");
                assert.equal(buf.Buffer, Buffer);
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

            it('new Buffer(Uint8Array)', () => {
                var arr = new Uint8Array(2);
                arr[0] = 50;
                arr[1] = 40;

                var buf = new Buffer(arr);

                assert.equal(buf.length, 2);
                assert.equal(buf.toString('hex'), "3228");

                var arr = new Uint8Array([0x10, 0x20, 0x30]);
                var arr1 = new Uint8Array(arr.buffer, 1, 2);
                var buf = new Buffer(arr1);
                assert.equal(buf.length, 2);
                assert.equal(buf.toString('hex'), "2030");
            });

            it('new Buffer(ArrayBuffer)', () => {
                var arr = new Uint16Array(2);
                arr[0] = 5000;
                arr[1] = 4000;

                var buf = new Buffer(arr.buffer);

                assert.equal(buf.length, 4);
                assert.equal(buf.toString('hex'), is_big_endian ? "13880fa0" : "8813a00f");
            });

            it('new Buffer(DataView)', () => {
                var arr = new Uint8Array([0x10, 0x20, 0x30]);
                var buf = new Buffer(new DataView(arr.buffer));

                assert.equal(buf.length, 3);
                assert.equal(buf.toString('hex'), "102030");
            });

            it('new Buffer(Buffer)', () => {
                var buf = new Buffer(new Buffer("abcd"));
                assert.equal(buf.length, 4);
                assert.equal(buf.toString(), "abcd");
                var buf = new Buffer({});
            });

            it('new Buffer(date)', () => {
                var data = Buffer.from(new Date("2016-03-09T07:58:57.303Z"));
                assert.equal(new Date(data.toString()).toISOString(), "2016-03-09T07:58:57.000Z");
            });

            it('Buffer.from encoding with extended formats', () => {
                // base32 support
                assert.equal(Buffer.from("gezdgna=", "base32").toString(), "1234");

                // base58 support  
                assert.equal(Buffer.from("2FwFnT", "base58").toString(), "1234");

                // base64url support
                assert.equal(Buffer.from("6am-77yG556_", "base64url").toString(), "驾＆瞿");
            });

            it('Buffer.isEncoding extended', () => {
                assert.equal(Buffer.isEncoding('base32'), true);
                assert.equal(Buffer.isEncoding('base58'), true);
                assert.equal(Buffer.isEncoding('base64url'), true);
                assert.equal(Buffer.isEncoding('EUC-JP'), true);
            });

            it('toString with extended formats', () => {
                var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
                assert.equal(buf.toString("base32"), "gezdgna");
                assert.equal(buf.toString("base58"), "2FwFnT");
                assert.equal(buf.toString("base64url"), "MTIzNA");

                assert.equal(buf.toString("base32", 2), "gm2a");
                assert.equal(buf.toString("base58", 2), "4u1");
            });

            it('charset', () => {
                var buf = Buffer.from("哈哈", "gbk");
                assert.equal(buf.toString("hex"), "b9feb9fe");

                buf = Buffer.from("哈哈", "gb2312");
                assert.equal(buf.toString("hex"), "b9feb9fe");
            });

            it("FIX: fibjs will crash when the offset of Buffer.read is negative", () => {
                var buf = Buffer.from([0x23, 0x42]);
                assert.throws(() => {
                    buf.readUInt8(-1);
                });
            });

            it("FIX: fibjs will crash when the offset of Buffer.write is negative", () => {
                var buf = Buffer.alloc(10);
                assert.throws(() => {
                    buf.writeUInt8(0x23, -1);
                });
            });

            it("FIX: passing a large offset to Buffer.readUIntLE will cause fibjs to crash", () => {
                var buf = Buffer.allocUnsafe(3);
                assert.throws(() => {
                    buf.readUIntLE(0, 9);
                });
            });

            it("FIX: passing a large offset to Buffer.writeUInt32BE will cause fibjs to crash", () => {
                var buf = Buffer.allocUnsafe(3);
                assert.throws(() => {
                    buf.writeUInt32BE(0, 1);
                });
            });
        });
    }

    describe('encoding shortcut methods', () => {
        it('utf8Slice / utf8Write', () => {
            const buf = Buffer.alloc(16);
            const written = buf.utf8Write('hello', 0, 5);
            assert.strictEqual(written, 5);
            assert.strictEqual(buf.utf8Slice(0, 5), 'hello');
        });

        it('utf8Write with multi-byte characters', () => {
            const buf = Buffer.alloc(16);
            const written = buf.utf8Write('你好', 0, 16);
            assert.strictEqual(written, 6);
            assert.strictEqual(buf.utf8Slice(0, 6), '你好');
        });

        it('latin1Slice / latin1Write', () => {
            const buf = Buffer.alloc(8);
            const written = buf.latin1Write('café', 0, 8);
            assert.ok(written > 0);
            assert.strictEqual(buf.latin1Slice(0, written), 'café');
        });

        it('asciiSlice / asciiWrite', () => {
            const buf = Buffer.alloc(8);
            const written = buf.asciiWrite('hello', 0, 5);
            assert.strictEqual(written, 5);
            assert.strictEqual(buf.asciiSlice(0, 5), 'hello');
        });

        it('hexSlice / hexWrite', () => {
            const buf = Buffer.from([0x68, 0x65, 0x6c, 0x6c, 0x6f]);
            assert.strictEqual(buf.hexSlice(0, 5), '68656c6c6f');

            const buf2 = Buffer.alloc(4);
            const written = buf2.hexWrite('deadbeef', 0, 4);
            assert.strictEqual(written, 4);
            assert.strictEqual(buf2.hexSlice(0, 4), 'deadbeef');
        });

        it('base64Slice / base64Write', () => {
            const buf = Buffer.from('hello');
            assert.strictEqual(buf.base64Slice(0, 5), Buffer.from('hello').toString('base64'));

            const buf2 = Buffer.alloc(5);
            const written = buf2.base64Write('aGVsbG8=', 0, 5);
            assert.strictEqual(written, 5);
            assert.strictEqual(buf2.toString('utf8'), 'hello');
        });

        it('ucs2Slice / ucs2Write', () => {
            const buf = Buffer.alloc(10);
            const written = buf.ucs2Write('hi', 0, 10);
            assert.strictEqual(written, 4);
            assert.strictEqual(buf.ucs2Slice(0, 4), 'hi');
        });

        it('slice parameters', () => {
            const buf = Buffer.from('abcdefgh');
            assert.strictEqual(buf.utf8Slice(2, 5), 'cde');
            assert.strictEqual(buf.hexSlice(0, 3), '616263');
        });
    });
});
