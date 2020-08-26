const test = require("test");
test.setup();

const io = require("io");
const fs = require("fs");
const path = require("path");
const coroutine = require("coroutine");

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

    describe('io.Range', () => {
        var filePath = path.resolve(__dirname, './http_files/range_test/cat_rat.mp4');
        var file = fs.openFile(filePath);
        var fsize = Number(file.size());
        const ranges = [
            [1, 10],
            [5, 15],
            [10, 20],
        ];

        describe('invalid constructor params', () => {
            it('NOT ALLOWED: negative begin', () => {
                assert.throws(() => {
                    new io.Range(fs.openFile(filePath), -100, 0);
                })
            });

            it('NOT ALLOWED: infinite begin', () => {
                assert.throws(() => {
                    new io.Range(file, Infinity, file.size());
                })
            });

            it('NOT ALLOWED: infinite end', () => {
                assert.throws(() => {
                    new io.Range(file, 0, Infinity);
                })
            });

            it('NOT ALLOWED: over file size when use range string', () => {
                assert.throws(() => {
                    new io.Range(file, `0-${fsize}`);
                });
            });

            it('NOT ALLOWED: bad range format', () => {
                assert.throws(() => {
                    new io.Range(file, ``);
                });
            });
        });

        describe('valid constructor params', () => {
            it("accept range string", () => {
                var rng = new io.Range(fs.openFile(filePath), '2-10');

                assert.equal(rng.begin, 2);
                assert.equal(rng.end, 11);

                file.seek(rng.begin, fs.SEEK_SET);
                rng.rewind();

                assert.equal(0, file.read(rng.end - rng.begin).compare(rng.slice()));
            });

            it("accept numberic begin_pos, end_pos", () => {
                var rng = new io.Range(fs.openFile(filePath), 0, 10);

                assert.equal(rng.begin, 0);
                assert.equal(rng.end, 10);

                file.seek(rng.begin, fs.SEEK_SET);
                rng.rewind();

                assert.equal(0, file.read(rng.end - rng.begin).compare(rng.slice()));
            });
        });

        describe('robust case', () => {
            var file;

            before(() => {
                file = fs.openFile(filePath)
            });

            describe("ALLOW: multiple range upon the same file in order", () => {
                ranges.forEach(([begin, end]) => {
                    it(`[${begin}, ${end}]`, () => {
                        var rng = new io.Range(file, begin, end);

                        assert.equal(rng.begin, begin);
                        assert.equal(rng.end, end);

                        file.seek(rng.begin, fs.SEEK_SET);
                        rng.rewind();

                        assert.equal(0, file.read(rng.end - rng.begin).compare(rng.slice()));
                    });
                });
            });

            it.skip('BAD CASE: parallel read upon SAME FILE INVALID!!', () => {
                coroutine.parallel(ranges, ([begin, end]) => {
                    var rng = new io.Range(file, begin, end);

                    assert.equal(rng.begin, begin);
                    assert.equal(rng.end, end);

                    file.seek(rng.begin, fs.SEEK_SET);
                    assert.equal(0, file.read(rng.end - rng.begin).compare(rng.slice()));
                });
            });

            it("ALLOW: io.Range::begin > filesize", () => {
                var file = fs.openFile(filePath);
                var rng = new io.Range(file, 0, file.size());

                assert.equal(rng.end, file.size());
            });

            it("ALLOW: io.Range::end = filesize", () => {
                var file = fs.openFile(filePath);
                var rng = new io.Range(file, 0, file.size());

                rng.seek(0, fs.SEEK_END);

                assert.equal(null, rng.slice());
            });

            it("range[begin, end] never change", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var rng = new io.Range(file, begin, sz * 2);

                assert.equal(begin, begin);
                assert.equal(rng.end, sz * 2);

                rng.seek(-sz, fs.SEEK_END);
                assert.equal(begin, begin);
                assert.equal(rng.end, sz * 2);

                rng.seek(-sz * 2 + begin, fs.SEEK_END);
                assert.equal(begin, begin);
                assert.equal(rng.end, sz * 2);

                assert.equal(begin, begin);
                assert.equal(rng.end, sz * 2);
            });

            it("range intersect with filesize", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var rng = new io.Range(file, sz / 2, sz * 2);

                rng.seek(0, fs.SEEK_END);

                assert.equal(null, rng.slice());
            });

            it("allow stream.end > stream.size()", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var rng = new io.Range(file, begin, sz * 2);

                assert.equal(begin, rng.begin);
                assert.equal(sz * 2, rng.end);

                assert.ok(rng.end - rng.begin > Number(rng.size()));
                assert.equal(sz - begin, Number(rng.size()));
            });

            it("seek relative to begin as SEEK_SET", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var rng = new io.Range(file, begin, sz * 2);

                rng.seek(0, fs.SEEK_SET);

                assert.equal(sz - begin, Number(rng.size()));
                assert.equal(sz - begin, rng.slice().length);
            });

            it("seek relative to end as SEEK_END, but never out range", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var rng = new io.Range(file, begin, sz * 2);

                assert.equal(rng.end, sz * 2);
                rng.seek(-sz, fs.SEEK_END);

                assert.equal(rng.current(), file.tell() - BigInt(rng.begin));

                rng.seek(-sz * 2 + begin, fs.SEEK_END);
                assert.throws(() => {
                    rng.seek(-sz * 2 + begin - 1, fs.SEEK_END);
                })
            });
        });

        describe('clone to get new stream', () => {
            [].concat(ranges)
                .concat([
                    [10, undefined],
                    [Math.floor(Number(file.size()) / 2), undefined],
                ])
                .map(([begin, end = file.size()]) => {
                    it(`::clone -- [${begin}, ${end}]`, () => {
                        var file = fs.openFile(filePath);
                        var rng = new io.Range(file, begin, end);

                        var stm = rng.clone();

                        assert.equal(stm.size(), rng.size());

                        rng.rewind();
                        assert.equal(0,
                            rng.slice().compare(
                                rng.clone().readAll()
                            )
                        );
                        assert.equal(0,
                            rng.clone().readAll().compare(
                                rng.clone().readAll()
                            )
                        );
                    });
                });
        });
    });
});

require.main === module && test.run(console.DEBUG);