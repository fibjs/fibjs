const test = require("test");
test.setup();

const io = require("io");
const fs = require("fs");
const path = require("path");
const coroutine = require("coroutine");

const { generateFakeMp4 } = require('./http_files/range_test/fake');
const { assert_error_msg } = require('./_helpers/error');

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
        var filePath;
        var file;
        var fsize;

        before(() => {
            filePath = generateFakeMp4().target;
            file = fs.openFile(filePath);
            fsize = Number(file.size());
        });

        describe('invalid constructor params', () => {
            it('NOT ALLOWED: negative begin', () => {
                assert_error_msg(() => {
                    new io.RangeStream(fs.openFile(filePath), -100, 0);
                }, `'begin' must be non-negative integer!`)
            });

            it('NOT ALLOWED: infinite begin', () => {
                assert.throws(() => {
                    new io.RangeStream(file, Infinity, file.size());
                })
            });

            it('NOT ALLOWED: infinite end', () => {
                assert.throws(() => {
                    new io.RangeStream(file, 0, Infinity);
                })
            });

            it('NOT ALLOWED: over file size when use range string', () => {
                assert.throws(() => {
                    new io.RangeStream(file, `0-${fsize}`);
                });

                assert_error_msg(() => {
                    new io.RangeStream(file, `0-${fsize}`);
                }, `Parse Range-String Error, invalid range string '0-${fsize}' which is out of file size range!`)
            });

            it('NOT ALLOWED: bad range format', () => {
                assert_error_msg(() => {
                    new io.RangeStream(file, ``);
                }, `Parse Range-String Error, expect next token '-'!`)
            });
        });

        describe('valid constructor params', () => {
            it("accept range string", () => {
                var stm = new io.RangeStream(fs.openFile(filePath), '2-10');

                assert.equal(stm.begin, 2);
                assert.equal(stm.end, 11);

                file.seek(stm.begin, fs.SEEK_SET);
                stm.rewind();
                assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
            });

            it("http 206 range string(but weired for RangeStream)", () => {
                var stm = new io.RangeStream(fs.openFile(filePath), 'bytes=2-10');

                assert.equal(stm.begin, 0);
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

        describe('robust case', () => {
            var file;
            const ranges = [
                [1, 10],
                [5, 15],
                [10, 20],
            ];

            before(() => {
                file = fs.openFile(filePath)
            });

            it("actions after close RangeStream", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(file, 0, file.size());

                // seekable
                stm.seek(0, fs.SEEK_SET);
                // readable
                stm.read(1);
                // query current position
                assert.equal(stm.tell(), 0);
                // get size of range
                assert.equal(stm.size(), file.size());
                // get the real stream's fd
                assert.isDefined(stm.fd);

                // but after close it
                stm.close();

                // you can call `.seek()` still.
                stm.seek(0, fs.SEEK_SET);

                // but you cannot read from it.
                assert_error_msg(() => {
                    stm.read(1);
                }, `[-100026] Object closed.`)

                // nor query current positoin
                assert_error_msg(() => {
                    stm.tell();
                }, `[-100026] Object closed.`)

                // not allowed to get size of it.
                assert_error_msg(() => {
                    stm.size();
                }, `[-100026] Object closed.`)

                // not allowed to get fd of it.
                assert_error_msg(() => {
                    stm.fd;
                }, `[-100026] Object closed.`)
            });

            describe("ALLOW: multiple range upon the same file in order", () => {
                ranges.forEach(([begin, end]) => {
                    it(`[${begin}, ${end}]`, () => {
                        var stm = new io.RangeStream(file, begin, end);

                        assert.equal(stm.begin, begin);
                        assert.equal(stm.end, end);

                        file.seek(stm.begin, fs.SEEK_SET);
                        stm.rewind();
                        assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
                    });
                });
            });

            it.skip('BAD CASE: parallel read upon SAME FILE INVALID!!', () => {
                coroutine.parallel(ranges, ([begin, end]) => {
                    var stm = new io.RangeStream(file, begin, end);

                    assert.equal(stm.begin, begin);
                    assert.equal(stm.end, end);

                    file.seek(stm.begin, fs.SEEK_SET);
                    stm.rewind();
                    assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
                });
            });

            it("ALLOW: io.RangeStream::begin > filesize", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(file, 0, file.size());

                assert.equal(stm.end, file.size());
            });

            it("ALLOW: io.RangeStream::end = filesize", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(file, 0, file.size());

                stm.seek(0, fs.SEEK_END);

                assert.equal(null, stm.readAll());
            });

            it("range[begin, end] never change", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                assert.equal(begin, begin);
                assert.equal(stm.end, sz * 2);

                stm.seek(-sz, fs.SEEK_END);
                assert.equal(begin, begin);
                assert.equal(stm.end, sz * 2);

                stm.seek(-sz * 2 + begin, fs.SEEK_END);
                assert.equal(begin, begin);
                assert.equal(stm.end, sz * 2);

                stm.rewind();
                assert.equal(begin, begin);
                assert.equal(stm.end, sz * 2);
            });

            it("range intersect with filesize", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var stm = new io.RangeStream(file, sz / 2, sz * 2);

                stm.seek(0, fs.SEEK_END);

                assert.equal(null, stm.readAll());
            });

            it("allow stream.end > stream.size()", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                assert.equal(begin, stm.begin);
                assert.equal(sz * 2, stm.end);

                assert.ok(stm.end - stm.begin > Number(stm.size()));
                assert.equal(sz - begin, Number(stm.size()));
            });

            it("seek relative to begin as SEEK_SET", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                stm.seek(0, fs.SEEK_SET);

                assert.equal(sz - begin, Number(stm.size()));
                assert.equal(sz - begin, stm.readAll().length);
            });

            it("seek relative to end as SEEK_END, but never out range", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                assert.equal(stm.end, sz * 2);
                stm.seek(-sz, fs.SEEK_END);

                assert.equal(stm.tell(), file.tell() - BigInt(stm.begin));

                stm.seek(-sz * 2 + begin, fs.SEEK_END);
                assert.throws(() => {
                    stm.seek(-sz * 2 + begin - 1, fs.SEEK_END);
                })
            });

            it("::stat", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                assert.ok(stm.stat().size < sz);

                var stm = new io.RangeStream(file, 0, sz * 2);
                assert.ok(stm.stat().size === sz);

                assert.isDefined(stm.stat().name);
                assert.isDefined(stm.stat().size);
                assert.isDefined(stm.stat().ctimeMs);
                assert.isDefined(stm.stat().atimeMs);
                assert.isDefined(stm.stat().mtimeMs);

                assert.strictEqual(stm.stat().size, file.stat().size);
                assert.strictEqual(stm.stat().ctimeMs, file.stat().ctimeMs);
                assert.strictEqual(stm.stat().atimeMs, file.stat().atimeMs);
                assert.strictEqual(stm.stat().mtimeMs, file.stat().mtimeMs);
                assert.strictEqual(stm.stat().isFile(), file.stat().isFile());
                assert.strictEqual(stm.stat().isFile(), true);
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);