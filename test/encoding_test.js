var test = require("test");
test.setup();

var encoding = require('encoding');
var json = require('json');
var bson = require('bson');
var msgpack = require('msgpack');
var base64 = require('base64');
var hex = require('hex');
var iconv = require('iconv');
var base64vlq = require('base64vlq');
var util = require('util');

describe('encoding', () => {
    it('base64', () => {
        var quote = 'Man is distinguished, not only by his reason, but by this ' + 'singular passion from other animals, which is a lust ' + 'of the mind, that by a perseverance of delight in the continued ' + 'and indefatigable generation of knowledge, exceeds the short ' + 'vehemence of any carnal pleasure.';
        var expected = 'TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24s' + 'IGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltY' + 'WxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZX' + 'JzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmR' + 'lZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo' + 'ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=';

        var b = base64.decode(expected);
        assert.equal(b.toString(), quote);

        var s = base64.encode(b);
        assert.equal(s, expected);

        // check that the base64 decoder ignores whitespace
        var expectedWhite = expected.slice(0, 60) + ' \n' + expected.slice(60, 120) + ' \n' + expected.slice(120, 180) + ' \n' +
            expected.slice(180, 240) + ' \n' + expected.slice(240, 300) + '\n' + expected.slice(300, 360) + '\n';
        b = base64.decode(expectedWhite);
        assert.equal(quote, b.toString());

        // check that the base64 decoder ignores illegal chars
        var expectedIllegal = expected.slice(0, 60) + ' \x80' + expected.slice(60, 120) + ' \xff' + expected.slice(120, 180) + ' \x01' + // '
            // \x00'
            expected.slice(180, 240) + ' \x98' + expected.slice(240, 300) + '\x03' + expected.slice(300, 360);
        b = base64.decode(expectedIllegal);
        assert.equal(quote, b.toString());

        assert.equal(base64.decode('').toString(), '');
        assert.equal(base64.decode('K').toString(), '');

        // multiple-of-4 with padding
        assert.equal(base64.decode('Kg==').toString(), '*');
        assert.equal(base64.decode('Kio=').toString(), '**');
        assert.equal(base64.decode('Kioq').toString(), '***');
        assert.equal(base64.decode('KioqKg==').toString(), '****');
        assert.equal(base64.decode('KioqKio=').toString(), '*****');
        assert.equal(base64.decode('KioqKioq').toString(), '******');
        assert.equal(base64.decode('KioqKioqKg==').toString(), '*******');
        assert.equal(base64.decode('KioqKioqKio=').toString(), '********');
        assert.equal(base64.decode('KioqKioqKioq').toString(), '*********');
        assert.equal(base64.decode('KioqKioqKioqKg==').toString(), '**********');
        assert.equal(base64.decode('KioqKioqKioqKio=').toString(), '***********');
        assert.equal(base64.decode('KioqKioqKioqKioq').toString(), '************');
        assert.equal(base64.decode('KioqKioqKioqKioqKg==').toString(), '*************');
        assert.equal(base64.decode('KioqKioqKioqKioqKio=').toString(), '**************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioq').toString(), '***************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKg==').toString(), '****************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKio=').toString(), '*****************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKioq').toString(), '******************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKioqKg==').toString(), '*******************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKioqKio=').toString(), '********************');

        // no padding, not a multiple of 4
        assert.equal(base64.decode('Kg').toString(), '*');
        assert.equal(base64.decode('Kio').toString(), '**');
        assert.equal(base64.decode('KioqKg').toString(), '****');
        assert.equal(base64.decode('KioqKio').toString(), '*****');
        assert.equal(base64.decode('KioqKioqKg').toString(), '*******');
        assert.equal(base64.decode('KioqKioqKio').toString(), '********');
        assert.equal(base64.decode('KioqKioqKioqKg').toString(), '**********');
        assert.equal(base64.decode('KioqKioqKioqKio').toString(), '***********');
        assert.equal(base64.decode('KioqKioqKioqKioqKg').toString(), '*************');
        assert.equal(base64.decode('KioqKioqKioqKioqKio').toString(), '**************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKg').toString(), '****************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKio').toString(), '*****************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKioqKg').toString(), '*******************');
        assert.equal(base64.decode('KioqKioqKioqKioqKioqKioqKio').toString(), '********************');

        // handle padding graciously, multiple-of-4 or not
        assert.equal(base64.decode('72INjkR5fchcxk9+VgdGPFJDxUBFR5/rMFsghgxADiw==').length, 32);
        assert.equal(base64.decode('72INjkR5fchcxk9+VgdGPFJDxUBFR5/rMFsghgxADiw=').length, 32);
        assert.equal(base64.decode('72INjkR5fchcxk9+VgdGPFJDxUBFR5/rMFsghgxADiw').length, 32);
        assert.equal(base64.decode('w69jACy6BgZmaFvv96HG6MYksWytuZu3T1FvGnulPg==').length, 31);
        assert.equal(base64.decode('w69jACy6BgZmaFvv96HG6MYksWytuZu3T1FvGnulPg=').length, 31);
        assert.equal(base64.decode('w69jACy6BgZmaFvv96HG6MYksWytuZu3T1FvGnulPg').length, 31);

        var dot = base64.decode('//4uAA==');
        assert.equal(dot[0], 0xff);
        assert.equal(dot[1], 0xfe);
        assert.equal(dot[2], 0x2e);
        assert.equal(dot[3], 0x00);
        assert.equal(base64.encode(dot), '//4uAA==');
    });

    it("base64url", () => {
        assert.equal(base64.encode(base64.decode('//4+AA=='), true), '__4-AA==');
        assert.deepEqual(base64.decode('//4+AA=='), base64.decode('__4-AA=='));
    });

    it('base64vlq', () => {
        var tests = {
            'AAAA': [0, 0, 0, 0],
            'AAgBC': [0, 0, 16, 1],
            '6rk2B': [886973],
            '6rB': [701],
            'mC3jxHAkhSliBqwG': [35, -123451, 0, 9234, -546, 3333]
        };

        for (var k in tests) {
            assert.equal(k, base64vlq.encode(tests[k]));
            assert.deepEqual(tests[k], base64vlq.decode(k));
        }
    });

    it('hex', () => {
        var hexb = new Buffer();
        hexb.resize(256);
        for (var i = 0; i < 256; i++) {
            hexb[i] = i;
        }
        var hexStr = hex.encode(hexb);
        assert.equal(hexStr, '000102030405060708090a0b0c0d0e0f' + '101112131415161718191a1b1c1d1e1f' + '202122232425262728292a2b2c2d2e2f' + '303132333435363738393a3b3c3d3e3f' + '404142434445464748494a4b4c4d4e4f' + '505152535455565758595a5b5c5d5e5f' + '606162636465666768696a6b6c6d6e6f' + '707172737475767778797a7b7c7d7e7f' + '808182838485868788898a8b8c8d8e8f' + '909192939495969798999a9b9c9d9e9f' + 'a0a1a2a3a4a5a6a7a8a9aaabacadaeaf' + 'b0b1b2b3b4b5b6b7b8b9babbbcbdbebf' + 'c0c1c2c3c4c5c6c7c8c9cacbcccdcecf' + 'd0d1d2d3d4d5d6d7d8d9dadbdcdddedf' + 'e0e1e2e3e4e5e6e7e8e9eaebecedeeef' + 'f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff');

        var hexb2 = hex.decode("    " + hexStr);

        assert.equal(hexb2.length, hexb.length);

        for (var i = 0; i < 256; i++) {
            assert.equal(hexb2[i], hexb[i]);
        }
    });

    it('iconv ucs2', () => {
        for (var i = 0; i < 65536; i++) {
            var s = String.fromCharCode(i);
            var buf = iconv.encode('utf16le', s);
            var n = buf.readUInt16LE();
            assert.equal(i, n);
            assert.equal(iconv.decode('utf16le', buf), s);
        }

        assert.equal(new Buffer([0xc8]).toString(), '\ufffd');
        assert.equal(Buffer.from('3DD84DDC', 'hex').toString('utf16le'), '👍');
    });

    var datas = [
        [
            0x7f,
            "7f000000",
            "7f000000"
        ],
        [
            0x80,
            "80000000",
            "80000000"
        ],
        [
            0x7ff,
            "ff070000",
            "ff070000"
        ],
        [
            0x800,
            "00080000",
            "00080000"
        ],
        [
            0xffff,
            "ffff0000",
            "ffff0000"
        ],
        [
            0x10000,
            "00000100",
            "00000100"
        ],
        [
            0x10ffff,
            "ffff1000",
            "ffff1000"
        ],
        [
            0x110000,
            "00001100",
            "00dc000000dc0000"
        ],
        [
            0x1fffff,
            "ffff1f00",
            "bfdf0000ffdf0000"
        ],
        [
            0x200000,
            "00002000",
            "c0df000000dc0000"
        ],
        [
            0x3ffffff,
            "ffffff03",
            "bfff0000ffdf0000"
        ],
        [
            0x4000000,
            "00000004",
            "c0ff000000dc0000"
        ]
    ];

    it('iconv ucs2 multi', () => {
        datas.forEach(d => {
            var buf = new Buffer(4);
            buf.writeUInt32LE(d[0]);
            var s = iconv.decode('utf32le', buf);
            var buf2 = new Buffer(s.length * 2);
            buf2.writeInt16LE(s.charCodeAt(0));
            if (s.length > 1)
                buf2.writeInt16LE(s.charCodeAt(1), 2);
            assert.equal(iconv.decode('utf16le', buf2), s);
        });
    });

    it('iconv ucs4', () => {
        datas.forEach(d => {
            var buf = new Buffer(4);
            buf.writeUInt32LE(d[0]);
            var s = iconv.decode('utf32le', buf);
            var buf1 = iconv.encode('utf32le', s);
            assert.deepEqual(buf.hex(), d[1]);
            assert.deepEqual(buf1.hex(), d[2]);
        });
    });

    it('uri', () => {
        var u = '中文测试';
        var u1 = escape(u);
        assert.equal(encoding.decodeURI(u1), u);

        u1 = u1.replace(/%/g, '\\');
        assert.equal(encoding.decodeURI(u1), u);

        for (var i = 32; i < 128; i++)
            u += String.fromCharCode(i);

        assert.equal(encodeURI(u), encoding.encodeURI(u));
        assert.equal(encodeURIComponent(u), encoding.encodeURIComponent(u));
        assert.equal(encoding.decodeURI(encodeURI(u)), u);
        assert.equal(encoding.decodeURI(encodeURIComponent(u)), u);
    });

    it('json', () => {
        assert.equal(json.encode({
            a: 100,
            b: 200
        }), '{"a":100,"b":200}');

        assert.equal(json.encode(json.decode('{"a":100,"b":200}')),
            '{"a":100,"b":200}');
    });

    it('json encode object', () => {
        var buf = new Buffer('test');
        var j = json.encode(buf);
        assert.equal(j, '{"type":"Buffer","data":[116,101,115,116]}');

        var buf1 = json.decode(json.encode({
            "type": "Buffer",
            "data": buf.toArray()
        }));

        assert.deepEqual(buf, buf1);

        var buf1 = json.decode(json.encode({
            "type": "Buffer",
            "data": buf.base64()
        }));

        assert.deepEqual(buf, buf1);

        var o = {
            "type": "Buffer",
            "data": {
                a: 100,
                b: 200
            }
        };
        assert.deepEqual(json.decode(json.encode(o)), o)

        var o = {
            bigint: 10000n
        };

        var j = json.encode(o);
        assert.equal(j, '{"bigint":{"type":"BigInt","data":"10000"}}')

        var o1 = json.decode(j);
        assert.deepEqual(o, o1);

        var o = {
            "type": "BigInt",
            "data": "aaaaa"
        };
        assert.deepEqual(json.decode(json.encode(o)), o)

        var o = {
            "type": "BigInt",
            "data": {
                a: 100,
                b: 200
            }
        };
        assert.deepEqual(json.decode(json.encode(o)), o)
    });

    it('json encode error when circular', () => {
        var a = {};
        a.aa = a;
        assert.throws(() => {
            json.encode(a);
        })
    });

    it('jsstr', () => {
        assert.equal(encoding.jsstr("[\r\n\t\\\'\"]"), "[\\r\\n\\t\\\\\\'\\\"]");
        assert.equal(encoding.jsstr("[abcd汉字]"), "[abcd汉字]");
    });

    it('json jsstr', () => {
        assert.equal(encoding.jsstr("[\r\n\t\\\'\"]", true), "[\\r\\n\\t\\\\'\\\"]");
    });

    describe("msgpack", () => {
        it('should be object', () => {
            assert.isNotNull(msgpack);
            assert.isObject(msgpack);
        });

        it('pack should return a Buffer object', () => {
            var buf = msgpack.encode('abcdef');
            assert.isNotNull(buf);
            assert.ok(util.isBuffer(buf));
        });

        it('test for string equality', () => {
            assert.deepEqual('abcdef', msgpack.decode(msgpack.encode('abcdef')));
        });

        it('test for numeric equality', () => {
            assert.deepEqual(123, msgpack.decode(msgpack.encode(123)));
            assert.isNumber(msgpack.decode(msgpack.encode(123)));
        });

        it('test for null', () => {
            assert.deepEqual(null, msgpack.decode(msgpack.encode(null)));
            assert.isNull(msgpack.decode(msgpack.encode(null)));
        });

        it('test for negative decimal number', () => {
            assert.deepEqual(-1243.111, msgpack.decode(msgpack.encode(-1243.111)));
            assert.isNumber(msgpack.decode(msgpack.encode(-1243.111)));
        });

        it('test for negative number', () => {
            assert.deepEqual(-123, msgpack.decode(msgpack.encode(-123)));
            assert.isNumber(msgpack.decode(msgpack.encode(-123)));
        });

        it('test for true', () => {
            assert.deepEqual(true, msgpack.decode(msgpack.encode(true)));
            assert.isBoolean(msgpack.decode(msgpack.encode(true)));
        });

        it('test for false', () => {
            assert.deepEqual(false, msgpack.decode(msgpack.encode(false)));
            assert.isBoolean(msgpack.decode(msgpack.encode(false)));
        });

        it('test for 2^31 negative', () => {
            assert.deepEqual(
                0 - Math.pow(2, 31) - 1,
                msgpack.decode(msgpack.encode(0 - Math.pow(2, 31) - 1))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(0 - Math.pow(2, 31) - 1)));
        });

        it('test for 2^40 negative', () => {
            assert.deepEqual(
                0 - Math.pow(2, 40) - 1,
                msgpack.decode(msgpack.encode(0 - Math.pow(2, 40) - 1))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(0 - Math.pow(2, 40) - 1)));
        });

        it('test for 2^31', () => {
            assert.deepEqual(
                Math.pow(2, 31) + 1,
                msgpack.decode(msgpack.encode(Math.pow(2, 31) + 1))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(Math.pow(2, 31) + 1)));
        });

        it('test for 2^40', () => {
            assert.deepEqual(
                Math.pow(2, 40) + 1,
                msgpack.decode(msgpack.encode(Math.pow(2, 40) + 1))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(Math.pow(2, 40) + 1)));
        });

        it('test number approaching 2^64', () => {
            assert.deepEqual(
                123456782345245,
                msgpack.decode(msgpack.encode(123456782345245))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(123456782345245)));
        });

        it('test for numeric array', () => {
            assert.deepEqual([1, 2, 3], msgpack.decode(msgpack.encode([1, 2, 3])));
            assert.isArray(msgpack.decode(msgpack.encode([1, 2, 3])));
        });

        it('test for mixed type array', () => {
            assert.deepEqual(
                [1, 'abc', false, null],
                msgpack.decode(msgpack.encode([1, 'abc', false, null]))
            );
            assert.isArray(msgpack.decode(msgpack.encode([1, 2, 3])));
        });

        it('test for object', () => {
            var object = {
                'a': [1, 2, 3],
                'b': 'cdef',
                'c': {
                    'nuts': 'qqq'
                }
            };
            assert.deepEqual(object, msgpack.decode(msgpack.encode(object)));
            assert.isObject(msgpack.decode(msgpack.encode(object)));
        });

        it('test unpacking a buffer', () => {
            var testBuffer = new Buffer([0x00, 0x01, 0x02]);
            assert.deepEqual(testBuffer, msgpack.decode(msgpack.encode(testBuffer)));
        });

        it('test unpacking a Uint8Array', () => {
            var testBuffer = new Buffer([0x00, 0x01, 0x02]);
            var testBuffer1 = Uint8Array.from([0x00, 0x01, 0x02]);
            assert.deepEqual(testBuffer, msgpack.decode(msgpack.encode(testBuffer1)));
        });

        describe("timestamp", () => {
            const TIME = 1556636810389;

            const SPECS = {
                ZERO: new Date(0),
                TIME_BEFORE_EPOCH_NS: new Date(-1),
                TIME_BEFORE_EPOCH_SEC: new Date(-1000),
                TIME_BEFORE_EPOCH_SEC_AND_NS: new Date(-1002),
                TIMESTAMP32: new Date(Math.floor(TIME / 1000) * 1000),
                TIMESTAMP64: new Date(TIME),
                TIMESTAMP64_OVER_INT32: new Date(Date.UTC(2200, 0)),
                TIMESTAMP96_SEC_OVER_UINT32: new Date(0x400000000 * 1000),
                TIMESTAMP96_SEC_OVER_UINT32_WITH_NS: new Date(0x400000000 * 1000 + 2),

                REGRESSION_1: new Date(1556799054803),
            };

            for (const name of Object.keys(SPECS)) {
                const value = SPECS[name];

                it(`encodes and decodes ${name} (${value.toISOString()})`, () => {
                    const encoded = msgpack.encode(value);
                    assert.deepEqual(msgpack.decode(encoded), value);
                });
            }
        });

        it('make sure dates are handled properly', () => {
            var date = new Date();
            var dateObj = {
                d: date
            };
            assert.deepEqual({
                    d: date
                },
                msgpack.decode(msgpack.encode(dateObj))
            );
            assert.isObject(msgpack.decode(msgpack.encode(dateObj)));
        });

        it('test for not catching multiple non-circular references', () => {
            var e = {};
            assert.deepEqual({
                    a: e,
                    b: e
                },
                msgpack.decode(msgpack.encode({
                    a: e,
                    b: e
                }))
            );
            assert.isObject(msgpack.decode(msgpack.encode({
                a: e,
                b: e
            })));
        });

        assert('unpacking a buffer with extra data doesn\'t lose the extra data', () => {
            // Object to test with
            var o = [1, 2, 3];

            // Create two buffers full of packed data, 'b' and 'bb', with the latter
            // containing 3 extra bytes
            var b = msgpack.encode(o);
            var bb = new Buffer(b.length + 3);
            b.copy(bb, 0, 0, b.length);

            // Expect no remaining bytes when unpacking 'b'
            assert.deepEqual(msgpack.decode(b), o);
            assert.deepEqual(msgpack.decode.bytes_remaining, 0);

            // Expect 3 remaining bytes when unpacking 'bb'
            assert.deepEqual(msgpack.decode(bb), o);
            assert.equal(msgpack.decode.bytes_remaining, 3);

        });

        it('circular reference marking algorithm doesn\'t false positive', () => {
            try {
                var d = {};
                for (var i = 0; i < 10; i++) {
                    msgpack.encode(d);
                }
                assert.ok(true, 'all clear');
            } catch (e) {
                assert.ok(false, e.message);
            }
        });

        it('test toJSON compatibility', () => {
            var expect = {
                msg: 'hello world'
            };
            var subject = {
                toJSON: function () {
                    return expect;
                }
            };

            assert.deepEqual(expect, msgpack.decode(msgpack.encode(subject)));
        });

        it('test toJSON compatibility for nested toJSON', () => {
            var expect = {
                msg: 'hello world'
            };
            var subject = {
                toJSON: function () {
                    return [
                        expect,
                        {
                            toJSON: function () {
                                return expect;
                            }
                        }
                    ];
                }
            };
            assert.deepEqual([expect, expect], msgpack.decode(msgpack.encode(subject)));
        });

        it('test toJSON compatibility with prototype', () => {
            var expect = {
                msg: 'hello world'
            };
            var subject = {
                __proto__: {
                    toJSON: function () {
                        return expect;
                    }
                }
            };
            assert.deepEqual(expect, msgpack.decode(msgpack.encode(subject)));
        });
    });

    it('bson', () => {

        for (var i = 0; i < 8; i++) {
            var attr = '';

            if (i & 1)
                attr += 'i';
            if (i & 2)
                attr += 'g';
            if (i & 4)
                attr += 'm';

            var re = new RegExp('pattern: ' + i, attr);
            var o = bson.decode(bson.encode({
                re: re
            }));

            assert.deepEqual(re, o.re);
        }
    });
});

require.main === module && test.run(console.DEBUG);