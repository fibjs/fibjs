var { describe, it } = require('node:test');
var assert = require('assert');

const StringDecoder = require('string_decoder').StringDecoder;

describe('string_decoder', () => {
    let decoder;

    it('default encoding', () => {
        decoder = new StringDecoder();
        assert.strictEqual(decoder.encoding, 'utf8');
    });

    it('utf8', () => {
        test('utf-8', Buffer.from('$', 'utf-8'), '$');
        test('utf-8', Buffer.from('¢', 'utf-8'), '¢');
        test('utf-8', Buffer.from('€', 'utf-8'), '€');
        test('utf-8', Buffer.from('𤭢', 'utf-8'), '𤭢');
        // A mixed ascii and non-ascii string
        // Test stolen from deps/v8/test/cctest/test-strings.cc
        // U+02E4 -> CB A4
        // U+0064 -> 64
        // U+12E4 -> E1 8B A4
        // U+0030 -> 30
        // U+3045 -> E3 81 85
        test(
            'utf-8',
            Buffer.from([0xCB, 0xA4, 0x64, 0xE1, 0x8B, 0xA4, 0x30, 0xE3, 0x81, 0x85]),
            '\u02e4\u0064\u12e4\u0030\u3045'
        );

        // Some invalid input, known to have caused trouble with chunking
        // in https://github.com/nodejs/node/pull/7310#issuecomment-226445923
        // 00: |00000000 ASCII
        // 41: |01000001 ASCII
        // B8: 10|111000 continuation
        // CC: 110|01100 two-byte head
        // E2: 1110|0010 three-byte head
        // F0: 11110|000 four-byte head
        // F1: 11110|001'another four-byte head
        // FB: 111110|11 "five-byte head", not UTF-8
        test('utf-8', Buffer.from('C9B5A941', 'hex'), '\u0275\ufffdA');
        test('utf-8', Buffer.from('E2', 'hex'), '\ufffd');
        test('utf-8', Buffer.from('E241', 'hex'), '\ufffdA');
        test('utf-8', Buffer.from('CCCCB8', 'hex'), '\ufffd\u0338');
        test('utf-8', Buffer.from('F0B841', 'hex'), '\ufffdA');
        test('utf-8', Buffer.from('F1CCB8', 'hex'), '\ufffd\u0338');
        test('utf-8', Buffer.from('F0FB00', 'hex'), '\ufffd\ufffd\0');
        test('utf-8', Buffer.from('CCE2B8B8', 'hex'), '\ufffd\u2e38');
        test('utf-8', Buffer.from('E2B8CCB8', 'hex'), '\ufffd\u0338');
        test('utf-8', Buffer.from('E2FBCC01', 'hex'), '\ufffd\ufffd\ufffd\u0001');
        test('utf-8', Buffer.from('CCB8CDB9', 'hex'), '\u0338\u0379');
        // // CESU-8 of U+1D40D
        // test('utf-8', Buffer.from('EDA0B5EDB08D', 'hex'), '\ufffd\ufffd');

        decoder = new StringDecoder('utf8');
        assert.strictEqual(decoder.write(Buffer.from('E1', 'hex')), '');
        assert.strictEqual(decoder.end(), '\ufffd');

        decoder = new StringDecoder('utf8');
        assert.strictEqual(decoder.write(Buffer.from('E18B', 'hex')), '');
        assert.strictEqual(decoder.end(), '\ufffd');

        decoder = new StringDecoder('utf8');
        assert.strictEqual(decoder.write(Buffer.from('\ufffd')), '\ufffd');
        assert.strictEqual(decoder.end(), '');

        decoder = new StringDecoder('utf8');
        assert.strictEqual(decoder.write(Buffer.from('\ufffd\ufffd\ufffd')),
            '\ufffd\ufffd\ufffd');
        assert.strictEqual(decoder.end(), '');

        decoder = new StringDecoder('utf8');
        assert.strictEqual(decoder.write(Buffer.from('EFBFBDE2', 'hex')), '\ufffd');
        assert.strictEqual(decoder.end(), '\ufffd');

        decoder = new StringDecoder('utf8');
        assert.strictEqual(decoder.write(Buffer.from('F1', 'hex')), '');
        assert.strictEqual(decoder.write(Buffer.from('41F2', 'hex')), '\ufffdA');
        assert.strictEqual(decoder.end(), '\ufffd');

        // Additional utf8Text test
        decoder = new StringDecoder('utf8');
        assert.strictEqual(decoder.text(Buffer.from([0x41]), 2), '');
    });

    it('ucs2/UTF-16LE', () => {
        // UCS-2
        test('ucs2', Buffer.from('ababc', 'ucs2'), 'ababc');

        // UTF-16LE
        // test('utf16le', Buffer.from('3DD84DDC', 'hex'), '\ud83d\udc4d'); // thumbs up

        decoder = new StringDecoder('utf16le');
        assert.strictEqual(decoder.write(Buffer.from('3DD8', 'hex')), '');
        assert.strictEqual(decoder.write(Buffer.from('4D', 'hex')), '');
        assert.strictEqual(decoder.write(Buffer.from('DC', 'hex')), '\ud83d\udc4d');
        assert.strictEqual(decoder.end(), '');

        decoder = new StringDecoder('utf16le');
        assert.strictEqual(decoder.write(Buffer.from('3DD8', 'hex')), '');
        // assert.strictEqual(decoder.end(), '\ud83d');

        decoder = new StringDecoder('utf16le');
        assert.strictEqual(decoder.write(Buffer.from('3DD8', 'hex')), '');
        assert.strictEqual(decoder.write(Buffer.from('4D', 'hex')), '');
        // assert.strictEqual(decoder.end(), '\ud83d');
    });

    it('should throws', () => {
        assert.throws(() => {
            new StringDecoder(1);
        });

        assert.throws(() => {
            new StringDecoder('test');
        });
    });

    it('end', () => {
        const encodings = ['base64', 'hex', 'utf8', 'utf16le', 'ucs2'];
        const bufs = ['☃💩', 'asdf'].map((b) => Buffer.from(b));

        // also test just arbitrary bytes from 0-15.
        for (let i = 1; i <= 16; i++) {
            const bytes = '.'.repeat(i - 1).split('.').map((_, j) => j + 0x78);
            bufs.push(Buffer.from(bytes));
        }

        encodings.forEach(testEncoding);

        function testEncoding(encoding) {
            bufs.forEach((buf) => {
                testBuf(encoding, buf);
            });
        }

        function testBuf(encoding, buf) {
            // write one byte at a time.
            let s = new StringDecoder(encoding);
            let res1 = '';
            for (let i = 0; i < buf.length; i++) {
                res1 += s.write(buf.slice(i, i + 1));
            }
            res1 += s.end();

            // write the whole buffer at once.
            let res2 = '';
            s = new StringDecoder(encoding);
            res2 += s.write(buf);
            res2 += s.end();

            // .toString() on the buffer
            const res3 = buf.toString(encoding);

            assert.strictEqual(res1, res3, 'one byte at a time should match toString');
            assert.strictEqual(res2, res3, 'all bytes at once should match toString');
        }
    });
});

describe('TextEncoder', () => {
    it('default encoding', () => {
        const enc = new TextEncoder();
        assert.strictEqual(enc.encoding, 'utf-8');
    });

    it('encode basic strings', () => {
        const enc = new TextEncoder();

        // ASCII
        assert.deepStrictEqual([...enc.encode('abc123')], [0x61, 0x62, 0x63, 0x31, 0x32, 0x33]);

        // empty
        assert.strictEqual(enc.encode().length, 0);
        assert.strictEqual(enc.encode('').length, 0);
        assert.strictEqual(enc.encode(undefined).length, 0);
    });

    it('encode multibyte characters', () => {
        const enc = new TextEncoder();

        // z (U+007A), cent (U+00A2), CJK water (U+6C34),
        // G-Clef (U+1D11E), PUA (U+F8FF), PUA (U+10FFFD), byte-swapped BOM (U+FFFE)
        const sample = 'z\xA2\u6C34\uD834\uDD1E\uF8FF\uDBFF\uDFFD\uFFFE';
        const expected = [
            0x7A, 0xC2, 0xA2, 0xE6, 0xB0, 0xB4,
            0xF0, 0x9D, 0x84, 0x9E, 0xEF, 0xA3,
            0xBF, 0xF4, 0x8F, 0xBF, 0xBD, 0xEF,
            0xBF, 0xBE,
        ];
        assert.deepStrictEqual([...enc.encode(sample)], expected);
    });

    it('encode with BOM prefix', () => {
        const enc = new TextEncoder();
        const buf = enc.encode('\ufefftest€');
        const expected = Buffer.from([0xef, 0xbb, 0xbf, 0x74, 0x65,
            0x73, 0x74, 0xe2, 0x82, 0xac]);
        assert.strictEqual(Buffer.compare(buf, expected), 0);
    });

    it('USVString surrogate handling', () => {
        const enc = new TextEncoder();

        // fibjs passes JS strings to C++ as WTF-8, lone surrogates
        // are encoded as-is (not replaced with U+FFFD at the engine level)
        // Only test valid surrogate pair case
        const validPair = enc.encode('\uD834\uDD1E');
        assert.deepStrictEqual([...validPair], [0xF0, 0x9D, 0x84, 0x9E], 'valid surrogate pair (G-Clef)');

        // Lone surrogates: verify encode doesn't throw
        const lone = enc.encode('\uD800');
        assert.ok(lone.length > 0, 'lone surrogate encodes to something');
    });

    it('return type is Uint8Array', () => {
        const enc = new TextEncoder();
        const result = enc.encode('test');
        assert.ok(result instanceof Uint8Array);
    });
});

describe('TextDecoder', () => {
    it('default encoding', () => {
        const dec = new TextDecoder();
        assert.strictEqual(dec.encoding, 'utf-8');
        assert.strictEqual(dec.fatal, false);
        assert.strictEqual(dec.ignoreBOM, false);
    });

    it('constructor with label undefined and options null', () => {
        const dec = new TextDecoder(undefined, null);
        assert.strictEqual(dec.encoding, 'utf-8');
        assert.strictEqual(dec.fatal, false);
        assert.strictEqual(dec.ignoreBOM, false);
    });

    it('encoding aliases', () => {
        const aliases = ['unicode-1-1-utf-8', 'unicode11utf8', 'unicode20utf8',
            'utf8', 'utf-8', 'x-unicode20utf8'];
        aliases.forEach((alias) => {
            const dec = new TextDecoder(alias);
            assert.strictEqual(dec.encoding, 'utf-8', `alias ${alias} should map to utf-8`);
        });
    });

    it('decode UTF-8 basic', () => {
        const buf = Buffer.from([0xef, 0xbb, 0xbf, 0x74, 0x65,
            0x73, 0x74, 0xe2, 0x82, 0xac]);

        const aliases = ['unicode-1-1-utf-8', 'unicode11utf8', 'unicode20utf8',
            'utf8', 'utf-8', 'x-unicode20utf8'];
        aliases.forEach((alias) => {
            const dec = new TextDecoder(alias);
            const res = dec.decode(buf);
            assert.strictEqual(res, 'test€', `decode with alias ${alias}`);
        });
    });

    it('decode UTF-8 sample characters', () => {
        // z (U+007A), cent (U+00A2), CJK water (U+6C34),
        // G-Clef (U+1D11E), PUA (U+F8FF), PUA (U+10FFFD), byte-swapped BOM (U+FFFE)
        const sample = 'z\xA2\u6C34\uD834\uDD1E\uF8FF\uDBFF\uDFFD\uFFFE';
        const bytes = [
            0x7A, 0xC2, 0xA2, 0xE6, 0xB0, 0xB4,
            0xF0, 0x9D, 0x84, 0x9E, 0xEF, 0xA3,
            0xBF, 0xF4, 0x8F, 0xBF, 0xBD, 0xEF,
            0xBF, 0xBE,
        ];
        assert.strictEqual(
            new TextDecoder('utf-8').decode(new Uint8Array(bytes)), sample);
        assert.strictEqual(
            new TextDecoder('utf-8').decode(new Uint8Array(bytes).buffer), sample);
    });

    it('decode UTF-16LE', () => {
        const dec = new TextDecoder('utf-16le');
        const res = dec.decode(Buffer.from('test€', 'utf-16le'));
        assert.strictEqual(res, 'test€');
    });

    it('decode UTF-16LE sample', () => {
        const sample = 'z\xA2\u6C34\uD834\uDD1E\uF8FF\uDBFF\uDFFD\uFFFE';
        const bytes = [
            0x7A, 0x00, 0xA2, 0x00, 0x34, 0x6C,
            0x34, 0xD8, 0x1E, 0xDD, 0xFF, 0xF8,
            0xFF, 0xDB, 0xFD, 0xDF, 0xFE, 0xFF,
        ];
        assert.strictEqual(
            new TextDecoder('utf-16le').decode(new Uint8Array(bytes)), sample);
    });

    it('decode empty input', () => {
        const dec = new TextDecoder();
        assert.strictEqual(dec.decode(), '');
        assert.strictEqual(dec.decode(new Uint8Array([])), '');
        assert.strictEqual(dec.decode(new Uint8Array([]).buffer), '');
    });

    it('invalid encoding labels', () => {
        ['meow', 'nonunicode', 'foo', 'bar'].forEach((fakeEncoding) => {
            assert.throws(
                () => { new TextDecoder(fakeEncoding); },
                {
                    name: 'RangeError'
                }
            );
        });
    });

    it('invalid input types', () => {
        // fibjs framework converts strings to Buffer, so exclude '' from invalid inputs
        const notArrayBufferViewExamples = [false, {}, 1, new Error()];
        notArrayBufferViewExamples.forEach((invalidInput) => {
            assert.throws(() => {
                new TextDecoder(undefined, null).decode(invalidInput);
            });
        });
    });

    it('ignoreBOM option', () => {
        const cases = [
            {
                encoding: 'utf-8',
                bytes: [0xEF, 0xBB, 0xBF, 0x61, 0x62, 0x63]
            },
            {
                encoding: 'utf-16le',
                bytes: [0xFF, 0xFE, 0x61, 0x00, 0x62, 0x00, 0x63, 0x00]
            },
        ];

        const BOM = '\uFEFF';
        cases.forEach((testCase) => {
            const bytes = new Uint8Array(testCase.bytes);

            // ignoreBOM: true => BOM preserved in output
            let dec = new TextDecoder(testCase.encoding, { ignoreBOM: true });
            assert.strictEqual(dec.decode(bytes), `${BOM}abc`,
                `${testCase.encoding} ignoreBOM:true`);

            // ignoreBOM: false (default) => BOM stripped
            dec = new TextDecoder(testCase.encoding, { ignoreBOM: false });
            assert.strictEqual(dec.decode(bytes), 'abc',
                `${testCase.encoding} ignoreBOM:false`);

            // default => BOM stripped
            dec = new TextDecoder(testCase.encoding);
            assert.strictEqual(dec.decode(bytes), 'abc',
                `${testCase.encoding} default`);
        });
    });

    it('ignoreBOM with streaming', () => {
        const buf = Buffer.from([0xef, 0xbb, 0xbf, 0x74, 0x65,
            0x73, 0x74, 0xe2, 0x82, 0xac]);

        // ignoreBOM: true, streaming
        const aliases = ['utf-8', 'utf8'];
        aliases.forEach((alias) => {
            const dec = new TextDecoder(alias, { ignoreBOM: true });
            let res = '';
            res += dec.decode(buf.slice(0, 8), { stream: true });
            res += dec.decode(buf.slice(8));
            assert.strictEqual(res, '\ufefftest€', `ignoreBOM streaming: ${alias}`);
        });
    });

    it('streaming decode UTF-8 split multibyte', () => {
        // From WPT: textdecoder-streaming.any.js
        const string =
            '\x00123ABCabc\x80\xFF\u0100\u1000\uFFFD\uD800\uDC00\uDBFF\uDFFF';
        const octets = {
            'utf-8': [
                0x00, 0x31, 0x32, 0x33, 0x41, 0x42, 0x43, 0x61, 0x62, 0x63, 0xc2, 0x80,
                0xc3, 0xbf, 0xc4, 0x80, 0xe1, 0x80, 0x80, 0xef, 0xbf, 0xbd, 0xf0, 0x90,
                0x80, 0x80, 0xf4, 0x8f, 0xbf, 0xbf],
            'utf-16le': [
                0x00, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x41, 0x00, 0x42, 0x00,
                0x43, 0x00, 0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x80, 0x00, 0xFF, 0x00,
                0x00, 0x01, 0x00, 0x10, 0xFD, 0xFF, 0x00, 0xD8, 0x00, 0xDC, 0xFF, 0xDB,
                0xFF, 0xDF]
        };

        Object.keys(octets).forEach((encoding) => {
            // Split at every possible chunk size from 1 to 5 bytes
            for (let len = 1; len <= 5; ++len) {
                const encoded = octets[encoding];
                const decoder = new TextDecoder(encoding);
                let out = '';
                for (let i = 0; i < encoded.length; i += len) {
                    const sub = [];
                    for (let j = i; j < encoded.length && j < i + len; ++j)
                        sub.push(encoded[j]);
                    out += decoder.decode(new Uint8Array(sub), { stream: true });
                }
                out += decoder.decode();
                assert.strictEqual(out, string,
                    `${encoding} streaming with chunk size ${len}`);
            }
        });
    });

    it('streaming decode UTF-8 two-part split', () => {
        const buf = Buffer.from([0xef, 0xbb, 0xbf, 0x74, 0x65,
            0x73, 0x74, 0xe2, 0x82, 0xac]);

        const aliases = ['utf-8', 'utf8'];
        aliases.forEach((alias) => {
            const dec = new TextDecoder(alias);
            let res = '';
            // Split right in the middle of the € character (E2 82 AC)
            res += dec.decode(buf.slice(0, 8), { stream: true });
            res += dec.decode(buf.slice(8));
            assert.strictEqual(res, 'test€', `two-part split: ${alias}`);
        });
    });

    it('streaming decode resets state on non-stream call', () => {
        // After a non-stream decode, internal buffer should be flushed
        const dec = new TextDecoder('utf-8');

        // Start with incomplete sequence
        let res = dec.decode(new Uint8Array([0xE2, 0x82]), { stream: true });
        assert.strictEqual(res, '');

        // Flush with no stream flag - incomplete bytes become replacement char
        res = dec.decode(new Uint8Array([]), { stream: false });
        assert.strictEqual(res, '\ufffd');

        // A new sequence should work fresh
        res = dec.decode(new Uint8Array([0xE2, 0x82, 0xAC]));
        assert.strictEqual(res, '€');
    });

    it('streaming decode byte-by-byte', () => {
        const dec = new TextDecoder('utf-8');
        const bytes = [0xF0, 0x9D, 0x84, 0x9E]; // U+1D11E G-Clef (4 bytes)

        let out = '';
        for (let i = 0; i < bytes.length - 1; i++) {
            out += dec.decode(new Uint8Array([bytes[i]]), { stream: true });
        }
        out += dec.decode(new Uint8Array([bytes[bytes.length - 1]]));
        assert.strictEqual(out, '\uD834\uDD1E'); // G-Clef as surrogate pair in JS
    });

    it('streaming decode UTF-8 invalid byte error recovery', () => {
        function bytes(arr) { return new Uint8Array(arr); }
        const decoder = new TextDecoder();

        // 0xC1 is always invalid (overlong 2-byte lead), should emit replacement immediately
        assert.strictEqual(decoder.decode(bytes([0xC1]), { stream: true }), '\uFFFD');
        assert.strictEqual(decoder.decode(), '');

        // 0xF5 is always invalid (> U+10FFFF), should emit replacement immediately
        assert.strictEqual(decoder.decode(bytes([0xF5]), { stream: true }), '\uFFFD');
        assert.strictEqual(decoder.decode(), '');

        // 3-byte lead (E0) followed by non-continuation (0x41='A')
        assert.strictEqual(decoder.decode(bytes([0xE0, 0x41]), { stream: true }), '\uFFFDA');
        assert.strictEqual(decoder.decode(bytes([0x42])), 'B');

        // 3-byte lead (E0) followed by out-of-range continuation (0x80 < 0xA0 for E0)
        assert.strictEqual(decoder.decode(bytes([0xE0, 0x80]), { stream: true }), '\uFFFD\uFFFD');
        assert.strictEqual(decoder.decode(bytes([0x80])), '\uFFFD');

        // ED followed by surrogate-range continuation (0xA0 >= 0xA0, surrogate territory)
        assert.strictEqual(decoder.decode(bytes([0xED, 0xA0]), { stream: true }), '\uFFFD\uFFFD');
        assert.strictEqual(decoder.decode(bytes([0x80])), '\uFFFD');

        // 4-byte lead (F0) followed by non-continuation
        assert.strictEqual(decoder.decode(bytes([0xF0, 0x41]), { stream: true }), '\uFFFDA');
        assert.strictEqual(decoder.decode(bytes([0x42]), { stream: true }), 'B');
        assert.strictEqual(decoder.decode(bytes([0x43])), 'C');

        // 4-byte lead (F0) followed by out-of-range continuations (0x80 < 0x90 for F0)
        assert.strictEqual(decoder.decode(bytes([0xF0, 0x80]), { stream: true }), '\uFFFD\uFFFD');
        assert.strictEqual(decoder.decode(bytes([0x80]), { stream: true }), '\uFFFD');
        assert.strictEqual(decoder.decode(bytes([0x80])), '\uFFFD');

        // 4-byte lead F4 followed by over-range continuation (0xA0 > 0x8F for F4)
        assert.strictEqual(decoder.decode(bytes([0xF4, 0xA0]), { stream: true }), '\uFFFD\uFFFD');
        assert.strictEqual(decoder.decode(bytes([0x80]), { stream: true }), '\uFFFD');
        assert.strictEqual(decoder.decode(bytes([0x80])), '\uFFFD');

        // F0 90 (valid start of 4-byte) then non-continuation
        assert.strictEqual(decoder.decode(bytes([0xF0, 0x90, 0x41]), { stream: true }), '\uFFFDA');
        assert.strictEqual(decoder.decode(bytes([0x42])), 'B');

        // 4-byte: first 3 bytes buffered in stream, 4th byte completes the character
        assert.strictEqual(decoder.decode(bytes([0xF0, 0x9F, 0x92]), { stream: true }), '');
        assert.strictEqual(decoder.decode(bytes([0xA9])), '\u{1F4A9}');
    });

    it('fatal mode with invalid UTF-8 sequences', () => {
        const bad = [
            { input: [0xFF], name: 'invalid code' },
            { input: [0xC0], name: 'ends early' },
            { input: [0xE0], name: 'ends early 2' },
            { input: [0xC0, 0x00], name: 'invalid trail' },
            { input: [0xC0, 0xC0], name: 'invalid trail 2' },
            { input: [0xE0, 0x00], name: 'invalid trail 3' },
            { input: [0xE0, 0xC0], name: 'invalid trail 4' },
            { input: [0xE0, 0x80, 0x00], name: 'invalid trail 5' },
            { input: [0xE0, 0x80, 0xC0], name: 'invalid trail 6' },
            { input: [0xFC, 0x80, 0x80, 0x80, 0x80, 0x80], name: '> 0x10FFFF' },
            { input: [0xFE, 0x80, 0x80, 0x80, 0x80, 0x80], name: 'obsolete lead byte' },
            // Overlong encodings
            { input: [0xC0, 0x80], name: 'overlong U+0000 - 2 bytes' },
            { input: [0xE0, 0x80, 0x80], name: 'overlong U+0000 - 3 bytes' },
            { input: [0xF0, 0x80, 0x80, 0x80], name: 'overlong U+0000 - 4 bytes' },
            { input: [0xC1, 0xBF], name: 'overlong U+007F - 2 bytes' },
            { input: [0xE0, 0x81, 0xBF], name: 'overlong U+007F - 3 bytes' },
            { input: [0xE0, 0x9F, 0xBF], name: 'overlong U+07FF - 3 bytes' },
            { input: [0xF0, 0x8F, 0xBF, 0xBF], name: 'overlong U+FFFF - 4 bytes' },
            // UTF-16 surrogates encoded as code points in UTF-8
            { input: [0xED, 0xA0, 0x80], name: 'lead surrogate' },
            { input: [0xED, 0xB0, 0x80], name: 'trail surrogate' },
            { input: [0xED, 0xA0, 0x80, 0xED, 0xB0, 0x80], name: 'surrogate pair' },
        ];

        bad.forEach((t) => {
            assert.throws(
                () => {
                    new TextDecoder('utf-8', { fatal: true })
                        .decode(new Uint8Array(t.input));
                },
                { name: 'TypeError' },
                `fatal: ${t.name}`
            );
        });
    });

    it('fatal mode UTF-16LE', () => {
        // Truncated code unit
        assert.throws(
            () => {
                new TextDecoder('utf-16le', { fatal: true })
                    .decode(new Uint8Array([0x00]));
            },
            { name: 'TypeError' }
        );
    });

    it('fatal mode with streaming - incomplete then complete', () => {
        const dec = new TextDecoder('utf-8', { fatal: true });
        // E2 82 AC = € (valid 3-byte)
        // Send first 2 bytes in stream mode
        const part1 = dec.decode(new Uint8Array([0xE2, 0x82]), { stream: true });
        assert.strictEqual(part1, '');
        // Send last byte - should complete successfully
        const part2 = dec.decode(new Uint8Array([0xAC]));
        assert.strictEqual(part2, '€');
    });

    it('fatal streaming - incomplete at end throws', () => {
        const dec = new TextDecoder('utf-8', { fatal: true });
        // Incomplete sequence in stream mode is OK
        dec.decode(new Uint8Array([0xE2, 0x82]), { stream: true });
        // But flushing (non-stream) with pending incomplete bytes should throw
        assert.throws(
            () => { dec.decode(); },
            { name: 'TypeError' }
        );
    });

    it('fatal streaming UTF-16LE - odd bytes', () => {
        const decoder = new TextDecoder('utf-16le', { fatal: true });
        const odd = new Uint8Array([0x00]);
        const even = new Uint8Array([0x00, 0x00]);

        // Even then odd - fails on flush
        assert.throws(() => {
            decoder.decode(even, { stream: true });
            decoder.decode(odd);
        }, { name: 'TypeError' });
    });

    it('UTF-16LE surrogate handling (non-fatal)', () => {
        const bad = [
            {
                input: [0x00, 0xd8],
                expected: '\uFFFD',
                name: 'lone surrogate lead'
            },
            {
                input: [0x00, 0xdc],
                expected: '\uFFFD',
                name: 'lone surrogate trail'
            },
            {
                input: [0x00, 0xd8, 0x00, 0x00],
                expected: '\uFFFD\u0000',
                name: 'unmatched surrogate lead'
            },
            {
                input: [0x00, 0xdc, 0x00, 0x00],
                expected: '\uFFFD\u0000',
                name: 'unmatched surrogate trail'
            },
            {
                input: [0x00, 0xdc, 0x00, 0xd8],
                expected: '\uFFFD\uFFFD',
                name: 'swapped surrogate pair'
            },
        ];

        bad.forEach((t) => {
            const result = new TextDecoder('utf-16le')
                .decode(new Uint8Array(t.input));
            assert.strictEqual(result, t.expected, `non-fatal: ${t.name}`);
        });
    });

    it('UTF-16LE surrogate handling (fatal)', () => {
        const bad = [
            { input: [0x00, 0xd8], name: 'lone surrogate lead' },
            { input: [0x00, 0xdc], name: 'lone surrogate trail' },
            { input: [0x00, 0xd8, 0x00, 0x00], name: 'unmatched surrogate lead' },
            { input: [0x00, 0xdc, 0x00, 0x00], name: 'unmatched surrogate trail' },
            { input: [0x00, 0xdc, 0x00, 0xd8], name: 'swapped surrogate pair' },
        ];

        bad.forEach((t) => {
            assert.throws(
                () => {
                    new TextDecoder('utf-16le', { fatal: true })
                        .decode(new Uint8Array(t.input));
                },
                { name: 'TypeError' },
                `fatal: ${t.name}`
            );
        });
    });

    it('encode/decode round-trip', () => {
        const enc = new TextEncoder();
        const dec = new TextDecoder('utf-8');

        const strings = [
            '',
            'hello',
            '日本語テスト',
            '𤭢𠮷',
            'z\xA2\u6C34\uD834\uDD1E\uF8FF\uDBFF\uDFFD\uFFFE',
            'Mixed: ASCII + 中文 + émojis 🎉🚀',
            '\x00\x01\x02 control chars',
        ];

        strings.forEach((str) => {
            const encoded = enc.encode(str);
            const decoded = dec.decode(encoded);
            assert.strictEqual(decoded, str, `round-trip: ${JSON.stringify(str)}`);
        });
    });

    it('multiple streaming calls then flush', () => {
        const dec = new TextDecoder('utf-8');
        const text = '你好世界Hello🌍';
        const encoded = new TextEncoder().encode(text);

        // Feed 3 bytes at a time
        let result = '';
        for (let i = 0; i < encoded.length; i += 3) {
            const chunk = encoded.slice(i, Math.min(i + 3, encoded.length));
            result += dec.decode(chunk, { stream: true });
        }
        result += dec.decode(); // flush
        assert.strictEqual(result, text);
    });

    it('decoder reuse after flush', () => {
        const dec = new TextDecoder('utf-8');

        // First decode cycle
        let r1 = dec.decode(new Uint8Array([0xE4, 0xB8]), { stream: true });
        assert.strictEqual(r1, '');
        r1 += dec.decode(new Uint8Array([0xAD])); // '中'
        assert.strictEqual(r1, '中');

        // Second decode cycle - decoder should be reusable
        let r2 = dec.decode(new Uint8Array([0xE4, 0xB8, 0x96])); // '世'
        assert.strictEqual(r2, '世');
    });

    it('non-fatal mode replaces invalid bytes with U+FFFD', () => {
        const dec = new TextDecoder('utf-8');

        // Single invalid byte
        assert.strictEqual(dec.decode(new Uint8Array([0xFF])), '\uFFFD');
        // Invalid byte surrounded by ASCII
        assert.strictEqual(
            dec.decode(new Uint8Array([0x41, 0xFF, 0x42])),
            'A\uFFFDB'
        );
        // Incomplete 2-byte sequence
        assert.strictEqual(
            dec.decode(new Uint8Array([0xC2])),
            '\uFFFD'
        );
        // Incomplete 3-byte sequence
        assert.strictEqual(
            dec.decode(new Uint8Array([0xE0, 0xA0])),
            '\uFFFD'
        );
        // Incomplete 4-byte sequence
        assert.strictEqual(
            dec.decode(new Uint8Array([0xF0, 0x90, 0x80])),
            '\uFFFD'
        );
    });

    it('TypedArray and ArrayBuffer inputs', () => {
        const bytes = new Uint8Array([0x48, 0x65, 0x6C, 0x6C, 0x6F]); // "Hello"
        const dec = new TextDecoder();

        // Uint8Array
        assert.strictEqual(dec.decode(bytes), 'Hello');
        // ArrayBuffer
        assert.strictEqual(dec.decode(bytes.buffer), 'Hello');
        // DataView
        assert.strictEqual(dec.decode(new DataView(bytes.buffer)), 'Hello');
        // Uint16Array (view over same buffer)
        // Note: This reads the bytes as-is, not reinterpreted as uint16
        const u16 = new Uint16Array(bytes.buffer, 0, 2); // first 4 bytes
        assert.strictEqual(dec.decode(u16), 'Hell');
    });

    it('BOM round-trip with ignoreBOM:true', () => {
        const enc = new TextEncoder();
        const dec = new TextDecoder('utf-8', { ignoreBOM: true });
        const input = '\ufeffBOM prefix';
        const encoded = enc.encode(input);
        const decoded = dec.decode(encoded);
        assert.strictEqual(decoded, input);
    });

    it('BOM stripped in default round-trip', () => {
        const enc = new TextEncoder();
        const dec = new TextDecoder('utf-8');
        const input = '\ufeffBOM prefix';
        const encoded = enc.encode(input);
        const decoded = dec.decode(encoded);
        // BOM is stripped by default
        assert.strictEqual(decoded, 'BOM prefix');
    });

    it('encodeInto', () => {
        const enc = new TextEncoder();

        // Basic test
        const dest = new Uint8Array(10);
        const result = enc.encodeInto('hello', dest);
        assert.strictEqual(result.read, 5);
        assert.strictEqual(result.written, 5);
        assert.deepStrictEqual([...dest.slice(0, 5)], [0x68, 0x65, 0x6C, 0x6C, 0x6F]);

        // Multibyte - buffer too small
        const small = new Uint8Array(2);
        const r2 = enc.encodeInto('€', small); // € is 3 bytes in UTF-8
        assert.strictEqual(r2.read, 0); // can't fit even one char
        assert.strictEqual(r2.written, 0);

        // Exact fit
        const exact = new Uint8Array(3);
        const r3 = enc.encodeInto('€', exact);
        assert.strictEqual(r3.read, 1);
        assert.strictEqual(r3.written, 3);
        assert.deepStrictEqual([...exact], [0xE2, 0x82, 0xAC]);

        // 4-byte character (surrogate pair in JS)
        const buf4 = new Uint8Array(4);
        const r4 = enc.encodeInto('\uD834\uDD1E', buf4); // G-Clef
        assert.strictEqual(r4.read, 2); // 2 UTF-16 code units consumed
        assert.strictEqual(r4.written, 4);
        assert.deepStrictEqual([...buf4], [0xF0, 0x9D, 0x84, 0x9E]);

        // Empty string
        const empty = new Uint8Array(5);
        const r5 = enc.encodeInto('', empty);
        assert.strictEqual(r5.read, 0);
        assert.strictEqual(r5.written, 0);
    });

    it('streaming with mixed ASCII and multibyte', () => {
        const dec = new TextDecoder('utf-8');
        const text = 'Hello, 世界! 🌍';
        const encoded = new TextEncoder().encode(text);

        // Feed one byte at a time
        let result = '';
        for (let i = 0; i < encoded.length; i++) {
            result += dec.decode(new Uint8Array([encoded[i]]), { stream: true });
        }
        result += dec.decode();
        assert.strictEqual(result, text);
    });

    it('streaming UTF-16LE byte by byte', () => {
        const dec = new TextDecoder('utf-16le');
        const text = 'AB';
        const bytes = [0x41, 0x00, 0x42, 0x00]; // 'A' = 0x0041, 'B' = 0x0042

        let result = '';
        for (let i = 0; i < bytes.length; i++) {
            result += dec.decode(new Uint8Array([bytes[i]]), { stream: true });
        }
        result += dec.decode();
        assert.strictEqual(result, text);
    });

    it('Symbol.toStringTag', () => {
        const dec = new TextDecoder();
        assert.strictEqual(dec[Symbol.toStringTag], 'TextDecoder');
        const enc = new TextEncoder();
        assert.strictEqual(enc[Symbol.toStringTag], 'TextEncoder');
    });

    it('decode UTF-16 alias (utf-16 maps to utf-16le)', () => {
        const sample = 'z\xA2\u6C34\uD834\uDD1E\uF8FF\uDBFF\uDFFD\uFFFE';
        const bytes = [
            0x7A, 0x00, 0xA2, 0x00, 0x34, 0x6C,
            0x34, 0xD8, 0x1E, 0xDD, 0xFF, 0xF8,
            0xFF, 0xDB, 0xFD, 0xDF, 0xFE, 0xFF,
        ];
        assert.strictEqual(
            new TextDecoder('utf-16').decode(new Uint8Array(bytes)), sample);
        assert.strictEqual(
            new TextDecoder('utf-16').decode(new Uint8Array(bytes).buffer), sample);
    });

    it('invalid encoding labels with special whitespace', () => {
        const validLabels = ['utf-8', 'utf-16le'];
        const badWhitespace = ['\u0000', '\u000b', '\u00a0', '\u2028', '\u2029'];

        validLabels.forEach((label) => {
            badWhitespace.forEach((ws) => {
                assert.throws(
                    () => new TextDecoder(`${ws}${label}`),
                    { name: 'RangeError' },
                    `prefix U+${ws.charCodeAt(0).toString(16).padStart(4, '0')} + ${label}`
                );
                assert.throws(
                    () => new TextDecoder(`${label}${ws}`),
                    { name: 'RangeError' },
                    `${label} + suffix U+${ws.charCodeAt(0).toString(16).padStart(4, '0')}`
                );
                assert.throws(
                    () => new TextDecoder(`${ws}${label}${ws}`),
                    { name: 'RangeError' },
                    `U+${ws.charCodeAt(0).toString(16).padStart(4, '0')} around ${label}`
                );
            });
        });
    });

    it('fatal mode with additional overlong encodings (5/6 byte)', () => {
        const additionalBad = [
            { input: [0xF8, 0x80, 0x80, 0x80, 0x80], name: 'overlong U+0000 - 5 bytes' },
            { input: [0xFC, 0x80, 0x80, 0x80, 0x80, 0x80], name: 'overlong U+0000 - 6 bytes' },
            { input: [0xF8, 0x80, 0x80, 0x81, 0xBF], name: 'overlong U+007F - 5 bytes' },
            { input: [0xFC, 0x80, 0x80, 0x80, 0x81, 0xBF], name: 'overlong U+007F - 6 bytes' },
            { input: [0xF0, 0x80, 0x81, 0xBF], name: 'overlong U+007F - 4 bytes' },
            { input: [0xF0, 0x80, 0x9F, 0xBF], name: 'overlong U+07FF - 4 bytes' },
            { input: [0xF8, 0x80, 0x80, 0x9F, 0xBF], name: 'overlong U+07FF - 5 bytes' },
            { input: [0xFC, 0x80, 0x80, 0x80, 0x9F, 0xBF], name: 'overlong U+07FF - 6 bytes' },
            { input: [0xF8, 0x80, 0x8F, 0xBF, 0xBF], name: 'overlong U+FFFF - 5 bytes' },
            { input: [0xFC, 0x80, 0x80, 0x8F, 0xBF, 0xBF], name: 'overlong U+FFFF - 6 bytes' },
            { input: [0xF8, 0x84, 0x8F, 0xBF, 0xBF], name: 'overlong U+10FFFF - 5 bytes' },
            { input: [0xFC, 0x80, 0x84, 0x8F, 0xBF, 0xBF], name: 'overlong U+10FFFF - 6 bytes' },
        ];

        additionalBad.forEach((t) => {
            assert.throws(
                () => {
                    new TextDecoder('utf-8', { fatal: true })
                        .decode(new Uint8Array(t.input));
                },
                { name: 'TypeError' },
                `fatal: ${t.name}`
            );
        });
    });

    it('fatal streaming UTF-16LE - odd then even bytes', () => {
        const decoder = new TextDecoder('utf-16le', { fatal: true });
        const odd = new Uint8Array([0x00]);
        const even = new Uint8Array([0x00, 0x00]);

        // Odd byte streamed, then even flush — total 3 bytes (not aligned), should throw
        assert.throws(() => {
            decoder.decode(odd, { stream: true });
            decoder.decode(even);
        }, { name: 'TypeError' });
    });

    it('USVString surrogate handling with null character', () => {
        const enc = new TextEncoder();

        // fibjs WTF-8: lone surrogates pass through as WTF-8, not replaced at engine level
        // Just test null character encoding
        const r2 = enc.encode('\u0000');
        assert.deepStrictEqual([...r2], [0x00]);
    });

    it('non-fatal incomplete UTF-8 trailing lead byte', () => {
        const decoder = new TextDecoder();
        // "foo" + incomplete 3-byte lead (0xED)
        const chunk = new Uint8Array([0x66, 0x6f, 0x6f, 0xed]);
        const str = decoder.decode(chunk);
        assert.strictEqual(str, 'foo\ufffd');
    });

    it('fatal mode streaming with valid split sequence (BOM)', () => {
        const buf = Buffer.from([0xef, 0xbb, 0xbf, 0x74, 0x65,
            0x73, 0x74, 0xe2, 0x82, 0xac]);

        // In fatal mode, valid streamed data should NOT throw
        ['unicode-1-1-utf-8', 'utf8', 'utf-8'].forEach((alias) => {
            const dec = new TextDecoder(alias, { fatal: true });
            dec.decode(buf.slice(0, 8), { stream: true });
            // Should not throw — remaining bytes complete the € char
            dec.decode(buf.slice(8));
        });
    });

    it('fatal mode with truncated valid sequence should throw', () => {
        const buf = Buffer.from([0xef, 0xbb, 0xbf, 0x74, 0x65,
            0x73, 0x74, 0xe2, 0x82, 0xac]);

        ['unicode-1-1-utf-8', 'utf8', 'utf-8'].forEach((alias) => {
            const dec = new TextDecoder(alias, { fatal: true });
            // buf.slice(0,8) = BOM + "test" + E2 82 (incomplete €)
            assert.throws(
                () => dec.decode(buf.slice(0, 8)),
                { name: 'TypeError' },
                `fatal truncated with alias ${alias}`
            );
        });
    });

    it('legacy encoding: GBK decode', () => {
        const dec = new TextDecoder('gbk');
        assert.strictEqual(dec.encoding, 'gbk');
        // D6 D0 CE C4 = '中文' in GBK
        assert.strictEqual(
            dec.decode(new Uint8Array([0xD6, 0xD0, 0xCE, 0xC4])),
            '中文'
        );
    });

    it('legacy encoding: GBK streaming split multibyte', () => {
        const dec = new TextDecoder('gbk');
        let r = '';
        // Split '中' (D6 D0) across two chunks
        r += dec.decode(new Uint8Array([0xD6]), { stream: true });
        r += dec.decode(new Uint8Array([0xD0]));
        assert.strictEqual(r, '中');
    });

    it('legacy encoding: GBK fatal with invalid byte', () => {
        // Node.js (WHATWG strict): GBK 0xFF is invalid, throws TypeError
        // fibjs (ICU extended): GBK 0xFF maps to PUA U+F8F5
        const dec = new TextDecoder('gbk', { fatal: true });
        try {
            const result = dec.decode(new Uint8Array([0xFF]));
            // ICU path: mapped to PUA
            assert.strictEqual(result, '\uF8F5');
        } catch (e) {
            // WHATWG path: invalid byte
            assert.strictEqual(e.constructor.name, 'TypeError');
        }
    });

    it('legacy encoding: GBK non-fatal replaces invalid byte', () => {
        const dec = new TextDecoder('gbk');
        const result = dec.decode(new Uint8Array([0xFF]));
        // Node.js (WHATWG): U+FFFD, fibjs (ICU): U+F8F5
        assert.ok(
            result === '\uFFFD' || result === '\uF8F5',
            `expected U+FFFD or U+F8F5, got U+${result.charCodeAt(0).toString(16)}`
        );
    });

    it('legacy encoding: gb2312 maps to gbk', () => {
        assert.strictEqual(new TextDecoder('gb2312').encoding, 'gbk');
    });

    it('legacy encoding: GB18030 decode 2-byte', () => {
        const dec = new TextDecoder('gb18030');
        assert.strictEqual(dec.encoding, 'gb18030');
        assert.strictEqual(
            dec.decode(new Uint8Array([0xD6, 0xD0, 0xCE, 0xC4])),
            '中文'
        );
    });

    it('legacy encoding: GB18030 decode 4-byte (non-BMP)', () => {
        const dec = new TextDecoder('gb18030');
        // U+20000 (𠀀) in GB18030 = 95 32 82 36
        const result = dec.decode(new Uint8Array([0x95, 0x32, 0x82, 0x36]));
        assert.strictEqual(result.codePointAt(0), 0x20000);
    });

    it('legacy encoding: Shift_JIS decode', () => {
        const dec = new TextDecoder('shift_jis');
        assert.strictEqual(dec.encoding, 'shift_jis');
        // 93 FA 96 7B 8C EA = '日本語' in Shift_JIS
        assert.strictEqual(
            dec.decode(new Uint8Array([0x93, 0xFA, 0x96, 0x7B, 0x8C, 0xEA])),
            '日本語'
        );
    });

    it('legacy encoding: Shift_JIS streaming split multibyte', () => {
        const dec = new TextDecoder('shift_jis');
        let r = '';
        // Split '日' (93 FA) across two chunks
        r += dec.decode(new Uint8Array([0x93]), { stream: true });
        r += dec.decode(new Uint8Array([0xFA]));
        assert.strictEqual(r, '日');
    });

    it('legacy encoding: Big5 decode', () => {
        const dec = new TextDecoder('big5');
        assert.strictEqual(dec.encoding, 'big5');
        // A4 A4 A4 E5 = '中文' in Big5
        assert.strictEqual(
            dec.decode(new Uint8Array([0xA4, 0xA4, 0xA4, 0xE5])),
            '中文'
        );
    });

    it('legacy encoding: EUC-JP decode', () => {
        const dec = new TextDecoder('euc-jp');
        assert.strictEqual(dec.encoding, 'euc-jp');
        // C6 FC CB DC B8 EC = '日本語' in EUC-JP
        assert.strictEqual(
            dec.decode(new Uint8Array([0xC6, 0xFC, 0xCB, 0xDC, 0xB8, 0xEC])),
            '日本語'
        );
    });

    it('legacy encoding: EUC-KR decode', () => {
        const dec = new TextDecoder('euc-kr');
        assert.strictEqual(dec.encoding, 'euc-kr');
        // C7 D1 B1 B9 BE EE = '한국어' in EUC-KR
        assert.strictEqual(
            dec.decode(new Uint8Array([0xC7, 0xD1, 0xB1, 0xB9, 0xBE, 0xEE])),
            '한국어'
        );
    });

    it('legacy encoding: ISO-8859-1 maps to windows-1252', () => {
        assert.strictEqual(new TextDecoder('iso-8859-1').encoding, 'windows-1252');
        assert.strictEqual(new TextDecoder('latin1').encoding, 'windows-1252');
    });

    it('legacy encoding: windows-1252 decode', () => {
        const dec = new TextDecoder('windows-1252');
        // 48 E9 6C 6C F6 = 'Héllö'
        assert.strictEqual(
            dec.decode(new Uint8Array([0x48, 0xE9, 0x6C, 0x6C, 0xF6])),
            'Héllö'
        );
        // 0x80 = Euro sign (€) in windows-1252
        assert.strictEqual(
            dec.decode(new Uint8Array([0x80])),
            '€'
        );
    });

    it('legacy encoding: windows-1251 decode (Cyrillic)', () => {
        const dec = new TextDecoder('windows-1251');
        assert.strictEqual(dec.encoding, 'windows-1251');
        // CF F0 E8 E2 E5 F2 = 'Привет'
        assert.strictEqual(
            dec.decode(new Uint8Array([0xCF, 0xF0, 0xE8, 0xE2, 0xE5, 0xF2])),
            'Привет'
        );
    });

    it('legacy encoding: KOI8-R decode (Cyrillic)', () => {
        const dec = new TextDecoder('koi8-r');
        assert.strictEqual(dec.encoding, 'koi8-r');
        // F0 D2 C9 D7 C5 D4 = 'Привет' in KOI8-R
        assert.strictEqual(
            dec.decode(new Uint8Array([0xF0, 0xD2, 0xC9, 0xD7, 0xC5, 0xD4])),
            'Привет'
        );
    });

    it('legacy encoding: single-byte encodings are always complete (no streaming state)', () => {
        // Single-byte encodings decode each byte independently
        const encodings = ['windows-1252', 'windows-1251', 'koi8-r', 'iso-8859-2', 'iso-8859-15'];
        encodings.forEach((enc) => {
            const dec = new TextDecoder(enc);
            // Feed byte-by-byte should produce same result as all-at-once
            const bytes = [0x41, 0x42, 0x43, 0xE9, 0xF6]; // ASCII + high bytes
            let streamed = '';
            for (const b of bytes) {
                streamed += dec.decode(new Uint8Array([b]), { stream: true });
            }
            streamed += dec.decode();
            const oneShot = new TextDecoder(enc).decode(new Uint8Array(bytes));
            assert.strictEqual(streamed, oneShot, `${enc} streaming == one-shot`);
        });
    });
});

// test verifies that StringDecoder will correctly decode the given input
// buffer with the given encoding to the expected output. It will attempt all
// possible ways to write() the input buffer, see writeSequences(). The
// singleSequence allows for easy debugging of a specific sequence which is
// useful in case of test failures.
function test(encoding, input, expected, singleSequence) {
    let sequences;
    if (!singleSequence) {
        sequences = writeSequences(input.length);
    } else {
        sequences = [singleSequence];
    }
    const hexNumberRE = /.{2}/g;
    sequences.forEach((sequence) => {
        const decoder = new StringDecoder(encoding);
        let output = '';
        sequence.forEach((write) => {
            output += decoder.write(input.slice(write[0], write[1]));
        });
        output += decoder.end();
        assert.deepEqual(output, expected);
    });
}

// unicodeEscape prints the str contents as unicode escape codes.
function unicodeEscape(str) {
    let r = '';
    for (let i = 0; i < str.length; i++) {
        r += `\\u${str.charCodeAt(i).toString(16)}`;
    }
    return r;
}

// writeSequences returns an array of arrays that describes all possible ways a
// buffer of the given length could be split up and passed to sequential write
// calls.
//
// e.G. writeSequences(3) will return: [
//   [ [ 0, 3 ] ],
//   [ [ 0, 2 ], [ 2, 3 ] ],
//   [ [ 0, 1 ], [ 1, 3 ] ],
//   [ [ 0, 1 ], [ 1, 2 ], [ 2, 3 ] ]
// ]
function writeSequences(length, start, sequence) {
    if (start === undefined) {
        start = 0;
        sequence = [];
    } else if (start === length) {
        return [sequence];
    }
    let sequences = [];
    for (let end = length; end > start; end--) {
        const subSequence = sequence.concat([
            [start, end]
        ]);
        const subSequences = writeSequences(length, end, subSequence, sequences);
        sequences = sequences.concat(subSequences);
    }
    return sequences;
}
