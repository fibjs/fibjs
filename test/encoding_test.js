var test = require("test");
test.setup();

var encoding = require('encoding');
var json = require('json');
var msgpack = require('msgpack');
var base58 = require('base58');
var base64 = require('base64');
var hex = require('hex');
var multibase = require('multibase');
var iconv = require('iconv');
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
        assert.equal(base64.encode(base64.decode('//4+AA=='), true), '__4-AA');
        assert.deepEqual(base64.decode('//4+AA=='), base64.decode('__4-AA'));
    });

    it("atob/btoa", () => {
        assert.equal(btoa(atob('//4+AA=='), true), '__4-AA');
        assert.deepEqual(atob('//4+AA=='), atob('__4-AA'));
    });

    it('base58', () => {
        var data = [
            {
                "hex": "",
                "string": "",
                "check_string": "BXvDbH"
            },
            {
                "hex": "61",
                "string": "2g",
                "check_string": "gqkwoXD"
            },
            {
                "hex": "626262",
                "string": "a3gV",
                "check_string": "ERv6dC71e7"
            },
            {
                "hex": "636363",
                "string": "aPEr",
                "check_string": "EU85aXPawp"
            },
            {
                "hex": "73696d706c792061206c6f6e6720737472696e67",
                "string": "2cFupjhnEsSn59qHXstmK2ffpLv2",
                "check_string": "arqzQCXBPJXKRhisjydDPNrVEuUgU5Z9u"
            },
            {
                "hex": "00eb15231dfceb60925886b67d065299925915aeb172c06647",
                "string": "1NS17iag9jJgTHD1VXjvLCEnZuQ3rJDE9L",
                "check_string": "CfURHbdSrFECX9eL1ymaVZa25G3wfd9WwLM4Fgmd"
            },
            {
                "hex": "516b6fcd0f",
                "string": "ABnLTmg",
                "check_string": "5J75jteGdYo93"
            },
            {
                "hex": "bf4f89001e670274dd",
                "string": "3SEo3LWLoPntC",
                "check_string": "eFw1s88fchjMPCX8r7"
            },
            {
                "hex": "572e4794",
                "string": "3EFU7m",
                "check_string": "zQHSXG3s6Vm"
            },
            {
                "hex": "ecac89cad93923c02321",
                "string": "EJDM8drfXA6uyA",
                "check_string": "488B8xyG2ERVDP3BqqwH"
            },
            {
                "hex": "10c8511e",
                "string": "Rt5zm",
                "check_string": "ndLFHWjKWMC"
            },
            {
                "hex": "00000000000000000000",
                "string": "1111111111",
                "check_string": "2d7dWtQMvj9WttCXfaeL"
            },
            {
                "hex": "801184cd2cdd640ca42cfc3a091c51d549b2f016d454b2774019c2b2d2e08529fd206ec97e",
                "string": "5Hx15HFGyep2CfPxsJKe2fXJsCVn5DEiyoeGGF6JZjGbTRnqfiD",
                "check_string": "2TEMEEdsZurFc8Kv4fey5CoL3MBDFqbQVnBVS3hQaeA9WV2aQdVqyxrLh"
            },
            {
                "hex": "003c176e659bea0f29a3e9bf7880c112b1b31b4dc826268187",
                "string": "16UjcYNBG9GTK4uq2f7yYEbuifqCzoLMGS",
                "check_string": "Cdg2fTwkNFPeEsRAAnaV1SmRpRAK7MoWnMiZj6eR"
            },
            {
                "hex": "02c0ded2bc1f1305fb0faac5e6c03ee3a1924234985427b6167ca569d13df435cfeb05f9d2",
                "string": "6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
                "check_string": "ykLqqCmtQy2YR7SjsD7W3q31Qq6dhAHFN9tr994y8TEWrexX3pLaXQ6q"
            }
        ];

        data.forEach(d => {
            var data = Buffer.from(d.hex, 'hex');
            assert.equal(base58.encode(data), d.string);
            assert.deepEqual(base58.decode(d.string), data);

            assert.equal(base58.encode(data, 1), d.check_string);
            assert.deepEqual(base58.decode(d.check_string, 1), data);
        });
    });

    it('hex', () => {
        var hexb = Buffer.alloc(256);
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

    describe('multibase', () => {
        const encoded = [
            {
                input: 'Decentralize everything!!',
                tests: [
                    ['base16', 'f446563656e7472616c697a652065766572797468696e672121'],
                    ['base16upper', 'F446563656E7472616C697A652065766572797468696E672121'],
                    ['base32', 'birswgzloorzgc3djpjssazlwmvzhs5dinfxgoijb'],
                    ['base32upper', 'BIRSWGZLOORZGC3DJPJSSAZLWMVZHS5DINFXGOIJB'],
                    ['base32pad', 'cirswgzloorzgc3djpjssazlwmvzhs5dinfxgoijb'],
                    ['base32padupper', 'CIRSWGZLOORZGC3DJPJSSAZLWMVZHS5DINFXGOIJB'],
                    ['base58btc', 'zUXE7GvtEk8XTXs1GF8HSGbVA9FCX9SEBPe'],
                    ['base64', 'mRGVjZW50cmFsaXplIGV2ZXJ5dGhpbmchIQ'],
                    ['base64pad', 'MRGVjZW50cmFsaXplIGV2ZXJ5dGhpbmchIQ=='],
                    ['base64url', 'uRGVjZW50cmFsaXplIGV2ZXJ5dGhpbmchIQ'],
                    ['base64urlpad', 'URGVjZW50cmFsaXplIGV2ZXJ5dGhpbmchIQ==']
                ]
            },
            {
                input: 'yes mani !',
                tests: [
                    ['base16', 'f796573206d616e692021'],
                    ['base16upper', 'F796573206D616E692021'],
                    ['base32', 'bpfsxgidnmfxgsibb'],
                    ['base32upper', 'BPFSXGIDNMFXGSIBB'],
                    ['base32pad', 'cpfsxgidnmfxgsibb'],
                    ['base32padupper', 'CPFSXGIDNMFXGSIBB'],
                    ['base58btc', 'z7paNL19xttacUY'],
                    ['base64', 'meWVzIG1hbmkgIQ'],
                    ['base64pad', 'MeWVzIG1hbmkgIQ=='],
                    ['base64url', 'ueWVzIG1hbmkgIQ'],
                    ['base64urlpad', 'UeWVzIG1hbmkgIQ==']
                ]
            },
            {
                input: 'hello world',
                tests: [
                    ['base16', 'f68656c6c6f20776f726c64'],
                    ['base16upper', 'F68656C6C6F20776F726C64'],
                    ['base32', 'bnbswy3dpeb3w64tmmq'],
                    ['base32upper', 'BNBSWY3DPEB3W64TMMQ'],
                    ['base32pad', 'cnbswy3dpeb3w64tmmq======'],
                    ['base32padupper', 'CNBSWY3DPEB3W64TMMQ======'],
                    ['base58btc', 'zStV1DL6CwTryKyV'],
                    ['base64', 'maGVsbG8gd29ybGQ'],
                    ['base64pad', 'MaGVsbG8gd29ybGQ='],
                    ['base64url', 'uaGVsbG8gd29ybGQ'],
                    ['base64urlpad', 'UaGVsbG8gd29ybGQ=']
                ]
            },
            {
                input: '\x00yes mani !',
                tests: [
                    ['base16', 'f00796573206d616e692021'],
                    ['base16upper', 'F00796573206D616E692021'],
                    ['base32', 'bab4wk4zanvqw42jaee'],
                    ['base32upper', 'BAB4WK4ZANVQW42JAEE'],
                    ['base32pad', 'cab4wk4zanvqw42jaee======'],
                    ['base32padupper', 'CAB4WK4ZANVQW42JAEE======'],
                    ['base58btc', 'z17paNL19xttacUY'],
                    ['base64', 'mAHllcyBtYW5pICE'],
                    ['base64pad', 'MAHllcyBtYW5pICE='],
                    ['base64url', 'uAHllcyBtYW5pICE'],
                    ['base64urlpad', 'UAHllcyBtYW5pICE=']
                ]
            },
            {
                input: '\x00\x00yes mani !',
                tests: [
                    ['base16', 'f0000796573206d616e692021'],
                    ['base16upper', 'F0000796573206D616E692021'],
                    ['base32', 'baaahszltebwwc3tjeaqq'],
                    ['base32upper', 'BAAAHSZLTEBWWC3TJEAQQ'],
                    ['base32pad', 'caaahszltebwwc3tjeaqq===='],
                    ['base32padupper', 'CAAAHSZLTEBWWC3TJEAQQ===='],
                    ['base58btc', 'z117paNL19xttacUY'],
                    ['base64', 'mAAB5ZXMgbWFuaSAh'],
                    ['base64pad', 'MAAB5ZXMgbWFuaSAh'],
                    ['base64url', 'uAAB5ZXMgbWFuaSAh'],
                    ['base64urlpad', 'UAAB5ZXMgbWFuaSAh']
                ]
            },
            {
                input: 'hello world',
                tests: [
                    ['base16', 'f68656c6c6f20776f726c64'],
                    ['base16upper', 'F68656C6C6F20776F726C64'],
                    ['base32', 'bnbswy3dpeb3w64tmmq'],
                    ['base32upper', 'BNBSWY3DPEB3W64TMMQ'],
                    ['base32pad', 'cnbswy3dpeb3w64tmmq======'],
                    ['base32padupper', 'CNBSWY3DPEB3W64TMMQ======'],
                    ['base58btc', 'zStV1DL6CwTryKyV'],
                    ['base64', 'maGVsbG8gd29ybGQ'],
                    ['base64pad', 'MaGVsbG8gd29ybGQ='],
                    ['base64url', 'uaGVsbG8gd29ybGQ'],
                    ['base64urlpad', 'UaGVsbG8gd29ybGQ=']
                ]
            }
        ]

        let index = 0
        for (const { input, tests } of encoded) {
            describe(`multibase spec ${index++}`, () => {
                for (const [name, output] of tests) {

                    describe(name, () => {
                        it('should encode buffer', () => {
                            const out = multibase.encode(input, name);
                            assert.strictEqual(out, output);
                        })

                        it('should decode string', () => {
                            assert.deepEqual(multibase.decode(output), Buffer.from(input));
                        })
                    })
                }
            })
        }
    });

    it('iconv ucs2', () => {
        for (var i = 0; i < 0xd800; i++) {
            var s = String.fromCharCode(i);
            var buf = iconv.encode('utf16le', s);
            var n = buf.readUInt16LE();
            assert.equal(i, n);
            assert.equal(iconv.decode('utf16le', buf), s);
        }

        for (var i = 0; i < 0xd800; i++) {
            var s = String.fromCharCode(i);
            var buf = iconv.encode('utf16be', s);
            var n = buf.readUInt16BE();
            assert.equal(i, n);
            assert.equal(iconv.decode('utf16be', buf), s);
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
            "00001100"
        ],
        [
            0x1fffff,
            "ffff1f00",
            "ffff1f00"
        ],
        [
            0x200000,
            "00002000",
            "00002000"
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
            var buf = Buffer.alloc(4);
            buf.writeUInt32LE(d[0]);
            var s = iconv.decode('utf32le', buf);
            var buf2 = Buffer.alloc(s.length * 2);
            buf2.writeInt16LE(s.charCodeAt(0));
            if (s.length > 1)
                buf2.writeInt16LE(s.charCodeAt(1), 2);
            assert.equal(iconv.decode('utf16le', buf2), s);
        });
    });

    it('iconv ucs4', () => {
        datas.forEach(d => {
            var buf = Buffer.alloc(4);
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

    xit('json encode object', () => {
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
            assert.deepEqual(true, msgpack.decode(msgpack.encode(new Boolean(true))));
            assert.isBoolean(msgpack.decode(msgpack.encode(true)));
        });

        it('test for false', () => {
            assert.deepEqual(false, msgpack.decode(msgpack.encode(false)));
            assert.deepEqual(false, msgpack.decode(msgpack.encode(new Boolean(false))));
            assert.isBoolean(msgpack.decode(msgpack.encode(false)));
        });

        it('test for 2^15 negative', () => {
            assert.deepEqual(
                0 - Math.pow(2, 15) - 1,
                msgpack.decode(msgpack.encode(0 - Math.pow(2, 15) - 1))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(0 - Math.pow(2, 15) - 1)));
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

        it('test for 2^15', () => {
            assert.deepEqual(
                Math.pow(2, 15) + 1,
                msgpack.decode(msgpack.encode(Math.pow(2, 15) + 1))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(Math.pow(2, 15) + 1)));
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

        it('test safe int of number', () => {
            assert.deepEqual(
                -32768,
                msgpack.decode(msgpack.encode(-32768))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(-32768)));

            assert.deepEqual(
                32767,
                msgpack.decode(msgpack.encode(32767))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(32767)));

            assert.deepEqual(
                -2147483648,
                msgpack.decode(msgpack.encode(-2147483648))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(-2147483648)));

            assert.deepEqual(
                2147483647,
                msgpack.decode(msgpack.encode(2147483647))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(2147483647)));

            assert.deepEqual(
                9007199254740992,
                msgpack.decode(msgpack.encode(9007199254740992))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(9007199254740992)));

            assert.deepEqual(
                -9007199254740992,
                msgpack.decode(msgpack.encode(-9007199254740992))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(-9007199254740992)));
        });

        it('test out safe int of number', () => {
            var tmp_out_a = 2 ** 54;
            var tmp_out_b = -(2 ** 54);

            assert.equal(typeof (msgpack.decode(msgpack.encode(tmp_out_a))), "bigint");
            assert.equal(BigInt(tmp_out_a), msgpack.decode(msgpack.encode(tmp_out_a)));

            assert.equal(typeof (msgpack.decode(msgpack.encode(tmp_out_b))), "bigint");
            assert.equal(BigInt(tmp_out_b), msgpack.decode(msgpack.encode(tmp_out_b)));
        });

        it('test encoding safe int number size', () => {
            assert.isTrue(msgpack.encode(1).length == 1);
            assert.isTrue(msgpack.encode(128).length == 2);
            assert.isTrue(msgpack.encode(0xfff).length == 3);
            assert.isTrue(msgpack.encode(0xfffff).length == 5);
        });

        it('test number approaching 2^64', () => {
            assert.deepEqual(
                123456782345245,
                msgpack.decode(msgpack.encode(123456782345245))
            );
            assert.isNumber(msgpack.decode(msgpack.encode(123456782345245)));
        });

        it('test number NaN', () => {
            assert.isTrue(Number.isNaN(msgpack.decode(msgpack.encode(NaN))));
        });

        it('test number repeating infinite decimal', () => {
            assert.isTrue(msgpack.decode(msgpack.encode(1 / 3)) == 1 / 3);
        });

        it('test number non-repeating infinite decimal', () => {
            assert.isTrue(msgpack.decode(msgpack.encode(Math.PI)) == Math.PI);
        });

        it('test number ±Infinity', () => {
            assert.isTrue(msgpack.decode(msgpack.encode(Infinity)) == Infinity);
            assert.isTrue(msgpack.decode(msgpack.encode(-Infinity)) == -Infinity);
        });

        it('test bigint approaching 2^64', () => {
            assert.deepEqual(
                2666666666666666666n,
                msgpack.decode(msgpack.encode(2666666666666666666n))
            );
            assert.deepEqual(
                -2666666666666666666n,
                msgpack.decode(msgpack.encode(-2666666666666666666n))
            );
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

        it('test for Set', () => {
            var tmp = [1, 2, 3, "a", "b", true, false];
            var set = new Set(tmp);
            assert.deepEqual(tmp, msgpack.decode(msgpack.encode(set)));
            assert.isArray(msgpack.decode(msgpack.encode(set)));
        });

        it('test for Map', () => {
            var tmp = {a: 12, b: [2, 3, 5], c: true};
            var map = new Map(Object.entries(tmp));
            assert.deepEqual(tmp, msgpack.decode(msgpack.encode(map)));
            assert.isObject(msgpack.decode(msgpack.encode(map)));
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

        it('test primitive bool / new Boolean()', () => {
            var obj1 = { 'b1': true, };
            var obj2 = { 'b1': new Boolean(true) };
            assert.deepEqual(msgpack.decode(msgpack.encode(obj1)), msgpack.decode(msgpack.encode(obj2)));
        });

        it('test primitive number / new Number()', () => {
            var obj1 = { 'n1': 1234, };
            var obj2 = { 'n1': new Number(1234) };
            assert.deepEqual(msgpack.decode(msgpack.encode(obj1)), msgpack.decode(msgpack.encode(obj2)));
        });

        it('test primitive string / new String()', () => {
            var obj1 = { 's1': 'abcd' };
            var obj2 = { 's1': new String('abcd') };
            assert.deepEqual(msgpack.decode(msgpack.encode(obj1)), msgpack.decode(msgpack.encode(obj2)));
        });
    });
});

require.main === module && test.run(console.DEBUG);