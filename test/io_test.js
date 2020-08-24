const test = require("test");
test.setup();

const io = require("io");
const fs = require("fs");
const path = require("path");

const LF = `\n`
const CRLF = `\r\n`
let EOL = null

describe('io', () => {
    before(() => {
        const ms = new io.MemoryStream()
        const bs = new io.BufferedStream(ms)
        EOL = bs.EOL
    });

    describe('io.BufferedStream', () => {
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
                ['system EOL', EOL],
                ['LF', LF],
                ['CRLF', CRLF],
                ['\\0', '\0'],
                ['\\r', '\r'],
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

    describe('io.RangeStream', () => {
        var filePath = path.resolve(__dirname, './http_files/range_test/cat_rat.mp4');
        var file = fs.openFile(filePath);
        var fsize = Number(file.size());

        describe('invalid constructor params', () => {
            it('over file size', () => {
                assert.throws(() => {
                    new io.RangeStream(file, `0-${Number(file.size())}`);
                });
            });

            it('bad range format', () => {
                assert.throws(() => {
                    new io.RangeStream(file, ``);
                });
            });
        });

        describe('valid constructor params', () => {
            it("accept range string", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(fs.openFile(filePath), '2-10');

                assert.equal(stm.begin, 2);
                assert.equal(stm.end, 11);

                file.seek(stm.begin, fs.SEEK_SET);
                stm.rewind();
                assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
            });

            it("accept numberic begin_pos, end_pos", () => {
                var stm = new io.RangeStream(fs.openFile(filePath), 0, 10);

                assert.equal(stm.begin, 0);
                assert.equal(stm.end, 10);

                file.seek(stm.begin, fs.SEEK_SET);
                stm.rewind();
                assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);