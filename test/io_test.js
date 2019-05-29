const test = require("test");
test.setup();

const io = require("io");
const LF = `\n`
const CRLF = `\r\n`
let EOL = null

describe('io', () => {
    before(() => {
        const ms = new io.MemoryStream()
        const bs = new io.BufferedStream(ms)
        EOL = bs.EOL
    });

    describe('BufferedStream', () => {
        it('readLines', () => {
            const ms = new io.MemoryStream()
            const bs = new io.BufferedStream(ms)

            ms.write(`line1${bs.EOL}`)
            ms.write(`line2${bs.EOL}`)
            ms.rewind()

            assert.deepEqual(
                bs.readLines(),
                [
                    `line1`,
                    `line2`,
                ]
            )
        });

        describe('readUntil', () => {
            ;[
                ['system EOL'   , EOL   ],
                ['LF'           , LF    ],
                ['CRLF'         , CRLF  ],
                ['\\0'          , '\0'  ],
                ['\\r'          , '\r'  ],
            ].forEach(([
                desc,
                eol,
            ]) => {
                it(desc, () => {
                    const ms = new io.MemoryStream()
                    const bs = new io.BufferedStream(ms)

                    assert.deepEqual(bs.readUntil(eol), null)

                    ms.write(`line1${eol}`)
                    ms.write(`line2${eol}`)
                    ms.rewind()

                    assert.deepEqual(bs.readUntil(eol), `line1`)
                    assert.deepEqual(bs.readUntil(eol), `line2`)
                    assert.deepEqual(bs.readUntil(eol), null)
                })
            })
        })
    });
});

require.main === module && test.run(console.DEBUG);