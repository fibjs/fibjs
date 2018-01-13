var t = require("test");
t.setup();

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
        test('utf16le', Buffer.from('3DD84DDC', 'hex'), '\ud83d\udc4d'); // thumbs up

        decoder = new StringDecoder('utf16le');
        assert.strictEqual(decoder.write(Buffer.from('3DD8', 'hex')), '');
        assert.strictEqual(decoder.write(Buffer.from('4D', 'hex')), '');
        assert.strictEqual(decoder.write(Buffer.from('DC', 'hex')), '\ud83d\udc4d');
        assert.strictEqual(decoder.end(), '');

        decoder = new StringDecoder('utf16le');
        assert.strictEqual(decoder.write(Buffer.from('3DD8', 'hex')), '');
        assert.strictEqual(decoder.end(), '\ud83d');

        decoder = new StringDecoder('utf16le');
        assert.strictEqual(decoder.write(Buffer.from('3DD8', 'hex')), '');
        assert.strictEqual(decoder.write(Buffer.from('4D', 'hex')), '');
        assert.strictEqual(decoder.end(), '\ud83d');
    });

    it('should throws', () => {
        assert.throws(() => {
            new StringDecoder(1);
        }, /^Error: Unknown encoding: 1$/);

        assert.throws(() => {
            new StringDecoder('test');
        }, /^Error: Unknown encoding: test$/);
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

require.main === module && t.run(console.DEBUG);