var test = require("test");
test.setup();

var encoding = require('encoding');
var json = require('json');
var bson = require('bson');
var base64 = require('base64');
var hex = require('hex');
var base64vlq = require('base64vlq');

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

        hexStr = hex.encode(hexb, ' ');
        assert.equal(hexStr, '00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f'
            + ' 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f'
            + ' 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f'
            + ' 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f'
            + ' 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f'
            + ' 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f'
            + ' 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f'
            + ' 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f'
            + ' 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f'
            + ' 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f'
            + ' a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af'
            + ' b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf'
            + ' c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf'
            + ' d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df'
            + ' e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef'
            + ' f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff'
        );

        var hexb2 = hex.decode(hexStr);
        for (var i = 0; i < 256; i++) {
            assert.equal(hexb2[i], hexb[i]);
        }
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

        var buf = new Buffer('test');
        var j = json.encode(buf);

        assert.isTrue(Buffer.isBuffer(json.decode(j)));
    });

    it('jsstr', () => {
        assert.equal(encoding.jsstr("[\r\n\t\\\'\"]"), "[\\r\\n\\t\\\\\\'\\\"]");
        assert.equal(encoding.jsstr("[abcd汉字]"), "[abcd汉字]");
    });

    it('json jsstr', () => {
        assert.equal(encoding.jsstr("[\r\n\t\\\'\"]", true), "[\\r\\n\\t\\\\'\\\"]");
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

test.run(console.DEBUG);
